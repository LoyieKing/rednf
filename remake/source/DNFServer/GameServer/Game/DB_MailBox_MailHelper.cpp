// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_MailBox_MailHelper/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "DB_MailBox_MailHelper.h"

#include "SigTypes.h"       // SIG_MAILBOX_LIST
#include "CMailBoxHelper.h" // SIG_LETTER_INFO（0x12b）

#include <vector>
#include <algorithm>

#pragma pack(push, 1)
// SIG_LETTER_INFO 内 +0x125 字段（occ_time）；必须在 pack(1) 内，
// 否则默认 4 字节对齐会把 m_occ 推到 +0x128（与 ORIG 0x125 不符）。
struct LetterOccView
{
    char m_pad[0x125];
    int  m_occ;              // +0x125
};
#pragma pack(pop)

// RemoveMailByLatestLetter —— ORIG 0x0841cd03，定义归 DB_MailBox_Req_List.cpp
#include "DB_MailBox_MailHelper/RemoveMailByLatestLetter.inc"

// ============================================================================
// GetOnlyMailCount —— ORIG 0x0841cc6a：postal 区扫 +0x5c（letter_id），
// 统计为 0 的条目数，返计数。
// ============================================================================
// [sweep] AE：ORIG 0x0841cc6a；改走 m_box.m_postals[i].m_tail.m_occ.m_letterId
// 后循环体装载序/寻址与 ORIG 逐条同形（25=25，p167 不适用=0 call）。
#include "DB_MailBox_MailHelper/GetOnlyMailCount.inc"
{
    int count = 0;
    int i;
    for (i = 0; i < list->m_box.m_count; ++i)
    {
        if (list->m_box.m_postals[i].m_tail.m_occ.m_letterId == 0)
            ++count;
    }
    return count;
}

// ============================================================================
// RemoveMailByMailIndex —— ORIG 0x0841ce06：postal 区找 postal_id==mailIdx
//   项；未找到返 0。找到则自该项起左移一项（0x61 字节 memmove），
//   count-1、remain+1，返 1。
// ============================================================================
// [b28d-27] ORIG 0x0841ce06；memmove→结构体整体赋值（m_postals[i]=m_postals[i+1]）
// 复现 ORIG 对齐检查+rep movsl+残段与双 lea 寻址（111 vs 111 段）。
// [sweep] 残差 1 处 = N18b（成员 vs 形参比较 4.4 恒折 `cmp mem,reg`，ORIG 两段
//   `mov mem,reg; mov arg,reg; cmp reg,reg`），指令数 110 vs 111。
#include "DB_MailBox_MailHelper/RemoveMailByMailIndex.inc"
{
    int i = 0;
    int foundIdx = 0;
    bool found = false;
    for (i = 0; i < list->m_box.m_count; ++i)
    {
        if (mailIdx == list->m_box.m_postals[i].m_postalId)
        {
            found = true;
            foundIdx = i;
            break;
        }
    }
    if (!found)
        return 0;
    for (i = foundIdx; i < list->m_box.m_count - 1; ++i)
    {
        list->m_box.m_postals[i] = list->m_box.m_postals[i + 1];
    }
    list->m_box.m_count -= 1;
    list->m_box.m_remain += 1;
    return 1;
}

