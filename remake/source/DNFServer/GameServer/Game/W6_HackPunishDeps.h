#ifndef DNF_GAME_W6_HACKPUNISHDEPS_H_
#define DNF_GAME_W6_HACKPUNISHDEPS_H_

// ============================================================================
// WongWork 黑名单 / 反作弊惩罚规则域权威声明（Wave6 W6_HackPunishDeps）。
// 全部布局经 df_game_r 反汇编核实；原 GlobalData.cpp 局部镜像已收编于此。
// ============================================================================

#include <map>
#include <vector>

// 全局容器元素类型（CUser.h 定义；本头仅前向，定义处见 W6_HackPunishDeps.cpp）
struct stLookOuts;
struct tyIPCounters;

#include "CHackAnalyzer.h"        // WongWork::ENUM_HACKTYPE
#include "WongWorkEvents_A.h"     // WongWork::HackTypeInfo_t（7 参 ctor inline）

namespace WongWork
{

#ifdef __GNUC__
#define W6_HP_ASSERT_CAT2(a, b) a##b
#define W6_HP_ASSERT_CAT(a, b) W6_HP_ASSERT_CAT2(a, b)
#define W6_HP_STATIC_ASSERT_SIZEOF(T, N) \
    __extension__ typedef char W6_HP_ASSERT_CAT(__w6_hp_assert_, __LINE__)[(sizeof(T) == (N)) ? 1 : -1]
#endif

// ----------------------------------------------------------------------------
// HackIPCounter_t：ORIG W C1/C2(EjjPcj) @0x080f9892 (size 0x63)。
//   this+0x00=(u16)a；+0x02=(u16)b；+0x04..+0x10 ip 区 memset 0xd 后 strncpy 0xd；
//   +0x14=(u32)cnt。sizeof 0x18。
// ----------------------------------------------------------------------------
struct HackIPCounter_t
{
    unsigned short m_a;      // +0x00
    unsigned short m_b;      // +0x02
    char m_ip[0xd];          // +0x04
    // （隐式填充 +0x11..+0x13）
    unsigned int m_cnt;      // +0x14

    HackIPCounter_t(unsigned int a, unsigned int b, char* ip, unsigned int cnt);
};

// ----------------------------------------------------------------------------
// HackFullIPCounter_t：ORIG W C1/C2(EjjPcj) @0x080f98f6 (size 0x63)。
//   同构但 ip 区长 0x10；+0x14=(u32)cnt。sizeof 0x18。
// ----------------------------------------------------------------------------
struct HackFullIPCounter_t
{
    unsigned short m_a;      // +0x00
    unsigned short m_b;      // +0x02
    char m_ip[0x10];         // +0x04
    unsigned int m_cnt;      // +0x14

    HackFullIPCounter_t(unsigned int a, unsigned int b, char* ip, unsigned int cnt);
};

#ifdef W6_HP_STATIC_ASSERT_SIZEOF
W6_HP_STATIC_ASSERT_SIZEOF(HackIPCounter_t, 0x18);
W6_HP_STATIC_ASSERT_SIZEOF(HackFullIPCounter_t, 0x18);
#endif

// ----------------------------------------------------------------------------
// CAutoPunishRuleBlackIPMgr：key=in_addr(u32 网络序) -> stIPInfo_t{min,max}。
// ctor/reset（ORIG W 0x082a5a88 / 0x082a5a74）定义仍留 GlobalData.cpp。
// ----------------------------------------------------------------------------
class CAutoPunishRuleBlackIPMgr
{
public:
    CAutoPunishRuleBlackIPMgr();
    void reset();

    // ORIG W C1/C2(Ehh) @0x084eb598 (size 0x27)：this+0x00=p1(u8)、+0x01=p2(u8)。
    struct stIPInfo_t
    {
        // 2026-08-31：ORIG sizeof 4（map 节点 _Rb_tree_node 24 = 16+pair(4+4)；
        // new_allocator 24B）——原 m_pad[0x16] 为无证据推断，删至对齐 4。
        unsigned char m_minIp;   // +0x00（区间下界）【推断命名】
        unsigned char m_maxIp;   // +0x01（区间上界）【推断命名】
        char m_pad[2];           // +0x02..+0x04（对齐）

        stIPInfo_t(unsigned char minIp, unsigned char maxIp);
    };

    bool addBlackIP(unsigned long addr, unsigned char minIp,
                    unsigned char maxIp);                      // ORIG W 0x084eb5c0
    int getBlackIPCount();                                     // ORIG W 0x084eb7ca
    bool isBlackIP(char const* ip);                            // ORIG W 0x084eb63e

private:
    std::multimap<unsigned long, stIPInfo_t> m_map;  // +0x00
};

// GetExceptionCnt ORIG 为 sret 返回（ret $0x4）→ 按值返回本 POD。
// ----------------------------------------------------------------------------
struct stHacktypeExceptionInfo_t
{
    unsigned short m_cnt;    // +0x00（ctor 第一参）
    unsigned short m_flag;   // +0x02（ctor 第二参）

