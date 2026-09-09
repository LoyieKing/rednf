// ============================================================================
// df_game_r 还原 —— DB_MailBox 域 DB 侧 dispatch 任务（OrigMailWave18 批）
// ORIG 符号（nm 核对，df_game_r elf32-i386）：
//   T _Z15getList2inQueryPjPc                             0x083ff968
//   T _ZN21DB_MailBox_Req_Delete8dispatchEiiP6Stream     0x0841f084
//   T _ZN28DB_MailBox_Req_CheckReceiver8dispatchEiiP6Stream 0x0841c882
//   T _ZN25DB_Multi_MailBox_Req_Send8dispatchEiiP6Stream 0x0843bada
//   T _ZN25DB_Multi_MailBox_Req_Send11_sendResultEiP26SIG_MAILBOX_CHECK_RECEIVER 0x0843bc16
//   T _ZN28DB_ReqQueryCharacInfoMailbox8dispatchEiiP6Stream 0x0843bd46
//   T _ZN28DB_ReqQueryCharacInfoMailbox11_sendResultEiiPKciiiii 0x0843c224
// 报告：docs/class_func_reports/DB_MailBox_Req_Delete/dispatch.md、
//   DB_MailBox_Req_CheckReceiver/dispatch.md、DB_Multi_MailBox_Req_Send/
//   {dispatch,_sendResult}.md、DB_ReqQueryCharacInfoMailbox/{dispatch,
//   _sendResult}.md、_lt_global_gt_/getList2inQuery.md（逐指令转写）。
// 注：W8LT_DBTask.cpp 有这些类的 TU-local 注册空壳（勿改）；本头为其 ORIG
// 成员函数的权威声明点，两者按 DB_JoinGuildInfo.h / DB_MailBox_Req_Send.h
// 先例并存。
// ============================================================================
#ifndef DNF_GAME_DB_MAILBOX_DISPATCH_H_
#define DNF_GAME_DB_MAILBOX_DISPATCH_H_

#include "DBDispatcher.h"
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX

class MySQL;
class Stream;
struct SIG_MAILBOX_CHECK_RECEIVER;

// ORIG 0x083ff968（全局，T）：把 list[1..list[0]] 拼为 "in (id1,id2,...)" 查询串。
// DB_MailBox_Req_Delete::dispatch / DB_MailBox_Req_List::setCreatureData 调用。
char* getList2inQuery(unsigned int* list, char* out);

class DB_MailBox_Req_Delete : public DBDispatcher
{
public:
    DB_MailBox_Req_Delete() {}   // ORIG C1 0x844e312（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0841f084（虚覆盖）：按删除类型生成 upDate postal 语句并执行。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

class DB_MailBox_Req_CheckReceiver : public DBDispatcher
{
public:
    DB_MailBox_Req_CheckReceiver() {}   // ORIG C1 0x844e2da（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0841c882（虚覆盖）：查收件人 + 黑名单，回填 SIG_MAILBOX_CHECK_RECEIVER。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

class DB_Multi_MailBox_Req_Send : public DBDispatcher
{
public:
    DB_Multi_MailBox_Req_Send() {}   // ORIG C1 0x844f492（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0843bada（虚覆盖）：InsertLetter + _sendResult，恒返 1。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);

    // ORIG 0x0843bc16（普通成员；__PRETTY_FUNCTION__ 实证 "bool ..."）：透传回包。
    bool _sendResult(int slot, SIG_MAILBOX_CHECK_RECEIVER* sig);
};

class DB_ReqQueryCharacInfoMailbox : public DBDispatcher
{
public:
    DB_ReqQueryCharacInfoMailbox() {}   // ORIG C1 0x844f4ae（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0843bd46（虚覆盖）：按名字查 charac_info，回填 SIG_QUERY_CHARAC_INFO。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);

    // ORIG 0x0843c224（普通成员）：写回查询结果缓冲并投递。
    // 返回类型 bool：dispatch 失败路径 `return _sendResult(...)` 在 ORIG
    // 0x0843be5b 处直接 `jmp +0x3e6`（跳过 mov $1），无 int→bool 归一化
    // ⇒ callee 必须已是 bool（P292 探针 /tmp/probe_ret.cpp f vs g 实证）。
    bool _sendResult(int errCode, int slot, const char* name,
                     int characNo, int job, int lev, int growType, int sex);
};

#endif  // DNF_GAME_DB_MAILBOX_DISPATCH_H_
