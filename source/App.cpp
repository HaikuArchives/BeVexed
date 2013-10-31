#include <Application.h>
#include "MainWindow.h"
#include <OS.h>
#include <stdlib.h>

class App : public BApplication
{
public:
	App(void);
};

App::App(void)
 : BApplication("application/x-vnd.wgp-BeVexed")
{
	MainWindow *mainwin = new MainWindow();
	mainwin->Show();
}

int main(void)
{
	srand(system_time());
	App app;
	app.Run();
	return 0;
}