#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pBinReader = new BinaryReader();
	pBinReader->Open(loadInfo.assetFullPath);

	if (!pBinReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}
	if (pBinReader->Read<char>() != 'B' || pBinReader->Read<char>() != 'M' || pBinReader->Read<char>() != 'F')
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent - Font file invalid!");
		return nullptr;
	}
	const int version{ 3 };
	if (pBinReader->Read<char>() < version)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent - version invalid!");
		return nullptr;
	}
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK 0 *
	//**********
	int blockId = pBinReader->Read<char>();
	int blockSize = pBinReader->Read<int>();
	fontDesc.fontSize = pBinReader->Read<short>();
	pBinReader->MoveBufferPosition(12);
	fontDesc.fontName += pBinReader->ReadNullString();

	//**********
	// BLOCK 1 *
	//**********
	blockId = pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	pBinReader->MoveBufferPosition(4);
	fontDesc.textureWidth = pBinReader->Read<short>();
	fontDesc.textureHeight = pBinReader->Read<short>();
	const int pageCount = pBinReader->Read<short>();
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent - pageCount invalid!");
		return nullptr;
	}
	pBinReader->MoveBufferPosition(5);

	//**********
	// BLOCK 2 *
	//**********
	blockId = pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	std::wstring pageName{};
	pageName = pBinReader->ReadNullString();
	const std::wstring path = loadInfo.assetFullPath.parent_path().append(pageName);
	fontDesc.pTexture = ContentManager::Load<TextureData>(path);

	//**********
	// BLOCK 3 *
	//**********
	blockId = pBinReader->Read<char>();
	blockSize = pBinReader->Read<int>();
	const int nrCharacters = blockSize / 20;
	for (int i{}; i < nrCharacters; ++i)
	{
		const wchar_t charId = (wchar_t)pBinReader->Read<int>();
		FontMetric fontMetric{};
		fontMetric.character = charId;
		const float posX = pBinReader->Read<short>();
		const float posY = pBinReader->Read<short>();
		fontMetric.width = pBinReader->Read<unsigned short>();
		fontMetric.height = pBinReader->Read<unsigned short>();
		fontMetric.offsetX = pBinReader->Read<short>();
		fontMetric.offsetY = pBinReader->Read<short>();
		fontMetric.advanceX = pBinReader->Read<short>();
		fontMetric.page = pBinReader->Read<unsigned char>();
		const int channel = pBinReader->Read<char>();
		
		//blue
		if ((channel & 0b00000001) > 0)
		{
			fontMetric.channel = 2;
		}
		//green
		if ((channel & 0b00000010) > 0)
		{
			fontMetric.channel = 1;
		}
		//red
		if ((channel & 0b00000100) > 0)
		{
			fontMetric.channel = 0;
		}
		//alpha
		if ((channel & 0b00001000) > 0)
		{
			fontMetric.channel = 3;
		}
		fontMetric.texCoord = XMFLOAT2(posX / (float)fontDesc.textureWidth, posY / (float)fontDesc.textureHeight);
		fontDesc.metrics[charId] = fontMetric;
	}
	delete pBinReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}