/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <string>
#include <cassert>

namespace ge {
class CullMode {
public:
	enum Type {
		UNSET = -1,
		/** Neither front or back face is culled. This is default. */
		None = 0,
		/** Cull the front faces. */
		Front,
		/** Cull the back faces. */
		Back,
		/** Cull both the front and back faces. */
		FrontAndBack
	};

private:

	Type _val;

public:

	CullMode() : _val(UNSET) {
	}

	CullMode(Type val) : _val(val) {
	}

	operator Type() const { return _val; }

	static CullMode valueOf(const std::string&);

	int getGLValue() const;
};

class PolygonWind {
public:
	enum Type {
		UNSET = -1,
		/** Polygons whose vertices are specified in CCW order are front facing. This is default. */
		CounterClockWise = 0,
		/** Polygons whose vertices are specified in CW order are front facing. */
		ClockWise
	};

private:

	Type _val;

public:

	PolygonWind() : _val(UNSET) {
	}

	PolygonWind(Type val) : _val(val) {
	}

	operator Type() const { return _val; }

	static PolygonWind valueOf(const std::string&);

	int getGLValue() const;
};

class SourceFunction {
public:

	enum Type {
		UNSET = -1,
		/**
		* The source value of the blend function is all zeros.
		*/
		Zero = 0,
		/**
		* The source value of the blend function is all ones.
		*/
		One,
		/**
		* The source value of the blend function is the destination color.
		*/
		DestinationColor,
		/**
		* The source value of the blend function is 1 - the destination color.
		*/
		OneMinusDestinationColor,
		/**
		* The source value of the blend function is the source alpha value.
		*/
		SourceAlpha,
		/**
		* The source value of the blend function is 1 - the source alpha value.
		*/
		OneMinusSourceAlpha,
		/**
		* The source value of the blend function is the destination alpha.
		*/
		DestinationAlpha,
		/**
		* The source value of the blend function is 1 - the destination alpha.
		*/
		OneMinusDestinationAlpha,
		/**
		* The source value of the blend function is the minimum of alpha or 1 - alpha.
		*/
		SourceAlphaSaturate,
		/**
		* The source value of the blend function is the value of the constant color. (Rc, Gc, Bc, Ac) If not set, black
		* with alpha = 0 is used. If not supported, falls back to One.
		*/
		ConstantColor,
		/**
		* The source value of the blend function is 1 minus the value of the constant color. (1-Rc, 1-Gc, 1-Bc, 1-Ac)
		* If color is not set, black with alpha = 0 is used. If not supported, falls back to One.
		*/
		OneMinusConstantColor,
		/**
		* The source value of the blend function is the value of the constant color's alpha. (Ac, Ac, Ac, Ac) If not
		* set, black with alpha = 0 is used. If not supported, falls back to One.
		*/
		ConstantAlpha,
		/**
		* The source value of the blend function is 1 minus the value of the constant color's alpha. (1-Ac, 1-Ac, 1-Ac,
		* 1-Ac) If color is not set, black with alpha = 0 is used. If not supported, falls back to One.
		*/
		OneMinusConstantAlpha
	};

	SourceFunction() : _val(UNSET) {
	}

	SourceFunction(Type val) : _val(val) {
	}

	operator Type() const { return _val; }

	bool usesConstantColor() const {
		switch (_val) {
		case Zero: return false;
		case One: return false;
		case DestinationColor: return false;
		case OneMinusDestinationColor: return false;
		case SourceAlpha: return false;
		case OneMinusSourceAlpha: return false;
		case DestinationAlpha: return false;
		case OneMinusDestinationAlpha: return false;
		case SourceAlphaSaturate: return false;
		case ConstantColor: return true;
		case OneMinusConstantColor: return true;
		case ConstantAlpha: return true;
		case OneMinusConstantAlpha: return true;
		case UNSET: assert(false);
		}
		assert(false);
		return false;
	}

	static SourceFunction valueOf(const std::string&);

	static std::string toString(SourceFunction);

#ifdef BTS_TARGET_PS4
	sce::Gnm::BlendMultiplier getGnmValue() const;
#else
	int getGLValue() const;
#endif

private:

	Type _val;
};

class BlendEquation {
public:
	enum Type {
		UNSET = -1,
		/**
		* Sets the blend equation so that the source and destination data are added. (Default) Clamps to [0,1] Useful
		* for things like antialiasing and transparency.
		*/
		Add = 0,
		/**
		* Sets the blend equation so that the source and destination data are subtracted (Src - Dest). Clamps to [0,1]
		* Falls back to Add if supportsSubtract is false.
		*/
		Subtract,
		/**
		* Same as Subtract, but the order is reversed (Dst - Src). Clamps to [0,1] Falls back to Add if
		* supportsSubtract is false.
		*/
		ReverseSubtract
	};

	BlendEquation() : _val(UNSET) {
	}

	BlendEquation(Type val) : _val(val) {
	}

	operator Type() const { return _val; }

#ifdef BTS_TARGET_PS4
	sce::Gnm::BlendFunc getGnmValue() const;
#else
	int getGLValue() const;
#endif

	static BlendEquation valueOf(const std::string&);

	static std::string toString(BlendEquation);
private:

	Type _val;
};

class DestinationFunction {
public:

