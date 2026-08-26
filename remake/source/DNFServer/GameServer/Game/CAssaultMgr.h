// df_game_r Game/ pvp_assault 域权威头（OnlyOrig_Assault 批：全域类声明归口）。
// 原 CGameManager.h 245-360 的 pvp_assault 段已整体迁入本头（唯一声明点）。
// 布局证据均为 ORIG 反汇编实证（地址随行注释）；静态点表取自 .rodata 内容。
#ifndef DNF_CASSAULTMGR_H
#define DNF_CASSAULTMGR_H

#include <map>
#include <vector>

#include "StaticPool.h"

class CUser;
class CParty;
class PacketGuard;
struct MapArea;
struct MSG_DIE_ASSAULT_PLAYER;

// ---- 全局作用域枚举（ORIG mangled `17ENUM_ASSAULT_TEAM` /
//      `30ENUM_POWER_WAR_WINNER_OR_LOSER`，均无 namespace 前缀）----
enum ENUM_ASSAULT_TEAM
{
    ENUM_ASSAULT_TEAM_0 = 0
};
enum ENUM_POWER_WAR_WINNER_OR_LOSER
{
    ENUM_POWER_WAR_WINNER_OR_LOSER_0 = 0
};
// ENUM_USERINFO / ENUM_POWER_SIDE_TYPE 权威定义见 GameEnums.h、CPowerManager.h。

namespace pvp_assault
{

// ORIG 枚举 tag（mangled NS_18ENUM_ASSAULT_STATEE）。
enum ENUM_ASSAULT_STATE
{
    ENUM_ASSAULT_STATE_0 = 0
};
class CForcedMoveCharacter
{
public:
    int m_field00;        // +0x00（CStaticDisposition::MoveUser 索引源）
    int m_slotIdx;        // +0x04（CExtraMatrix::MoveUser 索引源）
    CUser* m_user;        // +0x08
    int m_destVill;       // +0x0c
    int m_destArea;       // +0x10
    int m_x;              // +0x14（矩阵写入）
    int m_y;              // +0x18
    int m_dir;            // +0x1c

    void ForcedMove();    // ORIG T 0x82e567c（实现见 ForceMove TU）
};
class CAssaultPlace;

// =====================================================================
// Point2d —— ORIG Point2d::C1 0x82f046a（默认空）/C1Eii 0x82f0472。
// =====================================================================
class Point2d
{
public:
    Point2d() {}
    ~Point2d();
    Point2d(int x, int y) : m_x(x), m_y(y) {}

    // ORIG operator+= 0x82f04c0：分量相加，返回 *this
    Point2d& operator+=(const Point2d& o)
    {
        m_x += o.m_x;
        m_y += o.m_y;
        return *this;
    }
    // ORIG const operator- 0x82f0486：|dx| + |dy|（曼哈顿距离）
    int operator-(const Point2d& o) const;

    int m_x;
    int m_y;
};

// ORIG 只读数据表：
//   ASSAULT_RECT_PLAYER_POINT @0x8c1cce0（Point2d[8]：
//     (0,15) (56,5) (84,0) (28,10) (260,15) (204,5) (176,0) (232,10)）
//   ASSAULT_RECT_ICON_POINT   @0x8c1cd20 = {130, 5}
extern const Point2d ASSAULT_RECT_PLAYER_POINT[8];
extern const Point2d ASSAULT_RECT_ICON_POINT;

// =====================================================================
// PartyInfo —— ORIG PartyInfo::C1 0x82f05ee：清零单个 int；
// Get 0x82f060a 读该 int；Set(CParty*) 0x82f05fc 写指针值进该 int。
// =====================================================================
class PartyInfo
{
public:
    PartyInfo() : m_field00(0) {}

    unsigned int Get() { return m_field00; }               // ORIG 0x82f060a
    void Set(CParty* party);                               // ORIG 0x82f05fc

    unsigned int m_field00;   // 存放 CParty*（ORIG 按 int 位型写入）
};

// =====================================================================
// CandidatePoint —— ORIG C1 0x82f103c：只构造内嵌 Point2d(+4)，+0 不初始化；
// D1/D2 0x82f264a 空；operator< 0x82f04ea：return this->m_priority > o.m_priority。
// =====================================================================
class CandidatePoint
{
public:
    CandidatePoint();
    ~CandidatePoint();


