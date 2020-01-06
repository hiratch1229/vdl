#include "MemoryAllocator.hpp"

#include <vdl/Math.hpp>

#include <assert.h>

void MemoryAllocator::Initialize(void* _pBuffer, vdl::uint _Size)
{
  assert(_pBuffer && 0 < _Size && _Size <= kMaxSize);

  pBuffer_ = static_cast<vdl::uint8_t*>(_pBuffer);

  const vdl::uint FLI = GetFirstLevelIndex(_Size);
  const vdl::uint SLI = GetSecondLevelIndex(_Size, FLI);

  //  始端のダミーを作成
  BlockData* pStart = new(pBuffer_)BlockData(nullptr, 0);
  pStart->Lock();

  //  確保したメモリを作成
  BlockData* pBlock = new(pBuffer_ + kBlockSize)BlockData(pStart, _Size - kBlockSize * 3/* 自分用 + 始端ダミー用 + 終端ダミー用*/);
  RegistFreeList(pBlock, FLI, SLI);

  //  終端のダミーを作成
  BlockData* pEnd = new(pBuffer_ + _Size - kBlockSize)BlockData(pBlock, 0);
  pEnd->Lock();
}

void* MemoryAllocator::Allocate(vdl::uint _Size, vdl::uint _Alignment)
{
  assert(0 < _Size && _Size <= kMaxSize);

  //  サイズが小さすぎる場合最低値に変更
  _Size = vdl::Math::Max(_Size, kMinSize);

  BlockData* pBlock;
  {
    const vdl::uint SearchBlockSize = _Size + kBlockSize + _Alignment;

    vdl::uint FLI = GetFirstLevelIndex(SearchBlockSize);
    assert(FLI < kMaxFirstVisionNum);
    vdl::uint SLI = GetSecondLevelIndex(SearchBlockSize, FLI);
    assert(SLI < kSecondVisionNum);

    vdl::uint DataIndex = GetFreeListIndex(FLI, SLI);
    assert(DataIndex < kMaxFirstVisionNum * kSecondVisionNum);

    //  欲しいサイズのフリーリストが存在しない場合再取得
    if (FreeList_[DataIndex].isEndFreeListData())
    {
      //  同じ第1カテゴリー内で上位のフリーリストの取得
      SLI = GetFreeListSLI(FLI, SLI);

      //  存在して無かった場合第1カテゴリーから再取得
      if (SLI >= kSecondVisionNum)
      {
        FLI = GetFreeListFLI(FLI + 1);
        //  割り当てれるメモリが存在しない時nullptrを返す
        if (FLI >= kMaxFirstVisionNum)
        {
          return nullptr;
        }
        SLI = GetFreeListSLI(FLI, 0);
        assert(SLI < kSecondVisionNum);
      }

      //  求めたデータを使ってインデックスの再取得
      DataIndex = GetFreeListIndex(FLI, SLI);
      assert(DataIndex < kMaxFirstVisionNum * kSecondVisionNum);
    }
    assert(!FreeList_[DataIndex].isEndFreeListData());

    //  フリーリストから取り出す
    pBlock = FreeList_[DataIndex].GetNextFreeListData();
    UnregistFreeList(pBlock, FLI, SLI);

    assert(pBlock->GetBufferSize() >= SearchBlockSize);
  }

  ////  要求されたサイズより大きい場合分割
  //if (pBlock->GetBufferSize() > _Size + kMinBlockVisionSize)
  //{
  //  //  物理メモリの後ろから確保
  //  BlockData* pNextBlock = new(reinterpret_cast<uint8_t*>(pBlock) + pBlock->GetBufferSize() - _Size)BlockData(pBlock, _Size);
  //  pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pNextBlock);
  //
  //  pBlock->Resize(pBlock->GetBufferSize() - (kBlockSize + _Size));
  //  RegistFreeList(pBlock);
  //
  //  pBlock = pNextBlock;
  //}

  //  メモリの確保
  {
    const uintptr_t BufferPos = DownAlignment(reinterpret_cast<uintptr_t>(reinterpret_cast<uint8_t*>(pBlock) + kBlockSize + pBlock->GetBufferSize() - _Size), _Alignment);
    const vdl::uint BufferSize = static_cast<vdl::uint>(reinterpret_cast<uintptr_t>(pBlock->GetNextPhysicalData()) - BufferPos);

    BlockData* pVisionBlock = new(reinterpret_cast<void*>(BufferPos - kBlockSize))BlockData(pBlock, BufferSize);
    pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pVisionBlock);
    assert(reinterpret_cast<uintptr_t>(pVisionBlock->GetBuffer()) % _Alignment == 0);

    //  元の物理メモリデータを再登録
    pBlock->Resize(pBlock->GetBufferSize() - (kBlockSize + BufferSize));
    RegistFreeList(pBlock);

    pBlock = pVisionBlock;
  }

  pBlock->Lock();
  return pBlock->GetBuffer();
}

void MemoryAllocator::Release(void* _pBuffer)
{
  BlockData* pBlock = reinterpret_cast<BlockData*>(static_cast<vdl::uint8_t*>(_pBuffer) - kBlockSize);
  pBlock->Free();

  //  1つ前の物理メモリのデータとマージ
  if (BlockData* pPreviousBlock = pBlock->GetPreviousPhysicalData();
    !pPreviousBlock->isUsed())
  {
    //  フリーリストから削除
    UnregistFreeList(pPreviousBlock);

    //  サイズを結合
    pPreviousBlock->Resize(pPreviousBlock->GetBufferSize() + (kBlockSize + pBlock->GetBufferSize()));

    pBlock = pPreviousBlock;

    pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pBlock);
  }

  //  1つ次の物理メモリのデータとマージ
  if (BlockData* pNextBlock = pBlock->GetNextPhysicalData();
    !pNextBlock->isUsed())
  {
    //  フリーリストから削除
    UnregistFreeList(pNextBlock);

    //  サイズを結合
    pBlock->Resize(pBlock->GetBufferSize() + (kBlockSize + pNextBlock->GetBufferSize()));

    pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pBlock);
  }

  //  物理メモリのデータを再登録
  RegistFreeList(pBlock);
}
