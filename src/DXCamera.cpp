// Source file for the DXCamera class - camera class for DirectX

#include "DXCamera.h"

// param ctor 1
DXCamera::DXCamera(const DXVector3 eye, const DXVector3 lookAt, const DXVector3 up,
				const float fov, const float aspectRatio, const float nearDist, const float farDist)
				: m_eye(eye), m_lookAt(lookAt), m_up(up),	// init the view matrix params
				m_fov(fov), m_aspectRatio(aspectRatio), m_nearDist(nearDist), m_farDist(farDist),	// init the proj matrix params
				m_motionFactor(50.0f), m_rotationFactor(1.0f)	// init the motion and rotation factors to hardcoded default values
{

	// display a warning that the eye position and look at point cannot be the same and so,
	// the eye position is being pushed -100 units backward to accommodate this case
	if (m_eye == m_lookAt)
	{
		::MessageBoxA(NULL, "Eye position cannot be the same as the look at point. Pushing the camera 100 units backwards",
							"Camera initialization", MB_ICONWARNING | MB_OK);
		m_eye.z -= 100.0f;
	}

	// hardcode the axes u, v and n if the possibility of gimbal lock exists
	if (m_eye.x == m_lookAt.x && m_eye.z == m_lookAt.z)
	{
		if (m_eye.y < m_lookAt.y)	// look vertically down
		{
			m_u = DXVector3(1, 0, 0);	// pointing right
			m_v = DXVector3(0, 0, -1);	// pointing backwards
			m_n = DXVector3(0, 1, 0);	// pointing up
		}
		else if (m_eye.y > m_lookAt.y)	// look vertically up
		{
			m_u = DXVector3(1, 0, 0);	// pointing right
			m_v = DXVector3(0, 0, 1);	// pointing forwards
			m_n = DXVector3(0, -1, 0);	// pointing down
		}
	}
	else	// compute the axes u, v and n
	{
		m_n = m_lookAt - m_eye;
		m_n.Normalize();
		m_u = m_up.Cross(m_n);
		m_u.Normalize();
		m_v = m_n.Cross(m_u);
	}
	
	updateView();	// update the view matrix
	updateProj();	// update the projection matrix
}

// param ctor 2
DXCamera::DXCamera(const DXVector3 eye, const DXVector3 lookAt, const DXVector3 up,
	const float fov, const float aspectRatio, const float nearDist, const float farDist,
	const float motionFactor, const float rotationFactor)
	: DXCamera(eye, lookAt, up, fov, aspectRatio, nearDist, farDist)	
{
	// init the motion and rotation factors
	m_motionFactor = motionFactor; 
	m_rotationFactor = rotationFactor;
}

// accessor func for the view matrix
DXMatrix DXCamera::view()	{ return m_view;	}

// accessor func for the proj matrix
DXMatrix DXCamera::proj()	{ return m_proj;	}

// accessor func for the eye position in world space
DXVector3 DXCamera::eyePosW()	{ return m_eye; }

// roll (rotate about n) the camera by angle radians
void DXCamera::roll(const float angle)
{
	// rotate u and v about n by angle radians
	rotateAxes(m_n, angle, m_u, m_v);
}

// pitch (rotate about u) the camera by angle radians
void DXCamera::pitch(const float angle)
{
	// rotate v and n about u by angle radians
	rotateAxes(m_u, angle, m_v, m_n);
}

// yaw (rotate about v) the camera by angle radians
void DXCamera::yaw(const float angle)
{
	// rotate u and n about v by angle
	rotateAxes(m_v, angle, m_u, m_n);
	
	// update the view matrix
	updateView();
}

// rotate the camera about the up vector by angle radians (much easier to handle than yaw)
void DXCamera::rotateY(const float angle)
{
	// create a trans matrix to rotate by angle about the up vector
	DXMatrix trans = DXMatrix::CreateFromAxisAngle(m_up, angle);
	//D3DXMatrixRotationAxis(&trans, &rotAxis, angle);	// @TODO: remove when done testing

	// rotate m, v and n using trans
	m_u = DXVector3::Transform(m_u, trans);
	m_v = DXVector3::Transform(m_v, trans);
	m_n = DXVector3::Transform(m_n, trans);

	// update the view matrix
	updateView();
}

