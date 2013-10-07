#ifndef SOFTWARERENDERER_CAMERA_H
#define SOFTWARERENDERER_CAMERA_H

class Camera {
public:
	Camera();

	Vector3 Position() { return position; }
	Vector3 View()		{ return view; }
	Vector3 UpVector() { return upVector; }
	Vector3 Strafe()	{ return strafe; }

	VOID PositionCamera(
		FLOAT positionX, FLOAT positionY, FLOAT positionZ,
		FLOAT viewX,     FLOAT viewY,     FLOAT viewZ,
		FLOAT upVectorX, FLOAT upVectorY, FLOAT upVectorZ
	);
	VOID MoveCamera(FLOAT speed);
	VOID StrafeCamera(FLOAT speed);
	VOID RotateView(FLOAT angle, FLOAT x, FLOAT y, FLOAT z);
	VOID RotateAroundPoint(Vector3 center, FLOAT angle, FLOAT X, FLOAT Y, FLOAT Z);

	VOID SetViewByMouse();
	VOID CheckForMovement();

	VOID Look();
	VOID Update();

	~Camera() {};
private:
	Vector3 position;
	Vector3 view;
	Vector3 upVector;
	Vector3 strafe;
};

extern Camera g_Camera;

#endif