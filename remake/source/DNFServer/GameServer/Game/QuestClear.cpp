#include "QuestClear.h"
#include <stdarg.h>
#include <string.h>

namespace WongWork
{

// P1 组件视图：save-flag 字节位于对象 +0x7530，用组件视图折叠为单指令 disp
// （movb $x,0x7530(%eax)），与 ORIG 一致。
struct QC_SaveFlagView
{
    unsigned char m_pad[0x7530];
    unsigned char m_saveFlag;   // +0x7530
};
// ORIG W 0x0848f768：movb $0,(%eax)（仅置首字节）。
STClearedQuest::STClearedQuest()
{
    m_cleared = 0;
}

CQuestClear::CQuestClear()
{
    // ORIG W 0x08694b92 sz 0x3e（objdump 转写）：i=0x752f..0 循环 0x7530 次调用
    // STClearedQuest::C1Ev（步长 1，每次置对象首字节 0）后调 clear()。
    // STClearedQuest::C1Ev 仅置首字节 0（ORIG 0x0848f768：movb $0,(%eax)），
    // 逐字节循环净效果与之一致（m_cleared 为 unsigned char[0x7530]）。
    // [b22判词·N族] ORIG 循环体每元素 `mov %ebx,(%esp); call STClearedQuestC1Ev`
    // （步长 1），我方 m_cleared 建模为 unsigned char[0x7530]（STClearedQuest 为
    // 1 字节 {uchar}，布局/0x7530 计数一致），循环体退化 `movb $0,(%edx,%eax,1)`
    // [b27] m_cleared 已改型 STClearedQuest[0x7530]：成员隐式构造循环
    // per-element call STClearedQuestC1Ev 复现 ORIG 形（本 TU 已见 C1 定义体）。
    clear();
}

CQuestClear::~CQuestClear()
{
    // ORIG 0x0822e4cc（D1Ev/D2Ev）：push %ebp; mov %esp,%ebp; pop %ebp; ret —— 合法空析构。
}

void CQuestClear::clear()
{
    // ORIG 0x0808bb06：memset 0x7530 + save-flag(+0x7530) 单指令清零。
    memset(m_cleared, 0, 0x7530);
    ((QC_SaveFlagView*)this)->m_saveFlag = 0;
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
    ((QC_SaveFlagView*)this)->m_saveFlag = 1;
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

bool CQuestClear::setClearedQuest(unsigned int questIdx)
{
    // ORIG 0x0808ba78：save-flag=1 后置 m_cleared[idx]=1（单指令 disp 写入）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    ((QC_SaveFlagView*)this)->m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 1;
    return true;
}

unsigned char CQuestClear::checkSaveFlag() const
{
    // ORIG 0x0808bb3e：movzbl 0x7530(%eax) 直返 save-flag 字节（P9）。
    return ((const QC_SaveFlagView*)this)->m_saveFlag;
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
    ((QC_SaveFlagView*)this)->m_saveFlag = 0;
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
