#pragma once

#include <memory>

#include "Config.hpp"
#include "Hooks.hpp"
#include "GUI.hpp"
#include "Memory.hpp"
#include "SkinDatabase.hpp"
#include "Logger.hpp"

class CheatManager {
public:
	void init() noexcept
	{
		this->memory = std::make_unique<Memory>();
		this->config = std::make_unique<Config>();
		this->database = std::make_unique<SkinDatabase>();
		this->gui = std::make_unique<GUI>();
		this->hooks = std::make_unique<Hooks>();
		this->logger = std::make_unique<Logger>();
	}

	bool cheatState{ true };
	std::unique_ptr<Memory> memory;
	std::unique_ptr<Config> config;
	std::unique_ptr<SkinDatabase> database;
	std::unique_ptr<GUI> gui;
	std::unique_ptr<Hooks> hooks;
	std::unique_ptr<Logger> logger;
};

inline CheatManager cheatManager;