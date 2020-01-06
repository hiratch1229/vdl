#pragma once
#include <vdl/Fwd.hpp>

//  TLSFアロケータ
//  TODO:メモリ領域のリサイズ
class MemoryAllocator
{
  //  BoundaryTag
  class BlockData
  {
    //  使用確認フラグのマスク
    static constexpr vdl::uint32_t kUsedFlagMask = 1 << 31;
    //  バッファサイズのマスク
    static constexpr vdl::uint32_t kBufferSizeMask = ~kUsedFlagMask;
  private:
    //bool isUsed : 1;
    //vdl::uint32_t BufferSize : 31;
    vdl::uint32_t Data_;

    //  1つ前の物理データ
    BlockData* pPreviousPhysicalData_;

    //  1つ次のフリーリストデータ
    BlockData* pNextFreeListData_;
    //  1つ前のフリーリストデータ
    BlockData* pPreviousFreeListData_;
  public:
    BlockData()
      : pPreviousPhysicalData_(nullptr), Data_(0)
      , pNextFreeListData_(nullptr), pPreviousFreeListData_(nullptr) {}

    BlockData(BlockData* _pPreviousPhysicalData, vdl::uint _BufferSize)
      : pPreviousPhysicalData_(_pPreviousPhysicalData), Data_(_BufferSize & kBufferSizeMask)
      , pNextFreeListData_(nullptr), pPreviousFreeListData_(nullptr) { }
  public:
    //  使用状態に変更
    void Lock() { Data_ |= kUsedFlagMask; }

    //  未使用状態に変更
    void Free() { Data_ &= ~kUsedFlagMask; }

    //  バッファサイズの変更
    void Resize(vdl::uint _BufferSize) { Data_ = _BufferSize & kBufferSizeMask; }

    void SetPreviousPhysicalData(BlockData* _pPreviousPhysicalData) { pPreviousPhysicalData_ = _pPreviousPhysicalData; }

    //  使用確認
    [[nodiscard]] bool isUsed()const { return Data_ & kUsedFlagMask; }

    //  扱えるデータのサイズを取得
    [[nodiscard]] vdl::uint GetBufferSize()const { return Data_ & kBufferSizeMask; }

    //  管理メモリの先頭ポインタを取得
    [[nodiscard]] void* GetBuffer() { return reinterpret_cast<vdl::uint8_t*>(this) + kBlockSize; }

    //  1つ次の物理メモリのデータを取得
    [[nodiscard]] BlockData* GetNextPhysicalData() { return reinterpret_cast<BlockData*>(reinterpret_cast<vdl::uint8_t*>(this) + kBlockSize + GetBufferSize()); }

    //  1つ前の物理メモリのデータを取得
    [[nodiscard]] BlockData* GetPreviousPhysicalData() { return pPreviousPhysicalData_; }

    //  1つ次の物理メモリのデータを取得
    [[nodiscard]] BlockData* GetNextFreeListData() { return pNextFreeListData_; }

    //  1つ前の物理メモリのデータを取得
    [[nodiscard]] BlockData* GetPreviousFreeListData() { return pPreviousFreeListData_; }

    //  フリーリストの終端確認
    [[nodiscard]] bool isEndFreeListData()const { return pNextFreeListData_ == nullptr; }
  public:
    //  フリーリストに登録
    void RegistFreeList(BlockData* _pNextData)
    {
      if (pNextFreeListData_)
      {
        pNextFreeListData_->pPreviousFreeListData_ = _pNextData;
      }

      _pNextData->pNextFreeListData_ = pNextFreeListData_;
      _pNextData->pPreviousFreeListData_ = this;

      pNextFreeListData_ = _pNextData;
    }

