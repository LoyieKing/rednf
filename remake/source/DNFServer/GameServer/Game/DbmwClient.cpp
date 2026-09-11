// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DbmwClient/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// DbmwClient.cpp —— 按报告 docs/class_func_reports/DbmwClient/*.md 忠实转写。
#include "DbmwClient.h"

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10DbmwClientC2Ev 的前提，P338）
#include "GameTypes.h"     // cMyTrace
#include "LogManager.h"

// ===================== 生命周期 =====================

#include "DbmwClient/DbmwClient.inc"


#include "DbmwClient/dtor_DbmwClient.inc"


#include "DbmwClient/set_query.inc"


#include "DbmwClient/Init.inc"


#include "DbmwClient/SetServerAddress.inc"
{
    // ORIG @0x08120af6。
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
}
#include "DbmwClient/SendHeader.inc"
{
    // ORIG @0x08120654：18 字节头 {0x15,type,len+0x12,12B zero} 后 send。
    // ORIG 逐字段直 disp store（total 槽即 header+2）→ 具名视图结构。
    struct __attribute__((packed)) {
        unsigned char b0;
        unsigned char b1;
        int total;
        int z2;
        int z3;
        int z4;
    } header;
    header.b0 = 0x15;
    header.b1 = (unsigned char)type;
    header.total = bodyLen + 0x12;
    header.z2 = 0;
    header.z3 = 0;
    header.z4 = 0;

    ssize_t sent = ::send(m_sock, &header, 0x12, 0);
    if (sent <= 0)
        return false;
    return true;
}

#include "DbmwClient/RegistToMiddleware.inc"


#include "DbmwClient/ReceiveRegistAck.inc"


#include "DbmwClient/Regist.inc"


#include "DbmwClient/Connect.inc"


#include "DbmwClient/Close.inc"


#include "DbmwClient/ReadData.inc"


#include "DbmwClient/DoQuery.inc"


#include "DbmwClient/DoReceive.inc"


#include "DbmwClient/Reconnect.inc"


#include "DbmwClient/exec.inc"


#include "DbmwClient/getAffectedRowCount.inc"


// IsInitialized/IsConnected 头内联（W）——cpp 不再重复定义。

#include "DbmwClient/Error.inc"

