// df_game_r Game/ pvp_assault::CAssaultMgr 实现（批3：从 GameStubs.cpp 迁移）。
// 语义依据 ORIG：OnMoveArea 0x82ef610 / SendPacket 0x82efb54 /
// OnLeaveAssaultPlace 0x82ef176 / _GetAssaultPlace 0x82ed9fe。
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
bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y);

namespace pvp_assault
{

CAssaultMgr* GetInstanceAssaultMgr()
{
    // ORIG _ZN11pvp_assault21GetInstanceAssaultMgrEv：guard 单例。
    static CAssaultMgr s;
    return &s;
}

CAssaultPlace* CAssaultMgr::_GetAssaultPlace(unsigned int id)
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

int CAssaultMgr::OnMoveArea(CUser* user)
{
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

int CAssaultMgr::SendPacket(CUser* user, bool flag, PacketGuard* packet)
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

int CAssaultMgr::OnLeaveAssaultPlace(CUser* user, bool flag)
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

void CAssaultMgr::SetAssaultChannel(bool flag)
{
    // ORIG W _ZN11pvp_assault11CAssaultMgr17SetAssaultChannelEb @0x08376048：
    // movzbl flag → *(unsigned char*)this = flag（写入 +0x00 通道开关）。
    m_bAssaultChannel = flag;
}

bool CAssaultPlace::GetTeam(CUser* user, ENUM_ASSAULT_TEAM& team)
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
CAssaulter::~CAssaulter()
{
}

CUser* CAssaulter::GetUser()
{
    // ORIG W 0x82f0542：mov (%eax),%eax
    return m_user;
}

// ORIG W 0x82f055c：call IsAttachAble（0x82f054c）。
bool CAssaulter::Empty()
{
    return IsAttachAble();
}

ENUM_ASSAULT_TEAM CAssaulter::GetTeam()
{
    // ORIG W 0x82f0570
    return m_team;
}

unsigned short CAssaulter::GetUserUniqueId()
{
    // ORIG T 0x82e6662：user ? user->get_unique_id() : 0
    if (m_user == 0)
        return 0;
    return (unsigned short)(m_user)->get_unique_id();
}

void CAssaulter::SetState(ENUM_ASSAULT_STATE state)
{
    // ORIG W 0x82f057c
    m_state = state;
}

ENUM_ASSAULT_STATE CAssaulter::GetState()
{
    // ORIG W 0x82f058a
    return m_state;
}

int GetDirectionByTeam(ENUM_ASSAULT_TEAM team)
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

// ---- CAssaultPlace 内部前置函数 ----

void CAssaultPlace::_GetTeamCount(int* counts)
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

bool CAssaultPlace::_CheckPlayable()
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

bool CAssaultPlace::CheckPlayable()
{
    // ORIG W 0x82f06ea：tail-call _CheckPlayable。
    return _CheckPlayable();
}

int CAssaultPlace::GetState()
{
    // ORIG W 0x82f06fe：读 +0x10c。
    return m_state;
}

bool CAssaultPlace::IsArea(int vill, int area)
{
    // ORIG W 0x82f06ae：比较 +0x118/+0x11c（Point2d）。
    return m_pos.m_x == vill && m_pos.m_y == area;
}

int CAssaultPlace::_GetUserIndex(CUser* user, bool strict)
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

bool CAssaultPlace::ExistUser(CUser* user)
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

bool CAssaultPlace::SendPacket(PacketGuard& packet)
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

bool CAssaultPlace::SendPacket(PacketGuard& packet, ENUM_ASSAULT_TEAM team)
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

void CAssaultPlace::MakeViewAssaultIconOnePlayer(PacketGuard* packet, bool flag)
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

void CAssaultPlace::LeaveAssaultPlace(CUser* user, bool flag)
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
int CAssaulter::GetHp()
{
    return m_hp;
}

// ORIG T 0x82e6812：卸下 creature（TurnStomach false）+ 清 user/team 槽位。
void CAssaulter::DetachUser()
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
void CAssaulter::GiveupPanalty()
{
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
bool CAssaultPlace::_IsGiveupPanalty(bool flag)
{
    // ORIG 0x82eacf4：GetRepeatEvent 返回 %eax 直通 vtable 装载（无 event 槽）
    // ⇒ 匿名链式调用。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        return false;
    return m_state >= 2 && m_state <= 4 && !flag;
}

// ORIG T 0x82eab1a：_IsBroadcastRunaway —— PowerWar 且村庄==7 直接 false；
//   否则 getCurCharacR()!=0 决定返回值（0x82eab50 `mov $0` 在 t 为 true 分支）。
bool CAssaultPlace::_IsBroadcastRunaway(CUser* user)
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
void CAssaultPlace::_BroadcastRunaway(CUser* user, int index)
{
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
    unsigned char len = static_cast<unsigned char>(strlen(buf));
    memset(msg.m_text, 0, 0xff);
    strncpy(msg.m_text, buf, len);
    unsigned short len2 =
        static_cast<unsigned short>(len + 0xf);
    int packetSize = len2;
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&msg), packetSize);
}

// ORIG T 0x82e9046：_MakeLeaveUser —— 包 0x87 + 玩家 unique_id。
void CAssaultPlace::_MakeLeaveUser(PacketGuard* p, int index)
{
    p->put_header(0, 0x87);
    p->put_short((int)m_assaulters[index].GetUserUniqueId());
    p->finalize(true);
}

// ORIG T 0x82e88d6：_MakeCloseAssault —— 包 0x89 + tick。
void CAssaultPlace::_MakeCloseAssault(PacketGuard* p, int tick)
{
    p->put_header(0, 0x89);
    p->put_byte(tick);
    p->finalize(true);
}

// ORIG T 0x82e8d60：_MakeUserState(PacketGuard*, CUser*) —— 包 0x03 + state。
void CAssaultPlace::_MakeUserState(PacketGuard* p, CUser* user)
{
    // b28d-solo-CAssaultMgr: N 封顶（N18a）— ORIG 0x82e8d60+0x30 `call
    // GetUserState; mov %eax,%edx; mov p,%eax; mov %edx,0x4(%esp)`（34 条）；
    // 4.4 恒折 `mov p,%edx; mov %eax,0x4(%esp)`（33 条，少 1 条 hop）。P248 三档
    // 旋钮全试：探针 /tmp/probe_pb{,2,3,4}.cpp 十一形（int/uint/long/char/uchar/
    // enum/字段 cast/callee 返回 unsigned/具名局部）4.4 均产 this-first 无中转；
    // ORIG 同 TU SendUserState 同表达式亦无 hop（ORIG 自身分裂两形）。
    p->put_header(0, 3);
    p->put_byte(1);
    p->put_short(user->get_unique_id() & 0xffff);
    // b28 BM4 判词（N18a）：ORIG 0x82e8d60+0x30 为 call GetUserState 后
    // `mov %eax,%edx` 冗余中转（int→int 同型，4.4 恒直落 `mov %eax,0x4(%esp)`
    // 折叠；/tmp/pbq.cpp i/u/cast 三形探针实测 4.4 均无 hop，源形不可造，
    // 绑定形 P0 禁用）。本函数与单参重载同段各差 1 条 hop → difflib 连锁
    // 计 7/13，实残仅此。
    p->put_byte(user->GetUserState());
    p->finalize(true);
}

// ORIG T 0x82e83c4：SendUserState(CUser*) —— 组 0x03 状态包并全服广播
// （报告 pvp_assault__CAssaultPlace/SendUserState.md；ORIG 调用点仅压 CUser*，
//   头文件声明为静态形态）。
void CAssaultPlace::SendUserState(CUser* user)
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
int CAssaultPlace::_SendCloseAssault(CUser* user, int tick)
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
// ============================================================================
void CAssaultPlace::LeaveAssaultPlace(int index, bool delFlag, bool penaltyFlag)
{
    // b28d-L3: DIFF/N82 — ORIG 0x082ead6a 450/451 条；超大函数 N82 槽复用
    // （帧/槽号全帧错位），p167 MISSING=0（无语义缺口）。
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
        }

