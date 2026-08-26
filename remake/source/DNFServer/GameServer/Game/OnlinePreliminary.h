#ifndef GAME_ONLINEPRELIMINARY_H_
#define GAME_ONLINEPRELIMINARY_H_

// ============================================================================
// online_preliminary —— 线上预选赛域权威头（唯一归口）
// 依据 docs/class_func_reports/online_preliminary__*/ 与 objdump：
//   COnlinePreliminaryTeamMgr ctor 0x858860c / ~D1 0x85886fe
//   OnEnterUser 0x85887c8 / OnLeaveUser 0x8588ae8 / ResetDailyMidnight
//   0x8588c5c / FindTeam 0x858875e / OnLoadTeamInfo 0x8588be4
//   CTeamFactory：StaticPool<COnlinePreliminaryTeam,600> 薄包装
//     （Acquire 0x8588dd8 → StaticPool::Acquire 0x8588e56；Free 同型）
//   COnlinePreliminaryMember ctor 0x8585f34（CData 派生；
//     +0x08 rating / +0x0c win / +0x10 lose）
//   COnlinePreliminaryMemberMatchList ctor 0x8586398（CData 派生；
//     +0x08 map<CharacterInServer,PlayCountInfo>）
//   COnlinePreliminary ctor 0x855fefe（IMatch 派生 + PVP_ROOM_INFO@+0x08 +
//     静态 pWaitingRoomList_(CLeagueMatchRoomList)/nWaitingRoomListRefCount_）
// 布局契约：COnlinePreliminaryTeam sizeof == 0x40（原 CUserCharacInfo.h 版式）。
// ============================================================================
#include <map>
#include "CBoosterGage.h"      // charac_expand::CData

#include "StaticPool.h"
#include "MatchRoomList.h"      // IMatch / IRoomList / CLeagueMatchRoomList
#include "MissionTypes.h"       // PVP_BATTLE_MODE
#include "SchoolMatchEnums.h"   // ENUM_SCHOOL_MATCH_TEAM / ENUM_ELO_RATING_SYSTEM
#include "PvPTypes.h"           // PVP_ROOM_INFO

class CUser;
class Inven_Item;

namespace online_preliminary
{

struct PlayCountInfo
{
    PlayCountInfo() : m_count(0), m_flag(0) {}
    PlayCountInfo(unsigned int count, bool flag)
        : m_count(count), m_flag(flag ? 1 : 0) {}

    unsigned int  m_count;  // +0x00
    unsigned char m_flag;   // +0x04（对方本日已交手标记）
    char          m_pad[3]; // +0x05..0x07（保持未初始化语义由编译器补位）
};

// ---- CharacterInServer（服务器内角色键，sizeof 8） ----
// ctor W 0x858691e(int serverId, uint uniqueId)；operator< 见实现。
struct CharacterInServer
{
    CharacterInServer(int serverId, unsigned int uniqueId)
        : m_serverId(serverId), m_uniqueId(uniqueId) {}
    CharacterInServer() : m_serverId(0), m_uniqueId(0) {}

    bool operator<(const CharacterInServer& rhs) const;

    int           m_serverId;  // +0x00
    unsigned int  m_uniqueId;  // +0x04
};

// ---- PlayCountInfoEx / COnlinePreliminaryTeam（自 CUserCharacInfo.h 迁入） ----
struct PlayCountInfoEx
{
    PlayCountInfoEx() : m_count(0), m_flag(0) {}
    PlayCountInfoEx(unsigned int count, bool flag)
        : m_count(count), m_flag(flag ? 1 : 0) {}

    unsigned int   m_count;  // +0x00
    unsigned char  m_flag;   // +0x04
    char           m_pad[3]; // +0x05..0x07
};

class COnlinePreliminaryTeam
{
public:
    int GetTeamId() const;
    const char* GetTeamName() const;
    // ORIG T 0x0858831a：data 布局 int count@+4 + 记录 8B（key@0,value@4）
    // 填充 +0x24 map<int,PlayCountInfoEx>。
    int loadDataPreliminaryTeamMatchList(char* data);
    // ORIG T（报告 loadDataPreliminaryTeam）：data 头部填充本队基础字段。
    void loadDataPreliminaryTeam(char* data);

