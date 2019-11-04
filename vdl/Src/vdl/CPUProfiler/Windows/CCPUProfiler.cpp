#include "CCPUProfiler.hpp"

#include <vdl/Macro.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <PdhMsg.h>
#include <string>
#include <memory>
#include <thread>

void CCPUProfiler::Initialize()
{
  //  現在のカウンタ名を取得
  char CounterPath[PDH_MAX_COUNTER_PATH];
  {
    //  インスタンスリストの取得
    std::unique_ptr<char[]> InstanceList;
    {
      DWORD CounteListSize = 0;
      DWORD InstanceListSize = 0;
      if (PDH_MORE_DATA != ::PdhEnumObjectItemsA(nullptr, nullptr, "Process", nullptr, &CounteListSize, nullptr, &InstanceListSize, PERF_DETAIL_WIZARD, 0))
      {
        return;
      }

      CounteListSize = 0;
      InstanceList = std::make_unique<char[]>(InstanceListSize);
      if (PDH_MORE_DATA != ::PdhEnumObjectItemsA(nullptr, nullptr, "Process", nullptr, &CounteListSize, InstanceList.get(), &InstanceListSize, PERF_DETAIL_WIZARD, 0))
      {
        return;
      }
    }

    const DWORD CurrentProcessID = ::GetCurrentProcessId();

    PDH_COUNTER_PATH_ELEMENTS_A CounterPathElements;
    {
      CounterPathElements.szMachineName = nullptr;
      CounterPathElements.szObjectName = const_cast<char*>("Process");
      CounterPathElements.szParentInstance = nullptr;
      CounterPathElements.dwInstanceIndex = 0;
      CounterPathElements.szCounterName = const_cast<char*>("ID Process");
    }

    DWORD CounterPathSize = static_cast<DWORD>(vdl::Macro::ArraySize(CounterPath));
    
    //  現在のインスタンス名の取得
    char* InstanceName;
    {
      InstanceName = InstanceList.get();
      for (; *InstanceName != '\0'; InstanceName += lstrlenA(InstanceName) + 1)
      {
        CounterPathElements.szInstanceName = InstanceName;

        if (ERROR_SUCCESS != ::PdhMakeCounterPathA(&CounterPathElements, CounterPath, &CounterPathSize, 0))
        {
          continue;
        }

        struct GuardCloseQuery
        {
          HQUERY hQuery;
        public:
          ~GuardCloseQuery()
          {
            ::PdhCloseQuery(hQuery);
          }
        };

        GuardCloseQuery QueryGurad;
        if (ERROR_SUCCESS != ::PdhOpenQueryA(nullptr, 0, &QueryGurad.hQuery))
        {
          continue;
        }

        HCOUNTER hCounter;
        if (ERROR_SUCCESS != ::PdhAddCounterA(QueryGurad.hQuery, CounterPath, 0, &hCounter))
        {
          continue;
        }

        if (ERROR_SUCCESS != ::PdhCollectQueryData(QueryGurad.hQuery))
        {
          continue;
        }

        PDH_FMT_COUNTERVALUE FormatCounterValue;
        if (ERROR_SUCCESS != ::PdhGetFormattedCounterValue(hCounter, PDH_FMT_LONG, nullptr, &FormatCounterValue))
        {
          continue;
        }

        //  取得したカウントが現在のプロセスIDなら成功
        if (CurrentProcessID == FormatCounterValue.longValue)
        {
          break;
        }
      }
    }

    CounterPathElements.szInstanceName = InstanceName;
    CounterPathElements.szCounterName = const_cast<char*>("% Processor Time");
    CounterPathSize = static_cast<DWORD>(vdl::Macro::ArraySize(CounterPath));
    if (ERROR_SUCCESS != ::PdhMakeCounterPathA(&CounterPathElements, CounterPath, &CounterPathSize, 0))
    {
      return;
    }
  }

  //  新規クエリーの作成
  if (ERROR_SUCCESS != ::PdhOpenQueryA(nullptr, 0, &hQuery_))
  {
    return;
  }
  
  //  CPU使用率のカウンタをクエリーに追加
  if (ERROR_SUCCESS != ::PdhAddCounterA(hQuery_, CounterPath, 0, &hCounter_))
  {
    return;
  }
  
  // この時点での値を収集
  if (ERROR_SUCCESS != ::PdhCollectQueryData(hQuery_))
  {
    return;
  }

  CoreNum_ = std::thread::hardware_concurrency();
}

CCPUProfiler::~CCPUProfiler()
{
  ::PdhCloseQuery(hQuery_);
}

double CCPUProfiler::GetUseRate()const
{
  PDH_FMT_COUNTERVALUE FmtValue;
  {
    if (ERROR_SUCCESS != ::PdhCollectQueryData(hQuery_))
    {
      return 0.0;
    }
  
    if (ERROR_SUCCESS != ::PdhGetFormattedCounterValue(hCounter_, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, nullptr, &FmtValue))
    {
      return 0.0;
    }
  }
  
  return FmtValue.doubleValue / CoreNum_;
}
