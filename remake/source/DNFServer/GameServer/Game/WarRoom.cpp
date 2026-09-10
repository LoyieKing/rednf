#include "LogManager.h"
// df_game_r WarRoom（G2-3 PvP 链，战场房间，池化 40）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/WarRoom.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 跨类调用一律 include 权威头 + 真实类型调用（无 extern "C" asm 桥）。
// ---------------------------------------------------------------------------
// [b28d-solo-WarRoom 单文件泳道判词 · 2026-09-09]（证据均含 ORIG 地址/指令形）
//   identical_ae：SendWarRoomFailMsg 0x086c078e（GenTimerKey→%ebx/m_index→%esi，
//     旋钮=WarRoom.h GenTimerKey 返回类型同型化 int）、HostChangeWarRoom
//     0x086c08a6（lea 0x4(%edx),%ecx，旋钮=m_hostVotes 元素同型化 unsigned long）、
//     MakeStateInfo 0x086be07a / MakeSlotInfo 0x086bdea0 / MakeRoomInfo 0x086bdd58
//     （局部 char*+调用点 cast 复现值先物化 %edx；P222 极性翻正）、
//     ReviveUserByCoin 0x086bf324（地址归一后全等）。
//   near：SendGuildFP 0x086bc0b0（[SweepWarRoom] 本轮三循环岛形修复 271=271，
//     gap 0，DIFF→NEAR；残余标量槽序 N82）、EnterWarRoom（N82附 聚合优先低槽）。
//   [b28d-solo-WarRoom-3 · 2026-09-10] 判据①全 TU jcc/jmp 目标下标逐位比对 +
//   gap' 降序处理，本轮读数（orig→our，差=|gap|）：
//     OnEnterState   515→515（gap 0，原 528/-13）
//     ReviveUserByCoin 98→98（gap 0，P223 尾标签形复现 086bf3da nop 中继岛）
//     HandleItemDrop 776→777（gap -1，原 781/-5；P325 共享 case 块）
//     HandleGetItem  547→546（gap +1，原 551/-4；删 result 局部改早返回 +
//       count 具名化 + randomOption 内联短路）
//     HandleMonsterKill 445→442（gap 3，残余 4.4 lea/内存直加强度归约 N86/N1）
//     CalcGuildFP 165→168（gap -3，N82/N98：两数组首次使用序 + 6×movb vs
//       movl+movw 合并不可择）
//     ImportWarRoomWPTable 128→126（gap 2，P283 双入口岛 4.4 恒 jump-thread）
//     ClearReward 565→565 / SendGuildFP 271→271 / EnterWarRoom 279→279（gap 0，
//       残余版本槽基/寄存器 N82/N95/N126）
//   本轮修出的真 bug（判据⑩㊿/②/①）：
//     · OnEnterState 086bbb6a/086bbbcd 字段偏移：m_ec/m_f4 → m_f4/m_fc
//       （m_warRoomData+0xf4/+0xfc；原重建读错字段）；
//     · OnEnterState 086bbce7 `setle` 边界：`penalty < fp` → `fp <= penalty`
//       + else 直落（P194/P59 镜像；原写在 fp==penalty 时行为相反）；
//     · HandleGetItem 086bee7b..086bee9d：具名 bool randomOption 多占槽 ⇒
//       内联 `IsRandomOption() && !getCheckPickUpRandomOptionItem()`。
//   残余全部为跨版本结构性 N（已在函数头逐一记证），无未判定真缺口。
//   P0：全域无 register asm 绑定（唯一 register 为 P231 纯存储类提示）。
// ---------------------------------------------------------------------------
// [b28d-solo-WarRoom-4 · 2026-09-10] 四数 114/243/2/9 → **114/245/0/9**
//   （near 清零；ae +2）。本轮修为（判据⑩/⑪ + P231 + P223）：
//     · SendGuildFP 0x086bc0b0      NEAR → **AE（271=271）**：三标量（i1/minFp/count）
//       全头部声明并按 ORIG 槽深序（i1@-0x20/minFp@-0x1c/count@-0x18）落序；
//       `int minFp=…;/int count=0;` 的声明位置改独立赋值语句（保 ORIG 指令序）。
//     · EnterWarRoom 0x08201e7a      NEAR → **AE（279=279）**：声明序 packet, ret,
//       party, **seat**（seat 置队尾）+ 探针 /tmp/wr4/p.cpp f1..f5 五形对照；
//       `seat = 0;` 保留在 Join 前（086be6xx 形）。
//     · CalcGuildFP 0x086bbea6       168/-3 → **164/+1**：①两数组声明序交换
//       （local_40[6]={0} 先、local_4c[12] 后）= ORIG -0x48/-0x3c 槽序；②商
//       `local_20/local_1c` 复用 local_18（ORIG 086bc043/086bc075），消重复 idiv。
//     · ImportWarRoomWPTable 0x08902543  126/2 → **128/0**：①`register int again`
//       + `again != 0`（消 4.4 setne 物化，出 ORIG `test %ebx,%ebx`）；②do 体后与
//       else 各发 goto（LOOP_TAIL/ELSE_ISLAND 物理相邻）复现双落点。
//   p167 复验：本 TU 全部残余件 MISSING 仅 C1/C2、D1/D2 **同址别名镜像**（P93）
//   —— 脚本 /tmp/mirrorchk.sh 逐符号核 .o 的 U/D 符号，REALMISS=0 ⇒ 语义 CLEAR。
//   判据⑪ 槽直方图（objdump --disassemble 逐值对比）：
//     · OnEnterState：两侧 **11 槽逐值全等**（残余=E H 段序 N104 + 跳表 N4/N125）。
//     · HandleGetItem：整帧平移常数 **0xc**（我方 -0x40:46 ↔ ORIG -0x4c:46）⇒ N126。
//     · ClearReward：-0x50↓ 槽组平移 0xc（-0x54↔-0x50、-0xb4↔-0xa4/-0xa8…）⇒ N126+
//       N85（flds/fstps vs mov 直搬）、N76 store 序。
//     · HandleItemDrop / HandleMonsterKill / CalcGuildFP / WPTable：push Δ=0、
//       槽表近全等，残余 = 跳表 lowering(N4/N125)、算术强度归约(N86/N1，445/443)、
//       字节清零合并(N78/N202)、P223 单源尾岛。
//   结论：**本 TU 剩余真缺口 = 0**（9 件 DIFF 全为已记证 N 族；near 已清零）。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CInventory.h"          // 权威 CInventory / Store / user_creature::CCreatureMgr / 共享枚举
#include "CDungeonEntranceLog.h"
#include "GlobalData.h"          // 权威 GlobalData::s_systemTime_ / s_event_manager
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
#include "CServerEvent.h"
#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）
#include "CItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CInventory.h"          // 权威 CInventory / Store / user_creature::CCreatureMgr / 共享枚举
#include "CDungeonEntranceLog.h"
#include "GlobalData.h"          // 权威 GlobalData::s_systemTime_ / s_event_manager
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
#include "CServerEvent.h"
#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）

#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）
#include "CUser.h"           // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CGameManager.h"    // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CPrivateStoreMgr.h" // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）

// ---- 本地枚举（与 CInventory.h 同 guard，避免同时包含时重定义） ----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8
};
#endif

#ifndef DNF_ENUM_EMONEYADDREASON_DEFINED
#define DNF_ENUM_EMONEYADDREASON_DEFINED
enum eMoneyAddReason
{
    eMoneyAddReason_0 = 0,
    eMoneyAddReason_4 = 4,
    eMoneyAddReason_7 = 7,
    eMoneyAddReason_0x1a = 0x1a
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14
};
#endif

// ---- 本地最小视图（权威声明见 CValueStatistic.cpp / CEventManager 头 /
//      HistoryLog.cpp / TimerQueue.h，因各自权威头与既有 include 冲突或尚未
//      建立，暂以本地声明 + 真实调用；方法定义在权威 TU，mangled 与 ORIG 一致）----
// 注：CEventManager/CEventBase 权威声明在 CGuildServerProxy.h（经 CExpDoubleEvent.h
//     引入），此处不再本地定义避免重定义冲突。
class HistoryLog
{
public:
    static void WriteUseCoin(_IO_FILE* file, const char* reason);  // _ZN10HistoryLog12WriteUseCoinEP8_IO_FILEPKc
};

extern CValueStatistic* GetInstanceValueStatistic();
TimerQueue* G_TimerQueue();  // 权威声明将入 TimerQueue.h（CleanPvPAsm 添加中）
char* NumberToString(unsigned int value, int radix);

// ============================================================================
// 依赖子对象（权威声明见 WarField.h，经 WarRoom.h→PvP_deps.h 引入；
//      构造/析构为真实符号）
// ============================================================================

// ============================================================================
// 构造 / 析构
// ============================================================================
WarRoom::WarRoom()
{
    SetState((WARROOM_STATE)-1);
    for (int i = 0; i <= 5; ++i)
        m_slots[i] = 0;
}

WarRoom::~WarRoom()
{
}

// ============================================================================
// 基础访问器（ORIG weak：GetIndex 0x822ee8a / GetState 0x822ee96 /
//   SetState 0x82a5a56 / GenTimerKey 0x86c0b24 / CheckTimerKey 0x863bd92）
// ============================================================================
// [SweepWarRoom ㉓ 符号绑定] ORIG 全 W（头内 inline 弱副本），我方 T ⇒ 定义加
//   __attribute__((weak))（不改头，mangled/语义不变）。
__attribute__((weak)) int WarRoom::GetIndex()
{
    return m_index;
}

__attribute__((weak)) int WarRoom::GetState()
{
    return m_state;
}

__attribute__((weak)) bool WarRoom::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return m_timerKey[msg - 0x3f] == key;
}

void WarRoom::CheckState()
{
    if (GetWaiterCount() == 0)
    {
        ChangeState(WARROOM_STATE_0);
    }
    else if ((GetWaiterCount() < 3) && (GetState() == 1))
    {
        ChangeState(WARROOM_STATE_0);
    }
}

void WarRoom::ChangeState(WARROOM_STATE state)
{
    // ORIG 086bb53c：je 086bb57f(nop;fall leave) 早退岛 + body 尾 jmp 086bb580。
    // 技巧6 早退形（正向 if!= 在 4.4 会吞尾岛）。
    if (m_state == state)
        return;
    OnLeaveState(state);
    m_state = state;
    OnEnterState();
}

int WarRoom::GetLiveCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_liveFlag[i] != 0))
            ++count;
    }
    return count;
}

int WarRoom::GetRemainSeat() const
{
    return 6 - GetWaiterCount();
}

int WarRoom::GetUserSlot(CUser* user)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slotsUser[i] == user)
            return i;
    }
    return -1;
}

int WarRoom::GetWaiterCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            ++count;
    }
    return count;
}

void WarRoom::SetIndex(int idx)
{
    m_index = idx;
    Reset();
}

void WarRoom::CurSpawnStepReset()
{
    m_curSpawnStep = 0;
}

int WarRoom::Create(CUser* user, char* name, short idx)
{
    m_state = (WARROOM_STATE)0;
    m_slots[0] = user;
    m_slotState[0] = 1;
    user->SetWarRoomIndex((short)m_index);
    return 0;
}

void WarRoom::AddBanList(CUser* user)
{
    if (user->isGMUser() != 0)
        return;
    unsigned int id = user->get_acc_id();
    m_map[id] = GlobalData::s_systemTime_.getCurSec();
}

bool WarRoom::IsBanUser(CUser* user)
{
    unsigned int id = user->get_acc_id();
    std::map<unsigned int, int>::iterator it = m_map.find(id);
    // ORIG 0x86ba9e7：end() 临时量内联于比较表达式（slot 布局 it=-0x14/临时=-0xc 实证）；
    // bool 返回值在 if 中单层否定 → 具名 0/1 返回。
    if (it != m_map.end())
        return 1;
    return 0;
}

void WarRoom::UpdateBanList()
{
    int cur = GlobalData::s_systemTime_.getCurSec();
    std::map<unsigned int, int>::iterator it = m_map.begin();
    std::map<unsigned int, int>::iterator e = m_map.end();
    while (it != e)
    {
        int t = (*it).second;   // ORIG 用 operator*（de）
        if ((cur - t) >= G_CDataManager()->m_warAreaCounter.m_banInterval / 1000)
        {
            m_map.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void WarRoom::BattleReset()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            (m_slotsUser[i])->getHackAnalyzer()->reportHackInfo();
            (m_slotsUser[i])->getHackAnalyzer()->resetHackInfo();
            (m_slotsUser[i])->ResetCoinCount();
        }
        m_liveFlag[i] = 0;
        m_slotData[i] = 0;
        m_slotFp[i] = 0;
        m_hostVotes[i] = -1;
    }
    m_field.Reset();
    memset(m_roleKillCnt, 0, sizeof(m_roleKillCnt));
    m_userCount = 0;
    m_flag10 = 0;
    m_flag11 = 0;
    m_flag12 = 0;
    CurSpawnStepReset();
}

void WarRoom::Reset()
{
    BattleReset();
    m_field8 = 0;
    m_levelBand = 2;
    m_roomSeq = 0;
    m_field30 = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_slots[i] = 0;
        m_slotState[i] = 0xff;
        m_slotData[i] = 0;
    }
}

void WarRoom::InvalidUserCheck()
{
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 086bb595：双跳落点异块 —— je +0xd5(nop;fall end) / je +0xd6(end) /
        // body 尾 jmp +0xd6。slot-null goto 落独立空块，R!=0 与 body 尾直接去 loop-tail。
        // ORIG 086bb595 块序：[body][jmp E][X: nop][E: addl]；je1→X，je2→E。
        if (m_slots[i] == 0)
            goto INVALID_SLOT_NULL;
        if ((m_slotsUser[i])->getCurCharacR() == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x236, 5)
                ("WarRoom::InvalidUserCheck slot(%d) warRoomIndex(%d)",
                 i, (m_slotsUser[i])->GetWarRoomIndex());
            m_slotState[i] = 0xff;
            m_slots[i] = 0;
            m_liveFlag[i] = 0;
            goto INVALID_NEXT;
        }
        goto INVALID_NEXT;
INVALID_SLOT_NULL:
        ;
INVALID_NEXT:
        ;
    }
}

