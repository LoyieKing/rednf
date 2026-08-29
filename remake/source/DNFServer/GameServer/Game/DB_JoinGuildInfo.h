#ifndef GAME_DB_JOIN_GUILD_INFO_H_
#define GAME_DB_JOIN_GUILD_INFO_H_

// DB_JoinGuildInfo / DB_CancelJoinGuild（DspD4 B4 集成）。
// 原 W8LT_DBTask.cpp 仅 TU-local 空壳声明；此处为唯一权威声明点。
// ORIG 符号（nm 核实）：
//   _ZN16DB_JoinGuildInfo11makeRequestEi17ENUM_SERVER_GROUPj  T @0x0843d5fa
//   _ZN18DB_CancelJoinGuild11makeRequestEijj                  T @0x0843d78c
// 定义位于 Dispatchers.cpp 尾部（同批集成）。

#include "CEnvironment.h"   // ENUM_SERVER_GROUP（全局命名空间）

class DB_JoinGuildInfo
{
public:
    // ORIG 调用点（Dispatcher_JoinGuildInfo::process 0x081d549a）无 this 压栈，静态成员
    static void makeRequest(int uid, ENUM_SERVER_GROUP serverGroup, unsigned int characNo);
};

class DB_CancelJoinGuild
{
public:
    // ORIG 调用点（Dispatcher_CancelJoinGuild::process 0x081d509c）无 this 压栈，静态成员
    static void makeRequest(int uid, unsigned int requestIdx, unsigned int characNo);
};

#endif  // GAME_DB_JOIN_GUILD_INFO_H_
