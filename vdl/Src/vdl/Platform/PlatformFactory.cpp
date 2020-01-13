#include "IPlatform.hpp"

#include "CPlatform.hpp"

IPlatform* IPlatform::Create()
{
  return new CPlatform;
}