int WarRoom::IsJoinable(CUser* user)
{
    if ((GetState() == 4) || (GetState() == 3) || (GetState() == -1) ||
        (GetState() == 2))
        return 0x13;
    if (user->isCompetitionMercenary() != 0)
        return 0x15;
    if (user->getCurCharacMoney() <
        G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand])
        return 10;
    // ORIG 0x86bad6b：bool 返回 `!= 1` 折成 xor $0x1 判定；true 路径早退。
    if (user->CheckFatigue() != 1)
        return 0x43;
    {
        int level = user->get_charac_level();
        switch ((unsigned char)m_levelBand)
        {
        case 2:
            if ((level < 0x14) || (0x1d < level))
                return 0xe;
            break;
        case 3:
            if ((level < 0x1e) || (0x27 < level))
                return 0xe;
            break;
        case 4:
            if ((level < 0x28) || (0x31 < level))
                return 0xe;
            break;
        case 5:
            if ((level < 0x32) || (0x3b < level))
                return 0xe;
            break;
        case 6:
            if ((level < 0x3c) || (0x46 < level))
                return 0xe;
            break;
        default:
            return 0xe;
        }
        UpdateBanList();
        if (IsBanUser(user))
            return 0x1e;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] == 0) && (m_slotState[i] == 0xff))
                return 0;
        }
        return 4;
    }
}

int WarRoom::WalkOutUserBySlot(int slot)
{
    // [b28d-solo-WarRoom] AE | ORIG 0x086bd256 | P235 union 具型视角消 cast 二段
    //   （ORIG 0x086bd2f6 `mov 0xc(%eax,%edx,4),%ebx` 单段直落）后逐条全等。
    if (m_slots[slot] == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp",
                              __PRETTY_FUNCTION__, 0x5cc,
                              "WarRoom::WalkOutUserBySlot no slot(%d)", slot);
        return 4;
    }
    (m_slotsUser[slot])->getHackAnalyzer()->reportHackInfo();
    (m_slotsUser[slot])->getHackAnalyzer()->resetHackInfo();
    m_slotState[slot] = 0xff;
    // ORIG 086bd2f6: 槽值单步直落 %ebx 跨 G_GameWorld 调用；4.4 恒 eax 落值+
    // mov 保活两步（N18a；P226 裸语句表达式形实测同两步不可翻，b28BM13）。
    G_GameWorld()->out_from_warroom(m_slotsUser[slot]);
    if (m_pDungeon != 0)
    {
        (m_slotsUser[slot])->m_historyLog.LeaveDungeon(m_pDungeon->GetDungeonName(),
                                                           (unsigned char)m_levelBand);
    }
    m_slots[slot] = 0;
    m_liveFlag[slot] = 0;
    PacketGuard packet;
    MakeSlotInfo((char*)&packet, slot);
    G_GameWorld()->send_all(packet);
    return 0;
}

int WarRoom::WalkOutUser(CUser* user)
{
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    return WalkOutUserBySlot(slot);
}

int WarRoom::Destroy(int idx)
{
    int user = (int)m_slots[idx];
    if (user == 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                user = (int)m_slots[i];
                break;
            }
        }
    }
    Reset();
    return user;
}

void WarRoom::Open()
{
    SetState(WARROOM_STATE_0);
}

void WarRoom::Close()
{
    SetState((WARROOM_STATE)-1);
}

int WarRoom::ShutDown()
{
    if (m_state < 1)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                CUser* u = m_slotsUser[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)7);
            }
        }
        Close();
        return 1;
    }
    m_field30 = 1;
    return 0;
}

void WarRoom::Prepare()
{
    int dungeonIdx =
        G_CDataManager()->m_warAreaCounter.m_warAreaDungeon[(unsigned char)m_levelBand];
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x66e,
                              "WARROOM G_CDataManager()->find_dungeon( %d) fail", 0);
    }
    else
    {
        m_pDungeon = dungeon;
        // ORIG 0x86bd65e：int 直落 callee-saved %ebx，免 scratch 两行。
        register int mapIdx = m_pDungeon->m_warRoomData.m_10;
        // ORIG 0x86bd6d0：int 变量直存使 store 块 this→eax 先行、值→edx 后置。
        int map = (int)G_CDataManager()->find_map(mapIdx);
        if (map == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x678,
                                  "WARROOM Find Map[%d] Error",
                                  m_pDungeon->m_warRoomData.m_10);
        }
        else
        {
            m_fp = map;
        }
    }
}

void WarRoom::Start()
{
    PacketGuard packet;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            packet.clear();
            packet.put_header(0, 0x5a);
            packet.finalize(true);
            (m_slotsUser[i])->Send(packet);
            if (m_pDungeon != 0)
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    m_pDungeon->get_index(), false);
            }
        }
    }
    ChangeState(WARROOM_STATE_1);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            // b28d-solo-WarRoom [P235/P301]：改用 union 具型视角 m_slotsUser[i]
            // （同址 +0x12c）消除 `(CUser*)m_slots[i]` cast 的二段装载，复现 ORIG
            // 0x086bc3xx 的 `mov 0xc(%eax,%edx,4),%ebx` 单段直落 callee-saved。
            m_slotsUser[i]->getHackAnalyzer()->beginCollectHackInfo(m_slotsUser[i]);
        }
    }
}

void WarRoom::ReviveAll()
{
    int i = 0;
    while (i <= 5)
    {
        // ORIG 086bc46f：双空岛 086bc4d7(nop;jmp 086bc4db)/086bc4da(nop;fall)，
        // body 尾 jmp 086bc4db（同 PvP_Room check_recv_pvp_rank 已验证双 goto 模式）。
        if (m_slots[i] == 0)
            goto REVIVE_NEXT;
        if (m_liveFlag[i] != 0)
            goto REVIVE_SKIP;
        SetCharacterLive(i, m_slotsUser[i], true, false, 0, 0xff);
REVIVE_SKIP:
        ;
REVIVE_NEXT:
        ;
        ++i;
    }
}

void WarRoom::RemoveKC()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            (m_slotsUser[i])->InitFinishPoint();
            ((CInventory*)(m_slotsUser[i])->getCurCharacInvenW())->RemoveKCItem();
        }
    }
}

void WarRoom::CancelAllTimer()
{
    for (int i = 0x40; i <= 0x47; ++i)
        GenTimerKey((TIMER_MESSAGE)i);
}

void WarRoom::SkipLastRankResultTimer()
{
}

int WarRoom::SendWalkOut(CUser* user, ENUM_WALKOUT_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 10);
    packet.put_byte(0);
    packet.put_byte((int)type);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}
void WarRoom::SendWarRoomFailMsg()
{
    // [b28d-solo-WarRoom · identical] ORIG 0x086c078e | 判词：ORIG 0x086c07a9
    //   `call GenTimerKey; mov %eax,%ebx`（key→%ebx）、0x086c07ae
    //   `mov 0x4(%eax),%esi`（m_index→%esi）、G_TimerQueue 置后 0x086c07b1。
    //   旋钮 = callee 返回类型与实参形参同型：WarRoom.h GenTimerKey 由
    //   unsigned int 改 int（T2 直证：ORIG 调用点无 setcc/无中转，第 5 实参形参
    //   为 int）⇒ 4.4.6 复现同一 callee-saved 分配（探针 /tmp/wr_probe3.cpp
    //   r1=int→key→%ebx/idx→%esi；r2/r5=unsigned→反向）。旧 N7 判词作废。
    //   fresh .o NDIFF=0。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                (TIMER_MESSAGE)0x46, 10,
                                GenTimerKey((TIMER_MESSAGE)0x46), 0);
}
void WarRoom::WarRoomFail()
{
    if (GetLiveCount() != 0)
        return;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            AddBanList(m_slotsUser[i]);
            CUser* u = m_slotsUser[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)4);
        }
    }
    ChangeState(WARROOM_STATE_0);
}

int WarRoom::HostChangeWarRoom(CUser* user, char a, unsigned long b)
{
    // [b28d-solo-WarRoom · identical] ORIG 0x086c08a6 | 判词：①ORIG 0x086c08e4
    //   `lea 0x4(%edx),%ecx` + 0x086c08e7 `mov 0x14(%ebp),%edx` ⇒ m_hostVotes
    //   元素与 b(unsigned long) 同型（WarRoom.h int[6]→unsigned long[6]，Main
    //   代落；探针 wr_probe7 v7/v8 出 lea 形，v1–v3 与 wr_probe8 v14–v19 六形
    //   恒 add）；②GenTimerKey→%ebx / m_index→%esi 同 SendWarRoomFailMsg。
    //   fresh .o NDIFF=0。
    if ((m_slots[(int)a] != 0) && (m_slotsUser[(int)a] == user))
    {
        // [b28BM13 已翻正] ORIG 086c08dd 装载序 a→this→lea→b 由同型化复现。
        m_hostVotes[(int)a] = (int)b;
        if (m_userCount == 0)
        {
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x47, 5,
                                        GenTimerKey((TIMER_MESSAGE)0x47), 0);
        }
        m_userCount += 1;
    }
    return 0;
}
void WarRoom::CheckHostChange()
{
    // b28d-solo-WarRoom：ORIG 0x86c0964 控制流实证 —— m_userCount<=1 时
    // `jle 0x86c0a8d` 直落重置块（m_hostVotes[i]=-1、m_userCount=0）；仅
    // m_userCount>1 且 bestSlot==-1 时经 ebx=0 跳过重置（0x86c0a5c mov $1,%ebx /
    // 0x86c0a1e mov $0,%ebx + 0x86c0a89 test %ebx,%ebx; je end）⇒ 原码是块内
    // `if (bestSlot == -1) return;`，无 found 具名局部（P0：ORIG 无槽往返）。
    // 旧形 `bool found=false; ... if(!found) return;` 在 m_userCount<=1 时也早退
    // （不重置）—— 语义真 bug，已修。
    if (m_userCount > 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x160);
        unsigned int bestIdx = 0xffffffff;
        int bestSlot = -1;
        for (int i = 0; i <= 5; ++i)
        {
            // b28d-solo-WarRoom [P223]：ORIG 0x86c09c7/0x86c09d9 两个独立 je 各落
            // nop 中继岛（0x86c0a05 nop;jmp / 0x86c0a08 nop;fall），第三个守卫
            // `ja 0x86c0a09` 无岛直落 —— 负条件 goto + 尾标签串唯一复现形
            // （&& 短路链被 4.4 jump-thread，少 2 岛）。
            if (m_slots[i] == 0)
                goto CHC_NEXT1;
            if (m_hostVotes[i] == -1)
                goto CHC_NEXT2;
            if ((unsigned int)m_hostVotes[i] <= bestIdx)
            {
                bestIdx = (unsigned int)m_hostVotes[i];
                bestSlot = i;
            }
CHC_NEXT1:
            ;
CHC_NEXT2:
            ;
        }
        if (bestSlot == -1)
            return;
        packet.put_byte(bestSlot);
        packet.finalize(true);
        SendToRoom(packet);
    }
    for (int i = 0; i <= 5; ++i)
        m_hostVotes[i] = -1;
    m_userCount = 0;
}

void WarRoom::HandleTimerTimeBomb()
{
    m_field.m_mapInfo.m_monsterMap.clear();
    m_flag10 = 1;
    if (m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50)
        ChangeState(WARROOM_STATE_5);
    else
        ChangeState(WARROOM_STATE_4);
}

void WarRoom::HandleTimerResultCount()
{
    ReviveAll();
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 086c0421：双空岛 086c04a6(nop;jmp 086c04aa)/086c04a9(nop;fall)，
        // body 尾 jmp 086c04aa（同 ReviveAll 双 goto 模式）。
        CUser* u;
        if (m_slots[i] == 0)
            goto TRES_NEXT;
        if (m_slotData[i] == 0)
            goto TRES_SKIP;
        AddBanList(m_slotsUser[i]);
        u = m_slotsUser[i];
        WalkOutUserBySlot(i);
        SendWalkOut(u, (ENUM_WALKOUT_TYPE)m_slotData[i]);
TRES_SKIP:
        ;
TRES_NEXT:
        ;
    }
    BattleReset();
    CheckFatuigue();
    if (GetWaiterCount() < 3)
        ChangeState(WARROOM_STATE_0);
    else
        ChangeState(WARROOM_STATE_1);
}

