#pragma once
#include "Types.hpp"

#include <atomic>
#include <thread>
#include <assert.h>

namespace vdl
{
  //  排他制御クラス
  class SpinLock
  {
    std::atomic_flag Atomic_;
  public:
    SpinLock()noexcept = default;

    SpinLock(const SpinLock&) = delete;

    ~SpinLock() = default;

    SpinLock operator=(const SpinLock&) = delete;
  public:
    //  ロックを取得する
    void Lock()
    {
      //  ロックを取得するまで無限ループ
      while (!TryLock())
      {
        std::this_thread::yield();
      }
    }

    //  ロックの取得が出来ればtrueを返す
    [[nodiscard]] bool TryLock()
    {
      return !Atomic_.test_and_set(std::memory_order_acquire/*ロック後の読み取りを正しく機能させる*/);
    }

    //  ロックを手放す
    void Unlock()
    {
      assert(Atomic_._Storage.load(std::memory_order_relaxed));

      Atomic_.clear(std::memory_order_release/*ロック中の書き込みを正しく機能させる*/);
    }
  };

  //  再帰的なロックが可能な排他制御クラス
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
    //  ロックを取得する
    void Lock()
    {
      //  現在のスレッドIDからハッシュ値を取得
      const std::size_t ID = std::hash<std::thread::id>()(std::this_thread::get_id());

      //  現在のスレッドがロックを取得していない時
      if (Atomic_.load(std::memory_order_relaxed) != ID)
      {
        std::size_t UnlockValue = kUnlockValue;
        //  ロックが手放されるまで無限ループ
        while (!Atomic_.compare_exchange_weak(UnlockValue, ID, std::memory_order_relaxed, std::memory_order_relaxed))
        {
          UnlockValue = kUnlockValue;
          std::this_thread::yield();
        }
      }

      ++ReferenceCount_;
      std::atomic_thread_fence(std::memory_order_acquire/*ロック後の読み取りを正しく機能させる*/);
    }

    //  ロックの取得が出来ればtrueを返す
    [[nodiscard]] bool TryLock()
    {
      //  現在のスレッドIDからハッシュ値を取得
      const std::size_t ID = std::hash<std::thread::id>()(std::this_thread::get_id());

      bool isLock = false;
      {
        //  現在のスレッドがロックを取得している時
        if (Atomic_.load(std::memory_order_relaxed) == ID)
        {
          isLock = true;
        }
        else
        {
          std::size_t UnlockValue = kUnlockValue;
          //  ロックが手放されている場合true
          isLock = Atomic_.compare_exchange_strong(UnlockValue, ID, std::memory_order_relaxed, std::memory_order_relaxed);
        }
      }

      if (isLock)
      {
        ++ReferenceCount_;
        std::atomic_thread_fence(std::memory_order_acquire/*ロック後の読み取りを正しく機能させる*/);
      }

      return isLock;
    }

    //  ロックを手放す
    void Unlock()
    {
      std::atomic_thread_fence(std::memory_order_release/*ロック中の書き込みを正しく機能させる*/);
      assert(Atomic_.load(std::memory_order_relaxed) == std::hash<std::thread::id>()(std::this_thread::get_id()));

      //  参照カウントが0になった時にロックを手放す
      if (--ReferenceCount_ == 0)
      {
        Atomic_.store(kUnlockValue, std::memory_order_relaxed);
      }
    }
  };

  //  複数による読み込みが可能な排他制御クラス
  class SharedSpinLock
  {
    static constexpr uint kUnlockValue = 0u;
    static constexpr uint kWriteLockValue = 0x80000000u;
  private:
    std::atomic<uint> Atomic_;
  public:
    //  ロックを取得する
    void Lock()
    {
      //  ロックを取得するまで無限ループ
      while (!TryLock())
      {
        std::this_thread::yield();
      }
    }

    //  ロックの取得が出来ればtrueを返す
    [[nodiscard]] bool TryLock()
    {
      uint UnlockValue = kUnlockValue;
      return Atomic_.compare_exchange_strong(UnlockValue, kWriteLockValue, std::memory_order_acquire/*ロック後の読み取りを正しく機能させる*/, std::memory_order_relaxed);
    }

    //  ロックを手放す
    void Unlock()
    {
      assert(Atomic_.load(std::memory_order_acquire) == kUnlockValue);

      Atomic_.store(kUnlockValue, std::memory_order_release/*ロック中の書き込みを正しく機能させる*/);
    }

    //  共有ロックを取得する
    void LockShared()
    {
      //  ロックを取得するまで無限ループ
      while (!TryLockShared())
      {
        std::this_thread::yield();
      }
    }

    //  共有ロックの取得が出来ればtrueを返す
    [[nodiscard]] bool TryLockShared()
    {
      uint Value = Atomic_.load(std::memory_order_relaxed) & ~kWriteLockValue;
      return Atomic_.compare_exchange_strong(Value, Value + 1, std::memory_order_acquire/*ロック後の読み取りを正しく機能させる*/, std::memory_order_relaxed);
    }

    //  共有ロックを手放す
    void UnlockShared()
    {
      assert(Atomic_.load(std::memory_order_acquire) == kUnlockValue);

      Atomic_.store(Atomic_.load(std::memory_order_relaxed) - 1, std::memory_order_release/*ロック中の書き込みを正しく機能させる*/);
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

  //  ロックを取得する
  template<class L>
  void Lock(L& _Lock)
  {
    _Lock.Lock();
  }

  //  ロックを取得する
  template<class L, class... Args>
  void Lock(L& _Lock, Args&... _Args)
  {
    Lock(_Lock0);
    Lock(_Args...);
  }
}
