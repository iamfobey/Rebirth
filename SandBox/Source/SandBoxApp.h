/*
*  Thank you for staying with us.
*  (c)Yume Games 2020 - 2021
*/

#pragma once

#pragma comment (lib, "Rebirth.lib")
#include <Rebirth.h>

#ifndef SANDBOXAPP_H
#define SANDBOXAPP_H

class SandBoxApp : public rb::Application
{
	void Init() override;

	void Label() override;
};

#endif