#include <Rebirth.h>

class App  :  public rb::Application
{
	void Init() override
	{
		window.Width = 1600;
		window.Height = 900;
		window.Title = "EVERLASTING CODING";

		settings.imagePath = "Assets/Images/";
		settings.fontPath = "Assets/Fonts/Font.ttf";
	}

	void Label() override
	{
		scene("2.jpg");		
		showSprite("1.png");
		playMusic("Assets/music.ogg");
		text("Здравствуйте дамы и господа! Вас приветствует команда новичков, что состоит из сценаристки (которая никогда не писала сценарии), художницы (она рисовала, она молодец), сценариста (что согласился помочь и спас мне этим жизнь) и моей подруги, что будет помогать по мере сил. Мы впервые делаем визуальную новеллу и не умеем кодить, ничего не знаем, но учимся непосредственно в процессе. Вот такой научный эксперимент. Сценарий, код, подбор музыки, разработка концепта персонажей - на мне. Моя прекрасная художница будет же визуализировать все эти идеи. Надеемся на вашу поддержку!");
		showSprite("2.png");
		text("Dezlow", "До начала урока оставалось около 20 минут, думаю я мог бы постараться, дабы успокоить Аяко… Ай, ладно, все равно ведь делать нечего. Погрузившись в записи, я перестал ощущать пространство вокруг себя, будто бы находился в вакууме и время шло быстрее обычного.Наверняка со стороны я");
		stopMusic();
		hideSprite("1.png");
		text("Dezlow", "TEST");
		scene("1.jpg");
		text("TEST2");
		text("|\"!@#$T^&%");
		scene("3.jpg");
		text("test");
	}
};
 
int main()
{
	try
	{
		App* p = new App;

		p->Run();

		delete p;
	}
	catch (const std::exception& ex)
	{
		spdlog::get("log")->error(ex.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}