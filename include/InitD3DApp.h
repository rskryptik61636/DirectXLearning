// test d3d app - header file

#ifndef INIT_D3D_APP_H
#define INIT_D3D_APP_H

#include "d3dApp.h"

class InitD3DApp : public D3DApp
{
public:
	InitD3DApp(HINSTANCE hInstance);
	~InitD3DApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene();
};

#endif	//INIT_D3D_APP_H