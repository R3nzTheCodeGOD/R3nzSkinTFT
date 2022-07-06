#pragma once

#include <cstdint>
#include <string>
#include <vector>

class SkinDatabase {
public:
	class PetsInfo {
	public:
		std::string modelName;
		std::string skinName;
		std::uint32_t skinCount;
	};

	void update() noexcept;
 	
	// PetModelName, PetName, PetSkinCount(autoupdate)
	std::vector<PetsInfo> pets{
		{ "PetAkaliDragon", "Ossia",			1 },
		{ "PetAoShin",		"Ao Shin",			1 },
		{ "PetBallDragon",	"Poggless",			1 },
		{ "PetBellSwayer",	"Bell Swayer",		1 },
		{ "PetBuglet",		"Flutterbug",		1 },
		{ "PetChibiEkko",	"Ekko",				1 },
		{ "PetChibiJinx",	"Jinx",				1 },
		{ "PetChibiVi",		"Vi",				1 },
		{ "PetChibiYasuo",	"Yasuo",			1 },
		{ "PetChoncc",		"Choncc",			1 },
		{ "PetDowsie",		"Dowsie",			1 },
		{ "PetDsSquid",		"Squink",			1 },
		{ "PetDsSwordGuy",	"Abyssia",			1 },
		{ "PetDsWhale",		"Starmaw",			1 },
		{ "PetDuckBill",	"Duckbill",			1 },
		{ "PetFairy",		"Nixie",			1 },
		{ "PetFenroar",		"Fenroar",			1 },
		{ "PetGargoyle",	"Craggle",			1 },
		{ "PetGemTiger",	"Protector",		1 },
		{ "PetGhosty",		"Hauntling",		1 },
		{ "PetGloop",		"Gloop",			1 },
		{ "PetGriffin",		"Silverwing",		1 },
		{ "PetGrumpyLion",	"Furyhorn",			1 },
		{ "PetHextechBirb", "Tocker",			1 },
		{ "PetJawDragon",	"Burno",			1 },
		{ "PetMiner",		"Molediver",		1 },
		{ "PetMiniGolem",	"Runespirit",		1 },
		{ "PetNimbleFoot",	"Nimblefoot",		1 },
		{ "PetPegasus",		"Lightcharger",		1 },
		{ "PetPenguKnight", "Featherknight",	1 },
		{ "PetPixiMander",	"Piximander",		1 },
		{ "PetQiyanaDog",	"QiQi",				1 },
		{ "PetSennaBunny",	"Melisma",			1 },
		{ "PetSgCat",		"Dango",			1 },
		{ "PetSgPig",		"Fuwa",				1 },
		{ "PetSgShisa",		"Shisa",			1 },
		{ "PetSpiritFox",	"Hushtail",			1 },
		{ "PetTftAvatar",	"River Sprite",		1 },
		{ "PetTurtle",		"Paddlemar",		1 },
		{ "PetUmbra",		"Umbra",			1 }
	};
};