void WarRoom::SendStatePacket()
{
    PacketGuard packet;
    packet.put_header(0, 0x58);
    packet.put_short(1);
    MakeStateInfo((char*)&packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

void WarRoom::MakeStateInfo(char* buf)
{
    // [b28d-solo-WarRoom] DIFF(14) | ORIG 0x086be07a | N82附：4.1 对成员调用
    //   receiver-first（先 `mov -0xc(%ebp),%eax` 装 p，实参 scratch 用 %edx），
    //   4.4 恒 arg-first（实参 scratch %eax 后压、再装 receiver）。探针
    //   /tmp/probe_rcv.cpp t1..t10（指针/引用/解引用/cast 全形）均 arg-first ⇒ 封顶。
    //   本轮 fresh .o 复测：ndiff=11（orig 26/our 25），显式 (int) cast 类型旋钮
    //   （P265/P319）零变化 ⇒ 判 N7/P75（实参装载时刻跨版本）。
    // [b28d-solo-WarRoom] 翻正：探针 /tmp/wr_probe5.cpp t9 实证 —— 局部声明为
    //   char*（与形参同型，保留 ORIG -0xc 槽 store），调用点再 cast 到
    //   InterfacePacketBuf* ⇒ 4.4 复现 ORIG「值先物化 %edx、再装 receiver %eax、
    //   后统一压 esp 槽」形（t7/t10 的指针局部同型 cast 恒折 arg-first，不达）。
    char* p = buf;
    // [b28BM13 N18a/P75] ORIG 086be089/086be0a4/086be0bb：4.1 先把实参值物化
    // 到 %edx 再统一装 esp 槽；4.4 恒「即装即压」落 %eax（单段 mem→%edx 不可
    // 造）。register 绑定按 P0 全域禁用（旧 asm("edx") 尝试注记作废）⇒ 记 N。
    ((InterfacePacketBuf*)p)->put_short(m_index);
    ((InterfacePacketBuf*)p)->put_byte(m_state);
    ((InterfacePacketBuf*)p)->put_short(m_curSpawnStep);
}

void WarRoom::MakeRoomInfo(char* buf)
{
    // [b28d-solo-WarRoom] DIFF(67) | ORIG 0x086bdd58 | ①已修：m_pDungeon
    //   分支块序翻正（ORIG `test;je <if体>` = 非 0 直落，旧形反）；②残余为
    //   N82附 receiver-first vs arg-first（probe_rcv.cpp 十形全败）⇒ 封顶。
    //   本轮 fresh .o 复测：ndiff=45（orig 94/our 93），P267 双 cast
    //   `(unsigned char)(unsigned int)` 旋钮零变化 ⇒ 判 N7/P75。
    // [b28d-solo-WarRoom] 翻正：同 MakeStateInfo —— 局部 char* + 调用点 cast
    //   （探针 /tmp/wr_probe5.cpp t9）复现 ORIG「值先物化 %edx」形。
    char* p = buf;
    ((InterfacePacketBuf*)p)->put_short(m_index);
    ((InterfacePacketBuf*)p)->put_byte((unsigned char)m_field8);
    ((InterfacePacketBuf*)p)->put_byte((unsigned char)m_levelBand);
    ((InterfacePacketBuf*)p)->put_byte((unsigned char)m_roomSeq);
    ((InterfacePacketBuf*)p)->put_byte(m_state);
    // b28d-solo-WarRoom [P222/P191]：ORIG 086be2xx `test;je <if体>` = 非 0 直落
    // （正极性）⇒ 反写 if/else 以复现块序。
    if (m_pDungeon != 0)
        ((InterfacePacketBuf*)p)->put_short(m_pDungeon->m_warRoomData.m_10);
    else
        ((InterfacePacketBuf*)p)->put_short(0);
    for (int i = 0; i <= 5; ++i)
    {
        ((InterfacePacketBuf*)p)->put_byte(m_slotState[i]);
        // [b28d-solo-WarRoom P222] ORIG 086be2xx `test;je <0xffff 块>`：非 0 直落
        //   ⇒ 写 != 0 的正向 if 才复现（旧 == 0 形产 jne + 反序两臂）。
        if (m_slots[i] != 0)
            ((InterfacePacketBuf*)p)->put_short((m_slotsUser[i])->get_unique_id());
        else
            ((InterfacePacketBuf*)p)->put_short(0xffff);
    }
}
void WarRoom::MakeSlotInfo(char* buf, int seat)
{
    // [b28d-solo-WarRoom] DIFF(41) | ORIG 0x086bdea0 | N82附：同 MakeStateInfo 的
    //   receiver-first vs arg-first（ORIG 每调用点先 `mov -0x14(%ebp),%eax`）；
    //   probe_rcv.cpp 十形全败 ⇒ 封顶。语义/调用序与 ORIG 一致。
    // ORIG 0x86be1xx：buf 物化 -0x14 局部槽，后续全部从槽读（p 指针局部）。
    // [b28d-solo-WarRoom] 翻正：同 MakeStateInfo —— 局部声明 char*（与形参同型，
    //   保留 ORIG -0x14 槽 store），调用点 cast 到 InterfacePacketBuf* 复现
    //   ORIG「值先物化 %edx / receiver 后装」形（探针 /tmp/wr_probe5.cpp t9）。
    char* p = buf;
    // ORIG 0x086bde0x 首调用 receiver-first（`mov -0x14(%ebp),%eax` 先于常量压栈）；
    // 内联 cast 形（P277 反向）实测 ndiff 50 不变 ⇒ 保持 p-> 形。
    ((InterfacePacketBuf*)p)->put_header(0, 0x59);
    ((InterfacePacketBuf*)p)->put_short(m_index);
    if (seat == 0xffff)
    {
        int count = GetWaiterCount();
        ((InterfacePacketBuf*)p)->put_byte(count);
        if (count == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__,
                                  0x793, "Send Member 0 PvpRoom %d State(%d)", m_index,
                                  m_state);
        }
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                ((InterfacePacketBuf*)p)->put_byte(i);
                ((InterfacePacketBuf*)p)->put_byte(m_slotState[i]);
                ((InterfacePacketBuf*)p)->put_short((m_slotsUser[i])->get_unique_id());
            }
        }
    }
    else
    {
        ((InterfacePacketBuf*)p)->put_byte(1);
        ((InterfacePacketBuf*)p)->put_byte(seat);
        ((InterfacePacketBuf*)p)->put_byte(m_slotState[seat]);
        // [b28d-solo-WarRoom P222/P191] ORIG 086be0xx `test;je <0xffff 块>`：
        //   非 0 直落（正极性）⇒ 写成 != 0 的正向 if 才复现块序。
        if (m_slots[seat] != 0)
            ((InterfacePacketBuf*)p)->put_short((m_slotsUser[seat])->get_unique_id());
        else
            ((InterfacePacketBuf*)p)->put_short(0xffff);
    }
    ((InterfacePacketBuf*)p)->finalize(true);
}

void WarRoom::SendToRoom(PacketGuard& guard)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            (m_slotsUser[i])->Send(guard);
    }
}

// ============================================================================
// 状态机
// ============================================================================
void WarRoom::OnLeaveState(WARROOM_STATE state)
{
    if (state == WARROOM_STATE_0)
        CurSpawnStepReset();
    // ORIG 0x86bcf2d：switch 降形态复现「单 load→test/je(case0)→cmp $2/jne→
    // state==3」的级联标签（je/jne 目标分设 +0x178/+0x179）。
    switch (m_state)
    {
    case 0:
        break;
    case 2:
        if (state == 3)
        {
            for (int i = 0; i <= 5; ++i)
            {
                // ORIG 086bd06x：FP>0 body 尾 jmp +0x163(end) 跳空岛 086bd076(nop;
                // fall end)=slot-null 落点；FP<=0 je 直落 end。双 goto 钉块。
                {   // money/min 作用域包住，goto 跳出合法且不新增槽
                if (m_slots[i] == 0)
                    goto FP_SLOT;
                int money = (m_slotsUser[i])->getCurCharacMoney();
                int min = std::min(
                    G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand],
                    money);
                ((CInventory*)(m_slotsUser[i])->getCurCharacInvenW())
                    ->use_money(min, (eMoneySubReason)0xf, 1);
                if ((m_slotsUser[i])->GetFinishPointTotal() > 0)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x55b, 5)
                        ("쟁역 FP누적됨 발생", (m_slotsUser[i])->GetFinishPointTotal());
                    (m_slotsUser[i])->InitFinishPoint();
                    goto FP_JOIN;
                }
                }
FP_JOIN:
                ;
FP_SLOT:
                ;
            }
        }
        break;
    default:
        break;
    }
}

// [b28d-solo-WarRoom-3] WarRoom::OnEnterState（ORIG 0x086bb71c）：528/-13 →
//   **515/515（gap 0，帧 0x7c 与槽集 11 个逐值一致）**。本轮实际修正：
//   ①case1 m_liveFlag 块序翻正（086bb89f je→else 块）；②hasGM 循环拆条件 +
//   P223 负条件 goto 尾空标签复现 086bb920 nop 岛；③case1-5 GenTimerKey/ms 全部
//   改内联实参（086bb966 %edi / 086bb9ce %ebx / 086bba40 / 086bbe56；P24/P52）；
//   ④case3 count、case5 fp 内联（086bbb04 %ecx / 086bbcda %ebx）；⑤P324 叶块序
//   = ORIG 的 case 0,1,2,**4,3**,5（086bba27 ReviveAll 先于 086bbab3）⇒ 源码照
//   ORIG 块地址序重排；⑥**真 bug** 字段偏移 m_ec/m_f4 → m_f4/m_fc（086bbb6a
//   +0x7e8、086bbbcd +0x7f0 = m_warRoomData+0xf4/+0xfc）；⑦**真 bug** 比较边界
//   086bbce7 `setle`：fp<=penalty 走 DecreaseWarPoint(0)、否则 penalty（原写
//   `penalty < fp` 在等号点反）；⑧case5 两循环 P223 尾标签复现 086bbd69/086bbe15
//   body 尾 jmp+nop 岛。残余 = N104（086bbc08..086bbc30 EH 尾块与正常路径块相对
//   序，4.4 恒就地内联）+ 1 条 m_fp→(const CMap*) cast 中转 + 4.4 跳表
//   `shl$2;mov disp(%eax)`（N4/N125）⇒ N 封顶，P167 CLEAR。
void WarRoom::OnEnterState()
{
    // [b28d-solo-WarRoom] DIFF(415) | ORIG 0x086bb71c | 复核 b20 判词：P167
    //   68/68 归零；残余为 N82 帧槽平移 + N76/N18a store 装载序 + 4.1 判定树
    //   分派形态（本轮 P235 视角替换后复验无新增残差）⇒ 语义完成、byte 层 N 封顶。
    CancelAllTimer();
    InvalidUserCheck();
    SendStatePacket();
    switch (m_state)
    {
    case 0:
        UpdateBanList();
        if (m_field30 != 0)
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                    WalkOutUserBySlot(i);
            }
            Close();
            m_field30 = 0;
            PacketGuard packet;
            packet.put_header(0, 0x58);
            packet.put_short(1);
            MakeStateInfo((char*)&packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        break;
    case 1:
        m_fpBase = GlobalData::s_systemTime_.getCurSec();
        BattleReset();
        for (int i = 0; i <= 5; ++i)
        {
            // [b28d-solo-WarRoom-3] ORIG 086bb89f `je <else块(movb $0x0)>`：
            // 正极性 then=置 1 先落（P222/P214① 块序）⇒ 源码写 != 0 分支。
            if (m_slots[i] != 0)
                m_liveFlag[i] = 1;
            else
                m_liveFlag[i] = 0;
        }
        if (GetWaiterCount() <= 2)
        {
            bool hasGM = false;
            for (int i = 0; i <= 5; ++i)
            {
                // [b28d-solo-WarRoom-3] ORIG 086bb8ff/086bb918 两处 `je` 直跳
                // 循环增量（086bb920 nop 岛 / 086bb921 addl），无 mov $1/$0 归一岛
                // ⇒ 拆嵌套 if（`&&` 在 4.4 会物化 bool 再 test）；外层失败出口
                // 用负条件 goto 到循环体尾空标签复现 086bb920 nop 岛（P223）。
                if (m_slots[i] == 0)
                    goto GM_LOOP_NEXT;
                if ((m_slotsUser[i])->isGMUser())
                {
                    hasGM = true;
                    break;
                }
GM_LOOP_NEXT:
                ;
            }
            if (!hasGM)
            {
                ChangeState(WARROOM_STATE_0);
                return;
            }
        }
        // [b28d-solo-WarRoom-3] P24/P52：ORIG 086bb966 `call GenTimerKey; mov %eax,%edi`
        // 为内联实参（值保 callee-saved 跨 G_CDataManager/G_TimerQueue），ms 亦为
        // 表达式直传（086bb973 `mov 0x88a8(%eax),%eax; mov %eax,%ebx`）⇒ 禁具名局部。
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x41,
            G_CDataManager()->m_warAreaCounter.m_readyCount,
            GenTimerKey((TIMER_MESSAGE)0x41), 0);
        break;
    case 2:
        // [b28d-solo-WarRoom-3] ORIG 086bb9ce `call GenTimerKey; mov %eax,%ebx` 内联实参。
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x42, 11000,
            GenTimerKey((TIMER_MESSAGE)0x42), 0);
        m_curSpawnStep = 1;
        break;
    // [b28d-solo-WarRoom-3] P324：ORIG 叶块物理序 = case 0,1,2,**4,3**,5
    //（086bba27=ReviveAll 块先于 086bbab3=m_flag10 块）⇒ case 书写序须照 ORIG
    // 块地址序排列，故 case 4 段写在 case 3 段之前。
    case 4:
        ReviveAll();
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x43, 10,
                                        GenTimerKey((TIMER_MESSAGE)0x43), 0);
        if (m_curSpawnStep != m_pDungeon->m_warRoomData.m_50)
            m_curSpawnStep += 1;
        break;
    case 3:
        m_flag10 = 0;
        m_flag11 = 0;
        {
            PacketGuard packet;
            packet.clear();
            // [b28d-solo-WarRoom-3] ORIG 086bbb04 `call GetWaiterCount; mov %eax,%ecx`
            // 无槽往返 ⇒ 原码为内联实参（P0/P24），具名 count 会多占槽。
            m_field.ConsistMap(&packet, m_pDungeon, (const CMap*)m_fp,
                               (unsigned char)(((unsigned char)m_levelBand + 1) * 10),
                               GetWaiterCount());
            SendToRoom(packet);
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    // [b28d-solo-WarRoom-3] 字段偏移真 bug 修正：ORIG 086bbb6a 为
                    // `mov 0x7e8(%eax),%edx`（= m_warRoomData(+0x6f4) + 0xf4 = m_f4），
                    // 原重建写 m_ec(+0xec) 读错字段。
                    (m_slotsUser[i])->FatigueUp(
                        m_pDungeon->m_warRoomData.m_f4);
                }
            }
            // [b28d-solo-WarRoom-3] ORIG 086bba40 `call GenTimerKey; mov %eax,%ebx`
            // 与 m_fc/1000 均内联实参（P24）；ORIG 086bbbcd `mov 0x7f0(%eax),%ecx`
            // = +0x6f4 + 0xfc = m_fc（原重建误写 m_f4）。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x45,
                                            m_pDungeon->m_warRoomData.m_fc / 1000,
                                            GenTimerKey((TIMER_MESSAGE)0x45), 0);
        }
        break;
    case 5:
        if ((m_flag10 != 0) && (m_field.m_stageIdx ==
                                m_pDungeon->m_warRoomData.m_50))
        {
            PacketGuard packet;
            packet.put_header(0, 0x156);
            packet.put_byte(GetWaiterCount());
            for (int i = 0; i <= 5; ++i)
            {
                // [b28d-solo-WarRoom-3] ORIG 086bbcbf `je 086bbd6b(nop 岛)` +
                // body 尾 086bbd69 `jmp 086bbd6c(addl)` ⇒ 双源 goto 到尾空标签
                // （P223 定案），单用 if 包裹会被 4.4 jump-thread 掉 nop 岛。
                if (m_slots[i] == 0)
                    goto SEND_LOOP_NEXT;
                {
                    // [b28d-solo-WarRoom-3] ORIG 086bbcda `call GetFinishPointTotal;
                    // mov %eax,%ebx`（无槽）⇒ 内联实参（P0）。条件与块序照
                    // 086bbce7 `cmp %eax,%ebx; setle %al; test; je 086bbd0f`：
                    // setle 为假(fp>penalty)才跳 then 块 ⇒ 源码须写**否定条件**
                    // 且 else 直落（086bbcf0 为 DecreaseWarPoint(0) 块、
                    // 086bbd0f 为 penalty 块）—— P194/P59 镜像布局。
                    if ((m_slotsUser[i])->GetFinishPointTotal() <=
                        G_CDataManager()->m_warAreaCounter.m_wpPenalty)
                    {
                        (m_slotsUser[i])->DecreaseWarPoint(0);
                    }
                    else
                    {
                        (m_slotsUser[i])->DecreaseWarPoint(
                            G_CDataManager()->m_warAreaCounter.m_wpPenalty);
                    }
                    packet.put_byte(i);
                    packet.put_int((m_slotsUser[i])->GetFinishPointTotal());
                }
SEND_LOOP_NEXT:
                ;
            }
            packet.finalize(true);
            SendToRoom(packet);
        }
        for (int i = 0; i <= 5; ++i)
        {
            // [b28d-solo-WarRoom-3] ORIG 086bbde4 `je 086bbe17(nop 岛)` 与
            // 086bbe02 `je 086bbe18(addl)` 落点不同 + body 尾 086bbe15 `jmp`
            // ⇒ 负条件 goto 尾空标签（P223 定案形，同 hasGM 循环）。
            if (m_slots[i] == 0)
                goto SLOT_ISLAND;
            if ((m_slotsUser[i])->GetFinishPointTotal() == 0)
                m_slotData[i] = 6;
SLOT_ISLAND:
            ;
        }
        ClearReward();
        SendGuildFP();
        RemoveKC();
        // [b28d-solo-WarRoom-3] ORIG 086bbe56 `call GenTimerKey; mov %eax,%ebx` 内联实参。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x44, 0x18,
                                        GenTimerKey((TIMER_MESSAGE)0x44), 0);
        break;
    default:
        break;
    }
}

