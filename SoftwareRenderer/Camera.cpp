#include "stdafx.h"
#include "Camera.h"

Camera::Camera() {
	Vector3 vZero = { 0.0f, 0.0f, 0.0f };
	Vector3 vView = { 0.0f, 1.0f, 0.0f };
	Vector3 vUp = { 0.0f, 0.0f, 1.0f };

	position = vZero;
	view = vView;
	upVector = vUp;
}

VOID Camera::PositionCamera(FLOAT positionX, FLOAT positionY, FLOAT positionZ, FLOAT viewX, FLOAT viewY, FLOAT viewZ, FLOAT upVectorX, FLOAT upVectorY, FLOAT upVectorZ) {
	Vector3 vPos	= { positionX, positionY, positionZ };
	Vector3 vView	= { viewX, viewY, viewZ };
	Vector3 vUp		= { upVectorX, upVectorY, upVectorZ };

	position = vPos;
	view = vView;
	upVector = vUp;
}

VOID Camera::MoveCamera(FLOAT speed) {
	Vector3 vec = view - position;
	vec = Vector3::Normal(vec);
	
	position += vec * speed;
	view += vec * speed;
}

VOID Camera::StrafeCamera(FLOAT speed) {
	position -= strafe * speed;
	view -= strafe * speed;
}

VOID Camera::RotateView(FLOAT angle, FLOAT x, FLOAT y, FLOAT z) {
	if(0.0f == angle) {
		return;
	}

	Vector3 vec = view - position;	

	const FLOAT cosTheta = COS(angle);
	const FLOAT sinTheta = SIN(angle);
	const FLOAT invCosTheta = (1 - cosTheta);
	const FLOAT invCosThetaX = invCosTheta * x;
	const FLOAT invCosThetaY = invCosTheta * y;
	const FLOAT invCosThetaZ = invCosTheta * z;
	const FLOAT invCosThetaXY = invCosThetaX * y;
	const FLOAT invCosThetaYZ = invCosThetaY * z;
	const FLOAT invCosThetaZX = invCosThetaZ * x;
	const FLOAT sinThetaX = x * sinTheta;
	const FLOAT sinThetaY = y * sinTheta;
	const FLOAT sinThetaZ = z * sinTheta;
	
	Vector3 newView;

	newView.x  = (cosTheta + invCosThetaX * x) * vec.x;
	newView.x += (invCosThetaXY - sinThetaZ)   * vec.y;
	newView.x += (invCosThetaZX + sinThetaY)   * vec.z;

	newView.y  = (invCosThetaXY + sinThetaZ)   * vec.x;
	newView.y += (cosTheta + invCosThetaY * y) * vec.y;
	newView.y += (invCosThetaYZ - sinThetaX)   * vec.z;

	newView.z  = (invCosThetaZX - sinThetaY)   * vec.x;
	newView.z += (invCosThetaYZ + sinThetaX)   * vec.y;
	newView.z += (cosTheta + invCosThetaZ * z) * vec.z;

	view = position + newView;
}

VOID Camera::RotateAroundPoint(Vector3 center, FLOAT angle, FLOAT x, FLOAT y, FLOAT z) {
	Vector3 newPos;
	Vector3 pos = position - center;

	FLOAT cosTheta = COS(angle);
	FLOAT sinTheta = SIN(angle);
	FLOAT invCosTheta = (1 - cosTheta);
	FLOAT invCosThetaXY = invCosTheta * x * y;
	FLOAT invCosThetaYZ = invCosTheta * y * z;
	FLOAT invCosThetaZX = invCosTheta * z * x;
	FLOAT sinThetaX = x * sinTheta;
	FLOAT sinThetaY = y * sinTheta;
	FLOAT sinThetaZ = z * sinTheta;
	
	newPos.x = (cosTheta + invCosTheta * x * x) * pos.x;
	newPos.x += (invCosThetaXY - sinThetaZ) * pos.y;
	newPos.x += (invCosThetaZX + sinThetaY) * pos.z;

	newPos.y = (invCosThetaXY + sinThetaZ) * pos.x;
	newPos.y += (cosTheta + invCosTheta * y * y) * pos.y;
	newPos.y += (invCosThetaYZ - sinThetaX) * pos.z;

	newPos.z = (invCosThetaZX - sinThetaY) * pos.x;
	newPos.z += (invCosThetaYZ + sinThetaX) * pos.y;
	newPos.z += (cosTheta + invCosTheta * z * z) * pos.z;

	position = center + newPos;
}

