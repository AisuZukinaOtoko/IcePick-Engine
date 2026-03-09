#include "../Render Systems/Renderer.h"
#include "PhysicsDebugRenderer.h"
#include "PhysicsUtils.h"

namespace IcePick {
	void PysiscsDebugRenderer3D::DrawLine(JPH::RVec3Arg from, JPH::RVec3Arg to, JPH::ColorArg color) {
		glm::vec3 point1 = PhysicsVec3ToVec3(from);
		glm::vec3 point2 = PhysicsVec3ToVec3(to);
		IcePickRenderer::LinePointVertex3D vertex1{ point1, glm::vec4(1.0f) };
		IcePickRenderer::LinePointVertex3D vertex2{ point2, glm::vec4(1.0f) };

		//IcePickRenderer::DrawLine(vertex1, vertex2);
	}

	void PysiscsDebugRenderer3D::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) {

	}
}