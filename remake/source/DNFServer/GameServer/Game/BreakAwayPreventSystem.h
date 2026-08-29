// ============================================================================
// df_game_r 还原 —— break_away_prevent::CBreakAwayPreventSystem 权威声明点
//
// 唯一类定义处：GlobalData.cpp（ctor/dtor/loadDbData 实现）、
// BreakAwayPreventImpl.cpp（getDiffConnectDay/calBreakAwayUserPoint/
// updateUvList/selectReward/startBreakAwayPreventSystem）、
// W6_CeraShopDeps.cpp（updateUsedCera）统一 include 本头。
//
// 对象布局（ORIG ctor 0x0831c3e0 / loadDbData 0x0831c49e / selectReward
// 0x0831c872 汇编逐槽核对）：
//   +0x000 m_base[0x2e4]  SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER 基类镜像区
//   +0x2e4 std::map<STRewardDataKey, STRewardDataList>   （sizeof(map)=0x18）
//   +0x2fc m_keys[5]      STRewardDataKey[5] = 15 字节紧排
//   +0x30b m_tail[0x1f]   UV 连接名单串（'0'/'1' 码字），总 sizeof=0x32a
// ============================================================================

#ifndef DNF_GAME_BREAKAWAYPREVENTSYSTEM_H_
#define DNF_GAME_BREAKAWAYPREVENTSYSTEM_H_

#include <list>
#include <map>
#include <cstring>

class SIG_LOGIN_DATA;
class SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER;
class CUser;

// ORIG 中 STRewardDataKey/STRewardData/STRewardDataList 均为全局类
// （nm 无 ns 前缀，如 _ZN16STRewardDataListC1Ev），仅 CBreakAwayPreventSystem
// 位于 break_away_prevent 命名空间 —— 三个 struct 放全局、类留 ns。
struct STRewardDataKey
{
    char m_data[3];
    STRewardDataKey()
    {
        m_data[0] = 0;
        m_data[1] = 0;
        m_data[2] = 0;
    }
    bool operator<(const STRewardDataKey& other) const
    {
        if (m_data[0] < other.m_data[0])
            return true;
        if (m_data[0] != other.m_data[0])
            return false;
        if (m_data[1] < other.m_data[1])
            return true;
        if (m_data[1] != other.m_data[1])
            return false;
        return m_data[2] < other.m_data[2];
    }
};

// 奖励条目（loadDbData stride 0x18；selectReward 直接消费五字段）。
// ORIG C1/C2 W 0x0831d3c8：5×movl 0（+0..+0x10 逐字段清零）。
struct STRewardData
{
    STRewardData()
    {
        m_field0 = 0;
        m_field4 = 0;
        m_field8 = 0;
        m_fieldc = 0;
        m_field10 = 0;
    }

    int m_field0;                         // +0x00 点数下界
    int m_field4;                         // +0x04 点数上界
    int m_field8;                         // +0x08 → STBreakAwayRewards.m_rewardItemId
    int m_fieldc;                         // +0x0c → STBreakAwayRewards.m_rewardAItemCnt
    int m_field10;                        // +0x10 → STBreakAwayRewards.m_luckPoint
};

struct STRewardDataList
{
    // ORIG 均弱符号（W），头内联：
    //   C1     0x0831d412 (0x42)：list C1 + list clear + 异常清理(list D1+unwind)
    //   C1ERKS 0x0831d7a0 (0x1a)：仅 list C1ERKS
    //   D1/D2  0x0831d55c (0x13)：仅 list D1
    STRewardDataList()
    {
        m_list.clear();
    }

    STRewardDataList(const STRewardDataList& other) : m_list(other.m_list)
    {
    }

    ~STRewardDataList()
    {
    }

    std::list<STRewardData> m_list;
};

namespace break_away_prevent
{

class CBreakAwayPreventSystem
{
public:
    CBreakAwayPreventSystem();                                    // T 0x0831c3e0
    ~CBreakAwayPreventSystem();                                   // T 0x0831c486

    // T 0x0831c49e：拷贝基类区（0x2e4）后按条目填充 m_rewardMap 与 m_keys[]。
    void loadDbData(SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data);
    // T 0x0831ce5e：转发 DB_UpdateBreakAwayAccruedCera::makeRequest。
    void updateUsedCera(unsigned int accId, unsigned int cera);
    void GM_UpdateUvList(unsigned int accId, unsigned int day);   // ORIG T 0x0831ca5a（oldGmRoutine SetBreak）
    // T 0x0831cb56：登录 UV 名单维护；返回值即 ORIG EBX/EAX（bool 0/1，
    // InterDispatcher 原 asm 桥 extern char 同语义）。
    bool updateUvList(SIG_LOGIN_DATA const* login);
    // T 0x0831ce78：断线回归奖励入口（order 1/3 分支与 luck_point 处理）。
    void startBreakAwayPreventSystem(SIG_LOGIN_DATA const* login,
                                     CUser* user);

    unsigned int getDiffConnectDay(unsigned int timeSec);          // T 0x0831c794
    bool calBreakAwayUserPoint();                                  // T 0x0831c810
    bool selectReward(int keyIndex, int point, CUser* user);       // T 0x0831c872
    // T 0x0831d164（报告 break_away_prevent__CBreakAwayPreventSystem/
    // giveBreakAwayDungeonClearReward.md）：order==1→selectReward(1,accCera)
    // 否则 selectReward(3,accCera)；命中则 Inter_LoadEtc::giveBreakAwayReward。
    void giveBreakAwayDungeonClearReward(CUser* user);

private:
    char m_base[0x2e4];
    std::map<STRewardDataKey, STRewardDataList> m_rewardMap;
    STRewardDataKey m_keys[5];
    char m_tail[0x1f];
};

static_assert(sizeof(CBreakAwayPreventSystem) == 0x32c,
              "CBreakAwayPreventSystem ABI drift");

} // namespace break_away_prevent

#endif // DNF_GAME_BREAKAWAYPREVENTSYSTEM_H_
