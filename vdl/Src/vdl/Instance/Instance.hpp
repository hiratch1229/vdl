#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type2.hpp>
#include <vdl/Matrix.hpp>
#include <vdl/Color.hpp>

struct Instance2D
{
  vdl::Matrix NDCTransform;
  vdl::float2 TexcoordScale;
  vdl::float2 TexcoordTranslate;
  vdl::ColorF Color;
};
struct Instance3D
{
  vdl::Matrix World;
  vdl::ColorF Color;
  //  TODO:SkinnedMesh
};
