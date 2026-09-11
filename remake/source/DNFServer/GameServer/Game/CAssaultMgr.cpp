// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAssaultMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultMgr 实现（批3：从 GameStubs.cpp 迁移）。
// 语义依据 ORIG：OnMoveArea 0x82ef610 / SendPacket 0x82efb54 /
// OnLeaveAssaultPlace 0x82ef176 / _GetAssaultPlace 0x82ed9fe。
// b28d-solo-CAssaultMgr（2026-09-09，泳道 CAssaultMgr.cpp 9 件）判词：
//   · LeaveAssaultPlace(Eibb)：**DIFF → IDENTICAL_AE**（leave 包作用域延长消
//     独立 EH cleanup + 收尾链去冗余外层 `if(user!=0)` 真语义修正）。
//   · CloseAssault：PacketGuard 改独立块（析构点对齐 082e9ec0）；残差 N22×1 +
//     N121 尾岛×2 + 重定位。
//   · CheckPoint：P167 归位（`(*it).m_*` 代 `it->m_*`，deEv 代 ptEv；missing 61→60）。
//   · MakeUserState ×2：N7 单条 %eax↔%edx 角色互换（P248 三档旋钮穷举后封顶）。
//   · CStaticDisposition::C1：N4/P30/N102 族（m_x 元素访问 + m_y 数组引用视图的
//     基址载入/add-vs-lea 残差；八形探针无更优）。
//   · AbuseUser / GetCharacName / ChangeHp：清单 NEAR → 路由口径 AE（复验不变）。
//   · p167：9/9 符号 CLEAR（CStaticDisposition 的 CDisposition C1/C2 同址别名
//     属 P93 良性；CheckPoint 已由 ptEv 归位 deEv）。
#include "CAssaultMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CGameManager.h"     // CAssaultPlace 完整类型与方法声明
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_power_manager / s_event_manager / s_systemTime_
#include "TimerDispatcher.h"   // TimerCompleteLoadAfterAssaultTimeout
#include "Village.h"
#include "Area.h"
#include "CVillageObjectMgr.h"    // on_move_area（_DelUser）
#include "CVillageMonsterMgr.h"   // OnMoveArea（_DelUser）
#include "CDisconnectDetecter.h"  // disconnect_detecter::UnregisterUser（_DelUser）
#include "ServerParameterScript.h"  // getAssaultGhostTime（GiveupPanalty）
#include "CPowerManager.h"
#include "CGuildServerProxy.h"    // CEventBase / CEventManager::GetRepeatEvent（_IsGiveupPanalty）
#include "CInventory.h"           // user_creature::CCreatureMgr::TurnStomach（DetachUser）
#include "GameWorld.h"            // G_GameWorld / send_all（SendUserState）
#include "Packet_Broadcast_Msg.h" // _BroadcastRunaway 公告包
#include "CServerProxyMgr.h"      // CMonitorServerProxy（_BroadcastRunaway）
#include "CMonitorServerProxy.h"  // _BroadcastRunaway 完整类型

// 全局依赖前置（3 参版定义见本文件尾；6 参版 owner=CVillageMonsterMgr.cpp）
#include "CAssaultMgr/IsIncludeVirtualMoveAreas.inc"

// [W13F 2026-09-11] 复现 ORIG 文件作用域常量符号 `_ZL14MAX_PVP_PLAYER`
//   （`nm ORIG`：r, 4B, 0x8c1c344 = 0x00000008；本 TU 的 SetUserConnectP2P 取址使用）。
//   必须在**全局作用域**声明：放命名空间内会得到 `_ZN11pvp_assaultL14MAX_PVP_PLAYERE`。
static const int MAX_PVP_PLAYER = 0x8;

namespace pvp_assault
{

#include "CAssaultMgr/GetDirectionByTeam.inc"
{
    // ORIG T 0x82e564c（0x30 字节）：具名槽 dir(-0x4) 初值 0，`test;je`(==0)
    // /`cmp $1;je`(==1) 各自写槽 + default 直落 `jmp ret`，统一装载返回。
    // b28d-L3 实测：if/else-if 链在 4.4.6 产 `cmpl $0,0x8(%ebp);jne` 形参内存
    // 直比（差 10）；switch 形装 eax + `cmp $1;je`（差 3，见 VARIANT-ARTIFACT
    // 说明：4.4.6 对 2-case switch 把 case 0 折为 fall-through，缺 test/je 与
    // default jmp，源形不可择）⇒ 保留 switch 形（本 TU 路由 446h46 的最优形）。
    // b28d-solo-CAssaultMgr: ENUM_ASSAULT_TEAM 当前仅 1 个枚举量（值域 {0}）⇒
    // 4.4.6 把 case 0 折进 default（缺 ORIG 的 `test;je` 与 default `jmp`，14 vs 16）。
    // switch 表达式内联 (int) 转换即恢复全宽值域：探针 g2 `switch((int)e)` 与 ORIG
    // 逐条同形（test;je / cmp $1;je / jmp 三链 + 双赋值块）。
    int dir = 0;
    switch ((int)team)
    {
    case 0: dir = 6; break;
    case 1: dir = 4; break;
    }
    return dir;
}

CAssaultMgr* GetInstanceAssaultMgr()
{
    // ORIG _ZN11pvp_assault21GetInstanceAssaultMgrEv：guard 单例。
    // [W13F 2026-09-11] 局部 static 名实证：ORIG guard = `_ZGVZN11pvp_assault21
    //   GetInstanceAssaultMgrEvE8instance` @0x94001a0（8B）/ 对象
    //   `_ZZN11pvp_assault21GetInstanceAssaultMgrEvE8instance` @0x9400b520（0x20B）；
    //   我方原名 `s` ⇒ 两条 ORIG-only 符号缺口 ⇒ 改名复现（纯符号名，零 codegen 影响）。
    //   遗留：ORIG 两符号为 W/V（原厂头内 inline 定义所致），我方为 T/b —— 见 W13F 报告待核。
    static CAssaultMgr instance;
    return &instance;
}

#include "CAssaultMgr/_GetAssaultPlace.inc"
{
    // ORIG 0x82ed9fe：未命中抛 ENUM_ERROR(0x15)。
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
    if (it == m_places.end())
        throw (ENUM_ERROR)0x15;   // ORIG 抛 ENUM_ERROR(0x15)
    // ORIG 0x82eda3x：`mov 0x4(%eax),%eax; mov %eax,-0xc; mov -0xc,%eax`
    // ⇒ second 具名物化槽（技巧13 声明位置=落槽时机）。
    CAssaultPlace* v = it->second;
    return v;
}

#include "CAssaultMgr/OnMoveArea.inc"
{
    // b28d-solo-CAssaultMgr: AE — 本波修两处：①`place==0/!IsArea` 双 continue
    // 改嵌套正向 if 包裹（P296，消 xor$1 + 双 jmp+nop 岛）；②area 删 (char)
    // cast（ORIG 0x82ef6bf get_area 返回 int 直传 `mov %eax,%ebx`，getCurCharacVill
    // 才 movsbl）⇒ 157→149 与 ORIG 150 逐条 AE（余 1 条尾 nop 对齐豁免）。
    // b28d-L3: DIFF/N82 — ORIG 0x082eb... 临时对象槽 -0x24 vs 我方 -0x28（N82/P112 槽序）
    // ORIG 0x82ef610：向进入袭击区域的玩家汇总可见图标（包 0x86）。
    // 整函数 try/catch(int)（尾 landing `cmp $0x1,%edx; je resume` +
    // begin_catch `mov (%eax),%eax; mov %eax,-0xc` + end_catch），统一尾
    // `mov $0x1,%eax; ret` ⇒ 返回 int 1（P159）。view 判定为**单条 && 短路链**
    // （0x82ef6f0 ExistUser→xor$1 / CheckPlayable test / GetState cmp4 jne
    // 三级直落 false 岛 `mov $0,%eax`，+0x122 test 汇聚；if/else-if 三态
    // 赋值形产 setcc+cmpb 岛，不同形）。
    // 收敛口径：o=149 n=156。语义修正已全落（catch(int)/int 返回/view 短路/
    // dead 双清零/vill int 无截断+area (char) 收窄/movsbl 位点）。残余：
    // vill 跨 get_area 调用 ORIG 用 callee-saved %ebx 免槽直落，4.4 分配器
    // 因既有 ebx/esi 活跃改落栈槽（register asm("ebx") 实测反扰乱装载序
    // 已回滚自然形）；it/place/计数槽 +0x4 平移与 nop 岛 = N82/N76/
    // 跨版本 nop;jmp 岛族，byte 封顶。
    try
    {
    PacketGuard packet;
    packet.put_header(0, 0x86);
    // ORIG：countIdx/count 先双清零（`movl $0,-0x28; movl $0,-0x14`，
    // dead init 照抄 P171），随后 get_index 覆写 countIdx。
    int countIdx = 0;
    int count = 0;
    countIdx = packet.get_index();
    packet.put_short(count);
    for (std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.begin();
         it != m_places.end(); ++it)
    {
        CAssaultPlace* place = it->second;
        // b28d-solo-CAssaultMgr: ORIG 0x82ef6b0 两处守卫（place==0 / !IsArea）
        // 均 `je <循环出口>` 且无 xor ⇒ 原码是嵌套正向 if 包裹（P296），
        // `continue` 形在 4.4 恒产 `xor $1` + 双 `jmp+nop` 中继岛（+7）。
        // vill 内联为实参（P0：ORIG 无槽往返，`call; mov %eax,%ebx`）。
        if (place != 0)
        {
            // ORIG 0x82ef6bf：get_area 结果 `mov %eax,%ebx` 无 movsbl（返回 int
            // 直接传，原码无 (char) cast）；getCurCharacVill 才 movsbl（返回 char）。
            if (place->IsArea(user->getCurCharacVill(),
                              user->get_area(false)))
            {
                if (!place->ExistUser(user) && place->CheckPlayable() &&
                    place->GetState() == 4)
                {
                    place->MakeViewAssaultIconOnePlayer(&packet, true);
                    count += 1;
                }
            }
        }
    }
    if (count != 0)
    {
        packet.put_short(countIdx, count);
        packet.finalize(true);
        user->Send(packet);
        }
    }
    catch (int)
    {
    }
    return 1;
}

#include "CAssaultMgr/SendPacket.inc"
{
    // ORIG 0x82efb54：按用户所在袭击区域转发；flag=true 时限定队伍。
    // [b24F] DONE：返回类型 void→int + return 1（ORIG 尾部 mov $0x1,%eax）。
    // ORIG EH：catch(int)（landing 带类型过滤 + begin_catch 后 `mov (%eax),%eax`
    // 取值死槽，OnLeaveAssaultPlace/OnMoveArea 同款——此前 catch(...) 缺 6 条）。
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(
            (unsigned int)user->GetAssaultPlace());
        if (flag)
        {
            ENUM_ASSAULT_TEAM team;
            if (place->GetTeam(user, team))
                place->SendPacket(*packet, team);
        }
        else
        {
            place->SendPacket(*packet);
        }
    }
    catch (int)
    {
    }
    return 1;
}

#include "CAssaultMgr/OnLeaveAssaultPlace.inc"
{
    // ORIG 0x82ef176：取用户所在区域并执行离开流程；ORIG EH 为 **catch(int)**
    // （类型过滤 landing pad `cmp $0x1,%edx; je <resume>` + begin_catch 后
    // `mov (%eax),%eax; mov %eax,-0xc` 取值落槽 + end_catch），吞掉 P6 族
    // throw ENUM_ERROR(int)。此前写 catch(...) 少 6 条（无过滤器/无取值）。
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(
            (unsigned int)user->GetAssaultPlace());
        place->LeaveAssaultPlace(user, flag);
    }
    catch (int)
    {
    }
    // ORIG 0x82ef1da：尾 mov $1,%eax ⇒ int 返回 1
    return 1;
}

#include "CAssaultMgr/SetAssaultChannel.inc"
{
    // ORIG W _ZN11pvp_assault11CAssaultMgr17SetAssaultChannelEb @0x08376048：
    // movzbl flag → *(unsigned char*)this = flag（写入 +0x00 通道开关）。
    m_bAssaultChannel = flag;
}

