#ifndef GAME_CUSERPLAYTIMESTATISTICMGR_H_
#define GAME_CUSERPLAYTIMESTATISTICMGR_H_

#include <map>

// stUserPlayInfo_t —— 单账号游玩统计值（UpdateUserPlayTime 0x0860cc38
// 写 +0（累计秒 += 参数）与 +4（打点次数 += 1）；InsertUserPlayTime 0x0860cd14
// 将 value[0]/value[1] 逐条写入 SIG_USER_PLAY_TIME_STATISTIC）。
struct stUserPlayInfo_t
{
    int m_playSec;   // +0x00
    int m_count;     // +0x04
};                   // 0x8

// CUserPlayTimeStatisticMgr —— 在线用户游玩时长统计（Statistics.cpp 域）。
// ORIG 类符号：C1/C2 0x086102c0（W，构造内嵌 map）、D1/D2 0x086107ec（W）、
// InsertUserPlayTime 0x0860cd14（T）、UpdateUserPlayTime 0x0860cc38（T）。
// 实例为 GetInstanceUserPlayTimeStatisticMgr 0x0860cba5（T）内静态单例。
class CUserPlayTimeStatisticMgr
{
public:
    CUserPlayTimeStatisticMgr();
    ~CUserPlayTimeStatisticMgr();
    int InsertUserPlayTime();                                    // 0x0860cd14
    int UpdateUserPlayTime(unsigned int accId, int playSec);     // 0x0860cc38

private:
    std::map<unsigned int, stUserPlayInfo_t> m_userPlayInfo;     // +0x00（0x86108c2 map）
};

// ORIG T 0x0860cba5（__cxa_guard + __cxa_atexit 标准静态单例，instance 0x09473444）。
CUserPlayTimeStatisticMgr* GetInstanceUserPlayTimeStatisticMgr();

#endif  // GAME_CUSERPLAYTIMESTATISTICMGR_H_
