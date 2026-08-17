#ifndef DNF_GAME_GAMEWORLD_H_
#define DNF_GAME_GAMEWORLD_H_

// ============================================================================
// df_game_r 还原 —— GameWorld（G2-4，世界/频道/在线用户聚合根，GlobalInstance
// 单例）。
// 布局依据 ORIG 构造 086c3d4a / 析构 086c3f2c / create()（new 0x1b0）：
//   +0x000 Mutex（0x18）                   +0x018 m_iVillCount
//   +0x01c Village* m_pVill                +0x020 m_channelType
//   +0x024 unsigned short m_uniqueId       +0x028 StatisticsToObserveCrash(0x64)
//   +0x08c map<string,SimpleInfo> m_idMap  +0x0a4 StatisticsShusiaService(0x48)
//   +0x0ec StatisticsPvP(0x30)             +0x11c StatisticsNatType(0x18)
//   +0x134 map<ushort,CUser*> m_UserInWorld +0x14c map<uint,ushort> m_sessionList
//   +0x164 int m_channelUserCount[11]      +0x190 m_userCount
//   +0x194 m_hangameUserCount              +0x198 map<uint,CUser*> m_loginUserMap
//   sizeof = 0x1b0
// ============================================================================

#include <map>
#include <string>
#include <utility>

#include "GameTypes.h"
#include "GameWorld_deps.h"
// ENUM_SERVER_GROUP（CEnvironment.h）与 ENUM_NOTIPACKET（CUser.h）的权威声明
#include "CUser.h"

class CUser;
class CParty;
class PacketGuard;
class CBelong;
class CUserModifier;
class BuddyList;

class TownScript;
class CSyncScript;

// ---- Statistics 族（尚无权威头；尺寸/符号取自 ORIG 构造链）----
class StatisticsToObserveCrash
{
public:
    StatisticsToObserveCrash();
    ~StatisticsToObserveCrash();
    void reset();
    char m_pad[0x64];
};

class StatisticsShusiaService
{
public:
    StatisticsShusiaService();
    ~StatisticsShusiaService();
    char m_pad[0x48];
};

class StatisticsPvP
{
public:
    StatisticsPvP();
    ~StatisticsPvP();
    char m_pad[0x30];
};

class StatisticsNatType
{
public:
    StatisticsNatType();
    ~StatisticsNatType();
    char m_pad[0x18];
};

class GameWorld
{
public:
    GameWorld();
    ~GameWorld();

    bool init();
    void destroy();

    // ---- 频道 ----
    void InsertChannel(int gcNo, ENUM_SERVER_GROUP group);
    void DeleteChannel(int gcNo, ENUM_SERVER_GROUP group);
    void UpdateChannelUser();
    void updateDetailChannelServiceInfo();
    void UpdateStatistic4ChannelUserLev();
    int GetChannelUserCount(ENUM_SERVER_GROUP group, stOccStatisticByAge& age);
    int _GetChannelType();
    bool InitChannelType();
    int GetChannelType() const;
    bool IsSameTeamChannelJoin();
    bool IsIntegratedPvPBaseChannel() const;
    bool IsPVPChannel() const;
    bool IsFreePvPChannel() const;
    bool IsSchoolPvPChannel() const;
    bool IsWinPointPvPChannel() const;
    bool IsPvPSkilTreeChannel() const;
    bool IsEquipSlotSwitchChannel() const;
    bool IsEnchantRevisionChannel();
    bool IsCheckUnusableEquipmentChannel();
    bool IsPvPVillageMapChannel() const;
    bool is_dungeon_tag_matching_channel(char* channel);

