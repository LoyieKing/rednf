// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_List（邮件列表 DB 任务，OrigMailList19 批）
// ORIG 符号（nm 核对，df_game_r elf32-i386）：
//   T _ZN19DB_MailBox_Req_List12SelectLetterEP5MySQLiijP16SIG_MAILBOX_LIST 0x0841d52c
//   T _ZN19DB_MailBox_Req_List15setCreatureDataEP5MySQLjPjP16SIG_MAILBOX_LIST 0x0841dc84
//   T _ZN19DB_MailBox_Req_List8dispatchEiiP6Stream 0x0841de4a
//   W _ZN19DB_MailBox_Req_ListC1Ev/C2Ev 0x0844e2f6（注册表空壳，W8LT_DBTask.cpp 已有）
//   W _ZN19DB_MailBox_Req_ListD1Ev/D2Ev 0x0845f520、D0Ev 0x0845f550（编译器隐式；
//     本头不写用户析构）
// 报告：docs/class_func_reports/DB_MailBox_Req_List/{SelectLetter,setCreatureData,
//   dispatch}.md（逐指令转写）。
// SelectLetter / setCreatureData 由 dispatch（虚覆盖）以 this 调用 → 普通成员。
// 依赖：SIG_MAILBOX_LIST（SigTypes_D.h，m_data[0x338a]）按 ORIG 偏移访问（本 TU
//   用 pack(1) 视图 MailBoxListView 等，与 DB_MailBox_Dispatch.cpp 同款先例）；
//   ArrangeMailAndLetterIndex / DoChangeMailList / IsContainedLetter 定义归
//   W8LT_DBTask 域其它批次（本 TU 仅声明）。
// 注：W8LT_DBTask.cpp 有同名的 TU-local 注册空壳类（勿改）；本头为其 ORIG
// 成员函数的权威声明点，两者按 DB_JoinGuildInfo.h 先例并存。
// ============================================================================
#ifndef DNF_GAME_DB_MAILBOX_REQ_LIST_H_
#define DNF_GAME_DB_MAILBOX_REQ_LIST_H_

#include "DBDispatcher.h"
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX

class MySQL;
class Stream;
struct SIG_MAILBOX_LIST;

class DB_MailBox_Req_List : public DBDispatcher
{
public:
    DB_MailBox_Req_List() {}   // ORIG C1 0x844e2f6（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0841de4a（虚覆盖，vtable dispatch 槽）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);

    // ORIG 0x0841d52c（普通成员）：查 letter 表并回填 SIG_MAILBOX_LIST 信件区。
    bool SelectLetter(MySQL* db, int slot, int characNo, unsigned int letterIdx,
                      SIG_MAILBOX_LIST* out);

    // ORIG 0x0841dc84（普通成员）：按 ui_id 列表查 creature_items 回填宠物区。
    bool setCreatureData(MySQL* db, unsigned int characNo, unsigned int* list,
                         SIG_MAILBOX_LIST* out);
};

#endif  // DNF_GAME_DB_MAILBOX_REQ_LIST_H_
