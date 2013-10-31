#include "TileView.h"
#include <Font.h>
#include <Bitmap.h>
#include <String.h>
#include <Message.h>
#include <Window.h>

static bool sInit=false;

static float sFontHeight = 0;
static float sFontWidth = 0;
static float sFontSize = 0;

static BPoint sLeftPoint;
static BPoint sTopPoint;
static BPoint sRightPoint;
static BPoint sBottomPoint;

void TileView::CalcLayout(uint8 tilesize)
{
	sInit = true;
	
	font_height fh;
	be_bold_font->GetHeight(&fh);
	sFontHeight = fh.ascent + fh.descent + fh.leading;
	
	sFontWidth = be_bold_font->StringWidth("0");

	switch(tilesize)
	{
		case TILESIZE_SMALL:
		{
			sFontSize = 14;
			break;
		}
		case TILESIZE_MEDIUM:
		{
			sFontSize = 16;
			break;
		}
		case TILESIZE_LARGE:
		{
			sFontSize = 20;
			break;
		}
		default:
		{
			tilesize = TILESIZE_MEDIUM;
			sFontSize = 16;
		}
	}

	sLeftPoint.x = (tilesize / 4) - sFontWidth;
	sLeftPoint.y = (tilesize + sFontHeight) / 2;
	
	sTopPoint.x = (tilesize - sFontWidth) / 2;
	sTopPoint.y = (tilesize / 4) + (sFontHeight / 2);
	
	sRightPoint.x = (tilesize * .75);
	sRightPoint.y = sLeftPoint.y;
	
	sBottomPoint.x = sTopPoint.x;
	sBottomPoint.y = (tilesize * .75) + (sFontHeight / 2);
}

TileView::TileView(const BPoint &pt, uint8 tilesize, const char *name, const int32 &resize,
					const int32 &flags)
 :	BView(BRect(pt.x,pt.y,pt.x+tilesize,pt.y+tilesize),name,resize,flags),
 	fMouseDown(false),
 	fTile(NULL)
{
	if(tilesize!=TILESIZE_SMALL && tilesize!=TILESIZE_MEDIUM && tilesize!=TILESIZE_LARGE)
		tilesize = TILESIZE_MEDIUM;
	
	if(!sInit)
		CalcLayout(tilesize);
	
	BFont font(be_bold_font);
	font.SetSize(sFontSize);
	SetFont(&font);
}

void TileView::Draw(BRect r)
{
	DrawTile(this,Bounds());
}

void DrawTile(TileView *owner, BRect r)
{
	rgb_color highlight = {255,255,255,255};
	rgb_color shadow = {90,90,90,255};
	rgb_color base = {224,224,224,255};
	
	owner->SetHighColor(base);
	owner->FillRect(owner->Bounds());
	
	if(owner->GetTile()->IsEmpty())
	{
		// Draw empty square
		owner->BeginLineArray(4);
		owner->SetHighColor(shadow);
		owner->StrokeLine(r.LeftTop(),r.LeftBottom());
		owner->StrokeLine(r.LeftTop(),BPoint(r.right-1,r.top));
		owner->SetHighColor(highlight);
		owner->StrokeLine(r.RightBottom(),r.RightTop());
		owner->StrokeLine(r.RightBottom(),BPoint(r.left+1,r.bottom));
		owner->EndLineArray();
		return;
	}
	
	owner->BeginLineArray(6);
	
	// frame
	owner->SetHighColor(highlight);
	owner->StrokeLine(r.LeftTop(),r.LeftBottom());
	owner->StrokeLine(r.LeftTop(),BPoint(r.right-1,r.top));
	owner->SetHighColor(shadow);
	owner->StrokeLine(r.RightBottom(),r.RightTop());
	owner->StrokeLine(r.RightBottom(),BPoint(r.left+1,r.bottom));
	
	// dividers
	
	r.InsetBy(10,10);
	owner->SetHighColor(shadow);
	owner->StrokeLine(r.LeftTop(),r.RightBottom());
	owner->StrokeLine(r.LeftBottom(),r.RightTop());
	
	owner->EndLineArray();
	
	// Numbers
	owner->SetHighColor(0,0,0);
	BString string;
	
	string = "";
	string << (int)owner->GetTile()->left;
	owner->DrawString(string.String(),sLeftPoint);
	
	string = "";
	string << (int)owner->GetTile()->top;
	owner->DrawString(string.String(),sTopPoint);
	
	string = "";
	string << (int)owner->GetTile()->right;
	owner->DrawString(string.String(),sRightPoint);
	
	string = "";
	string << (int)owner->GetTile()->bottom;
	owner->DrawString(string.String(),sBottomPoint);
}

void TileView::SetTile(Tile *tile)
{
	fTile = tile;
	Invalidate();
}

void TileView::MouseDown(BPoint pt)
{
	fMouseDown = true;
	
	if(!fTile->IsEmpty())
		DoDrag();
}

void TileView::MouseMoved(BPoint pt,uint32 code, const BMessage *msg)
{
}

void TileView::MouseUp(BPoint pt)
{
	fMouseDown = false;
}

void TileView::MessageReceived(BMessage *msg)
{
	if(msg->WasDropped())
	{
		TileView *view;
		if(msg->FindPointer("view",(void**)&view)!=B_OK)
			return;
		
		if(!fTile->IsEmpty())
			return;
		
		// Well, we've gotten this far, so this tile must be empty and the other one not.
		// Now we actually have to do some game logic. We can only do this from the
		// hosting window, so post a message to determine the logic
		if(Window())
		{
			BMessage post(M_CHECK_DROP);
			post.AddPointer("from",view);
			post.AddPointer("to",this);
			Window()->PostMessage(&post);
		}
		return;
	}
	
	BView::MessageReceived(msg);
}

void TileView::DoDrag(void)
{
	BBitmap *bitmap = new BBitmap(Bounds(),B_RGB32, true);
	TileView *view = new TileView(BPoint(0,0),(uint8)Bounds().IntegerWidth(),NULL,0,0);
	view->SetTile(GetTile());
	bitmap->AddChild(view);
	bitmap->Lock();
	
	BFont font;
	GetFont(&font);
	view->SetFont(&font);
	DrawTile(view,Bounds());
	view->Sync();
	bitmap->Unlock();
	
	BMessage msg(B_SIMPLE_DATA);
	msg.AddPointer("view",this);
	DragMessage(&msg,bitmap,B_OP_BLEND,BPoint(Bounds().Width()/2,Bounds().Height()/2));
}
