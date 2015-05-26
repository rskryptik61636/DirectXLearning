//=======================================================================================
// d3dUtil.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

// Modified by Rohit S for use in DXEngine

#ifndef D3DUTIL_H
#define D3DUTIL_H

// Let VC++ know we are compiling for WinXP and up.  This let's us use
// API functions specific to WinXP (e.g., WM_INPUT API).
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

 
// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
	#define D3D_DEBUG_INFO
	#endif
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// link against DXEngine(d).lib so that any source under DXEngine is available
#ifndef DXENGINE_LIB_INTERNAL
	#if defined(DEBUG) || defined(_DEBUG)
		#pragma message("Linking against DXEngined.lib")
		#pragma comment(lib, "DXEngined.lib")
	#else
		#pragma message("Linking against DXEngine.lib")
		#pragma comment(lib, "DXEngine.lib")
	#endif	// DEBUG || _DEBUG
#endif	// DXENGINE_LIB_INTERNAL

#include <atlbase.h>	// added for CComPtr
#include <memory>		// included for smart ptr stuff
#include <vector>		// included for wherever dynamically resizable buffers are required
#include <utility>		// included for STL utility classes
#include <type_traits>	// including for type checking stuff
#include <array>		// included for wherever static buffers are required
#include <map>			// included for associative arrays
#include <random>		// included for random number generation

// @TODO: remove when done testing
#if 0
#include <d3dx10.h>
#endif	// 0

// modded version of dxerr
#include "dxerr.h"

// include D3D_11_2 header
#include <d3d11_2.h>

// include DirectXTK headers
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

// typedefs for convenience sake
namespace Math = DirectX::SimpleMath;	
typedef Math::Vector2 DXVector2;
typedef Math::Vector3 DXVector3;
typedef Math::Vector4 DXVector4;
typedef Math::Matrix DXMatrix;
typedef Math::Plane DXPlane;
typedef std::unique_ptr<DirectX::SpriteBatch> SpriteBatchPtr;
typedef std::unique_ptr<DirectX::SpriteFont> SpriteFontPtr;

// include DirectXMath header
#include <DirectXMath.h>

// include Effects11 header
#include <d3dx11effect.h>

// include D3D compiler header for shader stuff
#include <D3DCompiler.h>

// filesystem stuff (new w/ C++ 11)
#include <filesystem>
typedef std::tr2::sys::wpath wpath;
typedef std::tr2::sys::path npath;

//*****************************************************************************
// Filesystem utility functions.
//*****************************************************************************
wpath narrowToWidePath(const npath &src);
npath wideToNarrowPath(const wpath &src);

#include <cassert>


//*****************************************************************************
// Simple d3d error checker for book demos.
//*****************************************************************************

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTraceW(__FILEW__, (DWORD)__LINE__, hr, L#x, TRUE); \
		}                                                      \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 


//*****************************************************************************
// Convenience macro for releasing COM objects.
//*****************************************************************************

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
#define ReleaseCOMPtr(x) { if(x){ x.Release(); x = 0; } }

//*****************************************************************************
// Convenience macro for including shader files in the same dir
// as the one being compiled.
// Source: http://msdn.microsoft.com/en-us/library/windows/desktop/hh446872.aspx
//*****************************************************************************

#ifndef D3D_COMPILE_STANDARD_FILE_INCLUDE
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
#endif	// D3D_COMPILE_STANDARD_FILE_INCLUDE

//*****************************************************************************
// Convenience functions.
//*****************************************************************************

// D3DX has been deprecated as of Win 8.1, this is the original impl as in d3dx10.h
// source: http://www.falconview.org/svn/FalconView/trunk/public/third_party/directx/include/D3DX10.h
#ifndef D3DX10INLINE
#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#define D3DX10INLINE __forceinline
#else
#define D3DX10INLINE __inline
#endif
#else
#ifdef __cplusplus
#define D3DX10INLINE inline
#else
#define D3DX10INLINE
#endif
#endif
#endif

// Converts ARGB 32-bit color format to ABGR 32-bit color format.
D3DX10INLINE UINT ARGB2ABGR(UINT argb)
{
	BYTE A = (argb >> 24) & 0xff;
	BYTE R = (argb >> 16) & 0xff;
	BYTE G = (argb >>  8) & 0xff;
	BYTE B = (argb >>  0) & 0xff;

	return (A << 24) | (B << 16) | (G << 8) | (R << 0);
}

// Returns random float in [0, 1).
D3DX10INLINE float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
D3DX10INLINE float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

// Returns random vector on the unit sphere.
D3DX10INLINE DXVector3 RandUnitVec3()
{
	DXVector3 v(RandF(), RandF(), RandF());
	v.Normalize();
	//D3DXVec3Normalize(&v, &v);	// @TODO: remove when done testing
	return v;
}
 
template<typename T>
D3DX10INLINE T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
D3DX10INLINE T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}
 
template<typename T>
D3DX10INLINE T Lerp(const T& a, const T& b, float t)
{
	return a + (b-a)*t;
}

template<typename T>
D3DX10INLINE T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x); 
}

//*****************************************************************************
// typedefs for convenience sake 
//*****************************************************************************
typedef CComPtr<ID3D11RasterizerState> RasterizerStatePtr;
typedef CComPtr<ID3D11BlendState> BlendStatePtr;
typedef CComPtr<ID3D11DepthStencilState> DepthStencilStatePtr;
typedef CComPtr<ID3D11DepthStencilView> DepthStencilViewPtr;
typedef CComPtr<ID3D11ShaderResourceView> ShaderResourceViewPtr;
typedef ID3D11ShaderResourceView* ShaderResourceViewRawPtr;
typedef CComPtr<ID3D11RenderTargetView> RenderTargetViewPtr;
typedef CComPtr<ID3D11UnorderedAccessView> UnorderedAccessViewPtr;
typedef ID3D11UnorderedAccessView* UnorderedAccessViewRawPtr;
typedef CComPtr<ID3D11Resource> ResourcePtr;

