#ifndef GAME_DB_ACCOUNT_CARGO_SYNC_H_
#define GAME_DB_ACCOUNT_CARGO_SYNC_H_

// DB 同步类权威声明（batch-5 迁移 + GoalAuditDB16 ODR 收敛）。
// 本批 DB 请求类在 ORIG 中均为单一类：static/nonstatic makeRequest（调用点无
// this 压栈）与 DBDispatcher 虚 dispatch（注册表 new + 分发表）并存；权威声明
// 集中于此头，W8LT_DBTask.cpp 提供 dispatch 定义、DB_AccountCargoSync.cpp 提供
// makeRequest 定义（DB_GuildExpBookDelete::dispatch 亦在 W8LT_DBTask.cpp）。
// ENUM_SERVER_GROUP 在全局命名空间（CEnvironment.h）定义，参与
// DB_GuildExpBookDelete::makeRequest 的符号：_ZN21DB_GuildExpBookDelete
// 11makeRequestEji17ENUM_SERVER_GROUP，需与 ORIG 一致。

#include "CEnvironment.h"

#include <string>            // DB_UpdateActionPoint::MakeActionClearStateString 形参

#include "DBDispatcher.h"    // 本批类权威基类（叶子头，仅前置 class Stream）

class Stream;   // DBDispatcher::dispatch 形参（权威定义 Stream.h，仅引用）

// 全局作用域 DB 类（无命名空间），与 ORIG 符号一致。
class DB_CreateAccountCargo : public DBDispatcher
{
public:
    DB_CreateAccountCargo() {}                       // ORIG C1 0x844f3ce (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843ac52（W8LT_DBTask.cpp）
    static void makeRequest(int a, unsigned int b, unsigned int c);
};

class DB_DeleteAccountCargo : public DBDispatcher
{
public:
    DB_DeleteAccountCargo() {}                       // ORIG C1 0x844f406 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843b24e（W8LT_DBTask.cpp）
    static void makeRequest(int a, unsigned int b);
};

class DB_GuildExpBookDelete : public DBDispatcher
{
public:
    // ORIG 汇编无 this 压栈（仅 3 参），为静态成员；符号 _ZN21DB_GuildExpBookDelete
    // 11makeRequestEji17ENUM_SERVER_GROUP 与 ORIG 一致。
    DB_GuildExpBookDelete() {}                       // ORIG C1 0x844f4ca (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843c35a（W8LT_DBTask.cpp）
    static void makeRequest(unsigned int idx, int a, ENUM_SERVER_GROUP group);
};

class DB_UpgradeAccountCargo : public DBDispatcher
{
public:
    // ORIG 调用点无 this 压栈（UpgradeAccountCargo.md 0x82b3c4 仅 3 栈参），为静态成员
    DB_UpgradeAccountCargo() {}                      // ORIG C1 0x844f3ea (sz 0x4: base ctor+vptr only)
    static void makeRequest(int a, unsigned int b, unsigned int c);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843af76（DB_AccountCargoSync.cpp）
};

// ORIG _ZN31DB_InsertRandomOptionItemInform11makeRequestEijh @0x08443a40：
//   静态成员；Acquire("DBThread.cpp",0x..) 写头 + 随机选项信息，DB_Q 入队。
class DB_InsertRandomOptionItemInform : public DBDispatcher
{
public:
    // ORIG 调用点无 this 压栈（WarRoom::HandleGetItem 086beedf call 仅 3 参），故为静态成员
    DB_InsertRandomOptionItemInform() {}             // ORIG C1 0x844f8f2 (sz 0x4: base ctor+vptr only)
    static void makeRequest(int a, unsigned int b, unsigned char c);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08443a40（W8LT_DBTask.cpp）
};

