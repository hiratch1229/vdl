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

    //  �v�����J�n
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

    //  �v�����ꎞ��~
    void Pause()noexcept
    {
      Accumulation_ = GetDeltaTime();
      isPause_ = true;
    }

    //  �o�ߎ��Ԃ����Z�b�g
    void Reset()noexcept
    {
      isStart_ = false;
      isPause_ = true;
      Accumulation_ = Duration(0.0);
    }

    //  �v�����ĊJ
    void Resume()noexcept
    {
      if (!isPause())
      {
        return;
      }

      Start();
    }

    //  �o�ߎ��Ԃ����Z�b�g���ĊJ�n
    void Restart()noexcept
    {
      Reset();

      Start();
    }

    // �o�ߎ��Ԃ��擾(�b)
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
