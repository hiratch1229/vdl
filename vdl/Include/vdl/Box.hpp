#pragma once
#include "Fwd.hpp"
#include "type3.hpp"
#include "Quaternion.hpp"

namespace vdl::Geometry3D
{
  struct AABB
  {
    float3 Center;
    float3 Extent;
  public:
    AABB() = default;

    constexpr AABB(const float3& _Center, const float3& _Extent)noexcept
      : Center(_Center), Extent(_Extent) {}

    constexpr AABB(const float3& _Extent)noexcept
      : Center(0.0f), Extent(_Extent) {}
  public:
    [[nodiscard]] constexpr bool operator==(const AABB& _AABB)const noexcept { return Center == _AABB.Center && Extent == _AABB.Extent; }

    [[nodiscard]] constexpr bool operator!=(const AABB& _AABB)const noexcept { return Center != _AABB.Center || Extent != _AABB.Center; }
  };

  struct OBB : public AABB
  {
    Quaternion Orientation;
  public:
    OBB() = default;

    constexpr OBB(const float3& _Center, const float3& _Extent, const Quaternion& _Orientation)noexcept
      : AABB(_Center, _Extent), Orientation(_Orientation) {}

    constexpr OBB(const float3& _Extent)noexcept
      : AABB(_Extent), Orientation(Quaternion::Identity()) {}

    constexpr OBB(const float3& _Extent, const Quaternion& _Orientation)noexcept
      : AABB(_Extent), Orientation(_Orientation) {}
  public:
    [[nodiscard]] constexpr bool operator==(const OBB& _OBB)const noexcept { return Center == _OBB.Center && Extent == _OBB.Extent && Orientation == _OBB.Orientation; }

    [[nodiscard]] constexpr bool operator!=(const OBB& _OBB)const noexcept { return Center != _OBB.Center || Extent != _OBB.Center || Orientation != _OBB.Orientation; }
  };
}