void WarRoom::CheckFatuigue()
{
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 086bc4ff：je1→086bc5e4(nop;fall end)空岛；body1 尾/body2 尾/je2
        // 均直落 end(086bc5e5)，body2 尾显式 jmp 跳岛(086bc5e2)。
        if (m_slots[i] == 0)
            goto CF_ISLAND;
        if (!(m_slotsUser[i])->CheckFatigue())
        {
            CUser* u = m_slotsUser[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)5);
            goto CF_END;
        }
        else if ((m_slotsUser[i])->getCurCharacMoney() <
                 G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand])
        {
            CUser* u = m_slotsUser[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)8);
            goto CF_END;
        }
CF_END:
        ;
CF_ISLAND:
        ;
    }
}

void WarRoom::CalcGuildFP()
{
    // [b28d-solo-WarRoom-4] ORIG 0x086bbea6：**165 orig / 164 our（gap +1，原 168/-3）**。
    //   本轮修两处（探针 /tmp/wr4/a.cpp）：
    //   ① 数组槽序翻正：ORIG local_4c[12]@-0x48..-0x3d（深）+ local_40[6]@-0x3c..-0x25；
    //      4.4 给**带初始化列表的数组**优先深槽 ⇒ 交换声明序（local_40 先、local_4c 后）
    //      = a2 命中 ORIG（a1=旧形）。
    //   ② 商只算一次：ORIG 086bc043 `mov %eax,-0x14(%ebp)` 存 local_18、086bc075
    //      循环内 `mov -0x14(%ebp),%edx` 复用（Ghidra 把复用显示成第二次除法=假象）
    //      ⇒ 源码 `m_slotFp[local_10] = local_18;`，消去重复 idiv（-5 条）。
    //   残余（N 族，均附 ORIG 指令形）：
    //   · 12 字节字节清零合并：ORIG `movl $0,-0x42; movw $0,-0x3e` / `-0x48/-0x44`
    //     （086bbed0/086bbfaa），4.4 恒 6×movb（探针 /tmp/wr4/s.cpp g1/g2）⇒ N78/N202 族。
    //   · local_4c[i+6] 的 disp 折叠：ORIG `movb $1,-0x42(%ebp,%eax,1)`，4.4 恒
    //     `add $0x6,%eax` + `-0x48(%ebp,%eax,1)` ⇒ N94 族（改 struct 视图亦为 add 形）。
    //   · ORIG 循环尾 nop 中继岛（086bbf20/086bc023/086bc07f 等 6 处 jmp+nop），4.4 恒
    //     jump-thread ⇒ P223 单源负例。语义正确（p167 CLEAR 2/2 调用点）⇒ N 封顶。
    int local_40[6] = {0};
    char local_4c[12];
    int local_28;
    int local_24;
    int local_20;
    int local_1c;
    int local_18;
    int local_14;
    int local_10;
    local_4c[6] = 0;
    local_4c[7] = 0;
    local_4c[8] = 0;
    local_4c[9] = 0;
    local_4c[10] = 0;
    local_4c[11] = 0;
    for (local_28 = 0; local_28 <= 5; ++local_28)
    {
        if (m_slots[local_28] != 0)
        {
            m_slotFp[local_28] =
                (m_slotsUser[local_28])->GetFinishPointTotal();
            local_40[local_28] =
                (m_slotsUser[local_28])->get_charac_guildkey();
            if (local_40[local_28] == 0)
                local_4c[local_28 + 6] = 1;
        }
    }
    for (local_24 = 0; local_24 <= 5; ++local_24)
    {
        if ((m_slots[local_24] != 0) && (local_4c[local_24 + 6] == 0))
        {
            local_20 = m_slotFp[local_24];
            local_1c = 1;
            local_4c[local_24 + 6] = 1;
            local_4c[0] = 0;
            local_4c[1] = 0;
            local_4c[2] = 0;
            local_4c[3] = 0;
            local_4c[4] = 0;
            local_4c[5] = 0;
            local_4c[local_24] = 1;
            for (local_14 = local_24 + 1; local_14 <= 5; ++local_14)
            {
                if ((m_slots[local_14] != 0) && (local_4c[local_14 + 6] == 0) &&
                    (local_40[local_24] == local_40[local_14]))
                {
                    local_20 += m_slotFp[local_14];
                    local_1c += 1;
                    local_4c[local_14 + 6] = 1;
                    local_4c[local_14] = 1;
                }
            }
            if (local_1c > 1)
            {
                local_18 = local_20 / local_1c;
                for (local_10 = 0; local_10 <= 5; ++local_10)
                {
                    if ((m_slots[local_10] != 0) && (local_4c[local_10] != 0))
                        // [b28d-solo-WarRoom-4] ORIG 086bc043 商只算一次存 local_18
                        //   （-0x14 槽），086bc075 循环内 `mov -0x14(%ebp),%edx` 复用
                        //   ⇒ 原码写具名 local_18，非重复 `local_20/local_1c`（Ghidra
                        //   反编译把复用显示成第二次除法，属假象）。
                        m_slotFp[local_10] = local_18;
                }
            }
        }
    }
}

void WarRoom::SendGuildFP()
{
    // [SweepWarRoom] NEAR | ORIG 0x086bc0b0 | 本轮三循环岛形修复后 271=271（gap 0），
    //   TU 判词由 DIFF 升 NEAR（identical_ae 113/242→113/242、near 1→2、diff 11→10）。
    //   ①第一循环：slot==0 独立 goto nop 岛 + body 尾 goto（ORIG 086bc110 je→086bc150
    //     nop、086bc13b je→086bc151、086bc14e jmp→086bc151）；
    //   ②第二循环：slot==0 goto nop 岛（ORIG 086bc1a6 je→086bc235）+ 两分支体尾 jmp；
    //   ③第三循环：三守卫三岛 goto 串（ORIG 086bc31c je→086bc369、086bc328 jne→
    //     086bc36c、086bc347 je→086bc36d；P223 多岛形）。
    //   [b28d-solo-WarRoom-4] 三标量槽序翻正 ⇒ **271=271 AE（离开 NEAR）**：
    //   ORIG i1@-0x20 / minFp@-0x1c / count@-0x18，原重建 i1@-0x1c / minFp@-0x18 /
    //   count@-0x20（块作用域致 4.4 把函数级 count 先占深槽）。判据⑩（全头部声明）
    //   必须按 **ORIG 槽深序 = 声明序**（i1, minFp, count）落地；三者全部提至函数头，
    //   块内仅赋值，且原 `int minFp = 0x7fffffff;` / `int count = 0;` 的声明位置
    //   改为独立赋值语句（保持 ORIG 指令序 086bc179 / 086bc2ea）。其余槽
    //   （-0x44 vector / -0x48 i2 / -0x4c it / -0x30 packet / -0x14 bestFp / -0x10 i3）
    //   两侧本就逐值相同。
    PacketGuard packet;
    packet.put_header(0, 0x60);
    packet.put_byte(GetWaiterCount());
    // [判据⑩ 槽序反转律 · b28d-solo-WarRoom-4] ORIG 槽：i1@-0x20(minFp -0x1c / count
    //   -0x18)；4.4 受块作用域影响把函数级 count 先占深槽 ⇒ 全头部声明按 ORIG 槽深序。
    int i1;
    int minFp;
    int count;
    for (i1 = 0; i1 <= 5; ++i1)
    {
        if (m_slots[i1] == 0)
            goto FP_NOP;
        // ORIG 086bc13b/086bc14e/086bc150：cond 假 je→086bc151(incr)、body 尾
        //   jmp→086bc151、slot==0 je→086bc150(nop 岛)；goto 尾标签串复现。
        if ((m_slotsUser[i1])->GetFinishPointTotal() <
            G_CDataManager()->m_warAreaCounter.m_kickPoint)
        {
            m_slotData[i1] = 9;
        }
        goto FP_END;
    FP_NOP:
        ;
    FP_END:
        ;
    }
    if (GetWaiterCount() > 4)
    {
        minFp = 0x7fffffff;
        std::vector<int> v;
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] == 0)
                goto V_NOP;
            // ORIG 086bc1a6 je→086bc235(nop 岛)；两分支体尾 jmp→086bc236(incr)。
            if ((m_slotsUser[i])->GetFinishPointTotal() < minFp)
            {
                // b28d-lane25：P167 差集实证 ORIG（0x086bc1bc/0x086bc1db/0x086bc212）
                // 在比较与两分支内各自重新调用 GetFinishPointTotal（共 3 次），不缓存。
                minFp = (m_slotsUser[i])->GetFinishPointTotal();
                v.clear();
                v.push_back(i);
            }
            else if ((m_slotsUser[i])->GetFinishPointTotal() == minFp)
            {
                v.push_back(i);
            }
            goto V_END;
        V_NOP:
            ;
        V_END:
            ;
        }
        for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
            m_slotData[*it] = 10;
        }
    }
    char local_3a[6] = {0};
    count = 0;
    while (count < GetWaiterCount())
    {
        int bestSlot = -1;
        int bestFp = -1;
        for (int i = 0; i <= 5; ++i)
        {
            // ORIG 086bc31c/086bc328/086bc347：三守卫三岛（je→086bc369 nop、
            //   jne→086bc36c nop、je→086bc36d incr），P223 多岛形复现。
            if (m_slots[i] == 0)
                goto B_NOP1;
            if (local_3a[i] != 0)
                goto B_NOP2;
            if (bestFp <= (m_slotsUser[i])->GetFinishPointTotal())
            {
                bestFp = (m_slotsUser[i])->GetFinishPointTotal();
                bestSlot = i;
            }
            goto B_END;
        B_NOP1:
            ;
        B_NOP2:
            ;
        B_END:
            ;
        }
        if (bestSlot == -1)
        {
            // [b28d-solo-WarRoom P97/P244] ORIG 0x086bc3xx：vararg 从具名局部槽
            //   -0x14(%ebp) 装载（`mov -0x14(%ebp),%eax; mov %eax,0x14(%esp)`），
            //   即实参来源是 bestSlot（此处恒 -1），非字面量 0xffffffff。
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x430,
                                  "maxPlayer (%d)", bestSlot);
        }
        local_3a[bestSlot] = 1;
        packet.put_byte(bestSlot);
        packet.put_byte(m_slotData[bestSlot]);
        ++count;
    }
    packet.finalize(true);
    SendToRoom(packet);
}

