#ifndef GAME_CFAIRMATCH_H_
#define GAME_CFAIRMATCH_H_

// ============================================================================
// fair_pvp::CFairMatch —— 公平 PvP 对局单元（IMatch 派生，ORIG df_game_r）
// 依据 docs/class_func_reports/fair_pvp__CFairMatch/、fair_pvp__TeamInfoThird/
// 与 objdump（ctor 0x856284a、dtor 链、静态 pWaitingRoomList_/
// nWaitingRoomListRefCount_/bMatchable_）。
// 对象布局（字节偏移，自 ctor/GetUser/GetUserList 反汇编核实）：
//   +0x000 vptr（IMatch，36 槽虚表 PTR__CFairMatch_08ca3be8）
//   +0x004 int   匹配状态（1=排队中 / 2=对战中；OnRegisterMatch 判定）
//   +0x008 PVP_ROOM_INFO 房间信息（0x10）
//   +0x018 int   TimerStartMatching 附带 id（registNextTimer 第 3 参）
//   +0x01c TeamInfoThird[2] 两支队伍（各 0x48）
//   +0x0ac..+0x0bf 未命名 int×5（_ChangeTeamLeader/_MoveRoom 等使用区，
//                  本批未逐一定名，按偏移成员保留占位命名）
//   +0x0c0 int   ctor 置 0
// ============================================================================
#include <list>
#include <map>

#include "MatchRoomList.h"    // IMatch / CMultiMatchRoomList / CLeagueMatchRoomList
#include "PvPTypes.h"         // PVP_ROOM_INFO / PVP_BATTLE_MODE
#include "MissionTypes.h"     // PVP_BATTLE_MODE（守卫定义）
#include "SchoolMatchEnums.h" // ENUM_SCHOOL_MATCH_TEAM / ENUM_ELO_RATING_SYSTEM
#include "CFairPvPScore.h"    // JobKey（本域共享类型）
#include "CCompoundAvatar.h"   // ENUM_CHARACTERJOB（守卫幂等）
#include "CDataManager.h"      // CharacterKind（PlayerInfoThird +0x05 具名子对象）

class CUser;
class PacketGuard;
class Inven_Item;

namespace fair_pvp
{

// ---- ORIG r 符号 fair_pvp::maxRematchCount_（internal linkage，每包含 TU 一份
//      .rodata；值=3，objdump -s 0x8bbf830 实证）。CheckRematch/OnRematchTimeout
//      以它作最大重赛局数。 ----
static const int maxRematchCount_ = 3;

// ---- PlayerInfoThird（单队员视图，stride 0x10；add/del/refresh 推导） ----
struct PlayerInfoThird
{
    // ORIG W C1/C2 0x8568fda：仅构造 +0x05 的 CharacterKind 子对象（编译器发
    // call _ZN13CharacterKindC1Ev @0x8568652，清零 m_job/m_grow 两字节）；
    // 其余字段不初始化。成员具名为 CharacterKind 后隐式 ctor 调用与 ORIG 同形。
    // [b22判词·AE] 指令序列与 ORIG 逐行同（9=9）；唯一残差 call CharacterKindC1Ev
    // 为 --no-compile 读未 relink .o 的重定位占位，relink 后转 AE；非语义差。
    PlayerInfoThird() : m_kind() {}
    void reset();                                   // ORIG 经 TeamInfoThird::reset 调用

    CUser*        m_user;      // +0x00
    bool          m_flag;      // +0x04（ORIG CheckRematch 0x08565d04 +0x5e `xor $0x1,%eax` 取反实证 bool，P22/P73）
    CharacterKind m_kind;      // +0x05（{ m_job@+0x05, m_grow@+0x06 }，ORIG C1 实证）
    int           m_pvpGrade;  // +0x08
    float         m_rating;    // +0x0c
};  // sizeof 0x10

// ---- TeamInfoThird（一支队伍最多 4 人，sizeof 0x48） ----
class TeamInfoThird
{
public:
    TeamInfoThird();                                 // ORIG C1/C2（reset 语义）
    void reset();                                    // 0x8562582：清玩家数组，m_field00=0、m_field04=2
    int  add(CUser* user);                           // 0x85625c6：首个空位写入
    int  del(CUser* user);                           // 0x85626aa
    void refresh();                                  // 0x85624de：重算每人 rating
    int  size() const;                               // 0x856270c
    float GetTeamRating();                           // 0x85627b2（平均 rating，全 0 返 0）
    void SendToTeam(PacketGuard& pkt);               // 0x856274c：CUser::Send 全员

