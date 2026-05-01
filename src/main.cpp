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

	void loadPinnedStats() {
		m_pinnedStats = Mod::get()->getSavedValue<std::vector<std::string>>("pinned", {});
	}

	void savePinnedStats() {
		Mod::get()->setSavedValue<std::vector<std::string>>("pinned", m_pinnedStats);
	}

	bool isPinned(std::string stat) {
		for (int i = 0; i < m_pinnedStats.size(); i++) {
			if (m_pinnedStats[i] == stat) {
				return true;
			}
		}
		return false;
	}

	Stat getStat(std::string stat) {
		for (int i = 0; i < m_statList.size(); i++) {
			for (int j = 1; j < m_statList[i].size(); j++) {
				if (m_statList[i][j].stat == stat) {
					return m_statList[i][j];
				}
			}
		}
		return m_statList[1][1];
	}

	void goToPage(int page) {
		m_currentPage = page;
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
		m_pageTitleLabel->setCString(m_statList[page][0].stat.c_str());
	}

	void onArrow(CCObject* sender) {
		auto tag = sender->getTag();
		if (tag == 0) {
			m_currentPage--;
			if (m_pinnedStats.size() == 0) {
				if (m_currentPage == 0) {
					m_currentPage = m_statList.size()-1;
				}
			} else {
				if (m_currentPage == -1) {
					m_currentPage = m_statList.size()-1;
				}
			}
		} else if (tag == 1) {
			m_currentPage++;
			if (m_currentPage == m_statList.size()) {
				if (m_pinnedStats.size() == 0) {
					m_currentPage = 1;
				} else {
					m_currentPage = 0;
				}
			}
		}
		goToPage(m_currentPage);
	}

	void onNavDot(CCObject* sender) {
		m_currentPage = sender->getTag();
		goToPage(m_currentPage);
	}

	void onMainInfo(CCObject* sender) {
		FLAlertLayer::create("Stats", "To access <cl>more info</c> about a <cy>stat</c> you can press it!\nStats with a <cp>+ symbol</c> may include <cg>extra stats</c> after accessing <cl>more info!</c>", "OK")->show();
	}

	void onStat(CCObject* sender) {
		onInfo(static_cast<CCNode*>(sender)->getChildByID("stat")->getChildByID("button-menu")->getChildByID("info-button"));
	}

	void onInfo(CCObject* sender) {
		auto stat = static_cast<CCNode*>(sender)->getParent()->getParent()->getParent();
		m_statInfoId = stat->getID();
		auto statInfo = m_statInfo[m_statInfoId];
		auto statDesc = statInfo[1];

		if (m_statInfoId == "3") { // Official Levels
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
				if (id > 0 and id <= 1000 and id != 23) {
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
		} else if (m_statInfoId == "4") { // User Levels
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			auto unratedLevels = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_onlineLevels)) {
				if (level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
						default: unratedLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_dailyID > 100000 && level->m_normalPercent >= 100) { // The base User Levels stat doesn't seem to count completed daily levels... So only weekly and event levels
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}\n<cl>Unrated:</c> {}",
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels),
				addCommas(unratedLevels)
			));
		} else if (m_statInfoId == "7") { // Map Packs
			auto completedLevels = 0;
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			for (auto [key, pack] : CCDictionaryExt<std::string_view, GJMapPack>(GameLevelManager::sharedState()->m_savedPacks)) {
				for (auto id : CCArrayExt<CCString>(pack->m_levels)) {
					auto level = GameLevelManager::sharedState()->getSavedLevel(id->intValue());
					if (level) {
						if (level->m_normalPercent >= 100) {
							completedLevels++;
							switch (level->m_stars) {
								case 1: autoLevels++; break;
								case 2: easyLevels++; break;
								case 3: normalLevels++; break;
								case 4: hard4Levels++; break;
								case 5: hard5Levels++; break;
								case 6: harder6Levels++; break;
								case 7: harder7Levels++; break;
								case 8: insane8Levels++; break;
								case 9: insane9Levels++; break;
								case 10: demonLevels++; break;
							}
						}
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cf>Completed Levels:</c> {}\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}",
				addCommas(completedLevels),
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels)
			));
		} else if (m_statInfoId == "8") { // Secret Coins
			auto mainCoins = 0;
			auto towerCoins = 0;
			auto packCoins = 0;
			auto otherCoins = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
				auto coins = GameStatsManager::sharedState()->getCollectedCoinsForLevel(level);
				if (level->m_levelID <= 1000) {
					mainCoins += coins;
				} else if (level->m_levelID > 5000) {
					towerCoins += coins;
				}
			}
			for (auto [key, pack] : CCDictionaryExt<std::string_view, GJMapPack>(GameLevelManager::sharedState()->m_savedPacks)) {
				if (pack->hasCompletedMapPack()) {
					packCoins += pack->m_coins;
				}
			}
			for (int i = 0; i < m_otherCoins.size(); i++) {
				if (GameStatsManager::sharedState()->hasSecretCoin(m_otherCoins[i].c_str())) {
					otherCoins++;
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cy>Main Coins:</c> {}     <co>Tower Coins:</c> {}\n<cg>Map Pack Coins:</c> {}     <cp>Other Coins:</c> {}",
				mainCoins, towerCoins,
				packCoins, otherCoins
			));
		} else if (m_statInfoId == "12") { // User Coins
			statDesc.append(fmt::format("\n\n<co>Unverified Coins:</c> {}", addCommas(GameStatsManager::sharedState()->m_pendingUserCoins->count())));
		} else if (m_statInfoId == "13") { // Diamonds
			statDesc.append(fmt::format("\n\n<cf>Diamond Shards:</c> {}", addCommas(GameStatsManager::sharedState()->getStat("29"))));
		} else if (m_statInfoId == "15") { // Daily Levels
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_dailyID <= 100000 && level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}",
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels)
			));
		} else if (m_statInfoId == "22") { // Orbs
			statDesc.append(fmt::format("\n\n<cl>Current Orbs:</c> {}", addCommas(GameStatsManager::sharedState()->getStat("14"))));
		} else if (m_statInfoId == "40") { // Gauntlets
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cf>Completed Levels:</c> {}\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}",
				addCommas(GameLevelManager::sharedState()->getCompletedGauntletLevels()),
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels)
			));
		} else if (m_statInfoId == "level") {
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			auto mainLevels = 0;
			auto unratedLevels = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
				if (level->m_normalPercent >= 100) {
					mainLevels++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_onlineLevels)) {
				if (level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
						default: unratedLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}\n<cd>Official:</c> {}     <cl>Unrated:</c> {}",
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels),
				addCommas(mainLevels), addCommas(unratedLevels)
			));
		} else if (m_statInfoId == "classic") {
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			auto mainLevels = 0;
			auto unratedLevels = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					mainLevels++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_onlineLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
						default: unratedLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}\n<cd>Official:</c> {}     <cl>Unrated:</c> {}",
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels),
				addCommas(mainLevels), addCommas(unratedLevels)
			));
		} else if (m_statInfoId == "platformer") {
			auto autoLevels = 0;
			auto easyLevels = 0;
			auto normalLevels = 0;
			auto hard4Levels = 0;
			auto hard5Levels = 0;
			auto harder6Levels = 0;
			auto harder7Levels = 0;
			auto insane8Levels = 0;
			auto insane9Levels = 0;
			auto demonLevels = 0;
			auto mainLevels = 0;
			auto unratedLevels = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					mainLevels++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_onlineLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
						default: unratedLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					switch (level->m_stars) {
						case 1: autoLevels++; break;
						case 2: easyLevels++; break;
						case 3: normalLevels++; break;
						case 4: hard4Levels++; break;
						case 5: hard5Levels++; break;
						case 6: harder6Levels++; break;
						case 7: harder7Levels++; break;
						case 8: insane8Levels++; break;
						case 9: insane9Levels++; break;
						case 10: demonLevels++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}\n<cd>Official:</c> {}     <cl>Unrated:</c> {}",
				addCommas(autoLevels), addCommas(easyLevels),
				addCommas(normalLevels), addCommas(hard4Levels),
				addCommas(hard5Levels), addCommas(harder6Levels),
				addCommas(harder7Levels), addCommas(insane8Levels),
				addCommas(insane9Levels), addCommas(demonLevels),
				addCommas(mainLevels), addCommas(unratedLevels)
			));
		} else if (m_statInfoId == "weekly") {
			auto easyDemons = 0;
			auto mediumDemons = 0;
			auto hardDemons = 0;
			auto insaneDemons = 0;
			auto extremeDemons = 0;
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_dailyID > 100000 && level->m_dailyID <= 200000 && level->m_normalPercent >= 100) {
					switch (level->m_demonDifficulty) {
						case 3: easyDemons++; break;
						case 4: mediumDemons++; break;
						case 0: hardDemons++; break;
						case 5: insaneDemons++; break;
						case 6: extremeDemons++; break;
					}
				}
			}
			statDesc.append(fmt::format(
				"\n\n<ca>Easy:</c> {}     <cd>Medium:</c> {}\n<co>Hard:</c> {}     <cr>Insane:</c> {}\n<c>_Extreme:</c> {}",
				addCommas(easyDemons), addCommas(mediumDemons),
				addCommas(hardDemons), addCommas(insaneDemons),
				addCommas(extremeDemons)
			));
		} else if (m_statInfoId == "event") {
			statDesc.append(fmt::format(
				"\n\n<cc>Auto (1*):</c> {}     <cj>Easy (2*):</c> {}\n<cg>Normal (3*):</c> {}     <cy>Hard (4*):</c> {}\n<cs>Hard (5*):</c> {}     <co>Harder (6*):</c> {}\n<cr>Harder (7*):</c> {}     <cp>Insane (8*):</c> {}\n<ca>Insane (9*):</c> {}     <c>_Demon (10*):</c> {}",
				GameLevelManager::sharedState()->getCompletedEventLevels(1, 1),
				GameLevelManager::sharedState()->getCompletedEventLevels(2, 2),
				GameLevelManager::sharedState()->getCompletedEventLevels(3, 3),
				GameLevelManager::sharedState()->getCompletedEventLevels(4, 4),
				GameLevelManager::sharedState()->getCompletedEventLevels(5, 5),
				GameLevelManager::sharedState()->getCompletedEventLevels(6, 6),
				GameLevelManager::sharedState()->getCompletedEventLevels(7, 7),
				GameLevelManager::sharedState()->getCompletedEventLevels(8, 8),
				GameLevelManager::sharedState()->getCompletedEventLevels(9, 9),
				GameLevelManager::sharedState()->getCompletedEventLevels(10, 10)
			));
		} else if (m_statInfoId == "icon") {
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
				addCommas(maxAmount),
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
		} else if (m_statInfoId == "color") {
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
			statDesc.append(fmt::format("\n\n<cy>Main Colors:</c> {} / {}\n<cp>Secondary Colors:</c> {} / {}", colors1, 107, colors2, 107));
		} else if (m_statInfoId == "special") {
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
		} else if (m_statInfoId == "bought") {
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
		} else if (m_statInfoId == "shard") {
			auto shadowShards = GameStatsManager::sharedState()->getStat("16");
			auto poisonShards = GameStatsManager::sharedState()->getStat("17");
			auto fireShards = GameStatsManager::sharedState()->getStat("18");
			auto iceShards = GameStatsManager::sharedState()->getStat("19");
			auto lavaShards = GameStatsManager::sharedState()->getStat("20");
			auto earthShards = GameStatsManager::sharedState()->getStat("23");
			auto bloodShards = GameStatsManager::sharedState()->getStat("24");
			auto metalShards = GameStatsManager::sharedState()->getStat("25");
			auto lightShards = GameStatsManager::sharedState()->getStat("26");
			auto soulShards = GameStatsManager::sharedState()->getStat("27");
			auto bonus1Shards = std::min({shadowShards, poisonShards, fireShards, iceShards, lavaShards});
			auto bonus2Shards = std::min({earthShards, bloodShards, metalShards, lightShards, soulShards});
			statDesc.append(fmt::format(
				"\n\n<cy>Fire:</c> {} / {}     <cj>Ice:</c> {} / {}\n<cg>Poison:</c> {} / {}     <cp>Shadow:</c> {} / {}\n<co>Lava:</c> {} / {}     <cr>B</c><co>o</c><cy>n</c><cg>u</c><cj>s</c> <cp>1:</c> {} / {}\n<cs>Earth:</c> {} / {}     <cr>Blood:</c> {} / {}\nMetal: {} / {}     <cc>Light:</c> {} / {}\n<cd>Soul:</c> {} / {}     <cr>B</c><co>o</c><cy>n</c><cg>u</c><cj>s</c> <cp>2:</c> {} / {}",
				fireShards, 100,
				iceShards, 100,
				poisonShards, 100,
				shadowShards, 100,
				lavaShards, 100,
				bonus1Shards, 100,
				earthShards, 100,
				bloodShards, 100,
				metalShards, 100,
				lightShards, 100,
				soulShards, 100,
				bonus2Shards, 100
			));
		} else if (m_statInfoId == "path") {
			statDesc.append(fmt::format(
				"\n\n<cy>Fire:</c> {} / {}     <cj>Ice:</c> {} / {}\n<cg>Poison:</c> {} / {}     <cp>Shadow:</c> {} / {}\n<co>Lava:</c> {} / {}     <cs>Earth:</c> {} / {}\n<cr>Blood:</c> {} / {}     Metal: {} / {}\n<cc>Light:</c> {} / {}     <cd>Soul:</c> {} / {}",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("30"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("31"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("32"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("33"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("34"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("35"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("36"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("37"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("38"))), "1K",
				addCommas(std::min(1000, GameStatsManager::sharedState()->getStat("39"))), "1K"
			));
		} else if (m_statInfoId == "daily") {
			auto daily4 = 0;
			auto daily24 = 0;
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_dailyChests)) {
				if (key[0] == '1') {
					daily4++;
				} else if (key[0] == '2') {
					daily24++;
				}
			}
			statDesc.append(fmt::format("\n\n<co>4 Hour Chests:</c> {}\n<cj>24 Hour Chests:</c> {}", addCommas(daily4), addCommas(daily24)));
		} else if (m_statInfoId == "treasure") {
			auto allCount = 0;
			for (int i = 0; i < m_treasureTypes.size(); i++) {
				allCount += GameStatsManager::sharedState()->countSecretChests(m_treasureTypes[i]);
			}
			statDesc.append(fmt::format(
				"\nMaximum amount is <cp>{}</c>.\n\n<cr>1 Key:</c> {} / {}     <ca>5 Key:</c> {} / {}\n<cy>10 Key:</c> {} / {}     <cj>25 Key:</c> {} / {}\n<cg>50 Key:</c> {} / {}     <cp>100 Key:</c> {} / {}\n<cs>Gold:</c> {} / {}",
				allCount,
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::SmallTreasure), GameStatsManager::sharedState()->countSecretChests(GJRewardType::SmallTreasure),
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::LargeTreasure), GameStatsManager::sharedState()->countSecretChests(GJRewardType::LargeTreasure),
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::Key10Treasure), GameStatsManager::sharedState()->countSecretChests(GJRewardType::Key10Treasure),
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::Key25Treasure), GameStatsManager::sharedState()->countSecretChests(GJRewardType::Key25Treasure),
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::Key50Treasure), GameStatsManager::sharedState()->countSecretChests(GJRewardType::Key50Treasure),
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::Key100Treasure), GameStatsManager::sharedState()->countSecretChests(GJRewardType::Key100Treasure),
				GameStatsManager::sharedState()->countUnlockedSecretChests(GJRewardType::Gold), GameStatsManager::sharedState()->countSecretChests(GJRewardType::Gold)
			));
		} else if (m_statInfoId == "reward") {
			auto eventChests = 0;
			auto gauntletChests = 0;
			auto pathChests = 0;
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_miscChests)) {
				if (key[0] == 'd') {
					eventChests++;
				} else if (key[0] == 'g') {
					gauntletChests++;
				} else if (key[0] == 'p') {
					pathChests++;
				}
			}
			statDesc.append(fmt::format(
				"\n\n<cr>Weekly:</c> {}     <cp>Event:</c> {}\n<co>Gauntlet:</c> {}     <cy>Path:</c> {} / {}",
				GameStatsManager::sharedState()->m_weeklyChest->count(),
				eventChests,
				gauntletChests,
				pathChests, 10
			));
		} else if (m_statInfoId == "other") {
			auto specialChests = 0;
			auto socialChests = 0;
			auto adChests = 0;
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_miscChests)) {
				if (key[0] == '0') {
					auto id = utils::numFromString<int>(key).unwrapOr(0);
					if ((id >= 7 && id <= 11) || id == 24) {
						socialChests++;
					} else if (id >= 12 && id <= 21) {
						adChests++;
					} else if (id > 0) {
						specialChests++;
					}
				}
			}
			statDesc.append(fmt::format("\n\n<cy>Special Chests:</c> {}\n<cj>Social Chests:</c> {} / {}\n<cp>Ad Chests:</c> {} / {}", specialChests, socialChests, 6, adChests, 10));
		} else if (m_statInfoId == "key") {
			auto demonKeys = GameStatsManager::sharedState()->getStat("21");
			auto goldKeys = GameStatsManager::sharedState()->getStat("43");
			for (int i = 0; i < m_treasureTypes.size(); i++) {
				auto count = GameStatsManager::sharedState()->countUnlockedSecretChests(m_treasureTypes[i]);
				switch (m_treasureTypes[i]) {
					case GJRewardType::SmallTreasure: demonKeys += count; break;
					case GJRewardType::LargeTreasure: demonKeys += count*5; break;
					case GJRewardType::Key10Treasure: demonKeys += count*10; break;
					case GJRewardType::Key25Treasure: demonKeys += count*25; break;
					case GJRewardType::Key50Treasure: demonKeys += count*50; break;
					case GJRewardType::Key100Treasure: demonKeys += count*100; break;
					case GJRewardType::Gold: goldKeys += count; break;
				};
			}
			statDesc.append(fmt::format("\n\n<cr>Demon Keys:</c> {}\n<cs>Gold Keys:</c> {}", addCommas(demonKeys), addCommas(goldKeys)));
		} else if (m_statInfoId == "achievement") {
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
		} else if (m_statInfoId == "quest") {
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
			statDesc.append(fmt::format("\n\n<cl>10 Diamonds:</c> {}\n<cj>15 Diamonds:</c> {}\n<cf>20 Diamonds:</c> {}", addCommas(quests10), addCommas(quests15), addCommas(quests20)));
		}

		auto alert = FLAlertLayer::create(nullptr, statInfo[0].c_str(), statDesc.c_str(), "OK", nullptr, 330.f);
		alert->show();

		CCSprite* pinButtonSprite;
		auto pinButtonTag = 0;
		if (isPinned(m_statInfoId)) {
			pinButtonSprite = CCSprite::createWithSpriteFrameName("unpin.png"_spr);
			pinButtonTag = 1;
		} else {
			pinButtonSprite = CCSprite::createWithSpriteFrameName("pin.png"_spr);
		}
		auto pinButton = CCMenuItemSpriteExtra::create(
			pinButtonSprite,
			this,
			menu_selector(StatsPage::onPin)
		);
		pinButton->setID("pin-button"_spr);
		pinButton->setTag(pinButtonTag);
		pinButton->setPosition({140.f, alert->m_mainLayer->getChildByID("background")->getContentHeight() - 55.f});
		alert->m_mainLayer->getChildByID("main-menu")->addChild(pinButton);
	}

	void onPin(CCObject* sender) {
		auto pinButton = static_cast<CCMenuItemSpriteExtra*>(sender);
		auto tag = pinButton->getTag();
		if (tag == 0) {
			createQuickPopup("Pin Stat", fmt::format("Do you want to <cg>pin</c> the <cy>{}</c> stat?\nThis will <cg>add</c> the stat to the <cj>first page</c> of the <cl>Stats Menu</c>.", m_statInfo[m_statInfoId][0]), "Pin", "Cancel", [&, pinButton](auto, bool button2) {
				if (not button2) {
					m_pinnedStats.push_back(m_statInfoId);
					pinButton->setTag(1);
					pinButton->setNormalImage(CCSprite::createWithSpriteFrameName("unpin.png"_spr));
					savePinnedStats();

					auto pinPage = m_mainLayer->getChildByID("page-0");
					pinPage->addChild(createStat(getStat(m_statInfoId)));
					pinPage->updateLayout();
					m_navDotMenu->getChildByID("navdot-0")->setVisible(true);
					m_navDotMenu->updateLayout();
				}
			}, true, true);
		} else if (tag == 1) {
			createQuickPopup("Unpin Stat", fmt::format("Do you want to <cr>unpin</c> the <cy>{}</c> stat?\nThis will <cr>remove</c> the stat from the <cj>first page</c> of the <cl>Stats Menu</c>.", m_statInfo[m_statInfoId][0]), "Unpin", "Cancel", [&, pinButton](auto, bool button2) {
				if (not button2) {
					for (int i = 0; i < m_pinnedStats.size(); i++) {
						if (m_pinnedStats[i] == m_statInfoId) {
							m_pinnedStats.erase(m_pinnedStats.begin() + i);
						}
					}
					pinButton->setTag(0);
					pinButton->setNormalImage(CCSprite::createWithSpriteFrameName("pin.png"_spr));
					savePinnedStats();
				
					auto pinPage = m_mainLayer->getChildByID("page-0");
					pinPage->removeChildByID(m_statInfoId);
					pinPage->updateLayout();
					if (m_pinnedStats.size() == 0) {
						m_navDotMenu->getChildByID("navdot-0")->setVisible(false);
						m_navDotMenu->updateLayout();
						goToPage(1);
					}
				}
			}, true, true);
		}
	}

	void onDebug(CCObject* sender) {
		std::string debugMessage = "";

		// Main Levels
		debugMessage.append("# <cy>Main Levels\n");
		for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
			debugMessage.append(fmt::format("- #{} - {} - {}%\n", level->m_levelID, level->m_levelName, level->m_normalPercent));
		}
		debugMessage.append("</c>");

		MDPopup::create("Debug", debugMessage, "OK", "Copy", [&, debugMessage](bool button2){
			if (button2) {
				if (utils::clipboard::write(debugMessage)) {
					Notification::create("Copied to clipboard", NotificationIcon::Info)->show();
				} else {
					Notification::create("Failed to copy to clipboard", NotificationIcon::Error)->show();
				}
			}
		})->show();
	}

	std::string addCommas(int number) {
		auto numberString = std::to_string(number);
		for (int i = numberString.length() - 3; i > 0; i -= 3) {
			numberString.insert(i, ",");
		}
		return numberString;
	}

	CCMenuItemSpriteExtra* createStat(Stat statData) {
		auto stat = statData.stat;

		// Stat
		auto statSprite = CCScale9Sprite::create("square02b_001.png");
		statSprite->setID("stat");
		statSprite->setContentSize({120.f, 70.f});
		statSprite->setColor({130, 64, 33});

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
		auto iconSprite = CCSprite::createWithSpriteFrameName(statData.icon.c_str());
		iconSprite->setLayoutOptions(
			AxisLayoutOptions::create()
				->setScaleLimits(statData.iconScale, statData.iconScale)
		);
		titleMenu->addChild(iconSprite);

		// Title
		auto titleLabel = CCLabelBMFont::create(statData.title.c_str(), "goldFont.fnt");
		titleLabel->setLayoutOptions(
			AxisLayoutOptions::create()
				->setScaleLimits(0.f, statData.titleScale)
		);
		titleMenu->addChild(titleLabel);

		titleMenu->updateLayout();

		// Value
		auto value = 0;
		if (stat == "level") {
			value += GameStatsManager::sharedState()->getStat("3");
			value += GameStatsManager::sharedState()->getStat("4");
			value += GameStatsManager::sharedState()->getStat("15");
		} else if (stat == "classic") {
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					value++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_onlineLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					value++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					value++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100 && !level->isPlatformer()) {
					value++;
				}
			}
		} else if (stat == "platformer") {
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_mainLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					value++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_onlineLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					value++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_dailyLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					value++;
				}
			}
			for (auto [key, level] : CCDictionaryExt<std::string_view, GJGameLevel>(GameLevelManager::sharedState()->m_gauntletLevels)) {
				if (level->m_normalPercent >= 100 && level->isPlatformer()) {
					value++;
				}
			}
		} else if (stat == "weekly") {
			value = GameLevelManager::sharedState()->getCompletedWeeklyLevels();
		} else if (stat == "event") {
			value = GameLevelManager::sharedState()->getCompletedEventLevels(1, 10);
		} else if (stat == "icon") {
			for (int i  = 0; i < m_iconTypes.size(); i++) {
				for (int j = 1; j <= GameManager::sharedState()->countForType(m_iconTypes[i]); j++) {
					if (GameManager::sharedState()->isIconUnlocked(j, m_iconTypes[i])) {
						value++;
					}
				}
			}
		} else if (stat == "color") {
			for (int i = 0; i < 107; i++) {
				if (GameManager::sharedState()->isColorUnlocked(i, UnlockType::Col1)) {
					value++;
				}
				if (GameManager::sharedState()->isColorUnlocked(i, UnlockType::Col2)) {
					value++;
				}
			}
		} else if (stat == "special") {
			for (int i  = 0; i < m_specialTypes.size(); i++) {
				auto specialType = m_specialTypes[i];
				if (specialType == IconType::Item) {
					for (int j = 0; j < m_specialItems.size(); j++) {
						if (GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, m_specialItems[j])) {
							value++;
						}
					}
				} else {
					for (int j = 1; j <= GameManager::sharedState()->countForType(specialType); j++) {
						if (GameManager::sharedState()->isIconUnlocked(j, m_specialTypes[i])) {
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
		} else if (stat == "shard") {
			auto shadowShards = GameStatsManager::sharedState()->getStat("16");
			auto poisonShards = GameStatsManager::sharedState()->getStat("17");
			auto fireShards = GameStatsManager::sharedState()->getStat("18");
			auto iceShards = GameStatsManager::sharedState()->getStat("19");
			auto lavaShards = GameStatsManager::sharedState()->getStat("20");
			auto earthShards = GameStatsManager::sharedState()->getStat("23");
			auto bloodShards = GameStatsManager::sharedState()->getStat("24");
			auto metalShards = GameStatsManager::sharedState()->getStat("25");
			auto lightShards = GameStatsManager::sharedState()->getStat("26");
			auto soulShards = GameStatsManager::sharedState()->getStat("27");
			auto bonus1Shards = std::min({shadowShards, poisonShards, fireShards, iceShards, lavaShards});
			auto bonus2Shards = std::min({earthShards, bloodShards, metalShards, lightShards, soulShards});
			value = shadowShards+poisonShards+fireShards+iceShards+lavaShards+earthShards+bloodShards+metalShards+lightShards+soulShards+bonus1Shards+bonus2Shards;
		} else if (stat == "path") {
			value += std::min(1000, GameStatsManager::sharedState()->getStat("30"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("31"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("32"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("33"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("34"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("35"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("36"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("37"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("38"));
			value += std::min(1000, GameStatsManager::sharedState()->getStat("39"));
		} else if (stat == "daily") {
			value = GameStatsManager::sharedState()->m_dailyChests->count();
		} else if (stat == "treasure") {
			for (int i = 0; i < m_treasureTypes.size(); i++) {
				value += GameStatsManager::sharedState()->countUnlockedSecretChests(m_treasureTypes[i]);
			}
		} else if (stat == "reward") {
			value += GameStatsManager::sharedState()->m_weeklyChest->count();
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_miscChests)) {
				if (key[0] == 'd' || key[0] == 'g' || key[0] == 'p') {
					value++;
				}
			}
		} else if (stat == "vault") {
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_miscChests)) {
				if (key[0] == 'o') {
					value++;
				}
			}
		} else if (stat == "other") {
			for (auto [key, data] : CCDictionaryExt<std::string_view, CCString>(GameStatsManager::sharedState()->m_miscChests)) {
				if (key[0] == '0') {
					value++;
				}
			}
		} else if (stat == "key") {
			value += GameStatsManager::sharedState()->getStat("21");
			value += GameStatsManager::sharedState()->getStat("43");
			for (int i = 0; i < m_treasureTypes.size(); i++) {
				auto count = GameStatsManager::sharedState()->countUnlockedSecretChests(m_treasureTypes[i]);
				switch (m_treasureTypes[i]) {
					case GJRewardType::SmallTreasure: value += count; break;
					case GJRewardType::LargeTreasure: value += count*5; break;
					case GJRewardType::Key10Treasure: value += count*10; break;
					case GJRewardType::Key25Treasure: value += count*25; break;
					case GJRewardType::Key50Treasure: value += count*50; break;
					case GJRewardType::Key100Treasure: value += count*100; break;
					case GJRewardType::Gold: value += count; break;
				};
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
		} else if (stat == "rank") {
			auto accountId = GJAccountManager::sharedState()->m_accountID;
			if (accountId > 0) {
				auto userInfo = GameLevelManager::sharedState()->userInfoForAccountID(accountId);
				if (userInfo) {
					value = userInfo->m_globalRank;
				}
			}
		} else if (stat == "creator") {
			auto accountId = GJAccountManager::sharedState()->m_accountID;
			if (accountId > 0) {
				auto userInfo = GameLevelManager::sharedState()->userInfoForAccountID(accountId);
				if (userInfo) {
					value = userInfo->m_creatorPoints;
				}
			}
		} else {
			value = GameStatsManager::sharedState()->getStat(stat.c_str());
		}
		auto valueLabel = CCLabelBMFont::create(addCommas(value).c_str(), "bigFont.fnt");
		valueLabel->setPosition({60.f, 20.f});
		valueLabel->limitLabelWidth(110.f, 0.8f, 0.f);
		statSprite->addChild(valueLabel);

		// More Sprite
		auto moreSprite = CCSprite::createWithSpriteFrameName("more.png"_spr);
		moreSprite->setPosition({113.f, 7.f});
		moreSprite->setScale(0.6f);
		moreSprite->setVisible(false);
		statSprite->addChild(moreSprite);

		// Stat Button Menu
		auto statButtonMenu = CCMenu::create();
		statButtonMenu->setID("button-menu");
		statButtonMenu->setPosition({0.f, 0.f});
		statButtonMenu->setContentSize({120.f, 70.f});
		statSprite->addChild(statButtonMenu);

		// Info Button
		CCSprite* infoButtonSprite;
		auto infoButtonScale = 0.5f;
		if (statData.more) {
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

		if (not Mod::get()->getSettingValue<bool>("info-buttons")) {
			if (statData.more) {
				moreSprite->setVisible(true);
			}
			infoButton->setVisible(false);
		}

		// Stat Button
		auto statButton = CCMenuItemSpriteExtra::create(
			statSprite,
			this,
			menu_selector(StatsPage::onStat)
		);
		statButton->setID(stat);
		statButton->setLayoutOptions(
			AxisLayoutOptions::create()
				->setScaleLimits(0.f, 1.f)
		);
		statButton->m_scaleMultiplier = 1.15f;

		return statButton;
	}

	std::vector<std::vector<Stat>> m_statList = {
		{
			{"Pinned"}
		},
		{
			{"Basic"},
			{"1", "Jumps", 0.7f, "GJ_orderUpBtn_001.png", 0.55f},
			{"2", "Attempts", 0.7f, "GJ_updateBtn_001.png", 0.55f},
			{"level", "Levels", 0.7f, "GJ_playBtn2_001.png", 0.35f, true},
			{"achievement", "Achievements", 0.7f, "rankIcon_1_001.png", 0.9f, true},
			{"quest", "Quests", 0.7f, "quests.png"_spr, 0.475f, true}
		},
		{
			{"Collectibles"},
			{"6", "Stars", 0.7f, "GJ_bigStar_noShadow_001.png", 0.6f},
			{"28", "Moons", 0.7f, "GJ_bigMoon_noShadow_001.png", 0.6f},
			{"13", "Collected\nDiamonds", 0.6f, "GJ_bigDiamond_noShadow_001.png", 0.6f, true},
			{"8", "Secret\nCoins", 0.6f, "secretCoinUI_001.png", 0.6f, true},
			{"12", "User\nCoins", 0.6f, "secretCoinUI2_001.png", 0.6f, true},
			{"22", "Collected\nOrbs", 0.6f, "currencyOrbIcon_001.png", 1.2f, true}
		},
		{
			{"Levels"},
			{"3", "Official\nLevels", 0.6f, "GJ_playBtn_001.png", 0.275f, true},
			{"4", "User\nLevels", 0.6f, "GJ_playBtn2_001.png", 0.35f, true},
			{"42", "Insane\nLevels", 0.6f, "diffIcon_05_btn_001.png", 0.8f},
			{"classic", "Classic\nLevels", 0.6f, "GJ_bigStar_noShadow_001.png", 0.6f, true},
			{"platformer", "Platformer\nLevels", 0.6f, "GJ_bigMoon_noShadow_001.png", 0.6f, true},
			{"5", "Demon\nLevels", 0.6f, "diffIcon_06_btn_001.png", 0.8f}
		},
		{
			{"Completions"},
			{"15", "Daily\nLevels", 0.6f, "gj_dailyCrown_001.png", 0.425f, true},
			{"weekly", "Weekly\nDemons", 0.6f, "gj_weeklyCrown_001.png", 0.425f, true},
			{"event", "Event\nLevels", 0.6f, "gj_eventCrown_001.png", 0.425f, true},
			{"7", "Map\nPacks", 0.6f, "mapPacks.png"_spr, 0.425f, true},
			{"40", "Gauntlets", 0.7f, "gauntlets.png"_spr, 0.425f, true},
			{"41", "List\nRewards", 0.6f, "GJ_listAddBtn_001.png", 0.8f}
		},
		{
			{"Icons"},
			{"icon", "Icons", 0.7f, "icons.png"_spr, 0.75f, true},
			{"color", "Colors", 0.7f, "GJ_paintBtn_001.png", 0.75f, true},
			{"special", "Special\nItems", 0.6f, "player_special_02_001.png", 0.8f, true},
			{"bought", "Bought\nItems", 0.6f, "storeItemIcon_001.png", 1.2f, true},
			{"shard", "Shards", 0.7f, "bonusShardSmall_001.png", 1.3f, true},
			{"path", "Path\nProgress", 0.6f, "paths.png"_spr, 0.45f, true}
		},
		{
			{"Chests"},
			{"daily", "Daily\nChests", 0.6f, "GJ_dailyRewardBtn_001.png", 0.65f, true},
			{"treasure", "Treasure\nChests", 0.6f, "chest_04_02_001.png", 0.25f, true},
			{"reward", "Reward\nChests", 0.6f, "chest_06_02_001.png", 0.25f, true},
			{"vault", "Vault\nChests", 0.6f, "chest_09_02_001.png", 0.25f},
			{"other", "Other\nChests", 0.6f, "chest_01_02_001.png", 0.3f, true},
			{"key", "Collected\nKeys", 0.6f, "GJ_bigKey_001.png", 0.75f, true}
		},
		{
			{"Other"},
			{"10", "Liked\nLevels", 0.6f, "GJ_like2Btn_001.png", 0.55f},
			{"11", "Rated\nLevels", 0.6f, "GJ_starBtn_001.png", 0.55f},
			{"9", "Destroyed\nPlayers", 0.6f, "particle_17_001.png", 1.3f},
			{"rank", "Global\nRank", 0.6f, "rankIcon_top500_001.png", 1.3f},
			{"creator", "Creator\nPoints", 0.6f, "GJ_hammerIcon_001.png", 1.f}
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
		{"10", {"Liked Levels", "Total amount of <cp>Liked or Disliked</c> <cj>Online Levels</c>."}},
		{"11", {"Rated Levels", "Total amount of <cy>Rated</c> <cj>Online Levels</c>."}},
		{"12", {"User Coins", "Total amount of collected <cc>User Coins</c>."}},
		{"13", {"Collected Diamonds", "Total amount of collected <cf>Diamonds</c>."}},
		{"15", {"Daily Levels", "Total amount of completed <cg>Daily Levels</c> in <cp>Normal Mode</c>."}},
		{"22", {"Collected Orbs", "Total amount of collected <cl>Mana Orbs</c>.\nEvery <cp>500</c> <cl>Mana Orbs</c> collected you get a <cy>Demon Key</c>."}},
		{"28", {"Moons", "Total amount of collected <cb>Moons</c>."}},
		{"40", {"Gauntlets", "Total amount of completed <cg>Gauntlets</c>."}},
		{"41", {"List Rewards", "Total amount of claimed <co>List Rewards</c>."}},
		{"42", {"Insane Levels", "Total amount of completed <ca>Insane Difficulty Levels</c> in <cp>Normal Mode</c>."}},
		{"level", {"Levels", "Total amount of completed <cy>Levels</c> in <cp>Normal Mode</c>."}},
		{"achievement", {"Achievements", "Total amount of earned <cy>Achievements</c>."}},
		{"quest", {"Quests", "Total amount of completed <cg>Quests</c>."}},
		{"classic", {"Classic Levels", "Total amount of completed <cy>Classic Levels</c> in <cp>Normal Mode</c>."}},
		{"platformer", {"Platformer Levels", "Total amount of completed <cj>Platformer Levels</c> in <cp>Normal Mode</c>."}},
		{"weekly", {"Weekly Demons", "Total amount of completed <cr>Weekly Demon Levels</c> in <cp>Normal Mode</c>."}},
		{"event", {"Event Levels", "Total amount of completed <cy>Event Levels</c> in <cp>Normal Mode</c>."}},
		{"icon", {"Icons", "Total amount of unlocked <cg>Icons</c>."}},
		{"color", {"Colors", "Total amount of unlocked <cg>Colors</c>.\nMaximum amount is <cp>214</c>."}},
		{"special", {"Special Items", "Total amount of unlocked <cg>Trails, Ship Fires, Animations & Death Effects</c>."}},
		{"bought", {"Bought Items", "Total amount of bought <cg>Icons & Items</c>."}},
		{"shard", {"Shards", "Total amount of collected <cp>Shards</c>.\nMaximum amount is <cp>1,200</c>."}},
		{"path", {"Path Progress", "Total progress from all <co>Paths</c>.\nMaximum amount is <cp>10,000</c>."}},
		{"daily", {"Daily Chests", "Total amount of opened <cg>Daily Chests</c>."}},
		{"treasure", {"Treasure Chests", "Total amount of opened <cg>Treasure Chests</c>."}},
		{"reward", {"Reward Chests", "Total amount of opened <cg>Reward Chests</c>."}},
		{"vault", {"Vault Chests", "Total amount of opened <cg>Wraith Chests</c>."}},
		{"other", {"Other Chests", "Total amount of opened <cg>Special, Social & Ad Chests</c>."}},
		{"key", {"Collected Keys", "Total amount of collected <cy>Demon & Gold Keys</c>."}},
		{"rank", {"Global Rank", "Your rank on the <cy>Global Leaderboard</c>.\nThis updates every <cl>few hours</c>.\nWill show as <cr>0</c> if you aren't on the leaderboard."}},
		{"creator", {"Creator Points", "Total amount of earned <cg>Creator Points</c>."}}
	};

	std::vector<std::string> m_otherCoins {
		"secretB03", // Glubfub
		"secret04", // Master Detective
		"secret06" // Sparky
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

	std::vector<GJRewardType> m_treasureTypes {
		GJRewardType::SmallTreasure,
		GJRewardType::LargeTreasure,
		GJRewardType::Key10Treasure,
		GJRewardType::Key25Treasure,
		GJRewardType::Key50Treasure,
		GJRewardType::Key100Treasure,
		GJRewardType::Gold
	};

	std::vector<std::string> m_pinnedStats {};

	int m_currentPage = 0;
	ZStringView m_statInfoId = "1";

	CCMenu* m_navDotMenu;
	CCLabelBMFont* m_pageTitleLabel;
protected:
	bool init() {
		if (!Popup::init(400.f, 250.f)) {
			return false;
		}

		loadPinnedStats();

		setTitle("Stats", "bigFont.fnt", 1.f);

		// Button Menu
		auto buttonMenu = CCMenu::create();
		buttonMenu->setID("button-menu");
		buttonMenu->setZOrder(10);
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

		// Info Button
		auto infoButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
			this,
			menu_selector(StatsPage::onMainInfo)
		);
		infoButton->setID("info-button");
		infoButton->setPosition({20.f, 20.f});
		buttonMenu->addChild(infoButton);

		// Debug Button
		if (Mod::get()->getSettingValue<bool>("debug")) {
			auto debugButtonSprite = ButtonSprite::create("Debug", "goldFont.fnt", "GJ_button_05.png");
			debugButtonSprite->setScale(0.5);
			auto debugButton = CCMenuItemSpriteExtra::create(
				debugButtonSprite,
				this,
				menu_selector(StatsPage::onDebug)
			);
			debugButton->setID("debug-button");
			debugButton->setPosition({370.f, 15.f});
			buttonMenu->addChild(debugButton);
		}

		// Nav Dot Menu
		m_navDotMenu = CCMenu::create();
		m_navDotMenu->setID("navdot-menu");
		m_navDotMenu->setZOrder(10);
		m_navDotMenu->setLayout(
			RowLayout::create()
				->setGap(10.f)
		);
		m_navDotMenu->setPosition({0.f, 10.f});
		m_navDotMenu->setAnchorPoint({0.f, 0.f});
		m_navDotMenu->setContentSize({400.f, 20.f});
		m_mainLayer->addChild(m_navDotMenu);

		// Page Title
		m_pageTitleLabel = CCLabelBMFont::create("Basic", "goldFont.fnt");
		m_pageTitleLabel->setPosition({200.f, 205.f});
		m_pageTitleLabel->setScale(0.75f);
		m_mainLayer->addChild(m_pageTitleLabel);

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
			pageMenu->setPosition({200.f, 115.f});
			pageMenu->setContentSize({380.f, 160.f});
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
			if (i == 0) { // Pinned
				for (int j = 0; j < m_pinnedStats.size(); j++) {
					auto stat = createStat(getStat(m_pinnedStats[j]));
					pageMenu->addChild(stat);
				}
			} else {
				for (int j = 1; j < m_statList[i].size(); j++) {
					auto stat = createStat(m_statList[i][j]);
					pageMenu->addChild(stat);
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

		if (m_pinnedStats.size() == 0) {
			m_navDotMenu->getChildByID("navdot-0")->setVisible(false);
			m_navDotMenu->updateLayout();
			goToPage(1);
		} else {
			goToPage(0);
		}
		
		return true;
	}
};

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		auto accountId = GJAccountManager::sharedState()->m_accountID;
		if (accountId > 0) {
			if (!GameLevelManager::sharedState()->userInfoForAccountID(accountId)) {
				log::info("Getting user info");
				GameLevelManager::sharedState()->getGJUserInfo(accountId);
			}
		}

		if (GameLevelManager::sharedState()->m_savedPacks->count() < 65) {
			auto packSearch = GJSearchObject::create(SearchType::MapPack);
			for (int i = 0; i < 7; i++) {
				log::info("Getting map packs page {}", i);
				packSearch->m_page = i;
				GameLevelManager::sharedState()->getMapPacks(packSearch);
			}
		}

		return true;
	}
	void onStats(CCObject*) {
		StatsPage::create()->show();
	}
};