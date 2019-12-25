#include <vdl/Collision3D.hpp>

#include <vdl/Sphere.hpp>
#include <vdl/Capsule.hpp>

namespace vdl::Collision3D
{
  bool isHit(const Geometry3D::Sphere& _Sphere0, const Geometry3D::Sphere& _Sphere1)
  {
    return (_Sphere1.Position - _Sphere0.Position).LengthSq() <= ((_Sphere0.Radius + _Sphere1.Radius) * (_Sphere0.Radius + _Sphere1.Radius));
  }

  bool isHit(const Geometry3D::Sphere& _Sphere, const Geometry3D::Capsule& _Capsule)
  {
    //  カプセルの向き
    const float3 CapsuleDirection = _Capsule.Direction.Normalize();

    //  カプセルのベクトル上の最短距離を取得
    const float Dot = CapsuleDirection.Dot(_Sphere.Position - _Capsule.Position);

    //  カプセルのベクトル上の最短点
    float3 Position;
    {
      //  カプセルの始点より外側
      if (Dot <= 0.0f)
      {
        Position = _Capsule.Position;
      }
      //  カプセルの終点より外側
      else if (Dot >= _Capsule.Direction.Length())
      {
        Position = _Capsule.Position + _Capsule.Direction;
      }
      //  カプセルの内側
      else
      {
        Position = _Capsule.Position + CapsuleDirection * Dot;
      }
    }

    //  求めた点とカプセルの半径で球として当たり判定
    return isHit(_Sphere, Geometry3D::Sphere(Position, _Capsule.Radius));
  }
}
