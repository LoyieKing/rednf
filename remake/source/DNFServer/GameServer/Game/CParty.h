#ifndef GAME_CPARTY_H_
#define GAME_CPARTY_H_

// df_game_r CParty（G2 队伍聚合根）还原（2026-08-17）。
// 布局依据 ORIG 构造（0x082a5516）/析构（0x082a57fe）逐子对象推导：
//   +0x000 CItemRoutingData(0x4c)  +0x04c Mutex(0x18)
//   +0x078 cMember[4](0x18 each)   +0x0d8 cElection<int,4,4>(0x40)
//   +0x118 结果接收标志/标题区      +0x148 int[37]（快速队伍状态数组）
//   +0x1dc CPartyResultRecvFlag(0x34) +0x210 CTraceMobDieHack(0x94)
//   +0x2a4 secretshop::SECRET_SHOP_DATA(0x88) +0x32c BattleData(0x7f8)
//   +0xb24 CBattle_Field(0xcd0)    +0x17f4 GameResultSet(0x50)
//   +0x1844 std::map<int,int>(0x18)+0x185c 命中/硬币统计区
//   +0x1abc Secu_HackLogCheckByParty(0x14) +0x1ad0 CPartyTelePort(0x24)
//   +0x1af4 std::vector<MapInfo>(0xc)（总尺寸 0x1b00）
// 依赖子对象（CBattle_Field/BattleData/... 后续批次）以 char 数组占位，
// CParty.cpp 内以 asm-label extern 调用访问；主 agent 集成时统一替换。

#include <list>
#include <map>
#include <cstring>
#include <utility>
#include <vector>

#include "GameTypes.h"
#include "Inven_Item.h"
#include "CUser.h"

// ---- 本批次临时类型（主 agent 集成后迁移到权威头） ----
enum ENUM_PARTY_INFO_TYPE
{
    ENUM_PARTY_INFO_TYPE_0 = 0
};

enum ENUM_WALKOUT_TYPE
{
    ENUM_WALKOUT_TYPE_0 = 0
};

#ifndef DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
enum ENUM_DUNGEON_TYPE
{
    ENUM_DUNGEON_TYPE_0 = 0
};
#endif

enum ENUM_USER_DUNGEON_STATE
{
    ENUM_USER_DUNGEON_STATE_0 = 0
};

#ifndef DNF_ENUM_ENUM_DUNGEON_MODE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_MODE_DEFINED
enum ENUM_DUNGEON_MODE
{
    ENUM_DUNGEON_MODE_0 = 0
};
#endif

#ifndef DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
#define DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
enum ENUM_MAZE_QUEST_TYPE
{
    ENUM_MAZE_QUEST_TYPE_0 = 0
};
#endif

#ifndef DNF_ENUM_TIMER_MESSAGE_DEFINED
#define DNF_ENUM_TIMER_MESSAGE_DEFINED
enum TIMER_MESSAGE
{
    TIMER_MESSAGE_0 = 0
};
#endif

enum eClearRewardCardType_t
{
    eClearRewardCardType_t_0 = 0
};

#ifndef DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
#define DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
enum eRidableObjectState
{
    eRidableObjectState_0 = 0
};
#endif

namespace RetryFailReason
{
enum T
{
    T_0 = 0
};
}

namespace ManageBonusType
{
enum T
{
    T_0 = 0
};
}

namespace QuickParty
{
enum BasicRewardType
{
    BasicRewardType_0 = 0
};
enum RandomBuffType
{
    RandomBuffType_0 = 0
};
#ifndef DNF_ENUM_QUICKPARTY_RANDOMBUFFDUNGEON_DEFINED
#define DNF_ENUM_QUICKPARTY_RANDOMBUFFDUNGEON_DEFINED
enum RandomBuffDungeon
{
    RandomBuffDungeon_0 = 0
};
#endif
struct RewardData_Param;
struct RewardData_Result;
}

