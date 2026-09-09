#ifndef GAME_INTER_DISPATCHERTAIWAN_H_
#define GAME_INTER_DISPATCHERTAIWAN_H_

// df_game_r 还原 —— Inter_DispatcherTaiwan（台湾 INTER 内部分发入口，
// TaiwanInternalDispatcher 容器包装）。ORIG：C1 0x084e8bba / dispatch_sig
// 0x084e8c2e / D1 0x084f577a / D0 0x084f57dc（W）；对象 sizeof=0x1c（vptr +
// TaiwanInternalDispatcher map 0x18）。C1 汇编：ISigDispatcher() 基类 + 设
// vptr(0x8c8e350) + TaiwanInternalDispatcher C1 + init()。dispatch_sig 读
// data 前 4 字节 code → getDispatcher → 空则 cMyTrace(0x6dca,5)+getCurCharacNo
// 返回 0x6dcb；否则虚调槽2 dispatch_sig(data+4, len-4)。
// 报告：docs/class_func_reports/Inter_DispatcherTaiwan/。

#include "InterDispatcher.h"          // ISigDispatcher
#include "TaiwanInternalDispatcher.h"

class CUser;

class Inter_DispatcherTaiwan : public ISigDispatcher
{
public:
    Inter_DispatcherTaiwan();           // ORIG T 0x084e8bba
    // ORIG D0/D1/D2 全为 W 弱符号（COMDAT 区 0x084f577a/0x084f57dc）⇒ 原码为
    // 类内 inline 空析构；仅声明+外联定义会发射 T 强符号，故此处保留空体。
    virtual ~Inter_DispatcherTaiwan() {}  // ORIG W D1 0x084f577a / D0 0x084f57dc
    virtual int dispatch_sig(CUser* user, char* data, int len);  // ORIG T 0x084e8c2e

private:
    TaiwanInternalDispatcher m_disp;    // +0x04（ORIG C1 this+0x4 构造 + init）
};

#endif  // GAME_INTER_DISPATCHERTAIWAN_H_
