// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_Monitor_Guild_Chat：ORIG 仅导出弱内联 ctor W @0x086d1904；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_PACKET_MONITOR_GUILD_CHAT_H_
#define DNF_GAME_PACKET_MONITOR_GUILD_CHAT_H_

class Packet_Monitor_Guild_Chat
{
public:
    Packet_Monitor_Guild_Chat();   // ORIG W @0x086d1904
};

#endif  // DNF_GAME_PACKET_MONITOR_GUILD_CHAT_H_