        // ---- _IsBroadcastRunaway（82eae89）----
        if (_IsBroadcastRunaway(user))
            _BroadcastRunaway(user, index);

        // ---- 82eaeb8：leave 包 + PowerWar 计数 ----
        {
            PacketGuard packet;
            _MakeLeaveUser(&packet, index);
            SendPacket(packet);
        }
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
                PacketGuard packet;
                _MakeUserState(&packet, user);
                G_GameWorld()->send_all(packet);   // ORIG 82eb052 同形
            }
        }
    }

    // ---- 82eb07a：user!=0 的收尾链 ----
    if (user != 0)
    {
        // 非 PowerWar（或非村庄7）且状态<=3 且索引 0/1：双方队伍置 7 后关闭。
        bool nonPowerWar = !GlobalData::s_power_manager->IsPowerWarEventOn() ||
                           user->getCurCharacVill() != 7;
        if (nonPowerWar && m_state <= 3 && (index == 0 || index == 1))
        {
            if (m_parties[0].m_field00 != 0)
                ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
            if (m_parties[1].m_field00 != 0)
                ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
            m_state = 7;
            CloseAssault();
            return;
        }
        // 82eb13a：m_field100==0 → 同样置 7 后关闭。
        if (m_field100 == 0)
        {
            if (m_parties[0].m_field00 != 0)
                ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
            if (m_parties[1].m_field00 != 0)
                ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
            m_state = 7;
            CloseAssault();
            return;
        }
        // 82eb1b0：m_field100!=0 且场景可玩时，PowerWar 村庄7 且状态∈(3,4]
        // 走 guild 点数/连击清理；随后统一置 6 结束。
        if (!_CheckPlayable())
            return;
        if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
            user->getCurCharacVill() == 7 && m_state > 3 && m_state <= 4)
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
                    user, (ENUM_POWER_WAR_WINNER_OR_LOSER)2, winCnt, loseCnt,
                    aliveCnt);
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
        // 82eb349：双方队伍置 6 后结束。
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
        m_state = 6;
        EndAssault();
    }
}

