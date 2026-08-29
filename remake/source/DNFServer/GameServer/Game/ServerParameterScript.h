#ifndef GAME_SERVERPARAMETERSCRIPT_H_
#define GAME_SERVERPARAMETERSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本，尺寸 0x49f4）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）逐成员推导；
// getter 对照 docs/class_func_reports/ServerParameterScript.md 与 ORIG 反汇编。
// CDataManager.h 中的简化声明由本头替换。
// ============================================================================

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cstring>
#include "CFatigueBatteryHandle.h"  // stFatigueBatteryConstantTable_t（+0x5c0 成员）
#include "ItemFarmingData.h"       // ItemFarmingData（m_farming4c0 元素；ORIG W C1 0x08a8cdb0）

// ---- 子结构（map/vector 模板名与 ORIG mangled 名一致，布局从简） ----
struct stServerIndex
{
    stServerIndex()              // ORIG C1 W 0x08a8cc0a：两 int 清零
    {
        m_field0 = 0;
        m_field4 = 0;
    }
    ~stServerIndex() {}          // ORIG D1 W 0x0836f29c：空体

    int m_field0;   // +0x00
    int m_field4;   // +0x04
    bool operator<(const stServerIndex& o) const
    {
        // ORIG 0x08a8cbce：先比 +0x04（m_field4），相等再比 +0x00（m_field0）。
        return m_field4 < o.m_field4 ||
               (m_field4 == o.m_field4 && m_field0 < o.m_field0);
    }
};

struct stLuckPoint_t
{
    ~stLuckPoint_t() {}   // ORIG D1 W 0x08a922a6：空体（用户析构使符号对齐）
    int m_field0;   // +0x00
    int m_field1;   // +0x04
};

// ---- stLuckPointDeduction_t（ORIG C1 W 0x08a8ce26：仅 vector ctor；
//      C1ERKS W 0x08a8ee74 / D1 W 0x083dcc1a / op= W 0x08a8ef22，全 W → 头内联）----
struct stLuckPointDeduction_t
{
    stLuckPointDeduction_t() : m_levels() {}
    stLuckPointDeduction_t(const stLuckPointDeduction_t& o)
        : m_field0(o.m_field0), m_levels(o.m_levels) {}
    ~stLuckPointDeduction_t() {}
    stLuckPointDeduction_t& operator=(const stLuckPointDeduction_t& o)
    {
        m_field0 = o.m_field0;
        m_levels = o.m_levels;
        return *this;
    }

    int m_field0;               // +0x00（ORIG ix 后读 (%eax)）
    std::vector<int> m_levels;  // +0x04（getLuckPointDeductionRate 0x08a8c4fe 对
                                //   元素 +0x04 调 vector<int>::ix，实证存在）
};   // sizeof 0x10

struct stAssaultChaosPointWeight
{
    stAssaultChaosPointWeight()   // ORIG C1 W 0x08a8cd28：m_min/m_max 清零、w0/w1=1.0f
    {
        m_min = 0;
        m_max = 0;
        m_w0 = 1.0f;
        m_w1 = 1.0f;
    }
    ~stAssaultChaosPointWeight() {}   // ORIG D1 W 0x08a936f4：空体
    int m_min;      // +0x00
    int m_max;      // +0x04
    float m_w0;     // +0x08
    float m_w1;     // +0x0c
};

struct stAssaultLevelGhostPanalty
{
    stAssaultLevelGhostPanalty()   // ORIG C1 W 0x08a8cd56：四 int 清零
    {
        m_min = 0;
        m_max = 0;
        m_t0 = 0;
        m_t1 = 0;
    }
    ~stAssaultLevelGhostPanalty() {}   // ORIG D1 W 0x08a938fa：空体
    int m_min;      // +0x00
    int m_max;      // +0x04
    unsigned int m_t0;  // +0x08
    unsigned int m_t1;  // +0x0c
};

struct stAssaultChaosPointGhostPanalty
{
    stAssaultChaosPointGhostPanalty()   // ORIG C1 W 0x08a8cd82：min/max 清零、r0/r1=1.0f
    {
        m_min = 0;
        m_max = 0;
        m_r0 = 1.0f;
        m_r1 = 1.0f;
    }
    ~stAssaultChaosPointGhostPanalty() {}   // ORIG D1 W 0x08a93b00：空体
    int m_min;      // +0x00
    int m_max;      // +0x04
    float m_r0;     // +0x08
    float m_r1;     // +0x0c
};