    void IncreaseWinCount();                        // README 地址为准
    void IncreaseLoseCount();
    void SetRating(float rating);
    float GetRating() const;                         // _ZNK…9GetRatingEv
    void IncreaseCurrentUserCount();
    void DecreaseCurrentUserCount();
    void SetCurrentUserCount(int n);
    int  GetCurrentUserCount() const;
    void IncreaseEachOtherTeamPlayCount(int teamId);
    int  GetEachOtherTeamPlayCount(int teamId) const;
    void saveData();
    void _saveTeam();
    void _saveTeamMatchList();
    void Reset();                                    // T 0x858838a
    void ResetDailyMidnight();

    int m_field00;                    // +0x00（teamId；GetTeamId）
    int m_field04;                    // +0x04
    int m_field08;                    // +0x08
    int m_winCount;                   // +0x0c
    int m_loseCount;                  // +0x10
    char m_field14[0x10];             // +0x14..0x23（队名缓冲）
    std::map<int, PlayCountInfoEx> m_playCountMap;  // +0x24（0x18）
    unsigned char m_hasWinFlag;       // +0x3c
    unsigned char m_hasLoseFlag;      // +0x3d
};  // sizeof 0x40

// ---- COnlinePreliminaryMember（CData 派生成员数据） ----
class COnlinePreliminaryMember : public charac_expand::CData
{
public:
    COnlinePreliminaryMember();                      // 0x8585f34
    ~COnlinePreliminaryMember();

    virtual bool loadData(CUser* pUser, char* pData);   // 0x8585f9e
    virtual bool _saveData(CUser* pUser);               // 0x8585fd6
    virtual void _reset();                              // 0x8586100

    void SetRating(float rating);                    // 0x8586168
    float GetRating() const { return m_rating; }
    void IncreaseWinCount();                          // 0x8586124
    void IncreaseLoseCount();

    /*+0x08*/ float m_rating;
    /*+0x0c*/ int   m_winCount;
    /*+0x10*/ int   m_loseCount;
};  // 尺寸 0x14

// ---- COnlinePreliminaryMemberMatchList（每日对过谁） ----
class COnlinePreliminaryMemberMatchList : public charac_expand::CData
{
public:
    COnlinePreliminaryMemberMatchList();             // 0x8586398
    ~COnlinePreliminaryMemberMatchList();

    virtual bool loadData(CUser* pUser, char* pData);
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();                               // 0x8586848

    void IncreasePeerPlayCount(int serverId, unsigned int uniqueId);  // 0x8586208
    unsigned int GetPeerPlayCount(int serverId, unsigned int uniqueId) const;  // 0x8586184

    /*+0x08*/ std::map<CharacterInServer, PlayCountInfo> m_peerMap;
};  // 尺寸 0x20

// ---- CTeamFactory：StaticPool<600> 薄包装（对象尺寸 0x30） ----
class CTeamFactory : public StaticPool<COnlinePreliminaryTeam, 600>
{
public:
    COnlinePreliminaryTeam* Acquire();               // 0x8588dd8
    void Free(COnlinePreliminaryTeam* team);          // 0x8588dec
};

// ---- COnlinePreliminaryTeamMgr（s_onlinePreliminaryTeamMgr 全局持有者） ----
class COnlinePreliminaryTeamMgr
{
public:
    COnlinePreliminaryTeamMgr();                     // 0x858860c
    ~COnlinePreliminaryTeamMgr();                    // 0x85886fe

    bool OnEnterUser(CUser* user, char* teamData);   // 0x85887c8
    void OnLeaveUser(CUser* user);                   // 0x8588ae8
    void OnLoadTeamInfo(CUser* user, char* teamData);// 0x8588be4
    void ResetDailyMidnight();                       // 0x8588c5c
    static COnlinePreliminaryTeam* FindTeam(int teamId);

private:
    CTeamFactory* m_factory;                          // +0x00（new nothrow 0x30）
    std::map<int, COnlinePreliminaryTeam*> m_teams;   // +0x04
};  // 尺寸 0xc

// ---- COnlinePreliminary：预选赛对局单元（IMatch 派生） ----
class COnlinePreliminary : public IMatch
{
public:
    COnlinePreliminary();                             // 0x855fefe
    ~COnlinePreliminary();

