#include "QuestClear.h"
#include <stdarg.h>
#include <string.h>
// P338：ORIG 本 TU 含 std::__ioinit（0x093f57ee，b）与
// _GLOBAL__I__ZN8WongWork11CQuestClear15setClearedQuestEj / __static_initialization_and_destruction_0ii
// （0x0808bc03/0x0808bc43，GI 体 sub $0x18）⇒ 原码含 <iostream>（__ioinit 为 ORIG 实有符号）。
#include <iostream>

namespace WongWork
{

// ORIG 0x0808ba78：TU 内第一个**非内联**函数 ⇒ 决定 _GLOBAL__I_ 键名
// （ORIG 键 = _GLOBAL__I__ZN8WongWork11CQuestClear15setClearedQuestEj）；
// 构造/析构已按 ORIG 弱符号形改头内联定义（见 QuestClear.h）。
bool CQuestClear::setClearedQuest(unsigned int questIdx)
{
    // save-flag=1 后置 m_cleared[idx]=1（单指令 disp 写入）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 1;
    return true;
}

void CQuestClear::clear()
{
    // ORIG 0x0808bb06：memset 0x7530 + save-flag(+0x7530) 单指令清零。
    memset(m_cleared, 0, 0x7530);
    m_saveFlag = 0;
}

bool CQuestClear::isClearedQuest(unsigned int questIdx) const
{
    // ORIG 0x0808bae0：idx==0 或 idx>0x752f → 0；否则 movzbl 直返字节（P9 无 setne）。
    if (questIdx == 0 || questIdx > 0x752f)
        return 0;
    return m_cleared[questIdx].m_cleared;
}

const STClearedQuest* CQuestClear::getClearedQuest() const
{
    // ORIG（0x0808bb80）返回 this：m_cleared 位于 +0x00，get_quest 据此 memcpy 0x7530 字节。
    return reinterpret_cast<const STClearedQuest*>(this);
}

bool CQuestClear::resetClearedQuests(unsigned int questIdx)
{
    // ORIG 0x0808baac：save-flag=1 后清 m_cleared[idx]=0（movb $1,0x7530(%eax) 单指令）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 0;
    return true;
}

void CQuestClear::setClearedQuest(const STClearedQuest* quest)
{
    // ORIG 0x0808bb5e：memcpy(this, quest, 0x7530)，不含 save-flag。
    memcpy(m_cleared, quest, 0x7530);
}

bool CQuestClear::syncScript()
{
    // ORIG 0x0808bb34：mov $0x1,%eax; ret —— 恒返回 1。
    return true;
}

unsigned char CQuestClear::checkSaveFlag() const
{
    // ORIG 0x0808bb3e：movzbl 0x7530(%eax) 直返 save-flag 字节（P9）。
    return m_saveFlag;
}

int CQuestClear::getClearedQuestNum() const
{
    // ORIG 0x0808bbc4：统计 m_cleared[1..0x752f] 非零数；有符号比较 setle（P2：i <= 0x752f）。
    int count = 0;
    for (int i = 1; i <= 0x752f; ++i)
    {
        if (m_cleared[i].m_cleared != 0)
            ++count;
    }
    return count;
}

void CQuestClear::resetSaveFlags()
{
    // ORIG 0x0808bb4e：清零 save-flag（movb $0,0x7530 单指令）。
    m_saveFlag = 0;
}

void CQuestClear::enumQuestClear(bool (*cb)(unsigned int, char*), ...) const
{
    // ORIG 0x0808bb88：va_start 后对 i=1..0x752f 调 cb(i,(char*)ap)；setle 有符号比较（P2）。
    va_list ap;
    va_start(ap, cb);
    for (int i = 1; i <= 0x752f; ++i)
        cb(i, (char*)ap);
    va_end(ap);
}

}  // namespace WongWork