// ============================================================
// 批 A（OnlyOrig_Assault 恢复落盘；跳过 GetDistance(Point2d,Point2d) 与
// 6 参 IsIncludeVirtualMoveAreas——唯一 owner 归 CVillageMonsterMgr.cpp）
// ============================================================

static const Point2d ASSAULT_RECT_PLAYER_POINT[8] = {
    Point2d(0x000, 0x0f), Point2d(0x038, 0x05), Point2d(0x054, 0x00),
    Point2d(0x01c, 0x0a), Point2d(0x104, 0x0f), Point2d(0x0cc, 0x05),
    Point2d(0x0b0, 0x00), Point2d(0x0e8, 0x0a)
};                                   // .rodata 0x8c1cce0 实测内容
static const Point2d ASSAULT_RECT_ICON_POINT = { 0x82, 5 };   // .rodata 0x8c1cd20

int Point2d::operator-(const Point2d& o) const
{
    // ORIG 0x82f0486：|dx| + |dy|，abs 为无分支 `sar $0x1f; xor; sub` 三件套
    // ⇒ __builtin_abs 复现（实测 4.4.7 -O0 同族展开；if(x<0)x=-x 在 4.4 恒产
    // cmpl/jns/negl 分支形，多 3 条）。ORIG 全寄存器接力零槽 vs 4.4 store/load
    // 往返属 N82 帧槽族，收敛到指令序同构即止。
    return __builtin_abs(m_x - o.m_x) + __builtin_abs(m_y - o.m_y);
}

void PartyInfo::Set(CParty* party)                 // ORIG 0x82f05fc
{
    m_field00 = party;
}

// ORIG W 0x82f060a（size 0xa）：mov 0x0(%eax),%eax; ret —— 外联定义（P119）
unsigned int PartyInfo::Get()
{
    return (unsigned int)m_field00;
}

CandidatePoint::CandidatePoint() : m_point() {}    // +0 故意不初始化（ORIG 同）
CExtraMatrix::CExtraMatrix() {}

void CExtraMatrix::AddPoint(Point2d p)             // ORIG 0x82e5962
{
    m_points.push_back(p);
}

void CExtraMatrix::MovePoint(const Point2d& delta) // ORIG 0x82e597c
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

bool CExtraMatrix::CheckPoint(std::vector<MapArea>* areas,
                              Point2d& out)        // ORIG 0x82e5b4e
{
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e5b4e
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        if (IsIncludeVirtualMoveAreas(areas, it->m_x, it->m_y,
                                      out.m_x, out.m_y, 0))
        {
            return true;
        }
    }
    return false;
}

bool CExtraMatrix::MoveUser(CForcedMoveCharacter* fm)   // ORIG 0x82e5c0a
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

CCandidateMatrix::CCandidateMatrix() {}
CCandidateMatrix::~CCandidateMatrix() {}

