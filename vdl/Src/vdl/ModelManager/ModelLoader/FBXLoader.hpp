#pragma once
#include "ModelData.hpp"

#define USING_FBX_SDK

#ifdef USING_FBX_SDK
#include <ThirdParty/FBXSDK/include/fbxsdk.h>

class FBXLoader
{
public:
  [[nodiscard]] static bool CheckSupportFormat(const std::string& _FileFormat);
private:
  fbxsdk::FbxManager* pManager_;
public:
  FBXLoader();

  ~FBXLoader();

  [[nodiscard]] ModelData Load(const char* _FilePath)const;
};
#else
class FBXLoader
{
public:
  [[nodiscard]] static bool CheckSupportFormat(const std::string& _FileFormat);
public:
  FBXLoader() = default;

  ~FBXLoader() = default;

  [[nodiscard]] ModelData Load(const char* _FilePath)const;
};
#endif // USING_FBX_SDK
