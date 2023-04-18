//
// Created by 32725 on 2023/4/17.
//

#ifndef OPENGL_RENDER_GUIZMO_H
#define OPENGL_RENDER_GUIZMO_H
#include "glm/glm/glm.hpp"
namespace Z {
	class Guizmo {
	public:
		static void Init(ImVec2 pos,ImVec2 size);
		static void CheckOperation();
		static void DrawGuizmo(glm::mat4 &cameraView, glm::mat4 &cameraProjection, glm::mat4 &transform);
	};

}


#endif //OPENGL_RENDER_GUIZMO_H