    bool operator<(const CandidatePoint& o) const          // ORIG 0x82f04ea
    {
        return this->m_priority > o.m_priority;
    }

    int m_priority;      // +0x00（ctor 不初始化，由 _AddCandidatePoint 填写）
    Point2d m_point;     // +0x04
};

// =====================================================================
// CExtraMatrix —— ORIG C1 0x82f1088：vector<Point2d> 默认构造（成员 +0）。
// =====================================================================
class CExtraMatrix
{
public:
    CExtraMatrix();
    ~CExtraMatrix();


    void AddPoint(Point2d p);                              // ORIG 0x82e5962
    void MovePoint(const Point2d& delta);                  // ORIG 0x82e597c
    bool CheckPoint(std::vector<MapArea>* areas,
                    Point2d& out);                         // ORIG 0x82e5b4e
    bool MoveUser(CForcedMoveCharacter* fm);               // ORIG 0x82e5c0a

    std::vector<Point2d> m_points;   // +0x00
};

// =====================================================================
// CCandidateMatrix —— ORIG C1 0x82f1060 / D1 0x82f1074：
// 仅 vector<CandidatePoint>（+0）。At 恒返回 false。
// =====================================================================
class CCandidateMatrix
{
public:
    CCandidateMatrix();
    ~CCandidateMatrix();

    unsigned int At(unsigned int idx, int& x, int& y);          // ORIG 0x82e590c
    void MakeCandidatePoint(const Point2d& a, const Point2d& b);// ORIG 0x82e5ca8
    void _AddCandidatePoint(const Point2d& from1,
                            const Point2d& from2,
                            const Point2d& relTo, int weight);  // ORIG 0x82e5894
    void RemoveCandidatePoint(std::vector<MapArea>* areas);     // ORIG 0x82e5f36
    void ArrangeCandidatePoint();                               // ORIG 0x82e5fee

    std::vector<CandidatePoint> m_points;   // +0x00
};

// =====================================================================
// CDisposition —— 多态基类。
// ORIG 虚表 0x8c1d1c8 起：[SetVirtualMap, GetFlagPoint, MoveUser] 三槽全为
// __cxa_pure_virtual（0x08724fe0）。ORIG 纯虚基无 dtor 符号；此处补虚析构以
// 正确派生（派生类 ctor 覆盖 vptr 的调用面语义不变）。
// =====================================================================
class CDisposition
{
public:
    virtual ~CDisposition() {}

    virtual void SetVirtualMap(std::vector<MapArea>* areas) = 0;
    virtual void GetFlagPoint(int& x, int& y) = 0;
    virtual bool MoveUser(CForcedMoveCharacter* fm) = 0;
};

// =====================================================================
// CStaticDisposition —— 固定矩阵阵型。
// ORIG ctor 0x82e62aa：base → 本类 vptr(0x8c1d198) → ASSAULT_RECT_ICON_POINT
// 写入 +0xc/+0x10 → CExtraMatrix[2]@+0x14（步长 0xc；D1 0x82f10b0 反向析构）
// → 循环 ASSAULT_RECT_PLAYER_POINT[j*4+k]（k<=3 且 k<count[j*4]）AddPoint 到
// m_mats[j] → 以 users[0] 坐标 _MoveMatrix。
// =====================================================================
class CStaticDisposition : public CDisposition
{
public:
    CStaticDisposition(CUser** users, int* counts);   // ORIG 0x82e62aa
    ~CStaticDisposition();                             // ORIG 0x82f10b0

    void SetVirtualMap(std::vector<MapArea>* areas);   // ORIG 0x82e6588
    void GetFlagPoint(int& x, int& y);                 // ORIG 0x82f0526（读 +0xc/+0x10）
    bool MoveUser(CForcedMoveCharacter* fm);           // ORIG 0x82e65ee

    void _MoveMatrix(const Point2d& delta);            // ORIG 0x82e6498
    bool _CheckMatrix(std::vector<MapArea>* areas,
                      Point2d& out);                   // ORIG 0x82e64f2

private:
    char m_pad04[8];              // +0x04..+0x0b（ORIG 未触碰）
    Point2d m_iconPos;            // +0x0c（GetFlagPoint 输出源）
    CExtraMatrix m_mats[2];       // +0x14..+0x2c
};

// =====================================================================
// CDynamicDisposition —— 动态候选阵型。
// ORIG ctor 0x82e6032：base → vptr(0x8c1d1b0) → users[2]@+4 →
// anchor@+0xc（posX(u0)+(posX(u1)>>1)、同 y 规则）→
// CCandidateMatrix[2]@+0x14/+0x20（各 MakeCandidatePoint(u0Pos,u1Pos)）。
// =====================================================================
class CDynamicDisposition : public CDisposition
{
public:
    CDynamicDisposition(CUser** users, int* counts);   // ORIG 0x82e6032
    ~CDynamicDisposition() {}                          // ORIG 无独立 dtor 符号

