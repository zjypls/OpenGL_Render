//
// Created by 32725 on 2023/4/16.
//

#ifndef RENDERER003_OGL_MYIMGUI_H
#define RENDERER003_OGL_MYIMGUI_H
#include "imgui/imgui.h"
namespace Z{
class MyImGui {
public:
	static void Init();
	static void Begin();
	static void End();
	static void Shutdown();
};

}


#endif //RENDERER003_OGL_MYIMGUI_H
