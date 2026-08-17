#ifndef GAME_WARROOM_H_
#define GAME_WARROOM_H_

// ============================================================================
// df_game_r WarRoom（G2-3 PvP 链，战场房间，池化 40）
// 布局依据 ORIG 构造（0x086ba75c）/析构（0x086ba7e6）/Reset（0x086baa14）推导：
//   +0x000 char m_levelBand（2..6 等级段） +0x001 char
//   +0x004 int m_index          +0x008 char
//   +0x00c int m_curSpawnStep   +0x010 char x3
//   +0x02c int m_userCount      +0x030 char
//   +0x034 WarField(0xf8)       +0x12c int m_slots[6]（CUser* 槽）
//   +0x144 int m_state          +0x148 int m_slotState[6]
//   +0x18c int m_slotData[6]    +0x1cc int m_fpBase
//   +0x1d0 CDungeon* m_pDungeon +0x1d4 int m_fp
//   +0x1d8 std::map<uint,int>   （总尺寸 0x1f0）
// 方法签名以 ORIG mangled 名为准（/tmp/warroom_syms.txt 与报告）。
// ============================================================================

#include <map>

#include "PvP_deps.h"
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"

class CDungeon;

class WarRoom
{
public:
    WarRoom();
    ~WarRoom();

    // ---- 基础访问器 ----
    int GetIndex();
    void SetIndex(int idx);
    int GetState();
    void SetState(WARROOM_STATE state);
    void CheckState();
    void ChangeState(WARROOM_STATE state);
    int GetLiveCount() const;
    int GetRemainSeat() const;
    int GetWaiterCount() const;
    int GetUserSlot(CUser* user);
    int IsJoinable(CUser* user);
    int IsBanUser(CUser* user);
    void CheckFatuigue();
    void CheckHostChange();
    void InvalidUserCheck();
    unsigned int GenTimerKey(TIMER_MESSAGE msg);
    bool CheckTimerKey(TIMER_MESSAGE msg, int key);

    // ---- 房间流程 ----
    int Create(CUser* user, char* name, short idx);
    int Join(CUser* user, int& seat);
    void Open();
    void Close();
    void Prepare();
    void Start();
    int Destroy(int idx);
    int ShutDown();
    void Reset();
    void BattleReset();
    void ReviveAll();
    int ReviveUserByCoin(CUser* user, unsigned short coin);
    void SetCharacterLive(int seat, CUser* user, bool a, bool b, short c, int d);
    void CurSpawnStepReset();
    int WalkOutUser(CUser* user);
    int WalkOutUserBySlot(int seat);
    int SendWalkOut(CUser* user, ENUM_WALKOUT_TYPE type);
    void HandleSetSeatState(CUser* user, int seat, ENUM_SEAT_STATE state);
    int SetSlotState(CUser* user, int seat, ENUM_SEAT_STATE state,
                     CUser** out, bool& a, bool& b, bool& c);
    int HandleDieCharacer(CUser* user, int seat);
    int HandleMonsterKill(CUser* user, int seat, CUser* killer);
    int HandleGetItem(CUser* user, int idx);
    int HandleItemDrop(CUser* user, int a, int b, char c, int d, int e);
    int HandleWpPerMonster(CUser* user, int a, int b, CUser* c);
    void HandleTimerTimeBomb();
    void HandleTimerResultCount();
    void SkipLastRankResultTimer();
    int HostChangeWarRoom(CUser* user, char a, unsigned long b);
    void SendGuildFP();
    void CalcGuildFP();
    void WarRoomFail();
    void SendWarRoomFailMsg();
    void SendStatePacket();
    void AddBanList(CUser* user);
    void UpdateBanList();
    void RemoveKC();
    void ClearReward();
    void CancelAllTimer();
    void MakeRoomInfo(char* buf);
    void MakeSlotInfo(char* buf, int seat);
    void MakeStateInfo(char* buf);
    void SendToRoom(PacketGuard& guard);

    // ---- 状态机钩子（ORIG _ZN7WarRoom12OnEnterStateEv /
    //      _ZN7WarRoom12OnLeaveStateE13WARROOM_STATE，ChangeState 调用） ----
    void OnEnterState();
    void OnLeaveState(WARROOM_STATE state);

private:
    char m_levelBand;                   // +0x000
    char m_field1;                      // +0x001
    char m_pad2[2];                     // +0x002
    int m_index;                        // +0x004
    char m_field8;                      // +0x008
    char m_pad9[3];                     // +0x009
    int m_curSpawnStep;                 // +0x00c
    char m_flag10;                      // +0x010
    char m_flag11;                      // +0x011
    char m_flag12;                      // +0x012
    char m_pad13;                       // +0x013
    char m_pad14[0x18];                 // +0x014
    int m_userCount;                    // +0x02c
    char m_field30;                     // +0x030
    char m_pad31[3];                    // +0x031
    WarField m_field;                   // +0x034（0xf8）
    int m_slots[6];                     // +0x12c
    int m_state;                        // +0x144
    int m_slotState[6];                 // +0x148
    int m_timerKey[9];                  // +0x160（GenTimerKey 数组，msg 0x40..0x47 → 下标 1..8）
    char m_pad184[8];                   // +0x184（0x184..0x18b，含 6 个存活标志位）
    int m_slotData[6];                  // +0x18c
    char m_pad1a4[0x28];                // +0x1a4
    int m_fpBase;                       // +0x1cc
    CDungeon* m_pDungeon;               // +0x1d0
    int m_fp;                           // +0x1d4
    std::map<unsigned int, int> m_map;  // +0x1d8（0x18）
};

#endif  // GAME_WARROOM_H_
