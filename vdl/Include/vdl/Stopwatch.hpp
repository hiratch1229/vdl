#pragma once
#include <chrono>

namespace vdl
{
  class Stopwatch
  {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<double>;
  private:
    bool isStart_ = false;
    bool isPause_ = true;
    TimePoint Start_;
    Duration Accumulation_ = Duration(0.0);
  public:
    [[nodiscard]] bool isStart()const noexcept { return isStart_ && !isPause_; }
    [[nodiscard]] bool isPause()const noexcept { return isStart_ && isPause_; }
  public:
    Stopwatch() = default;

    //  計測を開始
    void Start()noexcept
    {
      if (!isPause_)
      {
        return;
      }

      isStart_ = true;
      isPause_ = false;
      Start_ = Clock::now();
    }

    //  計測を一時停止
    void Pause()noexcept
    {
      Accumulation_ = GetDeltaTime();
      isPause_ = true;
    }

    //  経過時間をリセット
    void Reset()noexcept
    {
      isStart_ = false;
      isPause_ = true;
      Accumulation_ = Duration(0.0);
    }

    //  計測を再開
    void Resume()noexcept
    {
      if (!isPause())
      {
        return;
      }

      Start();
    }

    //  経過時間をリセットして開始
    void Restart()noexcept
    {
      Reset();

      Start();
    }

    // 経過時間を取得(秒)
    [[nodiscard]] Duration GetDeltaTime()const noexcept
    {
      const TimePoint CurrentTime = Clock::now();

      if (isPause_)
      {
        return Accumulation_;
      }

      return (CurrentTime - Start_) + Accumulation_;
    }
  };
}