// ---- 前向声明（权威定义属后续批次 / 已有头） ----
class CDungeon;
class CGameManager;
class Quest;
class CLuckPoint;
class CTournamentDungeonReward;
class GameResultSet;
struct MSG_MONSTER_DIE;
struct MSG_MOVE_MAP;
struct MSG_PARTY_SET_INFO;
struct STGainedEachExpData;
struct stClearRewardExpParameter_t;
struct stClearRewardBonusExp_t;
struct RequiredItem;
struct map_monster;

namespace WongWork
{
struct stGenerateResult_t;
}

class CParty
{
public:
    // ---- 嵌套：物品路由数据（0x4c） ----
    class CItemRoutingData
    {
    public:
        CItemRoutingData();
        ~CItemRoutingData();

        int GetRoutingState();
        void SetRoutingState(char state);
        Inven_Item* GetRoutingInvenItem();
        unsigned int GetRoutingItemIndex();
        void SetRoutingItemIndex(Inven_Item& item, unsigned int idx);
        unsigned int GetRoutingItemUniqueID();
        char GetMemberRoutingState(int idx);
        void SetMemberRoutingState(int idx, char state);
        void Reset();

        int m_routingState;            // +0x00
        Inven_Item m_item;             // +0x04
        unsigned int m_itemUniqueID;   // +0x44
        char m_memberRoutingState[4];  // +0x48
    };

    // ---- 嵌套：队伍成员槽（0x18） ----
    class cMember
    {
    public:
        cMember();
        void Init();

        CUser* m_pUser;   // +0x00
        char m_field4;    // +0x04
        int m_field8;     // +0x08
        char m_fieldc;    // +0x0c
        char m_fieldd;    // +0x0d
        char m_fielde[4]; // +0x0e
        char m_field12;   // +0x12
        int m_field14;    // +0x14
    };

    CParty();
    ~CParty();

    // ---- 基础访问器 ----
    void SetIDX(int idx);
    int GetPartyIndex();
    void set_state(char s);
    char get_state();
    bool IsSinglePlay();
    void SetSinglePlay(bool flag);
    char GetEPLPState();
    void SetEPLPState(char state);
    char GetSelectedEPLPCmd();
    void SetSelectedEPLPCmd(char cmd);
    char GetAssaultState();
    void SetAssaultState(char state);
    cMember* GetMember();
    CUser* get_user(int idx);
    char getTitleIndex();
    void setTitleIndex(unsigned char idx);
    char* getTitle();
    void setTitle(const char* title);
    bool IsAutoCreated();
    void SetAutoCreated(bool flag);
    unsigned char getUserMax();
    void setUserMax(unsigned char max);
    unsigned short getDungIndex() const;
    void setDungIndex(unsigned short idx);
    unsigned char getDungDiffi() const;
    void setDungDiffi(unsigned char diffi);

    // ---- 结果接收标志区（+0x1dc） ----
    char get_dungeon_clear_state();
    void set_dungeon_clear_state(char state);
    void reset_recv_flag();
    void SetRecvResultFlag(bool flag);

    // ---- CTraceMobDieHack 区（+0x210） ----
    unsigned short getStraightVictories();
    void setStraightVictories(unsigned short v);
    void incStraightVictories();
    void resetStraightVictories();
    int getStartGamePartyCount();
    void setStartGamePartyCount(int v);

    // ---- SECRET_SHOP_DATA 区（+0x2a4） ----
    bool IsFirstMapClear();
    void SetFirstMapClear(bool flag);
    bool IsPremiumGoldCardParty();
    void SetPremiumGoldCardParty();
    void ResetPremiumGoldCardParty();
    unsigned int GetPremiumGoldCardDefaultItem();
    void SetPremiumGoldCardDefaultItem(unsigned int v);
    void ResetPremiumGoldCardDefaultItem();
    void* GetSecretShopData();

    // ---- CBattle_Field 转发 ----
    void checkBossRoom();
    void checkStartRoom();
    void DeleteDungeonDropItem(int idx);

