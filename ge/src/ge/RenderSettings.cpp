/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/RenderSettings.h>
#include <ge/RenderContext.h>
#include <ge/Exception.h>
#include <ge/tinyformat.h>
#include <algorithm>

using namespace ge;
using namespace std;

namespace {
	const std::string CullMode_names[] = {
		"Back",
		"Front",
		"FrontAndBack",
		"None",
	};
	const std::string* CullMode_end = &CullMode_names[sizeof(CullMode_names) / sizeof(CullMode_names[0])];
	const CullMode CullMode_values[] = {
		CullMode::Back,
		CullMode::Front,
		CullMode::FrontAndBack,
		CullMode::None,
	};
	const std::string PolygonWind_names[] = {
		"ClockWise",
		"CounterClockWise",
	};
	const std::string* PolygonWind_end =
		&PolygonWind_names[sizeof(PolygonWind_names) / sizeof(PolygonWind_names[0])];
	const PolygonWind PolygonWind_values[] = {
		PolygonWind::ClockWise,
		PolygonWind::CounterClockWise,
	};

	const std::string SourceFunction_names[] = {
		"ConstantAlpha",
		"ConstantColor",
		"DestinationAlpha",
		"DestinationColor",
		"One",
		"OneMinusConstantAlpha",
		"OneMinusConstantColor",
		"OneMinusDestinationAlpha",
		"OneMinusDestinationColor",
		"OneMinusSourceAlpha",
		"SourceAlpha",
		"SourceAlphaSaturate",
		"Zero",
	};
	const std::string* SourceFunction_end =
		&SourceFunction_names[sizeof(SourceFunction_names) / sizeof(SourceFunction_names[0])];
	const SourceFunction SourceFunction_values[] = {
		SourceFunction::ConstantAlpha,
		SourceFunction::ConstantColor,
		SourceFunction::DestinationAlpha,
		SourceFunction::DestinationColor,
		SourceFunction::One,
		SourceFunction::OneMinusConstantAlpha,
		SourceFunction::OneMinusConstantColor,
		SourceFunction::OneMinusDestinationAlpha,
		SourceFunction::OneMinusDestinationColor,
		SourceFunction::OneMinusSourceAlpha,
		SourceFunction::SourceAlpha,
		SourceFunction::SourceAlphaSaturate,
		SourceFunction::Zero,
	};

	const SourceFunction* SourceFunction_valuesEnd =
		&SourceFunction_values[sizeof(SourceFunction_values) / sizeof(SourceFunction_values[0])];

	const std::string DestinationFunction_names[] = {
		"ConstantAlpha",
		"ConstantColor",
		"DestinationAlpha",
		"One",
		"OneMinusConstantAlpha",
		"OneMinusConstantColor",
		"OneMinusDestinationAlpha",
		"OneMinusSourceAlpha",
		"OneMinusSourceColor",
		"SourceAlpha",
		"SourceColor",
		"Zero",
	};
	const std::string* DestinationFunction_end =
		&DestinationFunction_names[sizeof(DestinationFunction_names) / sizeof(DestinationFunction_names[0])];
	const DestinationFunction DestinationFunction_values[] = {
		DestinationFunction::ConstantAlpha,
		DestinationFunction::ConstantColor,
		DestinationFunction::DestinationAlpha,
		DestinationFunction::One,
		DestinationFunction::OneMinusConstantAlpha,
		DestinationFunction::OneMinusConstantColor,
		DestinationFunction::OneMinusDestinationAlpha,
		DestinationFunction::OneMinusSourceAlpha,
		DestinationFunction::OneMinusSourceColor,
		DestinationFunction::SourceAlpha,
		DestinationFunction::SourceColor,
		DestinationFunction::Zero
	};

	const DestinationFunction* DestinationFunction_valuesEnd =
		&DestinationFunction_values[sizeof(DestinationFunction_values) / sizeof(DestinationFunction_values[0])];