    // ---- 在线用户 ----
    void InsertLoginUser(CUser* user);
    void EraseLoginUser(CUser* user);
    bool InsertUserToIDMap(const char* name, unsigned int id);
    bool DeleteUserInIDMap(const char* name);
    unsigned int GetIDUserInIDMap(const char* name);
    bool reach_game_world(CUser* user);
    bool leave_game_world(CUser* user);
    unsigned short make_unique_id();
    unsigned short find_session(unsigned int accId);
    CUser* find_from_world(unsigned short uniqueId);
    CUser* find_user_from_world_byaccid(unsigned int accId);
    CUser* find_user_from_world_byaccid_loop(unsigned int accId);
    CUser* find_user_by_charac_name(const char* name);
    void arrange_users();
    int CheckUserCount(int add);
    int GetUserCount(int vill, int area) const;
    int get_UserCount_InWorld();
    void UpdateNatTypeUser();

    // ---- 广播 ----
    void send_all(PacketGuard& packet);
    void send_all(PacketGuard& packet, CUser* user);
    void send_all_excepted_user(PacketGuard& packet, CUser* user);
    void send_all_with_state(PacketGuard& packet, int state);
    void send_all_dungeon_inout_message(int dungeonIdx);
    void send_all_dungeon_inout_message(int dungeonIdx, bool open);
    void send_user_dungeon_inout_message(CUser* user, int dungeonIdx, bool open);
    void send_to_area(int vill, int area, PacketGuard& packet);
    void send_to_area(int vill, int area, PacketGuard& packet, CBelong* belong);
    void send_to_area(int vill, int area, PacketGuard& packet,
                      std::vector<CUser*>& users);
    void send_to_user(int uniqueId, PacketGuard& packet);
    void send_to_vill(int vill, PacketGuard& packet);
    void send_group(PacketGuard& packet, CBelong* belong);
    void send_server_group(PacketGuard& packet, char group);
    void send_party_info_to_all(CParty* party, int flag);
    void send_AllBasicInfo(CUser* user);
    void send_buddy_conn_list(unsigned int accId, BuddyList* list, int count);
    void send_power_war_hp_of_vill(int vill);

    // ---- 村庄 / 区域 ----
    Village* GetVillage(int vill);
    Village* getUserVillage(CUser* user);
    Village* getUserVillage(int vill);
    int get_gate_area(int vill) const;
    bool get_area_center_point(int vill, int area, int& x, int& y) const;
    bool is_available_point(int vill, int area, int x, int y) const;
    bool check_valid_area(int vill, int area);
    bool isDungeonEntranceArea(CUser* user);
    int getDungeonMinimumRequiredLevel(int dungeonIdx);
    int GetWorldMapIndex(CUser* user);
    void get_user_id_list(int vill, int area, std::vector<unsigned short>& out);
    void get_user_info(CUser* user, int targetId, int type);
    bool get_disposition(int vill, int area, pvp_assault::CDisposition* disposition);
    void modify_user(CUserModifier* modifier);
    int check_restrictive_commercial_transaction_zone(int vill, int area, int x, int y);

    // ---- 移动 ----
    void move_position(CUser* user, int x, int y, int z, unsigned short dir);
    int check_move_area(CUser* user, int vill, int area, int x, int y, int z,
                        bool flag, int param);
    bool move_area(CUser* user, int vill, int area, int x, int y, int z,
                   bool flag, int param, int prevVill, int prevArea);
    void goto_dungeon(CUser* user);
    void out_from_dungeon(CUser* user);
    void goto_pvp(CUser* user);
    void out_from_pvp(CUser* user);
    void goto_warroom(CUser* user);
    void out_from_warroom(CUser* user);
    void gotoBlueMarble(CUser* user);
    void outFromBlueMarble(CUser* user);
    void fishing(CUser* user);
    bool IsPowerWarSafeZone(int vill, int area, int side);
    bool IsPowerWarSafeZone(int vill, int area);

    // ---- 聊天 ----
    void make_packet_chat_msg(PacketGuard& packet, CUser* user, char* msg, int len,
                              char type, ENUM_NOTIPACKET notify);
    void make_packet_chat_msg_hyper_link(PacketGuard& packet, CUser* user,
                                         void* linkMsg, ENUM_NOTIPACKET notify);
    void send_chat_msg(CUser* from, char type, unsigned short target,
                       unsigned int characNo, char* msg, int len, char* nick,
                       int nickLen, ENUM_NOTIPACKET notify);
    void send_chat_msg_hyper_link(CUser* from, void* linkMsg, ENUM_NOTIPACKET notify);
    void check_peerTypemsg_sameWorldUser(CUser* from, CUser* to, char type,
                                         char* msg, int len);
    void send_peerTypemsg_sameWorldUser(CUser* from, CUser* to, char type,
                                        int a, int b, const char* msg, int len);
    void WriteLogChatMsg(CUser* user, char type, char* msg);

