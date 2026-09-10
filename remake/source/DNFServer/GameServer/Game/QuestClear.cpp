// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 QuestClear/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "QuestClear/setClearedQuest.inc"
{
    // save-flag=1 后置 m_cleared[idx]=1（单指令 disp 写入）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 1;
    return true;
}

#include "QuestClear/clear.inc"
{
    // ORIG 0x0808bb06：memset 0x7530 + save-flag(+0x7530) 单指令清零。
    memset(m_cleared, 0, 0x7530);
    m_saveFlag = 0;
}

#include "QuestClear/isClearedQuest.inc"
{
    // ORIG 0x0808bae0：idx==0 或 idx>0x752f → 0；否则 movzbl 直返字节（P9 无 setne）。
    if (questIdx == 0 || questIdx > 0x752f)
        return 0;
    return m_cleared[questIdx].m_cleared;
}

#include "QuestClear/getClearedQuest.inc"
{
    // ORIG（0x0808bb80）返回 this：m_cleared 位于 +0x00，get_quest 据此 memcpy 0x7530 字节。
    return reinterpret_cast<const STClearedQuest*>(this);
}

#include "QuestClear/resetClearedQuests.inc"
{
    // ORIG 0x0808baac：save-flag=1 后清 m_cleared[idx]=0（movb $1,0x7530(%eax) 单指令）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 0;
    return true;
}

#include "QuestClear/setClearedQuest_1.inc"
{
    // ORIG 0x0808bb5e：memcpy(this, quest, 0x7530)，不含 save-flag。
    memcpy(m_cleared, quest, 0x7530);
}

#include "QuestClear/syncScript.inc"
{
    // ORIG 0x0808bb34：mov $0x1,%eax; ret —— 恒返回 1。
    return true;
}

#include "QuestClear/checkSaveFlag.inc"
{
    // ORIG 0x0808bb3e：movzbl 0x7530(%eax) 直返 save-flag 字节（P9）。
    return m_saveFlag;
}

#include "QuestClear/getClearedQuestNum.inc"
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

#include "QuestClear/resetSaveFlags.inc"
{
    // ORIG 0x0808bb4e：清零 save-flag（movb $0,0x7530 单指令）。
    m_saveFlag = 0;
}

#include "QuestClear/enumQuestClear.inc"
{
    // ORIG 0x0808bb88：va_start 后对 i=1..0x752f 调 cb(i,(char*)ap)；setle 有符号比较（P2）。
    va_list ap;
    va_start(ap, cb);
    for (int i = 1; i <= 0x752f; ++i)
        cb(i, (char*)ap);
    va_end(ap);
}

}  // namespace WongWork
