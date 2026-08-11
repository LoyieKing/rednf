// df_dbmw_r - CQueryCounter::WriteFileLog 独立编译单元（2026-08-11）
// ORIG 中该方法的 memset 为 dword rep stos（默认 march 的 4.4.6 形态），
// 与 QueryCounter.cpp 其余方法（-march=i486 -mtune=pentiumpro 的 fucompp+sahf
// 形态）工具链配置不同，按证据拆分为两个 TU：本文件用默认 march 编译。
#include "DBMWCommon.h"

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

void CQueryCounter::WriteFileLog()
{
    // {0} 初始化触发 GCC 内联 rep stos（ORIG 形态），memset() 调用会生成 call
    char buf[0x400] = {0};
    for (int i = 1; i <= 0x140; i++)
        sprintf(buf, "%s\t%d(%d)", buf, i, m_counts[i]);
    // __FUNCTION__ 生成 _ZZ...E12__FUNCTION__ 符号（ORIG 形态），字面串会进 rodata
    CMyFileLog log(__FUNCTION__, 0x68);
    log("./log/QueryCount", "%s", buf);
}

// 单例守卫代码在 -march=i486 -mtune=pentiumpro 下生成 `mov $0x0,%bl`，
// ORIG 为 `mov $0x0,%ebx`；本 TU 用默认 march 编译，守卫形态与 ORIG 一致
// （2026-08-11 第 7 轮，按 .comment/A-B 证据拆 TU）。
CQueryCounter* CQueryCounterInstance()
{
    static CQueryCounter instance;
    return &instance;
}
