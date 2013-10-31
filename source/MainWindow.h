#ifndef MAINWIN_H
#define MAINWIN_H

#include <Application.h>
#include <Window.h>
#include <MenuBar.h>

class Grid;

class MainWindow : public BWindow
{
public:
	MainWindow(void);
	bool QuitRequested(void);
	void MessageReceived(BMessage *msg);

private:
	void GenerateGrid(uint8 size);
	Grid *fGrid, *fWorkGrid;
	BView *fBack;
	BMenuBar *fMenuBar;
	uint8 fGridSize;
	uint8 fTileSize;
};

#endif
