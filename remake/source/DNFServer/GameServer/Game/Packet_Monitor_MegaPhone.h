// 转发 shared/packet/include 权威头（字段布局+static_assert+内联 ctor+set 声明）。
// 原 Game/ 壳头含 ORIG 符号声明（ctor W 0x086d1960、set W 0x086d19b0）但无字段；
// shared 权威同类型（§4.4 合并，同族 Packet_Monitor_MegaPhone_Hyper_Link 先例）。
#ifndef DNF_GAME_PACKET_MONITOR_MEGAPHONE_H_
#define DNF_GAME_PACKET_MONITOR_MEGAPHONE_H_

#include "../../../shared/packet/include/Packet_Monitor_MegaPhone.h"

#endif  // DNF_GAME_PACKET_MONITOR_MEGAPHONE_H_