	const std::string BlendEquation_names[] = {
		"Add",
		"ReverseSubtract",
		"Subtract",
	};
	const std::string* BlendEquation_end =
		&BlendEquation_names[sizeof(BlendEquation_names) / sizeof(BlendEquation_names[0])];
	const BlendEquation BlendEquation_values[] = {
		BlendEquation::Add,
		BlendEquation::ReverseSubtract,
		BlendEquation::Subtract,
	};

	const BlendEquation* BlendEquation_valuesEnd =
		&BlendEquation_values[sizeof(BlendEquation_values) / sizeof(BlendEquation_values[0])];

	static const std::string TestFunction_names[] = {
		"Always",
		"EqualTo",
		"GreaterThan",
		"GreaterThanOrEqualTo",
		"LessThan",
		"LessThanOrEqualTo",
		"Never",
		"NotEqualTo",
	};
	static const std::string* TestFunction_end =
		&TestFunction_names[sizeof(TestFunction_names) / sizeof(TestFunction_names[0])];
	static const TestFunction TestFunction_values[] = {
		TestFunction::Always,
		TestFunction::EqualTo,
		TestFunction::GreaterThan,
		TestFunction::GreaterThanOrEqualTo,
		TestFunction::LessThan,
		TestFunction::LessThanOrEqualTo,
		TestFunction::Never,
		TestFunction::NotEqualTo,
	};

}
CullMode
CullMode::valueOf(const std::string& name) {
	const std::string* bound = lower_bound(&CullMode_names[0], CullMode_end, name);
	if (bound == CullMode_end) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %s", name));
	}
	return CullMode_values[bound - CullMode_names];
}

int
CullMode::getGLValue() const {
	switch (_val) {
	case Front:
		return GL_FRONT;
	case Back:
		return GL_BACK;
	case FrontAndBack:
		// n.b., GL_FRONT_AND_BACK does not seem to work on iOS
		return GL_FRONT_AND_BACK;
	default:
		assert(false);
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value: %d", _val));
	}
	return GL_BACK;
}

PolygonWind
PolygonWind::valueOf(const std::string& name) {
	const std::string* bound = lower_bound(&PolygonWind_names[0], PolygonWind_end, name);
	if (bound == PolygonWind_end) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %s", name));
	}
	return PolygonWind_values[bound - PolygonWind_names];
}

int
PolygonWind::getGLValue() const {
	switch (_val) {
	case CounterClockWise:
		return GL_CCW;
	case ClockWise:
		return GL_CW;
	default:
		assert(false);
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value: %d", _val));
	}
	return GL_CW;
}

SourceFunction
SourceFunction::valueOf(const std::string& name) {
	const std::string* bound = lower_bound(SourceFunction_names, SourceFunction_end, name);
	if (bound == SourceFunction_end) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %s", name));
	}
	return SourceFunction_values[bound - SourceFunction_names];
}

std::string
SourceFunction::toString(SourceFunction val) {
	const SourceFunction* bound = std::find(SourceFunction_values, SourceFunction_valuesEnd, val);
	if (bound == SourceFunction_valuesEnd) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %d", val));
	}
	return SourceFunction_names[bound - SourceFunction_values];
}

int
SourceFunction::getGLValue() const {
	switch (_val) {
	case SourceFunction::Zero:
		return GL_ZERO;
	case SourceFunction::DestinationColor:
		return GL_DST_COLOR;
	case SourceFunction::OneMinusDestinationColor:
		return GL_ONE_MINUS_DST_COLOR;
	case SourceFunction::SourceAlpha:
		return GL_SRC_ALPHA;
	case SourceFunction::OneMinusSourceAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
	case SourceFunction::DestinationAlpha:
		return GL_DST_ALPHA;
	case SourceFunction::OneMinusDestinationAlpha:
		return GL_ONE_MINUS_DST_ALPHA;
	case SourceFunction::SourceAlphaSaturate:
		return GL_SRC_ALPHA_SATURATE;
	case SourceFunction::ConstantColor:
		return GL_CONSTANT_COLOR;
	case SourceFunction::OneMinusConstantColor:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case SourceFunction::ConstantAlpha:
		return GL_CONSTANT_ALPHA;
	case SourceFunction::OneMinusConstantAlpha:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	case SourceFunction::One:
		return GL_ONE;
	default:
		assert(false);
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value: %d", _val));
	}
}