    bool             m_field00;    // +0x00（reset 清 0；ORIG movb 单字节清；CheckRematch 0x08565d04 +0x5e xor $1 取反实证 bool，P22）
    int              m_field04;    // +0x04（reset 置 2）
    PlayerInfoThird  m_players[4]; // +0x08（0x40）
};  // sizeof 0x48

// ---- CFairMatch 本体 ----
class CFairMatch : public IMatch
{
public:
    CFairMatch();                                    // ORIG C1/C2 0x856284a
    ~CFairMatch();                                   // ORIG D1/D2/D0（由报告 dtor 报告实现）

    // -- IMatch 覆盖（对应 worklist 符号，地址见注释） --
    virtual float GetGap(IMatch* other);                       // 0x856491c
    virtual CUser* GetUser(unsigned int idx);                  // 0x8565140
    virtual void GetUserList(void* out);                       // 0x8565184（拷贝第一支队伍 0x48 字节）
    virtual void RequestMap(short& mapNo, PVP_BATTLE_MODE& mode);  // 0x8562bae
    virtual int OnCreateRoom(CUser* user);                     // 0x8562d08（成功 0 / 装备不合规 0x13；IMatch 基类 int 同步）
    virtual void OnDestroyRoom(CUser* user);                   // 0x8562dc6
    virtual void OnJoinRoom(CUser* user);                      // 0x8562df6
    virtual void OnLeaveRoom(CUser* user, PvP_Room* room, int a, int b);  // 0x8562fbc
    virtual int OnRegisterMatch(CUser* user, PvP_Room* room); // 0x8563bba（ORIG 返 1）
    virtual int OnUnregisterMatch(CUser* user, PvP_Room* room);  // 0x85663cc6?（ORIG 返 1）
    virtual int OnMatchRoom(PvP_Room* room, int a);          // 0x8564150（ORIG 返 1）
    virtual void OnStartFight();                               // 0x8564952
    virtual void OnEndFight(PvP_Room* room);                  // 0x8565048
    virtual int OnWalkOut(PvP_Room* room, int a);            // 0x8564fb2（ORIG 返 1）
    virtual bool OnReady(CUser* user, bool ready);            // 0x8565018（!ready && state==4 → false；IMatch 基类 bool 同步）
    virtual void OnReward(PvP_Room* room, int a, bool b);    // 0x8565966
    virtual void OnEndPvP(PvP_Room* room);                   // 0x8565c4e
    virtual void OnCompleteLoad(CUser* user);                 // 0x8564962
    virtual void OnConnectP2PTimeout(PvP_Room* room);        // 0x8564d3a
    virtual void OnConnectP2P(CUser* user, PvP_Room* room, char* addr, int port);  // 0x8564a8e
    virtual int OnGiveup(CUser* user);                        // 0x8564de0
    virtual void OnRematchTimeout(PvP_Room* room);           // 0x85665f92?
    virtual void OnAskRematch(CUser* user, int a);           // 0x856664a0
    virtual void OnWaitRematch(CUser* user, PvP_Room* room); // 0x8566649a
    virtual int  CheckJoinable(PvP_Room* room, CUser* user);// 0x8562a6e
    virtual int  CheckSeat(int a);                            // 0x8562b78（return a != 2）
    virtual bool CheckMatchable(IMatch* other);               // 0x8565c54
    virtual int  GetEndPvPResultTimeout();                    // 0x8566（报告为准）
    virtual void OnEndPvPResult(PvP_Room* room, CUser* user);// 0x8566694c
    virtual int  GetPlayerCount();                            // 0x8566d3e
    virtual IMatch* GetNextMatch();                            // 无独立报告→依 vtable 槽实现
    virtual void SetNextMatch(IMatch* next);

    // -- 非 IMatch worklist 方法 --
    static void SetMatchAble(bool able);                     // 0x8562b96（写静态 bMatchable_）
    void UpdateScore(PvP_Room* room, PlayerInfoThird& info,
                     ENUM_SCHOOL_MATCH_TEAM team, int a, int b, int c);  // 0x8565700
    int  CheckRematch(PvP_Room* room);                       // 0x8565d04
    int  CheckUnusableEquipment(const Inven_Item* item);      // 0x8562bdc
    // ORIG 调用点 0x08562d14 仅压 user 无 this（P88）⇒ static。
    static bool CheckUnusableEquipmentInUser(CUser* user);    // 0x8562cfe（恒返 1；CheckJoinable/OnCreateRoom 以 ! 取反 xor $1 实证）
    void GiveTrainingSeal(CUser* user, bool bonus);           // 0x85652c8
    static int GetPlayCountPerOneTeam();                      // 0x8562834（ORIG 恒返 1；调用点不传 this 实证 static；
                                                              // 定义在 MatchRoomLCB.cpp，勿重定义）
    void SetPlayCountPerOneTeam(unsigned int n);              // 0x8562b90（ORIG 空体 ret）
    void _SetEnterCharacNo(int no);                            // 0x856（报告地址为准）

