#pragma once
#include <vdl/Fwd.hpp>

//  TLSF�A���P�[�^
//  TODO:�������̈�̃��T�C�Y
class MemoryAllocator
{
  //  BoundaryTag
  class BlockData
  {
    //  �g�p�m�F�t���O�̃}�X�N
    static constexpr vdl::uint32_t kUsedFlagMask = 1 << 31;
    //  �o�b�t�@�T�C�Y�̃}�X�N
    static constexpr vdl::uint32_t kBufferSizeMask = ~kUsedFlagMask;
  private:
    //bool isUsed : 1;
    //vdl::uint32_t BufferSize : 31;
    vdl::uint32_t Data_;

    //  1�O�̕����f�[�^
    BlockData* pPreviousPhysicalData_;

    //  1���̃t���[���X�g�f�[�^
    BlockData* pNextFreeListData_;
    //  1�O�̃t���[���X�g�f�[�^
    BlockData* pPreviousFreeListData_;
  public:
    BlockData()
      : pPreviousPhysicalData_(nullptr), Data_(0)
      , pNextFreeListData_(nullptr), pPreviousFreeListData_(nullptr) {}

    BlockData(BlockData* _pPreviousPhysicalData, vdl::uint _BufferSize)
      : pPreviousPhysicalData_(_pPreviousPhysicalData), Data_(_BufferSize & kBufferSizeMask)
      , pNextFreeListData_(nullptr), pPreviousFreeListData_(nullptr) { }
  public:
    //  �g�p��ԂɕύX
    void Lock() { Data_ |= kUsedFlagMask; }

    //  ���g�p��ԂɕύX
    void Free() { Data_ &= ~kUsedFlagMask; }

    //  �o�b�t�@�T�C�Y�̕ύX
    void Resize(vdl::uint _BufferSize) { Data_ = _BufferSize & kBufferSizeMask; }

    void SetPreviousPhysicalData(BlockData* _pPreviousPhysicalData) { pPreviousPhysicalData_ = _pPreviousPhysicalData; }

    //  �g�p�m�F
    [[nodiscard]] bool isUsed()const { return Data_ & kUsedFlagMask; }

    //  ������f�[�^�̃T�C�Y���擾
    [[nodiscard]] vdl::uint GetBufferSize()const { return Data_ & kBufferSizeMask; }

    //  �Ǘ��������̐擪�|�C���^���擾
    [[nodiscard]] void* GetBuffer() { return reinterpret_cast<vdl::uint8_t*>(this) + kBlockSize; }

    //  1���̕����������̃f�[�^���擾
    [[nodiscard]] BlockData* GetNextPhysicalData() { return reinterpret_cast<BlockData*>(reinterpret_cast<vdl::uint8_t*>(this) + kBlockSize + GetBufferSize()); }

    //  1�O�̕����������̃f�[�^���擾
    [[nodiscard]] BlockData* GetPreviousPhysicalData() { return pPreviousPhysicalData_; }

    //  1���̕����������̃f�[�^���擾
    [[nodiscard]] BlockData* GetNextFreeListData() { return pNextFreeListData_; }

    //  1�O�̕����������̃f�[�^���擾
    [[nodiscard]] BlockData* GetPreviousFreeListData() { return pPreviousFreeListData_; }

    //  �t���[���X�g�̏I�[�m�F
    [[nodiscard]] bool isEndFreeListData()const { return pNextFreeListData_ == nullptr; }
  public:
    //  �t���[���X�g�ɓo�^
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

