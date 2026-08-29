#ifndef DNF_GAME_DB_ARADDATABASEDISPATCHER_H_
#define DNF_GAME_DB_ARADDATABASEDISPATCHER_H_

// df_game_r 还原 —— DB_AradDatabaseDispatcher（ARAD 专属 DB 内部分发入口，
// ARAD::DISPATCHER::Arad_DatabaseDispatcher 容器包装）。ORIG：C1 0x0844bc38 /
// dispatch 0x0844bcac / D1 0x0845aa96 / D0 0x0845aaf8（W）；对象 sizeof=0x1c
// （new $0x1c 实证：vptr + Arad_DatabaseDispatcher map 0x18）。C1 汇编：
// DBDispatcher() 基类 + 设 vptr(0x8c5dff8) + Arad_DatabaseDispatcher C1 +
// Init()。dispatch 读 Stream 头 4 字节 code → GetDispatcher → 空则
// cMyTrace(0xbc8b,5) 返回 false；否则虚调槽2 dispatch。
// 报告：docs/class_func_reports/DB_AradDatabaseDispatcher/。

#include "DBDispatcher.h"
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::Arad_DatabaseDispatcher

class Stream;

class DB_AradDatabaseDispatcher : public DBDispatcher
{
public:
    DB_AradDatabaseDispatcher();          // ORIG T 0x0844bc38
    virtual ~DB_AradDatabaseDispatcher(); // ORIG W D1 0x0845aa96 / D0 0x0845aaf8
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844bcac（PRETTY_FUNCTION "virtual bool …"）

private:
    ARAD::DISPATCHER::Arad_DatabaseDispatcher m_disp;  // +0x04（ORIG C1 this+0x4 构造 + Init）
};

#endif  // DNF_GAME_DB_ARADDATABASEDISPATCHER_H_