class DB_PassPadUpdateFailCnt : public DBDispatcher
{
public:
    // ORIG 调用点无 this 压栈（GameWorld::ResetAllPassPadFailCount 086cca7c call 仅 3 栈参），
    // 定义为静态成员
    DB_PassPadUpdateFailCnt() {}                     // ORIG C1 0x844e4d2 (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int a, unsigned int b, char* c);
    // ORIG T 0x08423bc4 _ZN23DB_PassPadUpdateFailCnt8dispatchEiiP6Stream（虚覆盖；W8LT_DBTask.cpp 定义）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// ORIG _ZN27DB_InsertPoliceSaveChatting11makeRequestEjcPc @0x0843c6aa：
//   静态成员；Acquire("DBThread.cpp",0x904c)、头 0x20c/-1、
//   {characNo:uint, msgType:char, text:str}→QUEUE_IDX 4。
class DB_InsertPoliceSaveChatting : public DBDispatcher
{
public:
    DB_InsertPoliceSaveChatting() {}                 // ORIG C1 0x844f4e6 (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int a, char b, char* c);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843c4a0（W8LT_DBTask.cpp）
};

// ORIG _ZN24DB_GoblinPadUpdateReward11makeRequestEjcbPc @0x08424452：
//   静态成员；Acquire("DBThread.cpp",0x5fc3) 写 0x12b/-1 头 +
//   SIG_GOBLIN_UPDATE_REWARD{accId,byte,bool,strncpy(webAddr,0x10)}，DB_Q 入队。
class DB_GoblinPadUpdateReward : public DBDispatcher
{
public:
    DB_GoblinPadUpdateReward() {}                    // ORIG C1 0x844ebb6 (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int a, char b, bool c, char* d);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08424400（W8LT_DBTask.cpp）
};

// DB_PassPadUpdateCancelCnt / DB_SecurityCardUpdateCancelCnt（TSV 修复，与 ORIG
// 符号一致）：_ZN25DB_PassPadUpdateCancelCnt11makeRequestEjjPKc、
// _ZN30DB_SecurityCardUpdateCancelCnt11makeRequestEjj。
class DB_SecurityCardUpdateCancelCnt : public DBDispatcher
{
public:
    DB_SecurityCardUpdateCancelCnt() {}              // ORIG C1 0x844e94e (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int a, unsigned int b);   // ORIG 静态调用形态 0x0842c112
};

// ORIG _ZN25DB_PassPadUpdateCancelCnt11makeRequestEjjPc @0x0842403a：
//   非静态成员；发 passpad 取消计数请求，DB_Q 入队。
class DB_PassPadUpdateCancelCnt : public DBDispatcher
{
public:
    DB_PassPadUpdateCancelCnt() {}                   // ORIG C1 0x844e4ee (sz 0x4: base ctor+vptr only)
    void makeRequest(unsigned int a, unsigned int b, char* c);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08423a80（W8LT_DBTask.cpp）
};

// APSystem 命名空间：_SIG_LOAD_ACTION_POINT 唯一声明点在 APSystemTypes.h
// （CleanPvPAsm 新建，含 _TodayRewardItem 嵌套）。
#include "APSystemTypes.h"

// APSystem 批 Wave5（ORIG vtable 0x8b5f488 槽序 dispatch/D1/D0）：
namespace APSystem
{
class DB_UpdateActionPoint : public DBDispatcher
{
public:
    // ORIG 调用点无 this 压栈（GameWorld::ResetAllFatigue 086cc656 call 仅 3 栈参；
    // CUser.cpp 同模式静态调用），定义为静态成员
    static void makeRequest(int a, const _SIG_LOAD_ACTION_POINT& sig, bool flag);

    DB_UpdateActionPoint();          // ORIG W 0x0844dd96 C1/C2 同体
    virtual ~DB_UpdateActionPoint(); // D1 0x0812549a / D0 0x081254ca
    virtual bool dispatch(int a, int b, Stream* stream);   // ORIG T 0x0812432a
    bool Update(bool bExpireDate, const _SIG_LOAD_ACTION_POINT& sig);     // ORIG T 0x08124386
    void MakeActionClearStateString(std::string& out, const _ActionPoint* table,
                                    unsigned int count);                  // ORIG T 0x081246d6
};
}   // namespace APSystem

#endif  // GAME_DB_ACCOUNT_CARGO_SYNC_H_
