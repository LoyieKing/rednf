#include "QuestClear.h"
#include <stdarg.h>
#include <string.h>

namespace WongWork
{
CQuestClear::CQuestClear()
{
    // ORIG 0x08694b92：循环 0x7530 次调用 STClearedQuest::C1Ev（每次将首字节=0，等价
    // 于清零 m_cleared[0..0x752f]）后再调 clear()。STClearedQuest 为 POD（m_pad[0x14]，
    // C1Ev 仅置 0x00），净效果即全部清零 —— 故仅调用 clear() 与之等效。
    clear();
}

CQuestClear::~CQuestClear()
{
    // ORIG 0x0822e4cc（D1Ev/D2Ev，W/weak）：push %ebp; mov %esp,%ebp; pop %ebp; ret
    // —— 合法空析构，无成员需释放，空体正确，勿改。
}

void CQuestClear::clear()
{
    // ORIG 0x0808bb06：memset 0x7530 + 尾部 1 字节（+0x7530 save-flag）清零
    memset(m_cleared, 0, 0x7530);
    *((unsigned char*)this + 0x7530) = 0;
}

bool CQuestClear::isClearedQuest(unsigned int questIdx) const
{
    // ORIG 0x0808bae0：idx==0 或 idx>0x752f → 0；否则返回 m_cleared[idx]（0/非0）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    return m_cleared[questIdx] != 0;
}

const STClearedQuest* CQuestClear::getClearedQuest() const
{
    // ORIG（0x0808bb80）返回 this：m_cleared 位于 +0x00，UserQuest::get_quest 据此
    // memcpy 0x7530 字节到 SIG_LOAD_QUEST::m_cleared。
    return reinterpret_cast<const STClearedQuest*>(this);
}

bool CQuestClear::resetClearedQuests(unsigned int questIdx)
{
    // ORIG 0x0808baac：idx==0 或 idx>0x752f → 返回 0；否则置 save-flag(+0x7530)=1，
    // 清 m_cleared[idx]=0，返回 1。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    *((unsigned char*)this + 0x7530) = 1;   // save-flag
    m_cleared[questIdx] = 0;
    return true;
}

void CQuestClear::setClearedQuest(const STClearedQuest* quest)
{
    // ORIG 0x0808bb5e：memcpy(this, quest, 0x7530)，不含尾部 save-flag(+0x7530)。
    memcpy(m_cleared, quest, 0x7530);
}

bool CQuestClear::syncScript()
{
    // ORIG 0x0808bb34：mov $0x1,%eax; ret —— 恒返回 1。
    return true;
}

bool CQuestClear::setClearedQuest(unsigned int questIdx)
{
    // ORIG 0x0808ba78：idx==0 或 idx>0x752f → 返回 0；否则置 save-flag(+0x7530)=1，
    // 置 m_cleared[idx]=1，返回 1。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    *((unsigned char*)this + 0x7530) = 1;   // save-flag
    m_cleared[questIdx] = 1;
    return true;
}

bool CQuestClear::checkSaveFlag() const
{
    // ORIG 0x0808bb3e：返回 +0x7530 save-flag 字节（0/非0）。
    return *((const unsigned char*)this + 0x7530) != 0;
}

int CQuestClear::getClearedQuestNum() const
{
    // ORIG 0x0808bbc4：统计 m_cleared[1..0x752f] 非零个数。
    int count = 0;
    for (unsigned int i = 1; i < 30000; ++i)
    {
        if (m_cleared[i] != 0)
            ++count;
    }
    return count;
}

void CQuestClear::resetSaveFlags()
{
    // ORIG 0x0808bb4e：清零 +0x7530 save-flag。
    *((unsigned char*)this + 0x7530) = 0;
}

void CQuestClear::enumQuestClear(bool (*cb)(unsigned int, char*), ...) const
{
    // ORIG 0x0808bb88：va_start 后对 i=1..0x752f 调 cb(i, (char*)ap)。
    va_list ap;
    va_start(ap, cb);
    for (unsigned int i = 1; i < 30000; ++i)
        cb(i, (char*)ap);
    va_end(ap);
}

}  // namespace WongWork