struct stUnusableEquipmentInfo
{
    ~stUnusableEquipmentInfo() {}   // ORIG D1 W 0x08a928da：空体
    int m_field0;   // +0x00（装备类型枚举 2..11，import 0x8a8a28c 实证）
    int m_field4;   // +0x04（ScanInt 首值）
    int m_field8;   // +0x08（ScanInt 次值）
};

// ---- stLightServerNotice_t（ORIG C1 W 0x08a8ce10：仅构造 +0x08 std::string；
//      C1ERKS W 0x08a8e764 / D1 W 0x083dcbe4 / op= W 0x08a8e81e，全 W → 头内联）----
// TimerLightServerNotice/RegistNextTimer.md（读 +0 hour、+4 min）与
// dispatch_sig.md（读 +8 string）。
struct stLightServerNotice_t
{
    stLightServerNotice_t() : m_message() {}
    stLightServerNotice_t(const stLightServerNotice_t& o)
        : m_hour(o.m_hour), m_min(o.m_min), m_message(o.m_message) {}
    ~stLightServerNotice_t() {}
    stLightServerNotice_t& operator=(const stLightServerNotice_t& o)
    {
        m_hour = o.m_hour;
        m_min = o.m_min;
        m_message = o.m_message;
        return *this;
    }

    int m_hour;              // +0x00
    int m_min;               // +0x04
    std::string m_message;   // +0x08
};                           // 0x10

struct stBurningFatigueParam_t
{
    stBurningFatigueParam_t()   // ORIG C1 W 0x08a8cc64：逐字段清零，float +0x10=1.0f，
    {                           // 尾部 memset(+0x18, 0, 4)。
        m_level = 0;
        m_bonusExpRate = 0;
        m_refillFatigue = 0;
        m_field0c = 0;
        m_field10 = 1.0f;
        m_field14 = 0;
        memset(m_field18, 0, 4);
    }
    ~stBurningFatigueParam_t() {}   // ORIG D1 W 0x08a91bb0：空体。

    unsigned short m_level;       // +0x00（getBonusExpRate ORIG 0x810a0ec 比较）
    int m_bonusExpRate;           // +0x04
    unsigned short m_refillFatigue;  // +0x08（getRefillFatigue ORIG 0x810a166 返回）
    int m_field0c;                // +0x0c（importServerParameterScript ScanInt）
    float m_field10;              // +0x10（import ScanFloat；C1 默认 1.0f）
    int m_field14;                // +0x14（import ScanInt）
    char m_field18[4];            // +0x18（import 4×ScanInt；C1 memset 4 字节）
};

struct ConditionEnterRevenge
{
    int m_field0;          // +0x00（复仇条件下限）
    int m_field4;          // +0x04（复仇条件上限）
    unsigned char m_field8; // +0x08（复仇副本计数）
};

struct stFatigueEventParam_t
{
    stFatigueEventParam_t();  // ORIG 0x08a8ccc4 W
    ~stFatigueEventParam_t() {}  // ORIG 0x08a91dfc W（D1/D2 空体，须保留用户析构）
    unsigned short m_field0;   // +0x00（importServerParameterScript 0x8a883a3 mov %ax 实证）
    int m_field4;              // +0x04（0x8a883cc mov %eax 实证）
    unsigned char m_field8;    // +0x08（C1 memset(this+8,0,4)；0x8a883f8 mov %al 实证）
    unsigned char m_field9;    // +0x09
    unsigned char m_fielda;    // +0x0a
    unsigned char m_fieldb;    // +0x0b
};
static_assert(sizeof(stFatigueEventParam_t) == 0xc,
              "stFatigueEventParam_t ABI drift (ORIG size 0xc)");

struct stUpgradeItemTicketParam_t
{
    stUpgradeItemTicketParam_t()   // ORIG C1 W 0x08a8cc22：清零（ORIG 逐字节 movb×4）
    {
        m_field0 = 0;
    }
    int m_field0;   // +0x00
};

// 拍卖限购参数：ORIG 12 字节 {m_itemId(+0,-1 通配); m_startSec(+4); m_endSec(+8)}。
// 依据 Dispatcher_AuctionRegistItem::dispatch_sig ORIG 0x082147c5/0x082147d8：
//   - 0x0821478b cmp $0xffffffff → m_itemId==-1 通配
//   - 0x0821479d 元素 +0 与请求 itemIdx 匹配
//   - 0x082147c5 mov 0x4(%eax) → m_startSec（< curSec）
//   - 0x082147d8 mov 0x8(%eax) → m_endSec（> curSec）
// 循环逐元素写包 put_int(+0)/put_int(+4)/put_int(+8)，步长 0x0c。
struct stAuctionLimitItemParam_t
{
    stAuctionLimitItemParam_t()   // ORIG C1 W 0x08a8cc42：三 int 清零
        : m_itemId(0), m_startSec(0), m_endSec(0)
    {
    }
    ~stAuctionLimitItemParam_t() {}   // ORIG D1 W 0x08a934ee：空体
    int m_itemId;    // +0x00（-1 通配）
    int m_startSec;  // +0x04
    int m_endSec;    // +0x08
};

