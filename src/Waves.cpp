//=======================================================================================
// Waves.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Waves.h"
#include "Vertex.h"
#include <algorithm>
#include <vector>

Waves::Waves()
: mNumRows(0), mNumCols(0), mNumVertices(0), mNumFaces(0), 
  mK1(0.0f), mK2(0.0f), mK3(0.0f), mTimeStep(0.0f), mSpatialStep(0.0f),
  mPrevSolution(0), mCurrSolution(0), mNormals(0), md3dDevice(0), mVB(0), mIB(0)
{
}

Waves::~Waves()
{
	delete[] mPrevSolution;
	delete[] mCurrSolution;
	delete[] mNormals;

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Waves::init(ID3D11Device* device, DWORD m, DWORD n, float dx, float dt, float speed, float damping)
{
	md3dDevice = device;

	mNumRows  = m;
	mNumCols  = n;

	mNumVertices = m*n;
	mNumFaces    = (m-1)*(n-1)*2;

	mTimeStep    = dt;
	mSpatialStep = dx;

	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	mK1     = (damping*dt-2.0f)/ d;
	mK2     = (4.0f-8.0f*e) / d;
	mK3     = (2.0f*e) / d;

	mPrevSolution = new DXVector3[m*n];
	mCurrSolution = new DXVector3[m*n];
	mNormals      = new DXVector3[m*n];

	// Generate grid vertices in system memory.

	float halfWidth = (n-1)*dx*0.5f;
	float halfDepth = (m-1)*dx*0.5f;
	for(DWORD i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for(DWORD j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			mPrevSolution[i*n+j] = DXVector3(x, 0.0f, z);
			mCurrSolution[i*n+j] = DXVector3(x, 0.0f, z);
			mNormals[i*n+j]      = DXVector3(0.0f, 1.0f, 0.0f);
		}
	}
 
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(LightVertex) * mNumVertices;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    HR(md3dDevice->CreateBuffer(&vbd, 0, &mVB));


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<DWORD> indices(mNumFaces*3); // 3 indices per face

	// Iterate over each quad.
	int k = 0;
	for(DWORD i = 0; i < m-1; ++i)
	{
		for(DWORD j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Waves::update(float dt)
{
	static float t = 0;

	// Accumulate time.
	t += dt;

	// Only update the simulation at the specified time step.
	if( t >= mTimeStep )
	{
		// Only update interior points; we use zero boundary conditions.
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
				// keep consistent with our row indices going down.

				mPrevSolution[i*mNumCols+j].y = 
					mK1*mPrevSolution[i*mNumCols+j].y +
					mK2*mCurrSolution[i*mNumCols+j].y +
					mK3*(mCurrSolution[(i+1)*mNumCols+j].y + 
					     mCurrSolution[(i-1)*mNumCols+j].y + 
					     mCurrSolution[i*mNumCols+j+1].y + 
						 mCurrSolution[i*mNumCols+j-1].y);
			}
		}

		// We just overwrite the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f; // reset time

		// Compute normals using finite difference scheme.
		for(DWORD i = 1; i < mNumRows-1; ++i)
		{
			for(DWORD j = 1; j < mNumCols-1; ++j)
			{
				float l = mCurrSolution[i*mNumCols+j-1].y;
				float r = mCurrSolution[i*mNumCols+j+1].y;
				float t = mCurrSolution[(i-1)*mNumCols+j-1].y;
				float b = mCurrSolution[(i+1)*mNumCols+j+1].y;
				mNormals[i*mNumCols+j].x = -r+l;
				mNormals[i*mNumCols+j].y = 2*mSpatialStep;
				mNormals[i*mNumCols+j].z = b-t;
				mNormals[i*mNumCols+j].Normalize();
				//D3DXVec3Normalize(&mNormals[i*mNumCols+j], &mNormals[i*mNumCols+j]);	// @TODO: remove when done testing
			}
		}


		// Update the vertex buffer with the new solution.
		//LightVertex* v = 0;												// @TODO: remove when done testing
		ID3D11DeviceContext *deviceContext;
		md3dDevice->GetImmediateContext(&deviceContext);	// get the device's immediate rendering context
		D3D11_MAPPED_SUBRESOURCE vData;
		ZeroMemory(&vData, sizeof(D3D11_MAPPED_SUBRESOURCE));			// create a mapped subresource
		HR(deviceContext->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vData));	// map the vertex buffer's data to vData
		LightVertex *v = reinterpret_cast<LightVertex*>(vData.pData);	// get the pointer to the vertex data
		//HR(mVB->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&v ));	// @TODO: remove when done testing

		for(DWORD i = 0; i < mNumVertices; ++i)
		{
			v[i].position     = mCurrSolution[i];
			v[i].diffuse = BLUE;
			v[i].spec    = DXColor(1.0f, 1.0f, 1.0f, 128.0f);
			v[i].normal  = mNormals[i];
		}

		deviceContext->Unmap(mVB, 0);	// unmap the vertex buffer data and reallow access by the GPU
		deviceContext->Release();		// release the immediate rendering context
		//mVB->Unmap();	// @TODO: remove when done testing
	}
}

void Waves::disturb(DWORD i, DWORD j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < mNumRows-2);
	assert(j > 1 && j < mNumCols-2);

	float halfMag = 0.5f*magnitude;

	mCurrSolution[i*mNumCols+j].y     += magnitude;
	mCurrSolution[i*mNumCols+j+1].y   += halfMag;
	mCurrSolution[i*mNumCols+j-1].y   += halfMag;
	mCurrSolution[(i+1)*mNumCols+j].y += halfMag;
	mCurrSolution[(i-1)*mNumCols+j].y += halfMag;
}
	
void Waves::draw()
{
	UINT stride = sizeof(LightVertex);
	UINT offset = 0;
	ID3D11DeviceContext *pDeviceContext;
	md3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(mNumFaces * 3, 0, 0);
	pDeviceContext->Release();
}