unsigned int CCandidateMatrix::At(unsigned int idx, int& x, int& y)  // ORIG 0x82e590c（恒 false）
{
    if (m_points.size() > idx)
    {
        x = m_points[idx].m_point.m_x;
        y = m_points[idx].m_point.m_y;
    }
    return 0;
}

void CCandidateMatrix::_AddCandidatePoint(const Point2d& from1,
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

void CCandidateMatrix::MakeCandidatePoint(const Point2d& a, const Point2d& b) // ORIG 0x82e5ca8
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

void CCandidateMatrix::RemoveCandidatePoint(std::vector<MapArea>* areas) // ORIG 0x82e5f36
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

void CCandidateMatrix::ArrangeCandidatePoint()          // ORIG 0x82e5fee
{
    std::sort(m_points.begin(), m_points.end());
}

// ============================================================
// 全局依赖（本 TU 唯一 owner 部分）
// ============================================================

} // namespace pvp_assault

// ORIG T _Z24GetMaxCandidateMatirxRowv 0x82e5c9e（全局函数，非 pvp_assault 成员）。
int GetMaxCandidateMatirxRow()
{
    return 15;
}

// ORIG T _ZN7MapArea7includeEii 0x82f0298：矩形包含判定。
bool MapArea::include(int x, int y)
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
bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y)
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
void CAssaulter::AttachUser(CUser* user, ENUM_ASSAULT_TEAM team)   // ORIG T 0x82e67e6
{
    m_user = user;
    m_team = team;
    _PrepareAssault(user);
}

bool CAssaulter::IsAttachAble()        // ORIG W 0x82f054c
{
    return m_user == 0;
}

void CAssaulter::_PrepareAssault(CUser* user)     // ORIG T 0x82e6780
{
    if (user->getCurCharacR() != 0)
    {
        m_hp = (unsigned char)user->getCurCharacStamina();
        user->SetCurCharacLeaveTimeout(false);
        user->getCurCharacInvenW()->GetCreatureMgrW()->TurnStomach(true);
    }
}

bool CAssaulter::IsPowerWarWinner()     // ORIG W 0x82f05ae：movzbl 0x14(%eax)（直读 bool）
{
    return m_powerWarWinner;
}

void CAssaulter::SetPowerWarWinner(bool flag)   // ORIG W 0x82f0596
{
    m_powerWarWinner = (char)(flag ? 1 : 0);
}

void CAssaulter::SetUserConnectP2P(char* ip, int len)   // ORIG T 0x82e6a82
{
    // ORIG 0x82e6a96 call _ZSt3minIiERKT_S2_S2_（(%esp)=&len，0x4esp=&.rodata 8
    // @0x8c1c344 ⇒ static const int 取地址），`mov (%eax),%eax` 解引用返回引用。
    static const int kIpMax = 8;
    memcpy(m_connectIp, ip, std::min(len, kIpMax));
}

void CAssaulter::Reset()               // ORIG W 0x82f05ba
{
    m_user = 0;
    m_team = (ENUM_ASSAULT_TEAM)0;
    m_state = (ENUM_ASSAULT_STATE)0;
    m_lastHpSec = 0;
    m_powerWarWinner = 0;
}

void CAssaulter::DieUser()             // ORIG T 0x82e6a40
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

