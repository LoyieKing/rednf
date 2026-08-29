// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// StatisticsPacket：ORIG 仅导出弱内联 ctor W @0x08610132；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STATISTICSPACKET_H_
#define DNF_GAME_STATISTICSPACKET_H_

class StatisticsPacket
{
public:
    StatisticsPacket();   // ORIG W @0x08610132
};

#endif  // DNF_GAME_STATISTICSPACKET_H_
