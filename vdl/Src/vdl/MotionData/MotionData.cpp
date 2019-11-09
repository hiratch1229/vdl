#include <vdl/MotionData.hpp>

#include <vdl/Model.hpp>
#include <vdl/Math.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

#include <vdl/Constants/Constants.hpp>

namespace vdl
{
  void UpdateMotion(const SkinnedMesh& _SkinnedMesh, MotionData* _pMotionData, float _DeltaTime, bool _isLoop)
  {
    assert(!_SkinnedMesh.isEmpty());

    Mesh* pMesh = Engine::Get<IModelManager>()->GetMesh(_SkinnedMesh.GetID());

    assert(!pMesh->Animations.empty());

    _pMotionData->Frame += _DeltaTime;

    const size_t SkeletalNum = pMesh->Animations[_pMotionData->Number].Skeletals.size();
    if (_isLoop)
    {
      const float MaxAnimationTime = ::Constants::kAnimationSamplingTime * SkeletalNum;
      while (_pMotionData->Frame >= MaxAnimationTime)
      {
        _pMotionData->Frame -= MaxAnimationTime;
      }
    }
    else
    {
      _pMotionData->Frame = Math::Min(_pMotionData->Frame, ::Constants::kAnimationSamplingTime * (SkeletalNum - 1));
    }
  }
}
