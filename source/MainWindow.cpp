#include "MainWindow.h"
#include <View.h>
#include <Menu.h>
#include <MenuItem.h>
#include <stdio.h>
#include <Alert.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <Roster.h>
#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include <LayoutBuilder.h>
#include <Box.h>
#include <FindDirectory.h>
#include "AboutWindow.h"
#include "Preferences.h"
#include "TileView.h"
#include "Grid.h"
#include "ImageAlert.h"

enum
{
	M_NEW_GAME='nwgm',
	
	M_SMALL_TILES,
	M_MEDIUM_TILES,
	M_LARGE_TILES,
	M_HUGE_TILES,
	
	M_SET_BACKGROUND,
	
	M_SET_TILE_COUNT_3,
	M_SET_TILE_COUNT_4,
	M_SET_TILE_COUNT_5,
	M_SET_TILE_COUNT_6,
	M_SET_TILE_COUNT_7,
	
	M_HOW_TO_PLAY,

	M_RESET_SCORES
};

MainWindow::MainWindow(void)
 :	BWindow(BRect(100,100,500,400),"BeVexed",B_TITLED_WINDOW_LOOK,
 	B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
 	fGrid(NULL),
	fWorkGrid(NULL),
	fBackPaths(2, true)
{
	BPath path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeVexed");

	fPrefPath = BString(path.Path()) << "/settings";
	
	BString* pathString = new BString(path.Path());
	pathString->Append("/backgrounds/");
	fBackPaths.AddItem(pathString);

	find_directory(B_SYSTEM_DATA_DIRECTORY, &path);
	path.Append("BeVexed/backgrounds");
	fBackPaths.AddItem(new BString(path.Path()));

	static const rgb_color beos_blue = {51,102,152,255};
	LoadPreferences(fPrefPath);
	
	if(gPreferences.FindInt8("gridsize",(int8*)&fGridSize)!=B_OK)
	{
		fGridSize = 3;
		gPreferences.AddInt8("gridsize",3);
	}
	
	if(gPreferences.FindInt8("tilesize",(int8*)&fTileSize)!=B_OK)
	{
		fTileSize = TILESIZE_MEDIUM;
		gPreferences.AddInt8("tilesize",TILESIZE_MEDIUM);
	}
	
	fMenuBar = new BMenuBar("menubar");

	BMenu *menu = new BMenu("Game");
	fMenuBar->AddItem(menu);
	
	menu->AddItem(new BMenuItem("New",new BMessage(M_NEW_GAME),'N'));
	
	BMenu *submenu = new BMenu("Difficulty");
	submenu->AddItem(new BMenuItem("Beginner",new BMessage(M_SET_TILE_COUNT_3)));
	submenu->AddItem(new BMenuItem("Easy",new BMessage(M_SET_TILE_COUNT_4)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_SET_TILE_COUNT_5)));
	submenu->AddItem(new BMenuItem("Hard",new BMessage(M_SET_TILE_COUNT_6)));
	submenu->AddItem(new BMenuItem("Master",new BMessage(M_SET_TILE_COUNT_7)));
	submenu->SetRadioMode(true);
	menu->AddSeparatorItem();
	menu->AddItem(submenu);
	
	BMenuItem *item = submenu->ItemAt(fGridSize - 3);
	item->SetMarked(true);
	
	submenu = new BMenu("Tile Size");
	submenu->AddItem(new BMenuItem("Small",new BMessage(M_SMALL_TILES)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_MEDIUM_TILES)));
	submenu->AddItem(new BMenuItem("Large",new BMessage(M_LARGE_TILES)));
	submenu->AddItem(new BMenuItem("Extra Large",new BMessage(M_HUGE_TILES)));
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);
	fBackMenu = new BMenu("Background");
	menu->AddItem(fBackMenu);
	ScanBackgrounds();
	
	switch(fTileSize)
	{
		case TILESIZE_SMALL:
		{
			submenu->ItemAt(0)->SetMarked(true);
			break;
		}
		case TILESIZE_MEDIUM:
		{
			submenu->ItemAt(1)->SetMarked(true);
			break;
		}
		case TILESIZE_LARGE:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
		case TILESIZE_HUGE:
		{
			submenu->ItemAt(3)->SetMarked(true);
			break;
		}
		default:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
	}
	
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Reset records",new BMessage(M_RESET_SCORES)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("How to Play…",new BMessage(M_HOW_TO_PLAY)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("About BeVexed…",new BMessage(B_ABOUT_REQUESTED)));

	fWorkGridLayout = new BGridLayout(5.0,5.0);
	fGridLayout = new BGridLayout(5.0,5.0);

	fTimer = new TimerView();
	BFont font = be_bold_font;
	font.SetSize(20);
	fTimer->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fTimer->SetFontAndColor(&font);
	fTimer->SetAlignment(B_ALIGN_CENTER);

	fHighScores = new BTextView("scores");
	font = be_bold_font;
	font.SetSize(14);
	fHighScores->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fHighScores->SetFontAndColor(&font);
	fHighScores->MakeEditable(false);
	fHighScores->SetAlignment(B_ALIGN_CENTER);

	BBox *scoreBox = new BBox("box");
	BBox *timeBox = new BBox("box");
	fLayout = BLayoutBuilder::Group<>(this,B_VERTICAL,0)
		.Add(fMenuBar)
		.AddGroup(B_HORIZONTAL,B_USE_DEFAULT_SPACING)
			.SetInsets(B_USE_WINDOW_INSETS)
			.Add(scoreBox)
			.AddGroup(B_HORIZONTAL,50.0)
				.Add(fWorkGridLayout)
				.Add(fGridLayout)
			.End()
		.End()
			.AddGroup(B_VERTICAL)
			.SetInsets(B_USE_WINDOW_INSETS)
			.Add(timeBox)
		.End();
	SetLayout(fLayout);

	BLayoutBuilder::Group<>(timeBox,B_HORIZONTAL,0)
			.SetInsets(B_USE_ITEM_INSETS)
			.Add(fTimer);
	timeBox->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BLayoutBuilder::Group<>(scoreBox,B_HORIZONTAL,0)
		.SetInsets(B_USE_ITEM_INSETS)
		.Add(fHighScores);

	GenerateGrid(fGridSize);

	BPoint corner;
	if(gPreferences.FindPoint("corner",&corner)!=B_OK)
	{
		corner = Frame().LeftTop();
		gPreferences.AddPoint("corner",Frame().LeftTop());
	}
	
	BRect r(Frame());
	r.OffsetTo(corner.x,corner.y);
	ConstrainWindowFrameToScreen(&r);
	MoveTo(r.left,r.top);

	if(gPreferences.FindString("background",&fBackName) == B_OK)
		SetBackground(fBackName.String());
}

