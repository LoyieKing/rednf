#ifndef GAME_LWSERVERWRITER_H_
#define GAME_LWSERVERWRITER_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。

#include "LogManager.h"   // LogWriter（LwServerWriter 基类；C1 0x0854e2ae 先调 LogWriter::LogWriter）

// ORIG mangled `13ENUM_LOG_TYPE`（LwServerWriter 构造参数类型）。
// 取值实证：setLogWriterWired（0x0854e3db）以 2/1 构造；其余值【推断】。
enum ENUM_LOG_TYPE
{
    ENUM_LOG_TYPE_0 = 0,
    ENUM_LOG_TYPE_1 = 1,
    ENUM_LOG_TYPE_2 = 2
};

class LwServerWriter : public LogWriter
{
public:
    // ORIG 0x854e2ae _ZN14LwServerWriterC1E13ENUM_LOG_TYPE (size 0x00000025)  报告: docs/class_func_reports/LwServerWriter
    LwServerWriter(ENUM_LOG_TYPE);
    // ORIG 0x8550154 _ZN14LwServerWriterD0Ev (size 0x0000001e)  报告: docs/class_func_reports/LwServerWriter
    virtual ~LwServerWriter();
    // ORIG 0x854e2d4 _ZN14LwServerWriter3logEPKcS1_iS1_ (size 0x00000107)  报告: LwServerWriter/log.md
    // 虚覆盖 LogWriter::log（vtable+8）；Ghidra C 实证返回 void。
    virtual void log(char const*, char const*, int, char const*);

    ENUM_LOG_TYPE m_type;  // +0x04（C1 写 this+4；log 以 type*0x18 取 CLog mutex）
};

#endif  // GAME_LWSERVERWRITER_H_
