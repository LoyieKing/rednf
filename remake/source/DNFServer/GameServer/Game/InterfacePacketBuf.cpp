// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InterfacePacketBuf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "InterfacePacketBuf.h"

#include "InterfacePacketBuf/InterfacePacketBuf.inc"
{
}

#include "InterfacePacketBuf/dtor_InterfacePacketBuf.inc"
{
}

// throw() 与头声明同步（见 InterfacePacketBuf.h 的 ORIG EH 形证据注释）
#include "InterfacePacketBuf/AcquirePacketBuf.inc"
{
    m_packetBuf = packetBuf;
}

#include "InterfacePacketBuf/bind_packet.inc"
{
    return m_packetBuf->bind_packet(packet, size);
}

#include "InterfacePacketBuf/clear.inc"
{
    m_packetBuf->clear();
}

#include "InterfacePacketBuf/finalize.inc"
{
    m_packetBuf->finalize(isEncryptRequired);
}

#include "InterfacePacketBuf/get.inc"
{
    return m_packetBuf;
}

#include "InterfacePacketBuf/getLastError.inc"
{
    return m_packetBuf->getLastError();
}

#include "InterfacePacketBuf/get_binary.inc"
{
    return m_packetBuf->get_binary(out, size);
}

#include "InterfacePacketBuf/get_index.inc"
{
    return m_packetBuf->get_index();
}

#include "InterfacePacketBuf/get_int.inc"
{
    return m_packetBuf->get_int(out);
}

#include "InterfacePacketBuf/get_len.inc"
{
    return m_packetBuf->get_len();
}

#include "InterfacePacketBuf/get_packet.inc"
{
    return m_packetBuf->get_packet(size);
}

#include "InterfacePacketBuf/get_short.inc"
{
    return m_packetBuf->get_short(out);
}

#include "InterfacePacketBuf/is_finallized.inc"
{
    return m_packetBuf->is_finallized();
}

#include "InterfacePacketBuf/put_binary.inc"
{
    return m_packetBuf->put_binary(data, len);
}

#include "InterfacePacketBuf/put_binary_1.inc"
{
    return m_packetBuf->put_binary(data, len);
}

#include "InterfacePacketBuf/put_byte.inc"
{
    return m_packetBuf->put_byte(value);
}

#include "InterfacePacketBuf/put_byte_1.inc"
{
    return m_packetBuf->put_byte(index, value);
}

#include "InterfacePacketBuf/put_header.inc"
{
    return m_packetBuf->put_header(packetId, packetSize);
}

#include "InterfacePacketBuf/put_int.inc"
{
    return m_packetBuf->put_int(value);
}

#include "InterfacePacketBuf/put_int_1.inc"
{
    return m_packetBuf->put_int(index, value);
}

#include "InterfacePacketBuf/put_item_idx.inc"
{
    return m_packetBuf->put_item_idx(value);
}

#include "InterfacePacketBuf/put_packet.inc"
{
    m_packetBuf->put_packet(item);
}

#include "InterfacePacketBuf/put_short.inc"
{
    return m_packetBuf->put_short(value);
}

#include "InterfacePacketBuf/put_short_1.inc"
{
    return m_packetBuf->put_short(index, value);
}

#include "InterfacePacketBuf/put_str.inc"
{
    return m_packetBuf->put_str(str, len);
}

#include "InterfacePacketBuf/put_str_1.inc"
{
    return m_packetBuf->put_str(str, len);
}

#include "InterfacePacketBuf/set_index.inc"
{
    m_packetBuf->set_index(index);
}
