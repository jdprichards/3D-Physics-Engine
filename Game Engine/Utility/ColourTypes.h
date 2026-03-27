//========================================================================================================
// ColourRGB and ColourRGBA Classes encapsulating a colour with a floating point R, G, B and A values
//========================================================================================================

#ifndef _COLOUR_TYPES_H_INCLUDED_
#define _COLOUR_TYPES_H_INCLUDED_

#include "Vector3.h"
#include "Vector4.h"

#include <stdint.h>

// Forward declarations of ColourRGBA class so we can declare a ColourRGB constructor that uses it
class ColourRGBA;

//====================
// ColourRGB class
//====================
class ColourRGB
{
public:
	// Allow public access for such a simple, well-defined class
	float r, g, b;

	//=================
	// Constructors
	//=================

	// Default constructor - leaves values uninitialised (for performance)
	#pragma warning(suppress: 26495) // Disable warning about uninitalised variables on next line
	ColourRGB() {}

	// Construct by value
	ColourRGB(const float rIn, const float gIn, const float bIn)
		: r(rIn), g(gIn), b(bIn) {}

	// Construct using a pointer to three values
	// explictit means don't allow direct conversion from float* to ColourRGB without writing constructor name
	// ColourRGB c = somepointer; // Not allowed
	// ColourRGB c = ColourRGB(somepointer); // Allowed
	explicit ColourRGB(const float* pfElts)
		: r(pfElts[0]), g(pfElts[1]), b(pfElts[2]) {}


	// Construct from ColourRGBA - alpha will be discarded 

	ColourRGB(const ColourRGBA& rgb);

	//=============================
	// Conversion to/from Vector3
	//=============================
	// Allow automatic conversions either way as RGB and XYZ are effectively equivalent
	// Allows us to use vector math on colours

	// Construct from Vector3
	ColourRGB(const Vector3& v)
		:ColourRGB(v.x, v.y, v.x) {}

	// Automatic casting of coour to Vector3
	operator Vector3()
	{
		return { r, g, b };
	};


	//===========================================================
	// Conversion to/from 4 byte integers (hex-colour codes)
	//===========================================================
	// Only 3 bytes of the 4-byte integer will be used for RGB

	// Construct with a single 3-byte RGB value (component range 0->255), typically entered as a hex, 
	// e.g. 0xff00ff for R = 255(1.0), G = 0, B = 255(1.0)
	explicit ColourRGB(uint32_t rgb)
	{
		// Use bit shifting and bitwise "and" to extract the four bytes, then convert to float 0->1 range
		r = (rgb >> 16 & 0xff) / 255.0f;
		g = (rgb >> 8 & 0xff) / 255.0f;
		b = (rgb & 0xff) / 255.0f;
	}

	// Support casting of colour to single 4-byte integer. Integer will be in RGB order with each of the R, G, B in range 0->255
	// E.g. ColourRGB c(128, 240, 48);
	// int colourAsInt (int)c; // c will be 0x80f030, the (int) is required
	explicit operator uint32_t()
	{
		// Convert colours to 0->255 range
		uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
		uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
		uint8_t b8 = static_cast<uint8_t>(b * 255.0f);

		// Use bitshifting and addition to pack into single integer
		return (r8 << 16) + (g8 << 8) + b8;
	}
};


//====================
// ColourRGB class
//====================
class ColourRGBA
{
public:
	// Allow public access for such a simple, well-defined class
	float r, g, b, a;

	//=================
	// Constructors
	//=================

	// Default constructor - leaves values uninitialised (for performance)
	#pragma warning(suppress: 26495) // Disable warning about uninitalised variables on next line
	ColourRGBA() {}

	// Construct by value, alpha is optional
	ColourRGBA(const float rIn, const float gIn, const float bIn, const float aIn = 1)
		: r(rIn), g(gIn), b(bIn), a(aIn) {}


	// Construct using a pointer to four values
	// explictit means don't allow direct conversion from float* to ColourRGBA without writing constructor name
	// ColourRGBA c = somepointer; // Not allowed
	// ColourRGBA c = ColourRGBA(somepointer); // Allowed
	explicit ColourRGBA(const float* pfElts)
		: r(pfElts[0]), g(pfElts[1]), b(pfElts[2]), a(pfElts[3]) {}

	// Construct from ColourRGB - alpha is set to 1
	ColourRGBA(const ColourRGB& rgb); // Code at end of file


	//=============================
	// Conversion to/from Vector4
	//=============================
	// Allow automatic conversions either way as RGBA and XYZW are effectively equivalent
	// Allows us to use vector math on colours

	// Construct from Vector4
	ColourRGBA(const Vector4& v)
		: ColourRGBA(v.x, v.y, v.z, v.w) {}

	// Automatic casting of colour to Vector4
	operator Vector4()
	{
		return { r, g, b, a };
	}

	//===========================================================
	// Conversion to/from 4 byte integers (hex-colour codes)
	//===========================================================

	// Construct with a single 4-byte RGBA value (component range 0->255), typically entered as hex
	// e.g. 0xff00ff00 for A = 255(1.0), R = 0, G = 255(1.0), B = 0
	// As alhpa component is first, this allows input of 3-byte RGB colours
	// e.g. 0x80ff30, in which the alpha will be set to 0
	explicit ColourRGBA(uint32_t argb)
	{
		// Use bit shifting and bitwise "adn" extract the four bytes, then convert to float 0->1 range
		a = (argb >> 24) / 255.0f;
		r = (argb >> 16 & 0xff) / 255.0f;
		g = (argb >> 8 & 0xff) / 255.0f;
		b = (argb & 0xff) / 255.0f;
	}

	// Support explicit casting of colour to unsigned 4-byte integer. Integer will be in ARGB order with each of A, R, G, B in the range 0->255
	// E.g. ColourRGBA c(128, 240, 48);
	// unsigned int colourAsInt = (unsigned int)c; // c will be 0x0080f030, the (unsigned int) is required
	explicit operator uint32_t()
	{
		// Convert colours to 0->255 range
		uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
		uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
		uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
		uint8_t a8 = static_cast<uint8_t>(a * 255.0f);

		// Use bitshifting and additions to pack into a single integer
		return (a8 << 24) + (r8 << 16) + (g8 << 8) + b8;
	}

};

//======================
// Mutual Constructor
//======================

// The ColourRGB and ColourRGBA casses can construct each other, but the code is here after class
// because they need each other's declarations completed to compile

// Construct ColourRGBA from ColourRGB. Alpha is set to 1
inline ColourRGBA::ColourRGBA(const ColourRGB& rgb) 
	: ColourRGBA(rgb.r, rgb.g, rgb.b, 1) {}

// Construct ColourRGB from ColourRGBA. Alpha is discarded
inline ColourRGB::ColourRGB(const ColourRGBA& rgba) 
	: ColourRGB(rgba.r, rgba.g, rgba.b) {}

#endif // !_COLOUR_TYPES_H_INCLUDED_