#include "CAssaultMgr/GetTeam.inc"
{
    // b28 BL6: N96/P0 清理 register asm + P182 强撑 pad——还原自然形；
    // P210 定义前置（移于 CAssaulter::GetUser 之前）复现 ORIG 0x28 保守帧。
    // ORIG `cmp; sete; test; je` 物化岛 = 4.1.2 O0 setcc 顶产物（P197④/N93附
    // 同族），4.4 恒 `cmp; je` 直跳，残差封顶。
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].GetUser() == user) {
            team = m_assaulters[i].GetTeam();
            return true;
        }
    }
    return false;
}

// ---- CAssaulter 访问器（ORIG 布局：+0x00 user / +0x04 team / +0x08 state）----

// b28d-solo-CAssaultMgr: P210 三分律②——外联定义置于 CAssaultPlace::~CAssaultPlace
// 之前，使 4.4 对其数组析构循环精确继承 ORIG 的 sub $0x4 帧（原头内 inline 为①，
// 保守撑帧 sub $0x14 ⇒ D1 NEAR 2）。
#include "CAssaultMgr/dtor_CAssaulter.inc"
{
}

#include "CAssaultMgr/GetUser.inc"
{
    // ORIG W 0x82f0542：mov (%eax),%eax
    return m_user;
}

// ORIG W 0x82f055c：call IsAttachAble（0x82f054c）。
#include "CAssaultMgr/Empty.inc"
{
    return IsAttachAble();
}

#include "CAssaultMgr/GetTeam_1.inc"
{
    // ORIG W 0x82f0570
    return m_team;
}

#include "CAssaultMgr/GetUserUniqueId.inc"
{
    // ORIG T 0x82e6662：user ? user->get_unique_id() : 0
    if (m_user == 0)
        return 0;
    return (unsigned short)(m_user)->get_unique_id();
}

#include "CAssaultMgr/SetState.inc"
{
    // ORIG W 0x82f057c
    m_state = state;
}

#include "CAssaultMgr/GetState.inc"
{
    // ORIG W 0x82f058a
    return m_state;
}

// ---- CAssaultPlace 内部前置函数 ----

#include "CAssaultMgr/_GetTeamCount.inc"
{
    // ORIG T 0x82e7232：continue 形（call Empty 后 test al;jne，无 xor）；
    // GetTeam 结果 %eax 直通索引（技巧28）+ 后缀 ++（技巧4 单装载）。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // N96 清绑：GetTeam 结果作下标表达式直传（P0 表达式直读）。
        counts[m_assaulters[i].GetTeam()]++;
    }
}

#include "CAssaultMgr/_CheckPlayable.inc"
{
    // ORIG T 0x82e9142：两队人数均非 0 才可开局。
    int counts[2] = { 0, 0 };
    _GetTeamCount(counts);
    for (int i = 0; i < 2; ++i) {
        if (counts[i] == 0)
            return false;
    }
    return true;
}

#include "CAssaultMgr/CheckPlayable.inc"
{
    // ORIG W 0x82f06ea：tail-call _CheckPlayable。
    return _CheckPlayable();
}

#include "CAssaultMgr/GetState_1.inc"
{
    // ORIG W 0x82f06fe：读 +0x10c。
    return m_state;
}

#include "CAssaultMgr/IsArea.inc"
{
    // ORIG W 0x82f06ae：比较 +0x118/+0x11c（Point2d）。
    return m_pos.m_x == vill && m_pos.m_y == area;
}

#include "CAssaultMgr/_GetUserIndex.inc"
{
    // ORIG T 0x82e849a：找不到时 strict=true 抛 ENUM_ERROR(0x15)，否则返回 -1。
    // call Empty 后 test %al;jne skip（0x82e84bb 区）；比较 cmp+sete+test（技巧22）。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // N96 清绑：call 结果与形参直比（P197④ 出 sete 物化）。
        if (m_assaulters[i].GetUser() == user)
            return i;
    }
    if (strict)
        throw (ENUM_ERROR)0x15;
    return -1;
}

#include "CAssaultMgr/ExistUser.inc"
{
    // ORIG T 0x82e8366：call Empty 后 `test %al,%al; jne skip`（无 xor，
    // continue 形态）；GetUser==user 产 cmp+sete+test 三连接（技巧22）。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // N96 清绑：call 结果与形参直比（P197④ 出 sete 物化）。
        if (m_assaulters[i].GetUser() == user)
            return true;
    }
    return false;
}

#include "CAssaultMgr/SendPacket_1.inc"
{
    // ORIG T 0x82e6bf6：全员转发；任一 Send 失败则返回 false。
    bool ok = true;
    for (int i = 0; i < 8; ++i) {
        CUser* user = m_assaulters[i].GetUser();
        if (user != 0)
        {
            // [b28c-L7] P168a①：Main 已按 ORIG 串代落 CUser.h `bool Send(
            // PacketGuard&)`（同批 AntibotSend/SendCmdPacket/SendNotiPacket）⇒
            // 裸形直产 ORIG 0x082e6c2b `call; xor $0x1,%eax; test %al,%al; je`。
            if (!user->Send(packet))
                ok = false;
        }
    }
    return ok;
}

#include "CAssaultMgr/SendPacket_2.inc"
{
    // ORIG T 0x82e6c56：仅指定队伍转发。
    bool ok = true;
    for (int i = 0; i < 8; ++i) {
        CUser* user = m_assaulters[i].GetUser();
        if (user != 0 && m_assaulters[i].GetTeam() == team)
        {
            // [b28c-L7] 同上（ORIG 0x082e6cc6 同形）。
            if (!user->Send(packet))
                ok = false;
        }
    }
    return ok;
}

#include "CAssaultMgr/MakeViewAssaultIconOnePlayer.inc"
{
    // ORIG T 0x82e89c0：put_byte 直传 flag 零扩展（0x82e89xx `movzbl -0x1c,%edx`
    // 无三元物化）；`cmpb $0; je 尾` ⇒ 正向 if(flag) 块（尾 call 自然落 leave，
    // 无 return 岛）；countIdx/count 双 dead init（-0x14/-0x10）后 get_index 覆写。
    packet->put_int(m_field104);
    packet->put_byte((unsigned char)flag);
    if (flag)
    {
        packet->put_short((int)m_posBase.m_x);
        packet->put_short((int)m_posBase.m_y);
        int countIdx = 0;
        int count = 0;
        countIdx = packet->get_index();
        packet->put_short(count);
        for (int i = 0; i < 8; ++i) {
            if (m_assaulters[i].Empty())
                continue;
            packet->put_short((int)m_assaulters[i].GetUserUniqueId());
            packet->put_byte(GetDirectionByTeam(m_assaulters[i].GetTeam()));
            count += 1;
        }
        packet->put_short(countIdx, count);
    }
}

#include "CAssaultMgr/LeaveAssaultPlace.inc"
{
    // ORIG T 0x82eb450：入口 `mov %al,-0xc; movzbl -0xc,%ebx` 把 bool 形参
    // 截断+零扩展入 callee-saved %ebx，idx 直落 %eax，传参 `mov %ebx,0x8(%esp)`
    // 无二次物化。4.4 对 bool 值传 bool 形参恒发 `mov %ebx,%edx; movzbl %dl`
    // （P33/P73 物化，多 2 条）；P78 禁局部 register asm。
    LeaveAssaultPlace(_GetUserIndex(user, true), flag, false);
}

// ============================================================================
// CAssaulter 方法（ORIG 权威；LeaveAssaultPlace 链依赖）
// ============================================================================

// ORIG T 0x82e6688：return *(int*)(this+0x10)。
#include "CAssaultMgr/GetHp.inc"
{
    return m_hp;
}

// ORIG T 0x82e6812：卸下 creature（TurnStomach false）+ 清 user/team 槽位。
#include "CAssaultMgr/DetachUser.inc"
{
    if (m_user->getCurCharacR() != 0)
        m_user->getCurCharacInvenW()->GetCreatureMgrW()->TurnStomach(false);
    m_user = 0;
    m_team = (ENUM_ASSAULT_TEAM)0;
}

// ORIG T 0x82e6868：放弃惩罚 —— 备份耐力 → 记 GiveupPvP 日志 → 设 ghost 惩罚时间。
// 收敛口径（batch-14 EmoC11）：o=142 n=143（原 -18）。已修语义两处：第一段
// 条件真值 `!pw && vill!=7`→(7)/else(3)（旧写 `pw||vill==7`→7 为分支反了）+
// 两段各自独立 IsPowerWarEventOn call、两分支独立 GiveupPvP（P48/P133）。
// N96 清绑：两处 bool 绑定删除后，裸合并短路条件在 4.4 直产 ORIG 的
// $1/$0 物化+test 汇聚岛（探针 /tmp/x1.cpp 同族）；全展开 m_user->
// 直读（P183）。残 1 条 = ORIG push esi/ebx 寄存器接力 vs 4.4 栈槽 + .o
// 全局地址/槽号显示（AE 后核销）。
#include "CAssaultMgr/GiveupPanalty.inc"
{
    // b28d-solo-CAssaultMgr: AE — 尾块由「空 then + else body」改单出口正向
    // 包裹 `if (ghostTime != 0)`（消 4.4 尾 `jmp <epilogue>; nop` +2），
    // 144→142 与 ORIG 142 逐条 AE。
    // b28d-L3: DIFF/N7+N82 — ORIG 0x82e6694.. chaos/level 实参内联后帧 0x24 vs ORIG 0x20、缺 push %esi（ORIG 值留 %esi 跨调用）
    // P183/P157：ORIG 不缓存 m_user（每点 `mov this; mov (%eax)` 重装载，
    // penalty/now 处才用 %esi/%ebx 跨调用缓存）——重建禁 named user。
    if (m_user != 0)
    {
    // ORIG +0x1d：getCurCharacR `test %eax; setne %al; test %al; je`——4.4 对
    // 裸 `!= 0` 指针比较恒产同款两段物化（带变量赋值反改 slot store+cmpl）。
    if (m_user->getCurCharacR() != 0)
    {
        m_user->setCurCharacBackupStamina(m_user->getCurCharacStamina());
        // ORIG 0x082e6868（P48/P84）：两处独立 IsPowerWarEventOn call、$1/$0
        // 物化岛 + 二次 test 汇聚。第一段真值（+0x5d `xor $0x1` 后 test;je→$0
        // 岛，+0x73 `cmp $7; je→$0`）= `!pw && vill!=7` → GiveupPvP(7)，
        // else→(3)（两分支独立 call）；第二段（+0xd1 `test; jne→$1`，
        // +0xe2 `cmp $7; jne→$0`）= `pw || vill==7`。
        // N96 清绑：ORIG $1/$0 物化岛由合并短路条件自然产生（P133）。
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            m_user->getCurCharacVill() != 7)
        {
            m_user->m_historyLog.GiveupPvP((ENUM_PVP_HISTORY_REASON)7);
        }
        else
        {
            m_user->m_historyLog.GiveupPvP((ENUM_PVP_HISTORY_REASON)3);
        }
        int ghostTime = 0;                    // ORIG +0xbb 第二段前预清零
        if (GlobalData::s_power_manager->IsPowerWarEventOn() ||
            m_user->getCurCharacVill() == 7)
        {
            float penalty = GlobalData::s_power_manager->GetPowerWarResponPenalty(m_user);
            ghostTime = GlobalData::s_power_manager->GetPowerWarGhostTime(m_user, penalty);
        }
        else
        {
            // b28d-L3：ORIG 0x82e6a?? 先求第2实参 GetCurCharacChaosPoint→%esi
            // 跨 get_charac_level call 保活（无槽往返 P0）⇒ chaos/level 与
            // receiver（CDataManager::m_serverParameter 即 +0x68）全部内联为
            // 实参/receiver 表达式，消具名 sps 槽（帧 0x24→0x20 对齐 ORIG）。
            ghostTime = G_CDataManager()->m_serverParameter.getAssaultGhostTime(
                m_user->get_charac_level(), m_user->GetCurCharacChaosPoint());
        }
        // b28d-solo-CAssaultMgr: ORIG 0x82e69d0 `cmpl $0,-0x14; je <epilogue>`
        // + body 直落 epilogue（无尾 jmp/nop）。试单出口包裹形。
        if (ghostTime != 0)
        {
        m_user->SetCurCharacGhost(true);
            //（ORIG 用 %ebx callee-saved 缓存 now；4.4 绑 %ebx 实测反增 spill，
            // 维持具名槽，寄存器选择差记 N7/N82。）
            int now = GlobalData::s_systemTime_.getCurSec();
            m_user->SetCurCharacChaosDieTime(now);
            m_user->SetCurCharacChaosResponTime(now + ghostTime);
            m_user->m_characterView.enableSaveCharacView();
        }
    }
        }
}

