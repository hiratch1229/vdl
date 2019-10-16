#include "CCPUProfiler.hpp"

void CCPUProfiler::Initialize()
{
  //  �V�K�N�G���[�̍쐬
  ::PdhOpenQueryA(nullptr, 0, &hQuery_);

  //  CPU�g�p���̃J�E���^���N�G���[�ɒǉ�
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