static_assert(sizeof(stAuctionLimitItemParam_t) == 0xc,
              "stAuctionLimitItemParam_t ABI drift (ORIG size 0xc)");
static_assert(offsetof(stAuctionLimitItemParam_t, m_itemId) == 0x00,
              "stAuctionLimitItemParam_t m_itemId offset drift");
static_assert(offsetof(stAuctionLimitItemParam_t, m_startSec) == 0x04,
              "stAuctionLimitItemParam_t m_startSec offset drift");
static_assert(offsetof(stAuctionLimitItemParam_t, m_endSec) == 0x08,
              "stAuctionLimitItemParam_t m_endSec offset drift");

// ORIG 布局（C1 0x0836f2a2：清零两个 int + vector<int> 默认构造；D1 0x0836fa54）：
//   { int +0x00; int +0x04; vector<int> +0x08 }，sizeof 0x14。
// ServerParameterScript 析构 0x0837bf57 `add $0x454` 实证其为 +0x454 成员对象。
struct stEventIdolBringUpParam_t
{
    stEventIdolBringUpParam_t() : m_field0(0), m_field4(0), m_vec8() {}
    int m_field0;             // +0x00
    int m_field4;             // +0x04
    std::vector<int> m_vec8;  // +0x08
};

static_assert(sizeof(stEventIdolBringUpParam_t) == 0x14,
              "stEventIdolBringUpParam_t ABI drift (ORIG size 0x14)");

// ORIG C1 W 0x0836fb6c / D1 W 0x0836fbd8：{vector<pair<int,int>> +0x00;
// vector<pair<int,int>> +0x0c; vector<pair<int,int>> +0x18}，sizeof 0x24。
// C1 反汇编：依序构造 +0x00/+0x0c/+0x18 三 vector 并带 unwind 清理路径
// （与编译器隐式生成同形，故不写用户成员函数）。
struct stBloodDungeonRewardInfo
{
    std::vector<std::pair<int, int> > m_vec0;    // +0x00
    std::vector<std::pair<int, int> > m_vecc;    // +0x0c
    std::vector<std::pair<int, int> > m_vec18;   // +0x18
};

static_assert(sizeof(stBloodDungeonRewardInfo) == 0x24,
              "stBloodDungeonRewardInfo ABI drift (ORIG size 0x24)");

// ORIG C1 0x0836fa92：{vector<int> +0x00; vector<pair<int,int>> +0x0c;
// vector<pair<int,int>> +0x18}，sizeof 0x24（+0x770..+0x794 占位实证）。
// 成员名与 CSharedServerMessageManager.cpp 本地定义保持一致以避免 ODR 分歧。
struct stSharedServerMessageInfo
{
    std::vector<int> m_vecIndex;                     // +0x00
    std::vector<std::pair<int, int> > m_vecMapped;   // +0x0c
    std::vector<std::pair<int, int> > m_vecShareKind;  // +0x18
};

static_assert(sizeof(stSharedServerMessageInfo) == 0x24,
              "stSharedServerMessageInfo ABI drift (ORIG size 0x24)");

// ORIG C1 0x0836f374：清零 +0x00(word) 与 +0x04(dword)；大小 0xc 由
// ServerParameterScript 内 +0x4b4..+0x4c0 占位实证。
struct stWeekendBouns
{
    stWeekendBouns();  // ORIG 0x0836f374 W
    unsigned short m_bonusFatiguePercent;   // +0x00（importServerParameterScript.md
                                            //   0x8a8b90f "[weekend bonus fatigue]"
                                            //   mov %dx,0x4b4(%eax) 实证，word）
    unsigned short m_pad2;                  // +0x02
    int m_bonusExpPercent;                  // +0x04（importServerParameterScript.md
                                            //   0x8a8b95b "[weekend bonus exp]"
                                            //   mov %eax,0x4b8(%edx) 实证）
    int m_field8;                           // +0x08（ORIG 未初始化，仅占位）
};

static_assert(sizeof(stWeekendBouns) == 0xc,
              "stWeekendBouns ABI drift (ORIG size 0xc)");