// ============================================================================
// CAssaultPlace LeaveAssaultPlace 链私有 helper（ORIG 权威；自包含部分已还原，
// 其余依赖未还原子系统处标注，不造桩）。
// ============================================================================

// ORIG T 0x82eacf4：GetRepeatEvent(0x20) 后经 vtable+0x34 虚调用判活——该槽
// 即 IsEventing(CUser*)，与 IsPowerWarEventOn 0x0847f910 的 GetRepeatEvent→
// vtable+0x34→call(0) 形态逐指令一致；活动进行中直接返回 false（不惩罚）。
// 否则 m_state（+0x10c）∈[2,4] 且 !flag 时返回 true。ORIG 对事件指针无空检，
// 保持原样。
#include "CAssaultMgr/_IsGiveupPanalty.inc"
{
    // ORIG 0x82eacf4：GetRepeatEvent 返回 %eax 直通 vtable 装载（无 event 槽）
    // ⇒ 匿名链式调用。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        return false;
    return m_state >= 2 && m_state <= 4 && !flag;
}

// ORIG T 0x82eab1a：_IsBroadcastRunaway —— PowerWar 且村庄==7 直接 false；
//   否则 getCurCharacR()!=0 决定返回值（0x82eab50 `mov $0` 在 t 为 true 分支）。
#include "CAssaultMgr/_IsBroadcastRunaway.inc"
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
        user->getCurCharacVill() == 7)
        return false;
    // ORIG 0x82eab62：`test eax;setne al; test al; je; mov$1;...mov$0`
    // 物化尾岛（P197④ call 结果寄存器直比出 sete；N96 绑定已清）。
    if (user->getCurCharacR() != 0)
        return true;
    return false;
}

// ORIG T 0x82eab7a：_BroadcastRunaway(CUser*, int) —— 经 monitor server
// 广播逃跑公告（ServerParameter 的 notice 模板 + 双方角色名）。
#include "CAssaultMgr/_BroadcastRunaway.inc"
{
    // b28d-solo-CAssaultMgr: **AE（归零）** — S174 曾判 NEAR（槽基平移 4B）。
    // 真因：ORIG 的 len/len2 不是独立局部，而是 **msg 成员** —— 其槽位
    // -0x11c/-0x128 恰为 msg+0xe/+0x2（msg this=-0x12a、sizeof 0x10e）；
    // 写 `msg.m_len`/`msg.packetSize` 后局部集与 ORIG 一致，`char pad1[1]`
    // 死局部（P312 旧代偿）随之删除，buf 自然落 -0x52a（mod4=2）复现 ORIG
    // 对齐剥离慢路。orig 102 = ours 102，p167 CLEAR。
    char buf[0x400] = {0};
    // b28d-L3：ORIG 0x82eab7a 段把 name→%esi、curName→%ebx 跨 notice 求值
    // 保活、三值均无槽往返（P0）⇒ 三具名局部内联为 sprintf 实参（变参右到左
    // 求值序：第4参 name 先、第3参 curName 次、第2参 notice 末）。
    sprintf(buf,
            G_CDataManager()->m_serverParameter.getAssaultUserRunawayNotice(),
            user->getCurCharacName(),
            GetAnotherTeamCharacName(index));

    Packet_Broadcast_Msg msg;
    msg.m_padA = 0;
    // ORIG 0x82eac41/0x82eac9e 直写 msg+0xe/+0x2 ⇒ 原码即成员赋值（见上）。
    msg.m_len = static_cast<char>(strlen(buf));
    memset(msg.m_text, 0, 0xff);
    strncpy(msg.m_text, buf, (unsigned char)msg.m_len);
    msg.packetSize = static_cast<short>((unsigned char)msg.m_len + 0xf);
    int packetSize = (unsigned short)msg.packetSize;
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&msg), packetSize);
}

// ORIG T 0x82e9046：_MakeLeaveUser —— 包 0x87 + 玩家 unique_id。
#include "CAssaultMgr/_MakeLeaveUser.inc"
{
    p->put_header(0, 0x87);
    p->put_short((int)m_assaulters[index].GetUserUniqueId());
    p->finalize(true);
}

// ORIG T 0x82e88d6：_MakeCloseAssault —— 包 0x89 + tick。
#include "CAssaultMgr/_MakeCloseAssault.inc"
{
    p->put_header(0, 0x89);
    p->put_byte(tick);
    p->finalize(true);
}

// ORIG T 0x82e8d60：_MakeUserState(PacketGuard*, CUser*) —— 包 0x03 + state。
#include "CAssaultMgr/_MakeUserState.inc"
{
    // b28d-solo-CAssaultMgr: N 封顶（N7 寄存器角色互换；清单 DIFF 11 复验）
    // ORIG 0x82e8d60+0x30：`call GetUserState; mov %eax,%edx; mov p,%eax;
    // mov %edx,0x4(%esp); mov %eax,(%esp)`（值先中转 %edx、this 后载 %eax）；
    // 4.4 恒 `mov p,%edx; mov %eax,0x4(%esp); mov %edx,(%esp)`（this 先载、
    // 值留 %eax）——助记符序列全等，仅 %eax/%edx 角色互换（34 vs 33）。
    // 本轮补测 /tmp/probe_mus.cpp + probe_mus2.cpp 17 形（uint/long/enum/
    // 短整 cast、语句表达式、*&、receiver cast、具名 receiver、前置掩码上下文）
    // 4.4 全产 this-first；callee 返回类型旋钮不可用（CUser.h 冻结头）。
    // ORIG 同 TU SendUserState 同表达式无 hop（ORIG 自身分裂两形）。
    // [S6 2026-09-11 复核] p167 CALLSITE CLEAR（7/7 无差集）；`GetUserState()` 返回
    // int、`put_byte(int)` 同型（无窄化转换可借）⇒ 维持 N7 判词，零改动。
    p->put_header(0, 3);
    p->put_byte(1);
    p->put_short(user->get_unique_id() & 0xffff);
    // 同判词（N7）：单条 %eax↔%edx 角色互换，语义等价；绑定形 P0 禁用。
    p->put_byte(user->GetUserState());
    p->finalize(true);
}

// ORIG T 0x82e83c4：SendUserState(CUser*) —— 组 0x03 状态包并全服广播
// （报告 pvp_assault__CAssaultPlace/SendUserState.md；ORIG 调用点仅压 CUser*，
//   头文件声明为静态形态）。
#include "CAssaultMgr/SendUserState.inc"
{
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_byte(user->GetUserState());
    packet.finalize(true);
    // ORIG 0x82e845c：G_GameWorld 结果 %eax 直通 this 实参（无 world 槽，
    // 其余实参用 %edx 装载）⇒ P52/P134 链式内联（N96 绑定已清）。
    G_GameWorld()->send_all(packet, user);
}

// ORIG T 0x82e7b2a：_SendCloseAssault(CUser*, int) —— tick 下限 0x1e，发关闭包。
#include "CAssaultMgr/_SendCloseAssault.inc"
{
    // ORIG 0x82e7b2a：`test eax;setne al;test al;je bb1` 判 R!=0；
    // 两路径统一 `mov 0x10(%ebp),%eax` ⇒ 恒返回 tick（钳位直写参数槽）。
    // N96 清绑：call 结果直比出 setne 物化（P197④）。
    if (user->getCurCharacR() != 0)
    {
        if (tick < 0x1e)
            tick = 0x1e;
        {
            PacketGuard packet;
            _MakeCloseAssault(&packet, tick);
            user->Send(packet);
        }
    }
    return tick;
}