// ============================================================================
// FindLastOccTimeMail —— ORIG 0x0841cf5a：第一遍取 occ_time(+0x55)-0x13c680
//   大于入参的最大值；第二遍找等于该值且 postal_id(+0x00) 最大的 postal 下标。
// ============================================================================
// [sweep] AE：ORIG 0x0841cf5a；p167 CLEAR（0 call）。
//   真 bug 修复：第二循环的 maxId 取自 postal **+0x00**（ORIG 0x0841cff9
//   `mov (%edx,%eax,1),%eax`，Ghidra C 亦为 `*(uint *)(param_1 + local_c*0x61)`），
//   原码误读 +0x5c（m_occ.m_letterId）⇒ 选出的 lastIdx 与 ORIG 不同（删错邮件）。
//   同时改走权威头 m_box.m_postals[i].m_postalId / m_tail.m_occ.m_field55
//   （SigTypes_D.h 具名视图，67=67 逐条同形）。
#include "DB_MailBox_MailHelper/FindLastOccTimeMail.inc"
{
    int maxVal = 0;
    for (int i = 0; i < list->m_box.m_count; ++i)
    {
        int v = list->m_box.m_postals[i].m_tail.m_occ.m_field55 - 0x13c680;
        if (v > occTime && maxVal < v)
            maxVal = v;
    }
    int lastIdx = -1;
    unsigned int maxId = 0;
    for (int i = 0; i < list->m_box.m_count; ++i)
    {
        int v = list->m_box.m_postals[i].m_tail.m_occ.m_field55 - 0x13c680;
        if (v == maxVal &&
            maxId < (unsigned int)list->m_box.m_postals[i].m_postalId)
        {
            maxId = (unsigned int)list->m_box.m_postals[i].m_postalId;
            lastIdx = i;
        }
    }
    return lastIdx;
}

// ============================================================================
// CompareLetter —— ORIG 0x0841d032：先比 +0x125（occ_time）严格小于；相等时
//   比首字段（无符号）严格小于。
// ============================================================================
#include "DB_MailBox_MailHelper/CompareLetter.inc"
{
    if (((LetterOccView*)&a)->m_occ < ((LetterOccView*)&b)->m_occ)
        return 1;
    if (((LetterOccView*)&a)->m_occ == ((LetterOccView*)&b)->m_occ)
        return *(const unsigned int*)&a < *(const unsigned int*)&b;
    return 0;
}

// ============================================================================
// ArrangeMailAndLetter —— ORIG 0x0841d2ef：vecNew 按 CompareLetter 排序后，
//   从 size-count 处起逐项：按 +0x125 找 postal 区更新的邮件
//   （FindLastOccTimeMail）并删除（RemoveMailByLatestLetter），同时从 vecOld
//   删同 letter_id(+0x5c) 项。恒返 1。
// ============================================================================
// [sweep] DIFF：ORIG 0x0841d2ef；p167 调用点差集 CLEAR（14=14）。
//   残差 3 处均 4.1/4.4 codegen 固有差：①序言 `mov count,%edx; sub %edx,%eax`
//   vs 4.4 折 `sub mem,%eax`；②`cmpl $0xffffffff,slot; setne; test; je`
//   （4.1 槽比物化，P197④）vs 4.4 直 `je`；③外层尾测 ORIG mem 直比 vs 4.4
//   载 %edx 比较（P209）。109 vs 111。
// [F2d-C05② 甄别] 无命中形：gap=+2（show_func_diff orig=111 our=109，我们**少**）⇒ F2
//   减指令方向相反；且③的比较对象含跨调用返回值 size()（非两具名局部）。本轮实测
//   ORIG-only={setne,test}（=②物化段）⇒ 残余归 P197④/N18b。
#include "DB_MailBox_MailHelper/ArrangeMailAndLetter.inc"
                         std::vector<SIG_LETTER_INFO>& vecNew, int count,
                         std::vector<SIG_LETTER_INFO>& vecOld)
{
    std::sort(vecNew.begin(), vecNew.end(), CompareLetter);
    int start = (int)vecNew.size() - count;
    while ((unsigned int)vecNew.size() > (unsigned int)start)
    {
        int mailIdx = -1;
        mailIdx = FindLastOccTimeMail(list,
                                      ((LetterOccView*)&vecNew[(size_t)start])->m_occ);
        if (mailIdx != -1)
        {
            int letterIdx =
                (int)list->m_box.m_postals[mailIdx].m_tail.m_occ.m_letterId;
            if (RemoveMailByLatestLetter(list, mailIdx))
            {
                if (letterIdx != 0)
                {
                    std::vector<SIG_LETTER_INFO>::iterator it = vecOld.begin();
                    std::vector<SIG_LETTER_INFO>::iterator itEnd = vecOld.end();
                    for (; it != itEnd; ++it)
                    {
                        if (*(const int*)&*it == letterIdx)
                        {
                            vecOld.erase(it);
                            break;
                        }
                    }
                }
            }
        }
        ++start;
    }
    return 1;
}