// ORIG C1 0x0836f348：清零 4 个 int，sizeof 0x10。
struct stLevelCond4PremiumGoldCard
{
    stLevelCond4PremiumGoldCard();  // ORIG 0x0836f348 W
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
    int m_fieldc;   // +0x0c
};

static_assert(sizeof(stLevelCond4PremiumGoldCard) == 0x10,
              "stLevelCond4PremiumGoldCard ABI drift (ORIG size 0x10)");

// ORIG C1/C2 0x08a8ccfc：清零 4 个 int（+0x00..+0x0f），sizeof 0x10。
// 字段名【推断】自用法：Timer_DungeonInoutOpenTime::RegistNextTimer 0x8639b20 /
// checkDungeonInoutTime 0x8639a12 / dispatch_sig 0x8639e0a 按
// startHour<=curHour<=endHour && startMin<=curMin<=endMin 判定副本进出开放窗口。
struct STDungeonInoutTime
{
    STDungeonInoutTime()   // ORIG C1 W 0x08a8ccfc：四 int 清零
        : m_startHour(0), m_endHour(0), m_startMin(0), m_endMin(0)
    {
    }
    int m_startHour;   // +0x00 开放起始时
    int m_endHour;     // +0x04 开放结束时
    int m_startMin;    // +0x08 开放起始分
    int m_endMin;      // +0x0c 开放结束分
};

static_assert(sizeof(STDungeonInoutTime) == 0x10,
              "STDungeonInoutTime ABI drift (ORIG size 0x10)");

// ORIG C1 0x89154ee：vector<uchar>@+0x04 构造 + 3 字节清零（sizeof 0x10）。
// importDimensionActivationData [section]（0x89160f0 +0x9b8..+0xa85）：
//   f0=ScanInt(&ok) 首字节、f1/f2=ScanInt((bool*)0)、m_items push_back uchar×cnt。
struct EventRewardSection
{
    EventRewardSection() : m_items() { m_field0 = 0; m_field1 = 0; m_field2 = 0; }
    unsigned char m_field0;                  // +0x00
    unsigned char m_field1;                  // +0x01
    unsigned char m_field2;                  // +0x02
    std::vector<unsigned char> m_items;      // +0x04
};

// ORIG C1 0x891548a / clear 0x89154bc：int@0/int@4 清零、short@8 清零、
// vector<int>@+0xc 构造/clear（sizeof 0x18）。
// importDimensionActivationData [package selection]（0x89160f0 +0xbe4..+0xdd9）：
//   f0=首 ScanInt、f4=第三 ScanInt、s08(short)=第二 ScanInt，
//   m_growTypes=getGrowTypeIndex(tokenizeStr(str,",")) 逐项 push_back。
struct EventRewardItemInfo
{
    EventRewardItemInfo() : m_growTypes() { m_field0 = 0; m_field4 = 0; m_short8 = 0; }
    void clear()
    {
        m_field0 = 0; m_field4 = 0; m_short8 = 0;
        m_growTypes.clear();
    }
    int m_field0;                // +0x00
    int m_field4;                // +0x04
    short m_short8;              // +0x08
    std::vector<int> m_growTypes;  // +0x0c
};

#ifndef GAME_SITEMGENREF_DEFINED
#define GAME_SITEMGENREF_DEFINED
struct sItemGenRef
{
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldc;    // +0x0c
    int m_dropRate;  // +0x10 掉落率倍率（float 位模式，SetItemDropRate 按比例缩放）
};
#endif

enum eRankBonusIndex
{
    eRankBonusIndex_0 = 0
};

enum ENUM_UNUSABLE_EQUIPMENT_TYPE
{
    ENUM_UNUSABLE_EQUIPMENT_TYPE_0 = 0
};

#ifndef DNF_ENUM_EQUIPMENTTYPE_DEFINED
#define DNF_ENUM_EQUIPMENTTYPE_DEFINED
enum ENUM_EQUIPMENTTYPE
{
    ENUM_EQUIPMENTTYPE_0 = 0
};
#endif

class ServerParameterScript
{
public:
    ServerParameterScript();
    ~ServerParameterScript();
    void clear();

