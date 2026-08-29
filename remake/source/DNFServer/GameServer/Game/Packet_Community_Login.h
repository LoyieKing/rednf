// 转发到 shared/packet/include 权威定义（OrigCommPx 批：HdrCreate 骨架缺消费方
// 需要的成员字段/内联 ctor，权威头含 ORIG 反汇编定名布局 + inline ctor，
// 与 ORIG 弱内联 W 符号口径一致；SendLogin 0x0846ca06 消费 m_id/gameUserInfo/
// buddyCount/buddies）。
#include "../../../shared/packet/include/Packet_Community_Login.h"
