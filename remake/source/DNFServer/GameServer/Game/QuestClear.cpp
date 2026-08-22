#include "QuestClear.h"

namespace WongWork
{
CQuestClear::CQuestClear() {}  // TODO(G2)

CQuestClear::~CQuestClear() {}  // TODO(G2)

void CQuestClear::clear() {}  // TODO(G2)

bool CQuestClear::isClearedQuest(unsigned int) const { return false; }

const STClearedQuest* CQuestClear::getClearedQuest() const
{
    // ORIG（0x0808bb80）返回 this：m_cleared 位于 +0x00，UserQuest::get_quest 据此
    // memcpy 0x7530 字节到 SIG_LOAD_QUEST::m_cleared。
    return reinterpret_cast<const STClearedQuest*>(this);
}

void CQuestClear::resetClearedQuests(unsigned int) {}  // TODO(G2)

void CQuestClear::setClearedQuest(const STClearedQuest*) {}  // TODO(G2)

bool CQuestClear::syncScript() { return false; }

void CQuestClear::setClearedQuest(unsigned int) {}  // TODO(G2)

}  // namespace WongWork