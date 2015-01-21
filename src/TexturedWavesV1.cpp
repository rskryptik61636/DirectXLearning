// source file for TexturedWavesV1 which extends ObjectV2

#include "TexturedWavesV1.h"

/*#ifdef _DEBUG
	#pragma comment(lib, "DXEngined.lib")
#else
	#pragma comment(lib, "DXEngine.lib")
#endif	// _DEBUG*/

TexturedWavesV1::TexturedWavesV1() : ObjectV2(), mNumRows(0), mNumCols(0), 
	mK1(0.0f), mK2(0.0f), mK3(0.0f), mTimeStep(0.0f), mSpatialStep(0.0f),
	mPrevSolution(0), mCurrSolution(0), mNormals(0)
{
}

TexturedWavesV1::~TexturedWavesV1()
{
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;
}

void TexturedWavesV1::init(ID3D11Device *device, DWORD m, DWORD n,
					float dx, float dt, float speed, float damping)
{
	mD3dDevice = device;	// initialize the D3D device

	// intialize no. of rows and no. of cols
	mNumRows = m;
	mNumCols = n;

	// initialize no. of vertices and no. of faces
	mNumVertices = m*n;
	mNumFaces = (m-1)*(n-1)*2;
	mNumVerticesPerFace = 3;

	// initialize time step and spatial step
	mTimeStep = dt;
	mSpatialStep = dx;

	// set waves modulation factors
	float d = damping*dt + 2.0f;
	float e = (speed*speed) * (dt*dt)/(dx*dx);
	mK1 = (damping*dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f*e) / d;
	mK3 = (2.0f * e) / d;

	// initialize solutions and waves normals
	mPrevSolution = new DXVector3[m*n];
	mCurrSolution = new DXVector3[m*n];
	mNormals = new DXVector3[m*n];

	// generate grid vertices
	float halfWidth = (mNumCols-1) * dx * 0.5f;
	float halfDepth = (mNumRows-1) * dx * 0.5f;
	for(DWORD i = 0; i < mNumRows; ++i)
	{
		float z = halfDepth - i*dx;
		for(DWORD j = 0; j < mNumCols; ++j)
		{
			float x = -halfWidth + j*dx;
			mPrevSolution[i*n+j] = DXVector3(x,0.0f,z);
			mCurrSolution[i*n+j] = DXVector3(x,0.0f,z);
			mNormals[i*n+j] = DXVector3(0.0f,1.0f,0.0f);
		}
	}

	// allocate space for the vertex buffer, no behaviour is defined as it will be constantly updated
	// -while the app is running
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(TextureVertex) * mNumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	HR(mD3dDevice->CreateBuffer(&vertexBufferDesc, 0, &mVb));

	// create the index buffer, it is not going to be modified on the fly
	mIndexList.resize(mNumFaces * mNumVerticesPerFace);

	// Iterate over each quad.
	int k = 0;
	for(DWORD i = 0; i < m-1; ++i)
	{
		for(DWORD j = 0; j < n-1; ++j)
		{
			mIndexList[k]   = i*n+j;
			mIndexList[k+1] = i*n+j+1;
			mIndexList[k+2] = (i+1)*n+j;

			mIndexList[k+3] = (i+1)*n+j;
			mIndexList[k+4] = i*n+j+1;
			mIndexList[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = mIndexList.size() * sizeof(DWORD);
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA indexBufferSubresourceData;
	indexBufferSubresourceData.pSysMem = &mIndexList[0];
	HR(mD3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferSubresourceData, &mIb));
}

void TexturedWavesV1::update(float dt)
{
	static float t = 0;

	// accumulate time
	t += dt;

	if(t >= mTimeStep)
	{
		// only update boundary points, zero boundary conditions
		for(DWORD i = 1; i < mNumRows-1; ++i)
		{
			for(DWORD j = 1; j < mNumCols-1; ++j)
			{
				// After this update we will be discarding the old previous
				// buffer, so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element) 
				// because we won't need prev_ij again and the assignment happens last.

				// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				// Moreover, out +z axis goes "down"; this is just to 
				// keep consistent with our row mIndexList going down.
				mPrevSolution[i*mNumCols+j].y = mK1 * mPrevSolution[i*mNumCols+j].y + 
												mK2 * mPrevSolution[i*mNumCols+j].y + 
												mK3 * (mCurrSolution[(i+1)*mNumCols+j].y + 
														mCurrSolution[(i-1)*mNumCols+j].y + 
														mCurrSolution[i*mNumCols+j+1].y + 
														mCurrSolution[i*mNumCols+j-1].y);
			}
		}
	}

	// We just overwrite the previous buffer with the new data, so
	// this data needs to become the current solution and the old
	// current solution becomes the new previous solution.
	std::swap(mPrevSolution,mCurrSolution);

	t = 0.0f;	// reset time

	// compute normals using finite difference scheme
	for(DWORD i = 1; i < mNumRows-1; ++i)
	{
		for(DWORD j = 1; j < mNumCols-1; ++j)
		{
			// @TODO: Double check formula.  Find two tangent vectors, one in +x and one in -z
			// direction.  Then take cross product to get normal.  Use finite difference to
			// compute tangents.  
			float l = mCurrSolution[i*mNumCols+j-1].y;
			float r = mCurrSolution[i*mNumCols+j+1].y;
			float t = mCurrSolution[(i-1)*mNumCols+j].y;
			float b = mCurrSolution[(i+1)*mNumCols+j].y;
			mNormals[i*mNumCols+j].x = -r+l;
			mNormals[i*mNumCols+j].y = 2*mSpatialStep;
			mNormals[i*mNumCols+j].z = b-t;
			mNormals[i*mNumCols + j].Normalize();
			//D3DXVec3Normalize(&mNormals[i*mNumCols+j],&mNormals[i*mNumCols+j]);	// @TODO: remove when done testing
		}
	}

	// update the vertex buffer with the new solution
	float width = (mNumCols-1) * mSpatialStep;
	float height = (mNumRows-1) * mSpatialStep;

	float du = 1.0f / (mNumCols-1);
	float dv = 1.0f / (mNumRows-1);

	// map the vertex buffer to a temporary buffer so that it can be updated
	ID3D11DeviceContext *deviceContext;
	mD3dDevice->GetImmediateContext(&deviceContext);	// get the device's immediate rendering context
	D3D11_MAPPED_SUBRESOURCE vData;
	ZeroMemory(&vData, sizeof(D3D11_MAPPED_SUBRESOURCE));			// create a mapped subresource
	HR(deviceContext->Map(mVb, 0, D3D11_MAP_WRITE_DISCARD, 0, &vData));	// map the vertex buffer's data to vData
	TextureVertex *tmpVertexBuffer = reinterpret_cast<TextureVertex*>(vData.pData);	// get the pointer to the vertex data
	//TextureVertex *tmpVertexBuffer = NULL;
	//HR(mVb->Map(D3D11_MAP_WRITE_DISCARD, 0, reinterpret_cast<void**>(&tmpVertexBuffer)));	// @TODO: remove when done testing

	// update the vertex buffer with the new solution, normals and tex coords
	for(DWORD i = 0; i < mNumRows-1; ++i)
	{
		for(DWORD j = 0; j < mNumCols-1; ++j)
		{
			tmpVertexBuffer[i*mNumCols+j].position = mCurrSolution[i*mNumCols+j];
			tmpVertexBuffer[i*mNumCols+j].normal = mNormals[i*mNumCols+j];
			tmpVertexBuffer[i*mNumCols+j].texC.x = j * du;
			tmpVertexBuffer[i*mNumCols+j].texC.y = i * dv;
		}
	}

	// commit the changes to the vertex buffer
	deviceContext->Unmap(mVb, 0);	// unmap the vertex buffer data and reallow access by the GPU
	deviceContext->Release();		// release the immediate rendering context
	//mVB->Unmap();	// @TODO: remove when done testing
}

void TexturedWavesV1::disturb(DWORD i, DWORD j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < mNumRows-2);
	assert(j > 1 && j < mNumCols-2);
	//if(i > 1 && i < mNumRows-2 || j > 1 && j < mNumCols-2)
	//	return;
	
	float halfMag = 0.5f * magnitude;
	mCurrSolution[i*mNumCols+j].y		+=	magnitude;
	mCurrSolution[i*mNumCols+j+1].y		+=	halfMag;
	mCurrSolution[i*mNumCols+j-1].y		+=	halfMag;
	mCurrSolution[(i+1)*mNumCols+j].y	+=	halfMag;
	mCurrSolution[(i-1)*mNumCols+j].y	+=	halfMag;
}