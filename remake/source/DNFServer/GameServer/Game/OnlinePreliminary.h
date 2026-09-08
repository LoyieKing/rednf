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

// ---- CEloRating / PvPGlobalEnvironmentVariable（本 TU 引用的 ORIG 弱符号宿主） ----
// calc W 0x844dca1（实现见 OnlinePreliminaryMatch.cpp，objdump 忠实转写【推断】标注）；
// school_match_elo_k_ D 0x093abb6c / elo_k2_ D 0x093abb70。
class CEloRating
{
public:
    static void calc(float& ratingSelf, float& ratingOther,
                     ENUM_ELO_RATING_SYSTEM sys, int k);
};

class PvPGlobalEnvironmentVariable
{
public:
    static int school_match_elo_k_;   // ORIG D 0x093abb6c
    static int school_match_elo_k2_;  // ORIG D 0x093abb70
    static int school_match_daily_point_;                  // ORIG D 0x093abb60（初值 1）
    static int school_match_daily_point_weekends_;         // ORIG D 0x093abb64（初值 2）
    static int school_match_fight_count_for_gain_point_;   // ORIG D 0x093abb68（初值 5）
};

// ---- PlayerInfoSecond / TeamInfoSecond（ORIG 全局类） ----
// PlayerInfoSecond::reset W 0x8568850；TeamInfoSecond add T 0x855fdfe /
// del 0x855fe54 / reset 0x855fdc0 / refresh 0x855fcf6 / size const
// 0x855feaa（本 TU 实现全部符号）。sizeof(TeamInfoSecond)==0x28。
class PlayerInfoSecond
{
public:
    /* ORIG W 0x8568850（weak ⇒ 头内 inline 定义）。类内 inline 使 4.4 对
     * TeamInfoSecond::reset 的调用按"外部 callee"帧策略分配（探针
     * /tmp/p26_slot5.cpp probeI 实证帧 0x28/i=-0xc，与 ORIG 一致；
     * 类外定义时 4.4 塌缩帧 0x14/i=-0x4，probeC2 实证）。 */
    void reset() { m_user = 0; m_flag = 0; }

    /*+0x00*/ CUser*        m_user;
    /*+0x04*/ bool          m_flag;   // b28c-L26 档②（Main 三档口径）：ORIG add 0x0855fe18
                                      // 为 `movzbl 0xc(%eax,%edx,8),%eax; xor $0x1,%eax;
                                      // test %al,%al; je` 寄存器 xor 物化，lane20A 实证
                                      // 「字段 bool 化 + 裸 !field」为 4.4 自然复现路径
                                      // （CSharedServerMessageManager m_isSet 3→0 判例）。
                                      // 偏移/sizeof 8/mangled 不变；m_pad[3] 保 +0x05..07。
    char          m_pad[3];
};  // sizeof 8

class TeamInfoSecond
{
public:
    // ORIG nm 无 TeamInfoSecond::C1/C2 符号 ⇒ 隐式平凡 ctor；显式声明会令
    // 4.4 在 COnlinePreliminary ctor 为 m_teams[2] 生成元素构造循环（ORIG 无）。
    void reset();              // 0x855fdc0
    int  add(CUser* user);     // 0x855fdfe
    int  del(CUser* user);     // 0x855fe54
    void refresh();            // 0x855fcf6
    int  size() const;         // 0x855feaa

    /*+0x00*/ int   m_teamId;
    /*+0x04*/ float m_rating;
    /*+0x08*/ PlayerInfoSecond m_seats[4];
};  // sizeof 0x28

namespace online_preliminary
{

struct PlayCountInfo
{
    PlayCountInfo() : m_count(0), m_flag(0) {}
    // ORIG ctor 0x86ca0e4：bool 直存（mov %dl,0x4），无三目规范化。
    // b28 翻案（P211 异型化族）：m_flag uchar→bool ⇒ 4.4 尾 store 序翻为
    // LHS 地址先 = ORIG 形（/tmp/l12p1.cpp B/D 探针：bool 字段恒 `mov this;
    // movzbl; mov dl`，uchar 字段恒 RHS 先）。
    PlayCountInfo(unsigned int count, bool flag)
        : m_count(count), m_flag(flag) {}

    unsigned int  m_count;  // +0x00
    bool          m_flag;   // +0x04（对方本日已交手标记）
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

    // [b28c-16] short→unsigned short：ORIG operator< 0x085868d4 读形为
    // `mov 0x8(%ebp),%eax; movzwl (%eax),%edx` 一步零扩展直落 edx；short 字段
    // + static_cast 在 4.4 恒拆两段（探针 p6 op1/op2）。16 位写宽（ctor mov %dx）不变。
    unsigned short m_serverId;  // +0x00（ORIG ctor 0x858691e 以 mov %dx 16 位写）
    unsigned int  m_uniqueId;  // +0x04
};

// ---- PlayCountInfoEx / COnlinePreliminaryTeam（自 CUserCharacInfo.h 迁入） ----
struct PlayCountInfoEx
{
    PlayCountInfoEx() : m_count(0), m_flag(0) {}
    // ORIG ctor 0x86ca141：bool 直存（mov %dl,0x4），无三目规范化。
    // b28：同 PlayCountInfo，m_flag uchar→bool 翻 N76 变体。
    PlayCountInfoEx(unsigned int count, bool flag)
        : m_count(count), m_flag(flag) {}

