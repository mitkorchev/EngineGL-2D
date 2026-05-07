#pragma once
#include "../../components/ui/text.h"
#include "factory_interface.h"

class TextFactory : public FactoryInterface {
public:

	TextFactory() : FactoryInterface() {}

	TextFactory(
		const ResourceService* res
	)
		: FactoryInterface(res)
	{}

	Text GenerateText(
		const char32_t* _string,
		const TextOptions* _textOptions = nullptr
	) const;

};