#include "d3dUtil.h"

//*****************************************************************************
// Texture management helper functions
//*****************************************************************************

///<summary>
/// 
/// Does not work with compressed formats.
///</summary>
static ID3D11ShaderResourceView* CreateTexture2DArraySRV(
	ID3D11Device* device, ID3D11DeviceContext* context,
	const std::vector<std::wstring>& filenames)/* ,
											   DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
											   UINT filter = D3DX11_FILTER_NONE,
											   UINT mipFilter = D3DX11_FILTER_LINEAR)*/
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//

	UINT size = filenames.size();

	std::vector<Texture2DPtr> srcTex(size);

	for (UINT i = 0; i < size; ++i)
	{
		// modified to work without D3DX
		const wpath currFile(filenames[i]);

		// @TODO: see if we can get away with the cast to ID3D11Resource**
		//	      should be alright since it is an ID3D11Texture2D interface internally
		//		  which is derived from ID3D11Resource
		if (currFile.extension() == L".dds")
		{
			HR(DirectX::CreateDDSTextureFromFile(device, currFile.file_string().c_str(), reinterpret_cast<ID3D11Resource**>(&srcTex[i].p), NULL));
		}
		else
		{
			HR(DirectX::CreateWICTextureFromFile(device, context, currFile.file_string().c_str(), reinterpret_cast<ID3D11Resource**>(&srcTex[i].p), NULL));
		}


#if 0

		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		HR(D3DX11CreateTextureFromFile(device, filenames[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
#endif	// 0
	}

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	/*ID3D11Texture2D**/Texture2DPtr texArray = 0;
	HR(device->CreateTexture2D(&texArrayDesc, 0, &texArray.p));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(srcTex[texElement], mipLevel);
		}
	}

	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	//
	// Cleanup--we only need the resource view.
	//

	texArray.Release();
	//ReleaseCOM(texArray);

	for (UINT i = 0; i < size; ++i)
		srcTex[i].Release();
	//ReleaseCOM(srcTex[i]);

	return texArraySRV;
}

ShaderResourceViewPtr CreateRandomTexture1DSRV(const DevicePtr &device, const UINT nSize, const float fRangeMin /*= 0.0f*/, const float fRangeMax /*= 1.0f*/)
{
	// 
	// Create the random data.
	//
	std::vector<DXVector4> randomValues(nSize);
	//DXVector4 randomValues[nSize];

	// Use a constant seed for consistency
	std::mt19937 rng(1337);

	std::uniform_real<float> randomNum(fRangeMin, fRangeMax);

	for (int i = 0; i < nSize; ++i)
	{
		randomValues[i].x = randomNum(rng); //MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = randomNum(rng); //MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = randomNum(rng); //MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = randomNum(rng); //MathHelper::RandF(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues.data();
	initData.SysMemPitch = nSize * sizeof(DXVector4);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = nSize;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	Texture1DPtr randomTex = 0;
	HR(device->CreateTexture1D(&texDesc, &initData, &randomTex.p));

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ShaderResourceViewPtr randomTexSRV = 0;
	HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV.p));

	randomTex.Release();	

	return randomTexSRV;
}

//*****************************************************************************
// Filesystem utility functions.
//*****************************************************************************
wpath narrowToWidePath(const npath &src)
{
	const std::string tmp(src.file_string());
	return wpath(std::wstring(tmp.begin(), tmp.end()));
}

npath wideToNarrowPath(const wpath &src)
{
	const std::wstring tmp(src.file_string());
	return npath(std::string(tmp.begin(), tmp.end()));
}

// Param ctor
// @param strName: Name of the effect.
// @param iKey: Keyboard key which is pressed to toggle the state.
// @param bState: Effect state (defaults to false)
ToggleState::ToggleState(const std::wstring strName, const int iKey, const bool bState /*= false*/)
: m_strName(strName), m_iKey(iKey), m_bState(bState)
{}

// Mutator function to toggle the state
void ToggleState::toggleState()
{
	// Toggle the effect on pressing the N key
	if ((GetAsyncKeyState(m_iKey) & 0x8000) && !m_bToggle)	{
		m_bState = !m_bState;
		m_bToggle = true;
	}
	else if (!(GetAsyncKeyState(m_iKey) & 0x8000))
		m_bToggle = false;
}

// Function which return the effect's name and state
std::wstring ToggleState::dispState()	{ return std::wstring(m_strName + L": " + (getState() ? L"on" : L"off")); }