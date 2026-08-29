#ifndef GAME_INTER_DISPATCHERARADONLY_H_
#define GAME_INTER_DISPATCHERARADONLY_H_

// df_game_r 还原 —— Inter_DispatcherAradOnly（ARAD 专属 INTER 内部分发入口，
// ARAD::DISPATCHER::Arad_InternalDispatcher 容器包装）。ORIG：C1 0x084e7eb8 /
// dispatch_sig 0x084e7f2c / D1 0x084f5a1c / D0 0x084f5a7e（W）；对象
// sizeof=0x1c（vptr + Arad_InternalDispatcher map 0x18）。C1 汇编：
// ISigDispatcher() 基类 + 设 vptr(0x8c8e410) + Arad_InternalDispatcher C1 +
// Init()。dispatch_sig 读 data 前 4 字节 code → GetDispatcher → 空则
// cMyTrace(0x6ba2,5)+getCurCharacNo 返回 0x6ba3；否则虚调槽2 dispatch_sig
// (data+4, len-4)。
// 报告：docs/class_func_reports/Inter_DispatcherAradOnly/。

#include "InterDispatcher.h"          // ISigDispatcher
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::Arad_InternalDispatcher

class CUser;

class Inter_DispatcherAradOnly : public ISigDispatcher
{
public:
    Inter_DispatcherAradOnly();          // ORIG T 0x084e7eb8
    virtual ~Inter_DispatcherAradOnly(); // ORIG W D1 0x084f5a1c / D0 0x084f5a7e
    virtual int dispatch_sig(CUser* user, char* data, int len);  // ORIG T 0x084e7f2c

private:
    ARAD::DISPATCHER::Arad_InternalDispatcher m_disp;  // +0x04（ORIG C1 this+0x4 构造 + Init）
};

#endif  // GAME_INTER_DISPATCHERARADONLY_H_
