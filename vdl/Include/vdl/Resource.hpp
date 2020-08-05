#pragma once
#include "Fwd.hpp"
#include "Constants.hpp"

namespace vdl
{
  class Resource
  {
  protected:
    ID ID_;
  public:
    Resource()noexcept
      : ID_(Constants::kDisableID) {}

    Resource(ID _ID)noexcept
      : ID_(_ID) {}
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == Constants::kDisableID; }
  };
}
