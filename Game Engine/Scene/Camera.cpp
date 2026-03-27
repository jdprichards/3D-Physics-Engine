//==========================
// Encapsulating a Camera
//==========================
// Holds the transformation matrix for camera position / rotation, near / far clip and field of view
// Can use these to create view and projection matrices as required for rendering
// Transformation can be manpulated in a similar way to any modes
// Also supports functions for 3D->2D  "picking" (world coordinates <-> pixel position)

#include "Camera.h"

#include <cmath>

// Control the Camera's Positiom and Rotation using Keys provided
void Camera::Control(float frameTime,
	KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
	KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight,
	float movementSpeed, float rotationSpeed)
{
	// Mouse smoothing
	const float cameraSmoothing = 0.15f; // Mouse smoothign strength (0->1)
	const float pixelToRadians = 0.005f; // Converting pixels of mouse movement to rotations in radians
	static Vector2i mouse = GetMouse();
	static Vector2f mouseMove = { 0,0 };

	Vector2i newMouse = GetMouse();
	mouseMove.x += (newMouse - mouse).x;
	mouseMove.y += (newMouse - mouse).y;
	mouse = newMouse;

	Vector2f mouseDelta;
	mouseDelta.x = mouseMove.x * frameTime / (0.1f + cameraSmoothing * 0.25f);
	mouseDelta.y = mouseMove.y * frameTime / (0.1f + cameraSmoothing * 0.25f);
	mouseMove -= mouseDelta;

	auto tempPosition = mTransform.Position(); // Store positions before doing rotations allowing us to use the world rotations
											   // even if the camera is not at the origin (world rotations occur around origin)
	
	// Mouse rotation
	mTransform = MatrixRotationX(mouseDelta.y * rotationSpeed * pixelToRadians) * mTransform; // Pre-multiply = local-X rotation
	mTransform = mTransform * MatrixRotationY(mouseDelta.x * rotationSpeed * pixelToRadians); // Post-multiply = world-Y rotation


	// Keyboard Rotation
	if (KeyHeld(turnUp))
		mTransform = MatrixRotationX(-rotationSpeed * frameTime) * mTransform; // Pre-multiply = local-X rotation
	
	if (KeyHeld(turnDown))
		mTransform = MatrixRotationX(rotationSpeed * frameTime) * mTransform;

	if (KeyHeld(turnRight))
		mTransform = mTransform * MatrixRotationY(rotationSpeed * frameTime); // Post-multiply = world-Y rotation

	if (KeyHeld(turnLeft))
		mTransform = mTransform * MatrixRotationY(-rotationSpeed * frameTime);
	mTransform.Position() = tempPosition;

	// Keyboard Rotation
	if (KeyHeld(moveRight))
		mTransform.Position() += movementSpeed * mTransform.XAxis() * frameTime;
	
	if (KeyHeld(moveLeft))
		mTransform.Position() -= movementSpeed * mTransform.XAxis() * frameTime;

	if (KeyHeld(moveForward) || KeyHeld(Mouse_RButton))
		mTransform.Position() += movementSpeed * mTransform.ZAxis() * frameTime;
	
	if (KeyHeld(moveBackward))
		mTransform.Position() -= movementSpeed * mTransform.ZAxis() * frameTime;
}


// Update the matrices used for the camera in the rendering process
void Camera::UpdateMatrices()
{
	// View Matrix is the usual matrix used for the camera in shaders, it is the inverse of the world matrix
	mViewMatrix = InverseAffine(mTransform);

	// Projection Matrix, how to flatten the 3D world onto the screen
	float tanFOVx = std::tan(mFOVx * 0.5f);
	float scaleX = 1.0f / tanFOVx;
	float scaleY = mAspectRatio / tanFOVx;
	float scaleZa = mFarClip / (mFarClip - mNearClip);
	float scaleZb = -mNearClip * scaleZa;
	mProjectionMatrix =
	{
		scaleX, 0.0f, 0.0f, 0.0f,
		0.0f, scaleY, 0.0f, 0.0f,
		0.0f, 0.0f, scaleZa, 1.0f,
		0.0f, 0.0f, scaleZb, 0.0f
	};

	// The view-projection matrix combines the two matrices above into one, which can save a multiplication in the shader
	mViewProjectionMatrix = mViewMatrix * mProjectionMatrix;
}


// Return pixel coordinates corresponding to given world point when viewing from this camera
// Pass viewport width and height. Returned Vector3 contains pixel coordinates in x, y and z
// distances to the world point in Z. If Z distance is less than near clip, the world point is
// behind the camera and the 2D x and y coordinates can be ignored.
Vector3 Camera::PixelFromWorldPoint(Vector3 worldPoint, unsigned int viewportWidth, unsigned int viewportHeight)
{
	Vector3 pixelPoint;

	UpdateMatrices();

	// Transform world point into camera space and return immediately if point is behind camera near clip (it won't be on the screen)
	Vector4 cameraPoint = mViewMatrix.TransformPoint(worldPoint);
	if (cameraPoint.z < mNearClip)
	{
		return { 0, 0, cameraPoint.z };
	}

	// Transform into Viewport (2D) Space

	Vector4 viewportPoint = cameraPoint;
	viewportPoint = cameraPoint * mProjectionMatrix;

	viewportPoint.x /= viewportPoint.w;
	viewportPoint.y /= viewportPoint.w;

	float x = (viewportPoint.x + 1.0f) * viewportHeight * 0.5f;
	float y = (1.0f - viewportPoint.y) * viewportWidth * 0.5f;

	return { x, y, cameraPoint.z };
}

Vector3 Camera::WorldPointFromPixel(Vector2i pixelPoint, int viewportWidth, int viewportHeight)
{
	Vector4 cameraPoint;

	cameraPoint.x = pixelPoint.x / (viewportWidth / 2) - 1;
	cameraPoint.y = 1 - pixelPoint.y / (viewportHeight / 2);

	cameraPoint.z = 0;
	cameraPoint.w = mNearClip;

	cameraPoint.x *= cameraPoint.w;
	cameraPoint.y *= cameraPoint.w;
	cameraPoint.z *= cameraPoint.w;

	Vector3 worldPoint = cameraPoint * InverseAffine(mViewProjectionMatrix);

	return worldPoint;
}

// Return the size of a pixel at given Z distance. Allows to convert 2D size of areas on screen to actual sizes in the world
	// Pass viewport width and height
Vector2 Camera::PixelSizeInWorldSpace(float Z, unsigned int viewportWidth, unsigned int viewportHeight)
{
	Vector2 size;

	// Size of the entire viewport in world space at near clip distance
	Vector2 viewportSizeAtNearClip;
	viewportSizeAtNearClip.x = 2 * mNearClip * std::tan(mFOVx * 0.5f);
	viewportSizeAtNearClip.y = viewportSizeAtNearClip.x / mAspectRatio;

	/// Size of the entire viewport in world space at the given Z distance
	Vector2 viewportSizeAtZ = viewportSizeAtNearClip * (Z / mNearClip);

	// Return world size of single pixel at given Z distance
	return { viewportSizeAtZ.x / viewportWidth, viewportSizeAtZ.y / viewportHeight };
}