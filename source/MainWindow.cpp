#include "MainWindow.h"
#include <View.h>
#include <Menu.h>
#include <MenuItem.h>
#include <stdio.h>
#include <Alert.h>
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
	
	M_SET_TILE_COUNT_3,
	M_SET_TILE_COUNT_4,
	M_SET_TILE_COUNT_5,
	M_SET_TILE_COUNT_6,
	M_SET_TILE_COUNT_7,
	
	M_HOW_TO_PLAY
};

MainWindow::MainWindow(void)
 :	BWindow(BRect(100,100,500,400),"BeVexed",B_TITLED_WINDOW_LOOK,
 	B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
 	fGrid(NULL),
 	fWorkGrid(NULL)
{
	LoadPreferences(PREFERENCES_PATH);
	
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
	
	fBack = new BView(Bounds(),"background",B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(fBack);
	fBack->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	fMenuBar = new BMenuBar(BRect(0,0,Bounds().Width(),20),"menubar");
	fBack->AddChild(fMenuBar);
	
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
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);
	
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
		default:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
	}
	
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("How to Play…",new BMessage(M_HOW_TO_PLAY)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("About BeVexed…",new BMessage(B_ABOUT_REQUESTED)));
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
}

bool MainWindow::QuitRequested(void)
{
	gPreferences.ReplacePoint("corner",Frame().LeftTop());
	SavePreferences(PREFERENCES_PATH);
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
	
	fBack->RemoveChild(fMenuBar);
	while(fBack->CountChildren()>0)
	{
		BView *child = fBack->ChildAt(0);
		child->RemoveSelf();
		delete child;
	}
	fBack->AddChild(fMenuBar);
	
	fGrid = new Grid(size);
	fGrid->GeneratePuzzle();
	fWorkGrid = new Grid(size);
	
	ResizeTo( ((fTileSize+5) * size * 2) + 70,
				((fTileSize+5) * size) + fMenuBar->Frame().Height() + 20 );
	
	BRect r(10,fMenuBar->Frame().bottom + 10,
			10 + fTileSize,10 + fMenuBar->Frame().bottom + fTileSize);
	
	for(uint8 row=0; row<size; row++)
	{
		for(uint8 col=0; col<size; col++)
		{
			TileView *tile = new TileView(r.LeftTop(),fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fBack->AddChild(tile);
			r.OffsetBy(fTileSize + 5,0);
			
			tile->SetTile(fWorkGrid->TileAt((size*row) + col));
		}
		
		r.OffsetBy(50,0);
		for(uint8 col=0; col<size; col++)
		{
			TileView *tile = new TileView(r.LeftTop(),fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fBack->AddChild(tile);
			r.OffsetBy(fTileSize + 5,0);
			
			tile->SetTile(fGrid->TileAt((size*row) + col));
		}
		
		r.OffsetBy(-(r.left - 10),fTileSize+5);
	}
}
