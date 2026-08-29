// 转发到 shared/packet/include 权威定义（OrigCommPx 批：HdrCreate 骨架缺消费方
// 需要的成员字段/内联 ctor，权威头含 ORIG 反汇编定名布局 + inline ctor；
// SendLogout 0x0846cb44 消费 m_id/m_characNo）。
#include "../../../shared/packet/include/Packet_Community_Logout.h"
