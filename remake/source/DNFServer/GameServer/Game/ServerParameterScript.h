#ifndef GAME_SERVERPARAMETERSCRIPT_H_
#define GAME_SERVERPARAMETERSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本，尺寸 0x49f4）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）逐成员推导；
// getter 对照 docs/class_func_reports/ServerParameterScript.md 与 ORIG 反汇编。
// CDataManager.h 中的简化声明由本头替换。
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// ---- 子结构（map/vector 模板名与 ORIG mangled 名一致，布局从简） ----
struct stServerIndex
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    bool operator<(const stServerIndex& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_field4 < o.m_field4);
    }
};

struct stLuckPoint_t
{
    int m_field0;   // +0x00
};

struct stLuckPointDeduction_t
{
    int m_field0;   // +0x00
};

struct stAssaultChaosPointWeight
{
    int m_min;      // +0x00
    int m_max;      // +0x04
    float m_w0;     // +0x08
    float m_w1;     // +0x0c
};

struct stAssaultLevelGhostPanalty
{
    int m_min;      // +0x00
    int m_max;      // +0x04
    unsigned int m_t0;  // +0x08
    unsigned int m_t1;  // +0x0c
};

struct stAssaultChaosPointGhostPanalty
{
    int m_min;      // +0x00
    int m_max;      // +0x04
    float m_r0;     // +0x08
    float m_r1;     // +0x0c
};

struct stUnusableEquipmentInfo
{
    int m_field0;   // +0x00
};

struct stLightServerNotice_t
{
    int m_field0;   // +0x00
};

struct stBurningFatigueParam_t
{
    int m_field0;   // +0x00
};

struct ConditionEnterRevenge
{
    int m_field0;   // +0x00
};

struct stFatigueEventParam_t
{
    int m_field0;   // +0x00
};

struct ItemFarmingData
{
    int m_field0;   // +0x00
};

struct stUpgradeItemTicketParam_t
{
    int m_field0;   // +0x00
};

struct stAuctionLimitItemParam_t
{
    int m_field0;   // +0x00
};

struct stEventIdolBringUpParam_t
{
    int m_field0;   // +0x00
};

struct stBloodDungeonRewardInfo
{
    int m_field0;   // +0x00
};

struct stSharedServerMessageInfo
{
    int m_field0;   // +0x00
};

struct STDungeonInoutTime
{
    int m_field0;   // +0x00
};

struct EventRewardSection
{
    int m_field0;   // +0x00
};

struct EventRewardItemInfo
{
    int m_field0;   // +0x00
};

#ifndef GAME_CDATAMANAGER_H_
struct sItemGenRef
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
    int m_fieldc;   // +0x0c
    int m_field10;  // +0x10
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

