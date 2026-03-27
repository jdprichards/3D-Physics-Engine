//=============================
// Key / Mouse Input Functions
//=============================
// Not encapsulated into a class

#include "Input.h"
#include <array>

//===========
// Globals
//===========

// Current state of all keys (and mouse buttons)
std::array<KeyState, NumKeyCodes> gKeyStates;

// Current Position of mouse
Vector2i gMousePosition = { 0,0 };


//================
// Initilisation
//================

// Initialise Input System
void InitInput()
{
	// Initialise input data
	for (auto& keyState : gKeyStates)
		keyState = NotPressed;

	gMousePosition = { 0,0 };
}


//==========
// Events
//==========

// Event Called to indicate that a Key or Mouse Button has been Pressed Down
void KeyDownEvent(KeyCode eKeyCode)
{
	if (gKeyStates[eKeyCode] == NotPressed)
		gKeyStates[eKeyCode] = Pressed;

	else
		gKeyStates[eKeyCode] = Held;
}

// Event Called to indicate that a Key or Mouse Button has been Lifted Up
void KeyUpEvent(KeyCode eKeyCode)
{
	gKeyStates[eKeyCode] = NotPressed;
}

// Event Called to indicate the Mouse has been Moved
void MouseMoveEvent(int x, int y)
{
	gMousePosition += {x, y};
}


//====================
// Input Functions
//====================

// Whem the given Key or Button is pressed down the first call to this function during the key press will return true
// otherwise return false. Used for one-off actions or toggles that trigger once per key-press
// Example key codes: Key_A or Mouse_LButton, see Input.h for a full list.
bool KeyHit(KeyCode eKeyCode)
{
	if (gKeyStates[eKeyCode] == Pressed)
	{
		gKeyStates[eKeyCode] = Held;
		return true;
	}
	return false;
}

// Returns true if a given key or button is pressed down, otherwise false
// Example key codes: Key_A or Mouse_LButton, see Input.h for a full list.
bool KeyHeld(KeyCode eKeyCode)
{
	if (gKeyStates[eKeyCode] == NotPressed)
		return false;

	gKeyStates[eKeyCode] = Held;
	
	return true;
}

// When any key is pressed down the first call to this function during the key press will return true
bool AnyKeyHit()
{
	bool hit = false;
	for (auto& keyState : gKeyStates)
	{
		if (keyState == Pressed)
		{
			keyState = Held;
			hit = true;
		}
	}
	return hit;
}

// Returns true if any key or button is pressed down, otherwise false
bool AnyKeyHeld()
{
	bool hit = false;
	for (auto& keyState : gKeyStates)
	{
		if (keyState != NotPressed)
		{
			keyState = Held;
			hit = true;
		}
	}
	return hit;
}

// Return current position of mouse as Vector2i
Vector2i GetMouse()
{
	return gMousePosition;
}