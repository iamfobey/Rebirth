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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_S:
		if (action == GLFW_PRESS) SaveProgress();
		break;
	case GLFW_KEY_L:
		if (action == GLFW_PRESS) LoadProgress();
		break;
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) NextState = true;
	default:
		break;
	}
}

void SaveProgress()
{
	std::fstream file("save.data", std::ios::out | std::ios_base::trunc);

	if (!file.is_open())
		throw std::runtime_error("Error saving progress: Could not open file save.data!");

	file << it;
}

void LoadProgress()
{
	std::fstream file("save.data", std::ios::in);

	if (!file.is_open())
		throw std::runtime_error("Error loading progress: Could not open file save.data!");

	char buff[50];
	file.read(buff, 50);
	it = std::atoi(buff);
	IsLoadSave = true;
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
		mLogger.Init();
		InitWindow();
		mMainScene.Init();
		mDialogueBox.Init("Assets/Images/textbox.png", settings.fontPath, window.Width, window.Height);
		mDialogueBox.namePosX = 217.0f;
		mDialogueBox.namePosY = 197.0f;

		mDialogueBox.textPosX = 222.0f;
		mDialogueBox.textPosY = 160.0f;

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
		io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 13.5f, &font_config, ranges);
	}

	void Application::AppRender()
	{
		glfwSetKeyCallback(mWindow, key_callback);
		NextStatement();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		double ox, oy, oz;

		double xpos, ypos;

		float vol = 1.0;

		/*glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				if (action == GLFW_PRESS & button == GLFW_MOUSE_BUTTON_LEFT)
				{
					NextState = true;
				}
			});*/
		
		glfwSetWindowFocusCallback(mWindow, [](GLFWwindow* window, int focused) {
			if (focused)
			{
				glfwSwapInterval(1);
			}
			else
			{
				glfwSwapInterval(-7);
			}
			});

		while (!glfwWindowShouldClose(mWindow))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			if (NextState) NextStatement(); NextState = false;

			glfwGetCursorPos(mWindow, &xpos, &ypos);

			glu::ClientToGL(xpos, ypos, &ox, &oy, &oz);

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

			mMusic.update();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Debug Info");
			{
				ImGui::Text("FPS: %.f", io.Framerate);

				ImGui::Separator();

				if (ImGui::Button("Load Save"))
					LoadProgress();

				if (ImGui::Button("Save Progress"))
					SaveProgress();

				ImGui::Text("Iterator: %i", it);

				ImGui::Text("Cursor X: %.f", xpos);
				ImGui::Text("Cursor Y: %.f", ypos);

				ImGui::Text("OGL X: %f", ox);
				ImGui::Text("OGL Y: %f", oy);
				ImGui::Text("OGL Z: %f", ox);

				if (ImGui::DragFloat("Sound volume", &vol, 0.005f, 0, 1.0))
					mSound.SetVolume(vol);

				if (ImGui::Button("Stop music"))
					mSound.stop();

				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwPollEvents();

			glfwSwapBuffers(mWindow);
		}
	}

	void Application::AppClose()
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
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
					case CmdList::IMAGE:
						mMainScene.Load(settings.imagePath + list[temp].content);
						goto end;
					case CmdList::SHOWSPRITE:
					{
						Sprite tempSprite;
						tempSprite.Init();
						if (!spritesToDelete.empty())
						{
							for (uint32_t i = 0; i < spritesToDelete.size(); i++)
							{
								if (spritesToDelete[i] != list[temp].content)
								{
									tempSprite.Load(settings.imagePath + list[temp].content);
									mSprites[list[temp].content] = tempSprite;
									break;
								}
							}
						}
						else
						{
							tempSprite.Load(settings.imagePath + list[temp].content);
							mSprites[list[temp].content] = tempSprite;
						}
						break;
					}
					case CmdList::HIDESPRITE:
						spritesToDelete.push_back(list[temp].content);
						break;
					case CmdList::PLAYMUSIC:
						mMusic.SoundEngine->stopAllSounds();
						mMusic.play(list[temp].content.c_str());
						break;
					case CmdList::PLAYSOUND:
						mSound.SoundEngine->stopAllSounds();
						mSound.play(list[temp].content.c_str());
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
		case CmdList::IMAGE:
			mMainScene.Load(settings.imagePath + list[it].content);
			mSprites.clear();
			it++;
			it2++;
			goto start;
		case CmdList::SHOWSPRITE:
		{
			Sprite tempSprite;
			tempSprite.Init();
			tempSprite.Load(settings.imagePath + list[it].content);
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
			mMusic.play(list[it].content.c_str());
			it++;
			it2++;
			goto start;
		case CmdList::STOPMUSIC:
			mMusic.stop();
			it++;
			it2++;
			goto start;
		case CmdList::PLAYSOUND:
			mSound.play(list[it].content.c_str());
			it++;
			it2++;
			goto start;
		case CmdList::STOPSOUND:
			mSound.stop();
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
}