    PvP_Room* _MakeRoom(CUser* user);                         // 0x8563692
    void _RegisterWaitingRoom(PvP_Room* room);               // 0x8563a32（静态化处理见 cpp）
    void _BackupRoomInfo(PvP_Room* room);                    // 0x8563674
    int  _CheckMatchable(PvP_Room* room) const;               // 0x8563a54
    bool _CheckRematchable(PvP_Room* room) const;              // 0x8563b16（ORIG 返 bool：CheckRematch 以 ! 取反 xor $1）
    bool _CheckObserver(PvP_Room* room, CUser* user);        // 0x85629f2（CheckJoinable 0x08562a6e +0xdb `xor $0x1,%eax` bool 实证，P22）
    bool _CheckSameTeam(PvP_Room* room, CUser* user);        // 0x85629e8（同上 +0xb4 xor $0x1 实证）
    int  _CheckMovableRoom(IMatch* other, PvP_Room* room, PvP_Room** out);  // 0x8563ed8
    void _ChangeTeamLeader(PvP_Room* room, ENUM_SCHOOL_MATCH_TEAM team, int idx);  // 0x8563272
    void _SendMatchStatePacket(int a, int b);                  // 0x856（报告为准）
    int  _GetOppositeTeam(int team);                           // 0x8564f8c
    ENUM_SCHOOL_MATCH_TEAM _GetOtherTeam(ENUM_SCHOOL_MATCH_TEAM t);         // 0x8566476
    void _UpdateRating(int teamNo, ENUM_ELO_RATING_SYSTEM system);   // ORIG T 0x08562e1e（ORIG 为 void）
    float _GetRating(ENUM_SCHOOL_MATCH_TEAM team);                   // ORIG W 0x08568966
    int  _FindTeam(CUser* user);                              // ORIG T 0x085663a8（返回队伍号 0/1，找不到=2）
    void _PullOutRoom(CUser* user, PvP_Room* room, int seat); // 0x85633ce
    int  _PutInRoom(CUser* user, PvP_Room* room, bool observer);  // 0x85634bc
    PvP_Room* _ReturnTeam(PvP_Room* room, int team, CUser* user); // ORIG T 0x08564dea（返回自建房指针）
    int    _PullOutRoomTeam(PvP_Room* room, ENUM_SCHOOL_MATCH_TEAM team);  // ORIG T 0x08565d86（恒返 1）
    void _MoveRoom(PvP_Room* from, PvP_Room* to, bool flag);  // 0x8563d32

private:
    static CMultiMatchRoomList* pWaitingRoomList_;       // ORIG 全局 0x93xxxx（cpp 定义）
    static int nWaitingRoomListRefCount_;                // 引用计数
    static int nextAuthValue_;                           // ORIG B 0x0943e150（房间授权自增号）
    static bool bMatchable_;                             // SetMatchAble 写入

public:  // 布局字段（偏移契约固定，勿增删；语义随实现标注在 cpp 注释）
    /*+0x004*/ int m_field04;         // 匹配状态机值
    /*+0x008*/ PVP_ROOM_INFO m_roomInfo;
    /*+0x018*/ int m_matchNotifyId;   // TimerStartMatching 第 3 参
    /*+0x01c*/ TeamInfoThird m_teams[2];
    /*+0x0ac*/ int m_fieldAC;
    /*+0x0b0*/ int m_fieldB0;
    /*+0x0b4*/ int m_fieldB4;
    /*+0x0b8*/ int m_fieldB8;
    /*+0x0bc*/ int m_fieldBC;
    /*+0x0c0*/ IMatch* m_fieldC0;     // ctor 清零（next match 槽；指针与 SetNextMatch
                                      // 形参同型=真类型化，NL7 判据 N76 翻形）
};

}  // namespace fair_pvp

#endif  // GAME_CFAIRMATCH_H_
