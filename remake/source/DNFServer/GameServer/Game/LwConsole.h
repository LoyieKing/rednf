#ifndef GAME_LWCONSOLE_H_
#define GAME_LWCONSOLE_H_

#include "LogManager.h"

// LwConsole：LogWriter 的控制台输出实现（printf/puts 直出）。
// 证据：docs/class_func_reports/LwConsole/LwConsole.md（C1）+ objdump 取证其余符号
// （报告缺失：isSourcePositionVisible/setSourcePositionVisible/log/D1/D0 按
// nm 地址 0x08ad3a06/0x08ad3a12/0x08ad3a2a/0x08ad44fa/0x08ad452a 转写）。
// 布局：基类 LogWriter(vptr) 4B + bool m_sourcePositionVisible @ +0x4。
// vtable@0x08e32b48 三虚槽 = D1(0x08ad44fa)/D0(0x08ad452a)/log(0x08ad3a2a)
// —— log 虚覆盖 writeLog（vtable+8）。
class LwConsole : public LogWriter
{
public:
    // ORIG 0x08ad39e2 _ZN9LwConsoleC1Ev (size 0x23)  报告: docs/class_func_reports/LwConsole
    LwConsole();
    // ORIG 0x08ad44fa _ZN9LwConsoleD1Ev (size 0x30) / 0x08ad452a D0Ev (size 0x1e)（报告缺失，objdump 取证）
    virtual ~LwConsole();
    // ORIG 0x08ad3a06 _ZNK9LwConsole23isSourcePositionVisibleEv (size 0xc)（报告缺失，objdump 取证）
    bool isSourcePositionVisible() const;
    // ORIG 0x08ad3a12 _ZN9LwConsole24setSourcePositionVisibleEb (size 0x18)（报告缺失，objdump 取证）
    void setSourcePositionVisible(bool visible);
    // ORIG 0x08ad3a2a _ZN9LwConsole3logEPKcS1_iS1_ (size 0x3f)（报告缺失，objdump 取证；vtable+8，虚覆盖 writeLog）
    virtual void log(char const* file, char const* func, int line, char const* msg);

private:
    bool m_sourcePositionVisible;
};

#endif  // GAME_LWCONSOLE_H_
