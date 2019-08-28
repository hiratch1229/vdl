#pragma once
#include "Type2.hpp"
#include "Type3.hpp"
#include "Type3.hpp"
#include "Quaternion.hpp"
#include "Matrix.hpp"
#include "Angle.hpp"
#include "Angle.hpp"
#include "Vertex.hpp"

#include "../ThirdParty/cereal/cereal.hpp"
#include "../ThirdParty/cereal/archives/binary.hpp"
#include "../ThirdParty/cereal/types/vector.hpp"
#include "../ThirdParty/cereal/types/string.hpp"

#define CEREAL_SERIALIZE(...)\
template<class Archive>\
void serialize(Archive& _Archive)\
{\
  _Archive(__VA_ARGS__);\
}

template<class T>
inline void ImportFromBinary(const char* _FilePath, T& _Obj)
{
  std::ifstream IStream(_FilePath, std::ios::binary);
  {
    cereal::BinaryInputArchive InputArchive(IStream);
    InputArchive(_Obj);
  }
  IStream.close();
}

template<class T>
inline void ExportToBinary(const char* _FilePath, T& _Obj)
{
  std::ofstream OStream(_FilePath, std::ios::binary);
  {
    cereal::BinaryOutputArchive OutputArchive(OStream);
    OutputArchive(_Obj);
  }
  OStream.close();
}

namespace vdl
{
  template<class Archive, class T>
  inline void serialize(Archive& _Archive, Type2<T>& _v)
  {
    _Archive(_v.x, _v.y);
  }

  template<class Archive, class T>
  inline void serialize(Archive& _Archive, Type3<T>& _v)
  {
    _Archive(_v.x, _v.y, _v.z);
  }

  template<class Archive, class T>
  inline void serialize(Archive& _Archive, Type4<T>& _v)
  {
    _Archive(_v.x, _v.y, _v.z, _v.w);
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, Quaternion& _q)
  {
    _Archive(_q.x, _q.y, _q.z, _q.w);
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, Matrix& _m)
  {
    _Archive(_m.r[0], _m.r[1], _m.r[2], _m.r[3]);
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, Degree& _Angle)
  {
    _Archive(static_cast<float>(_Angle));
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, Radian& _Angle)
  {
    _Archive(static_cast<float>(_Angle));
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, Color& _Color)
  {
    _Archive(_Color.R, _Color.G, _Color.B, _Color.A);
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, ColorF& _Color)
  {
    _Archive(_Color.R, _Color.G, _Color.B, _Color.A);
  }

  template<class Archive>
  inline void serialize(Archive& _Archive, Vertex3D& _Vertex)
  {
    _Archive(_Vertex.Position, _Vertex.Normal, _Vertex.Tangent, _Vertex.Texcoord, _Vertex.BoneWeights, _Vertex.BoneIndices);
  }
}
