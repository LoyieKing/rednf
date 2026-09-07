#ifndef DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CFAIRPVPSCORE_H
#define DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CFAIRPVPSCORE_H

// ============================================================================
// fair_pvp::CFairPvPScore —— 公平 PvP 分数/训练结算容器（ORIG df_game_r）
// 布局与符号依据 docs/class_func_reports/fair_pvp__CFairPvPScore/*.md 与
// objdump（ctor 0x84a47dc / dtor 0x84a4850 / _reset 0x84a5942 /
// UpdateScore 0x84a5a28 / loadData 0x84a48f8 / getData 0x84a55a8 /
// _saveData 0x84a57fe / MakePacketSocre 0x84a5cc2）。
// W 内联 setter/getter 组：SetGiveItem 0x8234f74、SetDailyPlayCount
// 0x8234f92、SetRemainBonusCount 0x8234fae、GetDailyPlayCount 0x8568ec2、
// SetLastTrainingSealCount 0x8568ece、SetLastTrainingSealBonusCount
// 0x8568ede、GetLastTrainingSealCount 0x85dfdac、
// GetLastTrainingSealBonusCount 0x85dfdbc、GetRemainBonusCount 0x85dfdcc。
// ============================================================================
#include <list>
#include <map>

#include "CBoosterGage.h"  // charac_expand::CData

class PacketGuard;
class CUser;

#include "CCompoundAvatar.h"  // ENUM_CHARACTERJOB（守卫幂等）

// 三项计分结构（UpdateScore 对新键插入后累加 value+0/+4/+8；
// MakePacketSocre/loadData 按同样三个 int 序列化）。
// ORIG 中该类型位于全局作用域（mangled 无 N...E 嵌套前缀：
//   _ZNKSt3mapIN8fair_pvp6JobKeyE25FAIR_PVP_SCORE_MODE_COUNTSt4lessIS1_...
// ），故定义在命名空间外。
struct FAIR_PVP_SCORE_MODE_COUNT
{
    int m_cnt0;  // value+0x00
    int m_cnt1;  // value+0x04
    int m_cnt2;  // value+0x08
};  // sizeof 0x0c



namespace fair_pvp
{

// JobKey —— 职业键（ctor 0x84a643a W；operator< 0x84a63f2 W：
// 先比 int job，相等再比 unsigned char 子键）。
class JobKey
{
public:
    JobKey(ENUM_CHARACTERJOB job, unsigned char sub);

    bool operator<(const JobKey& rhs) const;

    int           m_job;   // +0x00
    unsigned char m_sub;   // +0x04
};  // sizeof 0x08

// CFairPvPScore 本体。charac_expand::CData 基类：vptr@+0x00、
// m_bIsSave@+0x04。
class CFairPvPScore : public charac_expand::CData
{
public:
    CFairPvPScore();                                     // ORIG C1/C2 0x84a47dc
    ~CFairPvPScore();                                    // ORIG D1/D2 0x84a4850、D0 0x84a48da

    virtual void ResetDaily();                           // ORIG W 0x84a645a
    virtual bool loadData(CUser* pUser, char* pData);    // ORIG T 0x84a48f8
    virtual void getData(char* pData) const;             // ORIG T 0x84a55a8
    virtual bool _saveData(CUser* pUser);                // ORIG T 0x84a57fe
    virtual void _reset();                               // ORIG T 0x84a5942

    void UpdateScore(int jobId, int subKey, int mode,
                     int cnt0, int cnt1, int cnt2, int resultKind);  // 0x84a5a28
    void MakePacketSocre(PacketGuard& pkt, int mode) const;          // 0x84a5cc2

    void SetGiveItem(int v);                             // 0x8234f74（字节写 +0x5b 后 alter）
    unsigned char GetGiveItem() const;                   // 0x8234f64（movzbl 直返 +0x5b）
    int  GetDailyPlayCount();                            // 0x8568ec2
    void SetDailyPlayCount(int v);                       // 0x8234f92（负值钳 0 后写 +0x50）
    void SetRemainBonusCount(int v);                     // 0x8234fae（字节 +0x5a）
    void SetLastTrainingSealCount(int v);                // 0x8568ece（字节 +0x58）
    void SetLastTrainingSealBonusCount(int v);           // 0x8568ede（字节 +0x59）
    int  GetRemainBonusCount();                          // 0x85dfdcc
    int  GetLastTrainingSealCount();                     // 0x85dfdac
    int  GetLastTrainingSealBonusCount();                // 0x85dfdbc

    std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT> m_jobScore_;        // +0x08（0x18）
    std::list<unsigned char> m_recentResults_;                      // +0x20（最多保留 10 条）

    // 模式统计记录 ×2（每模式连续三项计数，loadData/getData 按 0xc 步进序列化）
    int m_modeA0;  // +0x28
    int m_modeA1;  // +0x2c
    int m_modeA2;  // +0x30
    int m_modeB0;  // +0x34
    int m_modeB1;  // +0x38
    int m_modeB2;  // +0x3c

    int m_field40;              // +0x40（resultKind==2 计数）
    int m_field44;              // +0x44（resultKind==3 计数）
    int m_winStreak;            // +0x48（cnt0!=0&&cnt1==0 连胜链长）
    int m_bestWinStreak;        // +0x4c
    unsigned int m_dailyPlayCount;       // +0x50（与 SetDailyPlayCount(int) 形参异型化，
                                         // NL7 判据：4.4 store 展开序翻 RHS 先=ORIG 形；
                                         // ORIG jns 钳负后 v≥0，无符号读取无语义差）
    int m_field54;              // +0x54（上次结算时间）

    unsigned char m_lastTrainingSealCount;        // +0x58
    unsigned char m_lastTrainingSealBonusCount;   // +0x59
    unsigned char m_remainBonusCount;             // +0x5a
    unsigned char m_giveItem;                     // +0x5b
};

}  // namespace fair_pvp

#endif  // DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CFAIRPVPSCORE_H
