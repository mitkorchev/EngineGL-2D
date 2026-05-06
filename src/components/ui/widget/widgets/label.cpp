#include "label.h"

void Label::SendOwnRenderData(
	Batch* uiBatch,
	glm::vec2 absoluteCurrentWidgetOrigin,
	float z
) const {
	if (m_StoredText.GetTextString().empty()) {
		return;
	}

	if (!m_IsClampingText) {
		uiBatch->DrawText(&m_StoredText, absoluteCurrentWidgetOrigin.x, absoluteCurrentWidgetOrigin.y, z);
	}
	else {
		//	This will support one-line text for now only
		//	So, depending on the scrollX and dimensions.x, we fit all characters and cut what's needed
		
		//	This should return just the clipped text
		std::vector<FullSprite> textGeometry;
		for (const auto& sprite : m_StoredText.GetTextGeometry()) {
			FullSprite preparedSprite = sprite;
			preparedSprite.position.x = sprite.position.x - m_ScrollX;

			if (preparedSprite.position.x + preparedSprite.instance.dimensions.x > 0 && preparedSprite.position.x < GetDimensions().x) {
				textGeometry.emplace_back(preparedSprite);
			}
		}

		if (textGeometry.empty())
			return;

		//	First we clip left
		//	Remember this is already with scroll subtracted
		FullSprite& firstGlyph = textGeometry[0];
		if (firstGlyph.position.x < 0) {
			float difference = abs(firstGlyph.position.x);
			firstGlyph.instance.SetXCutPixels(difference, true);
			firstGlyph.position.x += difference;
		}

		//	Then we clip right
		FullSprite& lastGlyph = textGeometry.back();
		if (lastGlyph.position.x + lastGlyph.instance.dimensions.x > GetDimensions().x) {
			float difference = abs(lastGlyph.position.x + lastGlyph.instance.dimensions.x - GetDimensions().x);
			lastGlyph.instance.SetXCutPixels(difference, false);
			//	This one doesn't need an offset because we cut from the right
		}

		uiBatch->DrawSprites(
			textGeometry,
			absoluteCurrentWidgetOrigin.x,
			absoluteCurrentWidgetOrigin.y,
			z
		);
	}
}