    void SetVirtualMap(std::vector<MapArea>* areas);   // ORIG 0x82e624c（恒返回 true）
    void GetFlagPoint(int& x, int& y);                 // ORIG 0x82f050a（读 +0xc/+0x10）
    bool MoveUser(CForcedMoveCharacter*) { return true; } // ORIG 0x82e62a0：mov $1,%eax

private:
    CUser* m_users[2];              // +0x04
    Point2d m_anchor;               // +0x0c
    CCandidateMatrix m_matrices[2]; // +0x14、+0x20
};

// 6 参投影版（ORIG T 0x82e5a0b；定义 owner=CVillageMonsterMgr.cpp，
// CExtraMatrix::CheckPoint 等以 pvp_assault:: 名调用）
int IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas,
                              int x, int y, int& outX, int& outY,
                              int flag);   // ORIG 0x82e5a0b T：尾 mov $0,%eax → int 返回

}  // namespace pvp_assault

#endif  // DNF_CASSAULTMGR_H

// ---------------------------------------------------------------------------
// 第二段：CAssaulter / 后续扩展（CAssaultPlace/CAssaultMgr/Factory 续段）。
// ---------------------------------------------------------------------------
#ifndef DNF_CASSAULTMGR_H_PART2
#define DNF_CASSAULTMGR_H_PART2

namespace pvp_assault
{

// =====================================================================
// CAssaulter —— 单参战者槽位（CAssaultPlace 内 [8]，步长 0x20）。
// ORIG C1 0x82e661e 清 +0/+4/+8；Reset 0x82f05ba 清 +0/+4/+8/+0xc 与 +0x14 字节。
// =====================================================================
class CAssaulter
{
public:
    CAssaulter()
        : m_user(0), m_team(0), m_state(0), m_lastHpSec(0),
          m_hp(0), m_powerWarWinner(0)
    {
        for (int i = 0; i < (int)sizeof(m_connectIp); ++i)
            m_connectIp[i] = 0;
    }
    ~CAssaulter() {}                                  // ORIG D1/D2 0x82e6636 空

    CUser* GetUser();                                 // ORIG W 0x82f0542（读 +0）
    bool Empty();                                     // ORIG W 0x82f055c（+0==0）
    bool IsAttachAble();                              // ORIG W 0x82f054c
    ENUM_ASSAULT_TEAM GetTeam();                      // ORIG W 0x82f0570（读 +4）
    unsigned short GetUserUniqueId();                 // ORIG T 0x82e6662
    void SetState(ENUM_ASSAULT_STATE s);              // ORIG W 0x82f057c（写 +8）
    ENUM_ASSAULT_STATE GetState();                    // ORIG W 0x82f058a
    int GetHp();                                      // ORIG T 0x82e6688（读 +0x10）
    void AttachUser(CUser* user, ENUM_ASSAULT_TEAM team); // ORIG T 0x82e67e6
    void _PrepareAssault(CUser* user);                // ORIG T 0x82e6780
    bool IsPowerWarWinner();                          // ORIG W 0x82f05ae（+0x14!=0）
    void SetPowerWarWinner(bool flag);                // ORIG W 0x82f0596（写 +0x14 字节）
    void SetUserConnectP2P(char* ip, int len);        // ORIG T 0x82e6a82
    void Reset();                                     // ORIG W 0x82f05ba
    void DieUser();                                   // ORIG T 0x82e6a40
    bool ChangeHp(int hp);                            // ORIG T 0x82e6694
    void DetachUser();                                // ORIG T 0x82e6812
    void GiveupPanalty();                             // ORIG T 0x82e6868
    const char* GetCharacName() const;                // ORIG T 0x82e663c

