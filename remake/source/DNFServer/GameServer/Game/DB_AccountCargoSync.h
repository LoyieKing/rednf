#ifndef GAME_DB_ACCOUNT_CARGO_SYNC_H_
#define GAME_DB_ACCOUNT_CARGO_SYNC_H_

// DB 同步 stub 类迁移（batch-5）。
// 原位于 GameStubs.cpp 的本地桩，拆分为独立头/实现。
// ENUM_SERVER_GROUP 在全局命名空间（CEnvironment.h）定义，参与
// DB_GuildExpBookDelete::makeRequest 的符号：_ZN21DB_GuildExpBookDelete
// 11makeRequestEji17ENUM_SERVER_GROUP，需与 ORIG 一致。

#include "CEnvironment.h"

// 全局作用域 DB 类（无命名空间），与 ORIG 符号一致。
class DB_CreateAccountCargo
{
public:
    // ORIG 调用点无 this 压栈（CreateAccountCargo.md 0x82b0a6 仅 3 栈参），为静态成员
    static void makeRequest(int a, unsigned int b, unsigned int c);
};

class DB_DeleteAccountCargo
{
public:
    // ORIG 调用点无 this 压栈（DeleteAccountCargo.md 0x82b532 仅 2 栈参），为静态成员
    static void makeRequest(int a, unsigned int b);
};

class DB_GuildExpBookDelete
{
public:
    // ORIG 汇编无 this 压栈（仅 3 参），为静态成员；符号 _ZN21DB_GuildExpBookDelete
    // 11makeRequestEji17ENUM_SERVER_GROUP 与 ORIG 一致。
    static void makeRequest(unsigned int idx, int a, ENUM_SERVER_GROUP group);
};

class DB_UpgradeAccountCargo
{
public:
    // ORIG 调用点无 this 压栈（UpgradeAccountCargo.md 0x82b3c4 仅 3 栈参），为静态成员
    static void makeRequest(int a, unsigned int b, unsigned int c);
};

class DB_InsertRandomOptionItemInform
{
public:
    // ORIG 调用点无 this 压栈（WarRoom::HandleGetItem 086beedf call 仅 3 参），故为静态成员
    static void makeRequest(int a, unsigned int b, unsigned char c);
};

class DB_PassPadUpdateFailCnt
{
public:
    // ORIG 调用点无 this 压栈（GameWorld::ResetAllPassPadFailCount 086cca7c call 仅 3 栈参），
    // 定义为静态成员
    static void makeRequest(unsigned int a, unsigned int b, char* c);
};

// DB_PassPadUpdateCancelCnt / DB_SecurityCardUpdateCancelCnt（TSV 修复，与 ORIG
// 符号一致）：_ZN25DB_PassPadUpdateCancelCnt11makeRequestEjjPKc、
// _ZN30DB_SecurityCardUpdateCancelCnt11makeRequestEjj。
class DB_SecurityCardUpdateCancelCnt
{
public:
    void makeRequest(unsigned int a, unsigned int b);
};

class DB_PassPadUpdateCancelCnt
{
public:
    void makeRequest(unsigned int a, unsigned int b, const char* c);
};

// APSystem 命名空间：_SIG_LOAD_ACTION_POINT 唯一声明点在 APSystemTypes.h
// （CleanPvPAsm 新建，含 _TodayRewardItem 嵌套）。
#include "APSystemTypes.h"

namespace APSystem
{
class DB_UpdateActionPoint
{
public:
    // ORIG 调用点无 this 压栈（GameWorld::ResetAllFatigue 086cc656 call 仅 3 栈参；
    // CUser.cpp 同模式静态调用），定义为静态成员
    static void makeRequest(int a, const _SIG_LOAD_ACTION_POINT& sig, bool flag);
};
}

#endif  // GAME_DB_ACCOUNT_CARGO_SYNC_H_