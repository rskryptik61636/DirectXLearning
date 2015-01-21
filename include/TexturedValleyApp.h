#ifndef TEXTURED_VALLEY_APP_H
#define TEXTURED_VALLEY_APP_H

#include "GenericApp.h"

class TexturedValleyApp : public GenericApp
{
public:
	TexturedValleyApp(HINSTANCE hInstance);
	TexturedValleyApp(HINSTANCE hInstance, std::wstring shaderFilename);
	virtual ~TexturedValleyApp();

protected:
	virtual void createResources();
	virtual void drawObjects();
};

#endif	// TEXTURED_VALLEY_APP_H