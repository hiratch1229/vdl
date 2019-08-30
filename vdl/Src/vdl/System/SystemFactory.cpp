#include "ISystem.hpp"

#include "CSystem.hpp"

ISystem* ISystem::Create()
{
  return new CSystem;
}
