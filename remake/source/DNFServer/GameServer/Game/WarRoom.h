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

#include "WarField.h"   // 权威 WarField（+0x34 值成员 m_field 需完整类型；PvPTypes.h 亦引入 WarField.h）
#include "PvPTypes.h"   // PvP 子对象权威声明（原 PvP_deps.h 已并入；约束2 删除 *_deps.h）
#include "CParty.h"     // ENUM_WALKOUT_TYPE 权威定义（原经 PvPTypes→CParty 传递获得，
                        // 该边已断开以修复 CUser.h 重入环）
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
    // ORIG W weak（0x082a5a56）：in-class 定义产生 weak 且调用方保守 outgoing 帧 0x18
    void SetState(WARROOM_STATE state) { m_state = state; }
    void CheckState();
    void ChangeState(WARROOM_STATE state);
    int GetLiveCount() const;
    int GetRemainSeat() const;
    int GetWaiterCount() const;
    int GetUserSlot(CUser* user);   // ORIG CGameManager 域调用（席位查找，未命中 -1）
    // ORIG CGameManager::WarRoomAlloc 0x8298a81/+0x05b 分配时直写 +0x00/+0x01
    //（ORIG 无 setter；内联实现不产生新符号）。+0x00 暂存 war area 序号
    // （IsJoinable/Reset 按 2..6 等级段解释），+0x01 为区内分配序号。
    void setLevelBand(char band) { m_levelBand = band; }
    void setRoomSeq(char seq) { m_roomSeq = seq; }
    int IsJoinable(CUser* user);
    // ORIG 0x86bad7c：调用点 `test %al,%al` 单字节判定 ⇒ bool 返回（InterB 实证口径）。
    bool IsBanUser(CUser* user);
    void CheckFatuigue();
    void CheckHostChange();
    void InvalidUserCheck();
    // ORIG W weak（0x086c0b24）：in-class 定义→调用方保守 outgoing 帧
    // [b28d-Main·检查点14] S415 P211 同型律：返回类型 unsigned int→int
    //   （ORIG SendWarRoomFailMsg 0x086c078e `call; mov %eax,%ebx` + `mov 0x4(%eax),%esi`
    //   ⇒ int 返回时 key→%ebx/idx→%esi；探针 /tmp/wr_probe3.cpp r1 vs r2/r5 实证）。
    //   返回类型不入 mangled；sizeof/偏移/体零变化；消费点全在本 TU。
    //   （S415 三态复测：代落态 ae=240/near=1/diff=13 vs 回退态 238/2/14 ⇒ 确为必要。）
    int GenTimerKey(TIMER_MESSAGE msg) { return ++m_timerKey[msg - 0x3f]; }
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
    bool HandleMonsterKill(CUser* user, int seat, CUser* killer);   // [b28d-Main·P292] int→bool（ORIG 调用点 0x081ecbfe `call; xor $0x1,%eax`）
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

public:
    // +0x00/+0x01 由 CGameManager::WarRoomAlloc(0x8298a81/+0x05b) 分配时直写：
    // +0x00 暂存 war area 序号（IsJoinable/Reset 按 2..6 等级段解释），
    // +0x01 为区内分配序号（MakeRoomInfo 随等级段广播）。ORIG 无 setter，
    // 故仅调整访问段（Itanium ABI 按声明序布局，无布局/新符号影响）。
    char m_levelBand;                   // +0x000
    char m_roomSeq;                     // +0x001（WarRoomAlloc 分配计数、Reset 清零）

private:
    char m_pad2[2];                     // +0x002
    int m_index;                        // +0x004
    char m_field8;                      // +0x008
    char m_pad9[3];                     // +0x009
    int m_curSpawnStep;                 // +0x00c
    char m_flag10;                      // +0x010
    char m_flag11;                      // +0x011
    char m_flag12;                      // +0x012
    char m_pad13;                       // +0x013
    // [b28d-Main·检查点14] S415 P211 同型律：int→unsigned long（i386 同 4 字节，
    //   偏移 +0x14/sizeof/mangled 零变化）。ORIG HostChangeWarRoom 0x086c08dd
    //   `lea 0x4(%edx),%ecx` + `mov 0x14(%ebp),%edx`；探针 /tmp/wr_probe7.cpp
    //   v7(unsigned long) 出 ORIG lea 形，v1/v2/v3(int) 与视图 cast 六形恒 add $0x4。
    unsigned long m_hostVotes[6];       // +0x14（HostChangeWarRoom 直写低 32 位投票值，
                                        //      -1=未投；CheckHostChange 取最小非 -1 槽广播）
    int m_userCount;                    // +0x02c
    char m_field30;                     // +0x030
    char m_pad31[3];                    // +0x031
    WarField m_field;                   // +0x034（0xf8）
    // b28d-solo-WarRoom [P235/P301]：匿名 union 双视角。void* 视角保留既有语义；
    // CUser* 视角消除调用点 `(CUser*)m_slots[i]` cast 造成的
    // `mov mem,%eax; mov %eax,%ebx` 二段装载（ORIG 各调用点均为单段直落
    // callee-saved）。两视角同为 6×4B @+0x12c ⇒ sizeof/全部偏移/mangled 零影响。
    union
    {
        void* m_slots[6];               // +0x12c（ORIG Create 0x086bac7b this-先行直存指针=void* 元素探针实证）
        CUser* m_slotsUser[6];          // +0x12c 同址视角（调用点直用，免 cast 二段）
    };
    WARROOM_STATE m_state;              // +0x144（ORIG SetState 0x082a5a56 this-先行装载=成员与形参同类型探针实证）
    int m_slotState[6];                 // +0x148
    int m_timerKey[9];                  // +0x160（GenTimerKey 数组，msg 0x40..0x47 → 下标 1..8）
    bool m_liveFlag[6];                 // +0x184（槽存活标志：非 0=在场且存活。
                                        // b28BM13：char→bool 同型化（SetCharacterLive
                                        // bool 实参）翻 ORIG store 装载序，探针 bm13_p1）
    char m_pad18a[2];                   // +0x18a（ORIG Reset 不触及的尾随 2 字节）
    int m_slotData[6];                  // +0x18c
    int m_slotFp[6];                    // +0x1a4（CalcGuildFP 公会 FP 均摊工作区；
                                        //      ORIG BattleReset 初始化 -1）
    float m_roleKillCnt[4];             // +0x1bc（按 monster roleType 累计击杀权重，
                                        //      ClearReward 经验加权；BattleReset 清零）
    int m_fpBase;                       // +0x1cc
    CDungeon* m_pDungeon;               // +0x1d0
    int m_fp;                           // +0x1d4
    std::map<unsigned int, int> m_map;  // +0x1d8（0x18）
};

#endif  // GAME_WARROOM_H_
