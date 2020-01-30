#pragma once
#include <vdl/Fwd.hpp>

#include <queue>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <type_traits>

template<class T>
class Queue
{
  std::queue<T> Queue_;
  std::mutex Mutex_;
  std::condition_variable ConditionVariable_;
  bool isFinish_ = false;
public:
  Queue() = default;

  ~Queue() = default;

  //  データの挿入を強制する(挿入できるまで待機)
  template<class U, typename std::enable_if_t<std::is_constructible<T, U>::value, std::nullptr_t> = nullptr>
  void Push(U&& _Obj)
  {
    {
      std::lock_guard Lock(Mutex_);
      Queue_.push(_Obj);
    }

    ConditionVariable_.notify_one();
  }

  //  データの挿入を試みる
  template<class U, typename std::enable_if_t<std::is_constructible<T, U>::value, std::nullptr_t> = nullptr>
  bool TryPush(U&& _Obj)
  {
    {
      std::unique_lock Lock(Mutex_, std::try_to_lock);
      if (!Lock)
      {
        return false;
      }
      Queue_.push(_Obj);
    }

    ConditionVariable_.notify_one();
    return true;
  }

  //  データの取得を強制する(取得できるまで待機)
  bool Pop(T* _pData)
  {
    std::unique_lock Lock(Mutex_);
    while (Queue_.empty() && !isFinish_)
    {
      ConditionVariable_.wait(Lock);
    }

    //  終了
    if (Queue_.empty())
    {
      return false;
    }

    //  右辺参照が可能な限り右辺参照を使う
    if constexpr (std::is_move_assignable<T>::value)
    {
      (*_pData) = std::move(Queue_.front());
    }
    else
    {
      (*_pData) = Queue_.front();
    }
    Queue_.pop();

    return true;
  }

  //  データの取得を試みる
  bool TryPop(T* _pData)
  {
    std::unique_lock Lock(Mutex_, std::try_to_lock);
    if (!Lock || Queue_.empty())
    {
      return false;
    }

    //  右辺参照が可能な限り右辺参照を使う
    if constexpr (std::is_move_assignable<T>::value)
    {
      (*_pData) = std::move(Queue_.front());
    }
    else
    {
      (*_pData) = Queue_.front();
    }
    Queue_.pop();

    return true;
  }

  void Finish()noexcept
  {
    std::lock_guard Lock(Mutex_);
    isFinish_ = true;

    ConditionVariable_.notify_one();
  }

  [[nodiscard]] vdl::uint Size()const noexcept
  {
    std::lock_guard Lock(Mutex_);
    return Queue_.size();
  }

  [[nodiscard]] bool isEmpty()const noexcept
  {
    std::lock_guard Lock(Mutex_);
    return Queue_.empty();
  }
};

class ThreadPool
{
  using Func = std::function<void()>;
  std::vector<Queue<Func>> Queues_;
  std::vector<std::thread> Threads_;
  std::vector<bool> useThreads_;
  const vdl::uint ThreadNum_;
public:
  ThreadPool(vdl::uint _ThreadNum = std::thread::hardware_concurrency())
    : Queues_(_ThreadNum), useThreads_(_ThreadNum), ThreadNum_(_ThreadNum)
  {
    auto Work = [this](vdl::uint _Index)
    {
      while (true)
      {
        useThreads_[_Index] = false;

        Func Task;

        //  タスクの取得を試みる
        for (auto& Queue : Queues_)
        {
          if (Queue.TryPop(&Task))
          {
            break;
          }
        }

        //  タスクを取得出来ていない場合、タスク取得するまで待機
        if (!Task && !Queues_[_Index].Pop(&Task))
        {
          //  終了処理
          break;
        }

        useThreads_[_Index] = true;
        Task();
      }
    };

    for (vdl::uint i = 0; i < _ThreadNum; ++i)
    {
      Threads_.emplace_back(Work, i);
    }
  }

  ~ThreadPool()
  {
    for (auto& Queue : Queues_)
    {
      Queue.Finish();
    }
    for (auto& Thread : Threads_)
    {
      Thread.join();
    }
  }

  void Enqueue(const Func& _Func)
  {
    for (vdl::uint i = 0;; ++i %= ThreadNum_)
    {
      if (!useThreads_[i] && Queues_[i].TryPush(_Func))
      {
        break;
      }
    }
  }

  template<class F, class... Args>
  [[nodiscard]] auto EnqueueWithAcquireResult(F&& _Func, Args&&... _Args)->std::future<std::invoke_result_t<F, Args...>>
  {
    using ResultType = std::invoke_result_t<F, Args...>;
    using TaskType = std::packaged_task<ResultType()>;

    std::shared_ptr<TaskType> pTask = std::make_shared<TaskType>(std::bind(std::forward<F>(_Func), std::forward<Args>(_Args)...));
    Func Func = [pTask] { (*pTask)(); };
    std::future<ResultType> Result = pTask->get_future();

    Enqueue(Func);

    return Result;
  }
};
