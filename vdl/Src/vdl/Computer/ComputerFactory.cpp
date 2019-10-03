#include "IComputer.hpp"

#include "CComputer.hpp"

IComputer* IComputer::Create()
{
  return new CComputer;
}
