#ifndef GAME_CLEAGUEMATCH_H_
#define GAME_CLEAGUEMATCH_H_

// ============================================================================
// df_game_r 还原 —— CLeagueMatch（学院联赛匹配单元，IMatch 派生）
// 依据 docs/class_func_reports/CLeagueMatch/ 全部报告与 ORIG objdump 转写。
//
// 对象布局（ORIG ctor 0x0855cd74 / 各方法访问偏移推导）：
//   +0x000 vptr（IMatch 虚表 PTR__CLeagueMatch_08ca3d20，36 槽）
//   +0x004 int        m_state（匹配状态机：0 空闲 / 1 排队 / 2 匹配中 /
//                              3 待加载 / 4 对战 / 5 结算；见各 On* 报告）
//   +0x008 PVP_ROOM_INFO m_roomInfo（0x10；title@0/byte4/short6/byte8/passwd@c）
//   +0x018 int        m_authValue（ctor 时 nextAuthValue_ 快照，OnMatchRoom 校验）
//   +0x01c TeamInfo   m_teamA（队伍一，0x70）
//   +0x08c TeamInfo   m_teamB（队伍二，0x70）
//
// 静态成员（df_game_r nm -S 核实）：
//   pWaitingRoomList_           B 0x0943e064
//   nWaitingRoomListRefCount_   B 0x0943e068
//   nextAuthValue_              B 0x0943e06c
//   bMatchable_                 B 0x0943e060
//   nPlayerCountPerOneTeam_     D 0x093abb74（初值 2）
//   nBattleMode_                D 0x093abb75（初值 3）
// ============================================================================

#include "MatchRoomList.h"   // IMatch / CLeagueMatchRoomList / IRoomList
#include "PvPTypes.h"        // PVP_ROOM_INFO / PVP_BATTLE_MODE

// 与 CUser.h 共享同一守卫（CLeagueMatch.h 不直接引入重载全量 CUser.h）；
// 值域与 ORIG E15ENUM_BADGE_TYPE 兼容（addBadge 0/1/2 三色徽章）。
#ifndef DNF_ENUM_BADGE_TYPE_DEFINED
#define DNF_ENUM_BADGE_TYPE_DEFINED
enum ENUM_BADGE_TYPE
{
    ENUM_BADGE_TYPE_0 = 0
};
#endif

class CUser;
class PvP_Room;

// ---- PlayerInfo（单队员快照，stride 0x1c；布局依 TeamInfo::add/refresh 报告） ----
struct PlayerInfo
{
    void reset();                       // ORIG W 0x085687d4

    /*+0x00*/ unsigned int  m_accId;    // get_acc_id 快照
    /*+0x04*/ float         m_rating;   // Elo 个人分快照
    /*+0x08*/ CUser*        m_user;
    /*+0x0c*/ unsigned char m_flag;     // !=0 槽位占用
    /*+0x10*/ int           m_schoolNo; // GetSchoolNo 快照
    /*+0x14*/ short         m_badges[3];// GetDailyBadge(short*) 三枚章
};  // sizeof 0x1c
// PlayerInfo 排序比较器（ORIG T 0x0855c952）：先比 flag 升序占后，
// 同 flag 比 rating 高者在前（std::sort 用）。
bool CompareRating(PlayerInfo const& a, PlayerInfo const& b);

// ---- TeamInfo（一支队伍最多 4 人，sizeof 0x70） ----
class TeamInfo
{
public:
    void reset();                       // ORIG T 0x0855cb16
    void refresh();                     // ORIG T 0x0855c9ae
    int  add(CUser* user);              // ORIG T 0x0855cb54
    int  del(CUser* user);              // ORIG T 0x0855cc9c
    int  size() const;                  // ORIG T 0x0855cd26

    PlayerInfo m_players[4];            // +0x00
};  // sizeof 0x70

// ---- CLeagueMatch 本体 ----
class CLeagueMatch : public IMatch
{
public:
    CLeagueMatch();                     // ORIG C1/C2 0x0855cd74
    virtual ~CLeagueMatch();            // ORIG D1 0x0855ce4c / D0 0x0855ceda

