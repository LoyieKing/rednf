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

#include "CUser.h"
#include "Village.h"    // Village（GameWorld 成员 m_pVill / GetVillage）
#include "Area.h"       // Area（Village::getArea 返回 / GameWorld.cpp 访问）
#include "CPowerManager.h"  // ENUM_POWER_SIDE_TYPE（IsPowerWarSafeZone 三参版签名，ORIG mangled _ZN9GameWorld18IsPowerWarSafeZoneEii20ENUM_POWER_SIDE_TYPE）
#include "CPowerManager.h"  // ENUM_POWER_SIDE_TYPE（IsPowerWarSafeZone 三参版签名，ORIG mangled _ZN9GameWorld18IsPowerWarSafeZoneEii20ENUM_POWER_SIDE_TYPE）

class CUser;
class CParty;
class PacketGuard;
class CBelong;
class CUserModifier;
class BuddyList;
class MSG_ITEM_HYPER_LINK_MESSAGE;  // 权威定义见 PacketTypes.h（引用参数仅需前向）

class TownScript;
class CSyncScript;

// ---- ENUM_POWER_UP_ACTIVATE_TYPE：SendPowerUpList 激活类型 ----
// ORIG mangled _ZN9GameWorld15SendPowerUpListE27ENUM_POWER_UP_ACTIVATE_TYPERSt6vectorIP5CUserSaIS3_EE。
// 枚举值实证：CancelPowerUp（ORIG 086cf432 传 0）、DisPatcher_DebugCommand_cmds（传 1）。
enum ENUM_POWER_UP_ACTIVATE_TYPE
{
    ENUM_POWER_UP_ACTIVATE_TYPE_CANCEL = 0,   // CancelPowerUp 使用【实证】
    ENUM_POWER_UP_ACTIVATE_TYPE_ACTIVATE = 1, // _debugCommandPowerUpOn 使用【实证】
};

class CUser;
class CParty;
class PacketGuard;
class CBelong;
class CUserModifier;
class BuddyList;

class TownScript;
class CSyncScript;

// ---- Statistics 族（布局证据：docs/class_func_reports/Statistics/*/ 各 ctor/getter/reset 报告）----
class StatisticsToObserveCrash
{
public:
    StatisticsToObserveCrash();
    ~StatisticsToObserveCrash();
    void reset();
    void setAbnormalDownUserCount(int count);   // ORIG T 0x0860bb40
    void incAbnormalDownUserCount();            // ORIG T 0x0860bb52（定义 StatisticsToObserveCrash.cpp）
    int getAbnormalDownUserCount();              // ORIG T 0x0860bb0d（定义 StatisticsToObserveCrash.cpp）
    void incUserLastCrashPacketID(int userIdx);  // ORIG T 0x0860bb88
    void incUserLastCrashDungeonID(int userIdx); // ORIG T 0x0860bd2e
    void incUserLastCrashPlayStatus(int userIdx);// ORIG T 0x0860bed4
    void lock();    // ORIG W 0x0844d4d0（Mutex::lock 转发，定义 W8LT_DBTask.cpp）
    void unlock();  // ORIG W 0x0844d4e6（Mutex::unlock 转发，定义 W8LT_DBTask.cpp）
    // +0x00 int 计数（ctor 置 0 / reset 清 0；getAbnormalDownUserCount 0x0860bb0d 读 (this+0)）
    int m_abnormalDownUserCount;                        // +0x00
    Mutex m_mutex;                                      // +0x04（ctor Mutex::Mutex(this+4)）
    // 三个 map<int,int>，语义由三个 getter 偏移实证：
    //   getUserLastCrashPacketID 0x0860bc9a → this+0x1c
    //   getUserLastCrashDungeonID 0x0860be40 → this+0x34
    //   getUserLastCrashPlayStatus 0x0860bfe6 → this+0x4c
    std::map<int, int> m_userLastCrashPacketId;         // +0x1c
    std::map<int, int> m_userLastCrashDungeonId;        // +0x34
    std::map<int, int> m_userLastCrashPlayStatus;       // +0x4c
};
    static_assert(sizeof(StatisticsToObserveCrash) == 0x64, "StatisticsToObserveCrash size");