    stHacktypeExceptionInfo_t(short cnt, short flag)
        : m_cnt((unsigned short)cnt), m_flag((unsigned short)flag)
    {
    }
};

// ----------------------------------------------------------------------------
// CAutoPunishRuleHackTypeMgr：ctor(W)/reset(W) 定义仍留 GlobalData.cpp。
// ----------------------------------------------------------------------------
class CAutoPunishRuleHackTypeMgr
{
public:
    CAutoPunishRuleHackTypeMgr();
    void reset();

    void setHackCnt(ENUM_HACKTYPE type1, unsigned int value1,
                    ENUM_HACKTYPE type2, unsigned int value2,
                    unsigned int limitCnt, unsigned int ruleIdx,
                    unsigned int arg7);                        // ORIG W 0x084eb854
    void setTotalTrade(unsigned int type, unsigned long long total);  // ORIG W 0x084eb9d4
    stHacktypeExceptionInfo_t GetExceptionCnt(int key);        // ORIG T 0x080f972a

    void setIPCounterData(unsigned int a, unsigned int b, char* ip,
                          unsigned int cnt);                   // ORIG T 0x080f9218
    int _findIPCount(unsigned int a, unsigned int b, char const* ip);  // ORIG T 0x080f9322
    void WriteIPCounterLog(CHackAnalyzer* analyzer, int a, int b, int c,
                           int d, int e, char* ip);            // ORIG T 0x080f967a
    void ResetIpCounterData();                                 // ORIG W 0x084eba00

    void setFullIPCounterData(unsigned int a, unsigned int b, char* ip,
                              unsigned int cnt);               // ORIG T 0x080f946a
    int _findFullIPCount(unsigned int a, unsigned int b, char const* ip);  // ORIG T 0x080f9574
    void WriteFullIPCounterLog(CHackAnalyzer* analyzer, int a, int b, int c,
                               int d, int e, char* ip);        // ORIG T 0x080f96d2
    void ResetFullIpCounterData();                             // ORIG W 0x084eba16

    bool checkTotalTradeRuleOver(unsigned int tradeType,
                                 unsigned long long total) const;  // ORIG W 0x08531fd2

    // ORIG W 0x08695360：遍历 m_hackTypeMap 中 type 区间，任一规则条目的
    // 累计黑客计数超过阈值时经 _checkApplyFlag 决定处置（返回 true 表示命中）。
    bool checkMaxHackCntOver(ENUM_HACKTYPE type, unsigned int param,
                             CHackAnalyzer* analyzer, HackTypeInfo_t& outInfo,
                             std::vector<stLookOuts>& lookOutsA,
                             std::vector<tyIPCounters>& ipCounters,
                             std::vector<stLookOuts>& lookOutsB, char* ip);

    // ORIG W 0x0869550c：按规则 m_value3 类型分派——1/5 直接拷贝出参；
    // 2/10/3 查观察名单/计数器名单命中即拒绝；4/9 与 b/c 走 IP 计数日志；
    // 全部通过则拷贝出参并返回 true。
    bool _checkApplyFlag(const HackTypeInfo_t& info, CHackAnalyzer* analyzer,
                         HackTypeInfo_t& outInfo,
                         std::vector<stLookOuts>& lookOutsA,
                         std::vector<tyIPCounters>& ipCounters,
                         std::vector<stLookOuts>& lookOutsB, char* ip);

private:
    // ORIG 布局（ctor 0x82a5b1a / set* 实证偏移）总 sizeof 0x70。
    // 宿主 GCC 4.4.7 std::_Rb_tree 为 24B，ORIG（旧 libstdc++）为 28B；
    // m_stlGap* 为两者实现尺寸差的忠实还原（非语义字段）。
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t> m_hackTypeMap;     // +0x00
    unsigned int m_totalTradeType;                                  // +0x18【推断命名】
    unsigned long long m_totalTradeValue;                           // +0x1c【推断命名】
    std::multimap<unsigned short, HackIPCounter_t> m_ipCounterMap;        // +0x24
    char m_stlGap1[4];                                              // 0x3c..0x40 版本间隙
    std::multimap<unsigned short, HackFullIPCounter_t> m_fullIPCounterMap;// +0x40
    std::map<int, stHacktypeExceptionInfo_t> m_exceptionMap;        // +0x58
};

#ifdef W6_HP_STATIC_ASSERT_SIZEOF
W6_HP_STATIC_ASSERT_SIZEOF(CAutoPunishRuleHackTypeMgr, 0x70);
#endif

}  // namespace WongWork

#endif  // DNF_GAME_W6_HACKPUNISHDEPS_H_
