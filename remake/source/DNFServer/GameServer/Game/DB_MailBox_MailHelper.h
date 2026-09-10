// ============================================================================
// df_game_r 还原 —— DB_MailBox 域 Mail helper 自由函数（OrigDBMail20 批，
// 独占 TU；已入 cmake/game/CMakeLists.txt）
// ORIG 符号（nm 核对，df_game_r elf32-i386，全部 T）：
//   int  GetOnlyMailCount(SIG_MAILBOX_LIST*)              0x0841cc6a
//   bool RemoveMailByMailIndex(SIG_MAILBOX_LIST*, int)    0x0841ce06
//   int  FindLastOccTimeMail(SIG_MAILBOX_LIST*, int)      0x0841cf5a
//   bool CompareLetter(SIG_LETTER_INFO const&, SIG_LETTER_INFO const&) 0x0841d032
//   int  ArrangeMailAndLetter(SIG_MAILBOX_LIST*,
//        std::vector<SIG_LETTER_INFO>&, int,
//        std::vector<SIG_LETTER_INFO>&)                   0x0841d2ef
// 报告：docs/class_func_reports/_lt_global_gt_/{GetOnlyMailCount,
//   RemoveMailByMailIndex,FindLastOccTimeMail,CompareLetter,
//   ArrangeMailAndLetter}.md（逐指令转写）。
// 依赖：SIG_MAILBOX_LIST（SigTypes_D.h，sizeof 0x338a）经权威头具名视图
//   m_box.m_postals[i]（postal_id@+0、m_tail.m_occ.field55@+0x55 /
//   letterId@+0x5c）访问；SIG_LETTER_INFO（CMailBoxHelper.h，0x12b）的
//   +0x125 / 首字段用本 TU pack(1) 视图；RemoveMailByLatestLetter 定义归
//   DB_MailBox_Req_List.cpp（OrigMailList19 批，本 TU 仅声明）。
// 注：ArrangeMailAndLetterIndex / IsContainedLetter 等既有 helper 归
//   DB_MailBox_Req_List.cpp；本头只声明本批 5 个，不与其重复。
// ============================================================================
#ifndef DNF_GAME_DB_MAILBOX_MAILHELPER_H_
#define DNF_GAME_DB_MAILBOX_MAILHELPER_H_

#include <vector>

struct SIG_MAILBOX_LIST;
struct SIG_LETTER_INFO;

// ORIG 0x0841cc6a：统计 postal 区中 letter_id(+0x5c) 为 0 的条目数。
int GetOnlyMailCount(SIG_MAILBOX_LIST* list);

// ORIG 0x0841ce06：按 postal_id 删除邮件（0x61 字节左移），count-1、remain+1；
// 未找到返 0，找到返 1。
bool RemoveMailByMailIndex(SIG_MAILBOX_LIST* list, int mailIdx);

// ORIG 0x0841cf5a：找 occ_time(-0x13c680) 大于入参的最大条目；同值取
// postal_id 最大者，返回其 postal 下标；无则 -1。
int FindLastOccTimeMail(SIG_MAILBOX_LIST* list, int occTime);

// ORIG 0x0841d032：sort 比较器——先比 +0x125 字段，相等再比首字段（无符号）。
bool CompareLetter(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b);

// ORIG 0x0841d2ef：vecNew 按 CompareLetter 排序后，从 size-count 处起逐项：
// 按 +0x125 找 postal 区更新的邮件（FindLastOccTimeMail）并删除
// （RemoveMailByLatestLetter），同时从 vecOld 删同 letter_id 项。恒返 1。
int ArrangeMailAndLetter(SIG_MAILBOX_LIST* list,
                         std::vector<SIG_LETTER_INFO>& vecNew, int count,
                         std::vector<SIG_LETTER_INFO>& vecOld);

#endif  // DNF_GAME_DB_MAILBOX_MAILHELPER_H_
