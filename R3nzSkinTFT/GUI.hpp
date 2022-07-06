#pragma once

#include <mutex>
#include <string>

class GUI {
public:
	bool is_open{ true };

	void render() noexcept;
private:
	std::once_flag changeSkin;
	std::string manualModel{ "TFTDebug_DummyMelee" };
};