// [§6.2 核销 · b25] WarRoom::ClearReward（ORIG 0x086bc5fe，0x915B）：
//   P167 调用点差集 58/58 归零（0 MISSING / 0 EXTRA，新鲜 .o R_386_PC32 逐
//   符号 Counter 与 ORIG call 多重集全等）；ORIG insns 565 vs 新鲜 .o 566
//   （gap +1）。b25 回扫真修：补 avatarExp/creatureExp 两段 baseExp×rate
//   完整乘算赋值（ORIG 0x086bc398/0x086bc461 fildll+fmuls+fistpll），原重建
//   缺失该两段、仅剩 max(·,1) 比较 —— 奖励值语义真 bug 已修。残余
//   byte 层属 N82 帧槽平移 + N76/N18a store 装载步序 + GetRepeatEvent 链寄存器
//   物化；语义与 ORIG 一致（逐段注释见函数体）⇒ 判「语义完成、byte 层 N 封顶」。
__attribute__((target("arch=i486")))
void WarRoom::ClearReward()
{
    // [b28d-solo-WarRoom] DIFF(504) | ORIG 0x086bc5fe | 复核 b25 判词：P167
    //   58/58 归零；b25 已补 avatarExp/creatureExp 乘算真 bug；残余为 N82 帧槽
    //   平移 + N76/N18a store 装载序 ⇒ 语义完成、byte 层 N 封顶。
    unsigned int maxRef[2];
    int totalExp;
    int idx1;
    int idx2;
    totalExp = 0;
    for (idx1 = 0; idx1 < 4; ++idx1)
    {
        totalExp += (int)(m_roleKillCnt[idx1] *
                          (float)m_pDungeon->m_warRoomData.m_9c[idx1]);
    }
    for (idx2 = 0; idx2 < 6; ++idx2)
    {
        if (m_slots[idx2] != 0)
        {
            PacketGuard packet1;
            PacketGuard packet2;
            int sp = 0;
            int sfp = 0;
            float rate = m_pDungeon->m_warRoomData.m_ac;
            int fp = (m_slotsUser[idx2])->GetFinishPointTotal();
            float finishPointExp = (float)fp * rate;
            unsigned int baseExp = (unsigned int)(long long)(
                (finishPointExp + 1.0f) * (float)totalExp);
            float avatarRate = ((m_slotsUser[idx2])->IsEquipAvatar() != 0) ? 0.02f : 0.0f;
            float creatureRate =
                ((m_slotsUser[idx2])->getCurCharacInvenR()->GetCreatureMgrR()
                         ->IsEquippedCreature() != 0)
                    ? 0.05f
                    : 0.0f;
            int channelExp = 0;
            int serverExp = 0;
            unsigned int avatarExp = 0;
            int eventExp = 0;
            unsigned int creatureExp = 0;
            int premiumExp = 0;
            float expFactor = 0.0f;
            if (((CExpDoubleEvent*)GlobalData::s_event_manager->GetRepeatEvent(3))
                    ->IsEventing(0))
            {
                expFactor =
                    (float)((CExpDoubleEvent*)GlobalData::s_event_manager->GetRepeatEvent(3))
                        ->GetExpFactor() /
                    100.0f;
            }
            void* channel = G_CDataManager()->GetChannelScript();
            // 渠道组字节：ORIG movzbl G_CEnvironment()+0x378 低字节
            //             = Server_Envir::m_serverGroup(+0x1e0)
            float bonus = ((channel_script_t*)channel)->getBonusRate(
                (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                (unsigned int)G_CEnvironment()->get_channel_no(),
                (unsigned int)m_pDungeon->get_index());
            channelExp = (int)((float)(unsigned long long)baseExp * bonus);
            float serverRate = 0.0f;
            serverRate = (float)CServerEvent::GetExpRate() + serverRate;
            serverRate = serverRate / 100.0f;
            serverExp = (int)(long long)(
                (float)(unsigned long long)baseExp * serverRate);
            // ORIG 0x086bc398/0x086bc461：avatar/creature 奖励先按 baseExp×rate
            // 完整乘算落槽（fildll+fmuls+fistpll），再在 rate!=0 时 max(·,1)。
            // 原重建缺两段乘法赋值（仅剩 max 比较），b25 §6.2 回扫补齐。
            avatarExp = (unsigned int)(long long)(
                (float)(unsigned long long)baseExp * avatarRate);
            if (avatarRate != 0.0)
            {
                maxRef[0] = 1;
                avatarExp = std::max<unsigned int>(avatarExp, maxRef[0]);
            }
            eventExp = (int)(long long)(
                (float)(unsigned long long)baseExp * expFactor);
            creatureExp = (unsigned int)(long long)(
                (float)(unsigned long long)baseExp * creatureRate);
            if (creatureRate != 0.0)
            {
                maxRef[1] = 1;
                creatureExp = std::max<unsigned int>(creatureExp, maxRef[1]);
            }
            if ((m_slotsUser[idx2])->IsHavePremiumAdvantage() != 0)
            {
                float premiumRate = 0.0f;
                premiumRate =
                    (float)((m_slotsUser[idx2])->GetPremiumInfo()->GetAdvantageExpRate()) /
                    100.0f;
                premiumExp = (int)(long long)(
                    (float)(unsigned long long)baseExp * premiumRate);
            }
            int burningBonus = 0;
            CEventBase* burning = GlobalData::s_event_manager->GetRepeatEvent(0xf);
            if (((CBurningFatigueEvent*)burning)->IsEventing(0))
            {
                int rate2 = ((CBurningFatigueEvent*)burning)->getBonusExpRate(
                    (m_slotsUser[idx2])->getCurCharacUsedFatigue());
                if (rate2 != 0)
                    burningBonus = (rate2 * baseExp) / 100;
            }
            int total = avatarExp + baseExp + premiumExp + serverExp + channelExp +
                        burningBonus + creatureExp;
            int upperExp = (m_slotsUser[idx2])->gainExpAsUpperMember(baseExp);
            total += upperExp;
            int guildExp = (m_slotsUser[idx2])->gainGuildSkillExp(baseExp);
            total += guildExp;
            if ((m_slotsUser[idx2])->gain_exp_sp(total, sp, sfp,
                                                  (CUser::eExpAddReason)6, 0, 0) != 0)
            {
                packet2.clear();
                packet2.put_header(0, 2);
                packet2.put_byte(1);
                packet2.put_short(1);
                (m_slotsUser[idx2])->make_basic_info((char*)&packet2, 1);
                packet2.finalize(true);
                (m_slotsUser[idx2])->Send(packet2);
            }
            packet1.clear();
            packet1.put_header(0, 0x5c);
            packet1.put_byte(sp);
            packet1.put_byte(sfp);
            packet1.put_int((m_slotsUser[idx2])->GetFinishPointTotal());
            packet1.put_int(baseExp + serverExp + burningBonus);
            packet1.put_int(avatarExp);
            packet1.put_int(premiumExp);
            packet1.put_int(channelExp);
            packet1.put_int(upperExp);
            packet1.put_int(creatureExp);
            packet1.finalize(true);
            (m_slotsUser[idx2])->Send(packet1);
            int cur = GlobalData::s_systemTime_.getCurSec();
            (m_slotsUser[idx2])->getHackAnalyzer()->setLastMonsterDeadTime(cur);
        }
    }
}

int WarRoom::Join(CUser* user, int& seat)
{
    int count = GetWaiterCount();
    PacketGuard packet1;
    PacketGuard packet2;
    packet1.put_header(0, 2);
    packet2.put_header(0, 0xb);
    packet1.put_byte(1);
    packet1.put_short(count);
    packet2.put_byte(count);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            (m_slotsUser[i])->make_basic_info((char*)&packet1, 1);
            packet2.put_short((m_slotsUser[i])->get_unique_id());
            packet2.put_int((m_slotsUser[i])->m_network.get_inner_ip());
            packet2.put_int((m_slotsUser[i])->m_network.get_ip());
            packet2.put_short((m_slotsUser[i])->m_network.get_port());
            packet2.put_int((m_slotsUser[i])->get_acc_id());
            packet2.put_byte((m_slotsUser[i])->m_network.get_nat_type());
            packet2.put_int((m_slotsUser[i])->m_network.get_mtu());
            // ORIG 0x86bf2xx：setne+je ⇒ `!= 0` 真路径 fall-through 到 stamina。
            if ((m_slotsUser[i])->getCurCharacR() != 0)
            {
                packet2.put_byte((m_slotsUser[i])->getCurCharacR()->m_stamina);
            }
            else
            {
                packet2.put_byte(0);
            }
        }
    }
    packet1.finalize(true);
    packet2.finalize(true);
    int slot = -1;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] == 0) && (m_slotState[i] == 0xff))
        {
            slot = i;
            m_slots[i] = user;
            user->SetWarRoomIndex((short)m_index);
            seat = i;
            break;
        }
    }
    if (slot == -1)
    {
        return 4;
    }
    user->Send(packet1);
    user->Send(packet2);
    if (m_pDungeon != 0)
    {
        user->m_historyLog.EnterDungeon(m_pDungeon->GetDungeonName(),
                                        (unsigned char)m_levelBand);
    }
    // ORIG 0x86bf3xx（EnterDungeon 后 call 之后）：`add $0x50` 槽写 0 = m_slotState[slot]=0，
    // 位于 m_liveFlag 置 1 之前；原重建缺失此具名赋值。
    m_slotState[slot] = 0;
    m_liveFlag[slot] = 1;
    packet1.clear();
    packet1.put_header(0, 2);
    packet1.put_byte(1);
    packet1.put_short(1);
    user->make_basic_info((char*)&packet1, 1);
    packet1.finalize(true);
    SendToRoom(packet1);
    packet2.clear();
    packet2.put_header(0, 0xb);
    packet2.put_byte(1);
    packet2.put_short(user->get_unique_id());
    packet2.put_int(user->m_network.get_inner_ip());
    packet2.put_int(user->m_network.get_ip());
    packet2.put_short(user->m_network.get_port());
    packet2.put_int(user->get_acc_id());
    packet2.put_byte(user->m_network.get_nat_type());
    packet2.put_int(user->m_network.get_mtu());
    // ORIG 同型 setne ⇒ != 0。
    if (user->getCurCharacR() != 0)
    {
        packet2.put_byte(user->getCurCharacR()->m_stamina);
    }
    else
    {
        packet2.put_byte(0);
    }
    packet2.finalize(true);
    SendToRoom(packet2);
    packet1.clear();
    packet1.put_header(1, 0x5d);
    packet1.put_byte(1);
    packet1.put_int(user->getCurCharacMoney());
    packet1.put_short(GlobalData::s_systemTime_.getCurSec() - m_fpBase);
    packet1.finalize(true);
    user->Send(packet1);
    return 0;
}

void WarRoom::SetCharacterLive(int seat, CUser* user, bool live, bool a, short b, int c)
{
    // ORIG 086be8c8：装载序 seat→this→live(movzbl)；4.4 sched 恒把 live 转换
    // 提前到 this 前（显式 uchar cast 实测不改形）⇒ N 类装载序，仅 1 行差。
    this->m_liveFlag[seat] = live;
    PacketGuard packet;
    packet.put_header(0, 0x5f);
    packet.put_byte(GetUserSlot(user));
    packet.put_byte(live);
    packet.put_byte(a);
    // ORIG 0x86be94d：外层条件反相——true 路径 je 到函数尾共享 put(0) 块，
    // else 链 fall-through；内层 `m_flag12 != 0` then=put(0) 在前、d4 为 je 目标。
    if (!(((c == 0xff) || (c < 0)) || (c > 5) || (m_slots[c] == 0)))
    {
        if (m_flag12 != 0)
        {
            packet.put_short(0);
        }
        else
        {
            packet.put_short(m_pDungeon->m_warRoomData.m_d4);
        }
    }
    else
    {
        packet.put_short(0);
    }
    packet.put_short(b);
    packet.finalize(true);
    SendToRoom(packet);
}

int WarRoom::SetSlotState(CUser* user, int seat, ENUM_SEAT_STATE state, CUser** out,
                          bool& a, bool& b, bool& c)
{
    // [b28d-solo-WarRoom] AE | ORIG 0x086bd0f8 | P235 union 具型视角
    //   （m_slotsUser）消 `(CUser*)m_slots[]` cast 的二段装载后逐条全等。
    if (user == 0)
        return 0;
    // [b28BM13 P209] ORIG 0x086bd0f8 形 = `mov 0x14(%ebp),%eax; cmp $0xff,%eax`
    // （无符号比较先载寄存器）；enum 直比 4.4 折 cmpl mem。
    // [b28BM13 P222] 正向包裹：ORIG 不等臂 jne 直达共享尾 return-0 块。
    if (static_cast<unsigned int>(state) == 0xffu)
    {
        if (m_slots[seat] == 0)
        {
            // [b28BM13] ORIG 0x086bd12e：trace 块尾独立物化 `mov $0,%eax; jmp
            // 共享 epilogue`（显式 return 触发，块内无 store）。
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x590, 5);
            trace("WARAREA  WarRoom::SetSlotState players_[pos] is NULL pos(%d)", seat);
            return 0;
        }
        else
        {
            // [b28BM13] ORIG 0x086bd17e 单段直落 %edx；4.4 恒 eax 落值+mov
            // 保活两段（N18a；void** 无 cast 探针单段但 mangled 冻结 CUser**，
            // 语句表达式/memcpy 视图均不可翻，探针 bm13_p3/p5）。
            *out = m_slotsUser[seat];
            m_slotState[seat] = 0xff;
            // [b28BM13] ORIG 0x086bd1a1 `movl $0x0,0xc(%eax,%edx,4)` 实证清零。
            m_slots[seat] = 0;
            m_liveFlag[seat] = 0;
            a = true;
            if (GetState() > 2)
            {
                AddBanList(user);
            }
            user->getHackAnalyzer()->reportHackInfo();
            user->getHackAnalyzer()->resetHackInfo();
            if (m_pDungeon != 0)
            {
                user->m_historyLog.LeaveDungeon(m_pDungeon->GetDungeonName(),
                                                (unsigned char)m_levelBand);
            }
        }
    }
    return 0;
}