    //  フリーリストから解除
    void UnregistFreeList()
    {
      if (pPreviousFreeListData_)
      {
        pPreviousFreeListData_->pNextFreeListData_ = pNextFreeListData_;
      }
      if (pNextFreeListData_)
      {
        pNextFreeListData_->pPreviousFreeListData_ = pPreviousFreeListData_;
      }

      pNextFreeListData_ = pPreviousFreeListData_ = nullptr;
    }
  };
private:
  //  ブロックのデータサイズ
  static constexpr vdl::uint kBlockSize = sizeof(BlockData);
  //  第1カテゴリーの分割数の最大値
  static constexpr vdl::uint kMaxFirstVisionNum = 30;
  //  第2カテゴリーの分割数の指数
  static constexpr vdl::uint kSecondVisionExponent = 4;
  //  第2カテゴリーの分割数
  static constexpr vdl::uint kSecondVisionNum = 1 << kSecondVisionExponent;
  //  メモリの最大値
  static constexpr vdl::uint kMaxSize = ~(1 << 31);
  //  メモリの最小値
  static constexpr vdl::uint kMinSize = kSecondVisionNum;
  //  ブロックの分割の最小サイズ
  static constexpr vdl::uint kMinBlockVisionSize = kBlockSize + kMinSize;
private:
  //  最下位ビットを取得
  static constexpr vdl::uint GetLSB(vdl::uint _Value)
  {
    vdl::uint Bit = 0;

    _Value &= ~_Value + 1;
    if (_Value & 0xffff0000) Bit += 16;
    if (_Value & 0xff00ff00) Bit += 8;
    if (_Value & 0xf0f0f0f0) Bit += 4;
    if (_Value & 0xcccccccc) Bit += 2;
    if (_Value & 0xaaaaaaaa) ++Bit;
    ++Bit;

    return Bit - 1;
  }
  //  アライメントを揃える
  static constexpr vdl::uint64_t DownAlignment(vdl::uint64_t _Value, vdl::uint64_t _Alignment)
  {
    return _Value & ~(_Alignment - 1);
  }
  //  第1カテゴリーのインデックスを取得
  static constexpr vdl::uint GetFirstLevelIndex(vdl::uint _Size)
  {
    vdl::uint Bit = 32;

    if (!(_Size & 0xffff0000)) { _Size <<= 16; Bit -= 16; }
    if (!(_Size & 0xff000000)) { _Size <<= 8; Bit -= 8; }
    if (!(_Size & 0xf0000000)) { _Size <<= 4; Bit -= 4; }
    if (!(_Size & 0xc0000000)) { _Size <<= 2; Bit -= 2; }
    if (!(_Size & 0x80000000)) { _Size <<= 1; --Bit; }

    return Bit - 1;
  }
  //  第2カテゴリーのインデックスを取得
  static constexpr vdl::uint GetSecondLevelIndex(vdl::uint _Size, vdl::uint _FLI)
  {
    //  最上位ビット未満のみ有効にするビット
    const vdl::uint32_t Mask = ~static_cast<vdl::uint32_t>(0xff << _FLI);

    //  最上位ビットの1つ下のビットから分割数の指数桁を取得
    return (_Size & Mask) >> (_FLI - kSecondVisionExponent);
  }
  //  フリーリストのインデックスを取得
  static constexpr vdl::uint GetFreeListIndex(vdl::uint _FLI, vdl::uint _SLI)
  {
    return _FLI * kSecondVisionNum + _SLI;
  }
private:
  vdl::uint8_t* pBuffer_ = nullptr;
  vdl::uint32_t FirstFreeListBit_ = 0;
  vdl::uint16_t SecondFreeListBit_[kMaxFirstVisionNum] = {};
  BlockData FreeList_[kMaxFirstVisionNum * kSecondVisionNum] = {};
private:
  vdl::uint GetFreeListFLI(vdl::uint _FLI)
  {
    //  自分以上のマスクを作成
    const vdl::uint32_t Mask = 0xffffffff << _FLI;

    //  上位の有効なフリーリストを取得
    const vdl::uint32_t EnableFreeListBit = FirstFreeListBit_ & Mask;

    //  存在するなら一番近いフリーリストを取得
    return (EnableFreeListBit == 0 ? kMaxFirstVisionNum : GetLSB(EnableFreeListBit));
  }
  vdl::uint GetFreeListSLI(vdl::uint _FLI, vdl::uint _SLI)
  {
    //  自分以上のマスクを作成
    const vdl::uint32_t Mask = 0xffffffff << _SLI;

    //  上位の有効なフリーリストを取得
    const vdl::uint32_t EnableFreeListBit = SecondFreeListBit_[_FLI] & Mask;

    //  存在するなら一番近いフリーリストを取得
    return (EnableFreeListBit == 0 ? kSecondVisionNum : GetLSB(EnableFreeListBit));
  }
private:
  //  フリーリストに登録
  void RegistFreeList(BlockData* _pBlock, vdl::uint _FLI, vdl::uint _SLI)
  {
    FreeList_[GetFreeListIndex(_FLI, _SLI)].RegistFreeList(_pBlock);

    SecondFreeListBit_[_FLI] |= 1 << _SLI;
    FirstFreeListBit_ |= 1 << _FLI;
  }
  void RegistFreeList(BlockData* _pBlock)
  {
    const vdl::uint FLI = GetFirstLevelIndex(_pBlock->GetBufferSize());
    const vdl::uint SLI = GetSecondLevelIndex(_pBlock->GetBufferSize(), FLI);

    RegistFreeList(_pBlock, FLI, SLI);
  }

  //  フリーリストから解除
  void UnregistFreeList(BlockData* _pBlock, vdl::uint _FLI, vdl::uint _SLI)
  {
    _pBlock->UnregistFreeList();

    //  フリーリストが空になったらマスクを変更
    if (FreeList_[GetFreeListIndex(_FLI, _SLI)].isEndFreeListData())
    {
      SecondFreeListBit_[_FLI] &= ~(1 << _SLI);
      if (!SecondFreeListBit_[_FLI])
      {
        FirstFreeListBit_ &= ~(1 << _FLI);
      }
    }
  }
  void UnregistFreeList(BlockData* _pBlock)
  {
    const vdl::uint FLI = GetFirstLevelIndex(_pBlock->GetBufferSize());
    const vdl::uint SLI = GetSecondLevelIndex(_pBlock->GetBufferSize(), FLI);

    UnregistFreeList(_pBlock, FLI, SLI);
  }
public:
  MemoryAllocator() = default;

  //  初期化
  //  割り当て用のメモリは事前に確保しておく
  void Initialize(void* _pBuffer, vdl::uint _Size);

  //  メモリ割り当て
  //  割り当てる領域が無い場合nullptrが返ります
  [[nodiscard]] void* Allocate(vdl::uint _Size, vdl::uint _Alignment = 4);

  //  メモリ解放
  void Release(void* _pBuffer);
};
