#ifndef gridwindowH
#define gridwindowH

#define CEL_INACTIVE 0.0f
#define CEL_HOVER 0.25f
#define CEL_SELECTED 0.5f
#define CEL_NA0 0.75f
#define CEL_TOTAL 4
#define CEL_SHEETSIZE 0.25f

#include "ObjectBuffer.h"

/*
cells in buffer
[01][11][21]
[00][10][20]
cells in vector
[3][4][5]
[0][1][2]
*/

class MGridBuffer: public MObjectBuffer
{
private:
	glm::vec2 GridOffset;
	glm::vec2 CellSize;
	unsigned int CellsCount[2];
	MObject* HoverCell;
	MObject* SelectedCell;
	vector<MObject*> Cells;//need because texture can change
	map<unsigned int, MObject*> Objects;//cell number, object link
	stTexture* pCellTexture;//inactive, active, selected, etc
	bool Visible;
	map<unsigned int, MObject*>::iterator it;
	
	void SetCellType(MObject* Cell, glm::vec2 UVStart);
	MObject* GetCell(int CellNumberX, int CellNumberY);
	MObject* GetCellByPoint(glm::vec2 Point);
public:
	MGridBuffer();
	~MGridBuffer();
	bool Initialize(glm::vec2 inCellSize, unsigned int CellsCountX, unsigned int CellsCountY, glm::vec2 inOffset, stTexture* inpCellTexture);
	bool AddObjectToCell(MObject* pObject, unsigned int CellNumberX, unsigned int CellNumberY);
	bool RemoveObjectFromCell(MObject* pObject);
	void OnMouseOver(float x, float y);
	void OnMouseClick(float x, float y);
	void SetVisible(bool Value);
	bool GetVisible();
	void MoveByOffset(glm::vec2 inOffset);
	void MoveToPoint(glm::vec2 inOffset);
	void DrawGrid();
	void CloseGrid();
};

#endif
