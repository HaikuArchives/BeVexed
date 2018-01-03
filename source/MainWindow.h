#ifndef MAINWIN_H
#define MAINWIN_H

#include <Application.h>
#include <Window.h>
#include <MenuBar.h>
#include <String.h>
#include <GroupLayout.h>
#include <GridLayout.h>
#include <ObjectList.h>
#include "TimerView.h"

class Grid;

class MainWindow : public BWindow
{
public:
			MainWindow(void);
	bool	QuitRequested(void);
	void	MessageReceived(BMessage *msg);
	
private:
	void	GenerateGrid(uint8 size);
	void  DrawGrid(void);
	void	ScanBackgrounds(void);
	void	SetBackground(const char *name);
	void	PushHighScore(int grid, int score);
	void	ReloadHighScores();
	
	Grid *fGrid, *fWorkGrid;
	BGroupLayout *fLayout;
	BGridLayout *fGridLayout, *fWorkGridLayout;
	BMenuBar *fMenuBar;
	BMenu *fBackMenu;
	TimerView *fTimer;
	BTextView *fHighScores;
	uint8 fGridSize;
	uint8 fTileSize;
	
	BObjectList<BString> fBackPaths;
	BString fPrefPath;
	BString fBackName;
};

#endif
