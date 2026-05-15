#pragma once

const int INVALID_INDEX = -1;

struct AssetHandle {
	int index = INVALID_INDEX;
	bool isValid() const { return index != INVALID_INDEX; }
};