    virtual void OnCreateRoom(CUser* user);
    virtual void OnDestroyRoom(CUser* user);
    virtual void OnJoinRoom(CUser* user);
    virtual void OnLeaveRoom(CUser* user, PvP_Room* room, int a, int b);
    virtual void OnRegisterMatch(CUser* user, PvP_Room* room);
    virtual void OnMatchRoom(PvP_Room* room, int a);
    virtual void OnStartFight();
    virtual void OnEndFight(PvP_Room* room);
    virtual void OnWalkOut(PvP_Room* room, int a);
    virtual void OnReady(CUser* user, bool ready);
    virtual void OnReward(PvP_Room* room, int a, bool b);
    virtual void OnCompleteLoad(CUser* user);
    virtual void OnConnectP2PTimeout(PvP_Room* room);
    virtual void OnConnectP2P(CUser* user, PvP_Room* room, char* addr, int port);
    virtual void OnGiveup(CUser* user);
    virtual int  CheckJoinable(PvP_Room* room, CUser* user);
    virtual int  CheckSeat(int a);
    virtual bool CheckMatchable(IMatch* other);
    virtual CUser* GetUser(unsigned int idx);
    virtual void GetUserList(void* out);
    virtual void RequestMap(short& mapNo, PVP_BATTLE_MODE& mode);

    int  _GetRating(ENUM_SCHOOL_MATCH_TEAM team);
    int  _GetRating(CUser* user);
    void _SetRating(ENUM_SCHOOL_MATCH_TEAM team, float v);
    void _SetRating(CUser* user, float v);
    int  _PutInRoom(CUser* user, PvP_Room* room, bool observer);
    void _PullOutRoom(CUser* user, PvP_Room* room, int seat);
    void _ReturnTeam(PvP_Room* room, int team, CUser* user);
    void _UpdateRating(int diff, ENUM_ELO_RATING_SYSTEM sys);
    int  _CheckObserver(PvP_Room* room, CUser* user) const;
    int  _CheckSameTeam(PvP_Room* room, CUser* user) const;
    int  _CheckMatchable(PvP_Room* room) const;
    int  _GetOppositeTeam(int team);
    void _IncreaseWinCount(ENUM_SCHOOL_MATCH_TEAM team);
    void _IncreaseWinCount(CUser* user);
    void _IncreaseLoseCount(ENUM_SCHOOL_MATCH_TEAM team);
    void _IncreaseLoseCount(CUser* user);
    void _RegisterWaitingRoom(PvP_Room* room);
    void _SendMatchStatePacket(int a, int b);
    unsigned int GetPlayCountPerOneTeam();
    void SetPlayCountPerOneTeam(unsigned int n);
    unsigned int _GetEachOtherPlayCount(CUser* a, CUser* b);
    unsigned int _GetEachOtherPlayCount();
    void _IncreaseEachOtherPlayCount(CUser* a, CUser* b);
    void _IncreaseEachOtherPlayCount();
    int  CheckUnusableEquipmentInUser(CUser* user);
    void _MakeRoom(CUser* user);
    int  GetTeamId();
    static bool CheckUnusableEquipment(const Inven_Item* item);  // 迁自 CUserCharacInfo.h

private:
    static CLeagueMatchRoomList* pWaitingRoomList_;   // 全局延迟单例
    static int nWaitingRoomListRefCount_;

public:  // 布局区（偏移契约；扩展仅限有报告证据的字段——由实现批次在此追加命名）
    /*+0x004*/ int m_field04;
    /*+0x008*/ PVP_ROOM_INFO m_roomInfo;
    // ==== 成员扩展区（有报告证据才追加；禁止改上方字段偏移） ====
};

}  // namespace online_preliminary

#endif  // GAME_ONLINEPRELIMINARY_H_