	enum Type {
		UNSET = -1,
		/**
		* The destination value of the blend function is all zeros.
		*/
		Zero = 0,
		/**
		* The destination value of the blend function is all ones.
		*/
		One,
		/**
		* The destination value of the blend function is the source color.
		*/
		SourceColor,
		/**
		* The destination value of the blend function is 1 - the source color.
		*/
		OneMinusSourceColor,
		/**
		* The destination value of the blend function is the source alpha value.
		*/
		SourceAlpha,
		/**
		* The destination value of the blend function is 1 - the source alpha value.
		*/
		OneMinusSourceAlpha,
		/**
		* The destination value of the blend function is the destination alpha value.
		*/
		DestinationAlpha,
		/**
		* The destination value of the blend function is 1 - the destination alpha value.
		*/
		OneMinusDestinationAlpha,
		/**
		* The destination value of the blend function is the value of the constant color. (Rc, Gc, Bc, Ac) If not set,
		* black with alpha = 0 is used. If not supported, falls back to One.
		*/
		ConstantColor,
		/**
		* The destination value of the blend function is 1 minus the value of the constant color. (1-Rc, 1-Gc, 1-Bc,
		* 1-Ac) If color is not set, black with alpha = 0 is used. If not supported, falls back to One.
		*/
		OneMinusConstantColor,
		/**
		* The destination value of the blend function is the value of the constant color's alpha. (Ac, Ac, Ac, Ac) If
		* not set, black with alpha = 0 is used. If not supported, falls back to One.
		*/
		ConstantAlpha,
		/**
		* The destination value of the blend function is 1 minus the value of the constant color's alpha. (1-Ac, 1-Ac,
		* 1-Ac, 1-Ac) If color is not set, black with alpha = 0 is used. If not supported, falls back to One.
		*/
		OneMinusConstantAlpha
	};

	DestinationFunction() : _val(UNSET) {
	}

	DestinationFunction(Type val) : _val(val) {
	}

	operator Type() const { return _val; }

	bool usesConstantColor() const {
		switch (_val) {
		case Zero: return false;
		case One: return false;
		case SourceColor: return false;
		case OneMinusSourceColor: return false;
		case SourceAlpha: return false;
		case OneMinusSourceAlpha: return false;
		case DestinationAlpha: return false;
		case OneMinusDestinationAlpha: return false;
		case ConstantColor: return true;
		case OneMinusConstantColor: return true;
		case ConstantAlpha: return true;
		case OneMinusConstantAlpha: return true;
		case UNSET: assert(false);
		}
		assert(false);
		return false;
	}

	int getGLValue() const;

	static DestinationFunction valueOf(const std::string&);

	static std::string toString(DestinationFunction);

private:

	Type _val;
};

class TestFunction {
public:
	enum Type {
		UNSET,
		/**
		* Depth comparison never passes.
		*/
		Never,
		/**
		* Depth comparison always passes.
		*/
		Always,
		/**
		* Passes if the incoming value is the same as the stored value.
		*/
		EqualTo,
		/**
		* Passes if the incoming value is not equal to the stored value.
		*/
		NotEqualTo,
		/**
		* Passes if the incoming value is less than the stored value.
		*/
		LessThan,
		/**
		* Passes if the incoming value is less than or equal to the stored value.
		*/
		LessThanOrEqualTo,
		/**
		* Passes if the incoming value is greater than the stored value.
		*/
		GreaterThan,
		/**
		* Passes if the incoming value is greater than or equal to the stored value.
		*/
		GreaterThanOrEqualTo
	};

private:

	Type _val;

public:

	TestFunction() : _val(UNSET) {
	}

	TestFunction(Type val) : _val(val) {
	}

	operator Type() const { return _val; }

	int getGLValue() const;

	static TestFunction valueOf(const std::string&);
};

class RenderContext;

class RenderSettings {
public:

	RenderSettings();
	virtual ~RenderSettings();

	void bind(RenderContext& context);

	// First frame?
	bool first;

	/** Wireframe mode. */
	bool wireframe;
	/** The cull face set for this CullState. */
	CullMode cullMode;
	/** The polygonWind order set for this CullState. */
	PolygonWind polygonWind;
	/** Depth test enabled. */
	bool depthTest;
	/** Depth function. */
	TestFunction depthFunction;
	/** Depth mask is writable or not. */
	bool depthWrite;
	/** Enables/disables color writing. */
	bool colorWriteRed;
	bool colorWriteGreen;
	bool colorWriteBlue;
	bool colorWriteAlpha;
	/** The current value of if blend is enabled. */
	bool blendEnabled;
	/** The blend color used in constant blend operations. */
	glm::vec4 blendColor;
	/** The current source blend function. */
	SourceFunction sourceBlendFunctionRGB;
	/** The current destination blend function. */
	DestinationFunction destinationBlendFunctionRGB;
	/** The current blend equation. */
	BlendEquation blendEquationRGB;
	/** The current source blend function. */
	SourceFunction sourceBlendFunctionAlpha;
	/** The current destination blend function. */
	DestinationFunction destinationBlendFunctionAlpha;
	/** The current blend equation. */
	BlendEquation blendEquationAlpha;
	
	//BlendOrderHint blendOrderHint;
		
	/** Stencil values */
	//bool _stencilEnabled;
	//StencilFunction _stencilFrontFunction;
	//int _stencilFrontReference;
	//GLuint _stencilFrontFunctionMask;
	//GLuint _stencilFrontWriteMask;
	//StencilOperation _stencilFrontFailOp;
	//StencilOperation _stencilFrontZFailOp;
	//StencilOperation _stencilFrontZPassOp;
	//StencilFunction _stencilBackFunction;
	//int _stencilBackReference;

	//GLuint _stencilBackFunctionMask;
	//GLuint _stencilBackWriteMask;

	//StencilOperation _stencilBackFailOp;
	//StencilOperation _stencilBackZFailOp;
	//StencilOperation _stencilBackZPassOp;

	//ColorRGBA ambient;
	//ColorRGBA diffuse;
	//ColorRGBA specular;
	//float shininess;
	//ColorRGBA emissive;
};
}