#pragma once

#include "../../common/common.h"

//	Instance-related primitive structs for sets of data

struct SpriteInformation {

	SpriteInformation() {}
	
	SpriteInformation(
		uint32_t _sheetIndex,
		uint32_t _spriteIndex
	);

	uint32_t GetSheetIndex() const;
	uint32_t GetSpriteIndex() const;
	
	void SetSheetIndex(uint32_t sheetIndex);
	void SetSpriteIndex(uint32_t spriteIndex);
	void SetXCutoff(bool fromLeft);
	void SetYCutoff(bool fromTop);

private:

	uint32_t data = 0;

};

struct xyPair {
	float x = -1, y = -1;
};

struct SpriteInstance {
	SpriteInformation SpriteInfo;	//	ushort
	xyPair dimensions;				//	float *2
	unsigned short xCut = 65535;
	unsigned short yCut = 65535;

	unsigned short PackRemainFactor(float remainFactor);

	void SetXCut(float remainFactor, bool cutFromLeft = false);
	void SetYCut(float remainFactor, bool cutFromTop = false);
	void SetXCutPixels(float pixelsToCut, bool cutFromLeft = false);
	void SetYCutPixels(float pixelsToCut, bool cutFromTop = false);

	bool IsNull() { return fEqual(dimensions.x, -1) || fEqual(dimensions.y, -1); }
};

struct FullSprite {
	SpriteInstance instance;//	SpriteInstance
	xyPair position;		//	float *2;
	float rotation = 0.f;	//	float
	float z = 0.f;			//	float

	FullSprite() {}

	FullSprite(
		const SpriteInstance& newInstance,
		float x, float y
	) :
		instance(newInstance)
	{
		position.x = x;
		position.y = y;
	}

};