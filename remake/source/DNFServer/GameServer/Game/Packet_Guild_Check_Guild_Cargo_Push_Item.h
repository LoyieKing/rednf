// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_Guild_Check_Guild_Cargo_Push_Item：ORIG 仅导出弱内联 ctor W @0x0822bec2；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H_
#define DNF_GAME_PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H_

class Packet_Guild_Check_Guild_Cargo_Push_Item
{
public:
    Packet_Guild_Check_Guild_Cargo_Push_Item();   // ORIG W @0x0822bec2
};

#endif  // DNF_GAME_PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H_
