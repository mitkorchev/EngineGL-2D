#include "background_skin.h"

const char* NineSliceBgSkin::DEFAULT_BG_SUBSPRITE_NAMES[] = {
	"ul_corner",
	"ur_corner",
	"ll_corner",
	"lr_corner",
	"leftborder",
	"rightborder",
	"upperborder",
	"lowerborder",
	"middle"
};

void NineSliceBgSkin::CalculateBackgroundGeometry(
	std::vector<FullSprite>& OUT_bgGeometryBuffer,
	glm::vec2 widgetDimensions
) const {
	float verticalSpaceToFill = widgetDimensions.y - 2 * cornerLengthPx;
	float horizontalSpaceToFill = widgetDimensions.x - 2 * cornerLengthPx;

	//DEBUG_ASSERT(verticalSpaceToFill >= 0 && horizontalSpaceToFill >= 0, "Widget\'s background has bad lengths.");

	OUT_bgGeometryBuffer.clear();

	//	Vertical sides
	glm::vec2 leftVerticalSideOrigin = glm::vec2(0, cornerLengthPx);
	SpriteInstance leftVerticalSideInstance = instanceArray[4];

	glm::vec2 rightVerticalSideOrigin = glm::vec2(cornerLengthPx + horizontalSpaceToFill, cornerLengthPx);
	SpriteInstance rightVerticalSideInstance = instanceArray[5];

	//	Horizontal sides
	glm::vec2 upperHorizontalSideOrigin = glm::vec2(cornerLengthPx, 0);
	SpriteInstance upperHorizontalSideInstance = instanceArray[6];

	glm::vec2 lowerHorizontalSideOrigin = glm::vec2(cornerLengthPx, cornerLengthPx + verticalSpaceToFill);
	SpriteInstance lowerHorizontalSideInstance = instanceArray[7];

	auto FillSpace = [&OUT_bgGeometryBuffer](const SpriteInstance& instance, glm::vec2 origin, float totalDistance, bool repeatAboutXaxis) {
		if (totalDistance < 0.f) return;

		glm::vec2 step = glm::vec2(0.f, 0.f);
		float stepRawValue = 0;
		if (repeatAboutXaxis) {
			step.x = instance.dimensions.x;
			stepRawValue = step.x;
		}
		else {
			step.y = instance.dimensions.y;
			stepRawValue = step.y;
		}

		if (std::abs(stepRawValue) < FLOAT_COMPARE_TOLERANCE) {
			return;
		}

		int fullRepeats = static_cast<int>(totalDistance / stepRawValue);
		float lastElement = totalDistance - (fullRepeats * stepRawValue);

		for (int i = 0; i < fullRepeats; i++) {
			glm::vec2 calculatedPosition = origin + step * float(i);
			OUT_bgGeometryBuffer.emplace_back(
				instance,
				calculatedPosition.x,
				calculatedPosition.y
			);
		}

		if (std::abs(lastElement) > FLOAT_COMPARE_TOLERANCE) {
			glm::vec2 calculatedPosition = origin + step * float(fullRepeats);
			if (repeatAboutXaxis) {
				calculatedPosition.x -= stepRawValue - lastElement;
			}
			else {
				calculatedPosition.y -= stepRawValue - lastElement;
			}

			OUT_bgGeometryBuffer.emplace_back(
				instance,
				calculatedPosition.x,
				calculatedPosition.y
			);
		}
		};

	FillSpace(leftVerticalSideInstance, leftVerticalSideOrigin, verticalSpaceToFill, false);
	FillSpace(rightVerticalSideInstance, rightVerticalSideOrigin, verticalSpaceToFill, false);

	FillSpace(upperHorizontalSideInstance, upperHorizontalSideOrigin, horizontalSpaceToFill, true);
	FillSpace(lowerHorizontalSideInstance, lowerHorizontalSideOrigin, horizontalSpaceToFill, true);

	//	Center

	SpriteInstance centerBg = instanceArray[8];
	centerBg.dimensions.x = horizontalSpaceToFill;
	centerBg.dimensions.y = verticalSpaceToFill;
	OUT_bgGeometryBuffer.emplace_back(
		centerBg,
		cornerLengthPx,
		cornerLengthPx
	);

	//	Corners

	OUT_bgGeometryBuffer.emplace_back(
		instanceArray[0],
		0.f,
		0.f
	);

	OUT_bgGeometryBuffer.emplace_back(
		instanceArray[1],
		cornerLengthPx + horizontalSpaceToFill,
		0.f
	);

	OUT_bgGeometryBuffer.emplace_back(
		instanceArray[2],
		0.f,
		cornerLengthPx + verticalSpaceToFill
	);

	OUT_bgGeometryBuffer.emplace_back(
		instanceArray[3],
		cornerLengthPx + horizontalSpaceToFill,
		cornerLengthPx + verticalSpaceToFill
	);
}

void ImageBgSkin::CalculateBackgroundGeometry(
	std::vector<FullSprite>& OUT_bgGeometryBuffer,
	glm::vec2 widgetDimensions
) const {
	SpriteInstance centerBg = imageInstance;
	centerBg.dimensions.x = widgetDimensions.x;
	centerBg.dimensions.y = widgetDimensions.y;
	OUT_bgGeometryBuffer.emplace_back(
		centerBg,
		0,
		0
	);
}