// ============================================================================
// LeaveAssaultPlace(int,bool,bool) —— ORIG 0x082ead6a 逐段转写。
// 结构：索引/用户校验 → GiveupPanalty 判定（m_state 区间分支）→ 逃跑广播 →
// leave 包 + PowerWar 计数 + close 包 → _DelUser/用户状态包 → PowerWar 特判
// （村庄==7 时 guild 点数/连击广播）→ 双方 SetAssaultState(6)+EndAssault。
// 收敛口径（2026-09-05 EmoC11）：o=453 n=455。已消：G_GameWorld 具名中间槽
// （P52/P134 内联实参 → %eax 直通 ×2）、Empty() 正向判定（P22/P59 去 xor 物化）。
// P167 调用点差集 ONLY-ORIG=0（ONLY-OURS 仅 1×_Unwind_Resume：4.4 对第二个
// PacketGuard 块独立 EH 岛 vs 4.1 merge——N 表跨版本「EH 双份 dtor epilogue vs
// merge」族）。其余残余：帧 0x80 vs 0x70 + 全槽平移（N82/P112）、尾多 1 条
// `jmp <epilogue>` 岛边（跨版本 nop;jmp 岛/jump-threading 族）、`add $0x70` vs
// `sub $0xffffff80` 文本形差。语义完成、byte 层 N 封顶。
// b28d-solo-CAssaultMgr（2026-09-09）: **DIFF → IDENTICAL_AE**（TU diff 6→5、
// ae 225→226）。两处结构修正使 ORIG 的 PacketGuard/EH 布局与 p167 同时归位：
// ①leave 包（-0x24）作用域由独立块延到 082eb06f——ORIG 的 D1 在收尾链 +0x310
//   之前、ghost 块 +0x2e8 之后 ⇒ 消掉我方独立 EH cleanup（EXTRA _Unwind_Resume）；
// ②收尾链去掉冗余外层 `if(user!=0)`（裸作用域仅维持缩进）——ORIG +0x310 短路链
//   自含 user 项，user==0 时仍落到 +0x3d0 的 m_field100 检查（旧形真语义差异）。
// 修正后 clean 口径 orig 453 = ours 453、p167 calls 63=63 CLEAR。
// 本轮两处**语义/结构修正**（原形为真语义反向 + 缺条件）：
// ①82eb07a 收尾链 = `user != 0 && (!IsPowerWarEventOn() || vill != 7)` 嵌套
//   `m_state <= 3 && index∈{0,1}`（旧德摩根形缺 user 项且把 m_state 并入外层链）；
// ②82eb1b0 段 `call _CheckPlayable; xor$1; test; je <epilogue>` 是 4.1 对
//   **`if (!_CheckPlayable()) { 剩余全部 }`** 的形（旧写 `if (!_CheckPlayable())
//   return;` 早返 + 后续平铺，条件方向与块边界均与 ORIG 不符，且 4.4 产 jne 反跳）。
// ndiff 126→103（语义修正后）。
// ============================================================================
#include "CAssaultMgr/LeaveAssaultPlace_1.inc"
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（fresh .o 复验；ORIG 0x082ead6a 453 条）。
    if (index < 0 || index > 7)
        return;

    CUser* user = m_assaulters[index].GetUser();
    if (user != 0)
    {
        // ---- GiveupPanalty 分支（82eadb0；_IsGiveupPanalty true 时）----
        if (_IsGiveupPanalty(penaltyFlag))
        {
            m_assaulters[index].GiveupPanalty();
            if (m_state > 1 && m_state <= 3)
            {
                user->SetCurCharacLeaveTimeout(true);
            }
            else if (m_state > 3 && m_state <= 6)
            {
                PacketGuard packet;
                _MakeUserState(&packet, user);
                G_GameWorld()->send_all(packet);   // ORIG 82eaee3：%eax 直通实参
            }
            // [SweepCAssaultMgr 真语义修正] ORIG 082eadc5 `je 082eaeb8`（目标 =
            // PacketGuard::C1 @+0x14e，**非** _IsBroadcastRunaway 段 +0x11f）
            // 与 Ghidra C（LeaveAssaultPlace.md:523）同证：_IsBroadcastRunaway
            // 在 `if (_IsGiveupPanalty(penaltyFlag))` **块内**（GiveupPanalty →
            // 状态分支 → 逃跑判定）。旧形置于块外 ⇒ _IsGiveupPanalty 为 false 时
            // 仍执行逃跑广播（真语义反向；AE 口径把分支目标归一为 <T> 故不显形）。
            // ---- _IsBroadcastRunaway（82eae89）----
            if (_IsBroadcastRunaway(user))
                _BroadcastRunaway(user, index);
        }

        // ---- 82eaeb8：leave 包 + PowerWar 计数 ----
        // b28d-solo-CAssaultMgr: ORIG -0x24 的 D1 在 082eb06f（收尾链 +0x310
        // 之前、ghost 块 +0x2e8 之后）⇒ 原码作用域覆盖后续全部语句，非独立块
        //（旧形析构紧跟 SendPacket，多 1 个独立 EH cleanup/_Unwind_Resume）。
        PacketGuard packet;
        _MakeLeaveUser(&packet, index);
        SendPacket(packet);
        if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
            user->getCurCharacVill() == 7)
        {
            GlobalData::s_power_manager->GetPowerWarLog()
                ->IncTotalLeaveAssaultUser();
        }

        // ---- close 包：PowerWar 村庄用 powerWarHP，否则用槽位 HP ----
        if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
            user->getCurCharacVill() == 7)
        {
            _SendCloseAssault(user, user->getPowerWarHP());
        }
        else
        {
            _SendCloseAssault(user, m_assaulters[index].GetHp());
        }

        // ---- 82eafb0：槽位非空 → 删除用户 + 用户状态广播 ----
        if (!m_assaulters[index].Empty())
        {
            _DelUser(index, delFlag);
            if (!user->IsCurCharacGhost())
            {
                PacketGuard statePacket;
                _MakeUserState(&statePacket, user);
                G_GameWorld()->send_all(statePacket);   // ORIG 82eb052 同形
            }
        }
    }

    // ---- 82eb07a：收尾链 ----
    // b28d-solo-CAssaultMgr: ORIG +0x310 段无外层 `if(user!=0)` 包裹（user==0
    // 时 %al=0 仍落到 +0x3d0 的 m_field100 检查；旧形多一层包裹 = 真语义差异，
    // 与 Ghidra C 的 if/else-if/else 链一致）。裸作用域仅维持缩进，零代码生成。
    {
        // 非 PowerWar（或非村庄7）且状态<=3 且索引 0/1：双方队伍置 7 后关闭。
        // b28d-solo: ORIG 82eb1xx 段为短路链直接 test（无 bool 槽）⇒ P0 内联。
        // b28d-solo-CAssaultMgr: ORIG 0x82eb07a 段短路物化形
        //（`cmpl user; je F; IsPowerWarEventOn; xor$1; test; jne T; vill; cmp$7;
        //   je F; T: mov$1; F: mov$0; test; je`）⇒ 源形 `user != 0 &&
        //（!IsPowerWarEventOn() || vill != 7)` 嵌套 `m_state <= 3 &&
        //（index==0||index==1)`（旧形缺 user 项且 m_state 并入外层链）。
        if (user != 0 &&
            (!GlobalData::s_power_manager->IsPowerWarEventOn() ||
             user->getCurCharacVill() != 7))
        {
            if (m_state <= 3 && (index == 0 || index == 1))
            {
                if (m_parties[0].m_field00 != 0)
                    ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
                if (m_parties[1].m_field00 != 0)
                    ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
                m_state = (ENUM_ASSAULT_STATE)7;
                CloseAssault();
                return;
            }
        }
        // 82eb13a：m_field100==0 → 同样置 7 后关闭。
        if (m_field100 == 0)
        {
            if (m_parties[0].m_field00 != 0)
                ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
            if (m_parties[1].m_field00 != 0)
                ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
            m_state = (ENUM_ASSAULT_STATE)7;
            CloseAssault();
            return;
        }
        // b28d-solo-CAssaultMgr: ORIG 0x82eb1b0 段 `call _CheckPlayable; xor$1;
        // test; je <epilogue>` = 4.1 对 `if (!_CheckPlayable()) { 剩余全部 }` 的形
        //（je 跳过整个 if 体到 epilogue）；单语句 `if (...) return;` 4.4 产 `jne`
        // 反向跳（旧早返形同此）。其后条件链为两段：链①`IsPowerWarEventOn() &&
        // user!=0 && vill==7` 物化后 test，链②`m_state > 3 && m_state <= 4` 为
        // 嵌套 if（非并入链①）。
        if (!_CheckPlayable())
        {
            if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
                user != 0 &&
                user->getCurCharacVill() == 7)
            {
                if (m_state > 3 && m_state <= 4)
                {
                    int winCnt = 0;
                    int loseCnt = 0;
                    int aliveCnt = 0;
                    if (_CheckPowerWarInPlayer(
                            (ENUM_POWER_SIDE_TYPE)user->getPowerSide(),
                            (ENUM_POWER_WAR_WINNER_OR_LOSER)2, winCnt, loseCnt,
                            aliveCnt))
                    {
                        _SendPowerWarPointToGuildServer(
                            user, (ENUM_POWER_WAR_WINNER_OR_LOSER)2, winCnt,
                            loseCnt, aliveCnt);
                    }
                    CParty* party = (CParty*)user->GetParty();
                    if (party != 0)
                        party->resetStraightVictories();
                    else
                        user->resetStraightVictories();
                    for (int i = 0; i <= 7; ++i)
                    {
                        // ORIG 82eb2e6：`call Empty; test %al; jne <loop-inc>` 正向
                        // 判定（P22/P59：写 !Empty() 会 xor$1 物化多 1 条）。
                        if (m_assaulters[i].Empty())
                            continue;
                        CUser* slotUser = m_assaulters[i].GetUser();
                        if (slotUser->getPowerSide() != user->getPowerSide())
                        {
                            _SendStraightVictories(slotUser);
                            break;
                        }
                    }
                }
            }
            // 82eb349：双方队伍置 6 后结束。
            if (m_parties[0].m_field00 != 0)
                ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
            if (m_parties[1].m_field00 != 0)
                ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
            m_state = (ENUM_ASSAULT_STATE)6;
            EndAssault();
        }
    }
}

// ============================================================
// 批 A（OnlyOrig_Assault 恢复落盘；跳过 GetDistance(Point2d,Point2d) 与
// 6 参 IsIncludeVirtualMoveAreas——唯一 owner 归 CVillageMonsterMgr.cpp）
// ============================================================

// [SweepCAssaultMgr 真型修正] ORIG 本 TU 的 `_Z41__static_initialization_and_
// destruction_0ii`（GI 0x82f027c 调 0x82f023c）**只**含 ios_base::Init +
// __cxa_atexit，**无任何 Point2d::C1Eii 调用** ⇒ ORIG 的这张表是 POD 静态初始化
//（.rodata 0x8c1cce0，无动态 ctor）。旧形 `Point2d[8]`（用户 ctor）使 4.4 发 9
// 次 Point2d::C1Eii 动态初始化（我方 static_init 65 insn vs ORIG 41）。ORIG 的
// 访问形 `mov TABLE(,%eax,8)`（第一处，idx*2 折 scale 8）与 `add %eax,%eax;
// add $0x1,%eax; mov TABLE(,%eax,4)`（第二处，显式 idx*2+1）同为 int 数组证据。
static const int ASSAULT_RECT_PLAYER_POINT[16] = {
    0x000, 0x0f, 0x038, 0x05, 0x054, 0x00, 0x01c, 0x0a,
    0x104, 0x0f, 0x0cc, 0x05, 0x0b0, 0x00, 0x0e8, 0x0a
};                                   // .rodata 0x8c1cce0 实测内容
static const int ASSAULT_RECT_ICON_POINT[2] = { 0x82, 5 };   // .rodata 0x8c1cd20

int Point2d::operator-(const Point2d& o) const
{
    // ORIG 0x82f0486：|dx| + |dy|，abs 为无分支 `sar $0x1f; xor; sub` 三件套
    // ⇒ __builtin_abs 复现（实测 4.4.7 -O0 同族展开；if(x<0)x=-x 在 4.4 恒产
    // cmpl/jns/negl 分支形，多 3 条）。ORIG 全寄存器接力零槽 vs 4.4 store/load
    // 往返属 N82 帧槽族，收敛到指令序同构即止。
    return __builtin_abs(m_x - o.m_x) + __builtin_abs(m_y - o.m_y);
}

#include "CAssaultMgr/Set.inc"
{
    m_field00 = party;
}

// ORIG W 0x82f060a（size 0xa）：mov 0x0(%eax),%eax; ret —— 外联定义（P119）
#include "CAssaultMgr/Get.inc"
{
    return (unsigned int)m_field00;
}

#include "CAssaultMgr/CandidatePoint.inc"
#include "CAssaultMgr/CExtraMatrix.inc"

#include "CAssaultMgr/AddPoint.inc"
{
    m_points.push_back(p);
}

#include "CAssaultMgr/MovePoint.inc"
{
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        // ORIG 82e5999：operator* 结果先物化临时槽 -0xc（`mov %eax,-0xc`），
        // 再 `mov -0xc(%ebp),%eax` 装载 lhs 传 operator+=（4.1 物化形）。
        // 直写 `*it += delta` 被 4.4 折成 eax 直通，少 2 条 ⇒ 具名引用中转。
        Point2d& ref = *it;
        ref += delta;
    }
}

#include "CAssaultMgr/CheckPoint.inc"
                              Point2d& out)        // ORIG 0x82e5b4e
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 2 → AE；路由口径复验）
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e5b4e
    // b28d-solo-CAssaultMgr: P167 新判据（b28c C28L8）——ORIG 2×`deEv`(operator*)
    // 而旧写 `it->m_x/m_y` 产 2×`ptEv`(operator->)；改 `(*it).m_*` 归位。
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        if (IsIncludeVirtualMoveAreas(areas, (*it).m_x, (*it).m_y,
                                      out.m_x, out.m_y, 0))
        {
            return true;
        }
    }
    return false;
}

#include "CAssaultMgr/MoveUser.inc"
{
    // ORIG 0x82e5c0a：每处使用重新装载 fm->m_slotIdx（无 idx 槽）⇒ 表达式直用。
    if (fm->m_slotIdx < 0 ||
        (unsigned int)fm->m_slotIdx >= m_points.size())
        return false;
    fm->m_x = m_points.at(fm->m_slotIdx).m_x;
    fm->m_y = m_points.at(fm->m_slotIdx).m_y;
    fm->ForcedMove();
    return true;
}

#include "CAssaultMgr/CCandidateMatrix.inc"
#include "CAssaultMgr/dtor_CCandidateMatrix.inc"

#include "CAssaultMgr/At.inc"
{
    if (m_points.size() > idx)
    {
        x = m_points[idx].m_point.m_x;
        y = m_points[idx].m_point.m_y;
    }
    return 0;
}

#include "CAssaultMgr/_AddCandidatePoint.inc"
                                          const Point2d& from2,
                                          const Point2d& relTo,
                                          int weight)              // ORIG 0x82e5894
{
    CandidatePoint cp;
    cp.m_point = from1;                        // point ← from1
    cp.m_priority = weight;
    cp.m_priority += from1 - relTo;            // 曼哈顿 op-
    cp.m_priority -= from2 - from1;
    m_points.push_back(cp);
}

#include "CAssaultMgr/MakeCandidatePoint.inc"
{
    // b28d-L3：ORIG 0x82e5ca8 头部两条 `movl $0,-0x10(%ebp); movl $0,-0xc(%ebp)`
    // = 全头部声明 i/j 各带初值（P212），第一循环 init 覆写 `i=1` ⇒ 双初始化形。
    int i = 0;
    int j = 0;
    _AddCandidatePoint(a, a, b, 10);
    for (i = 1; i <= 7; ++i)
    {
        _AddCandidatePoint(Point2d(a.m_x, a.m_y + i * 10), a, b, 10);
        _AddCandidatePoint(Point2d(a.m_x, a.m_y - i * 10), a, b, 10);
    }
    for (j = 0; ::GetMaxCandidateMatirxRow() > j; ++j)
    {
        for (i = 1; i <= 7; ++i)
        {
            _AddCandidatePoint(Point2d(m_points[j].m_point.m_x + i * 10,
                                       m_points[j].m_point.m_y), a, b, 0);
            _AddCandidatePoint(Point2d(m_points[j].m_point.m_x - i * 10,
                                       m_points[j].m_point.m_y), a, b, 0);
        }
    }
}