    // ---- 禁言 / 禁移动 ----
    bool EnableChat(const char* name);
    bool EnableMove(const char* name);
    bool DisableChat(const char* name, int secs);
    bool DisableMove(const char* name, int secs);
    bool IsForbiddenChat(const char* name);
    bool IsForbiddenMove(const char* name);
    unsigned int WhenIsTheEndOfForbiddenChat(const char* name);
    unsigned int WhenIsTheEndOfForbiddenMove(const char* name);
    unsigned int GetLeftTimeOfForbiddenChat(const char* name);
    unsigned int GetLeftTimeOfForbiddenMove(const char* name);

    // ---- 每日重置 ----
    void resetDailyData();
    void ResetMidnight();
    void ResetAllFatigue();
    void ResetAllCoins();
    void ResetAllCleadpadPoint();
    void ResetAllPassPadFailCount();
    void ResetAllEventData();
    void ResetCacheCharactorMemory();
    bool ResetCacheCharactorMemory(unsigned int accId);
    void ResetEntireReliablePerson();
    void ResetOneADayItemShop();
    void testResetOneADayItemShop();
    void SetWeekendBounsEvent();
    void DeleteDailyItem();
    void AddDailyItem();
    bool ReselectDailyQuest();
    void DailyEventModify();
    void ResetCraneItemNeedMaterial(int param);
    void deleteSpecificItems(const std::vector<std::pair<int, int> >* list);
    void UpdateServerSnapShot();
    void UpdateMiniCraneSeed();
    StatisticsPvP* getStatisticsPvP();
    StatisticsNatType* GetStatisticsNatType();
    StatisticsShusiaService* getStatisticsShusiaService();
    StatisticsToObserveCrash* getStatisticsToObserveCrash();

    // 对世界内所有在线用户执行 functor（ORIG 0863d3bc/0816270c 模板实例；
    // 定义与显式实例化在 GameWorld.cpp）
    template <typename T>
    void ProcessAllUsers(T functor);

    // ---- 力战 ----
    void CleanupPowerWarVillToStart();
    void OnEndPowerWar(PacketGuard& packet);
    void OnBanAbuserFromPowerWar();
    void OnRefreshPowerWarProcessInfo();
    void GetPowerUpDomain(std::vector<CUser*>& listA, std::vector<CUser*>& listB);
    void SendPowerUpList(int activateType, std::vector<CUser*>& list);
    void CancelPowerUp();

    // ---- 成员（布局见文件头注释）----
    Mutex m_mutex;                     // +0x00
    int m_iVillCount;                  // +0x18
    Village* m_pVill;                  // +0x1c
    int m_channelType;                 // +0x20
    unsigned short m_uniqueId;         // +0x24
    char m_pad26[2];                   // +0x26
    StatisticsToObserveCrash m_obsCrash;   // +0x28
    std::map<std::string, SimpleInfo> m_idMap;   // +0x8c
    StatisticsShusiaService m_shusia;  // +0xa4
    StatisticsPvP m_pvp;               // +0xec
    StatisticsNatType m_natType;       // +0x11c
    std::map<unsigned short, CUser*> m_UserInWorld;   // +0x134
    std::map<unsigned int, unsigned short> m_sessionList;  // +0x14c
    int m_channelUserCount[11];        // +0x164
    int m_userCount;                   // +0x190
    int m_hangameUserCount;            // +0x194
    std::map<unsigned int, CUser*> m_loginUserMap;  // +0x198
};

GameWorld* G_GameWorld();
void CREATE_GameWorld();
void DESTROY_GameWorld();

#endif  // DNF_GAME_GAMEWORLD_H_
