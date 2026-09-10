// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CStatisticServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CStatisticServerProxyC2ESsi`（ORIG 0x8472162）；ctor 已是首强符号。
#include <iostream>

#include "CStatisticServerProxy.h"

#include <cstring>

#include "LogManager.h"
#include "Packet_File_Statistic.h"

// ORIG _ZN21CStatisticServerProxyC1ESsi（0x08471f84，T）：
// string 拷贝构造（this+0）+ m_port = param_2（this+4）+ CUdpHandler 构造（this+8）；
// CUdpHandler 构造可能抛异常，gcc 生成 string 析构清理（ORIG try/catch 同形）。
#include "CStatisticServerProxy/CStatisticServerProxy.inc"
}

// ORIG _ZN21CStatisticServerProxy4InitEv（0x08471fda，T）：
// m_port 为 0 直接成功；否则初始化 UDP 客户端套接字，失败经 LogManager 记日志
// 返回 false（日志函数名参数为 __PRETTY_FUNCTION__ 局部静态，P118）。
#include "CStatisticServerProxy/Init.inc"
}

// ORIG _ZN21CStatisticServerProxy10SendPacketEPci（0x08472044，T）：
// m_port 非 0 时经 CUdpHandler 发 UDP 包（端口取 m_port 低 16 位，IP 为 m_ip）。
#include "CStatisticServerProxy/SendPacket.inc"
}

// ORIG _ZN21CStatisticServerProxy17SendFileStatisticEPKc（0x08472092，T）。
#include "CStatisticServerProxy/SendFileStatistic.inc"
}

// ORIG _ZN21CStatisticServerProxy17SendFileStatisticEPKcS1_（0x084720b4，T）：
// Packet_File_Statistic 头 + 双缓冲（文件名 m_fileName / 内容 m_fileData），
// packetSize（+2 ushort）作发送长度。
#include "CStatisticServerProxy/SendFileStatistic_1.inc"
}
