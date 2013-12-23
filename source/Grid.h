#ifndef GRID_H
#define GRID_H

#include <List.h>
class TileView;

class Tile
{
public:
	Tile(void);
	Tile(const uint8 &l, const uint8 &t, const uint8 &r, const uint8 &b);
	Tile(const Tile &t);
	Tile &operator=(const Tile &t);
	
	void SetValues(const uint8 &l, const uint8 &t, const uint8 &r, const uint8 &b);
	
	bool IsEmpty(void) const { return (left<0 || right<0 || top<0 || bottom<0); }
	void MakeEmpty(void) { left=right=top=bottom=-1; }
	
	int8 left;
	int8 top;
	int8 right;
	int8 bottom;
	uint16 id;
	
	Tile *lefttile,*toptile,*righttile,*bottomtile;
};

class Grid
{
public:
	Grid(uint8 size);
	~Grid(void);
	
	uint8 Size(void) const { return fSize; }
	void GeneratePuzzle(void);
	Tile *TileAt(const uint16 &index);
	bool HasTile(Tile *t) { return fTiles.HasItem(t); }
	
	bool TryTile(Tile *src, Tile *dest);
	bool IsSolved(void);
private:
	BList fTiles;
	uint32 fSize;
};

#endif
