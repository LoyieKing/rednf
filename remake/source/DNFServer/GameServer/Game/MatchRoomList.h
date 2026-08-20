#ifndef GAME_MATCHROOMLIST_H_
#define GAME_MATCHROOMLIST_H_

// ============================================================================
// df_game_r 还原 —— 匹配房间列表（MatchingSystem 链）
// 依据 docs/class_func_reports/{CWaitingRoomList,CLeagueMatchRoomList,
// CMultiMatchRoomList}.md 与 ORIG 汇编（df_game_r 0x8566d9c..0x8567fe7、
// 0x8568fa2/0x8568fb0 构造、0x8ca3bb0/0x8ca3bc8/0x8ca3dc8/0x8ca3de8 虚表）。
// 类布局（ORIG 构造/析构推导）：
//   IMatch               匹配单元接口，虚表 36 槽（0x8ca3de8）；本类中与
//                        房间列表交互的槽位：+0x0c GetGap / +0x78
//                        CheckMatchable / +0x84 GetPlayerCount / +0x88
//                        GetNextMatch / +0x8c SetNextMatch / +0x10 GetUser。
//                        默认实现与 ORIG weak 符号（W）一致，后续具体匹配
//                        类（CLeagueMatch/fair_pvp::CFairMatch 等）覆盖。
//   IRoomList            抽象基类，3 个纯虚（AddRoom/DelRoom/Match），
//                        虚表 3 槽（0x8ca3dc8），无虚析构。
//   CWaitingRoomList     等待房间表：std::set<IMatch*>（+0x00）
//   CLeagueMatchRoomList 联赛房间表：IRoomList + std::map<IMatch*,int>（+0x04）
//                        （尺寸 0x1c，见 CLeagueMatch 单例 new 0x1c）
//   CMultiMatchRoomList  多队房间表：IRoomList + std::map<int,CWaitingRoomList>
//                        （+0x04）；组合拆分配置见 GetWaitingRoomCombination。
// 跨类符号：
//   fair_pvp::CFairMatch::GetPlayCountPerOneTeam（0x08562834，恒返 1，
//   本 TU 提供定义，后续 fair_pvp 批次可接管）
// ============================================================================

#include <map>
#include <set>
#include <vector>

#include "PvPTypes.h"   // PVP_BATTLE_MODE / PVP_ROOM_INFO

class CUser;
class PvP_Room;

// ---- IMatch（匹配单元接口，虚表 36 槽；默认实现对应 ORIG weak 符号） ----
class IMatch
{
public:
    virtual ~IMatch() {}
    virtual int GetIndentification() { return 0; }              // +0x08
    virtual float GetGap(IMatch* other) { return 0.0f; }        // +0x0c
    virtual CUser* GetUser(unsigned int idx) { return 0; }      // +0x10
    virtual void GetUserList(void* out) { (void)out; }          // +0x14
    virtual void RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)
    { (void)mapNo; (void)mode; }                                // +0x18
    virtual int GetExtraWinPoint(int a) { return 0; }           // +0x1c
    virtual void OnCreateRoom(CUser* user) { (void)user; }      // +0x20
    virtual void OnDestroyRoom(CUser* user) { (void)user; }     // +0x24
    virtual void OnJoinRoom(CUser* user) { (void)user; }        // +0x28
    virtual void OnLeaveRoom(CUser* user, PvP_Room* room,
                             int a, int b) { (void)user; (void)room; (void)a; (void)b; }  // +0x2c
    virtual void OnRegisterMatch(CUser* user, PvP_Room* room)
    { (void)user; (void)room; }                                 // +0x30
    virtual void OnUnregisterMatch(CUser* user, PvP_Room* room)
    { (void)user; (void)room; }                                 // +0x34
    virtual void OnMatchRoom(PvP_Room* room, int a)
    { (void)room; (void)a; }                                    // +0x38
    virtual void OnStartFight() {}                              // +0x3c
    virtual void OnEndFight(PvP_Room* room) { (void)room; }     // +0x40
    virtual void OnWalkOut(PvP_Room* room, int a)
    { (void)room; (void)a; }                                    // +0x44
    virtual void OnReady(CUser* user, bool ready)
    { (void)user; (void)ready; }                                // +0x48
    virtual void OnReward(PvP_Room* room, int a, bool b)
    { (void)room; (void)a; (void)b; }                           // +0x4c
    virtual void OnEndPvP(PvP_Room* room) { (void)room; }       // +0x50
    virtual void OnCompleteLoad(CUser* user) { (void)user; }    // +0x54
    virtual void OnConnectP2PTimeout(PvP_Room* room)
    { (void)room; }                                             // +0x58
    virtual void OnConnectP2P(CUser* user, PvP_Room* room,
                              char* addr, int port)
    { (void)user; (void)room; (void)addr; (void)port; }         // +0x5c
    virtual void OnGiveup(CUser* user) { (void)user; }          // +0x60
    virtual void OnRematchTimeout(PvP_Room* room) { (void)room; }  // +0x64
    virtual void OnAskRematch(CUser* user, int a)
    { (void)user; (void)a; }                                    // +0x68
    virtual void OnWaitRematch(CUser* user, PvP_Room* room)
    { (void)user; (void)room; }                                 // +0x6c
    virtual int CheckJoinable(PvP_Room* room, CUser* user)
    { (void)room; (void)user; return 0; }                       // +0x70
    virtual int CheckSeat(int a) { (void)a; return 1; }         // +0x74
    virtual bool CheckMatchable(IMatch* other)
    { (void)other; return true; }                               // +0x78
    virtual int GetEndPvPResultTimeout() { return 10000; }      // +0x7c
    virtual void OnEndPvPResult(PvP_Room* room, CUser* user)
    { (void)room; (void)user; }                                 // +0x80
    virtual int GetPlayerCount() { return 0; }                  // +0x84
    virtual IMatch* GetNextMatch() { return 0; }                // +0x88
    virtual void SetNextMatch(IMatch* next) { (void)next; }     // +0x8c
};

