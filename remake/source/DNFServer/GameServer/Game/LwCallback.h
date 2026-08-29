#ifndef GAME_LWCALLBACK_H_
#define GAME_LWCALLBACK_H_

#include "LogManager.h"

#include <functional>

// LwCallback：LogWriter 的 std::function 回调实现。
// 证据：docs/class_func_reports/LwCallback/（README/LwCallback/LwCallback_08ad3ab8/
// getCallback/setCallback/log 五报告）+ objdump D1/D0 与 vtable@0x08e32b38。
// 布局：基类 LogWriter(vptr) 4B + std::function m_callback @ +0x4（sizeof 0x14）。
// vtable 三槽 = D1(0x08ad447a)/D0(0x08ad44dc)/log(0x08ad3b56)——log 虚覆盖 writeLog。
// log/setCallback 返回 void、getCallback 返回 std::function（Ghidra C + ret $0x4 实证）。

class LwCallback : public LogWriter
{
public:
    // ORIG 0x8ad3a6a _ZN10LwCallbackC1Ev (size 0x0000004e)  报告: docs/class_func_reports/LwCallback
    LwCallback();
    // ORIG 0x8ad3ab8 _ZN10LwCallbackC1ESt8functionIFvPKcS2_iS2_EE (size 0x00000055)  报告: docs/class_func_reports/LwCallback
    LwCallback(std::function<void (char const*, char const*, int, char const*)>);
    // ORIG 0x8ad447a _ZN10LwCallbackD1Ev (size 0x00000062) / 0x8ad44dc D0Ev (size 0x1e)（报告缺失，objdump 取证）
    virtual ~LwCallback();
    // ORIG 0x8ad3b38 _ZN10LwCallback11setCallbackESt8functionIFvPKcS2_iS2_EE (size 0x1d)  报告: LwCallback/setCallback.md
    void setCallback(std::function<void (char const*, char const*, int, char const*)>);
    // ORIG 0x8ad3b56 _ZN10LwCallback3logEPKcS1_iS1_ (size 0x4a)  报告: LwCallback/log.md（vtable+8，虚覆盖 writeLog）
    virtual void log(char const*, char const*, int, char const*);
    // ORIG 0x8ad3b0e _ZNK10LwCallback11getCallbackEv (size 0x2a)  报告: LwCallback/getCallback.md
    std::function<void (char const*, char const*, int, char const*)> getCallback() const;

private:
    std::function<void (char const*, char const*, int, char const*)> m_callback;
};

#endif  // GAME_LWCALLBACK_H_
