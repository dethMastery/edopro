#include <IGUIComboBox.h>
#include <IGUIStaticText.h>
#include <IGUICheckBox.h>
#include "windbot_panel.h"
#include <sstream>
#include <fmt/format.h>
#include "data_manager.h"

namespace ygo {

void WindBotPanel::Refresh(int filterMasterRule) {
	int selected = deckBox->getSelected();
	int lastBot = selected >= 0 ? deckBox->getItemData(selected) : 0;
	deckBox->clear();
	for (int i = 0; i < bots.size(); i++) {
		const auto& bot = bots[i];
		if (filterMasterRule == 0 || bot.masterRules.find(filterMasterRule) != bot.masterRules.end()) {
			int index = deckBox->addItem(bot.name.c_str(), i);
			if (i == lastBot)
				deckBox->setSelected(i);
		}
	}
	UpdateDescription();
}

void WindBotPanel::UpdateDescription() {
	int selected = deckBox->getSelected();
	if (selected < 0) {
		deckProperties->setText(L"");
		return;
	}
	auto& bot = bots[deckBox->getItemData(selected)];
	std::wstringstream params;
	if (bot.difficulty != 0)
		params << fmt::format(dataManager.GetSysString(2055), bot.difficulty);
	else
		params << dataManager.GetSysString(2056);
	params << L"\n";
	if (bot.masterRules.size()) {
		std::wstring mr; // Due to short string optimization, a stream is not needed
		for (auto rule : bot.masterRules) {
			if (mr.size())
				mr.append(L",");
			mr.append(std::to_wstring(rule));
		}
		params << fmt::format(dataManager.GetSysString(2057), mr.c_str());
		params << L"\n";
	}
	deckProperties->setText(params.str().c_str());
}

bool WindBotPanel::LaunchSelected(int port) {
	int sel = deckBox->getSelected();
	if (sel < 0 || sel >= bots.size()) return false;
	// 1 = scissors, 2 = rock, 3 = paper
#if defined(_WIN32) || defined(__ANDROID__)
	return bots[sel].Launch(port, !chkMute->isChecked(), chkThrowRock->isChecked() * 2);
#else
	auto pid = bots[sel].Launch(port, !chkMute->isChecked(), chkThrowRock->isChecked() * 2);
	if(pid > 0) {
		windbotsPids.push_back(pid);
	}
	return pid > 0;
#endif
}

}