bool CAssaulter::ChangeHp(int hp)      // ORIG T 0x82e6694
{
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

const char* CAssaulter::GetCharacName() const    // ORIG T 0x82e663c
{
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

void CStaticDisposition::_MoveMatrix(const Point2d& delta)   // ORIG 0x82e6498
{
    for (int j = 0; j <= 1; ++j)
        m_mats[j].MovePoint(delta);
    m_iconPos += delta;
}

void CStaticDisposition::GetFlagPoint(int& x, int& y)        // ORIG 0x82f0526
{
    x = m_iconPos.m_x;
    y = m_iconPos.m_y;
}

bool CStaticDisposition::_CheckMatrix(std::vector<MapArea>* areas,
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

bool CStaticDisposition::SetVirtualMap(std::vector<MapArea>* areas) // ORIG 0x82e6588
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

bool CStaticDisposition::MoveUser(CForcedMoveCharacter* fm)  // ORIG 0x82e65ee
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
CStaticDisposition::CStaticDisposition(CUser** users, int* counts) // ORIG 0x82e62aa
{
    // b28d-L3: DIFF/N86 — ORIG 0x82e62aa 二维索引 `mov -0x20,%edx; mov -0x1c,%eax; shl $2,%edx; lea (%edx,%eax,1),%eax; add %eax,%eax`
    m_iconPos.m_x = ASSAULT_RECT_ICON_POINT.m_x;
    m_iconPos.m_y = ASSAULT_RECT_ICON_POINT.m_y;
    for (int j = 0; j <= 1; ++j)
    {
        // [b23S3C] counts[j]（非 j*4；int* 天然乘 4 字节）；P48 每循环迭代重读
        // counts[j]（ORIG 内层条件先判 counts[j]>k 再 k<=3）；ORIG 具名临时 Point2d
        // p 在 -0x30，独立于末端 origin(-0x28)，故两处各自命名。
        for (int k = 0; counts[j] > k && k <= 3; ++k)
        {
            Point2d p(ASSAULT_RECT_PLAYER_POINT[j * 4 + k].m_x,
                      ASSAULT_RECT_PLAYER_POINT[j * 4 + k].m_y);
            m_mats[j].AddPoint(p);
        }
    }
    Point2d origin(users[0]->get_posX(), users[0]->get_posY());
    _MoveMatrix(origin);
}

CStaticDisposition::~CStaticDisposition() {}                 // ORIG 0x82f10b0

void CDynamicDisposition::GetFlagPoint(int& x, int& y)       // ORIG 0x82f050a
{
    x = m_anchor.m_x;
    y = m_anchor.m_y;
}

bool CDynamicDisposition::SetVirtualMap(std::vector<MapArea>* areas) // ORIG 0x82e624c
{
    m_matrix0.RemoveCandidatePoint(areas);
    m_matrix1.RemoveCandidatePoint(areas);
    m_matrix0.ArrangeCandidatePoint();
    m_matrix1.ArrangeCandidatePoint();
    return true;           // ORIG mov $0x1,%eax
}

CDynamicDisposition::CDynamicDisposition(CUser** users, int* counts) // ORIG 0x82e6032
{
    // b28d-solo-CAssaultMgr: ORIG 0x82e6055 段用循环拷贝 m_users
    //（`movl $0,-0xc; jmp cond; ...; addl $1,-0xc; cmpl $1; setle; test; jne`），
    // 展开赋值恒少 5 条。
    for (int i = 0; i <= 1; ++i)
        m_users[i] = users[i];
    m_anchor.m_x = (unsigned short)m_users[0]->get_posX()
                 + ((unsigned short)m_users[1]->get_posX() >> 1);
    m_anchor.m_y = (unsigned short)m_users[0]->get_posY()
                 + ((unsigned short)m_users[1]->get_posY() >> 1);
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

CAssaultPlaceFactory::CAssaultPlaceFactory() {}   // ORIG C1 0x82f0f96
CAssaultPlaceFactory::~CAssaultPlaceFactory() {}  // ORIG D1 0x82f0f82

CAssaultPlace* CAssaultPlaceFactory::AcquireAssaultPlace()   // ORIG 0x82f0f54
{
    return m_pool.Acquire();
}

void CAssaultPlaceFactory::FreeAssaultPlace(CAssaultPlace* place)  // ORIG 0x82f0f68
{
    m_pool.Free(place);
}

CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory()       // ORIG 0x82f0fa9
{
    static CAssaultPlaceFactory instance;                    // guard 0x943bcc0/0x943bce0
    return &instance;
}

int CAssaultPlace::GetAuthenValue()             // ORIG T 0x82f06a0
{
    return m_authenValue;
}

int CAssaultPlace::GetAssaultPlaceId()          // ORIG T 0x82f0680
{
    return m_field104;
}

void CAssaultPlace::SetAssaultPlaceId(unsigned int id)   // ORIG T 0x82f068e
{
    m_field104 = id;
}

int CAssaultPlace::GetAssaulterCount()          // ORIG W 0x82f06dc
{
    return m_field100;
}

const char* CAssaultPlace::GetAnotherTeamCharacName(int idx)   // ORIG T 0x82eb3ba
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

void CAssaultPlace::AbuseUser(CUser* user)      // ORIG T 0x82eb5e2（size 0x192）
{
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
        m_state = 6;
    }
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
    TimeoutCloseAssault();
}

// ============================================================================
// CAssaultPlace 构造 / 析构（ORIG W C1/C2 0x82f0614 size 0x6c、D1/D2 0x82f13b6
// size 0x31）。ORIG ctor：CAssaulter[8] 循环默认构造 → Point2d::C1 于 this+0x110
// （m_posBase）→ PartyInfo[2] 循环构造；其余字段留待 Reset()/Acquire 初始化。
// ORIG dtor：仅反向析构 assaulters[8]（Point2d/PartyInfo 析构平凡）。
// ============================================================================
CAssaultPlace::CAssaultPlace()
{
}

CAssaultPlace::~CAssaultPlace()
{
    // b28d-solo-CAssaultMgr: AE（原 NEAR 2，旧 N-帧判词作废）——P210 三分律②：
    // 把 CAssaulter::~CAssaulter 由头内 inline 空体改为本 TU 外联定义并置于本
    // caller 之前（CAssaultMgr.cpp:189），4.4 对 assaulters[8] 析构循环由保守
    // sub $0x14 翻为精确继承 ORIG 0x82f13b6 的 sub $0x4，逐条 AE。
}

// ORIG T 0x82e6d3a：m_pos.x/y 非零才向所在区域广播。
void CAssaultPlace::_SendPacketToArea(PacketGuard& packet)
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet);
}

// ORIG T 0x82e6dce：同门槛；带用户列表的 4 参 send_to_area 版。
//   求值序（报告汇编 +0x25/+0x2e）：m_y 先入 esi、m_x 后入 ebx（右→左），
//   与 send_to_area(int vill, int area, ...) 形参序一致。
void CAssaultPlace::_SendPacketToArea(PacketGuard& packet,
                                      std::vector<CUser*>& users)
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet, users);
}

