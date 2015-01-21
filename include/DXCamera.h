// Header file for the DXCamera class - camera class for DirectX
#ifndef DX_CAMERA_H
#define DX_CAMERA_H

#include "d3dUtil.h"
#include <memory>

class DXCamera
{
public:
	// param ctor 1
	explicit DXCamera(const DXVector3 eye, const DXVector3 lookAt, const DXVector3 up,
					const float fov, const float aspectRatio, const float nearDist, const float farDist);

	// param ctor 2
	explicit DXCamera(const DXVector3 eye, const DXVector3 lookAt, const DXVector3 up,
		const float fov, const float aspectRatio, const float nearDist, const float farDist,
		const float motionFactor, const float rotationFactor);

	// accessor func for the view matrix
	DXMatrix view();

	// accessor func for the proj matrix
	DXMatrix proj();

	// accessor func for the eye position in world space
	DXVector3 eyePosW();

	// accessor func for the motion factor
	inline const float motionFactor()	const	{ return m_motionFactor; }

	// accessor func for the rotation factor
	inline const float rotationFactor()	const	{ return m_rotationFactor; }

	// roll (rotate about n) the camera by angle radians
	void roll(const float angle);

	// pitch (rotate about u) the camera by angle radians
	void pitch(const float angle);

	// yaw (rotate about v) the camera by angle radians
	void yaw(const float angle);

	// rotate the camera about the up vector by angle radians (much easier to handle than yaw)
	void rotateY(const float angle);

	// slide the camera by du, dv and dn along the u, v and n axes respectively
	void slide(const float du, const float dv, const float dn);

	// mutator func to set the aspect ratio (used when the window is resized)
	void setAspectRatio(const float aspectRatio);

	// mutator func to zoom out (increase the fov)
	void zoomOut();

	// mutator func to zoom in (decrease the fov)
	void zoomIn();

private:

	// updates the view matrix using eye, lookAt and up
	void updateView();

	// updates the projection matrix using fov, aspectRatio, nearDist and farDist
	void updateProj();

	// helper function to rotate 2 axes around a given axes by a given angle
	void rotateAxes(const DXVector3 rotAxis, const float angle, DXVector3 &axis1, DXVector3 &axis2);

	// projection matrix params
	float m_fov, m_aspectRatio, m_nearDist, m_farDist;

	// view matrix params
	DXVector3 m_eye, m_lookAt, m_up;

	// camera basis vectors
	DXVector3 m_u, m_v, m_n;

	// view and projection matrices
	DXMatrix m_view, m_proj;

	// motion and rotation factors
	float m_motionFactor, m_rotationFactor;

};	// end of class DXCamera
typedef std::unique_ptr<DXCamera> DXCameraPtr;

#endif	// DX_CAMERA_H