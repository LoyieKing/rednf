// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_P2P_Statistics：ORIG 仅导出弱内联 ctor W @0x0822c696；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_PACKET_P2P_STATISTICS_H_
#define DNF_GAME_PACKET_P2P_STATISTICS_H_

class Packet_P2P_Statistics
{
public:
    Packet_P2P_Statistics();  // ORIG W @0x0822c696
};

#endif  // DNF_GAME_PACKET_P2P_STATISTICS_H_