// ORIG T 0x82e8afc：包 0x86 图标列表 = put_header(0,0x86) + put_short(1) +
// MakeViewAssaultIconOnePlayer + finalize(true)。
void CAssaultPlace::_MakeViewAssaultIconPlayer(PacketGuard* packet, bool flag)
{
    packet->put_header(0, 0x86);
    packet->put_short(1);
    MakeViewAssaultIconOnePlayer(packet, flag);
    packet->finalize(true);
}

// ORIG T 0x82e79f6：_MakeUserState 后 G_GameWorld 全服广播。
void CAssaultPlace::_SendUserState()
{
    PacketGuard packet;
    _MakeUserState(&packet);
    G_GameWorld()->send_all(packet);
}

// ORIG T 0x82e8c5a：包 0x03 全场用户状态；先记录写位置，尾部回填人数。
void CAssaultPlace::_MakeUserState(PacketGuard* packet)
{
    // b28d-solo-CAssaultMgr: N 封顶（N18a）— 同双参重载：ORIG 0x82e8c5a+0x0a9
    // `call GetUserState; mov %eax,%edx; mov packet,%eax; ...`（73 条）vs 4.4 折
    // this-first（72 条，少 1 条 hop）。探针矩阵同双参件（11 形全败）。
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
void CAssaultPlace::_DelUser(int index, bool flag)
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
void CAssaultPlace::TimeoutCloseAssault()
{
    // b28d-solo-CAssaultMgr: ORIG 0x082ea188 先把 8 个 GetCharacName 实参求值
    // （-0x40..-0x2c 槽 + edi/esi），**之后**才 ctor cMyTrace（+0x30 段）⇒ 原码是
    // 全表达式临时链（P227/P37），具名对象恒 ctor 先于实参求值（DIFF 主因）。
    cMyTrace(__PRETTY_FUNCTION__,
             0x9ea, 0)("TimeoutCloseAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
          m_state,
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
        m_state = 7;
        CloseAssault();
    }
}

// ORIG T 0x82e6ab8（报告 Reset.md）：全场复位；归还池前调用。
void CAssaultPlace::Reset()
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
    m_state = 0;
    // ORIG 0x82e6b5d：Point2d(0,0) 临时构造（C1Eii sret -0x14/-0x10）后
    // 两字拷入 0x110/0x114 ⇒ 临时对象赋值形。
    m_posBase = Point2d(0, 0);
    m_pos.m_x = 0;
    m_pos.m_y = 0;
    // ORIG 0x82e6ba7：teamFlag 第二循环复用 i 槽。
    for (i = 0; i <= 1; ++i)
        m_teamFlag[i] = 0;
    m_winTeam = 0;
    m_checkUser = 0;   // ORIG *(int*)(this+0x134)=0：该槽用户指针清零
    m_abuseSec = 0;
}

