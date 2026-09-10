// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BossTowerFunc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "BossTowerFunc.h"

#include "CGameManager.h"   // G_CGameManager / getBossTower() / getBossTower(int)
#include "CBossTower.h"     // WongWork::CBossTower（handleStartGame/_destroy/handleUseItem）
#include "CParty.h"         // CParty::useItem / get_member_count（完整类型）
#include "Inven_Item.h"     // Inven_Item 完整布局（m_field1/m_addInfo 字段化读取）
#include "GameTypes.h"      // cMyTrace

#include "CUser.h"          // CUser 完整类型（get_state/GetParty/getBossTower）

// ----------------------------------------------------------------------------
// ORIG T 0x081466f0 _ZN6Global11CGlobalFunc15Get_State_CheckE8ch_state
// （docs/class_func_reports/Global__CGlobalFunc/Get_State_Check.md ## 汇编 +
// ## 反编译 C 全文转写）：cmp $0xd + ja（无符号比较）→ 越界直接 0；否则
// (1 << state) & 0x3420 测试位 5/6/12/13（5=队伍玩法、12(0xc)=BossTower，
// 参见同文件 UseItem/DieCharacInDungeon 的 state 分支注释）。命中 → 1，否则 0。
// ORIG 调用方（objdump xref）：DisPatcher_FinishLoad::check_state(0x82654a6)、
// 0x81c2bd4、0x81c4b5b、0x81feef2、0x8232256、0x82324c0。
// [b28d-D9] N18a：P231 裸 register 消槽成功（20/20）；残 2 条 = 4.1 冗余 mov $1,%edx; mov %edx,%ebx 不可造（ORIG 0x081466f0 sz 0x00000031）
// [P6S5 2026-09-11 本波复核] 20/20 NEAR 维持；残差直证 = ORIG 0x081466f0
//   `mov $0x1,%edx; mov %edx,%ebx`（4.1 常量先落 %edx 再中转入 %ebx）+ 我方 4.4
//   `mov $0x1,%ebx; mov 0x8(%ebp),%eax`（参数重载）⇒ 净条数相同（20），N7/N18a 族，不可造。
//   ⚠ 头轴线索（上报，未改 .h）：`CBossTowerFunc::GetItem`（本 TU，原 AE 89/89）的
//   ORIG prett 串 = "static **bool** Global::CBossTowerFunc::GetItem(CUser*, int, bool, char)"
//   （strall 实读 O-only），我方 = "static **char** …" ⇒ 返回类型拼写差；
//   因声明在 `BossTowerFunc.h:61`（子代理禁改 .h）⇒ 只上报 P6AuditOldN/P7HeaderAxis。
#include "BossTowerFunc/Get_State_Check.inc"
{
    // b28c-L8 复测（N6 位折叠判据）：ORIG 0x081466f0 保 `mov $0x1,%edx;
    // mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax; and $0x3420`
    // 的**左移位图**形。4.4 前端会把 `(1 << state) & 0x3420` 折成
    // `0x3420 >> state` 的 shr 形（N6），唯一阻断法是让被移位值成为占槽局部
    // （实测具名 bit 形保 shl，但引入 store/load）⇒ 封顶 N6。
    // 本形同时按 P222 取正向包裹，使越界路径直落尾部共享 `mov $0` 岛
    // （ORIG 两处失败均 `ja/je` 跳同一 +0x29 岛）。
    // [b28d-D9] P231：ORIG 0x081466f0 的 bit 全程寄存器（%ebx）无栈槽
    // （`mov $0x1,%edx; mov %edx,%ebx; shl %cl,%ebx`）；裸 register 存储类
    // 提示改变 4.4 分配偏好（非 asm 绑定，P0 合规）。
    if (static_cast<unsigned int>(state) <= 0xd)
    {
        // [round5 复测] 已穷举三形（均 20 条 NEAR，无法归零）：
        // ①`bit = 1; bit <<= state;`（两句式）= 20 条 NEAR —— 缺 4.1 常量两跳
        //   `mov $0x1,%edx; mov %edx,%ebx`，多一次 state 重载（净 0，序列异）；
        // ②一句式 `bit = 1U << state` = 21 条 DIFF —— 两跳复现，但 4.4 对
        //   `(unsigned)state` 二次使用恒**重载** `mov 0x8(%ebp),%eax`，ORIG 靠
        //   register 保活（%eax 跨 cmp/移位）。
        // ③`register unsigned int st = (unsigned)state;`（承载 state）= 23 条
        //   DIFF —— 4.4 把 register 变量落 %ebx，多 `mov %ebx,%eax` 中转。
        // ⇒ 残差 = N18a（4.1 常量两跳不可造）+ N7（4.1 register 值保活 %eax
        //   vs 4.4 重载/%ebx），取 NEAR 形（20=20 长度全等）。
        // [S4NexonProto 复测] 追加三形（探针 /tmp/probe/gsc.cpp G2/G4/G5，本轮实测）：
        //   ④ `bit <<= state;`（enum 直作移位量，去 cast）= 与 ① 逐条同形
        //      （`mov $0x1,%ebx` 单跳 + `mov 0x8(%ebp),%eax` 重载，20 条）；
        //   ⑤ `register int bit = 1;`（与 enum 同型）= 同 ①；
        //   ⑥ `register unsigned int bit = 1;` 声明于 if **之前**（函数级）=
        //      `mov $0x1,%ebx` 前移到 cmp 之前，长度仍 20、序列异位。
        //   ⇒ `mov $0x1,%edx; mov %edx,%ebx`（4.1 常量两跳）+ %eax 跨 cmp/shl 保活
        //   在 4.4 下三形皆不可造，N18a/N7 锥实；维持 NEAR（20=20）。
        register unsigned int bit = 1;
        bit <<= static_cast<unsigned int>(state);
        if ((bit & 0x3420u) != 0)
        {
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146c20 _ZN6Global14CBossTowerFunc7UseItemEP5CUserRK10Inven_Item
// state==5(队伍玩法) → CParty::useItem；state==12(0xc,BossTower) →
// CBossTower::handleUseItem(item.m_addInfo(+0x02), item.m_field1(+0x01))。
// 两分支均返回 1；其它 state 返回 0。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/UseItem.inc"
{
    switch (user->get_state())
    {
    case 5:
        {
            // ORIG _ZN5CUser8GetPartyEv 返回 CParty*；现头声明为 void*，此处语义转换
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->useItem(user, item);
            }
        }
        return true;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleUseItem(user,
                                     static_cast<unsigned long>(item.m_addInfo),  // +0x02 dword
                                     item.m_field1);                              // +0x01 ITEM_TYPE byte
            }
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146994 _ZN6Global14CBossTowerFunc7GetItemEP5CUseribc
// getBossTower() 判空 → cMyTrace("bossTower not find : charac_no - %u") 恒返 1；
// 否则虚槽[10] handlePickupItem(user,item,flag,type)；非零 →
// PacketGuard: put_header(1,0x2e)+put_byte(1)+finalize(true)+user->Send。
// ----------------------------------------------------------------------------
// [b28d-D9] P227/N82：cMyTrace 临时链 %ebx 保活 + PacketGuard 槽位差（ORIG 0x08146994 sz 0x00000144）
#include "BossTowerFunc/GetItem.inc"
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
    {
        const char result =
            (char)tower->handlePickupItem(user, item, flag, type);
        if (result != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0x2e);
            packet.put_byte(1);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    else
    {
        // [P227/P221] ORIG 在 ctor call 前求 vararg（`call getCurCharacNo;
        // mov %eax,%ebx`）且 cMyTrace 槽浅于 PacketGuard ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xb7, 5)(
            "bossTower not find : charac_no - %u", user->getCurCharacNo());
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146722 _ZN6Global14CBossTowerFunc13SelectDungeonEP6CPartys17ENUM_DUNGEON_TYPE
// party 判空 → 0；getBossTower() 判空 → cMyTrace(PRETTY_FUNCTION,0x34,5) +
// "StaticPool BossTower NULL" → 0；按 get_member_count 映射 (1→0,2→1,3→2,4→3，
// 其它保持 0)；handleStartGame 失败时 _destroy()；返回其结果。
// ----------------------------------------------------------------------------
// [b28d-D9] N82/槽差：帧/槽序差（74 vs 66）（ORIG 0x08146722 sz 0x00000102）
#include "BossTowerFunc/SelectDungeon.inc"
                                           ENUM_DUNGEON_TYPE type)
{
    if (party == 0)
    {
        return false;
    }

    // ORIG +0x1d/+0x21：两个具名字节局部（result 深于 memberCount）；
    // 决策树按 P163 补 case 1（与未列值同体）撑宽 range。
    // [P211/P300] handleStartGame 返回 char（ORIG `mov %al,-0x12` 直存，无
    // setne）；bool 目标会让 4.4 插 `test/setne` 归一化（多 2 条）⇒ 同址
    // union 双视角：写入走 char 视角（直存），读取走 bool 视角（xor 直测）。
    union ResByte
    {
        char c;
        bool b;
    };
    ResByte result;
    result.b = false;
    char memberCount = 0;

    WongWork::CBossTower* tower = G_CGameManager()->getBossTower();
    if (tower == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x34, 5);
        trace("StaticPool BossTower NULL");
    }
    else
    {
        const int member = party->get_member_count();
        switch (member)
        {
            case 1: memberCount = 0; break;
            case 2: memberCount = 1; break;
            case 3: memberCount = 2; break;
            case 4: memberCount = 3; break;
            default: break;  // ORIG: 越界值保持 0
        }

        result.c = tower->handleStartGame(party, static_cast<int>(dungeonIdx),
                                          memberCount, type);
        if (!result.b)
        {
            tower->_destroy();
        }
    }
    return result.b;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655220 _ZN5CUser12getBossTowerEv
// m_bossTowerIdx(+0x8d010 short)<0 → 0；否则 G_CGameManager()->getBossTower(idx)。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/getBossTower.inc"
{
    // ORIG 0x08655220：无 idx 槽——movzwl 直读→test %ax,%ax; js→尾零块+
    // call 后 jmp 中继 = 三目返回形（P47）；传参点重读 movzwl+movswl（P200）。
    return ((short)m_bossTowerIdx < 0)
               ? 0
               : G_CGameManager()->getBossTower(
                     static_cast<int>(static_cast<short>(m_bossTowerIdx)));
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655262 _ZN5CUser15getAdvanceAltarEv
// m_advanceAltarIdx(+0x8d012 short)<0 → 0；否则 G_CGameManager()->getAdvanceAltar(idx)。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/getAdvanceAltar.inc"
{
    // ORIG 0x08655262：同 getBossTower——三目返回形。
    return ((short)m_advanceAltarIdx < 0)
               ? 0
               : G_CGameManager()->getAdvanceAltar(
                     static_cast<int>(static_cast<short>(m_advanceAltarIdx)));
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146824 _ZN6Global14CBossTowerFunc18DieCharacInDungeonEP5CUser
// state==10(死亡塔) → CDeathTower::handleDieUser；state==12(BossTower) → 虚槽[4]
// handleDieUser；其它 state 返回 0。两分支均返回 1。
// b28c-L8 判词 [N(N7)]：原 if 链 + 具名 `const int state` 已改
// switch(user->get_state()) —— ORIG 0x08146824 无 state 栈槽、`cmp $0xa,%eax;je /
// cmp $0xc,%eax;je / jmp` 决策链 = P0「值无槽往返⇒右值内联」+ P17 switch 形（if 链下
// 4.4 必产 store+mem 比较）；翻形后 44=44 逐条等。残 7 行 N7：ORIG
// `mov (%eax),%edx … call *%edx`（vptr+0x10 handleDieUser），4.4 对间接 call 目标恒
// %eax、其余装载寄存器随之整体后移一格（助记符/长度全等，仅寄存器名差）。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/DieCharacInDungeon.inc"
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);  // ORIG vptr+0x10 = 槽[4] handleDieUser
            }
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146916 _ZN6Global14CBossTowerFunc10FinishLoadEP5CUser
// state==10 → CDeathTower::handleFinishLoading；state==12 →
// CBossTower::handleFinishLoading（直调）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/FinishLoad.inc"
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleFinishLoading(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleFinishLoading(user);  // ORIG 直调 0x08143f6a（非虚分派）
            }
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146ad8 _ZN6Global14CBossTowerFunc10GiveUpGameEP5CUser
// state==10 → CDeathTower::handleLeaveUser；state==12 → 虚槽[5] handleLeaveUser；
// state==5 且队伍存在 → giveup_game(user,true,false,false)；恒返回 1。
// ----------------------------------------------------------------------------
// [b28d-D9] N82/槽差：帧/槽序差（59 vs 57）（ORIG 0x08146ad8 sz 0x000000ca）
// ORIG 尾 `mov $0x1,%eax; leave; ret`（单出口返 1）；case 体依源码序 5/0xa/0xc
// 落槽 -0x14/-0x10/-0xc（ORIG 深→浅 = 4.1「先声明更深」）。
#include "BossTowerFunc/GiveUpGame.inc"
{
    switch (user->get_state())
    {
    case 5:
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->giveup_game(user, true, false, false);
            }
        }
        break;
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleLeaveUser(user);
            }
        }
        break;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleLeaveUser(user);  // ORIG vptr+0x14 = 槽[5]
            }
        }
        break;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146ba2 _ZN6Global14CBossTowerFunc7MoveMapEP5CUserPt
// state==10 → CDeathTower::handleMoveMap()；state==12 → 虚槽[6] handleMoveMap(
// mapIndex)（CBossStage 新增虚）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
// b28c-L8 判词 [N(N7)]：同 DieCharacInDungeon 的 switch 翻形（ORIG 0x08146ba2 42=42
// 逐条等）；残 6 行 = 虚槽 +0x18 handleMoveMap 的间接 call 目标 %edx vs 4.4 恒 %eax。
#include "BossTowerFunc/MoveMap.inc"
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleMoveMap();
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleMoveMap(mapIndex);  // ORIG vptr+0x18 = 槽[6] handleMoveMap
            }
        }
        return 1;
    }
    return 0;
}
