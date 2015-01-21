#ifndef TEXTURE_BOX_H
#define TEXTURE_BOX_H

//#include "Object.h"
#include "ObjectV2.h"

class TextureBox : public ObjectV2<TextureVertex>
{
public:
	TextureBox();
	virtual ~TextureBox();

	void initVertexBuffer();
	void initIndexBuffer();

	// variant of draw which sets the diffuse and specular maps in the pixel shader before drawing the box
	void drawBox(const UINT diffuseMapBindPoint, const ShaderResourceViewPtr &diffuseMapRV,
		const UINT specMapBindPoint, const ShaderResourceViewPtr &specMapRV);
};
typedef std::unique_ptr<TextureBox> TextureBoxPtr;	// smart ptr for TextureBox instances

#endif	// TEXTURE_BOX_H