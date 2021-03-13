#pragma once
#include "Types.hpp"

#include <atomic>
#include <thread>
#include <assert.h>

namespace vdl
{
  //  �r������N���X
  class SpinLock
  {
    std::atomic_flag Atomic_;
  public:
    SpinLock()noexcept = default;

    SpinLock(const SpinLock&) = delete;

    ~SpinLock() = default;

    SpinLock operator=(const SpinLock&) = delete;
  public:
    //  ���b�N���擾����
    void Lock()
    {
      //  ���b�N���擾����܂Ŗ������[�v
      while (!TryLock())
      {
        std::this_thread::yield();
      }
    }

    //  ���b�N�̎擾���o�����true��Ԃ�
    [[nodiscard]] bool TryLock()
    {
      return !Atomic_.test_and_set(std::memory_order_acquire/*���b�N��̓ǂݎ��𐳂����@�\������*/);
    }

    //  ���b�N�������
    void Unlock()
    {
      assert(Atomic_._Storage.load(std::memory_order_relaxed));

      Atomic_.clear(std::memory_order_release/*���b�N���̏������݂𐳂����@�\������*/);
    }
  };

  //  �ċA�I�ȃ��b�N���\�Ȕr������N���X
  class RecursiveSpinLock
  {
    static constexpr std::size_t kUnlockValue = 0u;
  private:
    std::atomic<std::size_t> Atomic_;
    uint ReferenceCount_;
  public:
    RecursiveSpinLock()
      : Atomic_(kUnlockValue), ReferenceCount_(0) {}

    RecursiveSpinLock(const RecursiveSpinLock&) = delete;

    ~RecursiveSpinLock() = default;

    RecursiveSpinLock& operator=(const RecursiveSpinLock&) = delete;
  public:
    //  ���b�N���擾����
    void Lock()
    {
      //  ���݂̃X���b�hID����n�b�V���l���擾
      const std::size_t ID = std::hash<std::thread::id>()(std::this_thread::get_id());

      //  ���݂̃X���b�h�����b�N���擾���Ă��Ȃ���
      if (Atomic_.load(std::memory_order_relaxed) != ID)
      {
        std::size_t UnlockValue = kUnlockValue;
        //  ���b�N����������܂Ŗ������[�v
        while (!Atomic_.compare_exchange_weak(UnlockValue, ID, std::memory_order_relaxed, std::memory_order_relaxed))
        {
          UnlockValue = kUnlockValue;
          std::this_thread::yield();
        }
      }

      ++ReferenceCount_;
      std::atomic_thread_fence(std::memory_order_acquire/*���b�N��̓ǂݎ��𐳂����@�\������*/);
    }

    //  ���b�N�̎擾���o�����true��Ԃ�
    [[nodiscard]] bool TryLock()
    {
      //  ���݂̃X���b�hID����n�b�V���l���擾
      const std::size_t ID = std::hash<std::thread::id>()(std::this_thread::get_id());

      bool isLock = false;
      {
        //  ���݂̃X���b�h�����b�N���擾���Ă��鎞
        if (Atomic_.load(std::memory_order_relaxed) == ID)
        {
          isLock = true;
        }
        else
        {
          std::size_t UnlockValue = kUnlockValue;
          //  ���b�N���������Ă���ꍇtrue
          isLock = Atomic_.compare_exchange_strong(UnlockValue, ID, std::memory_order_relaxed, std::memory_order_relaxed);
        }
      }

      if (isLock)
      {
        ++ReferenceCount_;
        std::atomic_thread_fence(std::memory_order_acquire/*���b�N��̓ǂݎ��𐳂����@�\������*/);
      }

      return isLock;
    }

    //  ���b�N�������
    void Unlock()
    {
      std::atomic_thread_fence(std::memory_order_release/*���b�N���̏������݂𐳂����@�\������*/);
      assert(Atomic_.load(std::memory_order_relaxed) == std::hash<std::thread::id>()(std::this_thread::get_id()));

      //  �Q�ƃJ�E���g��0�ɂȂ������Ƀ��b�N�������
      if (--ReferenceCount_ == 0)
      {
        Atomic_.store(kUnlockValue, std::memory_order_relaxed);
      }
    }
  };

  //  �����ɂ��ǂݍ��݂��\�Ȕr������N���X
  class SharedSpinLock
  {
    static constexpr uint kUnlockValue = 0u;
    static constexpr uint kWriteLockValue = 0x80000000u;
  private:
    std::atomic<uint> Atomic_;
  public:
    //  ���b�N���擾����
    void Lock()
    {
      //  ���b�N���擾����܂Ŗ������[�v
      while (!TryLock())
      {
        std::this_thread::yield();
      }
    }

    //  ���b�N�̎擾���o�����true��Ԃ�
    [[nodiscard]] bool TryLock()
    {
      uint UnlockValue = kUnlockValue;
      return Atomic_.compare_exchange_strong(UnlockValue, kWriteLockValue, std::memory_order_acquire/*���b�N��̓ǂݎ��𐳂����@�\������*/, std::memory_order_relaxed);
    }

    //  ���b�N�������
    void Unlock()
    {
      assert(Atomic_.load(std::memory_order_acquire) == kUnlockValue);

      Atomic_.store(kUnlockValue, std::memory_order_release/*���b�N���̏������݂𐳂����@�\������*/);
    }

    //  ���L���b�N���擾����
    void LockShared()
    {
      //  ���b�N���擾����܂Ŗ������[�v
      while (!TryLockShared())
      {
        std::this_thread::yield();
      }
    }

    //  ���L���b�N�̎擾���o�����true��Ԃ�
    [[nodiscard]] bool TryLockShared()
    {
      uint Value = Atomic_.load(std::memory_order_relaxed) & ~kWriteLockValue;
      return Atomic_.compare_exchange_strong(Value, Value + 1, std::memory_order_acquire/*���b�N��̓ǂݎ��𐳂����@�\������*/, std::memory_order_relaxed);
    }

    //  ���L���b�N�������
    void UnlockShared()
    {
      assert(Atomic_.load(std::memory_order_acquire) == kUnlockValue);

      Atomic_.store(Atomic_.load(std::memory_order_relaxed) - 1, std::memory_order_release/*���b�N���̏������݂𐳂����@�\������*/);
    }
  };

  template<class SpinLock>
  class SpinLockGuard
  {
    SpinLock* pLock_;
  public:
    explicit SpinLockGuard(SpinLock& _Lock)
      : pLock_(&_Lock)
    {
      pLock_->Lock();
    }

    ~SpinLockGuard()
    {
      pLock_->Unlock();
    }
  };

  class SharedSpinLockGuard
  {
    SharedSpinLock* pLock_;
  public:
    explicit SharedSpinLockGuard(SharedSpinLock& _Lock)
      : pLock_(&_Lock)
    {
      pLock_->LockShared();
    }

    ~SharedSpinLockGuard()
    {
      pLock_->UnlockShared();
    }
  };

  //  ���b�N���擾����
  template<class L>
  void Lock(L& _Lock)
  {
    _Lock.Lock();
  }

  //  ���b�N���擾����
  template<class L, class... Args>
  void Lock(L& _Lock, Args&... _Args)
  {
    Lock(_Lock0);
    Lock(_Args...);
  }
}
