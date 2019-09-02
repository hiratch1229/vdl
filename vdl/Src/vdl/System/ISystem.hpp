#pragma once
#include <vdl/Types.hpp>

class ISystem
{
public:
  static ISystem* Create();

  ISystem() = default;

  virtual ~ISystem() = default;

  virtual void Initialize() = 0;

  virtual bool Update() = 0;

  virtual void EnableDefaultActions(vdl::uint _DefaultActionFlags) = 0;

  virtual void DisableDefaultActions(vdl::uint _DefaultActionFlags) = 0;

  virtual void ReportDefaultActions(vdl::uint _DefaultActionFlags) = 0;

  virtual void SetMaxFramerate(vdl::uint _MaxFramerate) = 0;

  virtual float GetDeltaTime() = 0;

  virtual void Pause() = 0;

  virtual void Resume() = 0;
};
