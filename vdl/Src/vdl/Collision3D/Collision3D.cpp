#include <vdl/Collision3D.hpp>

#include <vdl/Sphere.hpp>
#include <vdl/Capsule.hpp>

namespace vdl::Collision3D
{
  bool isHit(const Geometry3D::Sphere& _Sphere0, const Geometry3D::Sphere& _Sphere1)noexcept
  {
    return (_Sphere1.Position - _Sphere0.Position).LengthSq() <= ((_Sphere0.Radius + _Sphere1.Radius) * (_Sphere0.Radius + _Sphere1.Radius));
  }

  bool isHit(const Geometry3D::Sphere& _Sphere, const Geometry3D::Capsule& _Capsule)noexcept
  {
    //  �J�v�Z���̌���
    const float3 CapsuleDirection = _Capsule.Direction.Normalize();

    //  �J�v�Z���̃x�N�g����̍ŒZ�������擾
    const float Dot = CapsuleDirection.Dot(_Sphere.Position - _Capsule.Position);

    //  �J�v�Z���̃x�N�g����̍ŒZ�_
    float3 Position;
    {
      //  �J�v�Z���̎n�_���O��
      if (Dot <= 0.0f)
      {
        Position = _Capsule.Position;
      }
      //  �J�v�Z���̏I�_���O��
      else if (Dot >= _Capsule.Direction.Length())
      {
        Position = _Capsule.Position + _Capsule.Direction;
      }
      //  �J�v�Z���̓���
      else
      {
        Position = _Capsule.Position + CapsuleDirection * Dot;
      }
    }

    //  ���߂��_�ƃJ�v�Z���̔��a�ŋ��Ƃ��ē����蔻��
    return isHit(_Sphere, Geometry3D::Sphere(Position, _Capsule.Radius));
  }
}
