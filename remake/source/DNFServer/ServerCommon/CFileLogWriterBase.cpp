#include "DNFFileLog.h"

// 原始：CFileLogWriter 构造/析构在独立 TU（-fno-exceptions，无 <iostream> 初始化桩）。
// 反汇编验证：原始 C1/C2/D1/D2 均无 EH 清理（无 map D1/_Unwind_Resume），
// 而 writeLog/writeRawLog 含 CGuard 隐式清理（-fexceptions）。
//
// 构建：必须 -fno-exceptions（coserver build-coserver.sh 已特殊处理；
// auction 侧 build-auction.sh 需同样对 CFileLogWriterBase.cpp 加 -fno-exceptions，
// 否则 gcc 4.4 对 map 成员构造固定生成 EH landing pad / _Unwind_Resume）。

CFileLogWriter::CFileLogWriter() {
}

CFileLogWriter::~CFileLogWriter() {
    // 原始析构仅销毁 map 与 mutex，不释放 CFLog 指针（与原始一致）
}
