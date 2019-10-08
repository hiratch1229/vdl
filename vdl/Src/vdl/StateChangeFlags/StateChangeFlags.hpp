#pragma once
#include <vdl/Fwd.hpp>

template<class CommandType, class FlagType = vdl::uint>
class StateChangeFlags
{
  FlagType Flags_;
public:
  StateChangeFlags() = default;

  void Clear() { Flags_ = 0; }

  [[nodiscard]] bool isEmpty()const { return Flags_ == 0; }

  [[nodiscard]] bool Has(CommandType _Command)const { return Flags_ & (1 << static_cast<vdl::uint>(_Command)); }

  void Set(CommandType _Command) { Flags_ |= (1 << static_cast<vdl::uint>(_Command)); }

  void Cancel(CommandType _Command) { Flags_ &= ~(1 << static_cast<vdl::uint>(_Command)); }
};
