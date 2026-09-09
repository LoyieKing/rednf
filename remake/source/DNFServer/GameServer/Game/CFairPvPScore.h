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
    // [b28d-solo-CFairPvPScore 试验] unsigned 字段 + int 形参 ⇒ ORIG 084a5aac
    // 的 `mov mem,%ecx; mov param,%edx; lea (%ecx,%edx,1)` 四条形（P274）。
    unsigned int m_cnt0;  // value+0x00
    unsigned int m_cnt1;  // value+0x04
    unsigned int m_cnt2;  // value+0x08
};  // sizeof 0x0c



namespace fair_pvp
{

// JobKey —— 职业键（ctor 0x84a643a W；operator< 0x84a63f2 W：
// 先比 job，相等再比子键字节）。
// [b28 P211 反形异型化] 字段类型由 int/unsigned char 恢复为与 ORIG ctor 发射形
// 一致的 enum/signed char：探针 /tmp/l2_p2.cpp JK 形 C1 与 ORIG 0x84a643a 13 条
// 逐字全等（store1 LHS 地址先 this→value；store2 uchar 形参→schar 字段触发转换
// 中转 = RHS 先 movzbl+this 重载）。sizeof 0x08、+0x00/+0x04 偏移不变；
// operator< 为 byte cmp + jge（movzbl/movsbl 低 8 位比较结果同向），语义不变。
class JobKey
{
public:
    JobKey(ENUM_CHARACTERJOB job, unsigned char sub);

    bool operator<(const JobKey& rhs) const;

    ENUM_CHARACTERJOB m_job;  // +0x00（4B，比较按 int 宽度 cmpl）
    signed char       m_sub;  // +0x04（ORIG operator< 0x84a63f2 以 byte cmp+jge
                               // 有符号序比较，P196 实证值域可 ≥0x80）
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
    int GetGiveItem() const;                             // 0x8234f64（双 movzbl：int 返回 + uchar 位域/标量读，0x8234f64 切片实证）
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
    // [b28 P172] _reset 0x084a5942 循环体 ORIG 对 st[i][k] 发「this→ecx、i*3→
    // add/add+shl、lea (ecx,ea)、add $0x28/0x2c/0x30 尾折叠」六步全链（探针
    // /tmp/l2_p6.cpp r1 与 4.4 逐字同形）⇒ 原码为 2D 数组成员而非 6 标量；
    // m_modeA0..m_modeB2 = m_modeStats[0..1][0..2]，偏移/sizeof 不变（+0x28..0x3c）。
    // [b28d-solo-CFairPvPScore 试验] P274 判据：unsigned 字段 + int 形参 ⇒
    // 4.4 产 `mov mem,%edx; mov param,%eax; add %eax,%edx`（ORIG 084a5b4a 形）。
    unsigned int m_modeStats[2][3];  // +0x28（A=m_modeStats[0][0..2]，B=[1][0..2]）

    // [b28d-solo-CFairPvPScore 试验] 四字段 unsigned 化：ORIG MakePacketSocre
    // 084a5e02/084a5e1f 的 put_int 实参为两段（unsigned 字段→int 形参异型），
    // UpdateScore 084a5c8c 的 `(unsigned)best < (unsigned)cur` 为单段。
    unsigned int m_field40;              // +0x40（resultKind==2 计数）
    unsigned int m_field44;              // +0x44（resultKind==3 计数）
    unsigned int m_winStreak;            // +0x48（cnt0!=0&&cnt1==0 连胜链长）
    unsigned int m_bestWinStreak;        // +0x4c
    unsigned int m_dailyPlayCount;       // +0x50（与 SetDailyPlayCount(int) 形参异型化，
                                         // NL7 判据：4.4 store 展开序翻 RHS 先=ORIG 形；
                                         // ORIG jns 钳负后 v≥0，无符号读取无语义差）
    // [b28d-solo-CFairPvPScore] 与 SIG.m_lastPlayTime(int) 异型 ⇒ getData
    // 084a57d3 / loadData 084a4b25 两段中转（ORIG 实证）；_saveData 直写
    // SIG 字段不受影响。
    unsigned int m_field54;     // +0x54（上次结算时间）

    unsigned char m_lastTrainingSealCount;        // +0x58
    unsigned char m_lastTrainingSealBonusCount;   // +0x59
    unsigned char m_remainBonusCount;             // +0x5a
    unsigned char m_giveItem;                     // +0x5b
};

}  // namespace fair_pvp

#endif  // DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CFAIRPVPSCORE_H
