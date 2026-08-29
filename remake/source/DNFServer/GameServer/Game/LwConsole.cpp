// LwConsole.cpp —— LwConsole 类实现（仅 ORIG）
// 报告: docs/class_func_reports/LwConsole/LwConsole.md（仅 C1）
// isSourcePositionVisible/setSourcePositionVisible/log/D1/D0 报告缺失，
// 按 nm 地址 0x08ad3a06/0x08ad3a12/0x08ad3a2a/0x08ad44fa/0x08ad452a objdump 取证：
//   isSourcePositionVisible: movzbl 0x4(%eax) 直返（bool 直返，P32）
//   setSourcePositionVisible: mov %al 暂存+movzbl 回读+mov %dl store（bool 形参，P33）
//   log: movzbl 0x4; test %al; je 跳过 printf("%s(%d) %s: ",file,line,func)；
//        尾部无条件 puts(msg)；格式串 @0x8e32b14 = "%s(%d) %s: "
//   D1: vptr 复位 + LogWriter::~LogWriter + __in_chrg 检查；D0 = D1 + operator delete。
// 全部符号与 ORIG mangled 一致（nm 核验）。

#include "LwConsole.h"

#include <cstdio>

LwConsole::LwConsole()
    : m_sourcePositionVisible(false)
{
}

LwConsole::~LwConsole()
{
}

bool LwConsole::isSourcePositionVisible() const
{
    return m_sourcePositionVisible;
}

void LwConsole::setSourcePositionVisible(bool visible)
{
    m_sourcePositionVisible = visible;
}

void LwConsole::log(char const* file, char const* func, int line, char const* msg)
{
    if (m_sourcePositionVisible)
        printf("%s(%d) %s: ", file, line, func);
    puts(msg);
}