    // ---- 传送 / 快速队伍区（+0x1ad0） ----
    int get_quick_party_index();
    void set_quick_party_index(int idx);
    bool is_quick_party();
    void set_quick_party(bool flag);
    void set_gm_random_buff_type(QuickParty::RandomBuffType type);
    int get_random_buff_type();
    bool IsWeekendEvent();
    void SetWeekendEvent(bool flag);
    bool getDungeonMapSaving();
    void setDungeonMapSaving(bool flag);
    void* GetPartyTelePort();
    void* getPassedMapInfoList();

    // ---- 硬币 / 掉落率区 ----
    int GetUsedCoinCount();
    void SetUsedCoinCount(int v);
    void IncreaseUsedCoinCount();
    int Get_party_overlapped_drop_ratio_rare() const;
    int Get_party_overlapped_drop_ratio_unique() const;
    void ClearMapHitCount();

    // ---- 物品路由 ----
    bool IsRoutingState();
    void SetStartRouting();
    void SetEndRouting();
    void SetRoutingItem(Inven_Item& item, unsigned int idx);
    int GetRoutingItemID();
    Inven_Item* GetRoutingInvenItem();
    unsigned int GetRoutingItemUniqueID();
    char GetMemberRoutingState(CUser* user);
    void SetMemberRoutingState(CUser* user, char state);
    void SetRoutingTimerOver();

    // ---- 生命周期 ----
    void init();
    void destroy();
    void init_quick_party_data();
    CGameManager* getManager();

    // ---- 成员管理 ----
    void create_party(CUser* user);
    void join_user(CUser* user);
    void leave_user(CUser* user, ENUM_PARTY_INFO_TYPE type);
    void walkout_member(CUser* user, int param);
    void change_manager(CUser* user, int slot);
    void change_manager();
    int set_host();
    void set_single_play(CUser* user);
    void game_start(CUser* user);
    bool IsExistUser(CUser* user);
    int isMyMember(unsigned int userNo);
    char _checkValidUser(int idx);
    bool checkValidUser(int idx);
    void enumPartyMember(bool (*func)(CUser*));
    int get_member_count();
    int get_live_count();
    int get_live_count_enter_map();
    int get_total_level();
    int get_party_seatno(CUser* user);
    unsigned int GetMemberSlotNo(unsigned int userNo);
    unsigned int GetMemberSlotNo(CUser const* user) const;
    void ClearBadP2PUser();
    void CheckMemberArea();
    int GetUserPosition(CUser* user);
    bool check_allmember_die();
    void check_allmember_loading(bool flag);
    bool IsExistCharacter(int characNo);
    bool IsExistInvisible();
    bool IsThereGhostInParty();
    int FindUserByCharId(unsigned int characNo);
    bool hasPCRoomPlayer();
    bool isSamePCRoomParty();
    bool existSamePCRoomUser(unsigned int pcbangNo);
    bool isGuildParty();
    bool IsEventCharacParty();
    bool IsReturnUserParty();
    void SetPartyInfoUI(MSG_PARTY_SET_INFO const& info);

    // ---- 队伍状态 / 移动 ----
    void changePartyMemberInfo(MSG_MOVE_MAP& info, char flag);
    void changePartyMemberInfoOnMoveMap(MSG_MOVE_MAP& info, bool flag);
    void move_map(MSG_MOVE_MAP& info);
    void switchPostion(char a, char b);
    void useItem(CUser* user, Inven_Item const& item);
    void use_skill(CUser* user, int skillIdx, int param);
    bool isCheckUseSkill(int skillIdx, int param);
    void OnUsedSkill(CUser* user, int skillIdx, int param);
    void die_user(CUser* user);
    void set_charac_live(CUser* user, ENUM_USER_DUNGEON_STATE state);
    void finish_loading(CUser* user);
    void OnStartMapFinishLoading(CUser* user);
    void map_load_forced_finish();
    void send_loadingcheck_msg();
    void resetFinishLoadingVar();
    bool CanDoPartyActionAssalutState();
    void TimeoutCompleteAfterAssault();
    bool check_timer_key(TIMER_MESSAGE msg, int param);
    int gen_timer_key(TIMER_MESSAGE msg);
    bool checkMoveMapHackCnt(MSG_MOVE_MAP& info);
    void setMonsterMoveHack();
    void CheckHackAverageHitCount();