#include "CAssaultMgr/RemoveCandidatePoint.inc"
{
    std::vector<CandidatePoint>::iterator it = m_points.begin();
    while (it != m_points.end())
    {
        if (!::IsIncludeVirtualMoveAreas(areas, (*it).m_point.m_x,
                                       (*it).m_point.m_y))   // ORIG 用 operator*（de）
        {
            it = m_points.erase(it);
            continue;
        }
        ++it;
    }
}

#include "CAssaultMgr/ArrangeCandidatePoint.inc"
{
    std::sort(m_points.begin(), m_points.end());
}

// ============================================================
// 全局依赖（本 TU 唯一 owner 部分）
// ============================================================

} // namespace pvp_assault

// ORIG T _Z24GetMaxCandidateMatirxRowv 0x82e5c9e（全局函数，非 pvp_assault 成员）。
#include "CAssaultMgr/GetMaxCandidateMatirxRow.inc"
{
    return 15;
}

// ORIG T _ZN7MapArea7includeEii 0x82f0298：矩形包含判定。
#include "CAssaultMgr/include.inc"
{
    // caller 0x82e5ed1 call 后 xor $0x1,%eax（技巧1 bool 专属形）⇒ bool 返回
    // ORIG 0x82f0298：四条件共用尾岛 `mov $0,%eax`（技巧24），true 路径
    // `mov $1; jmp 尾` 在岛前。4 个独立 goto 在 4.4 各产 nop 中继块 ⇒
    // 合并为单 || 短路链，跳边直指共享 false 块。
    if (m_x > x || m_x + m_w < x || m_y > y || m_y + m_h < y)
        goto RET0;
    return true;
RET0:
    return false;
}

// ORIG T _Z25IsIncludeVirtualMoveAreasPSt6vectorI7MapAreaSaIS0_EEii
// 0x82e5ed1：全部区域均包含 (x,y) 才返回 true。
#include "CAssaultMgr/IsIncludeVirtualMoveAreas_1.inc"
{
    for (unsigned int i = 0; i < areas->size(); ++i)
    {
        if (!areas->at(i).include(x, y))
            return false;
    }
    return true;
}

namespace pvp_assault
{

// ============================================================
// 批 B：CAssaulter（ORIG 逐指令转写）
#include "CAssaultMgr/AttachUser.inc"
{
    m_user = user;
    m_team = team;
    _PrepareAssault(user);
}

#include "CAssaultMgr/IsAttachAble.inc"
{
    return m_user == 0;
}

#include "CAssaultMgr/_PrepareAssault.inc"
{
    if (user->getCurCharacR() != 0)
    {
        m_hp = (unsigned char)user->getCurCharacStamina();
        user->SetCurCharacLeaveTimeout(false);
        user->getCurCharacInvenW()->GetCreatureMgrW()->TurnStomach(true);
    }
}

#include "CAssaultMgr/IsPowerWarWinner.inc"
{
    return m_powerWarWinner;
}

#include "CAssaultMgr/SetPowerWarWinner.inc"
{
    m_powerWarWinner = (char)(flag ? 1 : 0);
}

#include "CAssaultMgr/SetUserConnectP2P.inc"
{
    // ORIG 0x82e6a96 call _ZSt3minIiERKT_S2_S2_（(%esp)=&len，0x4esp=&.rodata 8
    // @0x8c1c344 ⇒ static const int 取地址），`mov (%eax),%eax` 解引用返回引用。
    // [W13F 2026-09-11] 符号名实证：ORIG 0x8c1c344 处符号 = `_ZL14MAX_PVP_PLAYER`（r, 4B,
    //   值 0x00000008）。原 `kIpMax` 为**发明符号**（`nm ORIG` 无 `_ZZ...E6kIpMax`）⇒
    //   改用同名文件作用域常量复现同一符号。
    memcpy(m_connectIp, ip, std::min(len, MAX_PVP_PLAYER));
}

#include "CAssaultMgr/Reset.inc"
{
    m_user = 0;
    m_team = (ENUM_ASSAULT_TEAM)0;
    m_state = (ENUM_ASSAULT_STATE)0;
    m_lastHpSec = 0;
    m_powerWarWinner = 0;
}

#include "CAssaultMgr/DieUser.inc"
{
    // ORIG 0x82e6a4b je->leave（无尾岛）：正向 if 使 body 尾自然落入 leave
    if (m_user != 0)
    {
        m_user->DieCurCharac(0, m_user);
        // §4.3 具名化：CUser::m_characterView @+0x796f4（CUser.h:2292 权威声明；
        // 本 TU 其它处（如 giveGhostPenalty）已用 m_user->m_characterView 直访）。
        m_user->m_characterView.enableSaveCharacView();
    }
}

#include "CAssaultMgr/ChangeHp.inc"
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 2 → AE；路由口径复验）
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e6694
    if (hp < 0 || hp > 100)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x18a, 0);
        trace("ASSAULT_CHANGE_HP error (%d)", hp);
        return false;
    }
    // ORIG 0x82e6717：&& 结果 mov$1/0 物化后 test %al（无槽）⇒ 匿名 bool 表达式
    if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
        m_user->getCurCharacVill() == 7)
        m_user->setPowerWarHP((unsigned short)hp);
    else
        m_hp = hp;

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = (int)(curSec - m_lastHpSec);
    m_lastHpSec = curSec;
    // ORIG 0x82e676b：`cmpl $2; jg T; mov $0; jmp E; T: mov $1; E: leave`
    // false 先落、true 跳后岛（技巧23 双 goto 钉块序；4.4 直写折 setg）。
    if (diff > 2)
        goto RET1;
    return false;
RET1:
    return true;
}

#include "CAssaultMgr/GetCharacName.inc"
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 1 → AE；路由口径复验）
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e663c
    // ORIG 0x82e663c：je→尾岛 `mov $""`，call 分支 fall-through 在前（技巧23 双 goto 钉块序）
    if (m_user == 0)
        goto EMPTY;
    return m_user->getCurCharacName();
EMPTY:
    return "";
}


// ============================================================
// 批 A 续：Disposition / Factory / CAssaultPlace 访问器（恢复落盘）
// ============================================================

#include "CAssaultMgr/_MoveMatrix.inc"
{
    for (int j = 0; j <= 1; ++j)
        m_mats[j].MovePoint(delta);
    m_iconPos += delta;
}

#include "CAssaultMgr/GetFlagPoint.inc"
{
    x = m_iconPos.m_x;
    y = m_iconPos.m_y;
}

#include "CAssaultMgr/_CheckMatrix.inc"
                                      Point2d& out)          // ORIG 0x82e64f2
{
    for (int j = 0; j <= 1; ++j)
    {
        if (!m_mats[j].CheckPoint(areas, out))
            return false;
    }
    return IsIncludeVirtualMoveAreas(areas, m_iconPos.m_x, m_iconPos.m_y,
                                     out.m_x, out.m_y, 0);
}

#include "CAssaultMgr/SetVirtualMap.inc"
{
    Point2d probe;
    for (int i = 0; i <= 4; ++i)
    {
        if (_CheckMatrix(areas, probe))
            return true;   // ORIG mov $0x1,%eax
        _MoveMatrix(probe);
    }
    return false;          // ORIG mov $0x0,%eax
}

#include "CAssaultMgr/MoveUser_1.inc"
{
    // ORIG 0x82e65ee：`mov 0xc(%ebp),%eax; mov (%eax),%eax; mov %eax,%edx`
    // 原地两级装载 ⇒ 下标表达式直传（N96 清绑）。残余：4.4 把
    // `mov (%eax),%eax; mov %eax,%edx` 折为 `mov (%eax),%edx`——N18a 单段
    // regmove 4.4 恒折、源形不可造（探针 /tmp/x1.cpp m1），byte 封顶。
    // b28d-L3 试：下标 cast 为 unsigned 触发 4.4.6 转换中转（P211 异型化）。
    return m_mats[(unsigned int)fm->m_field00].MoveUser(fm);
}

// [b23S3C] A：真修 counts[j*4]→counts[j]（原越界读）、P48 内层每次重读 counts[j]、
// icon 字段拆两句对齐 this 重载 store；残余 = N86(4.1 临时+拷贝 vs 4.4 直构形)
// + N87(ARP 索引 add/lea vs 折叠位移) + CDispositionC1vsC2 基构造（跨版本），
// fresh .o 复验 our=154/orig=158，调用点差集仅 C1/C2 同名差，语义等价。
// [round1-L29 2026-09-10 复核] 判据⑪ 槽位直方图逐值全等 ⇒ 非槽问题。唯一残差 = 索引两步寻址
//   `j*4+k`：ORIG `mov j,%edx; mov k,%eax; shl $2,%edx; lea (%edx,%eax,1),%eax` vs 4.4
//   `mov j,%eax; shl $2,%eax; add k_mem,%eax`（add-vs-lea，P30/N4/N102；八形探针全败）⇒ N。
//   ORIG 0x082e6386。
// [S6 2026-09-11 复核] 清单所指「缺 `CDisposition::C1Ev` 调用」= **C1/C2 同址别名假缺口**
//   （非语义）：ORIG 调用点 0x082e62b9 → callee 0x082f1052（14B，体只写 vptr 0x8c1d1c8）；
//   我方同位置 call 重定位到 `_ZN11pvp_assault12CDispositionC2Ev`，本 .o 内 C1Ev/C2Ev
//   皆 0x0e 字节弱符号**同址别名** ⇒ 运行/布局等价；p167 差集 CLEAR（余项仅 C1/C2、
//   D1/D2 别名）。改动前后 158/157、ndiff 17 不变 ⇒ 判 N（无源码旋钮）。
#include "CAssaultMgr/CStaticDisposition.inc"
{
    // b28d-solo-CAssaultMgr: N 封顶（P30/N4/N102 族；清单 DIFF 98 复验）— 用
    // `reinterpret_cast<const int (&)[16]>(ASSAULT_RECT_PLAYER_POINT)[idx*2+1]`
    // 数组引用视图复现 ORIG m_y 的 `add %eax,%eax; add $1,%eax` 链与 scaled
    // `mov (,%eax,4)`（gap 4→1）。残差 = 2 处 add-vs-lea（ORIG
    // `mov k,%eax; lea (%edx,%eax,1),%eax` vs 4.4 `shl; add k_mem,%eax`，P30/N86b
    // 源形不可择）+ 1 条基址载入 `mov $TABLE,%eax`（引用 cast 恒载基址）。
    // 本轮补测 /tmp/probe_arr.cpp 八形（数组引用/指针 cast/数组指针 cast/元素
    // 取址+1/交换加序/元素成员 m_y/整数索引重组）：唯「数组元素成员访问」
    // 产绝对寻址但索引形态差 3 条；其余 cast 形全载基址 ⇒ 无更优源形。
    // 历史穷举并回退的替代形：①union 双视角（Point2d 有用户 ctor，C++98 union
    // 成员不得有构造函数，编译拒绝）；②指针 cast `((const int*)TABLE)[i]`
    // （our 157→159 更差）；③独立 int[16] 表（可消基址载入但 .rodata 重复
    // 0x40B 且两处符号地址不同，AE 归一化仍不等，回退）。
    // 语义等价（int[16] 视图与 Point2d[8] 同布局；idx*2+1 即 .m_y）。
    m_iconPos.m_x = ASSAULT_RECT_ICON_POINT[0];
    m_iconPos.m_y = ASSAULT_RECT_ICON_POINT[1];
    for (int j = 0; j <= 1; ++j)
    {
        // [b23S3C] counts[j]（非 j*4；int* 天然乘 4 字节）；P48 每循环迭代重读
        // counts[j]（ORIG 内层条件先判 counts[j]>k 再 k<=3）；ORIG 具名临时 Point2d
        // p 在 -0x30，独立于末端 origin(-0x28)，故两处各自命名。
        for (int k = 0; counts[j] > k && k <= 3; ++k)
        {
            // [SweepCAssaultMgr 复验] 索引加法序旋钮实测无效（`j*4+k` 与
            // `k+j*4` 同折 `shl; add k_mem`，ndiff 不变）⇒ 残差维持判词：
            // ORIG 082e6386 `mov j,%edx; shl $0x2,%edx; mov k,%eax;
            // lea (%edx,%eax,1),%eax`（两步 lea）+ `mov TABLE(,%eax,4),%edx`
            // 绝对寻址 vs 4.4 `shl; add k_mem` + 引用 cast 基址载入 —— P30/
            // N4/N102 族，八形探针（/tmp/probe_arr.cpp）无更优，封 N。
            Point2d p(ASSAULT_RECT_PLAYER_POINT[(k + j * 4) * 2],
                      ASSAULT_RECT_PLAYER_POINT[(k + j * 4) * 2 + 1]);
            m_mats[j].AddPoint(p);
        }
    }
    Point2d origin(users[0]->get_posX(), users[0]->get_posY());
    _MoveMatrix(origin);
}

