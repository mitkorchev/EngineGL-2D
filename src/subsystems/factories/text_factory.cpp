#include "text_factory.h"

Text TextFactory::GenerateText(
	const char32_t* _string,
	const TextOptions* _textOptions
) const {
	DEBUG_ASSERT(_string != nullptr, "Passed nullptr to TextFactory::GenerateText()\n");

	TextOptions localOptions;

	if (_textOptions) {
		localOptions = *_textOptions;
	}
	
	if (!localOptions.font) {
		localOptions.font = GetResService()->GetFontByName();
	}

	return Text(_string, localOptions);
}