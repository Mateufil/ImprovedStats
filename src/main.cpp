#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// This is my first Geode mod and also my first time using C++, so the code is probably bad

struct Stat {
	std::string stat;
	std::string title;
	float titleScale;
	std::string icon;
	float iconScale;
	bool more;
};

class StatsPage : public Popup {
public:
	static StatsPage* create() {
		auto popup = new StatsPage;
		if (popup->init()) {
			popup->autorelease();
			return popup;
		}
		delete popup;
		return nullptr;
	}

	void goToPage(int page) {
		for (int i = 0; i < m_statList.size(); i++) {
			auto pageMenu = m_mainLayer->getChildByID("page-" + std::to_string(i));
			auto navDotButton = static_cast<CCMenuItemSpriteExtra*>(m_navDotMenu->getChildByID("navdot-" + std::to_string(i)));
			if (i == page) {
				pageMenu->setVisible(true);
				navDotButton->setNormalImage(CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png"));
			} else {
				pageMenu->setVisible(false);
				navDotButton->setNormalImage(CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"));
			}
		}
	}

	void onArrow(CCObject* sender) {
		auto tag = sender->getTag();
		if (tag == 0) {
			m_currentPage--;
			if (m_currentPage == -1) {
				m_currentPage = m_statList.size()-1;
			}
		} else if (tag == 1) {
			m_currentPage++;
			if (m_currentPage == m_statList.size()) {
				m_currentPage = 0;
			}
		}
		goToPage(m_currentPage);
	}

	void onNavDot(CCObject* sender) {
		m_currentPage = sender->getTag();
		goToPage(m_currentPage);
	}

	void onInfo(CCObject* sender) {
		auto statSprite = static_cast<CCNode*>(sender)->getParent()->getParent();
		auto stat = statSprite->getID();
		auto statInfo = m_statInfo[stat];
		auto statDesc = statInfo[1];
		if (stat == "3") { // Official Levels
			auto mainLevelsList = GameLevelManager::sharedState()->m_mainLevels;
			auto mainLevels = 0;
			auto mainCount = 0;
			auto towerLevels = 0;
			auto towerCount = 0;
			auto meltdownLevels = 0;
			auto meltdownCount = 0;
			auto worldLevels = 0;
			auto worldCount = 0;
			auto subzeroLevels = 0;
			auto subzeroCount = 0;
			auto otherLevels = 0;
			auto otherCount = 0;
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_completedLevels)) {
				if (key[0] == 'n') {
					auto id = utils::numFromString<int>(key.substr(2)).unwrapOr(0);
					if (id > 0 and id <= 1000) {
						mainLevels++;
					} else if (id > 1000 and id <= 2000) {
						meltdownLevels++;
					} else if (id > 2000 and id <= 3000) {
						worldLevels++;
					} else if (id > 4000 and id <= 5000) {
						subzeroLevels++;
					} else if (id > 5000 and id <= 6000) {
						towerLevels++;
					} else {
						otherLevels++;
					}
				}
			}
			for (auto [key, data] : CCDictionaryExt<std::string_view>(mainLevelsList)) {
				auto id = utils::numFromString<int>(key).unwrapOr(0);
				if (id > 0 and id <= 1000) {
					mainCount++;
				} else if (id > 1000 and id <= 2000) {
					meltdownCount++;
				} else if (id > 2000 and id <= 3000) {
					worldCount++;
				} else if (id > 4000 and id <= 5000) {
					subzeroCount++;
				} else if (id > 5000 and id <= 6000) {
					towerCount++;
				} else {
					otherCount++;
				}
			}
			statDesc.append(fmt::format(
				"\nMaximum amount is <cp>{}</c>.\n\n<cy>Main:</c> {} / {}     <co>Tower:</c> {} / {}\n<cr>Meltdown:</c> {} / {}     <cg>World:</c> {} / {}\n<cj>SubZero:</c> {} / {}     <ca>Other:</c> {} / {}",
				mainLevelsList->count(),
				mainLevels, mainCount,
				towerLevels, towerCount,
				meltdownLevels, meltdownCount,
				worldLevels, worldCount,
				subzeroLevels, subzeroCount,
				otherLevels, otherCount
			));
		} else if (stat == "12") { // User Coins
			statDesc.append(fmt::format("\n\n<co>Unverified Coins:</c> {}", GameStatsManager::sharedState()->m_pendingUserCoins->count()));
		} else if (stat == "13") { // Diamonds
			statDesc.append(fmt::format("\n\n<cf>Diamond Shards:</c> {}", GameStatsManager::sharedState()->getStat("29")));
		} else if (stat == "22") { // Orbs
			statDesc.append(fmt::format("\n\n<cl>Current Orbs:</c> {}", GameStatsManager::sharedState()->getStat("14")));
		} else if (stat == "40") { // Gauntlets
			auto gauntletLevels = GameLevelManager::sharedState()->getCompletedGauntletLevels();
			auto gauntletDemons = GameLevelManager::sharedState()->getCompletedGauntletDemons();
			statDesc.append(fmt::format("\n\n<cj>Completed Levels:</c> {}\n<cr>Completed Demons:</c> {}", gauntletLevels, gauntletDemons));
		} else if (stat == "icon") {
			auto maxAmount = 0;
			auto cubeIcons = 0;
			auto shipIcons = 0;
			auto ballIcons = 0;
			auto ufoIcons = 0;
			auto waveIcons = 0;
			auto robotIcons = 0;
			auto spiderIcons = 0;
			auto swingIcons = 0;
			auto jetpackIcons = 0;
			for (int i = 0; i < m_iconTypes.size(); i++) {
				auto iconType = m_iconTypes[i];
				auto count = GameManager::sharedState()->countForType(iconType);
				maxAmount += count;
				for (int j = 1; j <= count; j++) {
					if (GameManager::sharedState()->isIconUnlocked(j, iconType)) {
						switch (iconType) { // I know this is a horrible way to do this...
							case IconType::Cube: cubeIcons++; break;
							case IconType::Ship: shipIcons++; break;
							case IconType::Ball: ballIcons++; break;
							case IconType::Ufo: ufoIcons++; break;
							case IconType::Wave: waveIcons++; break;
							case IconType::Robot: robotIcons++; break;
							case IconType::Spider: spiderIcons++; break;
							case IconType::Swing: swingIcons++; break;
							case IconType::Jetpack: jetpackIcons++; break;
						}
					}
				}
			}
			statDesc.append(fmt::format(
				"\nMaximum amount is <cp>{}</c>.\n<cr>(Some aren't available yet)</c>\n\n<cg>Cubes:</c> {} / {}     <cp>Ships:</c> {} / {}\n<cr>Balls:</c> {} / {}     <co>UFOs:</c> {} / {}\n<cj>Waves:</c> {} / {}     Robots: {} / {}\n<ca>Spiders:</c> {} / {}     <cy>Swings:</c> {} / {}\n<cd>Jetpacks:</c> {} / {}",
				maxAmount,
				cubeIcons, GameManager::sharedState()->countForType(IconType::Cube),
				shipIcons, GameManager::sharedState()->countForType(IconType::Ship),
				ballIcons, GameManager::sharedState()->countForType(IconType::Ball),
				ufoIcons, GameManager::sharedState()->countForType(IconType::Ufo),
				waveIcons, GameManager::sharedState()->countForType(IconType::Wave),
				robotIcons, GameManager::sharedState()->countForType(IconType::Robot),
				spiderIcons, GameManager::sharedState()->countForType(IconType::Spider),
				swingIcons, GameManager::sharedState()->countForType(IconType::Swing),
				jetpackIcons, GameManager::sharedState()->countForType(IconType::Jetpack)
			));
		} else if (stat == "color") {
			auto colors1 = 0;
			auto colors2 = 0;
			for (int i = 0; i < 107; i++) {
				if (GameManager::sharedState()->isColorUnlocked(i, UnlockType::Col1)) {
					colors1++;
				}
				if (GameManager::sharedState()->isColorUnlocked(i, UnlockType::Col2)) {
					colors2++;
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cy>Main Colors:</c> {} / {}\n<cp>Secondary Colors:</c> {} / {}",
				colors1, 107,
				colors2, 107
			));
		} else if (stat == "special") {
			auto maxAmount = 0;
			auto special = 0;
			auto shipFires = 0;
			auto items = 0;
			auto deathEffects = 0;
			for (int i  = 0; i < m_specialTypes.size(); i++) {
				auto specialType = m_specialTypes[i];
				if (specialType == IconType::Item) {
					auto count = m_specialItems.size();
					maxAmount += count;
					for (int j = 0; j < count; j++) {
						if (GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, m_specialItems[j])) {
							items++;
						}
					}
				} else {
					auto count = GameManager::sharedState()->countForType(specialType);
					maxAmount += count;
					for (int j = 1; j <= count; j++) {
						if (GameManager::sharedState()->isIconUnlocked(j, specialType)) {
							switch (specialType) {
								case IconType::Special: special++; break;
								case IconType::ShipFire: shipFires++; break;
								case IconType::DeathEffect: deathEffects++; break;
							}
						}
					}
				}
			}
			statDesc.append(fmt::format(
				"\nMaximum amount is <cp>{}</c>.\n\n<cg>Trails:</c> {} / {}     <cp>Ship Fires:</c> {} / {}\n<co>Animations:</c> {} / {}     <cr>Death Effects:</c> {} / {}",
				maxAmount,
				special, GameManager::sharedState()->countForType(IconType::Special),
				shipFires, GameManager::sharedState()->countForType(IconType::ShipFire),
				items, m_specialItems.size(),
				deathEffects, GameManager::sharedState()->countForType(IconType::DeathEffect)
			));
		} else if (stat == "bought") {
			auto normalItems = 0;
			auto normalCount = 0;
			auto secretItems = 0;
			auto secretCount = 0;
			auto communityItems = 0;
			auto communityCount = 0;
			auto mechanicItems = 0;
			auto mechanicCount = 0;
			auto diamondItems = 0;
			auto diamondCount = 0;
			auto pathItems = 0;
			auto pathCount = 0;
			for (auto item : CCArrayExt<GJStoreItem>(GameStatsManager::sharedState()->m_storeItemArray)) {
				auto unlocked = false;
				if (GameStatsManager::sharedState()->isStoreItemUnlocked(item->m_index)) {
					unlocked = true;
				}
				switch (item->m_shopType) {
					case ShopType::Normal:
						normalCount++;
						if (unlocked) {
							normalItems++;
						}
						break;
					case ShopType::Secret:
						secretCount++;
						if (unlocked) {
							secretItems++;
						}
						break;
					case ShopType::Community:
						communityCount++;
						if (unlocked) {
							communityItems++;
						}
						break;
					case ShopType::Mechanic:
						mechanicCount++;
						if (unlocked) {
							mechanicItems++;
						}
						break;
					case ShopType::Diamond:
						diamondCount++;
						if (unlocked) {
							diamondItems++;
						}
						break;
					case ShopType::Paths:
						pathCount++;
						if (unlocked) {
							pathItems++;
						}
						break;
				}
			}
			statDesc.append(fmt::format(
				"\nMaximum amount is <cp>{}</c>.\n\n<cr>Normal:</c> {} / {}     <cg>Secret:</c> {} / {}\n<cy>Community:</c> {} / {}     <cp>Mechanic:</c> {} / {}\n<cj>Diamond:</c> {} / {}     <co>Paths:</c> {} / {}",
				GameStatsManager::sharedState()->m_storeItemArray->count(),
				normalItems, normalCount,
				secretItems, secretCount,
				communityItems, communityCount,
				mechanicItems, mechanicCount,
				diamondItems, diamondCount,
				pathItems, pathCount
			));
		} else if (stat == "achievement") {
			auto achievements = AchievementManager::sharedState()->m_allAchievements;
			auto mainAchievements = 0;
			auto mainCount = 0;
			auto meltdownAchievements = 0;
			auto meltdownCount = 0;
			auto worldAchievements = 0;
			auto worldCount = 0;
			auto subzeroAchievements = 0;
			auto subzeroCount = 0;
			for (auto achievement : CCArrayExt<CCDictionary>(achievements)) {
				auto id = achievement->valueForKey("identifier")->getCString();
				auto idString = std::string(id);
				auto earned = false;
				if (AchievementManager::sharedState()->isAchievementEarned(id)) {
					earned = true;
				}
				if (idString.contains("md")) {
					meltdownCount++;
					if (earned) {
						meltdownAchievements++;
					}
				} else if (idString.contains("world")) {
					worldCount++;
					if (earned) {
						worldAchievements++;
					}
				} else if (idString.contains("subzero")) {
					subzeroCount++;
					if (earned) {
						subzeroAchievements++;
					}
				} else {
					mainCount++;
					if (earned) {
						mainAchievements++;
					}
				}
			}
			statDesc.append(fmt::format(
				"\nMaximum amount is <cp>{}</c>.\n\n<cy>Main:</c> {} / {}     <cr>Meltdown:</c> {} / {}\n<cg>World:</c> {} / {}     <cj>SubZero:</c> {} / {}",
				achievements->count(),
				mainAchievements, mainCount,
				meltdownAchievements, meltdownCount,
				worldAchievements, worldCount,
				subzeroAchievements, subzeroCount
			));
		} else if (stat == "quest") {
			auto quests10 = 0;
			auto quests15 = 0;
			auto quests20 = 0;
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_challengeDiamonds)) {
				if (key[0] == 'c') {
					auto amount = data->intValue();
					if (amount == 10) {
						quests10++;
					} else if (amount == 15) {
						quests15++;
					} else if (amount == 20) {
						quests20++;
					}
				}
			}
			statDesc.append(fmt::format("\n\n<cl>10 Diamonds:</c> {}\n<cj>15 Diamonds:</c> {}\n<cf>20 Diamonds:</c> {}", quests10, quests15, quests20));
		}
		FLAlertLayer::create(statInfo[0].c_str(), statDesc.c_str(), "OK")->show();
	}

	std::string addCommas(int number) {
		auto numberString = std::to_string(number);
		for (int i = numberString.length() - 3; i > 0; i -= 3) {
			numberString.insert(i, ",");
		}
		return numberString;
	}

	std::vector<std::vector<Stat>> m_statList = {
		{
			{"1", "Jumps", 0.7f, "GJ_orderUpBtn_001.png", 0.55f},
			{"2", "Attempts", 0.7f, "GJ_updateBtn_001.png", 0.55f},
			{"42", "Insane\nLevels", 0.6f, "diffIcon_05_btn_001.png", 0.8f},
			{"3", "Official\nLevels", 0.6f, "GJ_playBtn_001.png", 0.275f, true},
			{"4", "User\nLevels", 0.6f, "GJ_playBtn2_001.png", 0.35f},
			{"5", "Demon\nLevels", 0.6f, "diffIcon_06_btn_001.png", 0.8f},
		},
		{
			{"6", "Stars", 0.7f, "GJ_bigStar_noShadow_001.png", 0.6f},
			{"28", "Moons", 0.7f, "GJ_bigMoon_noShadow_001.png", 0.6f},
			{"13", "Collected\nDiamonds", 0.6f, "GJ_bigDiamond_noShadow_001.png", 0.6f, true},
			{"8", "Secret\nCoins", 0.6f, "secretCoinUI_001.png", 0.6f},
			{"12", "User\nCoins", 0.6f, "secretCoinUI2_001.png", 0.6f, true},
			{"22", "Collected\nOrbs", 0.6f, "currencyOrbIcon_001.png", 1.2f, true}
		},
		{
			{"15", "Daily\nLevels", 0.6f, "gj_dailyCrown_001.png", 0.425f},
			{"weekly", "Weekly\nDemons", 0.6f, "gj_weeklyCrown_001.png", 0.425f},
			{"event", "Event\nLevels", 0.6f, "gj_eventCrown_001.png", 0.425f},
			{"7", "Map\nPacks", 0.6f, "mapPacks.png"_spr, 0.425f},
			{"40", "Gauntlets", 0.7f, "gauntlets.png"_spr, 0.425f, true},
			{"41", "List\nRewards", 0.6f, "GJ_listAddBtn_001.png", 0.8f}
		},
		{
			{"icon", "Icons", 0.7f, "particle_17_001.png", 1.3f, true},
			{"color", "Colors", 0.7f, "GJ_paintBtn_001.png", 0.75f, true},
			{"special", "Special\nItems", 0.6f, "player_special_02_001.png", 0.8f, true},
			{"bought", "Bought\nItems", 0.6f, "storeItemIcon_001.png", 1.2f, true}
		},
		{
			{"10", "Liked/Disliked\nLevels", 0.6f, "GJ_like2Btn_001.png", 0.55f},
			{"11", "Rated\nLevels", 0.6f, "GJ_starBtn_001.png", 0.55f},
			{"9", "Destroyed\nPlayers", 0.6f, "particle_17_001.png", 1.3f},
			{"achievement", "Achievements", 0.7f, "rankIcon_1_001.png", 0.9f, true},
			{"quest", "Quests", 0.7f, "quests.png"_spr, 0.475f, true}
		}
	};

	std::map<std::string, std::vector<std::string>> m_statInfo = {
		{"1", {"Jumps", "Total amount of <cl>Jumps</c>."}},
		{"2", {"Attempts", "Total amount of <cg>Attempts</c>."}},
		{"3", {"Official Levels", "Total amount of completed <cg>Official Levels</c> in <cp>Normal Mode</c>."}},
		{"4", {"User Levels", "Total amount of completed <cj>User Created Levels</c> in <cp>Normal Mode</c>."}},
		{"5", {"Demon Levels", "Total amount of completed <cr>Demon Difficulty Levels</c> in <cp>Normal Mode</c>."}},
		{"6", {"Stars", "Total amount of collected <cy>Stars</c>."}},
		{"7", {"Map Packs", "Total amount of completed <cg>Map Packs</c>."}},
		{"8", {"Secret Coins", "Total amount of collected <co>Secret Coins</c>.\nMaximum amount is <cp>164</c>."}},
		{"9", {"Destroyed Players", "Total amount of <co>Destroyed Players</c> on the <cl>Main Menu</c>."}},
		{"10", {"Liked/Disliked Levels", "Total amount of <cp>Liked or Disliked</c> <cj>Online Levels</c>."}},
		{"11", {"Rated Levels", "Total amount of <cy>Rated</c> <cj>Online Levels</c>."}},
		{"12", {"User Coins", "Total amount of collected <cc>User Coins</c>."}},
		{"13", {"Collected Diamonds", "Total amount of collected <cf>Diamonds</c>."}},
		{"15", {"Daily Levels", "Total amount of completed <cg>Daily Levels</c> in <cp>Normal Mode</c>."}},
		{"22", {"Collected Orbs", "Total amount of collected <cl>Mana Orbs</c>.\nEvery <cp>500</c> <cl>Mana Orbs</c> collected you get a <cy>Demon Key</c>."}},
		{"28", {"Moons", "Total amount of collected <cb>Moons</c>."}},
		{"40", {"Gauntlets", "Total amount of completed <cg>Gauntlets</c>."}},
		{"41", {"List Rewards", "Total amount of claimed <co>List Rewards</c>."}},
		{"42", {"Insane Levels", "Total amount of completed <ca>Insane Difficulty Levels</c> in <cp>Normal Mode</c>."}},
		{"weekly", {"Weekly Demons", "Total amount of completed <cr>Weekly Demon Levels</c> in <cp>Normal Mode</c>."}},
		{"event", {"Event Levels", "Total amount of completed <cy>Event Levels</c> in <cp>Normal Mode</c>."}},
		{"icon", {"Icons", "Total amount of unlocked <cg>Icons</c>."}},
		{"color", {"Colors", "Total amount of unlocked <cg>Colors</c>.\nMaximum amount is <cp>214</c>."}},
		{"special", {"Special Items", "Total amount of unlocked <cg>Trails, Ship Fires, Animations & Death Effects</c>."}},
		{"bought", {"Bought Items", "Total amount of bought <cg>Icons & Items</c>."}},
		{"achievement", {"Achievements", "Total amount of earned <cy>Achievements</c>."}},
		{"quest", {"Quests", "Total amount of completed <cg>Quests</c>."}}
	};

	std::vector<IconType> m_iconTypes {
		IconType::Cube,
		IconType::Ship,
		IconType::Ball,
		IconType::Ufo,
		IconType::Wave,
		IconType::Robot,
		IconType::Spider,
		IconType::Swing,
		IconType::Jetpack
	};

	std::vector<IconType> m_specialTypes {
		IconType::Special,
		IconType::ShipFire,
		IconType::Item,
		IconType::DeathEffect
	};

	std::vector<int> m_specialItems {
		18, // Robot Anim 1
		19, // Robot Anim 2
		20 // Spider Anim
	};

	int m_currentPage = 0;

	CCMenu* m_navDotMenu;