    unsigned int   m_count;  // +0x00
    bool           m_flag;   // +0x04
    char           m_pad[3]; // +0x05..0x07
};

class COnlinePreliminaryTeam
{
public:
    // ORIG C1 0x08587eb8 / D1 0x08587ece（实现 OnlinePreliminaryTeam.cpp）
    COnlinePreliminaryTeam();
    ~COnlinePreliminaryTeam();
    int GetTeamId() const;
    const char* GetTeamName() const;
    // ORIG T 0x0858831a：data 布局 int count@+4 + 记录 8B（key@0,value@4）
    // 填充 +0x24 map<int,PlayCountInfoEx>。
    int loadDataPreliminaryTeamMatchList(char* data);
    // ORIG T（报告 loadDataPreliminaryTeam）：data 头部填充本队基础字段。
    int loadDataPreliminaryTeam(char* data);

    void IncreaseWinCount();                        // README 地址为准
    void IncreaseLoseCount();
    void SetRating(float rating);
    float GetRating() const;                         // _ZNK…9GetRatingEv
    void IncreaseCurrentUserCount();
    void DecreaseCurrentUserCount();
    void SetCurrentUserCount(int n);
    int  GetCurrentUserCount();   // ORIG _ZN…GetCurrentUserCountEv（非 const，W 0x08588d7e）
    void IncreaseEachOtherTeamPlayCount(int teamId);
    int  GetEachOtherTeamPlayCount(int teamId) const;
    void saveData();
    bool _saveTeam();
    void _saveTeamMatchList();
    void Reset();                                    // T 0x858838a
    void ResetDailyMidnight();

    int m_field00;                    // +0x00（teamId；GetTeamId）
    int m_field04;                    // +0x04
    float m_rating08;                 // +0x08（ORIG float 位模式成员，SetRating/GetRating 位拷贝）
    int m_winCount;                   // +0x0c
    int m_loseCount;                  // +0x10
    char m_field14[0x10];             // +0x14..0x23（队名缓冲）
    std::map<int, PlayCountInfoEx> m_playCountMap;  // +0x24（0x18）
    bool m_hasWinFlag;                // +0x3c（ORIG xor$1 取反守卫实证 bool）
    bool m_hasLoseFlag;               // +0x3d
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
    // ORIG T 0x0858685e：经 CData 虚表 +0x1c 槽调用 _reset（每日重置）。
    virtual void ResetDailyMidnight();

    void IncreasePeerPlayCount(int serverId, unsigned int uniqueId);  // 0x8586208
    unsigned int GetPeerPlayCount(int serverId, unsigned int uniqueId) const;  // 0x8586184

    /*+0x08*/ std::map<CharacterInServer, PlayCountInfo> m_peerMap;
};  // 尺寸 0x20

// ---- CTeamFactory：StaticPool<600> 薄包装（对象尺寸 0x30） ----
class CTeamFactory : public StaticPool<COnlinePreliminaryTeam, 600>
{
public:
    CTeamFactory();                                  // ORIG W 0x8588e2e：转调基类 StaticPool ctor（定义在 GlobalData.cpp）
    ~CTeamFactory();                                 // ORIG W 0x8588e42
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
    COnlinePreliminaryTeam* FindTeam(int teamId);    // 0x858875e（__thiscall，查 m_teamMap）

private:
    CTeamFactory* m_factory;                          // +0x00（new nothrow 0x30）
    std::map<int, COnlinePreliminaryTeam*> m_teamMap; // +0x04
};  // 尺寸 0xc

// ---- COnlinePreliminary：预选赛对局单元（IMatch 派生） ----
class COnlinePreliminary : public IMatch
{
public:
    COnlinePreliminary();                             // 0x855fefe
    ~COnlinePreliminary();

    virtual int OnCreateRoom(CUser* user);        // ORIG 0x08560ae4：失败 0x13 / 成功 0（IMatch 基类 int 同步）
    virtual void OnDestroyRoom(CUser* user);
    virtual void OnJoinRoom(CUser* user);
    virtual void OnLeaveRoom(CUser* user, PvP_Room* room, int a, int b);
    virtual int OnRegisterMatch(CUser* user, PvP_Room* room);     // ORIG 返 1
    virtual int OnMatchRoom(PvP_Room* room, int a);               // ORIG 返 1
    virtual void OnStartFight();
    virtual void OnEndFight(PvP_Room* room);
    virtual int OnWalkOut(PvP_Room* room, int a);                 // ORIG 返 1
    virtual bool OnReady(CUser* user, bool ready);
    virtual void OnReward(PvP_Room* room, int a, bool b);
    virtual void OnCompleteLoad(CUser* user);
    virtual void OnConnectP2PTimeout(PvP_Room* room);
    virtual void OnConnectP2P(CUser* user, PvP_Room* room, char* addr, int port);
    virtual int OnGiveup(CUser* user);
    virtual int  CheckJoinable(PvP_Room* room, CUser* user);
    virtual int  CheckSeat(int a);
    virtual bool CheckMatchable(IMatch* other);
    virtual CUser* GetUser(unsigned int idx);
    virtual void GetUserList(void* out);
    virtual void RequestMap(short& mapNo, PVP_BATTLE_MODE& mode);
    virtual float GetGap(IMatch* other);   // 0x8561a0e