// slide the camera by du, dv and dn along the u, v and n axes respectively
void DXCamera::slide(const float du, const float dv, const float dn)
{
	// CORRECT APPROACH
	// This causes the camera to slide along its u, v and n axes
#if 1
	m_eye.x += du*m_u.x + dv*m_v.x + dn*m_n.x;
	m_eye.y += du*m_u.y + dv*m_v.y + dn*m_n.y;
	m_eye.z += du*m_u.z + dv*m_v.z + dn*m_n.z;
#endif	// 1

	// WRONG APPROACH
	// This causes the camera to slide along the fundamental x (1, 0, 0), y (0, 1, 0) and z (0, 0, 1) axes
	// instead of along its u, v, and n axes.
	// This breaks down rather quickly once the camera has been moved around sufficiently.
#if 0
	m_eye += DXVector3(du, dv, dn);
#endif	// 0

	updateView();
}

// mutator func to set the aspect ratio (used when the window is resized)
void DXCamera::setAspectRatio(const float aspectRatio)
{
	// update the aspect ratio and the proj matrix
	m_aspectRatio = aspectRatio;
	updateProj();
}

// updates the view matrix using eye, lookAt and up
void DXCamera::updateView()
{
	// build the view matrix by hand
	// the camera matrix is defined (in the DirectX documentation) as:
	// note how this is the transpose of the OpenGL camera matrix, must find out why
	// |ux	vx	nx	0|	dx = -eye.u
	// |uy	vy	ny	0|	dy = -eye.v
	// |uz	vz	nz	0|	dz = -eye.n
	// |dx	dy	dz	1|	
	//DXVector3 d(-D3DXVec3Dot(&m_eye, &m_u), -D3DXVec3Dot(&m_eye, &m_v), -D3DXVec3Dot(&m_eye, &m_n));	// @TODO: remove when done testing
	DXVector3 d(-m_eye.Dot(m_u), -m_eye.Dot(m_v), -m_eye.Dot(m_n));
	m_view(0, 0) = m_u.x;	m_view(0, 1) = m_v.x;	m_view(0, 2) = m_n.x;	m_view(0, 3) = 0;
	m_view(1, 0) = m_u.y;	m_view(1, 1) = m_v.y;	m_view(1, 2) = m_n.y;	m_view(1, 3) = 0;
	m_view(2, 0) = m_u.z;	m_view(2, 1) = m_v.z;	m_view(2, 2) = m_n.z;	m_view(2, 3) = 0;
	m_view(3, 0) = d.x;		m_view(3, 1) = d.y;		m_view(3, 2) = d.z;		m_view(3, 3) = 1;
}

