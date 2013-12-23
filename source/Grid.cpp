#include "Grid.h"
#include <OS.h>
#include <stdlib.h>
#include <stdio.h>
#include "TileView.h"

//#define TESTMODE 1

#define MKRAND 	uint8(float(rand()) / RAND_MAX * 10)

Tile::Tile(void)
 :	left(-1),
 	top(-1),
 	right(-1),
 	bottom(-1),
 	id(0),
 	lefttile(NULL),
 	toptile(NULL),
 	righttile(NULL),
 	bottomtile(NULL)
{
	SetValues(-1,-1,-1,-1);
	id=0;
}

Tile::Tile(const uint8 &l, const uint8 &t, const uint8 &r, const uint8 &b)
 :	left(l),
 	top(t),
 	right(r),
 	bottom(b),
 	id(0),
 	lefttile(NULL),
 	toptile(NULL),
 	righttile(NULL),
 	bottomtile(NULL)
{
	id=0;
}

Tile::Tile(const Tile &t)
 :	left(t.left),
 	top(t.top),
 	right(t.right),
 	bottom(t.bottom),
 	id(t.id),
 	lefttile(NULL),
 	toptile(NULL),
 	righttile(NULL),
 	bottomtile(NULL)
{
	id=t.id;
}

Tile &Tile::operator=(const Tile &t)
{
	SetValues(t.left,t.top,t.right,t.bottom);
	id=t.id;
	return *this;
}


void Tile::SetValues(const uint8 &l, const uint8 &t, const uint8 &r, const uint8 &b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

Grid::Grid(uint8 size)
{
	if(size == 0)
		debugger("Programmer Error: Grid has 0 tiles");
	
	fSize = size;
	
	for(uint8 row=0; row<fSize; row++)
	{
		uint16 index = row * fSize;
		for(uint8 col=0; col<fSize; col++)
		{
			Tile *tile = new Tile();
			fTiles.AddItem(tile);
			
			if(col == 0)
				tile->lefttile = NULL;
			else
				tile->lefttile = (Tile*)fTiles.ItemAt(index + col - 1);
			
			if(row == 0)
				tile->toptile = NULL;
			else
				tile->toptile = (Tile*)fTiles.ItemAt(index + col - fSize);
		}
	}
	uint16 arraysize = fSize * fSize;
	for(uint16 i=0; i<arraysize; i++)
	{
		Tile *tile = (Tile*)fTiles.ItemAt(i);
		if(tile->lefttile)
			tile->lefttile->righttile = tile;
		if(tile->toptile)
			tile->toptile->bottomtile = tile;
	}
}

Grid::~Grid(void)
{
	for(int32 i=0; i<fTiles.CountItems(); i++)
		delete (Tile*)fTiles.ItemAt(i);
	fTiles.MakeEmpty();
}

void Grid::GeneratePuzzle(void)
{
	Tile *tile;
	
	// Generate tiles and index when solved
	uint16 id=0;
	for(uint8 row=0; row<fSize; row++)
	{
		uint16 index = row * fSize;
		for(uint8 col=0; col<fSize; col++)
		{
			tile = (Tile*)fTiles.ItemAt(index + col);
			tile->left = tile->lefttile ? tile->lefttile->right : MKRAND;
			tile->top = tile->toptile ? tile->toptile->bottom : MKRAND;
			tile->right = MKRAND;
			tile->bottom = MKRAND;
			tile->id = id++;
		}
	}

#ifndef TESTMODE	
	// Now mix them all up
	uint16 arraysize = (fSize * fSize);
	uint16 count = arraysize * 4;
	for(uint8 i=0; i<count; i++)
	{
		// swap two random elements
		uint16 indexone = uint16(float(rand()) / RAND_MAX * arraysize);
		uint16 indextwo = uint16(float(rand()) / RAND_MAX * arraysize);
		
		fTiles.SwapItems(indexone,indextwo);
	}
#endif
}

Tile *Grid::TileAt(const uint16 &index)
{
	if(index > (fSize * fSize) - 1)
		debugger("Programmer Error: Tile index too big");
	
	return (Tile*)fTiles.ItemAt(index);
}

bool Grid::TryTile(Tile *src, Tile *dest)
{
	if(!src || !dest || src->IsEmpty())
		return false;

	
	// Return true if the thing is a valid play
	if(dest->lefttile && !dest->lefttile->IsEmpty())
	{
		if(dest->lefttile != src && dest->lefttile->right!=src->left)
			return false;
	}
	
	if(dest->toptile && !dest->toptile->IsEmpty())
	{
		if(dest->toptile != src && dest->toptile->bottom!=src->top)
			return false;
	}
	
	if(dest->righttile && !dest->righttile->IsEmpty())
	{
		if(dest->righttile != src && dest->righttile->left!=src->right)
		return false;
	}
	
	if(dest->bottomtile && !dest->bottomtile->IsEmpty())
	{
		if(dest->bottomtile != src && dest->bottomtile->top!=src->bottom)
			return false;
	}	
	return true;
}

bool Grid::IsSolved(void)
{
	// return true if the puzzle is solved
	for(int32 i=0; i<fTiles.CountItems(); i++)
	{
		Tile *t = (Tile*)fTiles.ItemAt(i);
		if(t->id != i || t->IsEmpty())
			return false;
	}
	return true;
}
