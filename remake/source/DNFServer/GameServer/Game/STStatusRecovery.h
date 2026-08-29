// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// STStatusRecovery：ORIG 弱内联 ctor W @0x089c30dc、clear W @0x089c30f0；
// 成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STSTATUSRECOVERY_H_
#define DNF_GAME_STSTATUSRECOVERY_H_

struct STStatusRecovery
{
public:
    STStatusRecovery();   // ORIG W @0x089c30dc
    void clear();         // ORIG W @0x089c30f0
};

#endif  // DNF_GAME_STSTATUSRECOVERY_H_
