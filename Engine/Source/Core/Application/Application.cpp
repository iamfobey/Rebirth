#include "Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

const char* vTextShaderCode = { "#version 330 core\n"
"layout(location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"	TexCoords = vertex.zw;\n"
"}\n"
};

const char* fTextShaderCode = { "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main()\n"
"{\n"
"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"	color = vec4(textColor, 1.0) * sampled;\n"
"}\n"
};

#include <fstream>

unsigned int it = 0;
unsigned int it2 = 0;

bool IsLoadSave = false;
bool NextState = false;
bool RenderEscMenu = false;
bool renderButtons = true;

extern int WNDwidth, WNDheight;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			if (RenderEscMenu)
				RenderEscMenu = false;
			else
				RenderEscMenu = true;
		renderButtons = true;
		break;
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) NextState = true;
	default:
		break;
	}
}

bool SaveProgress(std::string path, int slot)
{
	std::fstream file(path + "save.data" + std::to_string(slot), std::ios_base::out | std::ios_base::trunc);

	if (!file.is_open())
		return false;

	file << it;

	file.close();

	return true;
}

bool LoadProgress(std::string path, int slot)
{
	std::fstream file(path + "save.data" + std::to_string(slot), std::ios::in);

	if (!file.is_open())
		return false;

	char buff[50];
	file.read(buff, 50);
	it = std::atoi(buff);
	IsLoadSave = true;

	file.close();

	return true;
}

namespace rb
{
	void Application::Run()
	{
		Init();
		Label();

		AppInit();
		AppRender();
		AppClose();
	}

	void Application::AppInit()
	{
		mLogger.Init(settings.gamePath);
		InitWindow();
		mMainScene.Init();
		mMenu.Init(settings.gamePath + settings.fontPath, window.Width, window.Height);
		mEscSprite.Init();
		mEscSprite.Load(settings.gamePath + "images/ui/exitmenu.png");
		mDialogueBox.Init(settings.gamePath + "images/ui/textbox_blood.png", settings.gamePath + settings.fontPath, window.Width, window.Height);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		mDialogueBox.namePosX = 170.0f;
		mDialogueBox.namePosY = 227.0f;

		mDialogueBox.textPosX = 167.0f;
		mDialogueBox.textPosY = 192.0f;

		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		ImFontConfig font_config;
		font_config.OversampleH = 1;
		font_config.OversampleV = 1;
		font_config.PixelSnapH = 1;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF,
			0x0400, 0x044F,
			0,
		};
		std::string t = settings.gamePath + settings.fontPath;
		io.Fonts->AddFontFromFileTTF(t.c_str(), 13.5f, &font_config, ranges);
		io.IniFilename = nullptr;

		mMainScene.Load(settings.gamePath + settings.imagePath + "ui/bloodsplat_big.png");

		StartButton = mMenu.CreateTextButton();
		LoadSaveButton = mMenu.CreateTextButton();
		SaveButton = mMenu.CreateTextButton();
		ExitButton = mMenu.CreateTextButton();
		SaveSlotButton1 = mMenu.CreateTextButton();
		SaveSlotButton2 = mMenu.CreateTextButton();
		SaveSlotButton3 = mMenu.CreateTextButton();
		SaveSlotButton4 = mMenu.CreateTextButton();
		SaveSlotButton5 = mMenu.CreateTextButton();
		ESCImage = mMenu.CreateImage("game/images/ui/exitmenu.png");