#include "CAssaultMgr/dtor_CStaticDisposition.inc"

#include "CAssaultMgr/GetFlagPoint_1.inc"
{
    x = m_anchor.m_x;
    y = m_anchor.m_y;
}

#include "CAssaultMgr/SetVirtualMap_1.inc"
{
    m_matrix0.RemoveCandidatePoint(areas);
    m_matrix1.RemoveCandidatePoint(areas);
    m_matrix0.ArrangeCandidatePoint();
    m_matrix1.ArrangeCandidatePoint();
    return true;           // ORIG mov $0x1,%eax
}

#include "CAssaultMgr/CDynamicDisposition.inc"
{
    // b28d-solo-CAssaultMgr: ORIG `shr %ax` 是 4.1 对 **unsigned short 除以 2**
    // 的 16 位无符号除法降级（探针 /tmp/shr_probe.cpp f6 `g_b = g_a / 2;` 逐条
    // 同形 `movzwl g_a,%eax; shr %ax`）；写 `>> 1` 会被整型提升成 32 位 `sar %eax`
    //（N87）。值域 0..0xffff，`/2` 与 `>>1` 语义等价。
    // 另两处已修：m_users 循环拷贝（+5）、m_matrices 拆具名成员消数组构造循环+EH。
    // b28d-solo-CAssaultMgr: ORIG 0x82e6055 段用循环拷贝 m_users
    //（`movl $0,-0xc; jmp cond; ...; addl $1,-0xc; cmpl $1; setle; test; jne`），
    // 展开赋值恒少 5 条。
    for (int i = 0; i <= 1; ++i)
        m_users[i] = users[i];
    m_anchor.m_x = (unsigned short)m_users[0]->get_posX()
                 + ((unsigned short)m_users[1]->get_posX() / 2);
    m_anchor.m_y = (unsigned short)m_users[0]->get_posY()
                 + ((unsigned short)m_users[1]->get_posY() / 2);
    // ORIG 82e6101..82e6208：每个 MakeCandidatePoint 各自构造 Point2d（P48 去缓存）。
    // b28d-solo-CAssaultMgr 真语义 bug 修正：ORIG 0x82e6189 段 m_matrices[0] 的
    // a=&(-0x24)=Point2d(users[0])、b=&(-0x2c)=Point2d(users[1])（原码实参序
    // 为 (users[0],users[1])，旧写 (users[1],users[0]) 反了）；m_matrices[1]
    // 的 a=&(-0x14)=users[1]、b=&(-0x1c)=users[0] 与旧写一致。
    m_matrix0.MakeCandidatePoint(
        Point2d(m_users[0]->get_posX(), m_users[0]->get_posY()),
        Point2d(m_users[1]->get_posX(), m_users[1]->get_posY()));
    m_matrix1.MakeCandidatePoint(
        Point2d(m_users[1]->get_posX(), m_users[1]->get_posY()),
        Point2d(m_users[0]->get_posX(), m_users[0]->get_posY()));
}

#include "CAssaultMgr/CAssaultPlaceFactory.inc"
#include "CAssaultMgr/dtor_CAssaultPlaceFactory.inc"

// [S6 2026-09-11] 本 TU 三件伴生/假缺口判词（均零改动、无源码旋钮）：
//  ① `StaticPool<CAssaultPlace,128>::CNode::~CNode`（ORIG 0x082f13e8，19B）：ORIG
//     `sub $0x18,%esp`（4.1 保守帧基）vs 4.4 `sub $0x4,%esp`；助记符序与条数全等
//     （NEAR 8=8）⇒ 帧大小不受源侧控制（加 dead 局部会多 store，反劣）⇒ 判 N82。
//  ② `deque<CNode*>::_M_push_back_aux<CNode* const&>`（ORIG 0x082f3cc0，129B=41 条）
//     **无** EH 段；我方 61 条含 `__cxa_begin_catch`/`_Unwind_Resume` 清尾（+20，gap −20）
//     ⇒ 4.1 out-of-line STL 实例差异，判 N89 封顶。
//  ③ `_Z41__static_initialization_and_destruction_0ii`：工具**按符号名跨 TU 错配**
//     （ORIG 该符号有 493 个实例，工具取首个 0x0807ee76 属 PacketTypes）⇒ 逐 TU 读数
//     不可比、我方 ioinit 16 条形态正确 ⇒ 零改动跳过。
#include "CAssaultMgr/AcquireAssaultPlace.inc"
{
    return m_pool.Acquire();
}

#include "CAssaultMgr/FreeAssaultPlace.inc"
{
    m_pool.Free(place);
}

CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory()       // ORIG 0x82f0fa9
{
    static CAssaultPlaceFactory instance;                    // guard 0x943bcc0/0x943bce0
    return &instance;
}

#include "CAssaultMgr/GetAuthenValue.inc"
{
    return m_authenValue;
}

#include "CAssaultMgr/GetAssaultPlaceId.inc"
{
    return m_field104;
}

#include "CAssaultMgr/SetAssaultPlaceId.inc"
{
    m_field104 = id;
}

#include "CAssaultMgr/GetAssaulterCount.inc"
{
    return m_field100;
}

#include "CAssaultMgr/GetAnotherTeamCharacName.inc"
{
    // ORIG 0x82eb3ba：`call Empty; xor $1; test; je ""岛` ⇒ 正向 !Empty 块；
    // 循环内两侧各自 GetTeam（无 myTeam 变量），第一结果落 %ebx（0x82eb3f3
    // `mov %eax,%ebx`），比较 `cmp %eax,%ebx; setne %al`（技巧22/28 组合）。
    if (!m_assaulters[idx].Empty())
    {
        for (int j = 0; j <= 7; ++j)
        {
            if (m_assaulters[j].GetTeam() != m_assaulters[idx].GetTeam())
            {
                CUser* u = m_assaulters[j].GetUser();
                if (u != 0)
                    return u->getCurCharacName();
            }
        }
    }
    return "";
}

#include "CAssaultMgr/AbuseUser.inc"
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 1 → AE；446h46 路由口径）
    // b28d-L3: IDENTICAL_AE — b28d-L3：446h46 路由后 fresh 复验 AE（TSV 的 NEAR 系 447 口径 stale 伪差
    // ORIG 0x82eb5e2 转写：检查段第一次 getCurSec（`movl $0x941f714,(%esp)`），
    // 块内**第二次独立** getCurSec 重算差值入 callee-saved %esi（P48 双臂
    // 双 call），getCurCharacNo 入 %ebx；cMyTrace 用**临时链式**（P37：
    // varargs 表达式分到 callee-saved，命名对象则提前落槽多装载）。
    // 比较 `cmp $0x13,%eax; setle %al; test; je`（P51 setcc 物化）。
    if ((int)(GlobalData::s_systemTime_.getCurSec() -
              m_abuseSec) <= 19)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc23, 0)(
            "CAssaultPlace::AbuseUser() charac_no(%d), timeout sec(%d)",
            user->getCurCharacNo(),
            (int)(GlobalData::s_systemTime_.getCurSec() - m_abuseSec));
    }
    if (m_state == 4)
    {
        for (int j = 0; j <= 7; ++j)
        {
            if (m_assaulters[j].Empty())
                continue;
            // ORIG 82eb703：两次 GetUser 均无槽往返（P0 ⇒ 原码无具名局部，
            // 值右值直传）：第一次直落 getHackAnalyzer receiver 链，第二次
            // 结果经 %ebx 中转作 arg1（4.1 冗余 regmove=N18a 已证不可造，
            // 但 b28 实测**实参位内联** 4.4 自然分配 %ebx 逐条全等；
            // register asm("ebx") 绑定违例已按 P0/N96 清除）。共 2 次 GetUser。
            m_assaulters[j].GetUser()
                ->getHackAnalyzer()
                ->addServerHackCnt(m_assaulters[j].GetUser(),
                                   (WongWork::ENUM_HACKTYPE)0x32e, 1, 0, 0);
        }
        m_state = (ENUM_ASSAULT_STATE)6;
        // [SweepCAssaultMgr 真语义修正] ORIG 082eb672 `cmp $0x4; jne 082eb76d`
        // （目标 = epilogue）与 Ghidra C（AbuseUser.md:154-171）同证：party
        // SetAssaultState(6)×2 与 TimeoutCloseAssault **在 `if (m_state == 4)`
        // 块内**。旧形置于块外 ⇒ m_state != 4 时仍改 party 状态并强制关场
        // （真语义反向；AE 口径把分支目标归一为 <T> 故不显形）。
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
        TimeoutCloseAssault();
    }
}

// ============================================================================
// CAssaultPlace 构造 / 析构（ORIG W C1/C2 0x82f0614 size 0x6c、D1/D2 0x82f13b6
// size 0x31）。ORIG ctor：CAssaulter[8] 循环默认构造 → Point2d::C1 于 this+0x110
// （m_posBase）→ PartyInfo[2] 循环构造；其余字段留待 Reset()/Acquire 初始化。
// ORIG dtor：仅反向析构 assaulters[8]（Point2d/PartyInfo 析构平凡）。
// [SweepCAssaultMgr 迁移] C1 定义已迁至 W6_AssaultPlaceDepsA.cpp（Main 批准）。
//   依据（探针 /tmp/p_sp1..p_sp8，4.4.7 与 4.1.2 双编译器）：T 的默认 ctor 体在
//   StaticPool 实例化 TU 内**可见** ⇒ 编译器判 CNode ctor nothrow ⇒ 省掉
//   `new CNode[N]` 的异常回滚段；仅**声明**（定义在他 TU）⇒ 保守布 cleanup。
//   ORIG StaticPool<pvp_assault::CAssaultPlace,128>::C1 @0x82f1462（128 insn）
//   含完整 cleanup（0x82f14f9..0x82f153d：CNodeD1 循环 + _ZdaPv + _Unwind_Resume）；
//   本 ctor 定义在场时我方仅 103 insn（gap 25）。迁移后逐条对齐（P284）。
// ============================================================================

#include "CAssaultMgr/dtor_CAssaultPlace.inc"
{
    // b28d-solo-CAssaultMgr: AE（原 NEAR 2，旧 N-帧判词作废）——P210 三分律②：
    // 把 CAssaulter::~CAssaulter 由头内 inline 空体改为本 TU 外联定义并置于本
    // caller 之前（CAssaultMgr.cpp:189），4.4 对 assaulters[8] 析构循环由保守
    // sub $0x14 翻为精确继承 ORIG 0x82f13b6 的 sub $0x4，逐条 AE。
    // [b28d-PoolSmall 复验+反例] 本 TU 内把本 dtor 定义后置到 CloseAssault 之后
    // （P210 三分律③方向）实测**零效**：StaticPool<CAssaultPlace,128>::CNodeD1
    // 仍 `sub $0x4`（ORIG 0x82f1389 为 `sub $0x18`）——gcc4.4 在 finish_file 统一
    // 输出 cgraph，COMDAT 发射点不受源码顺序影响 ⇒ 该件残差纯为 N115（4.4 -O0
    // 恒预留对齐 outgoing 区，4.1 参数直写帧底），唯一解是 TU 级
    // `-mpreferred-stack-boundary=2`（会塌陷同 TU 其它帧）⇒ 已回滚位置，封 N115。
    // [round5-A29 + E02DungeonAltarMgr 双实验定论，2026-09-10] E02 另从「让 callee
    // 不再是可精确继承的简单符号」方向攻同一件：给 StaticPool.h 的 `CNode` 加用户空
    // 析构 `~CNode(){}`（+ 预留死局部撑帧），fresh 实测（19:37:08）**仍 `sub $0x4`**、
    // TU 四数逐值不变，E02 已回退该头。⇒ 两条路（源码顺序后置 / 加用户 dtor）全部
    // 证伪：4.4 的帧继承只认「callee 定义对该 COMDAT 实例化点是否可见」，而本 TU 内
    // 该定义恒可见（`T::~T` 外联定义与模板实例同 TU）⇒ 该件封 N115/P210②，
    // 后续泳道勿再为此烧轮次（除非把 `CAssaultPlace::~CAssaultPlace` 定义迁出本 TU，
    // 那属跨 TU 迁定义、需 Main 裁决且收益仅 1 件）。
    // ORIG 侧比对：`CAssaultPlaceD1Ev` @0x082f13b6 size 0x31（**sub $0x4**，与本 TU
    // 一致、本函数 AE）vs `StaticPool<CAssaultPlace,128>::CNodeD1Ev` @0x082f13e8
    // size 0x13（**sub $0x18**，保守）——ORIG 自身两件帧不同，正是「发射点可见性」
    // 差异的化石证据（ORIG 的 CNode 实例由看不到 `~CAssaultPlace` 定义的 TU 发射）。
}

