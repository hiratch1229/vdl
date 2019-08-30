#include "IDevice.hpp"

IDevice* IDevice::Create()
{
  return new CDevice;
}
