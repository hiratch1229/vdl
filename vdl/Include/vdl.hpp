#pragma once
#pragma comment (linker, "/entry:\"wWinMainCRTStartup\"")

#if defined _WINDOWS
#define VDL_USING_WINDOWS
#endif 

//  �g�p�`��API
#define VDL_USING_DIRECTX11
//#define VDL_USING_DIRECTX12
//#define VDL_USING_VULKAN

//--------------------------------------------------
//  ��{���
//--------------------------------------------------
#include <vdl/Platform.hpp>
#include <vdl/Types.hpp>
#include <vdl/Fwd.hpp>
#include <vdl/ID.hpp>
#include <vdl/System.hpp>
#include <vdl/Window.hpp>
#include <vdl/Constants.hpp>

//--------------------------------------------------
//  ���w
//--------------------------------------------------
#include <vdl/MathConstants.hpp>
#include <vdl/Math.hpp>
#include <vdl/Type2.hpp>
#include <vdl/Type3.hpp>
#include <vdl/Type4.hpp>
#include <vdl/Half.hpp>
#include <vdl/Quaternion.hpp>
#include <vdl/Matrix.hpp>
#include <vdl/Angle.hpp>

//--------------------------------------------------
//  �`��
//--------------------------------------------------
#include <vdl/Color.hpp>
#include <vdl/Camera.hpp>
#include <vdl/Vertex.hpp>
#include <vdl/Topology.hpp>
#include <vdl/InputLayout.hpp>
#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>
#include <vdl/BlendState.hpp>
#include <vdl/DepthStencilState.hpp>
#include <vdl/RasterizerState.hpp>
#include <vdl/GraphicsState.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Format.hpp>
#include <vdl/Image.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>
#include <vdl/MeshData.hpp>
#include <vdl/MotionData.hpp>
#include <vdl/Model.hpp>
#include <vdl/Renderer.hpp>
#include <vdl/Renderer2D.hpp>
#include <vdl/Renderer3D.hpp>
#include <vdl/GUI.hpp>

//--------------------------------------------------
//  ����
//--------------------------------------------------
#include <vdl/Button.hpp>
#include <vdl/Keyboard.hpp>
#include <vdl/Mouse.hpp>
#include <vdl/Gamepad.hpp>
#include <vdl/XInput.hpp>

//--------------------------------------------------
//  2D�����蔻��
//--------------------------------------------------
#include <vdl/Circle.hpp>
#include <vdl/Collision2D.hpp>

//--------------------------------------------------
//  3D�����蔻��
//--------------------------------------------------
#include <vdl/Box.hpp>
#include <vdl/Sphere.hpp>
#include <vdl/Capsule.hpp>
#include <vdl/Collision3D.hpp>

//--------------------------------------------------
//  ���[�e�B���e�B�[
//--------------------------------------------------
#include <vdl/Singleton.hpp>
#include <vdl/Macro.hpp>
#include <vdl/Hash.hpp>
#include <vdl/Serialize.hpp>
#include <vdl/Random.hpp>