		mSound.SetVolume(0.25f);
		mMusic.SetVolume(0.25f);
	}

	void Application::AppRender()
	{
		double xpos, ypos;

		mMusic.play("game/sounds/music/Azimuth.ogg");

		while (!glfwWindowShouldClose(mWindow))
		{
			int w = 1600, h = 900;

			glfwGetWindowSize(mWindow, &w, &h);

			glViewport(0, 0, w, h);

			WNDwidth = w;
			WNDheight = h;

			glClear(GL_COLOR_BUFFER_BIT);
			glfwGetCursorPos(mWindow, &xpos, &ypos);

			mMusic.update();

			if (mDrawStartMenu)
			{
				RenderMenu();
			}
			else
			{
				if (NextState) NextStatement(); NextState = false;

				mMainScene.Render();

				if (!mMainScene.mDissolve && !mMainScene.mIsStart)
				{
					mSpriteIt = mSprites.begin();
					for (; mSpriteIt != mSprites.end(); mSpriteIt++)
					{
						mSpriteIt->second.Render();
					}

					mDialogueBox.Render(content.name, content.text);

					if (IsLoadSave)
					{
						NextStatement();
					}
				}

				mMenu.SetMousePos(xpos, ypos);

				if (RenderEscMenu)
				{
					glfwSetMouseButtonCallback(mWindow, nullptr);
					RenderEscapeMenu();
				}
				else
				{
					glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
						{
							if (action == GLFW_PRESS & button == GLFW_MOUSE_BUTTON_LEFT)
							{
								NextState = true;
							}
						});
				}
			}

			glfwPollEvents();
			glfwSwapBuffers(mWindow);
		}
	}

	void Application::AppClose()
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	bool drawbuttons = true;

	void Application::RenderMenu()
	{
	st: {}
		double xpos, ypos;

		glfwGetCursorPos(mWindow, &xpos, &ypos);

		mMainScene.Render();

		if (!mMainScene.mDissolve && !mStartGame)
		{
			if (drawbuttons)
			{
				if (mMenu.RenderTextButton(StartButton, "Начать", WNDwidth/2.135, WNDheight/1.835, mWindow))
				{
					mStartGame = true;
					mMusic.stop();
					mMainScene.Dissolve();
				}

				if (mMenu.RenderTextButton(LoadSaveButton, "Загрузить", WNDwidth/2.2, WNDheight/2.05, mWindow))
				{
					while (true)
					{
						glClear(GL_COLOR_BUFFER_BIT);
						glfwGetCursorPos(mWindow, &xpos, &ypos);
						mMenu.SetMousePos(xpos, ypos);

						mMainScene.Render();

						if (mMenu.RenderTextButton(SaveSlotButton1, "Слот 1", WNDwidth/2.125, WNDheight/1.59, mWindow)) {
							if (LoadProgress(settings.gamePath, 1))
							{
								mDrawStartMenu = false;
								break;
							}
						}

						if (mMenu.RenderTextButton(SaveSlotButton2, "Слот 2", WNDwidth / 2.125, WNDheight /1.7, mWindow)) {
							if (LoadProgress(settings.gamePath, 2))
							{
								mDrawStartMenu = false;
								break;
							}
						}

						if (mMenu.RenderTextButton(SaveSlotButton3, "Слот 3", WNDwidth / 2.125, WNDheight / 1.83, mWindow)) {
							if (LoadProgress(settings.gamePath, 3))
							{
								mDrawStartMenu = false;
								break;
							}
						}

						if (mMenu.RenderTextButton(SaveSlotButton4, "Слот 4", WNDwidth / 2.125, WNDheight / 1.99, mWindow)) {
							if (LoadProgress(settings.gamePath, 4))
							{
								mDrawStartMenu = false;
								break;
							}
						}

						if (mMenu.RenderTextButton(SaveSlotButton5, "Слот 5", WNDwidth / 2.125, WNDheight / 2.19, mWindow)) {
							if (LoadProgress(settings.gamePath, 5))
							{
								mDrawStartMenu = false;
								break;
							}
						}

						if (mMenu.RenderTextButton(ReturnButton, "Назад", WNDwidth/2.1, WNDheight/2.5, mWindow)) {
							goto st;
						}

						if (glfwWindowShouldClose(mWindow))
						{
							break;
						}

						glfwPollEvents();
						glfwSwapBuffers(mWindow);
					}
					mMusic.stop();
					glfwSetKeyCallback(mWindow, key_callback);
				}

				if (mMenu.RenderTextButton(ExitButton, "Выход", WNDwidth/2.115, WNDheight/2.325, mWindow))
				{
					mCloseWindow = true;
					mMusic.stop();
					mMainScene.Dissolve();
				}
			}
		}

		mMenu.SetMousePos(xpos, ypos);

		if (mCloseWindow && !mMainScene.mDissolve)
		{
			drawbuttons = false;
			if (!mMusic.isPlaying)
			{
				glfwSetWindowShouldClose(mWindow, true);
			}
		}

		if (mStartGame && !mMainScene.mDissolve)
		{
			drawbuttons = false;
			if (!mMusic.isPlaying)
			{
				mDrawStartMenu = false;
				glfwSetKeyCallback(mWindow, key_callback);

				NextStatement();
			}
		}
	}

	void Application::RenderEscapeMenu()
	{
		mMenu.SetRenderImagePos(WNDwidth/119500.0f, -0.176);
		mMenu.RenderImage(ESCImage);

		if (renderButtons)
		{
			if (mMenu.RenderTextButton(LoadSaveButton, "Сохранить", WNDwidth/2.19, WNDheight/1.73, mWindow))
			{
				renderButtons = false;
				isLoadSave = false;

				return;
			}

			if (mMenu.RenderTextButton(SaveButton, "Загрузить", WNDwidth/2.19, WNDheight / 1.94, mWindow))
			{
				renderButtons = false;
				isLoadSave = true;

				return;
			}

			if (mMenu.RenderTextButton(ReturnButton3, "Назад", WNDwidth/2.11, WNDheight / 2.21, mWindow))
			{
				RenderEscMenu = false;
			}

			if (mMenu.RenderTextButton(ExitButton, "Выйти", WNDwidth/2.11, WNDheight/2.52, mWindow))
			{
				mMainScene.Dissolve();
				mMusic.stop();

				while (mMainScene.mDissolve && mMusic.isPlaying)
				{
					glClear(GL_COLOR_BUFFER_BIT);

					mMainScene.Render();
					mMusic.update();

					glfwPollEvents();
					glfwSwapBuffers(mWindow);
				}
				glfwSetWindowShouldClose(mWindow, true);
			}
		}
		else
		{
			if (isLoadSave)
			{
				if (mMenu.RenderTextButton(SaveSlotButton1, "Слот 1", WNDwidth/ 2.12, WNDheight/1.64, mWindow)) {
					if (LoadProgress(settings.gamePath, 1))
						RenderEscMenu = false;
				}

				if (mMenu.RenderTextButton(SaveSlotButton2, "Слот 2", WNDwidth / 2.12, WNDheight / 1.78, mWindow)) {
					if (LoadProgress(settings.gamePath, 2))
						RenderEscMenu = false;
				}

				if (mMenu.RenderTextButton(SaveSlotButton3, "Слот 3", WNDwidth / 2.12, WNDheight / 1.96, mWindow)) {
					if (LoadProgress(settings.gamePath, 3))
						RenderEscMenu = false;
				}

				if (mMenu.RenderTextButton(SaveSlotButton4, "Слот 4", WNDwidth / 2.12, WNDheight / 2.17, mWindow)) {
					if (LoadProgress(settings.gamePath, 4))
						RenderEscMenu = false;
				}

				if (mMenu.RenderTextButton(SaveSlotButton5, "Слот 5", WNDwidth / 2.12, WNDheight / 2.41, mWindow)) {
					if (LoadProgress(settings.gamePath, 5))
						RenderEscMenu = false;
				}

				if (mMenu.RenderTextButton(ReturnButton, "Назад", WNDwidth/2.11, WNDheight/2.75, mWindow)) {
					renderButtons = true;
				}
			}
			else
			{
				if (mMenu.RenderTextButton(SaveSlotButton1, "Слот 1", WNDwidth / 2.12, WNDheight / 1.64, mWindow)) {
					SaveProgress(settings.gamePath, 1);
				}

				if (mMenu.RenderTextButton(SaveSlotButton2, "Слот 2", WNDwidth / 2.12, WNDheight / 1.78, mWindow)) {
					SaveProgress(settings.gamePath, 2);
				}

				if (mMenu.RenderTextButton(SaveSlotButton3, "Слот 3", WNDwidth / 2.12, WNDheight / 1.96, mWindow)) {
					SaveProgress(settings.gamePath, 3);
				}

				if (mMenu.RenderTextButton(SaveSlotButton4, "Слот 4", WNDwidth / 2.12, WNDheight / 2.17, mWindow)) {
					SaveProgress(settings.gamePath, 4);
				}

				if (mMenu.RenderTextButton(SaveSlotButton5, "Слот 5", WNDwidth / 2.12, WNDheight / 2.41, mWindow)) {
					SaveProgress(settings.gamePath, 5);
				}

				if (mMenu.RenderTextButton(ReturnButton2, "Назад", WNDwidth / 2.11, WNDheight / 2.75, mWindow)) {
					renderButtons = true;
				}
			}
		}
	}

	void Application::NextStatement()
	{
		/*glfwWaitEvents();*/

		if (IsLoadSave)
		{
			if (it2 != it)
			{
				mSprites.clear();
				int temp = it;
				bool text = true;
				std::vector<std::string> spritesToDelete;
				std::vector<std::string> spritesToDisplay;
				bool soundStop = false;
				bool ambStop = false;
				IsLoadSave = false;
				temp--;
				bool t = false;
				int tit = 0;
				while (true)
				{
					if (temp < 0)
						break;

					switch (list[temp].command)
					{
					case CmdList::TEXT:
						if (text)
						{
							text = false;
							it = temp;
						}
						break;
					case CmdList::SCENE:
						if (!t)
						{
							mMainScene.Load(settings.gamePath + settings.imagePath + list[temp].content);
							t = true;
						}
					case CmdList::SHOWSPRITE:
					{
						if (!t)
						{
							spritesToDisplay.push_back(list[temp].content);
							Sprite tempSprite;
							tempSprite.Init();
							if (!spritesToDelete.empty())
							{
								for (size_t i = 0; i < spritesToDisplay.size(); i++)
								{
									if (i < spritesToDelete.size())
									{
										if (spritesToDelete[i] != spritesToDisplay[i + 1] && spritesToDelete[i] != spritesToDisplay[i + 2])
										{
											tempSprite.Load(settings.gamePath + settings.imagePath + list[temp].content);
											tempSprite.SetPosition(list[temp].posX, list[temp].poxY);
											mSprites[list[temp].content] = tempSprite;
										}
									}
									else
									{
										break;
									}
								}
							}
							else
							{
								tempSprite.Load(settings.gamePath + settings.imagePath + list[temp].content);
								tempSprite.SetPosition(list[temp].posX, list[temp].poxY);
								mSprites[list[temp].content] = tempSprite;
							}
							break;
						}
					}
					case CmdList::HIDESPRITE:
						spritesToDelete.push_back(list[temp].content);
						break;
					case CmdList::PLAYMUSIC:
						if (!soundStop)
						{
							mMusic.SoundEngine->stopAllSounds();
							std::string t = settings.gamePath + settings.soundPath + list[temp].content;
							mMusic.play(t.c_str());
						}
						break;
					case CmdList::PLAYSOUND:
						break;
					case CmdList::STOPMUSIC:
						mMusic.SoundEngine->stopAllSounds();
						soundStop = true;
						break;
					case CmdList::CHANGEBOX:
						mDialogueBox.SetBox(settings.gamePath + settings.imagePath + list[temp].content);
						goto end;
						break;
					case CmdList::CHANGEESCMENU:
						mMenu.ChangeImage(ESCImage, settings.gamePath + settings.imagePath + list[temp].content);
						break;
					case CmdList::PLAYAMBIENCE:
						if (!ambStop)
						{
							mAmbience.SoundEngine->stopAllSounds();
							std::string t = settings.gamePath + settings.soundPath + list[temp].content;
							mAmbience.play(t.c_str());
						}
						break;
					case CmdList::STOPAMBIENCE:
						ambStop = true;
						break;
					default:
						break;
					}
					temp--;
				}
			}
			else
			{
				IsLoadSave = false;
				return;
			}
		}

	end: {}

		if (it == list.size())
			return;

	start: {}

		switch (list[it].command)
		{
		case CmdList::TEXT:
			if (list[it].who != "")
			{
				content.name = list[it].who;
				content.text = list[it].what;
			}
			else
			{
				content.name = "";
				content.text = list[it].what;
			}
			break;
		case CmdList::SCENE:
			mMainScene.Load(settings.gamePath + settings.imagePath + list[it].content);
			mSprites.clear();
			it++;
			it2++;
			goto start;
		case CmdList::SHOWSPRITE:
		{
			Sprite tempSprite;
			tempSprite.Init();
			tempSprite.Load(settings.gamePath + settings.imagePath + list[it].content);
			tempSprite.SetPosition(list[it].posX, list[it].poxY);
			mSprites[list[it].content] = tempSprite;
			it++;
			it2++;
			goto start;
		}
		case CmdList::HIDESPRITE:
			mSprites.erase(mSprites.find(list[it].content));
			it++;
			it2++;
			goto start;
		case CmdList::PLAYMUSIC:
		{
			std::string t = settings.gamePath + settings.soundPath + list[it].content;
			mMusic.play(t.c_str());
			it++;
			it2++;
			goto start;
		}
		case CmdList::STOPMUSIC:
			mMusic.stop();
			it++;
			it2++;
			goto start;
		case CmdList::PLAYSOUND:
		{
			std::string t = settings.gamePath + settings.soundPath + list[it].content;
			mSound.play(t.c_str());
			it++;
			it2++;
			goto start;
		}
		case CmdList::STOPSOUND:
			mSound.stop();
			it++;
			it2++;
			goto start;
		case CmdList::CHANGEBOX:
			mDialogueBox.SetBox(settings.gamePath + settings.imagePath + list[it].content);
			it++;
			it2++;
			goto start;
		case CmdList::PLAYAMBIENCE:
		{
			std::string t = settings.gamePath + settings.soundPath + list[it].content;
			mAmbience.play(t.c_str());
			it++;
			it2++;
			goto start;
		}
		case CmdList::STOPAMBIENCE:
			mAmbience.stop();
			it++;
			it2++;
			goto start;
		case CmdList::CHANGEESCMENU:
			mMenu.ChangeImage(ESCImage, settings.gamePath + settings.imagePath + list[it].content);
			it++;
			it2++;
			goto start;
		case CmdList::RETURNTOMENU:
			mDrawStartMenu = true;
			mStartGame = false;
			drawbuttons = true;
			it = 0;
			it2 = 0;
			mMainScene.Load(settings.gamePath + settings.imagePath + "ui/bloodsplat_big.png");
			mMusic.play("game/sounds/music/Azimuth.ogg");
			glfwSetMouseButtonCallback(mWindow, nullptr);
			goto endsw;
		default:
			break;
		}
		it++;
		it2++;
	endsw: {}
	}

	void Application::InitWindow()
	{
		WindowsWindow tempWindow(&window);
		mWindow = tempWindow.GetWindow();
	}

	void Application::AppInput()
	{
	}

	void Application::text(std::string who, std::string what)
	{
		list.push_back({ CmdList::TEXT, " ", who, what, 0.0f, 0.0f });
	}
	void Application::text(std::string what)
	{
		list.push_back({ CmdList::TEXT,  " ",  " ", what, 0.0f, 0.0f });
	}
	void Application::scene(std::string path, bool dissolve)
	{
		list.push_back({ CmdList::SCENE, path,  " ",  " ", 0.0f, 0.0f });
	}
	void Application::showSprite(std::string path, float x, float y)
	{
		list.push_back({ CmdList::SHOWSPRITE, path,  " ",  " ", x, y });
	}
	void Application::hideSprite(std::string path)
	{
		list.push_back({ CmdList::HIDESPRITE, path,  " ",  " ", 0.0f, 0.0f });
	}
	void Application::playMusic(std::string path)
	{
		list.push_back({ CmdList::PLAYMUSIC, path,  " ",  " ", 0.0f, 0.0f });
	}
	void Application::stopMusic()
	{
		list.push_back({ CmdList::STOPMUSIC,  " ",  " ",  " ", 0.0f, 0.0f });
	}
	void Application::playSound(std::string path)
	{
		list.push_back({ CmdList::PLAYSOUND, path,  " ",  " ", 0.0f, 0.0f });
	}
	void Application::stopSound()
	{
		list.push_back({ CmdList::STOPSOUND,  " ",  " ",  " ", 0.0f, 0.0f });
	}
	void Application::playAmbience(std::string path)
	{
		list.push_back({ CmdList::PLAYAMBIENCE, path,  " ",  " ", 0.0f, 0.0f });
	}
	void Application::stopAmbience()
	{
		list.push_back({ CmdList::STOPAMBIENCE,  " ",  " ",  " ", 0.0f, 0.0f });
	}
	void Application::changeBox(std::string path)
	{
		list.push_back({ CmdList::CHANGEBOX, path, " ", " ", 0.0f, 0.0f });
	}
	void Application::changeESCMenu(std::string path)
	{
		list.push_back({ CmdList::CHANGEESCMENU, path, " ", " ", 0.0f, 0.0f });
	}
	void Application::returnToMenu()
	{
		list.push_back({ CmdList::RETURNTOMENU, " ", " ", " ", 0.0f, 0.0f });
	}
}