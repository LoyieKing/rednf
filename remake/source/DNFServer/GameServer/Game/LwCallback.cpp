// LwCallback.cpp —— LwCallback 类实现（仅 ORIG）
// 报告: docs/class_func_reports/LwCallback/{README,LwCallback,LwCallback_08ad3ab8,getCallback,setCallback,log}.md
// D1/D0 无报告，按 objdump 0x08ad447a/0x08ad44dc 取证：D1 = vptr 复位 + ~std::function
// + 异常路径 LogWriter::~LogWriter + __in_chrg 检查；D0 = D1 + operator delete。
// 全部符号与 ORIG mangled 一致（nm 核验）。

#include "LwCallback.h"

LwCallback::LwCallback()
{
}

LwCallback::LwCallback(std::function<void (char const*, char const*, int, char const*)> callback)
    : m_callback(callback)
{
}

LwCallback::~LwCallback()
{
}

void LwCallback::setCallback(std::function<void (char const*, char const*, int, char const*)> callback)
{
    m_callback = callback;
}

void LwCallback::log(char const* file, char const* func, int line, char const* msg)
{
    if (m_callback)
        m_callback(file, func, line, msg);
}

std::function<void (char const*, char const*, int, char const*)>
LwCallback::getCallback() const
{
    return m_callback;
}
