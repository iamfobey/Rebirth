/*
*  Thank you for staying with us.
*  (c) Yume Games 2020 - 2021
*/

#include "SandBoxApp.h"

void SandBoxApp::Init()
{
	window.Width = 1600;
	window.Height = 900;
	window.Title = "SandBox";

	settings.GameDir = "game/";
	settings.FontDir = settings.GameDir + "fonts/Font.ttf";
	settings.ImagesDir = settings.GameDir + "images/";
	settings.SoundsDir = settings.GameDir + "sounds/";
}

void SandBoxApp::Label()
{
	scene("bg/park.png");
	text("Hello World!");
	text("Test");
	text("s");
}
