#pragma once

#include <vector>
#include <string>

#include "asset.h"

class Sound : public Asset
{
	std::filesystem::path path;

	using Asset::Asset;
};