typedef ID3DX11EffectTechnique* EffectTechniquePtr;
typedef ID3DX11EffectMatrixVariable* EffectMatrixVariablePtr;
typedef ID3DX11EffectVariable* EffectVariablePtr;
typedef ID3DX11EffectShaderResourceVariable* EffectShaderResourceVariablePtr;
typedef ID3DX11EffectScalarVariable* EffectScalarVariablePtr;

typedef CComPtr<ID3DBlob> BlobPtr;
typedef ID3D11VertexShader* VertexShaderRawPtr;
typedef CComPtr<ID3D11VertexShader> VertexShaderPtr;
typedef ID3D11PixelShader* PixelShaderRawPtr;
typedef CComPtr<ID3D11PixelShader> PixelShaderPtr;
typedef ID3D11GeometryShader* GeometryShaderRawPtr;
typedef CComPtr<ID3D11GeometryShader> GeometryShaderPtr;
typedef ID3D11HullShader* HullShaderRawPtr;
typedef CComPtr<ID3D11HullShader> HullShaderPtr;
typedef ID3D11DomainShader* DomainShaderRawPtr;
typedef CComPtr<ID3D11DomainShader> DomainShaderPtr;
typedef ID3D11ComputeShader* ComputeShaderRawPtr;
typedef CComPtr<ID3D11ComputeShader> ComputeShaderPtr;
typedef CComPtr<ID3D11ShaderReflection> ShaderReflectionPtr;
typedef CComPtr<ID3D11SamplerState> SamplerStatePtr;
typedef ID3D11SamplerState* SamplerStateRawPtr;
typedef CComPtr<ID3D11Texture2D> Texture2DPtr;
typedef CComPtr<ID3D11Texture1D> Texture1DPtr;

typedef CComPtr<ID3DX11Effect> EffectPtr;
typedef CComPtr<ID3D11InputLayout> InputLayoutPtr;
typedef CComPtr<ID3D11Buffer> BufferPtr;
typedef ID3D11Buffer* BufferRawPtr;

typedef CComPtr<ID3D11Device> DevicePtr;
typedef CComPtr<ID3D11DeviceContext> DeviceContextPtr;

//*****************************************************************************
// Constants
//*****************************************************************************

//const float INFINITY = FLT_MAX;
const float PI       = 3.14159265358979323f;
const float MATH_EPS = 0.0001f;

// D3DX has been deprecated as of Win 8.1, XMFLOAT4 is the D3D11 equivalent of D3DXCOLOR (which is wrapped by DXColor)
typedef Math::Vector4 DXColor;
const DXColor WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const DXColor BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const DXColor RED(1.0f, 0.0f, 0.0f, 1.0f);
const DXColor GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const DXColor BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const DXColor YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
const DXColor CYAN(0.0f, 1.0f, 1.0f, 1.0f);
const DXColor MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);

const DXColor BEACH_SAND(1.0f, 0.96f, 0.62f, 1.0f);
const DXColor LIGHT_YELLOW_GREEN(0.48f, 0.77f, 0.46f, 1.0f);
const DXColor DARK_YELLOW_GREEN(0.1f, 0.48f, 0.19f, 1.0f);
const DXColor DARKBROWN(0.45f, 0.39f, 0.34f, 1.0f);

//*****************************************************************************
// Shader constant buffer structure (TODO: remove when done testing as these
//									are now part of DXEffect.h)
//*****************************************************************************
#if 0
typedef std::vector<std::pair<D3D11_SHADER_VARIABLE_DESC, D3D11_SHADER_TYPE_DESC>> ShaderBufferVariableList;
struct ShaderConstantBuffer1
{
	D3D11_SHADER_BUFFER_DESC desc;
	ShaderBufferVariableList variables;
};
#endif	// 0

typedef std::vector<std::pair<std::string, D3D11_SHADER_VARIABLE_DESC>> ShaderCBVariableInfoList;

//*****************************************************************************
// Texture management helper functions
//*****************************************************************************

///<summary>
/// 
/// Does not work with compressed formats.
///</summary>
ID3D11ShaderResourceView* CreateTexture2DArraySRV(
	ID3D11Device* device, ID3D11DeviceContext* context,
	const std::vector<std::wstring>& filenames);/* ,
	DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
	UINT filter = D3DX11_FILTER_NONE,
	UINT mipFilter = D3DX11_FILTER_LINEAR)*/


ShaderResourceViewPtr CreateRandomTexture1DSRV(const DevicePtr &device, const UINT nSize, const float fRangeMin = 0.0f, const float fRangeMax = 1.0f);

// Helper class which is used to toggle an effect's state
class ToggleState
{
public:

	// Param ctor
	// @param strName: Name of the effect.
	// @param iKey: Keyboard key which is pressed to toggle the state.
	// @param bState: Effect state (defaults to false)
	ToggleState(const std::wstring strName, const int iKey, const bool bState = false);

	// Accessor function to get the effect's state
	inline bool getState() const	{ return m_bState; }

	// Mutator function to toggle the state
	void toggleState();

	// Function which return the effect's name and state
	std::wstring dispState();

private:

	// Name of the state
	std::wstring m_strName;

	// Keyboard key which is pressed to toggle the state
	int m_iKey;

	// Effect state (true = on, false = off)
	bool m_bState;

	// Flag to help with toggling the state
	bool m_bToggle;
};
typedef std::unique_ptr<ToggleState> ToggleStatePtr;

#endif // D3DUTIL_H