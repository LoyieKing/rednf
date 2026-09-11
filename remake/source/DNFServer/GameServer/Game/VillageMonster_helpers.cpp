// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 VillageMonster_helpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— village_attacked::CVillageMonsterMgr::OnKillVillageMonster
// 调用链（CBattle_Field.cpp 引用；独立 TU，避免与并行批次争文件）。
// 逐函数对照 ORIG df_game_r 反汇编（证据见各函数注释）；不编译，主 agent 统一验证。
//
// 【接线说明】CBattle_Field.cpp:887 以 4 参调用：
//     OnKillVillageMonster(m_villageMonsterCount, m_field130, (unsigned short)m_villageMonsterId, false)
//   ORIG 存在匹配重载 _ZN16village_attacked18CVillageMonsterMgr20OnKillVillageMonsterEiitb
//   （int,int,unsigned short,bool）@ 0x86b4964 —— 调用点正确，本文件实现该重载。
//   任务备注中的 0x86b4866 是另一重载 OnKillVillageMonster(CUser*,bool)（本工程未引用，不实现）。

#include <map>
#include <vector>

#include <cstring>

#include "CUser.h"
#include "CParty.h"
#include "TimerQueue.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "CMonitorServerProxy.h"
#include "PacketGuard.h"
#include "GameWorld.h"          // GameWorld::send_to_area / G_GameWorld
#include "CVillageMonsterMgr.h" // village_attacked 域唯一权威头
#include "PacketHeader.h"       // [M2S4] Packet_VillageMonsterFightResult 基类（ORIG ctor 证据）

// ============================================================================
// 外部符号（真实头声明；不再使用 asm 标签）
// ============================================================================
TimerQueue* G_TimerQueue();

// ---- Packet_VillageMonsterFightResult（ORIG ctor 0x86b4b84，0x2b 字节） ----
// PacketHeader(0x1772, 0x2b) + [4](uid,characNo) @ +0x0a + flag byte @ +0x2a
// [b29-SymForceBatch] 定义移到文件全局作用域：ORIG mangled 为顶层
// _ZN32Packet_VillageMonsterFightResultC1Ev（W 0x086b4b84，无 namespace 前缀）；
// 原定义位于 namespace village_attacked 内 ⇒ 发明 _ZN16village_attacked32…C1/C2。
// 构造/字段语义逐字不变。
// [M2S4 2026-09-11] 语义轴修：ORIG 0x086b4b8d/0x086b4b95/0x086b4b9d 三次压栈
//   $0x1772(0x4(%esp))/0x2b(0x8(%esp))/this(0) + call _ZN12PacketHeaderC1Ett
//   ⇒ 原码是 **PacketHeader 基类构造**（id=0x1772, size=0x2b，与 PacketHeader.h
//   及 shared/packet/include/Packet_VillageMonsterFightResult.h 权威形态一致）。
//   旧「手工 m_header0/m_header2 + 不做基类构造」既缺 3 movl+call+帧差（gap 5），
//   又让线格式包 ID/size 恒为未初始化值（语义缺口）。
struct Packet_VillageMonsterFightResult : public PacketHeader
{
    Packet_VillageMonsterFightResult()   // ORIG W 0x86b4b84；P223 双成员索引对形
        : PacketHeader(0x1772, 0x2b), m_flag(0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            m_uids[i] = 0;
            m_characNos[i] = 0;
        }
    }
    unsigned int m_uids[4];     // +0x0a
    unsigned int m_characNos[4];// +0x1a
    unsigned char m_flag;       // +0x2a
} __attribute__((packed));      // ORIG ctor 0x86b4b84 紧凑 0x2b 布局（无 packed 会插 2B padding）

namespace village_attacked
{

// ============================================================================
// CVillageMonsterMgr::OnKillVillageMonster(int,int,unsigned short,bool)
//   —— ORIG 0x86b4964
//   objdump: Zone zone((char)a,(char)b); m_areas.find(zone);
//            it == end -> return 0x15
//            return it->second.OnKillVillageMonster(0, c, d);
// ============================================================================

// OnMoveArea / OnEvent / InsertVillageMonster 由已登记的 CVillageMonsterMgr.cpp
// 提供定义（本 TU 不再重复定义，避免多重定义）。

#include "VillageMonster_helpers/OnKillVillageMonster.inc"


#include "VillageMonster_helpers/OnKillVillageMonster_1.inc"


#include "VillageMonster_helpers/OnKillVillageMonster_2.inc"


#include "VillageMonster_helpers/OnChangeState.inc"
{
    // P95：ORIG 0x86b35a0 首句 store 先载 this 后载 state；4.4 直成员赋值先备值。
    // 具名视图复现（仅表达布局；m_state = CVillageMonster +0x14）。
    struct CVMonsterStateView
    {
        char m_head[0x14];
        int  m_state;
    };
    ((CVMonsterStateView*)this)->m_state = state;
    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0xf0);
    pg.put_byte(m_state);
    pg.put_short(m_id);
    pg.finalize(true);
    m_area->SendPacket(pg);
}

#include "VillageMonster_helpers/SendPacket.inc"


#include "VillageMonster_helpers/SendVillageMonsterFightResult.inc"


#include "VillageMonster_helpers/MakeVillageMonsterListPacket.inc"


#include "VillageMonster_helpers/SendVillageMonsterList.inc"


}  // namespace village_attacked

#include "VillageMonster_helpers/GetRewardType.inc"
{
    return m_field20;
}

#include "VillageMonster_helpers/SetRewardType.inc"
{
    // P95：ORIG 先载 this 后载 reward；具名视图复现（m_field20 = +0x20）。
    struct CVMgrRewardView
    {
        char m_head[0x20];
        int  m_field20;
    };
    ((CVMgrRewardView*)this)->m_field20 = reward;
}