#ifndef GAME_STEQUIPMENTSCRIPT_H_
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
    int getLuckPointDeductionRate(int idx, int& out);
    bool LoadScript(const char* path);

    // ---- 简单 getter（ORIG weak inline，本 TU 具名实现） ----
    int GetLottoCost() { return m_7b8; }
    int GetJackpot() { return m_7bc; }
    int GetLottePoint(int idx);
    int GetPriceAverage(int idx);
    int getAssaultPrice(int idx);
    float getChaosPointWeight(int idx);
    int GetAdmissionIncrease(int idx);
    int GetPvPPenaltyRevision(int grade);
    int getRecoverStaminaPrice(int idx, int price);
    int GetPvPChannelGradePenalty(int grade);
    int GetDailyMatchCount() { return m_f8; }
    bool CheckUnusableEquipmentInfo(int type, int idx);
    bool CheckUnusableItem(int equipType, unsigned long itemIdx) const;
    int GetPvPLiveEventReward() { return m_ac; }
    int GetPvPLiveEventPlayCount() { return m_map0.empty() ? 0 : 1; }
    int get_pvp_item_give(int idx);
    int GetPaneltyDupMessage() { return m_4964; }
    int GetCleanChattingCount(int level);
    int GetMaxCleanChattingCount() { return m_4960; }
    int GetTraningFirstGive(int a, int b, int c,
                            std::vector<std::pair<int, int> >& out);
    int GetTraningSealGive(int idx);
    bool CheckTraningFirstGive();
    bool isDungeonOpen(int dungeonIdx);
    void setDungeonOpen(int dungeonIdx, bool open);
    int getAssaultGhostTime2(int a, int b) { return 0; }

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

    struct stPremiumGoldCardEvent
    {
        stPremiumGoldCardEvent() : m_field0(0), m_field1(0) {}
        int m_field0;   // +0x00
        int m_field1;   // +0x04
    };

    // ---- 成员布局（0x49f4 字节） ----
    std::map<stServerIndex, bool> m_map0;                       // +0x00
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > > m_map0c;  // +0x0c
    std::vector<std::pair<int, int> > m_vec18;                  // +0x18
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
    char m_pad12c[0x228];                                       // +0x12c
    char m_354[4];                                              // +0x354
    int m_358;                                                  // +0x358
    float m_expBonus360[4];                                     // +0x360
    float m_370[4];                                             // +0x370
    float m_380[4];                                             // +0x380
    float m_390[4];                                             // +0x390
    float m_expBonusWeight3a0[5];                               // +0x3a0
    char m_pad3b4[0xc];                                         // +0x3b4
    std::vector<sItemGenRef> m_genRefs0;                        // +0x3c0
    std::vector<sItemGenRef> m_genRefs1;                        // +0x400
    std::map<unsigned int, stUpgradeItemTicketParam_t> m_ticket468;  // +0x468
    std::vector<stAuctionLimitItemParam_t> m_auction480;        // +0x480
    std::vector<stBurningFatigueParam_t> m_burning48c;          // +0x48c
    std::vector<ConditionEnterRevenge> m_revenge498;            // +0x498
    std::vector<stFatigueEventParam_t> m_fatigue4a8;            // +0x4a8
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
    char m_pad5b0[0x1bc];                                       // +0x5b0
    int m_76c;                                                  // +0x76c
    stSharedServerMessageInfo m_shared770;                      // +0x770
    stBloodDungeonRewardInfo m_blood794;                        // +0x794
    std::vector<std::pair<int, int> > m_vec7a0;                 // +0x7a0
    std::vector<std::pair<int, int> > m_vec7ac;                 // +0x7ac
    int m_7b8;                                                  // +0x7b8
    int m_7bc;                                                  // +0x7bc
    stAssaultLevelGhostPanalty m_ghost7c0[7];                   // +0x7c0
    int m_arr868[32][32];                                       // +0x868
    int m_arr1868[32][32];                                      // +0x1868
    char m_pad2868[0x2000];                                     // +0x2868
    std::map<int, unsigned int> m_map4868;                      // +0x4868
    std::vector<std::pair<int, int> > m_vec4880;                // +0x4880
    int m_488c;                                                 // +0x488c
    int m_4890;                                                 // +0x4890
    char m_pad4894[0xc0];                                       // +0x4894
    std::vector<std::pair<int, int> > m_vec4954;                // +0x4954
    int m_4960;                                                 // +0x4960
    int m_4964;                                                 // +0x4964
    int m_4968;                                                 // +0x4968
    int m_496c;                                                 // +0x496c
    char m_4970[8];                                             // +0x4970
    unsigned short m_4978;                                      // +0x4978
    std::map<int, int> m_map497c;                               // +0x497c
    std::map<int, int> m_map4994;                               // +0x4994
    std::map<PvPPlayerKey, std::vector<std::pair<int, int> > > m_pvp49ac;  // +0x49ac
    std::map<int, int> m_map49c4;                               // +0x49c4
    std::vector<int> m_vec49dc;                                 // +0x49dc
    int m_49e8;                                                 // +0x49e8
    int m_49ec;                                                 // +0x49ec
    int m_49f0;                                                 // +0x49f0
};

bool importServerParameterScript(ServerParameterScript* script,
                                 const char* path);

#endif  // GAME_SERVERPARAMETERSCRIPT_H_