DestinationFunction
DestinationFunction::valueOf(const std::string& name) {
	const std::string* bound = lower_bound(&DestinationFunction_names[0], DestinationFunction_end, name);
	if (bound == DestinationFunction_end) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %s", name));
	}
	return DestinationFunction_values[bound - DestinationFunction_names];
}

std::string
DestinationFunction::toString(DestinationFunction val) {
	const DestinationFunction* bound = std::find(DestinationFunction_values, DestinationFunction_valuesEnd, val);
	if (bound == DestinationFunction_valuesEnd) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %d", val));
	}
	return DestinationFunction_names[bound - DestinationFunction_values];
}

int
DestinationFunction::getGLValue() const {
	switch (_val) {
	case DestinationFunction::Zero:
		return GL_ZERO;
	case DestinationFunction::SourceColor:
		return GL_SRC_COLOR;
	case DestinationFunction::OneMinusSourceColor:
		return GL_ONE_MINUS_SRC_COLOR;
	case DestinationFunction::SourceAlpha:
		return GL_SRC_ALPHA;
	case DestinationFunction::OneMinusSourceAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
	case DestinationFunction::DestinationAlpha:
		return GL_DST_ALPHA;
	case DestinationFunction::OneMinusDestinationAlpha:
		return GL_ONE_MINUS_DST_ALPHA;
	case DestinationFunction::ConstantColor:
		return GL_CONSTANT_COLOR;
	case DestinationFunction::OneMinusConstantColor:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case DestinationFunction::ConstantAlpha:
		return GL_CONSTANT_ALPHA;
	case DestinationFunction::OneMinusConstantAlpha:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	case DestinationFunction::One:
		return GL_ONE;
	default:
		assert(false);
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value: %d", _val));
	}
}

BlendEquation
BlendEquation::valueOf(const std::string& name) {
	const std::string* bound = lower_bound(&BlendEquation_names[0], BlendEquation_end, name);
	if (bound == BlendEquation_end) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %s", name));
	}
	return BlendEquation_values[bound - BlendEquation_names];
}

std::string
BlendEquation::toString(BlendEquation val) {
	const BlendEquation* bound = std::find(BlendEquation_values, BlendEquation_valuesEnd, val);
	if (bound == BlendEquation_valuesEnd) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %d", val));
	}
	return BlendEquation_names[bound - BlendEquation_values];
}

int
BlendEquation::getGLValue() const {
	switch (_val) {
	case BlendEquation::Subtract:
		return GL_FUNC_SUBTRACT;
	case BlendEquation::ReverseSubtract:
		return GL_FUNC_REVERSE_SUBTRACT;
	case BlendEquation::Add:
		return GL_FUNC_ADD;
	default:
		assert(false);
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value: %d", _val));
	}
}

TestFunction
TestFunction::valueOf(const std::string& name) {
	const std::string* bound = lower_bound(&TestFunction_names[0], TestFunction_end, name);
	if (bound == TestFunction_end) {
		throw IllegalArgumentException(__FILE__, __LINE__, tfm::format("unknown enumeration value %s", name));
	}
	return TestFunction_values[bound - TestFunction_names];
}

