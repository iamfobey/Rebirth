#pragma once

#ifndef GLUTILS_H
#define GLUTILS_H

#include <OpenGL/GLCore.h>
#include <gl/GLU.h>

namespace rb
{
	namespace glu
	{
		void ClientToGL(int x, int y, double* ox, double* oy, double* oz);
	}
}

#endif