bool MainWindow::QuitRequested(void)
{
	gPreferences.ReplacePoint("corner",Frame().LeftTop());
	SavePreferences(fPrefPath);
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void MainWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case B_ABOUT_REQUESTED:
		{
			AboutWindow *ab = new AboutWindow();
			ab->Show();
			break;
		}
		case M_NEW_GAME:
		{
			GenerateGrid(fGridSize);
			break;
		}
		case M_SMALL_TILES:
		{
			fTileSize = TILESIZE_SMALL;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_SMALL);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_MEDIUM_TILES:
		{
			fTileSize = TILESIZE_MEDIUM;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_MEDIUM);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_LARGE_TILES:
		{
			fTileSize = TILESIZE_LARGE;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_LARGE);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_HUGE_TILES:
		{
			fTileSize = TILESIZE_HUGE;
			gPreferences.ReplaceInt8("tilesize",TILESIZE_LARGE);
			TileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_3:
		{
			fGridSize = 3;
			gPreferences.ReplaceInt8("gridsize",3);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_4:
		{
			fGridSize = 4;
			gPreferences.ReplaceInt8("gridsize",4);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_5:
		{
			fGridSize = 5;
			gPreferences.ReplaceInt8("gridsize",5);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_6:
		{
			fGridSize = 6;
			gPreferences.ReplaceInt8("gridsize",6);
			GenerateGrid(fGridSize);
			break;
		}
		case M_SET_TILE_COUNT_7:
		{
			fGridSize = 7;
			gPreferences.ReplaceInt8("gridsize",7);
			GenerateGrid(fGridSize);
			break;
		}
		case M_RESET_SCORES:
		{
			for(int i = 3; i <=7; i++)
				gPreferences.RemoveName(BString("highscore-") << i);
			ReloadHighScores();
			break;
		}
		case M_SET_BACKGROUND:
		{
			BString name;
			if (msg->FindString("name",&name) == B_OK)
				SetBackground(name.String());
			else
				SetBackground(NULL);
			break;
		}
		case M_HOW_TO_PLAY:
		{
			BString string =
				"How to Play BeVexed:\n\n"
				"Place the tiles in the grid on the right into the "
				"grid on the left so that the numbers match wherever "
				"one tile touches another. Easy to learn, tough to master.\n";
			BAlert *help = new BAlert("BeVexed",string.String(),"OK");
			help->Go();
			break;
		}
		case M_CHECK_DROP:
		{
			if (!fTimer->Running())
				fTimer->Start();

			TileView *from, *to;
			if(msg->FindPointer("from",(void**)&from)!=B_OK ||
					msg->FindPointer("to",(void**)&to)!=B_OK)
				break;
					
			// If we're dropping to the storage grid
			if(fGrid->HasTile(to->GetTile()))
			{
				*to->GetTile() = *from->GetTile();
				from->GetTile()->MakeEmpty();
				to->Invalidate();
				from->Invalidate();
			}
			else
			// If we're dropping to the tile grid
			if(fWorkGrid->HasTile(to->GetTile()))
			{
				if(fWorkGrid->TryTile(from->GetTile(),to->GetTile()))
				{
					*to->GetTile() = *from->GetTile();
					from->GetTile()->MakeEmpty();
					to->Invalidate();
					from->Invalidate();
					
					if(fWorkGrid->IsSolved())
					{
						ImageAlert *alert = new ImageAlert("BeVexedYouWin.jpg",'JPEG');
						alert->Show();

						PushHighScore(fGridSize,fTimer->Elapsed());
						fTimer->Stop();

						GenerateGrid(fGridSize);
					}
				}
			}
			else
				debugger("Programmer Error: Orphaned Tile");
						
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}

void MainWindow::GenerateGrid(uint8 size)
{
	if(fGrid)
	{
		delete fGrid;
		delete fWorkGrid;
	}

	while(fGridLayout->CountItems()>0)
		fGridLayout->RemoveItem((int32)0);
	while(fWorkGridLayout->CountItems()>0)
		fWorkGridLayout->RemoveItem((int32)0);
	
	fGrid = new Grid(size);
	fGrid->GeneratePuzzle();
	fWorkGrid = new Grid(size);

	BPoint origin = BPoint(0,0);

	for(uint8 row=0; row<size; row++)
	{
		for(uint8 col=0; col<size; col++)
		{
			TileView *tile = new TileView(origin,fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fWorkGridLayout->AddView(tile,col,row);
			
			tile->SetTile(fWorkGrid->TileAt((size*row) + col));
		}
		
		for(uint8 col=0; col<size; col++)
		{
			TileView *tile = new TileView(origin,fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fGridLayout->AddView(tile,col,row);
			
			tile->SetTile(fGrid->TileAt((size*row) + col));
		}
		
	}

	BSize psize = fLayout->PreferredSize();
	ResizeTo(psize.width,psize.height);

	ReloadHighScores();
}

void MainWindow::ScanBackgrounds(void)
{
	while (fBackMenu->CountItems() > 0)
	{
		BMenuItem *item = fBackMenu->ItemAt(0L);
		delete item;
	}
	
	BMessage *msg = new BMessage(M_SET_BACKGROUND);
	msg->AddString("name","");
	fBackMenu->AddItem(new BMenuItem("None",msg));
	fBackMenu->AddSeparatorItem();
	
	for (int i = 0; i < fBackPaths.CountItems(); i++) {
		BDirectory dir(fBackPaths.ItemAt(i)->String());
		dir.Rewind();
		entry_ref ref;
		while (dir.GetNextRef(&ref) == B_OK)
		{
			BPath path(&ref);
			BString name(path.Leaf());
			int32 ext = name.FindLast(".");
			if (ext > 0)
				name.Truncate(ext);

			msg = new BMessage(M_SET_BACKGROUND);
			msg->AddString("name", path.Path());
			fBackMenu->AddItem(new BMenuItem(name.String(),msg));
		}
	}
}

void MainWindow::SetBackground(const char *name)
{
	BView *view = fLayout->View();
	if (!name || strlen(name) == 0)
	{
		view->ClearViewBitmap();
		view->Invalidate();
		fBackName = "";
		gPreferences.RemoveData("background");
		return;
	}
	
	BBitmap *bmp = BTranslationUtils::GetBitmapFile(BString(name));
	if (bmp)
	{
		view->SetViewBitmap(bmp);
		delete bmp;
		fBackName = name;
		gPreferences.RemoveData("background");
		gPreferences.AddString("background",fBackName);
		view->Invalidate();
	}
}

void MainWindow::PushHighScore(int grid, int score)
{
	BString key("highscore-");
	key << grid;
	for(int i = 0; i < 10; i++)
	{
		int32 val;
		if(gPreferences.FindInt32(key,i,&val)!=B_OK) {
			gPreferences.AddInt32(key,score);
			break;
		}

		if(val > score) {
			gPreferences.ReplaceInt32(key,i,score);
			score = val;
		}
	}
}

void MainWindow::ReloadHighScores()
{
	int32 val;
	BString key("highscore-");
	key << fGridSize;
	fHighScores->Delete(0,fHighScores->TextLength());
	for(int i = 0; gPreferences.FindInt32(key,i,&val)==B_OK; i++)
		fHighScores->Insert(BString().SetToFormat("%02d:%02d\n", val / 60, val % 60));
}