    // -- IMatch 虚槽覆盖（worklist 符号；返回类型跟随 MatchRoomList.h 基类
    //    以保持虚表槽一致，ORIG 的 bool 返回值经 eax 传递此处不回传【推断】） --
    virtual float GetGap(IMatch* other);                        // 0x0855e12a
    virtual CUser* GetUser(unsigned int idx);                   // 0x0855e5e4
    virtual void GetUserList(void* out);                        // 0x0855e642
    virtual void RequestMap(short& mapNo, PVP_BATTLE_MODE& mode);  // 0x0855d096
    virtual int GetExtraWinPoint(int extra);                    // 0x0855d0bc
    virtual void OnCreateRoom(CUser* user);                     // 0x0855d104
    virtual void OnDestroyRoom(CUser* user);                    // 0x0855d168
    virtual void OnJoinRoom(CUser* user);                       // 0x0855d198
    virtual void OnLeaveRoom(CUser*, PvP_Room*, int, int);      // 0x0855d1c0
    virtual void OnRegisterMatch(CUser*, PvP_Room*);            // 0x0855db58
    virtual void OnMatchRoom(PvP_Room*, int authValue);        // 0x0855dc64
    virtual void OnStartFight();                                // 0x0855e21c
    virtual void OnEndFight(PvP_Room* room);                   // 0x0855e55a
    virtual void OnWalkOut(PvP_Room* room, int seat);          // 0x0855e4c4
    virtual void OnReady(CUser*, bool ready);                   // 0x0855e52a
    virtual void OnReward(PvP_Room*, int winTeam, bool draw);  // 0x0855eb22
    virtual void OnCompleteLoad(CUser* user);                   // 0x0855e22c
    virtual int CheckJoinable(PvP_Room* room, CUser* user);    // 0x0855cf8c
    virtual int CheckSeat(int seat);                            // 0x0855d058
    virtual bool CheckMatchable(IMatch* other);                 // 0x0855fa14

    // -- 非 IMatch 方法 --
    static void SetMatchAble(bool able);                        // 0x0855d07e
    void SetPlayCountPerOneTeam(unsigned int n);                // 0x0855d070
    // ORIG vtable +0x90（第 37 槽，派生新增唯一虚函数）＝ CLeagueMatch::GetSchoolNo；
    // GetIndentification 覆盖体即虚调本槽。
    virtual int GetSchoolNo();                                  // 0x0855f9aa (T)
    virtual int GetIndentification();                           // 0x08568834 (W)
    int GetBattleMode();                                        // 0x084ed387 (W)
    void SetBattleMode(int mode);                               // 0x082743a0 (W)
    int GetPlayCountPerOneTeam();                               // 0x084ed37b (W)

    PvP_Room* _MakeRoom(CUser* user);                           // 0x0855d572
    void _RegisterWaitingRoom(PvP_Room* room);                 // 0x0855d884
    void _BackupRoomInfo(PvP_Room* room);                      // 0x0855d554
    int  _CheckMatchable(PvP_Room* room) const;                 // 0x0855da56
    void _PullOutRoom(CUser* user, PvP_Room* room, int seat);   // 0x0855d3d2
    int  _PutInRoom(CUser* user, PvP_Room* room, bool observer);// 0x0855d42a
    void _ReturnTeam(PvP_Room* room, int team, CUser* except);  // 0x0855e378
    void _SendMatchStatePacket(int state, int teams);           // 0x0855e664
    int  _getSameSchool(unsigned int* schools, int selfIdx,
                        int count, unsigned int schoolNo);      // 0x0855e7a8
    int  _GetOppositeTeam(int team);                            // 0x0855e49e
    int  _CheckSchool(PvP_Room* room, CUser* user);            // 0x0855cef8
    int  addBadge(PlayerInfo* info, ENUM_BADGE_TYPE badgeType,
                  unsigned char* msgFlag, bool extra);          // 0x0855e7ee

public:  // 布局字段（偏移契约固定，勿增删）
    /*+0x004*/ int           m_state;
    /*+0x008*/ PVP_ROOM_INFO m_roomInfo;
    /*+0x018*/ int           m_authValue;
    /*+0x01c*/ TeamInfo      m_teamA;
    /*+0x08c*/ TeamInfo      m_teamB;

    // -- 静态成员（ORIG nm -S 地址见文件头注释） --
    static CLeagueMatchRoomList* pWaitingRoomList_;
    static int nWaitingRoomListRefCount_;
    static int nextAuthValue_;
    static bool bMatchable_;
    static unsigned char nBattleMode_;             // D 初值 3
    static unsigned char nPlayerCountPerOneTeam_;  // D 初值 2
};

#endif  // GAME_CLEAGUEMATCH_H_
