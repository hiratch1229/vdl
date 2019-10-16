#include "CCPUProfiler.hpp"

void CCPUProfiler::Initialize()
{
  //  新規クエリーの作成
  ::PdhOpenQueryA(nullptr, 0, &hQuery_);

  //  CPU使用率のカウンタをクエリーに追加
  ::PdhAddCounterA(hQuery_, "\\Processor(_Total)\\% Processor Time", 0, &hCounter_);

  ::PdhCollectQueryData(hQuery_);
}

CCPUProfiler::~CCPUProfiler()
{
  ::PdhCloseQuery(hQuery_);
}

double CCPUProfiler::GetAsyncUseRate()const
{
  ::PdhCollectQueryData(hQuery_);

  PDH_FMT_COUNTERVALUE FmtValue;
  ::PdhGetFormattedCounterValue(hCounter_, PDH_FMT_DOUBLE, nullptr, &FmtValue);

  return FmtValue.doubleValue;
}
