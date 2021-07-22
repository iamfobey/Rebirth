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
		mDialogueBox.namePosX = 170.0f;
		mDialogueBox.namePosY = 227.0f;

		mDialogueBox.textPosX = 167.0f;
		mDialogueBox.textPosY = 192.0f;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

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

		mMainScene.Load(settings.gamePath + settings.imagePath + "bg/1.jpg");

		StartButton = mMenu.CreateTextButton();
		LoadSaveButton = mMenu.CreateTextButton();
		SaveButton = mMenu.CreateTextButton();
		ExitButton = mMenu.CreateTextButton();
		SaveSlotButton1 = mMenu.CreateTextButton();
		SaveSlotButton2 = mMenu.CreateTextButton();
		SaveSlotButton3 = mMenu.CreateTextButton();
		SaveSlotButton4 = mMenu.CreateTextButton();
		SaveSlotButton5 = mMenu.CreateTextButton();
	}

	void Application::AppRender()
	{
		double xpos, ypos;

		glfwSetWindowFocusCallback(mWindow, [](GLFWwindow* window, int focused) {
			if (focused)
			{
				glfwSwapInterval(1);
			}
			else
			{
				glfwSwapInterval(-5);
			}
			});

		glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, 0.1f, 100.0f);

		while (!glfwWindowShouldClose(mWindow))
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glfwGetCursorPos(mWindow, &xpos, &ypos);

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

				mMusic.update();
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

	void Application::RenderMenu()
	{
	st: {}
		double xpos, ypos;

		glfwGetCursorPos(mWindow, &xpos, &ypos);

		mMainScene.Render();

		if (!mMainScene.mDissolve && !mStartGame)
		{
			if (mMenu.RenderTextButton(StartButton, "Начать", 750.0f, 500.0f, mWindow))
			{
				mStartGame = true;
				mMainScene.Dissolve();
			}

			if (mMenu.RenderTextButton(LoadSaveButton, "Загрузить", 730.0f, 440.0f, mWindow))
			{
				while (true)
				{
					glClear(GL_COLOR_BUFFER_BIT);
					glfwGetCursorPos(mWindow, &xpos, &ypos);
					mMenu.SetMousePos(xpos, ypos);

					mMainScene.Render();

					if (mMenu.RenderTextButton(SaveSlotButton1, "Слот 1", 757.0f, 550.0f, mWindow)) {
						if (LoadProgress(settings.gamePath, 1))
						{
							mDrawStartMenu = false;
							break;
						}
					}


					if (mMenu.RenderTextButton(SaveSlotButton2, "Слот 2", 757.0f, 510.0f, mWindow)) {
						if (LoadProgress(settings.gamePath, 2))
						{
							mDrawStartMenu = false;
							break;
						}
					}


					if (mMenu.RenderTextButton(SaveSlotButton3, "Слот 3", 757.0f, 470.0f, mWindow)) {
						if (LoadProgress(settings.gamePath, 3))
						{
							mDrawStartMenu = false;
							break;
						}
					}


					if (mMenu.RenderTextButton(SaveSlotButton4, "Слот 4", 757.0f, 430.0f, mWindow)) {
						if (LoadProgress(settings.gamePath, 4))
						{
							mDrawStartMenu = false;
							break;
						}
					}


					if (mMenu.RenderTextButton(SaveSlotButton5, "Слот 5", 757.0f, 390.0f, mWindow)) {
						if (LoadProgress(settings.gamePath, 5))
						{
							mDrawStartMenu = false;
							break;
						}
					}

					if (mMenu.RenderTextButton(ReturnButton, "Назад", 757.0f, 335.0f, mWindow)) {
						goto st;
					}

					if (glfwWindowShouldClose(mWindow))
					{
						break;
					}

					glfwPollEvents();
					glfwSwapBuffers(mWindow);
				}
				glfwSetKeyCallback(mWindow, key_callback);
			}

			if (mMenu.RenderTextButton(ExitButton, "Выход", 755.0f, 380.0f, mWindow))
			{
				mCloseWindow = true;
				mMainScene.Dissolve();
			}
		}

		mMenu.SetMousePos(xpos, ypos);

		if (mCloseWindow && !mMainScene.mDissolve)
		{
			glfwSetWindowShouldClose(mWindow, true);
		}

		if (mStartGame && !mMainScene.mDissolve)
		{
			mDrawStartMenu = false;
			glfwSetKeyCallback(mWindow, key_callback);
			NextStatement();
		}
	}

	void Application::RenderEscapeMenu()
	{
		mEscSprite.SetPosition(805, 535);
		mEscSprite.Render();

		if (renderButtons)
		{
			if (mMenu.RenderTextButton(LoadSaveButton, "Сохранить", 730.0f, 520.0f, mWindow))
			{
				renderButtons = false;
				isLoadSave = false;

				return;
			}

			if (mMenu.RenderTextButton(SaveButton, "Загрузить", 730.0f, 465.0f, mWindow))
			{
				renderButtons = false;
				isLoadSave = true;

				return;
			}

			if (mMenu.RenderTextButton(ReturnButton3, "Назад", 757.0f, 410.0f, mWindow))
			{
				RenderEscMenu = false;
			}

			if (mMenu.RenderTextButton(ExitButton, "Выйти", 757.0f, 355.0f, mWindow))
			{
				mMainScene.Dissolve();

				while (mMainScene.mDissolve)
				{
					glClear(GL_COLOR_BUFFER_BIT);

					mMainScene.Render();

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
				if (mMenu.RenderTextButton(SaveSlotButton1, "Слот 1", 757.0f, 550.0f, mWindow)) {
					if (LoadProgress(settings.gamePath, 1))
						RenderEscMenu = false;
				}
					

				if (mMenu.RenderTextButton(SaveSlotButton2, "Слот 2", 757.0f, 510.0f, mWindow)) {
					if (LoadProgress(settings.gamePath, 2))
						RenderEscMenu = false;
				}
					

				if (mMenu.RenderTextButton(SaveSlotButton3, "Слот 3", 757.0f, 470.0f, mWindow)) {
					if (LoadProgress(settings.gamePath, 3))
						RenderEscMenu = false;
				}
					

				if (mMenu.RenderTextButton(SaveSlotButton4, "Слот 4", 757.0f, 430.0f, mWindow)) {
					if (LoadProgress(settings.gamePath, 4))
						RenderEscMenu = false;
				}
					

				if (mMenu.RenderTextButton(SaveSlotButton5, "Слот 5", 757.0f, 390.0f, mWindow)) {
					if (LoadProgress(settings.gamePath, 5))
						RenderEscMenu = false;
				}

				if (mMenu.RenderTextButton(ReturnButton, "Назад", 757.0f, 335.0f, mWindow)) {
					renderButtons = true;
				}
			}
			else
			{
				if (mMenu.RenderTextButton(SaveSlotButton1, "Слот 1", 757.0f, 550.0f, mWindow)) {
					SaveProgress(settings.gamePath, 1);
				}

				if (mMenu.RenderTextButton(SaveSlotButton2, "Слот 2", 757.0f, 510.0f, mWindow)) {
					SaveProgress(settings.gamePath, 2);
				}

				if (mMenu.RenderTextButton(SaveSlotButton3, "Слот 3", 757.0f, 470.0f, mWindow)) {
					SaveProgress(settings.gamePath, 3);
				}

				if (mMenu.RenderTextButton(SaveSlotButton4, "Слот 4", 757.0f, 430.0f, mWindow)) {
					SaveProgress(settings.gamePath, 4);
				}

				if (mMenu.RenderTextButton(SaveSlotButton5, "Слот 5", 757.0f, 390.0f, mWindow)) {
					SaveProgress(settings.gamePath, 5);
				}

				if (mMenu.RenderTextButton(ReturnButton2, "Назад", 757.0f, 335.0f, mWindow)) {
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
				bool soundStop = false;
				IsLoadSave = false;
				temp--;
				bool t = false;
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
							Sprite tempSprite;
							tempSprite.Init();
							if (!spritesToDelete.empty())
							{
								for (uint32_t i = 0; i < spritesToDelete.size(); i++)
								{
									if (spritesToDelete[i] != list[temp].content)
									{
										tempSprite.Load(settings.gamePath + settings.imagePath + list[temp].content);
										tempSprite.SetPosition(list[it].posX, list[it].poxY);
										mSprites[list[temp].content] = tempSprite;
										break;
									}
								}
							}
							else
							{
								tempSprite.Load(settings.gamePath + settings.imagePath + list[temp].content);
								tempSprite.SetPosition(list[it].posX, list[it].poxY);
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
						if (!soundStop)
						{
							mSound.SoundEngine->stopAllSounds();
							std::string t = settings.gamePath + settings.soundPath + list[temp].content;
							mSound.play(t.c_str());
						}
						break;
					case CmdList::STOPMUSIC:
						soundStop = true;
						break;
					case CmdList::CHANGEBOX:
						mDialogueBox.SetBox(settings.gamePath + settings.imagePath + list[temp].content);
						goto end;
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
		default:
			break;
		}
		it++;
		it2++;
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
		list.push_back({ CmdList::TEXT, " ", who, what, 800, 500 });
	}
	void Application::text(std::string what)
	{
		list.push_back({ CmdList::TEXT,  " ",  " ", what, 800, 500 });
	}
	void Application::scene(std::string path, bool dissolve)
	{
		list.push_back({ CmdList::SCENE, path,  " ",  " ", 800, 500 });
	}
	void Application::showSprite(std::string path, float x, float y)
	{
		list.push_back({ CmdList::SHOWSPRITE, path,  " ",  " ", x, y });
	}
	void Application::hideSprite(std::string path) 
	{
		list.push_back({ CmdList::HIDESPRITE, path,  " ",  " ", 800, 500 });
	}
	void Application::playMusic(std::string path)
	{
		list.push_back({ CmdList::PLAYMUSIC, path,  " ",  " ", 800, 500 });
	}
	void Application::stopMusic()
	{
		list.push_back({ CmdList::STOPMUSIC,  " ",  " ",  " ", 800, 500 });
	}
	void Application::playSound(std::string path)
	{
		list.push_back({ CmdList::PLAYSOUND, path,  " ",  " ", 800, 500 });
	}
	void Application::stopSound()
	{
		list.push_back({ CmdList::STOPSOUND,  " ",  " ",  " ", 800, 500 });
	}
	void Application::changeBox(std::string path)
	{
		list.push_back({ CmdList::CHANGEBOX, path, " ", " ", 800, 500 });
	}
}