    // ---- 连接管理 ----
    void ConnectP2P(CUser* user, char* ip, int port);
    void _ClearConnectP2PAbsolute(int slot);
    void _ClearConnectP2PRelative(int slot);
    int _IsCannotConnectP2P(int a, int b);
    int _GetConnectP2PPoint(int slot);
    void _GetConnectP2PMaxMinPoint(int& max, int& min);
    void send_party_ipinfo();

    // ---- 发包 ----
    void send_to_party(PacketGuard& packet);
    void send_to_alive_member(PacketGuard& packet);
    void send_to_party_cmd_error(ENUM_CMDPACKET cmd, unsigned char error);
    void send_host_info();
    void send_party_realtime_info();
    void get_party_realtime_info(PacketGuard& packet);
    void send_invite_pvp(int mapIdx);
    void send_invite_warroom(int mapIdx);
    void send_invite_bluemarble(int mapIdx);
    void send_quick_party_matching_result(bool flag, CUser* user);
    void SendWalkout(int param, ENUM_WALKOUT_TYPE type);
    void SendDungeonCharacInfo(CUser* user, bool flag);
    void SendDungeonStatisticParty(bool flag, CUser* user);
    void SendDungeonStatisticPartyJob(bool flag, CUser* user);
    void SendDungeonStatisticPartyCharac(bool flag, CUser* user);
    void SendPartyMemberTagChracInfo();
    void SendSecretShopItemList(CUser* user);
    void sendPremiumGoldCardStatistic(int a, int b, int c);
    void send_hellParty_value();
    void send_dungeon_fail_msg();
    void sendInoutConditionDungeon(RetryFailReason::T reason);
    void makeBestClearTimePacket(PacketGuard& packet, int idx);
    void make_blood_dungeon_info(CUser* user, PacketGuard& packet);
    void make_blood_dungeon_info_blank(PacketGuard& packet);
    void setBestClearTime(int a, int b, int c);

