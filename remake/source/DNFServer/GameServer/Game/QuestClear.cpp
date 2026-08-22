#include "QuestClear.h"
#include <string.h>

namespace WongWork
{
CQuestClear::CQuestClear()
{
    // ORIG 0x08694b92：构造 STClearedQuest@+0（置首字节 0）后调 clear()：
    // memset(m_cleared, 0, 0x7530) + 尾部字节(0x7530)=0（对象实为 0x7531 字节，
    // 头文件 m_cleared[0x7530] 之后 1 字节按 ORIG 布局访问——推断）。
    m_cleared[0] = 0;
    clear();
}

CQuestClear::~CQuestClear() {}  // TODO(G2)

void CQuestClear::clear()
{
    // ORIG 0x0808bb06：memset 0x7530 + 尾部 1 字节清零
    memset(m_cleared, 0, 0x7530);
    *((unsigned char*)this + 0x7530) = 0;
}

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