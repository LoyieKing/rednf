// ============================================================================
// df_game_r 还原 —— DB_MailBox 域 Mail helper 自由函数（OrigDBMail20 批，
// 独占 TU，未入 CMake，待 Main 登记）
// 覆盖 5 个 ORIG T 符号（地址见 DB_MailBox_MailHelper.h 头注释）：
//   1. GetOnlyMailCount       0x0841cc6a
//   2. RemoveMailByMailIndex  0x0841ce06
//   3. FindLastOccTimeMail    0x0841cf5a
//   4. CompareLetter          0x0841d032
//   5. ArrangeMailAndLetter   0x0841d2ef
// 报告均已全文阅读后转写（docs/class_func_reports/_lt_global_gt_/{...}.md）。
// SIG 布局：SIG_MAILBOX_LIST（SigTypes_D.h m_data[0x338a]）按 ORIG 偏移访问
//   （裸偏移宏 + pack(1) 组件视图，与 DB_MailBox_Req_List.cpp 同款先例）；
//   SIG_LETTER_INFO（CMailBoxHelper.h，0x12b）的 +0x125 / 首字段用视图访问。
// 依赖：RemoveMailByLatestLetter 定义归 DB_MailBox_Req_List.cpp（本 TU 仅声明）。
// ============================================================================
#include "DB_MailBox_MailHelper.h"

#include "SigTypes.h"       // SIG_MAILBOX_LIST
#include "CMailBoxHelper.h" // SIG_LETTER_INFO（0x12b）

#include <vector>
#include <cstring>
#include <algorithm>

// SIG_MAILBOX_LIST 响应区字段偏移（与 DB_MailBox_Req_List.cpp 同口径）
#define SIG_I32(p, off)  (*(int*)((char*)(p) + (off)))
#define SIG_U32(p, off)  (*(unsigned int*)((char*)(p) + (off)))
#define SIG_PTR(p, off)  ((char*)(p) + (off))

enum
{
    SIG_OFF_POSTAL_COUNT = 0x794,   // int 已填 postal 条数
    SIG_OFF_REMAIN_COUNT = 0x798,   // int 剩余可拉条数
};

#pragma pack(push, 1)
struct MailPostalView
{
    char m_pad0[0x50];
    struct OccSub
    {
        char m_pad[0x5];
        int  m_field55;          // +0x55（occ_time + 0x13c680）
        char m_pad2[0x3];
        unsigned int m_letterId; // +0x5c
    } m_occ;                     // +0x50
};
#pragma pack(pop)
// SIG_LETTER_INFO 内 +0x125 字段（occ_time）
struct LetterOccView
{
    char m_pad[0x125];
    int  m_occ;              // +0x125
};

// RemoveMailByLatestLetter —— ORIG 0x0841cd03，定义归 DB_MailBox_Req_List.cpp
bool RemoveMailByLatestLetter(SIG_MAILBOX_LIST* list, int mailIdx);

// ============================================================================
// GetOnlyMailCount —— ORIG 0x0841cc6a：postal 区扫 +0x5c（letter_id），
// 统计为 0 的条目数，返计数。
// ============================================================================
int GetOnlyMailCount(SIG_MAILBOX_LIST* list)
{
    int count = 0;
    int i;
    for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
    {
        if (((MailPostalView*)(SIG_PTR(list, i * 0x61)))->m_occ.m_letterId == 0)
            ++count;
    }
    return count;
}

// ============================================================================
// RemoveMailByMailIndex —— ORIG 0x0841ce06：postal 区找 postal_id==mailIdx
//   项；未找到返 0。找到则自该项起左移一项（0x61 字节 memmove），
//   count-1、remain+1，返 1。
// ============================================================================
bool RemoveMailByMailIndex(SIG_MAILBOX_LIST* list, int mailIdx)
{
    int i;
    int foundIdx = 0;
    bool found = false;
    for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
    {
        if (SIG_I32(list, i * 0x61) == mailIdx)
        {
            found = true;
            foundIdx = i;
            break;
        }
    }
    if (!found)
        return 0;
    for (i = foundIdx; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT) - 1; ++i)
    {
        memmove(SIG_PTR(list, i * 0x61), SIG_PTR(list, (i + 1) * 0x61), 0x61);
    }
    SIG_I32(list, SIG_OFF_POSTAL_COUNT) -= 1;
    SIG_I32(list, SIG_OFF_REMAIN_COUNT) += 1;
    return 1;
}

// ============================================================================
// FindLastOccTimeMail —— ORIG 0x0841cf5a：第一遍取 occ_time(+0x55)-0x13c680
//   大于入参的最大值；第二遍找等于该值且 postal_id 最大的 postal 下标。
// ============================================================================
int FindLastOccTimeMail(SIG_MAILBOX_LIST* list, int occTime)
{
    int maxVal = 0;
    int i;
    for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
    {
        int v = ((MailPostalView*)(SIG_PTR(list, i * 0x61)))->m_occ.m_field55 - 0x13c680;
        if (occTime < v && maxVal < v)
            maxVal = v;
    }
    int lastIdx = -1;
    unsigned int maxId = 0;
    for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
    {
        if (((MailPostalView*)(SIG_PTR(list, i * 0x61)))->m_occ.m_field55 - 0x13c680 == maxVal &&
            maxId < ((MailPostalView*)(SIG_PTR(list, i * 0x61)))->m_occ.m_letterId)
        {
            maxId = ((MailPostalView*)(SIG_PTR(list, i * 0x61)))->m_occ.m_letterId;
            lastIdx = i;
        }
    }
    return lastIdx;
}

// ============================================================================
// CompareLetter —— ORIG 0x0841d032：先比 +0x125（occ_time）严格小于；相等时
//   比首字段（无符号）严格小于。
// ============================================================================
bool CompareLetter(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b)
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
int ArrangeMailAndLetter(SIG_MAILBOX_LIST* list,
                         std::vector<SIG_LETTER_INFO>& vecNew, int count,
                         std::vector<SIG_LETTER_INFO>& vecOld)
{
    std::sort(vecNew.begin(), vecNew.end(), CompareLetter);
    int start = (int)vecNew.size() - count;
    while ((int)vecNew.size() > start)
    {
        int mailIdx = -1;
        int occTime = ((LetterOccView*)&vecNew[(size_t)start])->m_occ;
        mailIdx = FindLastOccTimeMail(list, occTime);
        if (mailIdx != -1)
        {
            int letterIdx = SIG_I32(list, mailIdx * 0x61 + 0x5c);
            if (RemoveMailByLatestLetter(list, mailIdx) && letterIdx != 0)
            {
                std::vector<SIG_LETTER_INFO>::iterator it;
                for (it = vecOld.begin(); it != vecOld.end(); ++it)
                {
                    if (*(const unsigned int*)&*it == (unsigned int)letterIdx)
                    {
                        vecOld.erase(it);
                        break;
                    }
                }
            }
        }
        ++start;
    }
    return 1;
}