// ORIG T 0x82e6d3a：m_pos.x/y 非零才向所在区域广播。
#include "CAssaultMgr/_SendPacketToArea.inc"
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet);
}

// ORIG T 0x82e6dce：同门槛；带用户列表的 4 参 send_to_area 版。
//   求值序（报告汇编 +0x25/+0x2e）：m_y 先入 esi、m_x 后入 ebx（右→左），
//   与 send_to_area(int vill, int area, ...) 形参序一致。
#include "CAssaultMgr/_SendPacketToArea_1.inc"
                                      std::vector<CUser*>& users)
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet, users);
}

// ORIG T 0x82e8afc：包 0x86 图标列表 = put_header(0,0x86) + put_short(1) +
// MakeViewAssaultIconOnePlayer + finalize(true)。
#include "CAssaultMgr/_MakeViewAssaultIconPlayer.inc"
{
    packet->put_header(0, 0x86);
    packet->put_short(1);
    MakeViewAssaultIconOnePlayer(packet, flag);
    packet->finalize(true);
}

// ORIG T 0x82e79f6：_MakeUserState 后 G_GameWorld 全服广播。
#include "CAssaultMgr/_SendUserState.inc"
{
    PacketGuard packet;
    _MakeUserState(&packet);
    G_GameWorld()->send_all(packet);
}

// ORIG T 0x82e8c5a：包 0x03 全场用户状态；先记录写位置，尾部回填人数。
// [round1-L29 2026-09-10 复核] 同 2 参版：1 处 N7 寄存器角色互换 + 尾部 epilogue 1 条
//   （ORIG 0x082e8c5a）。判据① CLEAR（16 vs 16 分支，目标无错位）。
#include "CAssaultMgr/_MakeUserState_1.inc"
{
    // b28d-solo-CAssaultMgr: N 封顶（N7 寄存器角色互换；清单 DIFF 27 复验）
    // 同双参重载：ORIG 0x82e8c5a+0x0a9 `call GetUserState; mov %eax,%edx;
    // mov packet,%eax; ...`（73 条）vs 4.4 this-first（72 条）——单条
    // %eax↔%edx 角色互换，语义等价。探针矩阵同双参件（/tmp/probe_mus*.cpp）。
    // [S6 2026-09-11 复核] 同判词：p167 CALLSITE CLEAR（12/12）⇒ N7，零改动。
    // ORIG 0x82e8c5a +0x0b：`movl $0,-0x14; movl $0,-0x10` 双清零在前、
    // get_index 覆写 index 在后（dead init 照抄 P171，与 MakeView 同法）。
    packet->put_header(0, 3);
    int index = 0;
    int count = 0;
    index = packet->get_index();
    packet->put_byte(count);
    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // P48 去缓存每用点重取：ORIG 0x82e8c5a 对 GetUser() 两次独立重调
        //（+0x079 unique_id / +0x0a4 GetUserState），不缓存进具名局部。
        packet->put_short((int)(m_assaulters[i].GetUser()->get_unique_id()
                                & 0xffff));
        // ORIG +0x0a9 GetUserState 结果先借 edx（mov %eax,%edx）再重载 this
        // 于 %eax；本地实现为 eax/edx 换位（寄存器分配 N-residual，语义等价）。
        packet->put_byte(m_assaulters[i].GetUser()->GetUserState());
        ++count;
    }
    packet->put_byte(index, count);
    packet->finalize(true);
}

// ORIG T 0x82e6e2c：移除指定槽位用户并联动村庄/断线检测子系统。
#include "CAssaultMgr/_DelUser.inc"
{
    // b28d-L3: IDENTICAL_AE — b28d-L3：正向 wrap `if (delUser != 0)` 消函数尾 jmp 中继
    // b28 BL6: N96/P0 清理 register asm 绑定违例，还原自然形。ORIG +0x24 的
    // `cmp $4; setg %al; test %al; je` 无槽物化岛（setcc-fold 负例族）源形
    // 不可造，残差封顶。
    if (flag)
    {
        if (m_assaulters[index].GetState() > (ENUM_ASSAULT_STATE)4)
        {
            // b28 BL6: P0——ORIG 内层 GetUser 无槽往返（4.1 走 %ebx 直缓存，
            // 禁绑定形）⇒ 实参右值直传；嵌套 if 裸形 `cmp $4; setg; test; je`
            // 与 ORIG 同形（f1 探针实证）。
            disconnect_detecter::GetInstanceDisconnectDetecter()->UnregisterUser(
                m_assaulters[index].GetUser());
        }
    }
    int team = m_assaulters[index].GetTeam();
    CUser* delUser = m_assaulters[index].GetUser();
    m_assaulters[index].DetachUser();
    m_field100 -= 1;
    // b28d-L3：ORIG +0xb5 为正向 wrap `if (delUser != 0) {…}`（早退 return 形
    // 在 4.4.6 于函数尾留一条 jmp 中继，ORIG 无）；EndPvP 段用负条件包裹
    // （ORIG 条件成立 `jne` 直跳 EndPvP 之后，goto 岛形多一条尾 jmp）。
    if (delUser != 0)
    {
        if (delUser->getCurCharacR() != 0)
        {
            // ORIG +0x0f2：EndPvP 段 getCurSec 差值落 %edx 跨装载直落 0xc(%esp)
            // （P44 变参即时求值，无 now 槽）；+0x13d G_GameWorld 结果 %eax 直通
            // receiver（P89 合并形，无 world 槽）；area 参数 get_area 结果直落。
            // b28d-L3 实测：负条件包裹（`if (!(A&&B))`）在 4.4.6 产 xor$1+极性
            // 反转（残 7）；goto 尾标签形条件段全等但 body 尾多 1 条 jmp（残 1）。
            // ORIG 布局 = 条件真时 `jne` 跳过 body、body 直落 ⇒ 空 then + else
            // body 形（4.4.6 下 jump-thread 消掉尾 jmp）。
            if (GlobalData::s_power_manager->IsPowerWarEventOn()
                && delUser->getCurCharacVill() == 7)
            {
            }
            else
            {
                delUser->m_historyLog.EndPvP(
                    (ENUM_PVP_HISTORY_REASON)5, m_teamFlag[team],
                    GlobalData::s_systemTime_.getCurSec() - m_abuseSec);
            }

            Village* village = G_GameWorld()->getUserVillage(delUser);
            if (village != 0)
            {
                Area* area = village->getArea(delUser->get_area(false));
                if (area != 0)
                {
                    area->send_area_users(delUser);
                    GetInstanceAssaultMgr()->OnMoveArea(delUser);
                }
            }
            GlobalData::s_villageObjectMgr->on_move_area(delUser);
            GlobalData::s_villageMonsterMgr->OnMoveArea(delUser);
            delUser->SetAssaultPlace(0);
        }
    }
}

// ORIG T 0x82ea188 size 0x191：超时关场。trace 打印全场角色名；
// state 5/6 时两队下发 SetAssaultState(7)、置 state=7 并 CloseAssault()。
#include "CAssaultMgr/TimeoutCloseAssault.inc"
{
    // b28d-solo-CAssaultMgr: **IDENTICAL_AE（清单 DIFF 64 → AE）** — 唯一残差
    // 为 ORIG 082ea21a `mov 0x10c(%eax),%eax; mov %eax,%ebx`（m_state 载入后
    // eax→ebx 中转）vs 4.4 恒折 `mov 0x10c(%eax),%ebx`。修法：trace 实参
    // `(unsigned int)m_state` —— 探针 /tmp/probe_trace.cpp 三形实测：int 字段
    // 单段直落 %ebx，enum 字段与 unsigned cast 均产两段（`mov mem,%eax;
    // mov %eax,%ebx`=ORIG 形）⇒ 异型转换中转复现（P254/P265 同族）。
    // b28d-solo-CAssaultMgr: ORIG 0x082ea188 先把 8 个 GetCharacName 实参求值
    // （-0x40..-0x2c 槽 + edi/esi），**之后**才 ctor cMyTrace（+0x30 段）⇒ 原码是
    // 全表达式临时链（P227/P37），具名对象恒 ctor 先于实参求值（DIFF 主因）。
    cMyTrace(__PRETTY_FUNCTION__,
             0x9ea, 0)("TimeoutCloseAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
          (unsigned int)m_state,
          m_assaulters[0].GetCharacName(), m_assaulters[1].GetCharacName(),
          m_assaulters[2].GetCharacName(), m_assaulters[3].GetCharacName(),
          m_assaulters[4].GetCharacName(), m_assaulters[5].GetCharacName(),
          m_assaulters[6].GetCharacName(), m_assaulters[7].GetCharacName());

    // b28d-solo-CAssaultMgr: ORIG 尾块 `call CloseAssault` 后直落 epilogue；
    // 早退 return 使 4.4 尾块补 `jmp <epilogue>; nop`（+2）。改单出口正向包裹
    // （`==5 || ==6` 短路链）复现 ORIG `cmp $5;je body; cmp $6;jne epilogue`。
    if (m_state == 5 || m_state == 6)
    {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
        m_state = (ENUM_ASSAULT_STATE)7;
        CloseAssault();
    }
}

// ORIG T 0x82e6ab8（报告 Reset.md）：全场复位；归还池前调用。
#include "CAssaultMgr/Reset_1.inc"
{
    // ORIG 0x82e6abe/af：函数头 `movl $0,-0xc` 双写 = 外提 i 的 dead init +
    // 第一循环 init；第二循环（0x82e6ba7）复用同槽第三写 ⇒ 共享具名 i。
    int i = 0;
    for (i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].GetUser() != 0)
            m_assaulters[i].GetUser()->SetAssaultPlace(0);
        m_assaulters[i].Reset();
    }
    m_field100 = 0;
    m_field104 = 0;
    m_authenValue = 0;
    m_state = (ENUM_ASSAULT_STATE)0;
    // ORIG 0x82e6b5d：Point2d(0,0) 临时构造（C1Eii sret -0x14/-0x10）后
    // 两字拷入 0x110/0x114 ⇒ 临时对象赋值形。
    m_posBase = Point2d(0, 0);
    m_pos.m_x = 0;
    m_pos.m_y = 0;
    // ORIG 0x82e6ba7：teamFlag 第二循环复用 i 槽。
    for (i = 0; i <= 1; ++i)
        m_teamFlag[i] = 0;
    m_winTeam = (ENUM_ASSAULT_TEAM)0;
    m_checkUser = 0;   // ORIG *(int*)(this+0x134)=0：该槽用户指针清零
    m_abuseSec = 0;
}

// ORIG T 0x82edb6a：map 摘除 + place->Reset() + 归还 StaticPool。
#include "CAssaultMgr/FreeAssaultPlace_1.inc"
{
    // ORIG 0x82edb6a：id==0 的 je 直接落到 place 检查块（0x82edbde），
    // 无尾 jmp 岛 ⇒ 两个正向 if 块，非 early-return。
    if (id != 0)
    {
        std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
        if (it != m_places.end())
            m_places.erase(it);
    }
    if (place != 0)
    {
        place->Reset();
        GetInstanceAssaultPlaceFactory()->FreeAssaultPlace(place);
    }
}