    // ---- 副本流程 ----
    void dungeon_start(int dungeonIdx, char type, ENUM_DUNGEON_TYPE dtype);
    void do_after_dungeon_start(CDungeon const* dungeon, CUser* user);
    bool check_dungeon_start(CDungeon const* dungeon);
    bool check_dungeon_start(CDungeon const* dungeon, int& param);
    bool CheckEnterEventDungeon(CDungeon const* dungeon, unsigned char& flag,
                                Inven_Item* item, int* param);
    bool CheckEnterAncientDungeon(CDungeon const* dungeon, unsigned char& flag,
                                  Inven_Item* item, int* param);
    bool CheckEnterDimensionDungeon(CDungeon const* dungeon);
    bool CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                    char mode);
    bool CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                    bool flag2);
    bool CheckEnterVillageAttackRevenge(CDungeon const* dungeon);
    void CheckHaveHellPartyPassItem(CDungeon const* dungeon, ENUM_DUNGEON_TYPE type);
    void CheckHellDungeonFreepassItemHaveAndDel(CUser* user, bool flag);
    void CheckHellCoinLimint(int param);
    void checkDugeonInoutTime(int param);
    bool checkInoutConditionDungeon(CDungeon const* dungeon, ENUM_DUNGEON_MODE mode,
                                    ENUM_CMDPACKET cmd);
    bool checkInoutConditionDungeon();
    void checkDungeonDifficulty(char difficulty, CDungeon const* dungeon);
    bool checkFreeRevivalCondition(CDungeon const* dungeon);
    bool checkLimitUsedCoinCondition(CDungeon const* dungeon);
    bool checkUnLimitUsedCoinCondition(CDungeon const* dungeon);
    int getStandardDimensionLevel();
    void setStandardDimensionLevel(CDungeon const* dungeon);
    void UseAncientDungeonItems(CDungeon const* dungeon, Inven_Item* item, int* param);
    void UseSealDoorItems(std::vector<int>& itemList);
    bool CheckDestroyConditionSealDoor(CDungeon const* dungeon,
                                       std::vector<int>& list1,
                                       std::vector<int>& list2);
    void CheckClearQuestHellParty(std::vector<int>& list);
    void do_after_dungeon_start_checkndelete_item(RequiredItem const& item,
                                                  CUser* user);

    // ---- 副本结算 ----
    void dungeon_clear();
    void prepare_dungeon_clear();
    void clear_reward(int param);
    void OnDungeonClear(CUser* user, bool success);
    void OnFailedDungeonClear(CUser* user);
    void OnSuccessedDungeonClear(CUser* user);
    void RestartDungeon();
    void ReturnToVillage();
    void ClearDungeon();
    void giveup_game(CUser* user, bool a, bool b, bool c);
    void party_dungeon_fail();
    void set_play_result(CUser* user, GameResultSet& result);
    void forced_set_play_result();
    void SetPlayResult(CUser* user);
    void set_clear_reward_exp_parameter(CUser* user, unsigned int a,
                                       stClearRewardExpParameter_t& param);
    void getClearRewardBonusExp(CUser* user, unsigned int& a,
                                stClearRewardExpParameter_t const& param,
                                stClearRewardBonusExp_t& bonus);
    void set_basic_reward(QuickParty::BasicRewardType type,
                          QuickParty::RewardData_Param param,
                          QuickParty::RewardData_Result& result);
    void set_random_reward(QuickParty::RandomBuffType type,
                           QuickParty::RewardData_Param param,
                           QuickParty::RewardData_Result& result);
    void choose_random_buff(QuickParty::RandomBuffDungeon dungeon);
    void increase_dungeon_coin_count(int param);
    int GetPartyMemberCoinLimit();
    void SetPartyMemberCoinLimit(int limit);
    void resetMemberUseCoin();
    void increase_ancient_dungeon_ticket();
    void increase_dimension_inout_count(int param);
    int get_blood_dungeon_admission_fee(CUser* user);

    // ---- 怪物击杀 / 血之副本 ----
    void CheckMapClear();
    void OnKilledMonster(CUser* user, int monsterIdx, map_monster monster,
                         MSG_MONSTER_DIE const& dieInfo);
    void MemberRegisterKillMonster(map_monster& monster, unsigned char flag);
    void OnFightVillageMonster(CUser* user, int monsterIdx);
    void OnFinishVillageMonsterFighting(CUser* user, bool flag);
    void kill_monster(CUser* user, int monsterIdx, unsigned short mapIdx,
                      MSG_MONSTER_DIE const& dieInfo, int* param);
    void kill_blood_monster(CUser* user, int monsterIdx, unsigned short* param);
    void BloodHandleDieUser(CUser* user);
    void beginBloodClearProcess(bool flag);
    void onBloodClear(bool flag, int param);
    void onBloodRewardTimer(TIMER_MESSAGE msg);
    void finishBloodDungeon();
    void generateBloodRewardExp(int a, int b, unsigned int c, short d);
    void generateBloodRewardItem(int a, int b, unsigned int c, unsigned int d,
                                 bool flag);
    void BloodClearRewardPacket(int a, int b, int c, Inven_Item const* items1,
                                int d, Inven_Item const* items2, int e,
                                Inven_Item const* items3, int f,
                                Inven_Item const* items4);
    void historyBloodRoundClear(int a, unsigned int b, unsigned int c,
                                char const* str);
    void checkBloodRenewMyRecord(int a, int b, int c, int d);

    // ---- 卡片选择 ----
    void SendEachCardInfo();
    void CompleteCardSelect();
    void SendCardSelectComplete();
    bool IsSelectCard(char a, eClearRewardCardType_t type);
    bool HaveAlreadySelectedCard(char a, eClearRewardCardType_t type);
    bool IsCompleteCardSelectOfMembers(eClearRewardCardType_t type);
    bool HasAllMemberSelection();
    void SetCardNumber(CUser* user, char a, eClearRewardCardType_t type);
    bool _checkGoldCardSelectable(CUser* user, eClearRewardCardType_t type,
                                  int param);
    void insert_time_card_select_msg(int param);
    void insert_time_card_select_right_msg(int param);
    void insert_time_clear_dungeon_reward_msg(int param);
    void insert_time_dungeon_clear_msg(int param);
    void insert_time_dungeon_score_scroll_msg(int param);

    // ---- 锦标赛 / TOD ----
    void OnTODRewardTimer(TIMER_MESSAGE msg);
    void OnTimeoutBossDie(int a, unsigned int b);
    void _Send_TOD_APCInfo(int a);
    void _Send_TOD_ClearReward();
    void GetDungeonIndex_isTOD(int& a, int& b);
    void InsertTournamentDungeonTimer(TIMER_MESSAGE msg);
    bool isTournamentVictory();
    bool TournamentDungeonClear(bool flag);
    void MakeTournamentReward(CTournamentDungeonReward& reward);
    char GetTournamentDungeonClearState();
    void SetTournamentDungeonClearState(char state);

    // ---- 技能 / 任务 ----
    void SetProperDungeonEvent(CUser* user, int param);
    void SetProperLevelDungeon(CUser* user);
    void SetRidableObjectState(int state, eRidableObjectState type);
    void DestroyPassiveObject(int param);
    void check_passive_skill();
    void CheckQuestConnection(int questIdx, ENUM_MAZE_QUEST_TYPE type);
    bool HasNoQuestGoWithApcInParty(Quest const* quest);
    bool IsClearedQuest(int questIdx);
    void MakeQuestRewardPacket(PacketGuard* packet, CUser* user);
    void CheckRecvFromAllMember(CUser* user);
    void set_recv_party_packet_flag(CUser* user);
    void recv_party_packet_all();

    // ---- 收益 / 奖励 ----
    void GetUserSecretShop(CUser* user);
    void CalcurateManageBonus(ManageBonusType::T type, CUser* user,
                              unsigned int param);
    float CalcAvatarBonus();
    float CalcCreatureBonus(CUser* user);
    float getAvatarBonus();
    float getCreatureBonus(CUser* user);
    void GetLuckPoint(CLuckPoint* luckPoint, bool flag);
    int GetExpPenaltyPercent(int a, int b) const;
    int MemberPenaltyRate();
    int getMemberMaxLevel();
    int getMemberMinLevel();
    int getMemberMaxLevelBeforeDungeon();
    int getMemberLevelGap();
    int _getMemberLevelGap();
    int get_rate_level_difference();
    void sumGainedEachExp(CUser* user, STGainedEachExpData const& data);
    void getMonsterTotalExp(unsigned int monsterIdx, char flag, int a, float b,
                            float c, bool d);

    // ---- 物品 ----
    void get_item(CUser* user, int itemIdx, bool flag, char c, int& param,
                  unsigned char& flag2, bool& flag3);
    void _onGetItem(CUser* user, unsigned int itemIdx, unsigned int count);
    void drop_item(CUser* user, int itemIdx, int count, char a, int b, int c);
    void drop_item_gm(CUser* user, unsigned int itemIdx);
    bool isMonsterDrop_item(int monsterIdx);
    void _makeRewardItemInfo(CUser const* user, Inven_Item* item,
                             PacketGuard& packet);
    void _putItemIntoUser(CUser* user, Inven_Item* item, PacketGuard& packet);
    void increase_result_card_gold(WongWork::stGenerateResult_t& result);
    void increase_hell_party_ratio(char* str);
    void cal_hellparty_value(int a, int b, int c);
    void sub_hellparty_value();

    // ---- 其它 ----
    void ReviveUser(CUser* user, unsigned short a);
    void VoteLackUser(CUser* user, int param);
    void OnTimerWalkOutLackUser();
    void SaveCoin(eCoinSubReason reason);
    int GetCoinCount(CUser* user);
    void GetMapPlayingTime();
    void getDungeonTypeKind(CDungeon const* dungeon);
    void getDungeonDropTestFlag();
    void getPartyMemberAvgLevel();
    void isPremiumGoldCardReward(CDungeon const* dungeon, unsigned int& param);
    void is_limit_minimum_age(int age, int& param);
    void is_limit_minimum_level(int level, int& param);
    void CheckMemberFatigue();
    void CheckMemberLastTryAssaultTime();
    void GetFirstValidMemberState();
    void GetValidPartyMember(std::vector<int>& list);
    void GetRoutingDiceMemberList(std::vector<std::pair<int, int> >& list,
                                  CItem* item);
    int GetPartyType();
    int GetNoSendSetPlayResultUser(int param);
    void IncreaseSetPlayResultCount(CUser* user);
    int GetSetPlayResultCount();
    int GetPartyMemberForceDropFlag();
    void isDungeonDifficultyInfromNotice(bool flag);
    void getMemberPassParty(CUser* user);
    bool isHelpAbuseParty(CUser* user);
    bool isHelpAbuseParty();

