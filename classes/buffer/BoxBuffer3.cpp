#include "../../stdafx.h"
#include "BoxBuffer3.h"

MBoxBuffer3::MBoxBuffer3():MStaticBuffer()
{
	TextureSize = glm::vec2(0, 0);
	UnitSize = glm::vec2(0, 0);
	LastEnd = 1;
	SliceCount = 0;
}

MBoxBuffer3::~MBoxBuffer3()
{
	TextureSize = glm::vec2(0, 0);
	UnitSize = glm::vec2(0, 0);
	LastEnd = 0;
	SliceCount = 0;
}

void MBoxBuffer3::Close()
{
	Type_Offset.clear();
	Type_Tempates.clear();
	MStaticBuffer::Close();
}

bool MBoxBuffer3::Initialize(stTexture* inpTexture, glm::vec2 inUnitSize)
{
	if(!MStaticBuffer::Initialize(inpTexture)) return false;
	
	TextureSize.x = (float) 1 / TXP_TOTAL;
	TextureSize.y = 1;//this can be changed while more line types
	
	if(inUnitSize.x < 0.16f || inUnitSize.y < 0.16f) return false;
	UnitSize = inUnitSize;
	
	Type_Offset.insert(pair<unsigned char, const float>(0, TXP_LBA));
	Type_Offset.insert(pair<unsigned char, const float>(1, TXP_LTA));
	Type_Offset.insert(pair<unsigned char, const float>(2, TXP_RTA));
	Type_Offset.insert(pair<unsigned char, const float>(3, TXP_RBA));
	Type_Offset.insert(pair<unsigned char, const float>(4, TXP_LBC));
	Type_Offset.insert(pair<unsigned char, const float>(5, TXP_LTC));
	Type_Offset.insert(pair<unsigned char, const float>(6, TXP_RTC));
	Type_Offset.insert(pair<unsigned char, const float>(7, TXP_RBC));
	Type_Offset.insert(pair<unsigned char, const float>(8, TXP_BF));
	Type_Offset.insert(pair<unsigned char, const float>(9, TXP_LF));
	Type_Offset.insert(pair<unsigned char, const float>(10, TXP_TF));
	Type_Offset.insert(pair<unsigned char, const float>(11, TXP_RF));
	Type_Offset.insert(pair<unsigned char, const float>(12, TXP_CF));
	Type_Offset.insert(pair<unsigned char, const float>(13, TXP_NA0));
	Type_Offset.insert(pair<unsigned char, const float>(14, TXP_NA2));
	Type_Offset.insert(pair<unsigned char, const float>(15, TXP_NA1));
	
	return true;
}

void MBoxBuffer3::FillUnits(glm::vec2 Pos, unsigned int SizeX, unsigned SizeY, glm::vec2 texStart)
{
	AddUVQuad(Pos.x * UnitSize.x, Pos.y * UnitSize.y, SizeX * UnitSize.x, SizeY * UnitSize.y, 
		texStart.x + pTexture->uvTilePosOffset.x, texStart.y + pTexture->uvTilePosOffset.y, TextureSize.x - pTexture->uvTileSizeOffset.x, TextureSize.y - pTexture->uvTileSizeOffset.y);
}

void MBoxBuffer3::FillVerticalUnits(glm::vec2 Pos, unsigned int Size, glm::vec2 texStart)
{
	for(unsigned int i=0; i<Size; i++)
	{
		AddUVQuad(Pos[0] * UnitSize.x, (Pos[1] + i) * UnitSize.y, UnitSize.x, UnitSize.y, 
			texStart.x + pTexture->uvTilePosOffset.x, texStart.y + pTexture->uvTilePosOffset.y, TextureSize.x - pTexture->uvTileSizeOffset.x, TextureSize.y - pTexture->uvTileSizeOffset.y);
	}
}

void MBoxBuffer3::FillHorizontalUnits(glm::vec2 Pos, unsigned int Size, glm::vec2 texStart)
{
	for(unsigned int i=0; i<Size; i++)
	{
		AddUVQuad((Pos[0] + i) * UnitSize.x, Pos[1] * UnitSize.y, UnitSize.x, UnitSize.y, 
			texStart.x + pTexture->uvTilePosOffset.x, texStart.y + pTexture->uvTilePosOffset.y, TextureSize.x - pTexture->uvTileSizeOffset.x, TextureSize.y - pTexture->uvTileSizeOffset.y);
	}
}