// ORIG T 0x82e9ad6 size 0x52d：关场主流程 —— 全员按 PowerWar/普通分支
// 发关闭包并回补体力，广播状态与图标， ghost 记录，定时两队重载，
// 清场后归还池。
// b28d-solo-CAssaultMgr: N 封顶（§6.2 三件套，S174 复验）— P167 calls 60 vs 60
// **CLEAR**；gap -15→-3（372 vs 375）、ndiff 593→35。本轮修复：
// ①4 个 bool 局部（powerWarVill/gainChaosKill/powerWar/reloadParty）内联为
//   短路条件（ORIG 无 bool 槽、我方 cmpb×4 全消）；②ghost 分支改正极性
//   `if (IsCurCharacGhost())`（ORIG 082e9bd6 正极性，消 xor$1）；
// ③P191 双初始化：`int i=0` 函数头声明、两循环复用（对齐 ORIG 082e9ade 双
//   初始化 + 两处 `movl $0,-0x28`）；④now/chaos/level/sps 内联（ORIG 无槽往返，
//   P0）；⑤stamina 内联（ORIG `movzbl %al,%edx` 直传）。
// 残差仅 N18b 比较操作数序（ORIG `cmp %edx,%eax`）、N121 尾 jmp 岛 ×2、
// N104 EH 块相对序（ORIG cleanup 居中 vs 我方居末），无语义缺口。
// b28d-solo-CAssaultMgr（2026-09-09）: N 封顶（§6.2 三件套；清单 DIFF 235）。
// 本轮修正：`PacketGuard packet` 由函数级作用域改**独立块**——ORIG 082e9ec0
// 在 _SendPacketToArea 后立即 `jmp` 跳过 landing pad 直落 D1 ⇒ 原码是独立块
//（旧形析构落函数尾，多尾岛 ×2 + EH 块位置整体偏移；归一化 ratio 0.449→0.572）。
// 修正后 clean 口径 orig 374 / ours 379（+5 = N22 比较段 1 条 + N121 尾
// `jmp+nop` 岛 ×2）；p167 调用点差集 CLEAR（ORIG 60 = OUR 60）；帧 `sub $0x40`
// 双方一致。残差族：N22/N18b（`call GetTeam; mov this,%edx; mov 0x124(%edx),%edx;
// cmp %edx,%eax` vs 4.4 `mov %eax,%edx; ...; cmp %eax,%edx`，字段/返回异型旋钮
// 需改冻结头，S2 归 Main）+ N121（循环尾与 FreeAssaultPlace 后 `jmp+nop` 岛，
// 三形实测不消）+ N123/重定位（`mov 0x0,%eax` 系 .o 未解析 R_386_32，P264）。
// 无语义缺口。
// [round1-L29 2026-09-10 复核] difflib 唯一差异 = 比较两操作数装载序（ORIG 0x082e9ad6：
//   `mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx; cmp %edx,%eax` = call 结果留 %eax；
//   我方 `mov %eax,%edx; mov this,%eax; mov 0x124(%eax),%eax; cmp %eax,%edx`）⇒ N22/N18b 封顶。
//   p167：calls 60=60；MISSING/EXTRA 仅 PacketGuard C1/C2·D1/D2 同址别名（P93 良性）。
// [round5-A29 破 N · 类型旋钮净账留痕，2026-09-10] 上述 N22/N18b 判词**已作废**：按
//   P254/P248档③ 对 `CAssaultPlace::m_state`(+0x10c) 与 `m_winTeam`(+0x124) 做 **enum 同型化**
//   （本 TU 侧先落 7 处显式 cast：`m_state = (ENUM_ASSAULT_STATE){0,6,7}` ×6 +
//   `m_winTeam = (ENUM_ASSAULT_TEAM)0`）⇒ 4.4 恢复 ORIG 的两段装载
//   `mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx; cmp %edx,%eax`，**本件 DIFF→AE 归零**。
//   **净账（Main「类型旋钮先算两侧 identical 增量」红线）**：CAssaultMgr TU
//   `identical 109→109（不降） / ae 292→293（+1） / near 1→1 / diff 6→5（−1）` ⇒
//   **净 identical ≥ 0 且总非 identical 下降**，合规保留。字段侧为 enum（非返回类型）
//   ⇒ 定义侧 store 无归一化插入（`(ENUM)7` 直存），符合「两侧同核」要求。
#include "CAssaultMgr/CloseAssault.inc"
{
    // b28d-solo: ORIG 0x082e9ade/0x082e9ae5 头部双初始化 `movl $0,-0x28;
    // movl $0,-0x24` + 两循环各再 `movl $0,-0x28` ⇒ P191 双初始化形：
    // i 在函数头声明并初始化，两处 for 复用（`for (i=0;…)`）。
    int i = 0;
    int lastVill = 0;
    // [SweepCAssaultMgr 真语义修正] ORIG 082e9af7 `test %eax,%eax; jle +0x396`
    // 的落点 = 082e9e6c（PacketGuard::C1）⇒ m_field100<=0 只跳过「for 循环 +
    // _SendUserState」，**仍执行** 图标广播块 / party 定时器块 / _DelUser 循环 /
    // FreeAssaultPlace。旧形 `if (m_field100 <= 0) return;` 会连归还池一起跳过
    // （漏 FreeAssaultPlace ⇒ 泄漏）。Ghidra C 同证 `if (0 < *(int*)(this+0x100)){…}`。
    if (0 < m_field100)
    {
    for (i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        CUser* slotUser = m_assaulters[i].GetUser();
        // [SweepCAssaultMgr 真语义修正] ORIG 082e9b36 `cmpl $0,-0x20(%ebp);
        // je 082e9c57`（目标 = SetAssaultPlace 段 0x82e9c57，**非**循环增量）
        // ⇒ slotUser==0 只跳过本处理块，仍执行 SetAssaultPlace/IsPowerWarWinner/
        // GetTeam 比较/ghostTime/_DelUser（Ghidra C 同证 `if (local_24 != 0) {…}`
        // 后继续）。旧形 `if (slotUser == 0) continue;` 的 je 落循环增量
        //（我方 0x3602 nop）——clean 口径把分支目标归一为 <T> 故不显形，属
        // jcc 落点差异（真语义反向）。
        if (slotUser != 0)
        {
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() == 7)
        {
            if (!m_assaulters[i].IsPowerWarWinner())
                ForcedMoveAtPowerWarPlace(slotUser);
            _SendCloseAssault(slotUser,
                              (int)(slotUser->getPowerWarHP() & 0xffff));
        }
        else
        {
            if (slotUser->getCurCharacR() != 0)
            {
                // b28d-solo: ORIG 082e9bd6 正极性 `test %al; je <非ghost>`
                // ⇒ 源码为 `if (IsCurCharacGhost())`（ghost 分支直落）。
                if (slotUser->IsCurCharacGhost())
                {
                    // b28d-solo: ORIG 082e9bf9 `movzbl %al,%edx` 无 stamina 槽
                    //（P0 内联右值；具名 uchar 局部恒多 store+load 两条）。
                    slotUser->setCurCharacStamina(
                        (unsigned char)_SendCloseAssault(slotUser, 100));
                }
                else
                {
                    slotUser->setCurCharacStamina(
                        (unsigned char)_SendCloseAssault(
                            slotUser, m_assaulters[i].GetHp()));
                }
            }
        }
        lastVill = (int)slotUser->getCurCharacVill();
        }

        // b28d-solo-CAssaultMgr: ORIG 0x082e9c63 在 SetAssaultPlace 前**重取**
        // `m_assaulters[i].GetUser()`（P167 MISSING ×1；ORIG 调用点直读展开，
        // 非缓存 slotUser）。
        m_assaulters[i].GetUser()->SetAssaultPlace(0);
        if (m_assaulters[i].IsPowerWarWinner())
        {
            if (slotUser->getPowerWarHP() < 0x1e)
                slotUser->setPowerWarHP(0x1e);
        }

        // [SweepCAssaultMgr] 本比较段残差 = N7/N18a 寄存器角色互换：ORIG
        // 082e9cc2 `call GetTeam; mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx;
        // cmp %edx,%eax` vs 4.4 `mov %eax,%edx; mov 0x8(%ebp),%eax;
        // mov 0x124(%eax),%eax; cmp %eax,%edx`。P226 语句表达式形实测反劣
        // （372→376：多 sete/test/je 物化）已回滚；W6 EvadeAssaultPlace 同残差
        // 亦封 N7（P321-T5）。
        if (m_assaulters[i].GetTeam() == m_winTeam
            && !GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() != 7)
        {
            // b28d-solo: ORIG 082e9d07 无 now 槽（`call getCurSec` 结果直压实参）
            // ⇒ P0 内联右值。
            slotUser->SetCurCharacChaosKillTime(
                GlobalData::s_systemTime_.getCurSec());
        }

        int ghostTime = 0;
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            || slotUser->getCurCharacVill() == 7)
        {
            float penalty =
                GlobalData::s_power_manager->GetPowerWarResponPenalty(slotUser);
            ghostTime =
                GlobalData::s_power_manager->GetPowerWarGhostTime(slotUser,
                                                                 penalty);
        }
        else
        {
            // b28d-solo: ORIG 082e9d90 段把 chaos→%esi、level→%ebx 跨调用保活
            // 且无槽往返（P0/P52）⇒ 两实参内联，sps 亦内联为成员访问（+0x68）。
            ghostTime = G_CDataManager()->m_serverParameter.getAssaultGhostTime(
                slotUser->get_charac_level(),
                slotUser->GetCurCharacChaosPoint());
        }

        // b28d-solo: ORIG 082e9dcb `test %al,%al; je` 直落循环尾（bool 正极性，
        // 短路两守卫同落点）⇒ 去掉 `!= 0` 以免 4.4 生成块尾 jmp 中继。
        if (slotUser->IsCurCharacGhost() && ghostTime != 0)
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            slotUser->SetCurCharacChaosDieTime(now);
            slotUser->SetCurCharacChaosResponTime(now + ghostTime);
            slotUser->m_characterView.enableSaveCharacView();
            CAssaultMgr::NotifyGhostTime(slotUser, ghostTime, ghostTime);
        }
    }
    _SendUserState();
    }

    // b28d-solo-CAssaultMgr: ORIG 082e9ec0 在 _SendPacketToArea 后**立即**析构
    // -0x34（jmp 跳过 landing pad 直落 D1）⇒ 原码是独立块，非函数级作用域
    //（旧形析构落函数尾，多 `jmp+nop` 岛 ×2 + landing 位置偏移）。
    {
        PacketGuard packet;
        _MakeViewAssaultIconPlayer(&packet, false);
        _SendPacketToArea(packet);
    }

    if (GlobalData::s_power_manager->IsPowerWarEventOn() && lastVill == 7)
    {
        // b28d-solo-CAssaultMgr: ORIG 0x082e9f01/0x082e9f4e 两处 `call
        // PartyInfo::Get` 后 `mov %eax,-0x10/-0xc; cmpl $0,-...`（具名 party
        // 局部），非 m_field00 直读（P119 恢复外联 Get 调用点）。
        CParty* party0 = (CParty*)m_parties[0].Get();
        if (party0 != 0)
        {
            TimerCompleteLoadAfterAssaultTimeout::registNextTimer(
                0x3c, party0->GetPartyIndex(),
                party0->gen_timer_key((TIMER_MESSAGE)0x2a));
        }
        CParty* party1 = (CParty*)m_parties[1].Get();
        if (party1 != 0)
        {
            TimerCompleteLoadAfterAssaultTimeout::registNextTimer(
                0x3c, party1->GetPartyIndex(),
                party1->gen_timer_key((TIMER_MESSAGE)0x2a));
        }
    }

    for (i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        _DelUser(i, true);
    }
    GetInstanceAssaultMgr()->FreeAssaultPlace(m_field104, this);
}

} // namespace pvp_assault

// ORIG T 0x0863771a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x2a,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 CompleteLoadAfterAssault 超时重载。
#include "CAssaultMgr/registNextTimer.inc"
                                                           int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x2a, (int)sec, (unsigned int)p2,
                                0);
}
