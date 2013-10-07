#include "stdafx.h"

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
	Vector3 vUp	= { upVectorX, upVectorY, upVectorZ };

	position = vPos;
	view = vView;
	upVector = vUp;
}

VOID Camera::MoveCamera(FLOAT speed) {
	Vector3 vec = view - position;
	vec = Vector3::Normal(vec);

	position.x += vec.x * speed;
	position.z += vec.z * speed;
	
	view.x += vec.x * speed;
	view.z += vec.z * speed;
}

VOID Camera::StrafeCamera(FLOAT speed) {
	position.x -= strafe.x * speed;
	position.z -= strafe.z * speed;

	view.x -= strafe.x * speed;
	view.z -= strafe.z * speed;
}

VOID Camera::RotateView(FLOAT angle, FLOAT x, FLOAT y, FLOAT z) {
	Vector3 newView;
	Vector3 vec;	

	vec.x = view.x - position.x;	
	vec.y = view.y - position.y;
	vec.z = view.z - position.z;

	FLOAT cosTheta = COS(angle);
	FLOAT sinTheta = SIN(angle);
	FLOAT invCosTheta = (1 - cosTheta);
	FLOAT invCosThetaXY = invCosTheta * x * y;
	FLOAT invCosThetaYZ = invCosTheta * y * z;
	FLOAT invCosThetaZX = invCosTheta * z * x;
	FLOAT sinThetaX = x * sinTheta;
	FLOAT sinThetaY = y * sinTheta;
	FLOAT sinThetaZ = z * sinTheta;

	newView.x  = (cosTheta + invCosTheta * x * x) * vec.x;
	newView.x += (invCosThetaXY - sinThetaZ) * vec.y;
	newView.x += (invCosThetaZX + sinThetaY) * vec.z;

	newView.y  = (invCosThetaXY + sinThetaZ) * vec.x;
	newView.y += (cosTheta + invCosTheta * y * y) * vec.y;
	newView.y += (invCosThetaYZ - sinThetaX) * vec.z;

	newView.z  = (invCosThetaZX - sinThetaY) * vec.x;
	newView.z += (invCosThetaYZ + sinThetaX) * vec.y;
	newView.z += (cosTheta + invCosTheta * z * z) * vec.z;

	view.x = position.x + newView.x;
	view.y = position.y + newView.y;
	view.z = position.z + newView.z;
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

	angleY = (FLOAT)((middleX - mousePos.x)) / 1000.0f;		
	angleZ = (FLOAT)((middleY - mousePos.y)) / 1000.0f;
	
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
	if((GetKeyState(VK_UP) & 0x80) || (GetKeyState('W') & 0x80)) {
		MoveCamera(kSpeed);
	}

	if((GetKeyState(VK_DOWN) & 0x80) || (GetKeyState('S') & 0x80)) {
		MoveCamera(-kSpeed);
	}

	if(GetKeyState(VK_LEFT) & 0x80) {
		RotateView(30.0f, 0, 1, 0);	
	}

	if(GetKeyState(VK_RIGHT) & 0x80) {
		RotateView(-30.0f, 0, 1, 0);	
	}

	if(GetKeyState('A') & 0x80) {
		StrafeCamera(-kSpeed);
	}

	if(GetKeyState('D') & 0x80) {
		StrafeCamera(kSpeed);
	}	
}

VOID Camera::Update() {
	Vector3 vCross = Vector3::Cross(view - position, upVector);
	strafe = Vector3::Normal(vCross);

	//SetViewByMouse();
	CheckForMovement();
}

VOID Camera::Look() {
	//gluLookAt(position.x, position.y, position.z,	
	//		  view.x,	 view.y,     view.z,	
	//		  upVector.x, upVector.y, upVector.z);
}

Camera g_Camera;