// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// STSummonMonster：ORIG 导出弱内联 ctor W @0x089c31d2 与 clear() W @0x089c31e6；
// 成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STSUMMONMONSTER_H_
#define DNF_GAME_STSUMMONMONSTER_H_

class STSummonMonster
{
public:
    STSummonMonster();   // ORIG W @0x089c31d2
    void clear();        // ORIG W @0x089c31e6
};

#endif  // DNF_GAME_STSUMMONMONSTER_H_
