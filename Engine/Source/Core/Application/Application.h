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

#include <OpenGL/GLUtils.h>

#include <Core/Debug/Logger.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void SaveProgress();
void LoadProgress();

enum CmdList
{
	TEXT,
	IMAGE,
	PLAYSOUND,
	STOPSOUND,
	PLAYMUSIC,
	STOPMUSIC,
	SHOWSPRITE,
	HIDESPRITE,
	TXT
};

struct Cmds
{
	CmdList command;
	std::string content;

	std::string who;
	std::string what;
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

		void text(std::string who, std::string what) {
			Cmds temp;
			temp.command = CmdList::TEXT;
			temp.who = who;
			temp.what = what;
			list.push_back(temp);
		}

		void text(std::string what) {
			Cmds temp;
			temp.command = CmdList::TEXT;
			temp.what = what;
			list.push_back(temp);
		}

		void scene(std::string path, bool dissolve = true) {
			Cmds temp;
			temp.command = CmdList::IMAGE;
			temp.content = path;
			list.push_back(temp);
		}

		void showSprite(std::string path) {
			Cmds temp;
			temp.command = CmdList::SHOWSPRITE;
			temp.content = path;
			list.push_back(temp);
		}

		void hideSprite(std::string path) {
			Cmds temp;
			temp.command = CmdList::HIDESPRITE;
			temp.content = path;
			list.push_back(temp);
		}

		void playMusic(std::string path) {
			Cmds temp;
			temp.command = CmdList::PLAYMUSIC;
			temp.content = path;
			list.push_back(temp);
		}

		void stopMusic() {
			Cmds temp;
			temp.command = CmdList::STOPMUSIC;
			list.push_back(temp);
		}

		void playSound(std::string path) {
			Cmds temp;
			temp.command = CmdList::PLAYSOUND;
			temp.content = path;
			list.push_back(temp);
		}

		void stopSound() {
			Cmds temp;
			temp.command = CmdList::STOPSOUND;
			list.push_back(temp);
		}

	private:
		void AppInit();

		void InitWindow();

		void AppInput();

		void AppRender();
		void AppClose();
	private:
		std::vector<Cmds> list;

		Logger mLogger;

		GLFWwindow* mWindow;

		RenderContent content;

		std::map<std::string, Sprite> mSprites;
		std::map<std::string, Sprite>::iterator mSpriteIt;
		
		Scene mMainScene;
		DialogueBox mDialogueBox;

		Sound mSound;
		Music mMusic;

		void NextStatement();

		unsigned int VAO, VBO;
	};
}

#endif