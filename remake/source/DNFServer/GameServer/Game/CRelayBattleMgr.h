#ifndef GAME_RELAY_BATTLE_MGR_H_
#define GAME_RELAY_BATTLE_MGR_H_

// ============================================================================
// df_game_r CRelayBattleMgr（G2-4 PvP 链，接力战管理器，PvP_Room +0x620）
// 尺寸 0x7c，布局依据 ORIG 各方法位移访问（见
// docs/class_func_reports/CRelayBattleMgr.md）推导：
//   +0x000 CRelayBattle m_battles[8]（0xc * 8 = 0x60）
//     CRelayBattle: +0 int m_state | +4 short m_seq | +6 char m_team
//                   +7 char m_requestFight | +8 char m_killCount | +9 pad[3]
//   +0x060 int m_changeSeqCount[3]（下标 1..2 使用，+0x64/+0x68）
//   +0x06c PvP_Room* m_pRoom        +0x070 int m_lastFightStartTime
//   +0x074 int m_state              +0x078 int m_readyCount
// 方法签名以 ORIG mangled 名为准。
// 注意：PvP_deps.h 当前仍含本类最小桩声明；主 agent 集成时需以本头替换，
// 避免与 PvP_Room.h 同 TU 重复定义。
// ============================================================================

#include <vector>

class PvP_Room;

// ---- CRelayBattle（0xc，接力战单槽子对象） ----
class CRelayBattle
{
public:
    void Reset();

    int m_state;          // +0x00
    short m_seq;          // +0x04
    char m_team;          // +0x06
    char m_requestFight;  // +0x07
    char m_killCount;     // +0x08
    char m_pad9[3];       // +0x09
};

class CRelayBattleMgr
{
public:
    // ---- 生命周期 / 房间钩子（ORIG 0x085de0cc 起） ----
    void Reset();
    void SetParent(PvP_Room* room);
    void OnCreateRoom(int a);
    void OnJoinRoom(int a);
    void OnLeaveRoom(int a);
    void OnDiePlayer(int victimSeat, int killerSeat, bool bTime);
    void OnChangeTeam(int seat, int team);
    void OnReady(int seat, bool ready);
    void OnRequestFight(int seat);
    void OnStart();
    void OnFight();

    // ---- 访问器 ----
    int GetKillCount(int seat);
    int GetTimerKey();
    int GetChangeSequenceCount(int team);
    int GetAllKillUserIndex(int* out);

    // ---- 顺序编排（ORIG 0x085de474 起） ----
    void IncreaseChangeSequenceCount(int team);
    void DecreaseChangeSequenceCount(int team);
    void ResetChangeSequenceCount();
    void CreateRandomSequenceVector(std::vector<int>& seq, int count);
    void ArrangeChangeSequence(std::vector<int>& seq, int team, int& outIdx);
    void ArrangeNoChangeSequence(std::vector<int>& seq, int team, int& outIdx);
    void ArrangeMatchSequence();
    int GetNextPlayer(int team);
    int SendRequestFight(int seat);
    int TurnPlayer();

private:
    CRelayBattle m_battles[8];   // +0x000（0x60）
    int m_changeSeqCount[3];     // +0x060
    PvP_Room* m_pRoom;           // +0x06c
    int m_lastFightStartTime;    // +0x070
    int m_state;                 // +0x074
    int m_readyCount;            // +0x078
};

#endif  // GAME_RELAY_BATTLE_MGR_H_