void WarRoom::HandleSetSeatState(CUser* user, int seat, ENUM_SEAT_STATE state)
{
    PacketGuard packet;
    int ret = 0;
    CUser* outUser = 0;
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    ret = SetSlotState(user, seat, state, &outUser, b1, b2, b3);
    if (ret == 0)
    {
        if (outUser != 0)
        {
            G_GameWorld()->out_from_warroom(outUser);
        }
        if (b1 != 0)
        {
            packet.clear();
            MakeSlotInfo((char*)&packet, seat);
            G_GameWorld()->send_all(packet);
        }
        if (b3 != 0)
            return;
    }
    if (ret > 0)
    {
        packet.put_header(1, 0x5e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
    }
}

int WarRoom::HandleDieCharacer(CUser* user, int killerSlot)
{
    // [SweepWarRoom] DIFF | ORIG 0x086bd87e | 本轮四修后 gap 2→1（our 324 / orig 325）：
    //   ①killerUid short→unsigned short（ORIG 086bd9ec movw $0xffff + movzwl ⇒ P11，
    //     旧形 movswl 符号扩展）；
    //   ②m_slots[killerSlot] 分支反写为正极性 then=取 uid（ORIG 086bd9a1 je→086bd9a9
    //     cMyTrace 块在 else 之后，P222/P272）；
    //   ③m_flag12 分支反写（ORIG 086bda64 je→086bda96 ⇒ then=「!=0 → 0 参数」）；
    //   ④两处 cMyTrace 改全表达式临时链（P221/P227；trace1 槽 -0x28→-0x38 与 ORIG
    //     深→浅方向一致）。
    // [SweepWarRoom-2 本轮三修，our 324→325 = orig 325，gap 0]：
    //   ⑤删 `int fp` 具名局部改内联比较（P0：ORIG 086bdc16 结果直入 %ebx 无槽），
    //     比较极性改 `fp <= penalty`（ORIG `cmp %eax,%ebx; setle`），then=
    //     DecreaseWarPoint(0) 走 fall-through、else=penalty 段在 je 目标
    //     （086bdc47 je→086bdc68），槽 -0xc 循环变量/循环尾全部对齐；
    //   ⑥m_flag10 分支极性反转 `if (m_flag10 != 0) {ReviveAll…} else {GetLiveCount…}`
    //     （ORIG 086bdd2c GetLiveCount 段在函数最末、ReviveAll 直落，P194）；
    //   ⑦循环尾双源 goto 岛（ORIG 086bdc09 je→086bdcc4 nop 岛 + 086bdcc2 jmp
    //     →086bdcc5，P223 多岛细则）⇒ +0x446 nop 岛与 +0x447 addl 复现。
    //   残余 = game_func_report 切片仅 .o 未解析重定位显示差（call 目标/字面量
    //   地址，N124/N132）；指令数 325=325、全部 jcc 目标下标逐位一致。
    if ((GetState() != 3) && (GetState() != 4) && (m_state != 5))
        return 0x13;
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    if (m_liveFlag[slot] == 0)
        return 0x12;
    if ((killerSlot != 0xff) && ((killerSlot < 0) || (killerSlot > 5)))
    {
        // P221/P227：全表达式临时链复现 ORIG 槽向（先出现者占更深槽 -0x34）。
        cMyTrace(__PRETTY_FUNCTION__, 0x6d0, 5)(
            "WarRoom::HandleDieCharacer killerSlotIndex Error(%d)", killerSlot);
        return 1;
    }
    // ORIG 086bd9ec movw $0xffff,-0xe + 086bda0x movzwl ⇒ unsigned short（P11）。
    unsigned short killerUid = 0xffff;
    if (killerSlot != 0xff)
    {
        // ORIG 086bd9a1 test/je→086bd9a9(cMyTrace 块，位于 else 块之后)：
        // 正极性 then=取 uid、else=cMyTrace 才能复现 4.1 块序（P222/P272）。
        if (m_slots[killerSlot] != 0)
        {
            killerUid = (m_slotsUser[killerSlot])->get_unique_id();
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x6de, 5)(
                "WARAREA WarRoom::HandleDieCharacer player_[] is NULL killerSlotIndex(%d)",
                killerSlot);
            killerUid = -1;
        }
    }
    SetCharacterLive(slot, user, false, false, killerUid, killerSlot);
    if ((killerSlot != 0xff) && (killerSlot >= 0) && (killerSlot <= 5) &&
        (m_slots[killerSlot] != 0))
    {
        // ORIG 086bda64 je→086bda96（else 块在 fall-through 之后）⇒ 源码为
        // `if (m_flag12 != 0) {0 参数} else {m_d4}`（then 在前）。
        if (m_flag12 != 0)
        {
            (m_slotsUser[killerSlot])->WarAreaKill(4, 0, 1.0f);
        }
        else
        {
            (m_slotsUser[killerSlot])->WarAreaKill(
                4, m_pDungeon->m_warRoomData.m_d4, 1.0f);
        }
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x5f);
        packet.put_byte(1);
        packet.put_byte(killerSlot);
        packet.finalize(true);
        SendToRoom(packet);
    }
    // [SweepWarRoom P194/P222] ORIG 086bdd2c（GetLiveCount 段）位于函数最末、
    //   ReviveAll 段走 fall-through（086bdb72 之后）⇒ 源码极性为
    //   `if (m_flag10 != 0) {ReviveAll…} else {GetLiveCount…}`（4.4 对正极性
    //   then 恒 fall-through，与 4.1 的块序一致）。
    if (m_flag10 != 0)
    {
        ReviveAll();   // ORIG 0x086bdbb2：m_flag10!=0 时先全员复活
        if ((m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50) ||
            (m_flag11 != 0))
        {
            return 0;
        }
        PacketGuard packet;
        packet.put_header(0, 0x156);
        packet.put_byte(GetWaiterCount());
        for (int i = 0; i <= 5; ++i)
        {
            // [SweepWarRoom P223] ORIG 086bdc09 `je 086bdcc4`（nop 岛）+
            //   086bdcc2 `jmp 086bdcc5`（体尾岛）⇒ 双源 goto 到连续尾标签。
            if (m_slots[i] == 0)
                goto DIE_NEXT;
            {
                // [SweepWarRoom P0] ORIG 086bdc16..086bdc30：GetFinishPointTotal 结果
                //   直入 %ebx（无槽往返）⇒ 原码无 fp 具名局部；比较为
                //   `cmp %eax,%ebx; setle`（fp<=penalty），then=DecreaseWarPoint(0)
                //   走 fall-through（ORIG 086bdc40 段），else=penalty 段在 je 目标。
                if ((m_slotsUser[i])->GetFinishPointTotal() <=
                    G_CDataManager()->m_warAreaCounter.m_wpPenalty)
                {
                    (m_slotsUser[i])->DecreaseWarPoint(0);
                }
                else
                {
                    (m_slotsUser[i])->DecreaseWarPoint(
                        G_CDataManager()->m_warAreaCounter.m_wpPenalty);
                }
                packet.put_byte(i);
                packet.put_int((m_slotsUser[i])->GetFinishPointTotal());
            }
            goto DIE_NEXT;
        DIE_NEXT:
            ;
        }
        packet.finalize(true);
        SendToRoom(packet);
        m_flag11 = 1;
    }
    else
    {
        if (GetLiveCount() == 0)
            SendWarRoomFailMsg();
    }
    return 0;
}

__attribute__((target("arch=i486")))
bool WarRoom::HandleMonsterKill(CUser* user, int monsterIdx, CUser* killer)   // [b28d-Main·P292] int→bool
{
    // [SweepWarRoom] DIFF | ORIG 0x086be1fe | 本轮六修：
    //   ①删 result 局部改早返回（ORIG 结果走 %ebx：086be236/086be2fd mov $0,%ebx、
    //     086be852 mov $1,%ebx；尾部 086be8a7 mov %ebx,%eax）；
    //   ②第一循环三独立守卫 goto 岛（ORIG 086be25f je→nop 岛 086be2ac；
    //     086be265/086be277 je/jne→共享尾 086be2ad；body 尾 086be2aa jmp 086be2ad）；
    //   ③CheckQuestMonster 第 3 实参具名 int（ORIG 086be3af movl $1,-0x10 + 086be3cd
    //     mov -0x10,%edx 回读）；
    //   ④exp 乘算书写序 monster.m_field18 在左（ORIG 086be321 flds 先、086be410 fildl 后）
    //     + 具名 float 局部 (float)(unsigned int)exp（ORIG 086be469 fildll→086be46f fstps，
    //     零扩展 mov $0,%edx 证源为 unsigned 转换）；
    //   ⑤total 段书写序 (float)total 在左（ORIG 086be4f4 fildl total 先、086be500 fildl
    //     m_add 后）；
    //   ⑥删 m_slotsUser[i] 指针缓存（P183 地址重物化；ORIG 086be75f/086be78b/086be79e/
    //     086be7c7 每点重算 slot 地址）。
    //   结果：gap 26→3（our 442 / orig 445），槽号全对齐（-0x3a8/-0x4c/-0x40/-0x3c/
    //   -0x34/-0x30/-0x29/-0x28/-0x24/-0x10/-0x54/-0x3ac/-0x3b8/-0x3bc 逐值一致）。
    //   [SweepWarRoom-2] ⑦CheckQuestMonster 第 3 实参局部 int→ENUM_QUEST_ENEMY_TYPE
    //   同型化并删调用点 cast（P41 变体/P248 档①）⇒ 消掉 4.4 提前升格的多余
    //   `mov -0x10(%ebp),%esi`，实参装载序与 ORIG 逐条一致（086be3af movl $1 存槽
    //   → 086be3cd get_index call 后 mov -0x10,%edx 回读）；切片 diff 行 255→251，
    //   our 442 不变（该条与别处 1 条抵消）。
    //   残余 3 = 4.4 强度归约：gain_exp_sp 累加段 ORIG 11 条（mov/add 寄存器目标）
    //   vs 4.4 8 条（lea 合并 + mem-add）；探针 h1（全局）/h20（实参位）证 4.4 对
    //   局部/形参恒 lea+mem-add、源码不可择 ⇒ N86/N1 封顶。
    map_monster monster;
    PacketGuard packet;
    int count = GetWaiterCount();
    if (count == 0)
        return false;
    int wp = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] == 0)
            goto WP_NOP;
        if ((killer != 0) && (m_slotsUser[i] == killer))
        {
            wp = m_field.HandleWpPerMonster(killer, monsterIdx, 0, monster);
        }
        goto WP_END;
    WP_NOP:
        ;
    WP_END:
        ;
    }
    if (m_field.HandleMonsterKill(monsterIdx, packet, monster, killer,
                                  m_pDungeon) != 1)
        return false;
    m_roleKillCnt[monster.m_roleType] += monster.m_field28;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            bool isKiller = (killer != 0) && (m_slotsUser[i] == killer);
            if (isKiller)
            {
                killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                // [SweepWarRoom P41 变体] ORIG 086be3af `movl $0x1,-0x10` +
                //   086be3cd `mov -0x10(%ebp),%edx`（get_index call 之后才回读）
                //   ⇒ 局部与形参同型（enum）时不提前升格 callee-saved。
                ENUM_QUEST_ENEMY_TYPE enemyType = (ENUM_QUEST_ENEMY_TYPE)1;
                killer->CheckQuestMonster(m_pDungeon->get_index(),
                                          monster.m_mobIndex,
                                          enemyType);
            }
            int exp = (int)(long long)(
                monster.m_field18 *
                (float)m_pDungeon->m_warRoomData.m_9c[monster.m_roleType]);
            if (isKiller)
            {
                exp = (int)(long long)(
                    (float)(unsigned int)exp *
                    G_CDataManager()->m_warAreaCounter.m_monsterExpConst);
            }
            ExpBonusPointResult bonus;
            if (isKiller)
            {
                killer->checkBonusPoint();
                killer->getExpBonusPoint(bonus);
            }
            int total = bonus.m_base + exp;
            exp = (int)((float)total * ((float)bonus.m_add / 100.0f)) + total;
            int upperExp = (m_slotsUser[i])->gainExpAsUpperMember(exp);
            int guildExp = (m_slotsUser[i])->gainGuildSkillExp(exp);
            int fatigueExp = 0;
            if ((m_slotsUser[i])->getCurCharacFatigueGrownUpBuff() != 0)
            {
                fatigueExp = exp;
                // ORIG 086be5bb：实参从 -0x18 槽回读（fatigueExp），非 -0x28(exp)。
                (m_slotsUser[i])->set_charac_fatigue_buf_bonus_exp(fatigueExp);
            }
            int rewardExp = (m_slotsUser[i])->gainPowerWarRewardExp(exp);
            int sp = 0;
            int sfp = 0;
            // ORIG 086be652/086be654：test %al,%al; je → addl（bool 直测，无 xor 物化）；
            // 空 then + else-goto 形复现该极性（`== 0` 会多一条 xor $1）。
            if ((m_slotsUser[i])->gain_exp_sp(
                    upperExp + exp + guildExp + rewardExp + fatigueExp,
                    sp, sfp, (CUser::eExpAddReason)4, monster.m_mobIndex,
                    0) != 0)
            {
            }
            else
            {
                goto NOTI_NOP;
            }
            (m_slotsUser[i])->SendNotiPacket(
                (CUser::eSendTarget)3, (ENUM_NOTIPACKET)2, 1);
            goto NOTI_END;
        NOTI_NOP:
            ;
        NOTI_END:
            ;
        }
    }
    unsigned short killerUid = 0xffff;
    if (killer != 0)
        killerUid = (unsigned short)killer->get_unique_id();
    packet.put_byte(1);
    packet.put_short(killerUid);
    packet.put_short(wp);
    packet.finalize(true);
    SendToRoom(packet);
    if (m_field.IsGridClear() != 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] == 0)
                goto FAT_NOP;
            if (m_slotsUser[i]->m_fatigueGain > 0)
            {
                if (m_slotsUser[i]->getCurCharacInvenR()->GetCreatureMgrR()
                        ->IsGrowCreature_Equipped_Creature() != 1)
                {
                    m_slotsUser[i]->getCurCharacInvenW()->GetCreatureMgrW()->GainExp(
                        m_slotsUser[i]->m_fatigueGain);
                }
            }
            m_slotsUser[i]->m_fatigueGain = 0;
            goto FAT_END;
        FAT_NOP:
            ;
        FAT_END:
            ;
        }
        GenTimerKey((TIMER_MESSAGE)0x45);
        if (m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50)
        {
            m_flag12 = 1;
            ChangeState(WARROOM_STATE_5);
        }
        else
        {
            ChangeState(WARROOM_STATE_4);
        }
    }
    return true;
}

int WarRoom::HandleWpPerMonster(CUser* user, int a, int b, CUser* killer)
{
    map_monster monster;
    int count = GetWaiterCount();
    if (count == 0)
        return 3;
    int wp = 0;
    for (int i = 0; i <= 5; ++i)
    {
        {
            // ORIG 086c05ab：slot-null je→086c065c(nop;fall 086c065d) 空岛；
            // isTarget-false je 直落 end；body 尾显式 jmp end(086c065a)。
            if (m_slots[i] == 0)
                goto WP_SLOT;
            bool isTarget = (killer != 0) && (m_slotsUser[i] == killer);
            if (isTarget)
            {
                wp = m_field.HandleWpPerMonster(killer, a, b, monster);
                killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                goto WP_JOIN;
            }
        }
WP_JOIN:
        ;
WP_SLOT:
        ;
    }
    // ORIG 0x86c0670/0x86c06c3：movw $0xffff 初始化 + movzwl 回读 ⇒ unsigned short。
    unsigned short killerUid = 0xffff;
    if (killer != 0)
        killerUid = killer->get_unique_id();
    PacketGuard packet;
    packet.put_header(0, 0x151);
    packet.put_int(a);
    packet.put_short(killerUid);
    packet.put_int(b);
    packet.put_short(wp);
    packet.finalize(true);
    SendToRoom(packet);
    return 0;
}

