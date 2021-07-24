#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#if defined(RE_PLATFORM_WINDOWS_32) || defined(RE_PLATFORM_WINDOWS_64)
#include <Platform/Windows/WindowsWindow.h>
#else
#error Rebirth Engine support only Windows!
#endif

#include <VisualNovel/Scene.h>
#include <VisualNovel/Sprite.h>
#include <VisualNovel/DialogueBox.h>
#include <VisualNovel/Sound.h>
#include <VisualNovel/Music.h>
#include <VisualNovel/Menu.h>
#include <VisualNovel/Ambience.h>

#include <OpenGL/GLUtils.h>

#include <Core/Debug/Logger.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool SaveProgress(std::string path, int slot);
bool LoadProgress(std::string path, int slot);

enum CmdList
{
	TEXT,
	SCENE,
	PLAYSOUND,
	STOPSOUND,
	PLAYMUSIC,
	STOPMUSIC,
	SHOWSPRITE,
	HIDESPRITE,
	PLAYAMBIENCE,
	STOPAMBIENCE,
	CHANGEBOX,
	CHANGEESCMENU,
	RETURNTOMENU
};

struct Cmds
{
	CmdList command;
	std::string content;

	std::string who;
	std::string what;

	float posX = 0.0f;
	float poxY = 1.0f;
};

struct RenderContent
{
	std::string text;
	std::string name;
	std::string imagePath;
};

struct Setttings
{
	std::string imagePath = "images/";
	std::string fontPath = "fonts/";
	std::string soundPath = "sounds/";
	std::string gamePath = "game/";
};

namespace rb
{
	class Application
	{
	public:
		void Run();
	protected:
		WindowProps window;
		Setttings settings;

		virtual void Init() = 0;
		virtual void Label() = 0;

		void text(std::string who, std::string what);
		void text(std::string what);
		void scene(std::string path, bool dissolve = true);
		void showSprite(std::string path, float x = 0.0f, float y = 0.0f);
		void hideSprite(std::string path);
		void playMusic(std::string path);
		void stopMusic();
		void playSound(std::string path);
		void stopSound();
		void playAmbience(std::string path);
		void stopAmbience();
		void changeBox(std::string path);
		void changeESCMenu(std::string path);
		void returnToMenu();

	private:
		void AppInit();

		void InitWindow();

		void AppInput();

		void AppRender();
		void AppClose();

		void RenderMenu();
		void RenderEscapeMenu();
	private:
		std::vector<Cmds> list;

		Logger mLogger;

		GLFWwindow* mWindow;

		RenderContent content;

		std::map<std::string, Sprite> mSprites;
		std::map<std::string, Sprite>::iterator mSpriteIt;

		Scene mMainScene;
		DialogueBox mDialogueBox;

		Sprite mEscSprite;
		Sprite mSaveSprite;

		Sound mSound;
		Music mMusic;
		Ambience mAmbience;

		Menu mMenu;

		void NextStatement();

		bool mDrawStartMenu = true;
		bool mCloseWindow = false;
		bool mStartGame = false;

		int StartButton, LoadSaveButton, SaveButton, ExitButton, SaveSlotButton1, SaveSlotButton2, SaveSlotButton3, SaveSlotButton4, SaveSlotButton5, ReturnButton, ReturnButton2, ReturnButton3, ESCImage;

		bool isLoadSave = false;

		float tx1 = 500.0f;
		float ty1 = 1.0f;
		float tx2 = 1.0f;
		float ty2 = 1.0f;
		float tx3 = 1.0f;
		float ty3 = 1.0f;

		unsigned int VAO, VBO;
	};
}

#endif