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
		if (stat == "40") { // Gauntlets
			auto gauntletLevels = GameLevelManager::sharedState()->getCompletedGauntletLevels();
			auto gauntletDemons = GameLevelManager::sharedState()->getCompletedGauntletDemons();
			statDesc.append(fmt::format("\n\n<cj>Completed Levels:</c> {}\n<cr>Completed Demons:</c> {}", gauntletLevels, gauntletDemons));
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
				"\nMaximum amount is <cp>{}</c>.\n\n<cy>Main:</c> {} / {}\n<cr>Meltdown:</c> {} / {}\n<cg>World:</c> {} / {}\n<cj>SubZero:</c> {} / {}",
				achievements->count(),
				mainAchievements, mainCount,
				meltdownAchievements, meltdownCount,
				worldAchievements, worldCount,
				subzeroAchievements, subzeroCount)
			);
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
			{"3", "Official\nLevels", 0.6f, "GJ_playBtn_001.png", 0.275f},
			{"4", "User\nLevels", 0.6f, "GJ_playBtn2_001.png", 0.35f},
			{"5", "Demon\nLevels", 0.6f, "diffIcon_06_btn_001.png", 0.8f},
		},
		{
			{"6", "Stars", 0.7f, "GJ_bigStar_noShadow_001.png", 0.6f},
			{"28", "Moons", 0.7f, "GJ_bigMoon_noShadow_001.png", 0.6f},
			{"13", "Collected\nDiamonds", 0.6f, "GJ_bigDiamond_noShadow_001.png", 0.6f},
			{"8", "Secret\nCoins", 0.6f, "secretCoinUI_001.png", 0.6f},
			{"12", "User\nCoins", 0.6f, "secretCoinUI2_001.png", 0.6f},
			{"22", "Collected\nOrbs", 0.6f, "currencyOrbIcon_001.png", 1.2f}
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
		{"achievement", {"Achievements", "Total amount of earned <cy>Achievements</c>."}},
		{"quest", {"Quests", "Total amount of completed <cg>Quests</c>."}}
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