protected:
	bool init() {
		if (!Popup::init(400.f, 250.f)) {
			return false;
		}
		setTitle("Stats", "bigFont.fnt", 1.f);

		// Button Menu
		auto buttonMenu = CCMenu::create();
		buttonMenu->setID("button-menu");
		buttonMenu->setPosition({0.f, 0.f});
		buttonMenu->setContentSize({400.f, 250.f});
		m_mainLayer->addChild(buttonMenu);

		// Next Button
		auto nextButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
		nextButtonSprite->setFlipX(true);
		auto nextButton = CCMenuItemSpriteExtra::create(
			nextButtonSprite,
			this,
			menu_selector(StatsPage::onArrow)
		);
		nextButton->setID("next-button");
		nextButton->setTag(1);
		nextButton->setPosition({425.f, 125.f});
		buttonMenu->addChild(nextButton);

		// Previous Button
		auto previousButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
		auto previousButton = CCMenuItemSpriteExtra::create(
			previousButtonSprite,
			this,
			menu_selector(StatsPage::onArrow)
		);
		previousButton->setID("previous-button");
		previousButton->setTag(0);
		previousButton->setPosition({-25.f, 125.f});
		buttonMenu->addChild(previousButton);

		// Nav Dot Menu
		m_navDotMenu = CCMenu::create();
		m_navDotMenu->setID("navdot-menu");
		m_navDotMenu->setLayout(
			RowLayout::create()
				->setGap(10.f)
		);
		m_navDotMenu->setPosition({0.f, 10.f});
		m_navDotMenu->setAnchorPoint({0.f, 0.f});
		m_navDotMenu->setContentSize({400.f, 20.f});
		m_mainLayer->addChild(m_navDotMenu);

		// Pages
		for (int i = 0; i < m_statList.size(); i++) {
			// Page Menu
			auto pageMenu = CCMenu::create();
			pageMenu->setID("page-" + std::to_string(i));
			pageMenu->setLayout(
				RowLayout::create()
					->setGap(5.f)
					->setGrowCrossAxis(true)
					->setCrossAxisOverflow(false)
			);
			pageMenu->setPosition({200.f, 122.f});
			pageMenu->setContentSize({380.f, 170.f});
			pageMenu->setVisible(false);
			m_mainLayer->addChild(pageMenu);

			// Nav Dot Button
			auto navDotButtonSprite = CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png");
			auto navDotButton = CCMenuItemSpriteExtra::create(
				navDotButtonSprite,
				this,
				menu_selector(StatsPage::onNavDot)
			);
			navDotButton->setID("navdot-" + std::to_string(i));
			navDotButton->setTag(i);
			m_navDotMenu->addChild(navDotButton);

			// Stats
			for (int j = 0; j < m_statList[i].size(); j++) {
				auto stat = m_statList[i][j].stat;

				// Stat
				auto statSprite = CCScale9Sprite::create("square02_001.png");
				statSprite->setID(stat);
				statSprite->setContentSize({120.f, 70.f});
				statSprite->setOpacity(50);
				pageMenu->addChild(statSprite);

				// Title Menu
				auto titleMenu = CCMenu::create();
				titleMenu->setID("title-menu");
				titleMenu->setLayout(
					RowLayout::create()
				);
				titleMenu->setPosition({5.f, 55.f});
				titleMenu->setAnchorPoint({0.f, 0.5f});
				titleMenu->setContentSize({110.f, 30.f});
				statSprite->addChild(titleMenu);

				// Icon
				auto iconSprite = CCSprite::createWithSpriteFrameName(m_statList[i][j].icon.c_str());
				iconSprite->setLayoutOptions(
					AxisLayoutOptions::create()
						->setScaleLimits(m_statList[i][j].iconScale, m_statList[i][j].iconScale)
				);
				titleMenu->addChild(iconSprite);

				// Title
				auto titleLabel = CCLabelBMFont::create(m_statList[i][j].title.c_str(), "goldFont.fnt");
				titleLabel->setLayoutOptions(
					AxisLayoutOptions::create()
						->setScaleLimits(0.f, m_statList[i][j].titleScale)
				);
				titleMenu->addChild(titleLabel);

				titleMenu->updateLayout();

				// Value
				auto value = 0;
				if (stat == "weekly") {
					value = GameLevelManager::sharedState()->getCompletedWeeklyLevels();
				} else if (stat == "event") {
					value = GameLevelManager::sharedState()->getCompletedEventLevels(1, 10);
				} else if (stat == "icon") {
					for (int k  = 0; k < m_iconTypes.size(); k++) {
						for (int l = 1; l <= GameManager::sharedState()->countForType(m_iconTypes[k]); l++) {
							if (GameManager::sharedState()->isIconUnlocked(l, m_iconTypes[k])) {
								value++;
							}
						}
					}
				} else if (stat == "color") {
					for (int k = 0; k < 107; k++) {
						if (GameManager::sharedState()->isColorUnlocked(k, UnlockType::Col1)) {
							value++;
						}
						if (GameManager::sharedState()->isColorUnlocked(k, UnlockType::Col2)) {
							value++;
						}
					}
				} else if (stat == "special") {
					for (int k  = 0; k < m_specialTypes.size(); k++) {
						auto specialType = m_specialTypes[k];
						if (specialType == IconType::Item) {
							for (int l = 0; l < m_specialItems.size(); l++) {
								if (GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, m_specialItems[l])) {
									value++;
								}
							}
						} else {
							for (int l = 1; l <= GameManager::sharedState()->countForType(specialType); l++) {
								if (GameManager::sharedState()->isIconUnlocked(l, m_specialTypes[k])) {
									value++;
								}
							}
						}
					}
				} else if (stat == "bought") {
					for (auto item : CCArrayExt<GJStoreItem>(GameStatsManager::sharedState()->m_storeItemArray)) {
						if (GameStatsManager::sharedState()->isStoreItemUnlocked(item->m_index)) {
							value++;
						}
					}
				} else if (stat == "achievement") {
					for (auto achievement : CCArrayExt<CCDictionary>(AchievementManager::sharedState()->m_allAchievements)) {
						if (AchievementManager::sharedState()->isAchievementEarned(achievement->valueForKey("identifier")->getCString())) {
							value++;
						}
					}
				} else if (stat == "quest") {
					for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_challengeDiamonds)) {
						if (key[0] == 'c') {
							value++;
						}
					}
				} else {
					value = GameStatsManager::sharedState()->getStat(stat.c_str());
				}
				auto valueLabel = CCLabelBMFont::create(addCommas(value).c_str(), "bigFont.fnt");
				valueLabel->setPosition({60.f, 20.f});
				valueLabel->limitLabelWidth(110.f, 0.8f, 0.f);
				statSprite->addChild(valueLabel);

				// Stat Button Menu
				auto statButtonMenu = CCMenu::create();
				statButtonMenu->setID("button-menu");
				statButtonMenu->setPosition({0.f, 0.f});
				statButtonMenu->setContentSize({120.f, 70.f});
				statSprite->addChild(statButtonMenu);

				// Info Button
				if (Mod::get()->getSettingValue<bool>("info-buttons") or m_statList[i][j].more) {
					CCSprite* infoButtonSprite;
					auto infoButtonScale = 0.5f;
					if (m_statList[i][j].more) {
						infoButtonSprite = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
						infoButtonScale = 0.7f;
					} else {
						infoButtonSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
					}
					auto infoButton = CCMenuItemSpriteExtra::create(
						infoButtonSprite,
						this,
						menu_selector(StatsPage::onInfo)
					);
					infoButton->setID("info-button");
					infoButton->setPosition({113.f, 7.f});
					infoButton->setScale(infoButtonScale);
					infoButton->m_baseScale = infoButtonScale;
					statButtonMenu->addChild(infoButton);
				}
			}
			pageMenu->updateLayout();
		}
		m_navDotMenu->updateLayout();

		// Corners
		auto topLeftCornerSprite = CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
		topLeftCornerSprite->setPosition({0.f, 250.f});
		topLeftCornerSprite->setAnchorPoint({0.f, 1.f});
		topLeftCornerSprite->setFlipY(true);
		m_mainLayer->addChild(topLeftCornerSprite);

		auto topRightCornerSprite = CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
		topRightCornerSprite->setPosition({400.f, 250.f});
		topRightCornerSprite->setAnchorPoint({1.f, 1.f});
		topRightCornerSprite->setFlipX(true);
		topRightCornerSprite->setFlipY(true);
		m_mainLayer->addChild(topRightCornerSprite);

		auto bottomLeftCornerSprite = CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
		bottomLeftCornerSprite->setAnchorPoint({0.f, 0.f});
		m_mainLayer->addChild(bottomLeftCornerSprite);

		auto bottomRightCornerSprite = CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
		bottomRightCornerSprite->setPosition({400.f, 0.f});
		bottomRightCornerSprite->setAnchorPoint({1.f, 0.f});
		bottomRightCornerSprite->setFlipX(true);
		m_mainLayer->addChild(bottomRightCornerSprite);

		goToPage(0);
		
		return true;
	}
};

class $modify(MenuLayer) {
	void onStats(CCObject*) {
		StatsPage::create()->show();
	}
};