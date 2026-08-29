#ifndef GAME_DB_DISPATCHERTAIWAN_H_
#define GAME_DB_DISPATCHERTAIWAN_H_

// df_game_r 还原 —— DB_DispatcherTaiwan（台湾 DB 内部分发入口，TaiwanDBDispatcher
// 容器包装）。ORIG：C1 0x0844c9d4 / dispatch 0x0844ca48 / D1 0x0845a7f4 /
// D0 0x0845a856（W）；对象 sizeof=0x1c（new $0x1c 实证：vptr + TaiwanDBDispatcher
// map 0x18）。C1 汇编：DBDispatcher() 基类 + 设 vptr(0x8c5df38) +
// TaiwanDBDispatcher C1 + init()。dispatch 读 Stream 头 4 字节 code →
// getDispatcher → 空则 cMyTrace(0xc17a,5) 返回 false；否则虚调槽2 dispatch。
// 报告：docs/class_func_reports/DB_DispatcherTaiwan/。

#include "DBDispatcher.h"
#include "TaiwanDBDispatcher.h"

class Stream;

class DB_DispatcherTaiwan : public DBDispatcher
{
public:
    DB_DispatcherTaiwan();          // ORIG T 0x0844c9d4
    virtual ~DB_DispatcherTaiwan(); // ORIG W D1 0x0845a7f4 / D0 0x0845a856
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844ca48（PRETTY_FUNCTION "virtual bool …"）

private:
    TaiwanDBDispatcher m_disp;      // +0x04（ORIG C1 this+0x4 构造 + init）
};

#endif  // GAME_DB_DISPATCHERTAIWAN_H_
