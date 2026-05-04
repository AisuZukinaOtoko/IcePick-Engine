#pragma once
#include "Vertex.h"

namespace IcePickRenderer {
	struct RenderLineBuffer {
		static constexpr unsigned int MaxLinePointCount = 500;
		unsigned int NumPoints = 0;
		LinePointVertex3D LinePoints[MaxLinePointCount];
	};
}