// updates the projection matrix using fov, aspectRatio, nearDist and farDist
void DXCamera::updateProj()
{
	// build the projection matrix using D3DXPerspectiveFovLH
	// @TODO: build the projection matrix by hand as we did in OpenGLCamera
	//D3DXMatrixPerspectiveFovLH(&m_proj, m_fov, m_aspectRatio, m_nearDist, m_farDist);

	// define the left, right, bottom and top corners of the projection window
	const float top = m_nearDist * float(tan( m_fov/2 /** PI / 180.0*/ ));	// no need to convert to radians because fov is already in radians
	const float bottom = -top;
	const float left = bottom * m_aspectRatio;
	const float right = -left;

	// the perspective projection matrix is defined as follows
	// transposed from original OpenGL version with some additional modifications for use in Direct3D
	// emulates D3DXMatrixPerspectiveOffCenterLH
	// also note that pseudo-depth is computed differently than it is in OpenGL
	// |		2N/(right-left)						0					0			0 |	N = m_nearDist
	// |				0					2N/(top-bottom)				0			0 | F = m_farDist
	// |	(left+right)/(left-right)	(top+bottom)/(bottom-top)	(F)/(F-N)		1 |
	// |				0							0				FN/(N-F)		0 |
	m_proj(0, 0) = 2*m_nearDist/(right-left);	m_proj(0, 1) = 0;							m_proj(0, 2) = 0;												m_proj(0, 3) = 0;
	m_proj(1, 0) = 0;							m_proj(1, 1) = 2*m_nearDist/(top-bottom);	m_proj(1, 2) = 0;												m_proj(1, 3) = 0;
	m_proj(2, 0) = (left+right)/(left-right);	m_proj(2, 1) = (top+bottom)/(bottom-top);	m_proj(2, 2) = (m_farDist)/(m_farDist-m_nearDist);				m_proj(2, 3) = 1;
	m_proj(3, 0) = 0;							m_proj(3, 1) = 0;							m_proj(3, 2) = m_farDist*m_nearDist/(m_nearDist-m_farDist);		m_proj(3, 3) = 0;

	// OpenGL version
#if 0
	// the perspective projection matrix is defined as follows
	// transposed from original OpenGL version for use in Direct3D
	// |		2N/(right-left)						0					0			0 |	N = m_nearDist
	// |				0					2N/(top-bottom)				0			0 | F = m_farDist
	// |	(right+left)/(right-left)	(top+bottom)/(top-bottom)	-(F+N)/(F-N)	-1|
	// |				0							0				-2FN/(F-N)		0 |
	m_proj(0, 0) = 2*m_nearDist/(right-left);	m_proj(0, 1) = 0;							m_proj(0, 2) = 0;												m_proj(0, 3) = 0;
	m_proj(1, 0) = 0;							m_proj(1, 1) = 2*m_nearDist/(top-bottom);	m_proj(1, 2) = 0;												m_proj(1, 3) = 0;
	m_proj(2, 0) = (right+left)/(right-left);	m_proj(2, 1) = (top+bottom)/(top-bottom);	m_proj(2, 2) = -(m_farDist+m_nearDist)/(m_farDist-m_nearDist);	m_proj(2, 3) = -1;
	m_proj(3, 0) = 0;							m_proj(3, 1) = 0;							m_proj(3, 2) = -2*m_farDist*m_nearDist/(m_farDist-m_nearDist);	m_proj(3, 3) = 0;
#endif	// 0
}

// helper function to rotate 2 axes around a given axes by a given angle
void DXCamera::rotateAxes(const DXVector3 rotAxis, const float angle, DXVector3 &axis1, DXVector3 &axis2)
{
	// create a trans matrix to rotate by angle about rotAxis
	DXMatrix trans = DXMatrix::CreateFromAxisAngle(rotAxis, angle);
	//D3DXMatrixRotationAxis(&trans, &rotAxis, angle);	// @TODO: remove when done testing

	// rotate axis1 and axis2 using trans
	axis1 = DXVector3::Transform(axis1, trans);
	axis2 = DXVector3::Transform(axis2, trans);
	/*D3DXVec3TransformCoord(&axis1, &axis1, &trans);
	D3DXVec3TransformCoord(&axis2, &axis2, &trans);*/

	// update the view matrix
	updateView();
}


// mutator func to zoom out (increase the fov)
void DXCamera::zoomOut()
{
	// max possible fov (hardcoding to 45 degress (PI/4 radians) for now, shouldn't really have to change though)
	static const float maxFov(PI / 4);
	static const float inc(PI / 1800);

	// increase the field-of-view of the camera to simulate a zoom out effect but ensure that its doesn't cross maxFov
	m_fov = (m_fov + inc > maxFov) ? maxFov : m_fov + inc;

	// update the projection matrix
	updateProj();
}

// mutator func to zoom in (decrease the fov)
void DXCamera::zoomIn()
{
	// min possible fov (hardcoding to 10 degrees for now)
	static const float minFov(10 * PI / 180.0f);
	static const float dec(-PI / 1800);

	// decrease the field-of-view of the camera to simulate a zoom in effect but ensure it doesn't cross minFov
	m_fov = (m_fov + dec < minFov) ? minFov : m_fov + dec;

	// update the projection matrix
	updateProj();
}