private:
    CItemRoutingData m_routingData;   // +0x000
    Mutex m_mutex;                    // +0x04c

    int m_partyIndex;        // +0x064
    char m_field68;          // +0x068
    bool m_singlePlay;       // +0x069
    char m_state;            // +0x06a
    char m_eplpState;        // +0x06b
    char m_selectedEPLPCmd;  // +0x06c
    char m_assaultState;     // +0x06d
    char m_pad6e[2];         // +0x06e
    char m_pad70[4];         // +0x070
    void* m_field74;         // +0x074

    cMember m_member[4];     // +0x078

    unsigned char m_padElection[0x40];  // +0x0d8 cElection<int,4,4>
    char m_recvResultFlag;              // +0x118
    char m_pad119[2];                   // +0x119
    char m_titleIndex;                  // +0x11b
    char m_title[0x20];                 // +0x11c
    char m_autoCreated;                 // +0x13c
    unsigned char m_userMax;            // +0x13d
    unsigned short m_dungIndex;         // +0x13e
    unsigned char m_dungDiffi;          // +0x140
    char m_pad141[7];                   // +0x141
    int m_field148[37];                 // +0x148

    unsigned char m_padRecvFlag[0x34];  // +0x1dc CPartyResultRecvFlag
    unsigned char m_padTrace[0x94];     // +0x210 CTraceMobDieHack
    unsigned char m_padShop[0x88];      // +0x2a4 secretshop::SECRET_SHOP_DATA
    unsigned char m_padBattleData[0x7f8];  // +0x32c BattleData
    unsigned char m_padBattleField[0xcd0]; // +0xb24 CBattle_Field
    unsigned char m_padResult[0x50];    // +0x17f4 GameResultSet
    unsigned char m_padMap[0x18];       // +0x1844 std::map<int,int>

    int m_field185c;        // +0x185c
    int m_usedCoinCount;    // +0x1860
    int m_field1864;        // +0x1864
    int m_field1868;        // +0x1868
    int m_overlapRare;      // +0x186c
    int m_overlapUnique;    // +0x1870
    int m_mapHitCount;      // +0x1874
    int m_mapHitSum;        // +0x1878
    int m_mapHitList1[0x2d];   // +0x187c（0xb4 字节 = 45 int）
    int m_mapHitSum2;       // +0x1930
    int m_mapHitList2[0x2d];   // +0x1934
    int m_mapHitSum3;       // +0x19e8
    int m_mapHitList3[0x2d];   // +0x19ec
    char m_field1aa0;       // +0x1aa0
    char m_pad1aa1[3];      // +0x1aa1
    unsigned char m_field1aa4[0x10];  // +0x1aa4
    int m_field1ab4;        // +0x1ab4
    int m_field1ab8;        // +0x1ab8

    unsigned char m_padSecu[0x14];     // +0x1abc Secu_HackLogCheckByParty
    unsigned char m_padTelePort[0x24]; // +0x1ad0 CPartyTelePort
    unsigned char m_padPassedMap[0xc]; // +0x1af4 std::vector<MapInfo>
};

static_assert(sizeof(CParty) == 0x1b00, "CParty size");

#endif  // GAME_CPARTY_H_