class StatisticsShusiaService
{
public:
    StatisticsShusiaService();
    ~StatisticsShusiaService();
    void reset();   // ORIG T 0x0860c154（清空两张统计 map）
    void lock();    // ORIG W 0x0844d4fc（Mutex::lock 转发）
    void unlock();  // ORIG W 0x0844d510（Mutex::unlock 转发）
    // +0x00 Mutex（ctor）；+0x18 / +0x30 两个 map<int,int>（ctor map::map(this+0x18/this+0x30)）：
    //   incDungeonFailure 0x0860c1d7 → this+0x18；getServiceUSage 0x0860c463 → this+0x30
    Mutex m_mutex;                                      // +0x00
    std::map<int, int> m_dungeonFailure;                // +0x18
    std::map<int, int> m_serviceUsage;                  // +0x30
};
    static_assert(sizeof(StatisticsShusiaService) == 0x48, "StatisticsShusiaService size");

class StatisticsPvP
{
public:
    StatisticsPvP();
    ~StatisticsPvP();
    void lock();    // ORIG W 0x0844d524（Mutex::lock 转发）
    void unlock();  // ORIG W 0x0844d538（Mutex::unlock 转发）
    // +0x00 Mutex（ctor）；+0x18 map<PvP_Key,PvP_Room>（incPvPRoomResult 0x0860c603 → this+0x18）。
    struct PvP_Key   // 8 字节（incPvPRoomResult local_40/local_3c 两个 int 组 key）
    {
        int m_a;    // +0x00
        int m_b;    // +0x04
        bool operator<(const PvP_Key& rhs) const
            { return m_a < rhs.m_a || (m_a == rhs.m_a && m_b < rhs.m_b); }
    };
    struct PvP_Room  // 8 字节 value（find 命中时 value+0/+4 各自自增，incPvPRoomResult 0x0860c6b4..0x0860c6ce）
    {
        int m_roomCount;    // +0x00
        int m_resultSum;    // +0x04（+= param_3）
    };
    Mutex m_mutex;                                      // +0x00
    std::map<PvP_Key, PvP_Room> m_roomResults;          // +0x18
};
    static_assert(sizeof(StatisticsPvP) == 0x30, "StatisticsPvP size");

class StatisticsNatType
{
public:
    StatisticsNatType();
    ~StatisticsNatType();

    void Reset();
    void IncreaseNatType(int natA, int natB, unsigned char count);  // ORIG T 0x0860c86c（StatisticsNatType.cpp 定义）
    void UpdateNatType();  // ORIG T 0x0860c950（StatisticsNatType.cpp 定义；TimerNatType::dispatch_sig 调用）
    // +0x00 单个 map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>（ctor map::map(this+0)；
    // IncreaseNatType 0x0860c86c find/insert，value 为 operator new(8) 的 RESULT_COUNT*）。
    struct NAT_TYPE_PEER_TO_PEER   // 8 字节 key（ctor 0x08610262 接收两个 int）
    {
        NAT_TYPE_PEER_TO_PEER(int a, int b);
        int m_a;    // +0x00
        int m_b;    // +0x04
        bool operator<(const NAT_TYPE_PEER_TO_PEER& rhs) const
        {
            // P56 分支岛：ORIG `jge;mov$1;jmp` 独立 return-1 岛，
            // `||` 短路合并成共享 `jl` 岛 (ndiff4)，显式 if 还原。
            if (m_a < rhs.m_a)
                return true;
            if (m_a == rhs.m_a && m_b < rhs.m_b)
                return true;
            return false;
        }
    };
    struct RESULT_COUNT  // 8 字节（ctor 0x08610294；IncreaseNatType 按 +0/+4 自增）
    {
        RESULT_COUNT();
        int m_countA;   // +0x00（param_3!=0 分支自增）
        int m_countB;   // +0x04（param_3==0 分支自增）
    };
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*> m_counts;  // +0x00（gcc4.4 map=0x18，GameWorld 偏移 0x11c→0x134 实证）
};
static_assert(sizeof(StatisticsNatType) == 0x18, "StatisticsNatType size");

