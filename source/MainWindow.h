#ifndef MAINWIN_H
#define MAINWIN_H

#include <Application.h>
#include <Window.h>
#include <MenuBar.h>
#include <String.h>
#include <GroupLayout.h>
#include <GridLayout.h>

class Grid;

class MainWindow : public BWindow
{
public:
			MainWindow(void);
	bool	QuitRequested(void);
	void	MessageReceived(BMessage *msg);
	
private:
	void	GenerateGrid(uint8 size);
	void	ScanBackgrounds(void);
	void	SetBackground(const char *name);
	
	Grid *fGrid, *fWorkGrid;
	BGroupLayout *fLayout;
	BGridLayout *fGridLayout, *fWorkGridLayout;
	BMenuBar *fMenuBar;
	BMenu *fBackMenu;
	uint8 fGridSize;
	uint8 fTileSize;
	
	BString fBackPath;
	BString fBackName;
};

#endif