    // ---- 策划参数访问器（ORIG 反汇编逐条对照） ----
    float GetExpBonusWeight(eRankBonusIndex idx) const;
    bool CheckCollectDungeonStatisticServer(int serverGroup, int channelNo);
    bool CheckLightServer(int serverIdx);
    std::vector<std::pair<int, int> >* GetEscaladeTutorialReward(
        unsigned int rewardIdx);
    bool checkAssaultServer(int serverGroup, int channelNo);
    unsigned int getAssaultGhostTime(int level, int chaosPoint);
    float getAssaultPriceChaospointWeight(int chaosPoint);
    int getLuckPoint(int idx, int& out);
    int getLuckPointDeductionRate(int level, int grade);  // ORIG T 0x08a8c4fe
    bool LoadScript(const char* path);

    // ---- 简单 getter（ORIG weak inline，本 TU 具名实现） ----
    int GetLottoCost() { return m_7b8; }
    int GetJackpot() { return m_7bc; }
    int GetLottePoint(int idx);
    int GetPriceAverage(int idx);
    int getAssaultPrice(int idx);
    float getChaosPointWeight(int idx);
    int GetAdmissionIncrease(int idx);
    float GetPvPPenaltyRevision(int grade);
    int getRecoverStaminaPrice(int idx, int price);
    int GetPvPChannelGradePenalty(int grade) const;  // ORIG W 0x08693994（_ZNK）
    int GetDailyMatchCount() { return m_f8; }
    bool CheckUnusableEquipmentInfo(ENUM_UNUSABLE_EQUIPMENT_TYPE type, int idx);  // ORIG W 0x08568af6
    bool CheckUnusableItem(ENUM_EQUIPMENTTYPE equipType, unsigned long itemIdx) const;  // ORIG W 0x08568bbe
    std::vector<std::pair<int, int> >& GetPvPLiveEventReward() { return m_veca0; }
    int GetPvPLiveEventPlayCount() { return m_ac; }   // ORIG W 0x08269508（_ZN 无 K，非 const）
    int get_pvp_item_give(int idx);
    int GetPaneltyDupMessage() { return m_4964; }
    int GetCleanChattingCount(int level);
    int GetMaxCleanChattingCount() { return m_4960; }
    int GetTraningFirstGive(int a, int b, int c,
                            std::vector<std::pair<int, int> >& out);
    int GetTraningSealGive(int idx) const;   // ORIG W 0x084a60e2（_ZNK）
    bool CheckTraningFirstGive() const;      // ORIG W 0x083703e2（_ZNK）
    const char* getAssaultUserRunawayNotice();  // ORIG W 0x082f03f8（返回 m_str594.c_str()）
    bool isDungeonOpen(int dungeonIdx);
    void setDungeonOpen(int dungeonIdx, bool open);

    struct PvPPlayerKey
    {
        PvPPlayerKey() : m_field0(0), m_field1(0), m_field2(0) {}
        PvPPlayerKey(unsigned char a, unsigned char b, unsigned char c)
            : m_field0(a), m_field1(b), m_field2(c)
        {
        }
        bool operator<(const PvPPlayerKey& o) const
        {
            return m_field0 < o.m_field0 ||
                   (m_field0 == o.m_field0 &&
                    (m_field1 < o.m_field1 ||
                     (m_field1 == o.m_field1 && m_field2 < o.m_field2)));
        }
        unsigned char m_field0;   // +0x00
        unsigned char m_field1;   // +0x01
        unsigned char m_field2;   // +0x02
    };

    // ORIG C1 0x0836fc5a：+0x8 起循环构造 10×stLevelCond4PremiumGoldCard（步长 0x10，
    // while(esi=9..-1) setne 形态）；+0x00/+0x04 与 +0xA8..+0xBF 均不初始化。
    // 尺寸由 SPS C1 0x836ff0c/0x836ff19 实证：+0x4894 构造、+0x4954 下一成员 → 0xC0。
    struct stPremiumGoldCardEvent
    {
        stPremiumGoldCardEvent();  // ORIG C1/C2 0x0836fc5a
        int m_field0;                                  // +0x00（ctor 不初始化）
        int m_field4;                                  // +0x04（ctor 不初始化）
        stLevelCond4PremiumGoldCard m_conds[10];       // +0x08..+0xa7
        char m_padA8[0x18];                            // +0xa8..+0xbf（ctor 不初始化）
    };

