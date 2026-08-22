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
    void makeRequest(int a, unsigned int b, unsigned int c);
};

class DB_DeleteAccountCargo
{
public:
    void makeRequest(int a, unsigned int b);
};

class DB_GuildExpBookDelete
{
public:
    void makeRequest(unsigned int idx, int a, ENUM_SERVER_GROUP group);
};

class DB_UpgradeAccountCargo
{
public:
    void makeRequest(int a, unsigned int b, unsigned int c);
};

class DB_InsertRandomOptionItemInform
{
public:
    void makeRequest(int a, unsigned int b, unsigned char c);
};

class DB_PassPadUpdateFailCnt
{
public:
    void makeRequest(unsigned int a, unsigned int b, char* c);
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

// APSystem 命名空间：_SIG_LOAD_ACTION_POINT 无独立 TU，在此定义。
namespace APSystem
{
struct _SIG_LOAD_ACTION_POINT
{
    char m_pad[4];
};

class DB_UpdateActionPoint
{
public:
    void makeRequest(int a, const _SIG_LOAD_ACTION_POINT& sig, bool flag);
};
}

#endif  // GAME_DB_ACCOUNT_CARGO_SYNC_H_