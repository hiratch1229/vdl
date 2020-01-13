#pragma once

class IPlatform;
class ISystem;
class IWindow;
class IDevice;
class IDeviceContext;
class ISwapChain;
class ICPUProfiler;
class IMemoryProfiler;
class IKeyboard;
class IMouse;
class IXInput;
class IGamepad;
class ISoundManager;
class ITextureManager;
class IModelManager;
class IBufferManager;
class IShaderManager;
class IRenderer;
class IComputer;
class IGUI;

template <class Interface>
class Component
{
  Interface* pInterface_ = nullptr;
public:
  [[nodiscard]] Interface* Get() { return pInterface_; }
  void Release()
  {
    delete pInterface_;
    pInterface_ = nullptr;
  }
public:
  Interface* operator->() { return pInterface_; }
public:
  Component()
    : pInterface_(Interface::Create()) {}
};

class Engine
{
  inline static Engine* pEngine = nullptr;
private:
  Component<IPlatform> pPlatform_;
  Component<ISystem> pSystem_;
  Component<IWindow> pWindow_;
  Component<IDevice> pDevice_;
  Component<IDeviceContext> pDeviceContext_;
  Component<ISwapChain> pSwapChain_;
  Component<ICPUProfiler> pCPUProfiler_;
  Component<IMemoryProfiler> pMemoryProfiler_;
  Component<IKeyboard> pKeyboard_;
  Component<IMouse> pMouse_;
  Component<IXInput> pXInput_;
  Component<IGamepad> pGamepad_;
  Component<ISoundManager> pSoundManager_;
  Component<ITextureManager> pTextureManager_;
  Component<IModelManager> pModelManager_;
  Component<IBufferManager> pBufferManager_;
  Component<IShaderManager> pShaderManager_;
  Component<IRenderer> pRenderer_;
  Component<IComputer> pComputer_;
  Component<IGUI> pGUI_;
public:
  Engine();
  ~Engine();
public:
  [[nodiscard]] static bool isActive() { return pEngine != nullptr; }
public:
  template<class T> [[nodiscard]] static T* Get() { return nullptr; }
  template<> [[nodiscard]] static IPlatform* Get() { return pEngine->pPlatform_.Get(); }
  template<> [[nodiscard]] static ISystem* Get() { return pEngine->pSystem_.Get(); }
  template<> [[nodiscard]] static IWindow* Get() { return pEngine->pWindow_.Get(); }
  template<> [[nodiscard]] static IDevice* Get() { return pEngine->pDevice_.Get(); }
  template<> [[nodiscard]] static IDeviceContext* Get() { return pEngine->pDeviceContext_.Get(); }
  template<> [[nodiscard]] static ISwapChain* Get() { return pEngine->pSwapChain_.Get(); }
  template<> [[nodiscard]] static ICPUProfiler* Get() { return pEngine->pCPUProfiler_.Get(); }
  template<> [[nodiscard]] static IMemoryProfiler* Get() { return pEngine->pMemoryProfiler_.Get(); }
  template<> [[nodiscard]] static IKeyboard* Get() { return pEngine->pKeyboard_.Get(); }
  template<> [[nodiscard]] static IMouse* Get() { return pEngine->pMouse_.Get(); }
  template<> [[nodiscard]] static IXInput* Get() { return pEngine->pXInput_.Get(); }
  template<> [[nodiscard]] static IGamepad* Get() { return pEngine->pGamepad_.Get(); }
  template<> [[nodiscard]] static ISoundManager* Get() { return pEngine->pSoundManager_.Get(); }
  template<> [[nodiscard]] static ITextureManager* Get() { return pEngine->pTextureManager_.Get(); }
  template<> [[nodiscard]] static IModelManager* Get() { return pEngine->pModelManager_.Get(); }
  template<> [[nodiscard]] static IBufferManager* Get() { return pEngine->pBufferManager_.Get(); }
  template<> [[nodiscard]] static IShaderManager* Get() { return pEngine->pShaderManager_.Get(); }
  template<> [[nodiscard]] static IRenderer* Get() { return pEngine->pRenderer_.Get(); }
  template<> [[nodiscard]] static IComputer* Get() { return pEngine->pComputer_.Get(); }
  template<> [[nodiscard]] static IGUI* Get() { return pEngine->pGUI_.Get(); }
};
