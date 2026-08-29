#ifndef GAME_DB_UPDATEINVENTORYCAPACITY_H_
#define GAME_DB_UPDATEINVENTORYCAPACITY_H_

// df_game_r 还原 —— DB_UpdateInventoryCapacity（背包容量更新 DB 请求，DBThread 派发）。
// 唯一声明点（原定义于 DB_UpdateInventoryCapacity.cpp，本头拆出供多 TU include）。
// 语义：向 DB 队列发一条请求，表头 0x2ae + -1，正文为 serverGroup、characNo(uint)、
// capacity；dispatch 侧据此执行 "upDate inventory set inventory_capacity=..."。
#include "DBDispatcher.h"   // 基类（dispatch 虚覆盖；唯一继承点）
#include "CEnvironment.h"   // ENUM_SERVER_GROUP / ENUM_DB_HANDLE_IDX

class Stream;

class DB_UpdateInventoryCapacity : public DBDispatcher
{
public:
    DB_UpdateInventoryCapacity() {}   // ORIG C1 0x844fa0a（sz 0x4: base ctor+vptr only）
    // ORIG T 0x08444d12：mangled _ZN26DB_UpdateInventoryCapacity11makeRequestE17ENUM_SERVER_GROUPiji
    // 第 2 个 int（uid）在 ORIG 汇编中不写入流（仅入参），签名按 ORIG mangled 保留。
    static void makeRequest(ENUM_SERVER_GROUP serverGroup, int uid,
                            unsigned int characNo, int capacity);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_UPDATEINVENTORYCAPACITY_H_
