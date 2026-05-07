#pragma once
#include "../../../opengl/batch.h"

//	TODO: this can be expanded to maintain an array of instances for the normal and pressed version of buttons or something
//	We operate under the assumption all UI-related data is in one sheet


struct BackgroundSkinInterface {
	
	BackgroundSkinInterface(
		const char* name
	) :
		name(name)
	{}

	std::string name;

	virtual void CalculateBackgroundGeometry(
		std::vector<FullSprite>& OUT_bgGeometryBuffer,
		glm::vec2 widgetDimensions
	) const = 0;

	virtual ~BackgroundSkinInterface() {}

	BackgroundSkinInterface(BackgroundSkinInterface& other) = delete;
	BackgroundSkinInterface& operator=(const BackgroundSkinInterface& other) = delete;

};

struct NineSliceBgSkin : public BackgroundSkinInterface {
	float cornerLengthPx = 0;
	SpriteInstance instanceArray[9];

	static const char* DEFAULT_BG_SUBSPRITE_NAMES[];

	NineSliceBgSkin(
		const char* name
	) :
		BackgroundSkinInterface(name)
	{}

	virtual void CalculateBackgroundGeometry(
		std::vector<FullSprite>& OUT_bgGeometryBuffer,
		glm::vec2 widgetDimensions
	) const;

	virtual ~NineSliceBgSkin() {}
};

struct ImageBgSkin : public BackgroundSkinInterface {
	SpriteInstance imageInstance;

	ImageBgSkin(
		const char* name
	):
		BackgroundSkinInterface(name)
	{}

	virtual void CalculateBackgroundGeometry(
		std::vector<FullSprite>& OUT_bgGeometryBuffer,
		glm::vec2 widgetDimensions
	) const;

	virtual ~ImageBgSkin() {}
};