// ---- IRoomList（房间列表抽象基类，虚表 3 槽，无虚析构） ----
class IRoomList
{
public:
    virtual bool AddRoom(IMatch* room) = 0;
    virtual bool DelRoom(IMatch* room) = 0;
    virtual bool Match(IMatch* me, IMatch** out) = 0;
};

// ---- CWaitingRoomList（等待房间表，std::set<IMatch*> at +0x00） ----
// ORIG：_ZN16CWaitingRoomListC1Ev 0x0856904e / D1 0x08569062
//       find 0x08568872 / erase 0x085688be / insert 0x085688d8
//       doMatch 0x08566d9c（报告 CWaitingRoomList.md）
class CWaitingRoomList
{
public:
    CWaitingRoomList();
    ~CWaitingRoomList();

    bool find(IMatch* room);
    void erase(IMatch* room);
    bool insert(IMatch* room);
    // 收集与 me 可匹配（CheckMatchable）的候选：权重 GetGap 升序入 out；
    // 列表空返 0，收集后 out 非空返 1。
    int doMatch(IMatch* me, std::multimap<float, IMatch*>& out);

private:
    std::set<IMatch*> m_roomSet;   // +0x00
};

// ---- CLeagueMatchRoomList（联赛房间表，IRoomList + map<IMatch*,int> at +0x04） ----
// ORIG：C1 0x08568fb0；AddRoom 0x08567e9e / DelRoom 0x08567f72 /
//       Match 0x08567d00（报告 CLeagueMatchRoomList.md）。
class CLeagueMatchRoomList : public IRoomList
{
public:
    CLeagueMatchRoomList();

    virtual bool AddRoom(IMatch* room);
    virtual bool DelRoom(IMatch* room);
    virtual bool Match(IMatch* me, IMatch** out);

private:
    std::map<IMatch*, int> m_roomMap;   // +0x04（key=房间，value=加入计数）
};

// ---- CMultiMatchRoomList（多队房间表，IRoomList + map<int,CWaitingRoomList>） ----
// ORIG：C1 0x08566efc；AddRoom 0x08567954 / DelRoom 0x08567bba /
//       Match 0x085675d0 / GetWaitingRoomCombination 0x0856703c
//       （报告 CMultiMatchRoomList.md）。组合表 CombinationTable
//       （0x8ca3200，[7][10][14]）数据自 ORIG 导出。
class CMultiMatchRoomList : public IRoomList
{
public:
    CMultiMatchRoomList();

    virtual bool AddRoom(IMatch* room);
    virtual bool DelRoom(IMatch* room);
    virtual bool Match(IMatch* me, IMatch** out);

    // 按 playCount 的组合拆分（CombinationTable）收集候选队伍，
    // 选平均权重最小且可凑满 playCount 人的方案，append 到 out；
    // 无可用组合返 0，否则返 1。
    int GetWaitingRoomCombination(IMatch* me, int playCount,
                                  std::vector<IMatch*>& out);

private:
    std::map<int, CWaitingRoomList> m_roomMap;   // +0x04（key=每队人数）
};

// ---- 自由函数 ----
// 取匹配单元 4 个槽位（0..3）的账号名，sprintf 为 "%s,%s,%s,%s" 到 out。
// ORIG：GetUserName(IMatch*, char*) 0x08563ff7。
char* GetUserName(IMatch* match, char* out);

namespace fair_pvp
{
class CFairMatch
{
public:
    static int GetPlayCountPerOneTeam();   // 0x08562834，恒返 1
};
}

#endif  // GAME_MATCHROOMLIST_H_
