//=======================================================================================
// d3dApp.h by Frank Luna (C) 2008 All Rights Reserved.
//
// Simple Direct3D demo application class.  
// Make sure you link: d3d10.lib d3dx10d.lib dxerr.lib dxguid.lib.
// Link d3dx10.lib for release mode builds instead of d3dx10d.lib.
//=======================================================================================


#ifndef D3DAPP_H
#define D3DAPP_H


#include "d3dUtil.h"
#include "GameTimer.h"
#include <string>


class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE getAppInst();
	HWND      getMainWnd();

	int run();

	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.

	virtual void initApp();
	virtual void onResize();// reset projection/etc
	virtual void updateScene(float dt);
	virtual void drawScene(); 
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:
	void initMainWindow();
	void initDirect3D();
	
protected:

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;

	GameTimer mTimer;

	std::wstring mFrameStats;
 
	DevicePtr    md3dDevice;
	IDXGISwapChain*  mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	//ID3DX10Font* mFont;	// @TODO: remove when done testing
	DeviceContextPtr md3dDeviceContext;
	D3D_FEATURE_LEVEL mFeatureLevel;
	D3D11_VIEWPORT mDefaultViewport;

	// ID3DX10Font has been deprecated as of Win 8.1, DirectXTK's SpriteBatch and SpriteFont classes can serve as a replacement
	// Usage: http://directxtk.codeplex.com/wikipage?title=SpriteFont&referringTitle=DirectXTK
	SpriteBatchPtr mSpriteBatch;
	SpriteFontPtr mFont;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE md3dDriverType;
	DXColor mClearColor;
	int mClientWidth;
	int mClientHeight;

	// some things to help with profiling draw calls
	float mDrawTime, mUpdateTime;
	int mDrawCounter, mUpdateCounter;
};




#endif // D3DAPP_H