void MBoxBuffer3::AddOneQuad(glm::vec2 Pos, glm::vec2 texStart)
{
	FillUnits(Pos, 1, 1, texStart);
}

void MBoxBuffer3::AddFullBox(glm::vec2 Pos, unsigned int Size[2])
{
	if(Size[0] < 3 || Size[1] < 3) return;
	//left bottom
	FillUnits(glm::vec2(Pos.x, Pos.y), 1, 1, (glm::vec2){TXP_LBA, 0});
	//left top
	FillUnits(glm::vec2(Pos.x, Pos.y + Size[1] - 1), 1, 1, (glm::vec2){TXP_LTA, 0});
	//right top
	FillUnits(glm::vec2(Pos.x + Size[0] - 1, Pos.y + Size[1] - 1), 1, 1, (glm::vec2){TXP_RTA, 0});
	//right bottom
	FillUnits(glm::vec2(Pos.x + Size[0] - 1, Pos.y), 1, 1, (glm::vec2){TXP_RBA,0});
	//center
	FillUnits(glm::vec2(Pos.x + 1, Pos.y + 1), Size[0] - 2, Size[1] - 2, (glm::vec2){TXP_CF, 0});
	//bottom
	FillHorizontalUnits(glm::vec2(Pos.x + 1, Pos.y), Size[0] - 2, (glm::vec2){TXP_BF, 0});
	//left
	FillVerticalUnits(glm::vec2(Pos.x, Pos.y + 1), Size[1] - 2, (glm::vec2){TXP_LF, 0});
	//top
	FillHorizontalUnits(glm::vec2(Pos.x + 1, Pos.y + Size[1] - 1), Size[0] - 2, (glm::vec2){TXP_TF, 0});
	//right
	FillVerticalUnits(glm::vec2(Pos.x + Size[0] - 1, Pos.y + 1), Size[1] - 2, (glm::vec2){TXP_RF, 0});
}

void MBoxBuffer3::AddLevelEdges(glm::vec2 EdgeSize, glm::vec2 GroundPos, glm::vec2 GroundSize)
{
	//ground top
	FillHorizontalUnits(glm::vec2(GroundPos.x, GroundPos.y + GroundSize.y - 1), (int)GroundSize.x, (glm::vec2){TXP_TF, 0});
	//ground body
	if(GroundSize.y > 1) FillUnits(glm::vec2(GroundPos.x, GroundPos.y), (int)GroundSize.x, (int)GroundSize.y - 1, (glm::vec2){TXP_CF, 0});
	//left edge simple
	if(GroundSize.x > 1 && GroundSize.y > 1) FillUnits(glm::vec2(GroundPos.x - EdgeSize.x, GroundPos.y), (int)EdgeSize.x, (int)EdgeSize.y, (glm::vec2){TXP_CF, 0});
	//right edge simple
	if(GroundSize.x > 1 && GroundSize.y > 1) FillUnits(glm::vec2(GroundPos.x + GroundSize.x, GroundPos.y), (int)EdgeSize.x, (int)EdgeSize.y, (glm::vec2){TXP_CF, 0});
}