    CUser* m_user;                  // +0x00
    unsigned int m_team;            // +0x04
    unsigned int m_state;           // +0x08
    unsigned int m_lastHpSec;       // +0x0c（ChangeHp 刷新）
    int m_hp;                       // +0x10
    char m_powerWarWinner;          // +0x14
    char m_connectIp[9];            // +0x15..+0x1d（SetUserConnectP2P memcpy ≤ min(n,8)）
    char m_pad1e[2];                // +0x1e..+0x1f（sizeof(CAssaulter)==0x20）
};

// =====================================================================
// CAssaultPlace —— 单场攻击区状态。
// =====================================================================

// ORIG C1 0x82f0614 / D1 0x82f13b6：
//   CAssaulter[8]@+0x000（步长 0x20）→ … → m_pos(Point2d)@+0x118 →
//   PartyInfo[2]@+0x12c（D1 反向清数组）。
// =====================================================================
class CAssaultPlace
{
public:
    CAssaultPlace();
    ~CAssaultPlace();

    CAssaulter m_assaulters[8];     // +0x000
    int m_field100;                 // +0x100（GetAssaulterCount 读）
    int m_field104;                 // +0x104（图标包 put_int）
    int m_authenValue;              // +0x108（GetAuthenValue 读）
    int m_state;                    // +0x10c（GetState）
    Point2d m_posBase;              // +0x110（ORIG ctor 调 Point2d::C1 于 this+0x110；
                                    //   Reset 以 Point2d(0,0) 写回；图标包 put_short 分量）
    Point2d m_pos;                  // +0x118（IsArea 比较 x/y；_SendPacketToArea 门槛）
    char m_teamFlag[2];             // +0x120..+0x121（_DelUser EndPvP flag 按 team 读）
    int m_winTeam;                  // +0x124（CloseAssault 获胜队比较）
    char m_pad125[7];               // +0x125..+0x12b
    PartyInfo m_parties[2];         // +0x12c
    char m_pad134[8];               // +0x134..+0x13b
    int m_abuseSec;                 // +0x13c（AbuseUser 节流时间戳）

    void Reset();                                   // 报告 Reset.md
    bool CheckPlayable();                           // ORIG W 0x82f06ea
    int GetState();                                 // ORIG W 0x82f06fe（读 +0x10c）
    bool IsArea(int vill, int area);                // ORIG W 0x82f06ae
    bool ExistUser(CUser* user);                    // ORIG T 0x82e8366
    bool GetTeam(CUser* user, ENUM_ASSAULT_TEAM& team);   // ORIG T 0x82e6cdc
    bool SendPacket(PacketGuard& p);                // ORIG T 0x82e6bf6
    bool SendPacket(PacketGuard& p, ENUM_ASSAULT_TEAM team); // ORIG T 0x82e6c56
    void MakeViewAssaultIconOnePlayer(PacketGuard* p, bool flag); // ORIG T 0x82e89c0
    void LeaveAssaultPlace(CUser* user, bool flag); // ORIG T 0x82eb450
    void LeaveAssaultPlace(int index, bool delFlag,
                           bool penaltyFlag);        // ORIG T 0x82ead6a
    static void SendUserState(CUser* user);         // ORIG T 0x82e83c4
    static void ForcedMoveAtPowerWarPlace(CUser* user); // ORIG T 0x082ed6a6

    bool _CheckPlayable();                          // ORIG T 0x82e9142
    void _GetTeamCount(int* counts);                // ORIG T 0x82e7232（STATE 版签名）
    int _GetUserIndex(CUser* user, bool strict);    // ORIG T 0x82e849a

    // ---- LeaveAssaultPlace 链私有 helper（既有还原保留）----
    bool _IsGiveupPanalty(bool flag);                     // ORIG T 0x82eacf4
    void _MakeUserState(PacketGuard* p);                  // ORIG T 0x82e8c5a（包 0x03 全场状态）
    void _SendUserState();                                // ORIG T 0x82e79f6
    void _MakeViewAssaultIconPlayer(PacketGuard* p, bool flag); // ORIG T 0x82e8afc
    void _SendPacketToArea(PacketGuard& packet);          // ORIG T 0x82e6d3a
    void _DelUser(int index, bool flag);                  // ORIG T 0x82e6e2c
    void CloseAssault();                                  // ORIG T 0x82e9ad6
    bool _IsBroadcastRunaway(CUser* user);                // ORIG T 0x82eab1a
    void _BroadcastRunaway(CUser* user, int index);       // ORIG T 0x82eab7a
    void _MakeLeaveUser(PacketGuard* p, int index);       // ORIG T 0x82e9046
    void _MakeCloseAssault(PacketGuard* p, int tick);     // ORIG T 0x82e88d6
    void _MakeUserState(PacketGuard* p, CUser* user);     // ORIG T 0x82e8d60
    int  _SendCloseAssault(CUser* user, int tick);        // ORIG T 0x82e7b2a