VOID Camera::SetViewByMouse() {
	FLOAT maxAngle = 89; //1.55334303f;
	POINT mousePos;
	INT middleX = props.screenWidth  >> 1;
	INT middleY = props.screenHeight >> 1;
	FLOAT angleY = 0.0f;
	FLOAT angleZ = 0.0f;
	static FLOAT currentRotX = 0.0f;
	
	GetCursorPos(&mousePos);						

	if((mousePos.x == middleX) && (mousePos.y == middleY)) {
		return;
	}

	SetCursorPos(middleX, middleY);							

	angleY = (FLOAT)((middleX - mousePos.x)); // 1000.0f;		
	angleZ = (FLOAT)((middleY - mousePos.y)); // 1000.0f;
	
	static float lastRotX = 0.0f; 
 	lastRotX = currentRotX;
	
	currentRotX += angleZ;

	if(currentRotX > maxAngle)  {
		currentRotX = maxAngle;

		if(lastRotX != maxAngle) {
			Vector3 vAxis = Vector3::Cross(view - position, upVector);
			vAxis = Vector3::Normal(vAxis);
				
			RotateView(maxAngle - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	} else if(currentRotX < -maxAngle) {
		currentRotX = -maxAngle;
		
		if(lastRotX != -maxAngle) {
			Vector3 vAxis = Vector3::Cross(view - position, upVector);
			vAxis = Vector3::Normal(vAxis);
			
			RotateView(-maxAngle - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	} else {
		Vector3 vAxis = Vector3::Cross(view - position, upVector);
		vAxis = Vector3::Normal(vAxis);

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	RotateView(angleY, 0, 1, 0);
}

VOID Camera::CheckForMovement() {
	FLOAT mSpeed = kSpeed * moveAdjust;
	FLOAT rSpeed = 30.0f * moveAdjust;
	
	if((GetKeyState(VK_UP) & 0x80) || (GetKeyState('W') & 0x80)) {
		MoveCamera(-mSpeed);
	}

	if((GetKeyState(VK_DOWN) & 0x80) || (GetKeyState('S') & 0x80)) {
		MoveCamera(mSpeed);
	}

	if(GetKeyState(VK_LEFT) & 0x80) {
		RotateView(-rSpeed, 0, 1, 0);	
	}

	if(GetKeyState(VK_RIGHT) & 0x80) {
		RotateView(rSpeed, 0, 1, 0);	
	}

	if(GetKeyState('A') & 0x80) {
		StrafeCamera(-mSpeed);
	}

	if(GetKeyState('D') & 0x80) {
		StrafeCamera(mSpeed);
	}	
}

VOID Camera::Update() {
	Vector3 vCross = Vector3::Cross(view - position, upVector);
	strafe = Vector3::Normal(vCross);

	//SetViewByMouse();
	CheckForMovement();
}

Matrix4x4 Camera::Look() {
	Vector3 z = position - view;
	z = Vector3::Normal(z);
	
	Vector3 x = Vector3::Cross(upVector, z);
	x = Vector3::Normal(x);
	
	Vector3 y = Vector3::Cross(z, x);
    y = Vector3::Normal(y);
    
    FLOAT ex = -Vector3::Dot(x, position);
    FLOAT ey = -Vector3::Dot(y, position);
    FLOAT ez = -Vector3::Dot(z, position);
	
	Matrix4x4 m = {
		x.x, y.x, z.x, 0.0f
		, x.y, y.y, z.y, 0.0f
		, x.z, y.z, z.z, 0.0f
		, ex, ey, ez, 1.0f
	};
    
    return m;
}

Camera g_Camera;