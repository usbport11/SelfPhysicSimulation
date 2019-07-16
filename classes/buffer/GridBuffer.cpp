#include "../../stdafx.h"
#include "GridBuffer.h"

MGridBuffer::MGridBuffer():MObjectBuffer()
{
	GridOffset = glm::vec2(0, 0);
	CellSize = glm::vec2(0, 0);
	CellsCount[0] = CellsCount[1] = 0;
	pCellTexture = NULL;
	Visible = false;
	HoverCell = NULL;
	SelectedCell = NULL;
}

MGridBuffer::~MGridBuffer()
{
	GridOffset = glm::vec2(0, 0);
	CellSize = glm::vec2(0, 0);
	CellsCount[0] = CellsCount[1] = 0;
	pCellTexture = NULL;
	Visible = false;
	HoverCell = NULL;
	SelectedCell = NULL;
}

bool MGridBuffer::Initialize(glm::vec2 inCellSize, unsigned int CellsCountX, unsigned int CellsCountY, glm::vec2 inGridOffset, stTexture* inpCellTexture)
{
	if(!CellsCountX || !CellsCountY) return false;
	if(!inpCellTexture) return false;
	if(inCellSize.x < 0.16f || inCellSize.y < 0.16f) return false;
	
	CellSize = inCellSize,
	CellsCount[0] = CellsCountX;
	CellsCount[1] = CellsCountY;
	GridOffset = inGridOffset;
	pCellTexture = inpCellTexture;
	
	//MObject Cell;
	stQuad Quad;
	int Offset[2];
	for(int i=0; i<CellsCountX * CellsCountY; i++)
	{
		Offset[1] = (i / CellsCountX);
		Offset[0] = i - Offset[1] * CellsCountX;
		Cells.push_back(new MObject);
		if(!AddObject(Cells[i], GridOffset.x + Offset[0] * CellSize.x, GridOffset.y + Offset[1] * CellSize.y, CellSize.x, CellSize.y, 0 + CEL_INACTIVE, 0, CEL_SHEETSIZE, 1, pCellTexture->Id)) return false;
	}
	if(!DisposeAll()) return false;
	
	return true;
}

bool MGridBuffer::AddObjectToCell(MObject* pObject, unsigned int CellNumberX, unsigned int CellNumberY)
{
	if(!pObject) return false;
	if(CellNumberX >= CellsCount[0] || CellNumberY >= CellsCount[1]) return false;
	//test that object not added before
	it = Objects.find(CellNumberY * CellsCount[0] + CellNumberX);
	if(it != Objects.end()) return false;
	//change vertex of adding object
	MObject* pCell = GetCell(CellNumberX, CellNumberY);
	if(!pCell) return false;
	pObject->SetVertex(pCell->GetVertex());
	if(AddObject(pObject)) return false;
	//add link in map
	Objects.insert(pair<unsigned int, MObject*>(CellNumberY * CellsCount[1] + CellNumberX, pObject));
	if(!DisposeAll()) return false;
	
	return true;
}

bool MGridBuffer::RemoveObjectFromCell(MObject* pObject)
{
	if(!pObject) return false;
	for(it=Objects.begin(); it!=Objects.end(); ++it)
	{
		if(it->second == pObject)
		{
			if(!RemoveObject(pObject)) return false;
			Objects.erase(it);
			return true;
		}
	}
	
	return true;
}

void MGridBuffer::OnMouseOver(float x, float y)
{
	if(!Visible) return;
	if(!Cells.size()) return;
	//find cell
	MObject* Cell = GetCellByPoint(glm::vec2(x, y));
	if(Cell == HoverCell) return;
	else
	{
		SetCellType(HoverCell, glm::vec2(CEL_INACTIVE, 0));
		HoverCell = Cell;
		if(Cell) SetCellType(Cell, glm::vec2(CEL_HOVER, 0));
	}
}

void MGridBuffer::OnMouseClick(float x, float y)
{
	if(!Visible) return;
	if(!Cells.size()) return;
	//find cell
	MObject* Cell = GetCellByPoint(glm::vec2(x, y));
	if(Cell == HoverCell) return;
	else
	{
		SetCellType(HoverCell, glm::vec2(CEL_INACTIVE, 0));
		HoverCell = Cell;
		if(Cell) SetCellType(Cell, glm::vec2(CEL_SELECTED, 0));
	}
}

void MGridBuffer::MoveByOffset(glm::vec2 Offset)
{
	GridOffset += Offset;
	for(int i=0; i<Cells.size(); i++)
	{
		Cells[i]->MoveVertex(Offset);
	}
	for(it=Objects.begin(); it!=Objects.end(); ++it)
	{
		it->second->MoveVertex(Offset);
	}
	UpdateAll();
}

void MGridBuffer::MoveToPoint(glm::vec2 Point)
{
	MoveByOffset(Point - GridOffset);
}

void MGridBuffer::SetVisible(bool Value)
{
	Visible = Value;
}

bool MGridBuffer::GetVisible()
{
	return Visible;
}

void MGridBuffer::SetCellType(MObject* Cell, glm::vec2 UVStart)
{
	if(!Cell) return;
	stQuad UV;
	SetQuad(UV, UVStart.x + pCellTexture->uvTilePosOffset.x, UVStart.y + pCellTexture->uvTilePosOffset.y, CEL_SHEETSIZE - pCellTexture->uvTileSizeOffset.x, 1 - pCellTexture->uvTileSizeOffset.y);
	Cell->SetUV(UV);
	UpdateObject(Cell);
}

MObject* MGridBuffer::GetCell(int CellNumberX, int CellNumberY)
{
	if(CellNumberX >= CellsCount[0] || CellNumberY >= CellsCount[1]) return NULL;
	return Cells[CellNumberY * CellsCount[1] + CellNumberX];
}

MObject* MGridBuffer::GetCellByPoint(glm::vec2 Point)
{
	glm::vec2 PreCellNumber = glm::vec2((Point.x - GridOffset.x) / CellSize.x, (Point.y - GridOffset.y) / CellSize.y);
	if(PreCellNumber.x < 0 || PreCellNumber.y < 0) return NULL;
	else return GetCell((int)PreCellNumber.x, (int)PreCellNumber.y);
}

void MGridBuffer::DrawGrid()
{
	if(Visible) DrawAll();
}

void MGridBuffer::CloseGrid()
{
	Close();
	for(int i=0; i<Cells.size(); i++)
	{
		if(Cells[i]) delete Cells[i];
	}
	Cells.clear();
	Objects.clear();
}