    float _GetRating(ENUM_SCHOOL_MATCH_TEAM team);   // ORIG 0x08560342：flds 返回 float
    // ORIG 0x08560082 尾 flds -0xc(%ebp)：st0 float 返回（int→float 修正，
    // mangled 无返回类型）
    float _GetRating(CUser* user);
    void _SetRating(ENUM_SCHOOL_MATCH_TEAM team, float v);
    void _SetRating(CUser* user, float v);
    int  _PutInRoom(CUser* user, PvP_Room* room, bool observer);
    void _PullOutRoom(CUser* user, PvP_Room* room, int seat);
    void _ReturnTeam(PvP_Room* room, int team, CUser* user);
    void _UpdateRating(int diff, ENUM_ELO_RATING_SYSTEM sys);
    int  _CheckObserver(PvP_Room* room, CUser* user);
    int  _CheckSameTeam(PvP_Room* room, CUser* user);
    int  _CheckMatchable(PvP_Room* room) const;
    int  _GetOppositeTeam(int team);
    void _IncreaseWinCount(ENUM_SCHOOL_MATCH_TEAM team);
    void _IncreaseWinCount(CUser* user);
    void _IncreaseLoseCount(ENUM_SCHOOL_MATCH_TEAM team);
    void _IncreaseLoseCount(CUser* user);
    void _RegisterWaitingRoom(PvP_Room* room);
    // ORIG T 0x085608da 无参静态语义（TeamInfoSecond::refresh 调用点实证
    // call 前不传 this）——声明为 static 以匹配 ABI；mangled 与非静态版相同。
    static unsigned int GetPlayCountPerOneTeam();                 // 0x0855fee8
    void _SendMatchStatePacket(int a, int b);
    void SetPlayCountPerOneTeam(unsigned int n);
    unsigned int _GetEachOtherPlayCount(CUser* a, CUser* b);
    unsigned int _GetEachOtherPlayCount();
    void _IncreaseEachOtherPlayCount(CUser* a, CUser* b);
    void _IncreaseEachOtherPlayCount();
    // ORIG 0x08560a42：0x8(%ebp) 即 user（无 this）→ static 实证
    static bool CheckUnusableEquipmentInUser(CUser* user);
    PvP_Room* _MakeRoom(CUser* user);             // ORIG 0x8560f92（返回池化房间）
    int  GetTeamId();
    static bool CheckUnusableEquipment(const Inven_Item* item);  // 迁自 CUserCharacInfo.h，0x08560920
    static void SetMatchAble(bool able);          // 0x085608da（写静态 bMatchable_）
    void _BackupRoomInfo(PvP_Room* room);         // 0x8560f74
    ENUM_SCHOOL_MATCH_TEAM _GetTeam(ENUM_SEAT_STATE state);  // 0x08568866：return state-1

private:
    // ORIG 0x0943e13c：delete 处仅 _ZdlPv（无 CLeagueMatchRoomListD1 调用，
    // 且 ORIG 符号表无 CLeagueMatchRoomList D1）⇒ 静态类型为 IRoomList*
    // （无虚析构），new 用完整类型构造、调用点全虚调用。
    static IRoomList* pWaitingRoomList_;   // 全局延迟单例
    static int nWaitingRoomListRefCount_;
    static bool bMatchable_;                      // ORIG B 0x0943e138
    static int  nextAuthValue_;                   // ORIG B→D 0x0943e144（OnCreateRoom 自增令牌）

public:  // 布局区（偏移契约；扩展仅限有报告证据的字段——由实现批次在此追加命名）
    /*+0x004*/ int m_field04;
    /*+0x008*/ PVP_ROOM_INFO m_roomInfo;
    // ==== 成员扩展区（有报告证据才追加；禁止改上方字段偏移） ====
    /*+0x018*/ int m_authValue;                   // OnCreateRoom 写入 nextAuthValue_++ / OnRegisterMatch 作 TimerStartMatching 第3参 / OnMatchRoom 校验键【推断：匹配授权号】
    /*+0x01c*/ TeamInfoSecond m_teams[2];         // +0x1c / +0x44
    /*+0x06c*/ int m_p2pReadyCount;               // OnConnectP2P ++ / OnMatchRoom 清零
};  // sizeof == 0x70（CMatchingSystem::GetMatchingSystem new $0x70 @0x85dfcf1 实证）

}  // namespace online_preliminary

// ---- 布局/偏移断言（ORIG 实证，勿动数值） ----
static_assert(sizeof(TeamInfoSecond) == 0x28, "TeamInfoSecond size drift");
static_assert(sizeof(online_preliminary::COnlinePreliminary) == 0x70, "COnlinePreliminary size drift");
#endif  // GAME_ONLINEPRELIMINARY_H_