    // ---- 成员布局（0x49f4 字节） ----
    std::map<stServerIndex, bool> m_map0;                       // +0x00（ORIG C1 0x836fcb2）
    std::vector<std::pair<int, int> > m_vec18;                  // +0x18（ORIG C1 0x81349d6）
    std::map<int, bool> m_map24;                                // +0x24
    std::vector<stLightServerNotice_t> m_light3c;               // +0x3c
    std::vector<stLuckPoint_t> m_luck48;                        // +0x48
    std::vector<stLuckPointDeduction_t> m_luckDed54;            // +0x54
    std::vector<stLuckPointDeduction_t> m_luckDed60;            // +0x60
    std::vector<int> m_vec6c;                                   // +0x6c
    int m_78;                                                   // +0x78
    int m_7c;                                                   // +0x7c
    int m_80;                                                   // +0x80
    int m_84;                                                   // +0x84
    std::map<int, std::vector<std::pair<int, int> > > m_map88;  // +0x88
    std::vector<std::pair<int, int> > m_veca0;                  // +0xa0
    int m_ac;                                                   // +0xac
    std::map<int, int> m_mapb0;                                 // +0xb0
    std::map<int, STDungeonInoutTime> m_dungeonInoutc8;         // +0xc8
    std::vector<int> m_dungeonOpenE0;                           // +0xe0
    std::vector<stUnusableEquipmentInfo> m_unusableEC;          // +0xec
    int m_f8;                                                   // +0xf8
    std::set<unsigned long> m_unusableItemFC;                   // +0xfc
    std::map<ENUM_EQUIPMENTTYPE, std::set<unsigned long> > m_map114;  // +0x114
    // LinkClosureD 拆分（§8.3，sizeof 不变；ORIG hasPreventPrefixCharacName
    // 0x0836360c 实证：count@+0x12c，条目 @+0x130 起步长 0x1e，串区含 NUL）。
    int m_preventPrefixCount;                    // +0x12c
    char m_preventPrefixEntries[0x224];          // +0x130（0x1e 步长条目数据区）
    // ---- 版本字节（CDataManager::init ORIG 0x08358xxx 实证：读 4 字节计算
    //      antibot 版本并写回 +0x35c）----
    unsigned char m_versionBytes[4];                            // +0x354（CDataManager::init 逐字节读 4 字节；clear 清零）
    unsigned int m_358;                                         // +0x358（ORIG clear 写 0；与版本字节为两个独立槽位）
    unsigned int m_antibotVersion;                              // +0x35c（init 写入 b0*1e7+b1*1e5+(b2-hi*100)*100+hi*1e4+b3，hi=(b2*0x29)>>12；
                                                                //   若声明 [4]+uint 相邻，antibot 会落 +0x358 使其后全表 -4 漂移）
    // ---- 浮点池（+0x360..+0x400，ORIG clear 0x0836f38c 全部初始化 1.0f；
    //      importServerParameterScript 各节以 (基址*4+disp) 写入，节字符串见注释）----
    float m_expRate360[2];          // +0x360（clear 单独写两个 1.0f；import 0x8e2855d 节）
    float m_expGroup368[4];         // +0x368（import 0x8e2858c 节；clear 循环 lea 0xd8 disp 8）
    float m_expGroup378[4];         // +0x378（0x8e285b0；lea 0xdc）
    float m_expGroup388[4];         // +0x388（0x8e285e4；lea 0xe0）
    // CParty::getMonsterTotalExp(0x085a23dc) 经验表：
    //   *(sp + 0x398 + monsterIdx*4)（即 &m_monsterExpTable398[monsterIdx]）。
    // import 0x8e2860c 节写 5 个 float；ORIG clear 仅复位前 4 个（保留该行为）。
    float m_monsterExpTable398[5];  // +0x398
    // ServerParameterScript::GetExpBonusWeight(0x08a8c28c)：
    //   *(this + (idx+0xe8)*4 + 0xc) = +0x3ac + idx*4，idx∈[0,4]。
    float m_expBonusWeight3ac[5];   // +0x3ac
    std::vector<sItemGenRef> m_genRefs0;                        // +0x3c0
    float m_floatPool3cc[5];        // +0x3cc（import 0x8e2866c 节 lea 0xf0 disp 0xc ×5）
    float m_floatPool3e0[4];        // +0x3e0（import 0x8e28690 节 lea 0xf8 ×4）
    // getMonsterTotalExp flag 费率表：*(sp + 0x3f0 + flag*4)（flag 有符号 ≤3，
    // flag<0 时 ORIG 同样按该式向前越界读取）；import 0x8e286b4 节 ×4。
    float m_flagExpRate3f0[4];      // +0x3f0
    std::vector<sItemGenRef> m_genRefs1;                        // +0x400
    float m_floatPool40c[5];        // +0x40c（import 0x8e28708 节 lea 0x100 disp 0xc ×5）
    float m_floatPool420[4];        // +0x420（import 0x8e28730 节 lea 0x108 ×4）
    int m_430;                      // +0x430（import 0x8e28755 节 ScanInt）
    int m_ints434[4];               // +0x434（import 0x8e28770 节 lea 0x10c disp 4 ×4）
    int m_ints444[4];               // +0x444（import 0x8e28798 节 lea 0x110 disp 4 ×4）
    stEventIdolBringUpParam_t m_eventIdolBringUp454;            // +0x454（dtor 0x837bf57 实证）
    std::map<unsigned int, stUpgradeItemTicketParam_t> m_ticket468;  // +0x468
    std::vector<stAuctionLimitItemParam_t> m_auction480;        // +0x480
    std::vector<stBurningFatigueParam_t> m_burning48c;          // +0x48c
    std::vector<ConditionEnterRevenge> m_revenge498;            // +0x498
    int m_field4a4;                                             // +0x4a4（村庄袭击事件物品 idx；ORIG ctor/dtor 0x498→0x4a8 空隙）
    std::vector<stFatigueEventParam_t> m_fatigue4a8;            // +0x4a8
    stWeekendBouns m_weekendBouns4b4;                           // +0x4b4（ORIG C1 0x836fe24 实证）
    std::map<int, ItemFarmingData> m_farming4c0;                // +0x4c0
    std::vector<std::pair<int, float> > m_vec4d8;               // +0x4d8
    std::vector<stAssaultChaosPointWeight> m_chaosWeight4e4;    // +0x4e4
    std::vector<stAssaultLevelGhostPanalty> m_ghostLevel4f0;    // +0x4f0
    std::vector<stAssaultChaosPointGhostPanalty> m_ghostChaos4fc;  // +0x4fc
    int m_508[20];                                              // +0x508
    int m_558;                                                  // +0x558（-1）
    char m_55c[0x1a];                                           // +0x55c
    int m_578;                                                  // +0x578
    std::vector<int> m_vec57c;                                  // +0x57c
    std::vector<int> m_vec588;                                  // +0x588
    std::string m_str594;                                       // +0x594
    int m_598;                                                  // +0x598
    int m_59c;                                                  // +0x59c
    int m_5a0;                                                  // +0x5a0
    std::vector<float> m_float5a4;                              // +0x5a4
    char m_pad5b0[0x10];                                        // +0x5b0
    stFatigueBatteryConstantTable_t m_fatigueTable5c0;          // +0x5c0（ORIG C1 0x836fec4；
    //   与 CFatigueBatteryHandle 注释 (mgr+0x68)+0x5c0 同一起点互证），sizeof 0x1ac
    int m_76c;                                                  // +0x76c
    stSharedServerMessageInfo m_shared770;                     // +0x770（ORIG C1 0x836fa92；0x770..0x794 占位实证）
    stBloodDungeonRewardInfo m_bloodReward794;                  // +0x794（ORIG C1 0x836fee4 整体构造
    int m_7b8;                                                  // +0x7b8
    int m_7bc;                                                  // +0x7bc
    // 纹章合成表（ORIG ServerParameterScript::clear 0x836f7c0 写入成本=-1）：
    //   +0x7c0..+0x868 = 7 个纹章合成条目，每条 0x18 = { int cost; int result[5]; }
    //   (ORIG Dispatcher_CompoundEmblem::dispatch_sig 0x8220bb2 实证：
    //    成本读 *(this+0x828+grade*0x18)；结果读 *(this+0x82c+(grade*6+jewelType)*4)
    //    —— 即同一数组 cost@+0、result[j]@+0x04+j*4)
    //   注意：+0x7c0 区域旧误标为 stAssaultLevelGhostPanalty m_ghost7c0[7]（步长 0x10
    //   与 ORIG 0x18 不一致），且 getAssaultGhostTime(0x8a8c804) 实际读 m_ghostLevel4f0
    //   (+0x4f0 vector) 而非此数组，故删除该误标。
    struct EmblemCompoundEntry
    {
        int m_cost;             // +0x00（==-1 → throw ENUM_ERROR 7）
        int m_result[5];        // +0x04..+0x14（result[grade*6+jewelType] 索引）
    };
    EmblemCompoundEntry m_emblemCompound[7];                    // +0x7c0
    int m_arr868[32][32];                                       // +0x868
    int m_arr1868[32][32];                                      // +0x1868
    char m_pad2868[0x2000];                                     // +0x2868
    std::map<int, unsigned int> m_map4868;                      // +0x4868
    std::vector<std::pair<int, int> > m_vec4880;                // +0x4880
    int m_488c;                                                 // +0x488c（ORIG clear 写 0）
    int m_4890;                                                 // +0x4890（ORIG clear 写 0）
    stPremiumGoldCardEvent m_premiumGoldCard4894;               // +0x4894（ORIG C1 0x836ff14 实证；尺寸 0xC0 至 +0x4954）
    std::vector<std::pair<int, int> > m_vec4954;                // +0x4954
    int m_4960;                                                 // +0x4960
    int m_4964;                                                 // +0x4964
    int m_4968;                                                 // +0x4968
    int m_496c;                                                 // +0x496c
    // 训练印章/公平PvP配置块（ORIG fair_pvp__CFairMatch/GiveTrainingSeal 0x85652c8
    // 经 CDataManager+0x68 嵌入读 dm+0x49d8..0x49e0 = 本结构 +0x4970..+0x497a）：
    //   [0]/[1] 1人队 胜/负 场次数；[2]/[3] 1人队 奖励胜/负；
    //   [4]/[5] 3人队 胜/负；[6]/[7] 3人队 奖励胜/负。
    unsigned char m_schoolMatchSeal[8];                         // +0x4970（dm+0x49d8）
    unsigned short m_schoolMatchDailyPlayLimit;                 // +0x4978（dm+0x49e0，每日场次上限）
    std::map<int, int> m_map497c;                               // +0x497c
    std::map<int, int> m_map4994;                               // +0x4994
    std::map<PvPPlayerKey, std::vector<std::pair<int, int> > > m_pvp49ac;  // +0x49ac
    std::map<int, int> m_map49c4;                               // +0x49c4
    std::vector<int> m_vec49dc;                                 // +0x49dc
    int m_49e8;                                                 // +0x49e8
    int m_49ec;                                                 // +0x49ec
    int m_49f0;                                                 // +0x49f0（末成员；
    //   ORIG clear 最大写此处，类尺寸 0x49f4）
};