bool MBoxBuffer3::LoadTempltesFromFile(const char* FileName)
{
	int BlockBegin, BlockEnd;
	int BlocksCount;
	int BoxesCount;
	vector<int> Data;
	stTexturedBox AddBox;
	stTexturedBlock AddBlock;
	vector<stTexturedBlock> AddBlocks;
	map<unsigned int, vector<stTexturedBlock> >::iterator it;
	char iStr[3];
	char jStr[3];
	//new
	char fStr[4] = "f";
	int FixturesCount;
	stFixtureData FixtureData;

	if(!FileJSON.Read(FileName)) return false;
	if(!FileJSON.GetValue("blocks", BlocksCount))
	{
		FileJSON.Close();
		return false;
	}
	if(!FileJSON.GetValue("begin", BlockBegin))
	{
		FileJSON.Close();
		return false;
	}
	
	it = Type_Tempates.find(BlockBegin);
	if(it != Type_Tempates.end())
	{
		LogFile<<"BoxBuffer2: This begin type alredy exist"<<endl;
		return false;
	}
	
	//create blocks templates
	for(unsigned int i=0; i<BlocksCount; i++)
	{
		memset(iStr, 0, 3);
		sprintf(iStr, "%d", i);
		if(!FileJSON.GetValue(iStr, "boxes", BoxesCount))
		{
			FileJSON.Close();
			return false;
		}
		if(!FileJSON.GetValue(iStr, "end", BlockEnd))
		{
			FileJSON.Close();
			return false;
		}
		
		//get boxes
		AddBlock.TexturedBoxes.clear();
		for(unsigned int j=0; j<BoxesCount; j++)
		{
			memset(jStr, 0, 3);
			sprintf(jStr, "%d", j);
			Data.clear();
			if(!FileJSON.GetArray(iStr, jStr, &Data))
			{
				FileJSON.Close();
				return false;
			}
			if(Data.size() < 3)
			{
				LogFile<<"BoxBuffer2: not full array data"<<endl;
				FileJSON.Close();
				return false;
			}
			AddBox.vp[0] = Data[0];
			AddBox.vp[1] = Data[1];
			AddBox.tp = Data[2];
			if(AddBox.vp[0] >= MAX_POS || AddBox.vp[1] >= MAX_POS)
			{
				LogFile<<"BoxBuffer2: wrong position data"<<endl;
				return false;
			}
			if(AddBox.tp < 0 || AddBox.tp >= TXP_TOTAL)
			{
				LogFile<<"BoxBuffer2: wrong texture offset data"<<endl;
				return false;
			}
			AddBlock.TexturedBoxes.push_back(AddBox);
			AddBlock.BlockEnd = BlockEnd;
		}
		
		//get fixtures
		AddBlock.FixtureData.clear();
		if(!FileJSON.GetValue(iStr, "fixtures", FixturesCount))
		{
			FileJSON.Close();
			return false;
		}
		if(FixturesCount < 1 || FixturesCount > 5) return false;
		for(int j=0; j<FixturesCount; j++)
		{
			memset(jStr, 0, 3);
			sprintf(jStr, "%d", j);
			memset(fStr + 1, 0, 3);
			strcat(fStr, jStr);
			Data.clear();
			if(!FileJSON.GetArray(iStr, fStr, &Data))
			{
				FileJSON.Close();
				return false;
			}
			if(Data.size() != 4) return false;
			FixtureData.Position[0] = Data[0];
			FixtureData.Position[1] = Data[1];
			FixtureData.Size[0] = Data[2];
			FixtureData.Size[1] = Data[3];
			AddBlock.FixtureData.push_back(FixtureData);
		}
		AddBlocks.push_back(AddBlock);
	}
	if(!AddBlocks.size())
	{
		LogFile<<"BoxBuffer2: no templates data"<<endl;
		return false;
	}
	Type_Tempates.insert(pair<unsigned int, vector<stTexturedBlock> >(BlockBegin, AddBlocks));
	AddBlocks.clear();
	
	FileJSON.Close();
	
	return true;
}

bool MBoxBuffer3::Generate(int inSliceCount)
{
	if(inSliceCount <= 0) return false;
	SliceCount = inSliceCount;
		
	unsigned int Rand;
	int OffsetX;//float
	unsigned int TempatesCount = Type_Tempates.size();
	unsigned int BlocksLimit;
	stTexturedBlock* pRandBlock;
	map<unsigned int, vector<stTexturedBlock> >::iterator it;
	
	if(!TempatesCount)
	{
		LogFile<<"BoxBuffer2: no templates founded"<<endl;
		return false;
	}
	for(unsigned int i=0; i<SliceCount; i++)
	{
		OffsetX = i * MAX_POS;
		it = Type_Tempates.find(LastEnd);
		if(it == Type_Tempates.end())//need check this by another method and not in cycle
		{
			LogFile<<"BoxBuffer2: Can't find this end type in templates"<<endl;
			return false;
		}
		Rand = rand() % Type_Tempates[LastEnd].size();
		pRandBlock = &Type_Tempates[LastEnd][Rand];
		for(unsigned int j=0; j<pRandBlock->TexturedBoxes.size(); j++)
		{
			AddOneQuad(glm::vec2(OffsetX + pRandBlock->TexturedBoxes[j].vp[0], pRandBlock->TexturedBoxes[j].vp[1]), glm::vec2(Type_Offset[pRandBlock->TexturedBoxes[j].tp], 0));
		}
		cout<<"Number: "<<Rand<<" Type: "<<LastEnd<<endl;
		LastEnd = pRandBlock->BlockEnd;
	}
	if(!MStaticBuffer::Dispose()) return false;
	
	return true;
}