// ORIG T 0x82edb6a：map 摘除 + place->Reset() + 归还 StaticPool。
void CAssaultMgr::FreeAssaultPlace(int id, CAssaultPlace* place)
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
// 收敛口径（batch-14 EmoC11）：o=374 n=387。已修 PartyInfo::Get 发明 wrapper
// 改 m_field00 直访（ORIG nm 无该符号、两处 `mov 0x12c(%eax)` 直读，P154/P169）。
// 残 +13 = 循环体 slotUser 槽装载 vs ORIG callee-saved 接力 + ONLY-ORIG
// 1×GetUser 重取点（P183 反向警告域，逐点展开反增差），N82/N76 封顶。
void CAssaultPlace::CloseAssault()
{
    // b28d-L3: DIFF/N82 — ORIG 0x082e... 372 vs 383
    int lastVill = 0;
    if (m_field100 <= 0)
        return;

    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        CUser* slotUser = m_assaulters[i].GetUser();
        if (slotUser == 0)
            continue;

        bool powerWarVill =
            GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() == 7;
        if (powerWarVill)
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
                if (slotUser->IsCurCharacGhost() == 0)
                {
                    unsigned char stamina =
                        (unsigned char)_SendCloseAssault(
                            slotUser, m_assaulters[i].GetHp());
                    slotUser->setCurCharacStamina(stamina);
                }
                else
                {
                    unsigned char stamina =
                        (unsigned char)_SendCloseAssault(slotUser, 100);
                    slotUser->setCurCharacStamina(stamina);
                }
            }
        }
        lastVill = (int)slotUser->getCurCharacVill();

        // b28d-solo-CAssaultMgr: ORIG 0x082e9c63 在 SetAssaultPlace 前**重取**
        // `m_assaulters[i].GetUser()`（P167 MISSING ×1；ORIG 调用点直读展开，
        // 非缓存 slotUser）。
        m_assaulters[i].GetUser()->SetAssaultPlace(0);
        if (m_assaulters[i].IsPowerWarWinner())
        {
            if (slotUser->getPowerWarHP() < 0x1e)
                slotUser->setPowerWarHP(0x1e);
        }

        bool gainChaosKill =
            m_assaulters[i].GetTeam() == m_winTeam
            && !GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() != 7;
        if (gainChaosKill)
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            slotUser->SetCurCharacChaosKillTime(now);
        }

        int ghostTime = 0;
        bool powerWar = GlobalData::s_power_manager->IsPowerWarEventOn()
                        || slotUser->getCurCharacVill() == 7;
        if (powerWar)
        {
            float penalty =
                GlobalData::s_power_manager->GetPowerWarResponPenalty(slotUser);
            ghostTime =
                GlobalData::s_power_manager->GetPowerWarGhostTime(slotUser,
                                                                 penalty);
        }
        else
        {
            int chaos = slotUser->GetCurCharacChaosPoint();
            int level = slotUser->get_charac_level();
            ServerParameterScript* sps =
                &G_CDataManager()->m_serverParameter;   // §4.3 具名化（+0x68）
            ghostTime = sps->getAssaultGhostTime(level, chaos);
        }

        if (slotUser->IsCurCharacGhost() != 0 && ghostTime != 0)
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            slotUser->SetCurCharacChaosDieTime(now);
            slotUser->SetCurCharacChaosResponTime(now + ghostTime);
            slotUser->m_characterView.enableSaveCharacView();
            CAssaultMgr::NotifyGhostTime(slotUser, ghostTime, ghostTime);
        }
    }
    _SendUserState();

    PacketGuard packet;
    _MakeViewAssaultIconPlayer(&packet, false);
    _SendPacketToArea(packet);

    bool reloadParty =
        GlobalData::s_power_manager->IsPowerWarEventOn() && lastVill == 7;
    if (reloadParty)
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

    for (int i = 0; i < 8; ++i)
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
void TimerCompleteLoadAfterAssaultTimeout::registNextTimer(long sec,
                                                           int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x2a, (int)sec, (unsigned int)p2,
                                0);
}