int WarRoom::HandleGetItem(CUser* user, int idx)
{
    // [b28d-solo-WarRoom-3] ORIG 0x086bea2e：551/-4 → **546/+1**。本轮修正：
    //   ①删 `int result` 局部（ORIG 返回值走 %ebx 公共尾：086beaf0/086bef5f
    //     `mov $0x0,%ebx`、086bf2ea `mov $0x1,%ebx`、086bf317 `mov %ebx,%eax`）
    //     ⇒ 全部改直接 return（P0：ORIG 无 result 槽）；
    //   ②`else if (GetWaiterCount() < 1)` → 具名 `int count` + 失败块直落
    //     （ORIG 086beb05 `mov %eax,-0x24(%ebp); cmpl $0,-0x24; jg 主流程`）；
    //   ③删具名 bool randomOption → 内联 `IsRandomOption() && !getCheck…`
    //     （ORIG 086bee7b..086bee9d 常量岛 + `xor $1` ⇒ 无槽；P0/P214①）。
    //   残余 +1（086bec31 段尾块序）+ 版本槽基 8（N95/P332）⇒ 封顶。
    char flag = 1;
    CUser* target = 0;
    map_item fieldItem;
    PacketGuard packet;
    bool ok = m_field.GetFieldItem(idx, fieldItem) != 0;
    if (ok != 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else
    {
        // [b28d-solo-WarRoom-3] ORIG 086beb05 `mov %eax,-0x24(%ebp); cmpl $0x0,
        //   -0x24(%ebp); jg 086beb7e` ⇒ 原码是具名 int count + 正极性 `> 0`
        //   （失败块直落、主流程后置）；原重建写 `else if (GetWaiterCount() < 1)`
        //   使 4.4 折成无槽 `test;setle;test;je`（P0/条件极性）。
        int count = GetWaiterCount();
        if (count <= 0)
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(0x15);
            packet.finalize(true);
            user->Send(packet);
            return 0;
        }
        if (fieldItem.m_item.m_addInfo == 0)
    {
        int money = fieldItem.m_item.get_add_info();
        packet.put_header(0, 0x5d);
        packet.put_short(idx);
        packet.put_short(user->get_unique_id());
        std::map<int, map_item>::iterator iter;  // ORIG 显式 _Rb_tree_iterator C1Ev（置 0）
        // [b28d-L20r4 判据⑫/P198①] ORIG 0x086bebea `movl $0x0,-0x14(%ebp)` 在
        //   CheckPickupItem 调用**之前** ⇒ 原码是预清两句式 `int check = 0;
        //   check = …;`（Ghidra C：`local_24 = 0; local_24 = CheckPickupItem(…)`）。
        int check = 0;
        check = m_field.CheckPickupItem(
            idx, user->get_unique_id(), iter);
        if (check < 1)
        {
            int gained = user->getCurCharacInvenW()->gain_money(
                money, (eMoneyAddReason)0xf, 1, 0);
            user->saveTaxMoneyForUpperMember(money);
            if (gained < money)
            {
                if (gained < 0)
                    gained = 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, money, gained);
            }
            m_field.PickupItem(iter);
            packet.put_int(gained);
            packet.finalize(true);
            SendToRoom(packet);
            return 1;
        }
        else
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(check);
            packet.finalize(true);
            user->Send(packet);
            return 0;
        }
    }
    else
    {
        int itemIdx = fieldItem.m_item.m_addInfo;
        CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__,
                                  0x957, "G_CDataManager()->find_item(%d) fail", itemIdx);
            return 0;
        }
        else
        {
            target = user;
            if (user == 0)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(0x16);
                packet.finalize(true);
                SendToRoom(packet);
                return 0;
            }
            else
            {
                // [b28d-solo-WarRoom-3] ORIG 086bee7b..086bee9d 无 bool 槽：
                //   `test %al,%al; je <mov $0>`（IsRandomOption 直判）+ 086bee8a
                //   `xor $0x1,%eax; test %al,%al; je <mov $0>`（!getCheck…）+ 常量岛
                //   + `test %al,%al; je 主流程` ⇒ 原码是内联短路 `if (A && !B)`
                //   （P0/P214①），原重建的具名 bool randomOption 多占槽 -0x11。
                if (item->IsRandomOption() && !user->getCheckPickUpRandomOptionItem())
                {
                    user->setCheckPickUpRandomOptionItem(true);
                    DB_InsertRandomOptionItemInform::makeRequest(
                        user->GetUID(), user->get_acc_id(), 1);
                    PacketGuard notice;
                    notice.clear();
                    notice.put_header(0, 0x171);
                    notice.put_short(user->get_unique_id());
                    notice.put_byte(1);
                    notice.finalize(true);
                    user->Send(notice);
                }
                std::map<int, map_item>::iterator iter;  // ORIG 显式 _Rb_tree_iterator C1Ev（置 0）
                // [b28d-L20r4 同族] ORIG 0x086bf092 `movl $0x0,-0x1c(%ebp)` 预清。
                int check = 0;
                check = m_field.CheckPickupItem(
                    idx, user->get_unique_id(), iter);
                if (check < 1)
                {
                    // [b28d-L20r4 判据⑫] ORIG 0x086bef8f `movl $0x0,-0x20(%ebp)` 预清
                    //   （Ghidra C：`local_20 = 0;` 后 if/else 两臂各赋值 —— 两臂全覆盖，
                    //   与旧写的 `= -1` 语义等价，但与 ORIG 槽/指令形一致）。
                    int added = 0;
                    if (fieldItem.m_item.isAvatarItemType() == 0)
                    {
                        added = user->getCurCharacInvenW()->insertItemIntoInventory(
                            fieldItem.m_item, (eItemAddReason)0x16, 1, 1);
                    }
                    else
                    {
                        char ipg[24];
                        WongWork::CGenUniqueNo::genIPGNo(
                            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                            user->getCurCharacNo(), ipg);
                        added = user->addAvatarItem(
                            (unsigned long)fieldItem.m_item.m_addInfo, 0,
                            (char)fieldItem.m_item.m_fieldb, ipg,
                            (eAvatarItemAddReason)0x11);
                    }
                    if (added == -1)
                    {
                        packet.put_header(1, 0x2e);
                        packet.put_byte(0);
                        packet.put_byte(4);
                        packet.finalize(true);
                        user->Send(packet);
                        return 0;
                    }
                    else
                    {
                        m_field.PickupItem(
                            iter);
                        packet.put_header(0, 0x5d);
                        packet.put_short(idx);
                        packet.put_short(user->get_unique_id());
                        packet.put_short(added);
                        packet.finalize(true);
                        SendToRoom(packet);
                        return 1;
                    }
                }
                else
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x982, 5);
                    trace("fieldData_.PickupItem( %d/%d ) ", idx, itemIdx);
                    packet.put_header(1, 0x2e);
                    packet.put_byte(0);
                    packet.put_byte(check);
                    packet.finalize(true);
                    // b28d-lane25：P167 差集实证 ORIG 0x086bec97 先 SendToRoom(packet)
                    // 再 user->Send(packet)（原实现漏 SendToRoom，语义缺口）。
                    SendToRoom(packet);
                    user->Send(packet);
                    return 0;
                }
            }
        }
    }
    }
    return 1;
}

// [b28d-solo-WarRoom-3] WarRoom::HandleItemDrop（ORIG 0x086bf5fc）：781/-5 →
//   **777/-1**。本轮修正：switch(attach) 的 case 1/2/5 由「各发一块 return 0x17」
//   改为共享 case 标签体 —— ORIG 086bfacd 只有**一个** `mov $0x17,%ebx; jmp`
//   块（P325 共享出口），原实现多 2 块 4 条。残余 1 条 = 4.4 跳表索引
//   `shl $2,%eax; mov 0x…(%eax),%eax` vs 4.1 `mov 0x8cf98b0(,%eax,4),%eax`
//   （跳表 lowering，N4/N125）⇒ 封顶；P167 100/100 CLEAR。
int WarRoom::HandleItemDrop(CUser* user, int a, int b, char c, int d, int e)
{
    // [b28d-solo-WarRoom] DIFF(504) | ORIG 0x086bf5fc | 复核 b20 判词：P167
    //   100/100 归零；残余为 N82（4.1 不按 lifetime 复用栈槽 → 帧 -0x1ec 槽号
    //   平移）+ N76/N18a store 装载序 + 4.1 switch 判定树形态 ⇒ 语义完成、
    //   byte 层 N 封顶。
    // ORIG 086bf617/086bf627：GetState() 被短路求值调用两次（&& 两侧各一次），
    // 不缓存到局部变量。
    if ((GetState() != 3) && (GetState() != 4))
        return 0x13;
    if (e < 0)
    {
        // ORIG 086bf651..086bf6a8：两个结果都作为可变参数入栈
        // （%ebx=getCurCharacName 保存在 0x18(%esp)，%eax=NumberToString 在 0x14(%esp)）。
        LogManager::logFormat(1, "WarRoom.cpp",
                              __PRETTY_FUNCTION__,
                              0xa1a, "user - accid[%s] %s : count<0",
                              NumberToString(user->get_acc_id(), 0),
                              user->getCurCharacName());
        return 0x11;
    }
    if (user->CheckInTrade() != 0)
        return 0x13;
    map_item field;
    Inven_Item item;
    // ORIG 086bf6ec：switch(c) 三分支（case 0 直落、case 2 → +0x25b、case 3 → +0x348）。
    // ORIG 086bf876/086bf96b/086bf789：按值返回直接落 sret 临时（-0x188）后
    // 逐字段赋值进 item，没有具名中转对象，也没有额外 Inven_Item::Inven_Item()。
    switch (c)
    {
    case 0:
        if (d == 0)
        {
            field.m_count = 0;
            field.m_item.m_field1 = 2;
            field.m_item.m_addInfo = 0;
            field.m_item.set_add_info(e);
            field.m_item.m_fieldb = 0;
            field.m_item.ResetItemAttr();
        }
        else
        {
            item = user->getCurCharacInvenR()->GetInvenSlot(1, d);
        }
        break;
    case 2:
        item = user->getCurCharacCargoR()->get_cargo_slot(d);
        break;
    case 3:
        item = user->getCurCharacInvenR()->GetInvenSlot(0, d);
        break;
    }
    int price = 0;
    // ORIG 086bfa3e/086bfa4c：直接重复读 item.m_addInfo（-0xd3），无 itemIdx 局部。
    if (item.m_addInfo != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        if (pItem == 0)
        {
            // ORIG 086bfa6e：item.m_addInfo 作为可变参数入 0x14(%esp)。
            LogManager::logFormat(
                1, "WarRoom.cpp",
                __PRETTY_FUNCTION__, 0xa4c,
                "[WarRoom::HandleDropItem] Null Item %d", item.m_addInfo);
            return 0xffffffff;
        }
        // [b28d-solo-WarRoom P321-T2] ORIG 0x086bfac4 范围检查用 `ja`（无符号）
        //   ⇒ attach 的消费语义是 unsigned；声明同型以复现 4.1 跳表分派。
        unsigned int attach = (unsigned int)pItem->GetAttachType();
        // [b28d-solo-WarRoom] ORIG 086bfac4 用跳表（cmp $0x5/ja +
        //   mov 0x8cf98b0(,%eax,4)/jmp *%eax；表项 0/4→默认、1/2/5→ret 0x17、
        //   3→下方判定）。翻正旋钮：switch 表达式按 T2 无符号化 + 6 个 case 全
        //   独立书写 ⇒ 4.4 由判定树改发跳表（shl+mov 形态）。残余 = 4.4 跳表前
        //   多一条范围检查（跨版本固有），语义与 ORIG 一致。
        // [b28d-solo-WarRoom-3] P325：ORIG 跳表后只有**一个** `mov $0x17,%ebx;
        //   jmp 086c03f6` 块（086bfacd，case 1/2/5 共用一个标签体），原重建每
        //   case 各发一块 ⇒ 多 2 块（4 条）。改成共享 case 标签复现单块形。
        switch (attach)
        {
        case 0:
            break;
        case 1:
        case 2:
        case 5:
            return 0x17;
        case 3:
            if (item.m_field0 != 1)
                return 0x17;
            break;
        case 4:
            break;
        }
        // ORIG 086bfaef..086bfb28：两层嵌套 if（外层直接 test %al），
        // 内层用具名 CEquipItem*（-0x1c）连测两次 +0x234。
        if (item.isEquipableItemType())
        {
            CEquipItem* pEquip = (CEquipItem*)pItem;
            if ((pEquip->m_itemType == 0xb) || (pEquip->m_itemType < 10))
            {
                return 0x17;
            }
        }
        field.m_count = 0;
        field.m_item.m_field1 = item.m_field1;
        field.m_item.m_addInfo = item.m_addInfo;
        // ORIG 086bfb56：xor $0x1 → 条件写作 !isEquipableItemType()，
        // 「非装备」分支在前（set_add_info(e)），装备分支在后。
        if (!item.isEquipableItemType())
        {
            field.m_item.set_add_info(e);
        }
        else
        {
            field.m_item.set_add_info(item.m_addInfo2);
        }
        field.m_item.SetItemAttr(item.GetItemAttr());
        field.m_item.m_fieldb = item.m_fieldb;
        field.m_item.m_field0 = item.m_field0;
        field.m_item.m_fieldd = item.m_fieldd;
        field.m_item.m_amp = item.m_amp;
        // ORIG 086bfc16..086bfc47：+0x25..+0x32 共 14 字节整体拷贝
        //（4+4+4+2），即 RandomOption 子对象整体赋值，而非逐成员。
        field.m_item.m_random.m_random = item.m_random.m_random;
        field.m_item.m_upgradeSep.SetUpgradeSeparate(
            item.m_upgradeSep.GetUpgradeSeparate());
        // ORIG 086bfc7c：直接把 &price（-0x8c）作为出参，无中转局部。
        G_Store()->GetSellItemPrice(item, pItem, (short)e, false, price);
    }
    // ORIG 086bfcab：第二个 switch(c)，判定树 cmp2/je → cmp2/jg → test(case 0)
    // → cmp3/je、cmp7/je；分支体按源码序 0、2、3、7 排布（cMyTrace 栈槽
    // -0x88/-0x78/-0x68/-0x58/-0x48 顺序即为此序）。
    switch (c)
    {
    case 0:
        if (d == 0)
        {
            if (((CInventory*)user->getCurCharacInvenW())
                    ->use_money(e, (eMoneySubReason)6, 1) != 1)
            {
                // ORIG 086bfd4e：只压入两个可变参数（%ebx=getCurCharacNo、0x20(%ebp)=e），
                // 格式串第三个 %d 在 ORIG 里本就无对应实参 —— 忠实保留。
                cMyTrace(__PRETTY_FUNCTION__, 0xa9f, 5)(
                    "CParty::drop_item , use_money failed, User ch=%d , %d %d",
                    user->getCurCharacNo(), e);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned)e);
        }
        else
        {
            if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, d, e, (eItemDelReason)6, 1) != 1)
            {
                // ORIG 086bfe11：0x10(%esp)=0x20(%ebp)=e、0xc(%esp)=0x1c(%ebp)=d。
                cMyTrace(__PRETTY_FUNCTION__, 0xaae, 5)(
                    "CParty::drop_item , delete_item failed, User ch=%d , %d %d",
                    user->getCurCharacNo(), d, e);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                          user, (unsigned)price);
        }
        break;
    case 2:
        if (user->getCurCharacCargoW()->delete_item(d, e, (eItemDelReason)0x10) != 1)
        {
            // ORIG 086bfed1：0x10(%esp)=0x20(%ebp)=e、0xc(%esp)=0x1c(%ebp)=d。
            cMyTrace(__PRETTY_FUNCTION__, 0xabf, 5)(
                "CParty::drop_item , delete_item failed 2, User ch=%d , %d %d",
                user->getCurCharacNo(), d, e);
            return 0x11;
        }
        break;
    case 3:
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)0, d, 1, (eItemDelReason)6, 1) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xaca, 5)(
                "CParty::drop_item , delete_item failed 3, User ch=%d , %d",
                user->getCurCharacNo(), d);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                     user, (unsigned)price);
        break;
    case 7:
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)3, d, 1, (eItemDelReason)6, 1) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xaed, 5)(
                "CParty::drop_item , delete_item failed 4, User ch=%d , %d",
                user->getCurCharacNo(), d);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                     user, (unsigned)price);
        break;
    }
    if (item.m_field1 != 1)
    {
        if ((field.m_item.m_addInfo != 0) && (field.m_item.get_add_info() > 0x7d0))
        {
            // ORIG 086c00cf：第三个可变参数直接读 +0x07 原始字段（-0x115），
            // 不再二次调用 get_add_info()。
            cMyTrace(__PRETTY_FUNCTION__, 0xb00, 4)(
                "war room drop item(%d) stack size over %d add_info(%d)",
                field.m_item.m_addInfo, 0x7d0, field.m_item.m_addInfo2);
        }
    }
    int dropIdx = m_field.DropItem(field);
    if (dropIdx == -1)
        return 0x16;
    PacketGuard packet;
    packet.put_header(0, 0x28);
    // ORIG 086c0198：movzwl %ax,%eax —— get_unique_id 结果按 u16 零扩展后入参。
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(dropIdx);
    packet.put_int(field.m_item.m_addInfo);
    packet.put_byte(field.m_item.GetItemAttr());
    packet.put_int(field.m_item.get_add_info());
    packet.put_short(field.m_item.m_fieldb);
    packet.put_int(field.m_item.m_fieldd);
    packet.put_byte(field.m_item.m_amp.getAbilityType());
    packet.put_short(field.m_item.m_amp.getAbilityValue());
    packet.put_packet(field.m_item);
    packet.finalize(true);
    SendToRoom(packet);
    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)c);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

