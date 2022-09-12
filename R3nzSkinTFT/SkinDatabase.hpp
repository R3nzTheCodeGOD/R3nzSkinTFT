#pragma once

#include <cstdint>
#include <string>
#include <vector>

class SkinDatabase {
public:
	class PetsInfo {
	public:
		const char* modelName;
		const char* skinName;
		std::uint32_t skinCount{ 1 };
	};

	void update() noexcept;
	std::uint32_t getSkinsLenForModel(std::string model) noexcept;
 	
	// PetModelName, PetName, PetSkinCount(autoupdate)
	std::vector<PetsInfo> pets{
		{ "PetAkaliDragon",		"Ossia"			},
		{ "PetAoShin",			"Ao Shin"		},
		{ "PetBallDragon",		"Poggless"		},
		{ "PetBellSwayer",		"Bell Swayer"	},
		{ "PetBuglet",			"Flutterbug"	},
		{ "PetChibiEkko",		"Chibi Ekko"	},
		{ "PetChibiJinx",		"Chibi Jinx"	},
		{ "PetChibiLeeSin",		"Chibi Lee Sin"	},
		{ "PetChibiVi",			"Chibi Vi"		},
		{ "PetChibiYasuo",		"Chibi Yasuo"	},
		{ "PetChoncc",			"Choncc"		},
		{ "PetDowsie",			"Dowsie"		},
		{ "PetDsSquid",			"Squink"		},
		{ "PetDsSwordGuy",		"Abyssia"		},
		{ "PetDsWhale",			"Starmaw"		},
		{ "PetDuckBill",		"Duckbill"		},
		{ "PetElegantDragon",	"Prancie"		},
		{ "PetFairy",			"Nixie"			},
		{ "PetFenroar",			"Fenroar"		},
		{ "PetGargoyle",		"Craggle"		},
		{ "PetGemTiger",		"Protector"		},
		{ "PetGhosty",			"Hauntling"		},
		{ "PetGloop",			"Gloop"			},
		{ "PetGriffin",			"Silverwing"	},
		{ "PetGrumpyLion",		"Furyhorn"		},
		{ "PetHextechBirb",		"Tocker"		},
		{ "PetJawDragon",		"Burno"			},
		{ "PetMiner",			"Molediver"		},
		{ "PetMiniGolem",		"Runespirit"	},
		{ "PetNimbleFoot",		"Nimblefoot"	},
		{ "PetPegasus",			"Lightcharger"	},
		{ "PetPenguKnight",		"Featherknight"	},
		{ "PetPixiMander",		"Piximander"	},
		{ "PetPupDragon",		"Bungo"			},
		{ "PetQiyanaDog",		"QiQi"			},
		{ "PetSennaBunny",		"Melisma"		},
		{ "PetSgCat",			"Dango"			},
		{ "PetSgPig",			"Fuwa"			},
		{ "PetSgShisa",			"Shisa"			},
		{ "PetSpiritFox",		"Hushtail"		},
		{ "PetTftAvatar",		"River Sprite"	},
		{ "PetTurtle",			"Paddlemar"		},
		{ "PetUmbra",			"Umbra"			}
	};
};
