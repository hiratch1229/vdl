#include "MemoryAllocator.hpp"

#include <vdl/Math.hpp>

#include <assert.h>

void MemoryAllocator::Initialize(void* _pBuffer, vdl::uint _Size)
{
  assert(_pBuffer && 0 < _Size && _Size <= kMaxSize);

  pBuffer_ = static_cast<vdl::uint8_t*>(_pBuffer);

  const vdl::uint FLI = GetFirstLevelIndex(_Size);
  const vdl::uint SLI = GetSecondLevelIndex(_Size, FLI);

  //  �n�[�̃_�~�[���쐬
  BlockData* pStart = new(pBuffer_)BlockData(nullptr, 0);
  pStart->Lock();

  //  �m�ۂ������������쐬
  BlockData* pBlock = new(pBuffer_ + kBlockSize)BlockData(pStart, _Size - kBlockSize * 3/* �����p + �n�[�_�~�[�p + �I�[�_�~�[�p*/);
  RegistFreeList(pBlock, FLI, SLI);

  //  �I�[�̃_�~�[���쐬
  BlockData* pEnd = new(pBuffer_ + _Size - kBlockSize)BlockData(pBlock, 0);
  pEnd->Lock();
}

void* MemoryAllocator::Allocate(vdl::uint _Size, vdl::uint _Alignment)
{
  assert(0 < _Size && _Size <= kMaxSize);

  //  �T�C�Y������������ꍇ�Œ�l�ɕύX
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

    //  �~�����T�C�Y�̃t���[���X�g�����݂��Ȃ��ꍇ�Ď擾
    if (FreeList_[DataIndex].isEndFreeListData())
    {
      //  ������1�J�e�S���[���ŏ�ʂ̃t���[���X�g�̎擾
      SLI = GetFreeListSLI(FLI, SLI);

      //  ���݂��Ė��������ꍇ��1�J�e�S���[����Ď擾
      if (SLI >= kSecondVisionNum)
      {
        FLI = GetFreeListFLI(FLI);
        //  ���蓖�Ă�郁���������݂��Ȃ���nullptr��Ԃ�
        if (FLI >= kMaxFirstVisionNum)
        {
          return nullptr;
        }
        SLI = GetFreeListSLI(FLI, 0);
        assert(SLI < kSecondVisionNum);
      }

      //  ���߂��f�[�^���g���ăC���f�b�N�X�̍Ď擾
      DataIndex = GetFreeListIndex(FLI, SLI);
      assert(DataIndex < kMaxFirstVisionNum * kSecondVisionNum);
    }
    assert(!FreeList_[DataIndex].isEndFreeListData());

    //  �t���[���X�g������o��
    pBlock = FreeList_[DataIndex].GetNextFreeListData();
    UnregistFreeList(pBlock, FLI, SLI);

    assert(pBlock->GetBufferSize() >= SearchBlockSize);
  }

  ////  �v�����ꂽ�T�C�Y���傫���ꍇ����
  //if (pBlock->GetBufferSize() > _Size + kMinBlockVisionSize)
  //{
  //  //  �����������̌�납��m��
  //  BlockData* pNextBlock = new(reinterpret_cast<uint8_t*>(pBlock) + pBlock->GetBufferSize() - _Size)BlockData(pBlock, _Size);
  //  pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pNextBlock);
  //
  //  pBlock->Resize(pBlock->GetBufferSize() - (kBlockSize + _Size));
  //  RegistFreeList(pBlock);
  //
  //  pBlock = pNextBlock;
  //}

  //  �������̊m��
  {
    const uintptr_t BufferPos = DownAlignment(reinterpret_cast<uintptr_t>(reinterpret_cast<uint8_t*>(pBlock) + kBlockSize + pBlock->GetBufferSize() - _Size), _Alignment);
    const vdl::uint BufferSize = static_cast<vdl::uint>(reinterpret_cast<uintptr_t>(pBlock->GetNextPhysicalData()) - BufferPos);

    BlockData* pVisionBlock = new(reinterpret_cast<void*>(BufferPos - kBlockSize))BlockData(pBlock, BufferSize);
    pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pVisionBlock);
    assert(reinterpret_cast<uintptr_t>(pVisionBlock->GetBuffer()) % _Alignment == 0);

    //  ���̕����������f�[�^���ēo�^
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

  //  1�O�̕����������̃f�[�^�ƃ}�[�W
  if (BlockData* pPreviousBlock = pBlock->GetPreviousPhysicalData();
    !pPreviousBlock->isUsed())
  {
    //  �t���[���X�g����폜
    UnregistFreeList(pPreviousBlock);

    //  �T�C�Y������
    pPreviousBlock->Resize(pPreviousBlock->GetBufferSize() + (kBlockSize + pBlock->GetBufferSize()));

    pBlock = pPreviousBlock;

    pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pBlock);
  }

  //  1���̕����������̃f�[�^�ƃ}�[�W
  if (BlockData* pNextBlock = pBlock->GetNextPhysicalData();
    !pNextBlock->isUsed())
  {
    //  �t���[���X�g����폜
    UnregistFreeList(pNextBlock);

    //  �T�C�Y������
    pBlock->Resize(pBlock->GetBufferSize() + (kBlockSize + pNextBlock->GetBufferSize()));

    pBlock->GetNextPhysicalData()->SetPreviousPhysicalData(pBlock);
  }

  //  �����������̃f�[�^���ēo�^
  RegistFreeList(pBlock);
}
