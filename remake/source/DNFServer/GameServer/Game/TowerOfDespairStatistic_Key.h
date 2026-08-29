// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_324 ctor W 0x08374ca2：两个 dword 清零）+ 定义提取自
// Game/CDungeon.cpp:497-509。注意 Game/CPartyDepsD2.cpp:1623 有一处 int 版重复
// 定义（unsigned int vs int，语义一致）；头落成后由宿主统一收敛两处 cpp 局部段。
#ifndef DNF_GAME_TOWEROFDESPAIRSTATISTIC_KEY_H_
#define DNF_GAME_TOWEROFDESPAIRSTATISTIC_KEY_H_

struct TowerOfDespairStatistic_Key
{
    unsigned int m_0;   // +0x00（packet+0xe，当日层写入口）
    unsigned int m_4;   // +0x04

    TowerOfDespairStatistic_Key();  // ORIG W 0x08374ca2
};

#endif  // DNF_GAME_TOWEROFDESPAIRSTATISTIC_KEY_H_