int
TestFunction::getGLValue() const {
	switch (_val) {
	case TestFunction::Never:
		return GL_NEVER;
	case TestFunction::LessThan:
		return GL_LESS;
	case TestFunction::EqualTo:
		return GL_EQUAL;
	case TestFunction::LessThanOrEqualTo:
		return GL_LEQUAL;
	case TestFunction::GreaterThan:
		return GL_GREATER;
	case TestFunction::NotEqualTo:
		return GL_NOTEQUAL;
	case TestFunction::GreaterThanOrEqualTo:
		return GL_GEQUAL;
	case TestFunction::Always:
		return GL_ALWAYS;
	default:
		break;
	}
	return GL_NEVER;
}
RenderSettings::RenderSettings() :
	wireframe(false),
	cullMode(CullMode::Back),
	polygonWind(PolygonWind::CounterClockWise),
	depthTest(true),
	depthFunction(TestFunction::LessThan),
	depthWrite(true),
	colorWriteRed(true),
	colorWriteGreen(true),
	colorWriteBlue(true),
	colorWriteAlpha(true),
	blendEnabled(false),
	blendColor(0, 0, 0, 0),
	sourceBlendFunctionRGB(SourceFunction::SourceAlpha),
	destinationBlendFunctionRGB(DestinationFunction::OneMinusSourceAlpha),
	blendEquationRGB(BlendEquation::Add),
	sourceBlendFunctionAlpha(SourceFunction::SourceAlpha),
	destinationBlendFunctionAlpha(DestinationFunction::OneMinusSourceAlpha),
	blendEquationAlpha(BlendEquation::Add)
	//blendOrderHint(BlendOrderHint::TransitionLike),
	//sortHint(0),
	//stencilEnabled(false),
	//stencilFrontFunction(StencilFunction::Always),
	//stencilFrontReference(0),
	//stencilFrontFunctionMask(~0),
	//stencilFrontWriteMask(~0),
	//stencilFrontFailOp(StencilOperation::Keep),
	//stencilFrontZFailOp(StencilOperation::Keep),
	//stencilFrontZPassOp(StencilOperation::Keep),
	//stencilBackFunction(StencilFunction::Always),
	//stencilBackReference(0),
	//stencilBackFunctionMask(~0),
	//stencilBackWriteMask(~0),
	//stencilBackFailOp(StencilOperation::Keep),
	//stencilBackZFailOp(StencilOperation::Keep),
	//stencilBackZPassOp(StencilOperation::Keep),
	//ambient(0.2f, 0.2f, 0.2f, 1.0f),
	//diffuse(0.8f, 0.8f, 0.8f, 1.0f),
	//specular(0.0f, 0.0f, 0.0f, 1.0f),
	//shininess(0.0f),
	//emissive(0.0f, 0.0f, 0.0f, 1.0f)
{
}

RenderSettings::~RenderSettings() {

}