// ---- SimpleInfo：GameWorld m_idMap（+0x8c）的 value（0x10 字节） ----
struct SimpleInfo
{
    int m_id;            // +0x00（ID 映射 / 玩家 acc id）
    bool m_chatBan;      // +0x04（禁言标记；ORIG 条件形态 movzbl+test+je ⇒ bool，P73，证据 GameWorld/IsForbiddenChat.md @086cd9f3）
    bool m_moveBan;      // +0x05（禁移动标记；同上 bool 建模）
    char m_pad6[2];      // +0x06
    int m_chatEndTick;   // +0x08（禁言结束毫秒时间）
    int m_moveEndTick;   // +0x0c（禁移动结束毫秒时间）

    SimpleInfo()   // ORIG C1/C2 W 0x086d2108：逐字段清零（movl/movb）
    {
        m_id = 0;
        m_chatBan = 0;
        m_moveBan = 0;
        m_chatEndTick = 0;
        m_moveEndTick = 0;
    }
};

// ---- stOccStatisticByAge：按年龄统计（short[100]）----
struct stOccStatisticByAge
{
    short m_age[100];
};

namespace pvp_assault
{
class CDisposition;   // 力战区域谓词基类；权威定义见 CAssaultMgr.h
}

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
    bool IsCheckUnusableEquipmentChannel();          // ORIG W 0869779e（m_channelType==0xe）
    bool IsCharacterPvPExpRevisionChannel() const;   // ORIG 085dfa76（返回 m_channelType==0xf）
    bool IsCharacterLevelRevisionChannel() const;    // ORIG 084ed128（m_channelType∈[0xe,0x10]）
    bool IsCharacterStatRevisionChannel() const;     // ORIG W 086977da（m_channelType∈[0xe,0x10]）
    bool IsPvPVillageMapChannel() const;
    bool is_dungeon_tag_matching_channel(char* channel);

    // ---- 在线用户 ----
    void InsertLoginUser(CUser* user);
    void EraseLoginUser(CUser* user);
    bool InsertUserToIDMap(const char* name, unsigned int id);
    bool DeleteUserInIDMap(const char* name);
    // b28d-lane11：unsigned int → int（P248 档①，返回类型不入 mangled）。
    // ORIG 0x086cd8d2 `mov (%eax),%ebx` 单步直落（SimpleInfo::m_id int 同型）；
    // unsigned 返回使 4.4 插 `mov (%eax),%eax; mov %eax,%ebx` 两段中转。
    int GetIDUserInIDMap(const char* name);
    bool reach_game_world(CUser* user);
    bool leave_game_world(CUser* user);
    // b28c-lane3：ORIG 0x086c4b8f 段双零扩展（movzwl 0x24(%eax),%eax;
    // movzwl %ax,%eax）⇒ 返回类型 unsigned int（mu3 探针同形）；返回类型不入
    // mangled，符号不变。全仓无调用点（grep 实证）。
    unsigned int make_unique_id();
    int find_session(unsigned int accId);   // ORIG caller 0x86c4d5f 全字直存 = int 返回视角
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
    bool IsPowerWarSafeZone(int vill, int area, ENUM_POWER_SIDE_TYPE side);
    bool IsPowerWarSafeZone(int vill, int area);

    // ---- 聊天 ----
    void make_packet_chat_msg(PacketGuard& packet, CUser* user, char* msg, int len,
                              char type, ENUM_NOTIPACKET notify);
    void make_packet_chat_msg_hyper_link(PacketGuard& packet, CUser* user,
                                         MSG_ITEM_HYPER_LINK_MESSAGE& linkMsg, ENUM_NOTIPACKET notify);
    void send_chat_msg(CUser* from, char type, unsigned short target,
                       unsigned int characNo, char* msg, int len, char* nick,
                       int nickLen, ENUM_NOTIPACKET notify);
    void send_chat_msg_hyper_link(CUser* from, MSG_ITEM_HYPER_LINK_MESSAGE& linkMsg, ENUM_NOTIPACKET notify);
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
    void SendPowerUpList(ENUM_POWER_UP_ACTIVATE_TYPE activateType, std::vector<CUser*>& list);
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

// ---- 全局数据（定义见 GameWorld.cpp）----
extern int MAX_VILLAGE_NUM;                    // 0948b2b0
extern std::map<int, std::string> g_townScriptFileList;  // 09500ea0

int _makeBasicInfoHeader(PacketGuard& packet);  // ORIG 086c8396

#endif  // DNF_GAME_GAMEWORLD_H_