// ---- 布局断言（ORIG 证据：clear 0x0836f38c / dtor 0x0837bb42 /
//      SetItemDropRate CDM+0x428/+0x468 两 vector<sItemGenRef>）----
// ORIG 尺寸证据：CDM C1/init/dtor 以 add $0x4a5c 定位下一成员 m_inGameAd
// （SPS 起点 +0x68）→ sizeof 0x49f4；clear 0x0836f38c 最大写 +0x49f0。
static_assert(sizeof(ServerParameterScript) == 0x49f4,
              "ServerParameterScript layout drift (ORIG size 0x49f4)");
static_assert(offsetof(ServerParameterScript, m_monsterExpTable398) == 0x398,
              "ServerParameterScript exp table offset drift");
static_assert(offsetof(ServerParameterScript, m_expBonusWeight3ac) == 0x3ac,
              "ServerParameterScript exp bonus weight offset drift");
static_assert(offsetof(ServerParameterScript, m_genRefs0) == 0x3c0,
              "ServerParameterScript genRefs0 offset drift");
static_assert(offsetof(ServerParameterScript, m_flagExpRate3f0) == 0x3f0,
              "ServerParameterScript flag exp rate table offset drift");
static_assert(offsetof(ServerParameterScript, m_genRefs1) == 0x400,
              "ServerParameterScript genRefs1 offset drift");