    void TimeoutConnectP2P();
    void TimeoutEndAssault();
    void TimeoutCloseAssault();
    void TimeoutCompleteLoad();
    // ---- 访问器（worklist 符号）----
    int GetAuthenValue();                           // ORIG 0x82f06a0（读 +0x108）
    int GetAssaultPlaceId();                        // ORIG 0x82f0680（读 +0x104）
    void SetAssaultPlaceId(unsigned int id);        // ORIG 0x82f068e（写 +0x104）
    int GetAssaulterCount();                        // ORIG 0x82f06dc（读 +0x100）
    const char* GetAnotherTeamCharacName(int idx);  // ORIG 0x82eb3ba
    void AbuseUser(CUser* user);                    // ORIG T 0x82eb5e2

};
// =====================================================================
// CAssaultMgr —— 攻击区管理器（保持既有布局与 API）。
// ORIG ctor 0x8235084：map 默认构造 + 首字节清零；
//   +0x00 bool（SetAssaultChannel 写入）+0x08 std::map<uint, CAssaultPlace*>。
// =====================================================================
class CAssaultMgr
{
public:
    CAssaultMgr() : m_bAssaultChannel(false) {}

    void OnMoveArea(CUser* user);                        // ORIG 0x82ef610
    void OnLeaveAssaultPlace(CUser* user, bool flag);    // ORIG 0x82ef176
    void SendPacket(CUser* user, bool flag, PacketGuard* packet);
                                                         // ORIG 0x82efb54
    CAssaultPlace* _GetAssaultPlace(unsigned int id);    // ORIG 0x82ed9fe
    void SetAssaultChannel(bool flag);                   // ORIG W SetAssaultChannelEb
    void ForcedMoveAtPowerWarPlace(CUser* user);         // ORIG T 0x082ed672
    static void NotifyGhostTime(CUser* user, int a, int ghostTime);
                                                         // ORIG T 0x082eff42
    void OnTimeoutEndAssault(unsigned int a, int b);               // ORIG 0x82efaa8
    void OnTimeoutCloseAssault(unsigned int a, int b);             // ORIG 0x82efafe
    void OnTimeoutConnectP2P(unsigned int a, int b);               // ORIG 0x82efa52
    void OnTimeoutCompleteLoad(unsigned int a, int b);             // ORIG 0x82ef818
    void OnTimeoutConnectP2PAfterAssault(unsigned int a, int b);   // ORIG 0x82ef9d8
    void OnTimeoutCompleteLoadAfterAssault(unsigned int a, int b); // ORIG 0x82ef86e
    void FreeAssaultPlace(int id, CAssaultPlace* place);          // ORIG T 0x82edb6a
                                                                  // （map 摘除 + Reset + 归还池）

    bool m_bAssaultChannel;                              // +0x00
    char m_pad01[7];                                     // +0x01..+0x07
    std::map<unsigned int, CAssaultPlace*> m_places;     // +0x08
};
// =====================================================================
// CAssaultPlaceFactory —— StaticPool 包装。
// ORIG C1 0x82f0f96 / D1 0x82f0f82：成员即 StaticPool<CAssaultPlace,128>；
// Acquire 0x82f0f54 / Free 0x82f0f68 直接转发池方法。
// =====================================================================
class CAssaultPlaceFactory
{
public:
    CAssaultPlaceFactory();
    ~CAssaultPlaceFactory();

    CAssaultPlace* AcquireAssaultPlace();                 // ORIG 0x82f0f54
    void FreeAssaultPlace(CAssaultPlace* place);          // ORIG 0x82f0f68

    StaticPool<CAssaultPlace, 128> m_pool;
};

CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory();  // ORIG T 0x82f0fa9（guard 单例）


CAssaultMgr* GetInstanceAssaultMgr();                    // guard 单例

}  // namespace pvp_assault

#endif  // DNF_CASSAULTMGR_H_PART2