void
RenderSettings::bind(RenderContext& context) {
	RenderSettings& currentSettings = context.renderSettings;

	if (currentSettings.first || wireframe != currentSettings.wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
		currentSettings.wireframe = wireframe;
	}

	if (currentSettings.first || cullMode != currentSettings.cullMode) {
		if (cullMode == CullMode::None) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(cullMode.getGLValue());
		}
		currentSettings.cullMode = cullMode;
	}

	if (currentSettings.first || polygonWind != currentSettings.polygonWind) {
		glFrontFace(polygonWind.getGLValue());
		currentSettings.polygonWind = polygonWind;
	}

	if (currentSettings.first || depthTest != currentSettings.depthTest) {
		if (depthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
		currentSettings.depthTest = depthTest;
	}

	if (currentSettings.first || depthFunction != currentSettings.depthFunction) {
		glDepthFunc(depthFunction.getGLValue());
		currentSettings.depthFunction = depthFunction;
	}

	if (currentSettings.first || depthWrite != currentSettings.depthWrite) {
		glDepthMask(depthWrite);
		currentSettings.depthWrite = depthWrite;
	}

	if (currentSettings.first || colorWriteRed != currentSettings.colorWriteRed || colorWriteGreen != currentSettings.colorWriteGreen ||
		colorWriteBlue != currentSettings.colorWriteBlue || colorWriteAlpha != currentSettings.colorWriteAlpha) {
		glColorMask(colorWriteRed, colorWriteGreen, colorWriteBlue, colorWriteAlpha);
		currentSettings.colorWriteRed = colorWriteRed;
		currentSettings.colorWriteGreen = colorWriteGreen;
		currentSettings.colorWriteBlue = colorWriteBlue;
		currentSettings.colorWriteAlpha = colorWriteAlpha;
	}

	if (currentSettings.first || blendEnabled != currentSettings.blendEnabled) {
		if (blendEnabled) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
		currentSettings.blendEnabled = blendEnabled;
	}

	if (currentSettings.first || blendColor != currentSettings.blendColor) {
		glBlendColor(blendColor.r, blendColor.g, blendColor.b, blendColor.a);
		currentSettings.blendColor = blendColor;
	}

	if (currentSettings.first || sourceBlendFunctionRGB != currentSettings.sourceBlendFunctionRGB || destinationBlendFunctionRGB !=
		currentSettings.destinationBlendFunctionRGB || sourceBlendFunctionAlpha !=
		currentSettings.sourceBlendFunctionAlpha || destinationBlendFunctionAlpha !=
		currentSettings.destinationBlendFunctionAlpha) {
		if (sourceBlendFunctionRGB == sourceBlendFunctionAlpha && destinationBlendFunctionRGB ==
			destinationBlendFunctionAlpha) {
			glBlendFunc(sourceBlendFunctionRGB.getGLValue(), destinationBlendFunctionRGB.getGLValue());
		}
		else {
			glBlendFuncSeparate(sourceBlendFunctionRGB.getGLValue(),
				destinationBlendFunctionRGB.getGLValue(),
				sourceBlendFunctionAlpha.getGLValue(),
				destinationBlendFunctionAlpha.getGLValue());
		}
		currentSettings.sourceBlendFunctionRGB = sourceBlendFunctionRGB;
		currentSettings.destinationBlendFunctionRGB = destinationBlendFunctionRGB;
		currentSettings.sourceBlendFunctionAlpha = sourceBlendFunctionAlpha;
		currentSettings.destinationBlendFunctionAlpha = destinationBlendFunctionAlpha;
	}

	if (currentSettings.first || blendEquationRGB != currentSettings.blendEquationRGB || blendEquationAlpha !=
		currentSettings.blendEquationAlpha) {
		if (blendEquationRGB == blendEquationAlpha) {
			glBlendEquation(blendEquationRGB.getGLValue());
		}
		else {
			glBlendEquationSeparate(blendEquationRGB.getGLValue(), blendEquationAlpha.getGLValue());
		}
		currentSettings.blendEquationRGB = blendEquationRGB;
		currentSettings.blendEquationAlpha = blendEquationAlpha;
	}
	currentSettings.first = false;

#ifdef never
	if (stencilEnabled != currentSettings.stencilEnabled) {
		if (stencilEnabled) {
			glEnable(GL_STENCIL_TEST);
		}
		else {
			glDisable(GL_STENCIL_TEST);
		}
		currentSettings.stencilEnabled = stencilEnabled;
	}

	if (stencilEnabled) {
		if (stencilFrontFunction != currentSettings.stencilFrontFunction ||
			stencilFrontReference != currentSettings.stencilFrontReference ||
			stencilFrontFunctionMask != currentSettings.stencilFrontFunctionMask ||
			stencilBackFunction != currentSettings.stencilBackFunction ||
			stencilBackReference != currentSettings.stencilBackReference ||
			stencilBackFunctionMask != currentSettings.stencilBackFunctionMask) {
			if (stencilFrontFunction == stencilBackFunction &&
				stencilFrontReference == stencilBackReference &&
				stencilFrontFunctionMask == stencilBackFunctionMask) {
				glStencilFunc(stencilFrontFunction.getGLValue(), stencilFrontReference, stencilFrontFunctionMask);
			}
			else {
				glStencilFuncSeparate(GL_FRONT,
					stencilFrontFunction.getGLValue(), stencilFrontReference,
					stencilFrontFunctionMask);
				glStencilFuncSeparate(GL_BACK,
					stencilBackFunction.getGLValue(), stencilBackReference,
					stencilBackFunctionMask);
			}
			currentSettings.stencilFrontFunction = stencilFrontFunction;
			currentSettings.stencilFrontReference = stencilFrontReference;
			currentSettings.stencilFrontFunctionMask = stencilFrontFunctionMask;
			currentSettings.stencilBackFunction = stencilBackFunction;
			currentSettings.stencilBackReference = stencilBackReference;
			currentSettings.stencilBackFunctionMask = stencilBackFunctionMask;
		}

		if (stencilFrontWriteMask != currentSettings.stencilFrontWriteMask ||
			stencilBackWriteMask != currentSettings.stencilBackWriteMask) {
			if (stencilFrontWriteMask == stencilBackWriteMask) {
				glStencilMask(stencilFrontWriteMask);
			}
			else {
				glStencilMaskSeparate(GL_FRONT, stencilFrontWriteMask);
				glStencilMaskSeparate(GL_BACK, stencilBackWriteMask);
			}
			currentSettings.stencilFrontWriteMask = stencilFrontWriteMask;
			currentSettings.stencilBackWriteMask = stencilBackWriteMask;
		}

		if (stencilFrontFailOp != currentSettings.stencilFrontFailOp ||
			stencilFrontZFailOp != currentSettings.stencilFrontZFailOp ||
			stencilFrontZPassOp != currentSettings.stencilFrontZPassOp ||
			stencilBackFailOp != currentSettings.stencilBackFailOp ||
			stencilBackZFailOp != currentSettings.stencilBackZFailOp ||
			stencilBackZPassOp != currentSettings.stencilBackZPassOp) {
			if (stencilFrontFailOp == stencilBackFailOp &&
				stencilFrontZFailOp == stencilBackZFailOp &&
				stencilFrontZPassOp == stencilBackZPassOp) {
				glStencilOp(stencilFrontFailOp.getGLValue(),
					stencilFrontZFailOp.getGLValue(), stencilFrontZPassOp.getGLValue());
			}
			else {
				if (stencilFrontFailOp != currentSettings.stencilFrontFailOp ||
					stencilFrontZFailOp != currentSettings.stencilFrontZFailOp ||
					stencilFrontZPassOp != currentSettings.stencilFrontZPassOp) {
					glStencilOpSeparate(GL_FRONT, stencilFrontFailOp.getGLValue(),
						stencilFrontZFailOp.getGLValue(), stencilFrontZPassOp.getGLValue());
				}
				if (stencilBackFailOp != currentSettings.stencilBackFailOp ||
					stencilBackZFailOp != currentSettings.stencilBackZFailOp ||
					stencilBackZPassOp != currentSettings.stencilBackZPassOp) {
					glStencilOpSeparate(GL_BACK, stencilBackFailOp.getGLValue(),
						stencilBackZFailOp.getGLValue(), stencilBackZPassOp.getGLValue());
				}
			}
			currentSettings.stencilFrontFailOp = stencilFrontFailOp;
			currentSettings.stencilFrontZFailOp = stencilFrontZFailOp;
			currentSettings.stencilFrontZPassOp = stencilFrontZPassOp;
			currentSettings.stencilBackFailOp = stencilBackFailOp;
			currentSettings.stencilBackZFailOp = stencilBackZFailOp;
			currentSettings.stencilBackZPassOp = stencilBackZPassOp;
		}
	}
#endif
}