static_assert(offsetof(ServerParameterScript, m_eventIdolBringUp454) == 0x454,
              "ServerParameterScript eventIdol member offset drift");
static_assert(offsetof(ServerParameterScript, m_ticket468) == 0x468,
              "ServerParameterScript ticket offset drift");
static_assert(offsetof(ServerParameterScript, m_preventPrefixCount) == 0x12c,
              "ServerParameterScript prevent prefix count offset drift");
static_assert(offsetof(ServerParameterScript, m_preventPrefixEntries) == 0x130,
              "ServerParameterScript prevent prefix entries offset drift");
static_assert(offsetof(ServerParameterScript, m_weekendBouns4b4) == 0x4b4,
              "ServerParameterScript weekend bouns offset drift");
static_assert(offsetof(ServerParameterScript, m_fatigueTable5c0) == 0x5c0,
              "ServerParameterScript fatigue battery table offset drift");
static_assert(offsetof(ServerParameterScript, m_shared770) == 0x770,
              "ServerParameterScript shared server message offset drift");
static_assert(offsetof(ServerParameterScript, m_premiumGoldCard4894) == 0x4894,
              "ServerParameterScript premium gold card offset drift");

bool importServerParameterScript(ServerParameterScript* script,
                                 const char* path);

#endif  // GAME_SERVERPARAMETERSCRIPT_H_