    //  �t���[���X�g�������
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
  //  �u���b�N�̃f�[�^�T�C�Y
  static constexpr vdl::uint kBlockSize = sizeof(BlockData);
  //  ��1�J�e�S���[�̕������̍ő�l
  static constexpr vdl::uint kMaxFirstVisionNum = 30;
  //  ��2�J�e�S���[�̕������̎w��
  static constexpr vdl::uint kSecondVisionExponent = 4;
  //  ��2�J�e�S���[�̕�����
  static constexpr vdl::uint kSecondVisionNum = 1 << kSecondVisionExponent;
  static constexpr vdl::uint kMaxSize = ~(1 << 31);
  //  
  static constexpr vdl::uint kMinSize = kSecondVisionNum;
  //  �u���b�N�̕����̍ŏ��T�C�Y
  static constexpr vdl::uint kMinBlockVisionSize = kBlockSize + kMinSize;
private:
  //  �����Ă���r�b�g�����擾
  static constexpr vdl::uint GetBitNum(vdl::uint _Value)
  {
    //  A SWAR Algorithm for Popcount
    //  https://www.playingwithpointers.com/blog/swar.html
    _Value -= (_Value >> 1) & 0x55555555;
    _Value = (_Value & 0x33333333) + ((_Value >> 2) & 0x33333333);
    return (((_Value + (_Value >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;

    ////  ���������r�b�g���J�E���g�A���S���Y��
    //_Value = (_Value & 0x55555555) + ((_Value >> 1) & 0x55555555);
    //_Value = (_Value & 0x33333333) + ((_Value >> 2) & 0x33333333);
    //_Value = (_Value & 0x0f0f0f0f) + ((_Value >> 4) & 0x0f0f0f0f);
    //_Value = (_Value & 0x00ff00ff) + ((_Value >> 8) & 0x00ff00ff);
    //return (_Value & 0x0000ffff) + ((_Value >> 16) & 0x0000ffff);
  }
  //  �ŏ�ʃr�b�g���擾
  static constexpr vdl::uint GetMSB(vdl::uint _Value)
  {
    vdl::uint Bit = 32;

    if (!(_Value & 0xffff0000)) { _Value <<= 16; Bit -= 16; }
    if (!(_Value & 0xff000000)) { _Value <<= 8; Bit -= 8; }
    if (!(_Value & 0xf0000000)) { _Value <<= 4; Bit -= 4; }
    if (!(_Value & 0xc0000000)) { _Value <<= 2; Bit -= 2; }
    if (!(_Value & 0x80000000)) { _Value <<= 1; --Bit; }

    return Bit - 1;
  }
  //  �ŉ��ʃr�b�g���擾
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
  //  �A���C�����g�𑵂���
  static constexpr vdl::uint64_t DownAlignment(vdl::uint64_t _Value, vdl::uint64_t _Alignment)
  {
    return _Value & ~(_Alignment - 1);
  }
  //  ��1�J�e�S���[�̃C���f�b�N�X���擾
  static constexpr vdl::uint GetFirstLevelIndex(vdl::uint _Size)
  {
    _Size |= (_Size >> 1);
    _Size |= (_Size >> 2);
    _Size |= (_Size >> 4);
    _Size |= (_Size >> 8);
    _Size |= (_Size >> 16);

    return GetBitNum(_Size) - 1;
  }
  //  ��2�J�e�S���[�̃C���f�b�N�X���擾
  static constexpr vdl::uint GetSecondLevelIndex(vdl::uint _Size, vdl::uint _FLI)
  {
    //  �ŏ�ʃr�b�g�����̂ݗL���ɂ���r�b�g
    const vdl::uint32_t Mask = ~(0xffffffff << _FLI);

    //  �ŏ�ʃr�b�g��1���̃r�b�g���番�����̎w�������擾
    return (_Size & Mask) >> (_FLI - kSecondVisionExponent);
  }
  //  �t���[���X�g�̃C���f�b�N�X���擾
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
    //  �����ȏ�̃}�X�N���쐬
    const vdl::uint32_t Mask = 0xffffffff << _FLI;

    //  ��ʂ̗L���ȃt���[���X�g���擾
    const vdl::uint32_t EnableFreeListBit = FirstFreeListBit_ & Mask;

    //  ���݂���Ȃ��ԋ߂��t���[���X�g���擾
    return (EnableFreeListBit == 0 ? kMaxFirstVisionNum : GetLSB(EnableFreeListBit));
  }
  vdl::uint GetFreeListSLI(vdl::uint _FLI, vdl::uint _SLI)
  {
    //  �����ȏ�̃}�X�N���쐬
    const vdl::uint32_t Mask = 0xffffffff << _SLI;

    //  ��ʂ̗L���ȃt���[���X�g���擾
    const vdl::uint32_t EnableFreeListBit = SecondFreeListBit_[_FLI] & Mask;

    //  ���݂���Ȃ��ԋ߂��t���[���X�g���擾
    return (EnableFreeListBit == 0 ? kSecondVisionNum : GetLSB(EnableFreeListBit));
  }
private:
  //  �t���[���X�g�ɓo�^
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

  //  �t���[���X�g�������
  void UnregistFreeList(BlockData* _pBlock, vdl::uint _FLI, vdl::uint _SLI)
  {
    _pBlock->UnregistFreeList();

    //  �t���[���X�g����ɂȂ�����}�X�N��ύX
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

  //  ������
  //  ���蓖�ėp�̃������͎��O�Ɋm�ۂ��Ă���
  void Initialize(void* _pBuffer, vdl::uint _Size);

  //  ���������蓖��
  //  ���蓖�Ă�̈悪�����ꍇnullptr���Ԃ�܂�
  [[nodiscard]] void* Allocate(vdl::uint _Size, vdl::uint _Alignment = 4);

  //  ���������
  void Release(void* _pBuffer);
};