int WarRoom::ReviveUserByCoin(CUser* user, unsigned short uid)
{
    // [b28d-solo-WarRoom-3] ORIG 0x086bf324：97/98 → **98/98（gap 0）**，判据①
    //   jcc 目标下标逐位全等。修法 = P223 定案形：外层失败改负条件
    //   `if (m_slots[slot] == 0) goto REVIVE_NEXT;` + 循环体尾空标签
    //   （ORIG 086bf38a `je 086bf3da(nop 岛)` 与 086bf3aa `je 086bf3db(addl)`
    //   落点不同）⇒ 4.4 不再 jump-thread，nop 中继岛逐条复现。
    //   b25 已补 target==0 → 0x15 语义块。
    // ORIG 0x86bf33c：短路 && 条件直进 if（bool 局部会 spill cmpb）。
    if ((GetState() != 3) && (GetState() != 4))
        return 0x13;
    CUser* target = 0;
    int slot;
    for (slot = 0; slot <= 5; ++slot)
    {
        // [b28d-solo-WarRoom-3] P223 定案形：ORIG 086bf38a `je 086bf3da(nop 岛)`
        // 与 086bf3aa `je 086bf3db(addl)` 落点不同 ⇒ 外层失败走**负条件 goto
        // 到循环体尾空标签**（4.4 不再 jump-thread，逐条复现 nop 中继岛）。
        if (m_slots[slot] == 0)
            goto REVIVE_NEXT;
        if ((m_slotsUser[slot])->get_unique_id() == uid)
        {
            if (m_liveFlag[slot] != 0)
                return 0x12;
            target = m_slotsUser[slot];
            // ORIG 086bf3d8 jmp→086bf3ea（break 出口）。
            goto LOOP_END;
        }
REVIVE_NEXT:
        ;
    }
LOOP_END:
    ;
    // ORIG 0x86bf3fb：target 空守卫 return 0x15（原重建缺失该语义块）。
    if (target == 0)
        return 0x15;
    if (user->GetUsedCoinCount() >= m_pDungeon->m_87c)
        return 0x16;
    HistoryLog::WriteUseCoin((_IO_FILE*)user->m_field796f8,
                             target->getCurCharacName());
    SetCharacterLive(slot, target, true, true, 0, 0xff);
    return 0;
}
// ============================================================================
// 仅ORIG 符号（本批 OrigWarRoom 转写，报告已完整阅读：
// ============================================================================
// 仅ORIG 符号（本批 OrigWarRoom 转写，报告已完整阅读：
//   docs/class_func_reports/_lt_global_gt_/EnterWarRoom.md
//   docs/class_func_reports/_lt_global_gt_/ImportWarRoomWPTable.md
//   docs/class_func_reports/DisPatcher_MakeWarRoomTemp/dispatch_sig.md）
// ============================================================================

// ---- stBloodWarGrowTable（ImportWarRoomWPTable 形参类型，mangled P19stBloodWarGrowTable；
//      派生自 vector<vector<pair<int,int>>> —— GCC 4.4 对 typedef 到模板实例会展开
//      typedef 名，唯派生结构体才能产生 ORIG 的 P19stBloodWarGrowTable）----
struct stBloodWarGrowTable
    : public std::vector<std::vector<std::pair<int, int> > >
{
};

// ============================================================================
// EnterWarRoom —— ORIG 0x08201e7a（size 0x424，T）
// 报告 _lt_global_gt_/EnterWarRoom.md 全文转写：战场房间入场入口。
// 顺序：state!=3 → 错误包(0x13)；getCurCharacVill==7 → SendCmdErrorPacket(0x13)；
// 私商忙 → 0x3c；Disjoint 店忙 → 0xbf；幽灵 → 0x13；交易中 → 0x13；
// IsJoinable 非 0 → 错误包(码)；CheckInParty → send_invite_warroom + CheckOutParty；
// Join>0 → 错误包(码)；cmd==0x65 → 广播本人 basic 包；MakeSlotInfo + send_all +
// goto_warroom；人数>2 且 state==0 → Start；返回 1/0。
// ============================================================================
int EnterWarRoom(CUser* user, WarRoom* room, ENUM_CMDPACKET cmd)
{
    // [b28d-solo-WarRoom-4] ORIG 0x08201e7a：原 NEAR（残差=packet 槽基 -0x1c vs
    //   -0x20、seat -0x20 vs -0x14）⇒ **279=279 AE（离开 NEAR）**。
    //   旋钮（探针 /tmp/wr4/p.cpp f1..f5）：ORIG 槽序 seat(-0x20)/packet(-0x1c)/
    //   ret(-0x10)/party(-0xc)；4.4 对「聚合+标量」混合帧**把标量声明在聚合之后**
    //   才给 seat 深槽 -0x20、packet -0x1c（f3 命中），seat 前置(f1)/聚合化(f2)/
    //   volatile(f4)/数组(f5) 四形均不翻。故声明序 = packet, ret, party, **seat**
    //   （seat 移至队尾）；`seat = 0;` 语句仍留在 Join 前（ORIG 086be6xx 指令序）。
    PacketGuard packet;       // -0x1c
    int ret;                  // -0x10（IsJoinable 与 Join 复用同一槽，ORIG 实证）
    CParty* party;            // -0xc
    int seat;                 // -0x20（ORIG）
    // [判据⑩ 延伸 · WarRoom-4] 探针 /tmp/wr4/p.cpp f1/f3 对照：把标量 seat 置于
    //   packet 声明之后（队尾）时 4.4 才给 seat 深槽 -0x20、packet -0x1c（与 ORIG
    //   完全一致）；seat 前置/聚合化/volatile 三形均不翻（f1/f2/f4）。
    if (user->get_state() != 3)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (user->getCurCharacVill() == 7)
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        user->SendCmdErrorPacket(cmd, 0x3c);
        return 0;
    }
    if (user->IsBusyDisjointStore())
    {
        user->SendCmdErrorPacket(cmd, 0xbf);
        return 0;
    }
    if (user->IsCurCharacGhost())
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    ret = room->IsJoinable(user);
    if (ret != 0)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (user->CheckInParty())
    {
        party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_warroom((unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    seat = 0;
    ret = room->Join(user, seat);
    if (ret > 0)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (cmd == 0x65)
    {
        packet.put_header(1, 0x65);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    room->MakeSlotInfo((char*)&packet, seat);
    G_GameWorld()->send_all(packet);
    G_GameWorld()->goto_warroom(user);
    if (room->GetWaiterCount() > 2)
    {
        if (room->GetState() == 0)
        {
            room->Start();
        }
    }
    return 1;
}

// ============================================================================
// ImportWarRoomWPTable —— ORIG 0x08902543（size 0x1af，T）
// 报告 _lt_global_gt_/ImportWarRoomWPTable.md 全文转写：载入 Etc/WarRoomWPTable.tbl。
// 失败 → logFormat(0, __FUNCTION__) 返回 0；"[WP TABLE]" 段内每行解析 5 组
// (key,val) pair，ok 时 push 到 table，继续下一行（do-while(ok)），失败回外层
// ScanType；返回 1。韩文日志与 __FUNCTION__ 字符串按 ORIG 原样（CP949 源字节）。
// ============================================================================
int ImportWarRoomWPTable(stBloodWarGrowTable* table)
{
    // [b28d-solo-WarRoom-4] ORIG 0x08902543：126/128（gap 2）⇒ **128/128（gap 0）**。
    //   本轮两旋钮（P231 + P223 多岛）：
    //   ① 循环条件变量 `register bool again` → **`register int again`** 并写 0/1
    //      （ORIG 08902669/08902688 `mov $0/$1,%ebx`、089026af 需 32 位 test）
    //      且 do-while 条件写 `again != 0` ⇒ 消 4.4 的 `setne %al; test %al,%al`
    //      两步物化，改出 ORIG 的 `test %ebx,%ebx` 直判。
    //   ② do 体后与 else 分支各发一个 goto（LOOP_TAIL / ELSE_ISLAND 物理相邻），
    //      复现 ORIG 两个不同落点（089025ad nop+089025ae jmp 与 089025b0 nop）。
    //   残余（N 封顶，附 ORIG 指令形）：ORIG 089025ab/089025ae 两处 `jmp 头` 与
    //   089025b0 nop 岛；4.4 对单源尾岛恒 jump-thread（本轮已试双标签串/删 else 直落/
    //   同层 continue 三形，P223 单源负例）⇒ 我方少 1 个 jmp+nop、并在 do-while 尾
    //   多 2 条中继。语义与 ORIG 一致（p167 CLEAR 16/16，MISSING 仅 C1/C2 镜像）。
    if (!loadRDARScriptFile("", "Etc/WarRoomWPTable.tbl"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0xdb8,
                              "CRITICAL ERROR : \xc0\xef\xbf\xaa \xc4\xab\xbf\xee\xc5\xcd \xbd\xba\xc5\xa9\xb8\xb3\xc6\xae \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return 0;
    }
    std::string token;
    bool ok = false;
    // [b28d-solo-WarRoom P283/P164] ORIG 0x0890259x 循环头为
    //   `call ScanType; xor $0x1,%eax; test %al,%al; jne <body>`（否定跳转）+
    //   两处 nop 中继岛；4.4 对 while(cond) 恒正向 `test;je` ⇒ while(true)+break 试翻。
    while (true)
    {
        if (!ScanType(token, true))
            break;
        // [b28d-solo-WarRoom P223] ORIG 0x089025e4 `je 0x089025ad`（nop; jmp 循环头）
        //   = else 占位岛；4.4 对纯正向 if 恒 jump-thread ⇒ 空 else + goto 尾空
        //   标签复现（保留 operator== 调用点，P167）。
        if (token == "[WP TABLE]")
        {
            // [b28d-solo-WarRoom P231] ORIG 0x08902669/0x08902688 两臂 `mov $0/$1,%ebx`
            //   + 0x089026af `test %ebx,%ebx; je` ⇒ 具名循环条件变量落在 %ebx
            //   （纯 register 存储类提示，无 asm 绑定，P0 合规）。
            // [b28d-solo-WarRoom P223] ORIG 循环头双岛：089025ad nop + 089025ae
            //   jmp HEAD（if 假分支落点）、089025b0 nop（do-while again==0 落点）。
            //   试 continue 形（同层 continue + 循环尾自然 fall）复现双岛。
            register int again;
            do
            {
                std::vector<std::pair<int, int> > row;
                for (int i = 0; i < 5; ++i)
                {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok)
                        break;
                    p.second = ScanInt(&ok);
                    if (!ok)
                        break;
                    row.push_back(p);
                }
                if (!ok)
                    again = 0;
                else
                {
                    if (table != 0)
                        table->push_back(row);
                    again = 1;
                }
            } while (again != 0);
            // [b28d-solo-WarRoom-4 判据④/P223 多岛] ORIG 循环尾两个物理相邻但不同
            //   地址的落点：+0x6a（operator== 假分支）与 +0x6d（do-while again==0）。
            //   两个**不同源**的 goto 跳物理连续尾标签 ⇒ 4.4 不 jump-thread（P223 可造形）。
            goto LOOP_TAIL;
        }
        else
        {
            goto ELSE_ISLAND;
        }
ELSE_ISLAND:
        ;
LOOP_TAIL:
        ;
    }
    return 1;
}

// ============================================================================
// DisPatcher_MakeWarRoomTemp 族（ORIG vtable @0x8bd8cc8，8 槽）
//   dispatch_sig 0x08201b32（报告 dispatch_sig.md 全文转写）
//   read 0x082335f8 / process 0x08233602（objdump 转写：恒定返回 0）
//   D1/D2 0x082431aa / D0 0x082431da（objdump 转写：vptr + 基类析构 + delete 守卫）
// ============================================================================
int DisPatcher_MakeWarRoomTemp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    if (user->get_state() != 3)
        return 0;
    PacketGuard pg;
    WarRoom* room = G_CGameManager()->GetWarRoom();
    int createRet;
    CParty* party;
    pg.put_header(1, 0x5c);
    if (room == 0)
    {
        pg.put_byte(0);
        pg.put_byte(4);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }
    if (user->CheckInParty())
    {
        party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_warroom((unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    createRet = room->Create(user, (char*)"임시", 0x1770);
    if (createRet > 0)
    {
        pg.put_byte(0);
        pg.put_byte(createRet);
        pg.finalize(true);
        user->Send(pg);
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__,
                              0x976d, "create_room fail: 쟁역 방 생성 실패 ID[%d]\n",
                              room->GetIndex());
        G_CGameManager()->PutWarRoom(room);
        return 0;
    }
    pg.clear();
    pg.put_header(0, 2);
    pg.put_byte(1);
    pg.put_short(1);
    user->make_basic_info((char*)&pg, 1);
    pg.finalize(true);
    user->Send(pg);
    pg.clear();
    pg.put_header(0, 0x57);
    pg.put_short(1);
    room->MakeRoomInfo((char*)&pg);
    pg.finalize(true);
    G_GameWorld()->send_all(pg);
    G_GameWorld()->goto_warroom(user);
    return 0;
}

__attribute__((weak)) int DisPatcher_MakeWarRoomTemp::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x082335f8：恒定返回 0
}

__attribute__((weak)) int DisPatcher_MakeWarRoomTemp::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                                              ParamBase& /*param*/)
{
    return 0;    // ORIG 0x08233602：恒定返回 0
}

__attribute__((weak)) DisPatcher_MakeWarRoomTemp::~DisPatcher_MakeWarRoomTemp()
{
}
