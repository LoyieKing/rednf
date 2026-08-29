// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_Forbid_Chat_By_Monitor：ORIG 仅导出弱内联 ctor W @0x0822bc70；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_PACKET_FORBID_CHAT_BY_MONITOR_H_
#define DNF_GAME_PACKET_FORBID_CHAT_BY_MONITOR_H_

class Packet_Forbid_Chat_By_Monitor
{
public:
    Packet_Forbid_Chat_By_Monitor();  // ORIG W @0x0822bc70
};

#endif  // DNF_GAME_PACKET_FORBID_CHAT_BY_MONITOR_H_
