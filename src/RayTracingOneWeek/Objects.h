//
// Created by 32725 on 2023/4/19.
//

#ifndef OPENGL_RENDER_OBJECTS_H
#define OPENGL_RENDER_OBJECTS_H

#include "glm/glm/glm.hpp"
#define LAMBERT 1
#define MIRROR 2
#define GLASS 3
#define META 4
#define LIGHT 5
#define FOG 6

struct Sphere {
	glm::vec4 center;
	glm::vec4 color{1,1,1,1.f};
	glm::ivec4 info{LAMBERT,0,0,1};
};
struct Face {
	glm::vec4 v0, v1, v2,v3;
	glm::vec4 minV, maxV;
	glm::vec4 normal;
	glm::vec4 color{1,1,1,1.f};
	glm::ivec4 info{LAMBERT,0,0,0};
};
struct World{
	Sphere spheres[7];
	Face faces[2];
	glm::vec4 count{7,2,0,0};
};


#endif //OPENGL_RENDER_OBJECTS_H
