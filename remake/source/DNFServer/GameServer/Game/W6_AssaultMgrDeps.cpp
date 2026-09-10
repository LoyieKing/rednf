// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_AssaultMgrDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultMgr Wave6 补批实现（WAMgr：only_orig 23 符号清零）。
// 转写来源：docs/class_func_reports/pvp_assault__CAssaultMgr/ 各报告（## 汇编 为准，
// 每个定义前标注 ORIG T/W 地址与尺寸；ORIG 内联浮点常量 _DAT_08c1d18c = -100.0f）。
//
// 本 TU 同时承载 OnRequestAssault / check_party_teleport_user 链上的独立依赖定义：
//   IsStayGate(CUser*)                                ORIG T 0x82edc05 (0x81)
//   CPowerWarLog 生命周期/统计小方法族                  ctor T 0x848021e / Reset T 0x8480232 /
//                                                     Inc* W 见 CPowerManager.h 字段注释
//   CPowerManager::GetPowerWarLog()                   ORIG W 0x82f0906
#include "CAssaultMgr.h"

#include <map>
#include <cmath>

#include <utility>

#include "MSG_DIE_ASSAULT_PLAYER.h"
#include "GameTypes.h"            // ENUM_ERROR
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "CInventory.h"
#include "CEquipItem.h"
#include "Inven_Item.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_power_manager/s_event_manager/s_villageMonsterMgr/s_systemTime_
#include "CPartyTelePort.h"      // CParty::GetPartyTelePort 返回类型
#include "CSystemTime.h"          // CSystemTime::getCurSec
#include "CDataManager.h"         // G_CDataManager（m_serverParameter@+0x68）
#include "CPowerManager.h"
#include "CGameManager.h"         // G_CGameManager()->GetParty(int)
#include "CEnvironment.h"        // G_CEnvironment()->m_serverEnvir.m_serverGroup(+0x378)
#include "GameWorld.h"            // G_GameWorld()/getUserVillage()/IsPowerWarSafeZone()
#include "Village.h"              // Village::get_gate_area
#include "CVillageMonsterMgr.h"   // village_attacked::OnEvent
#include "CPrivateStoreMgr.h"     // private_store 单例链
#include "CExpertJob.h"           // expert_job::CAlchemist::GetAssaultPrevent
#include "TimerDispatcher.h"      // TimerCompleteLoadAssaultTimeout::registNextTimer
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent / CEventBase::IsEventing
#include "LogManager.h"

#include "W6_AssaultMgrDeps/IsStayGate.inc"

namespace pvp_assault
{

namespace
{

// ---------------------------------------------------------------------------
// ORIG 0x0822f6d2：内联 getAssaultPlace(CUser) 的公共前奏 —— 读用户挂接场次 id。
// （函数体内联消失，仅保留语义形状。）
// ---------------------------------------------------------------------------
inline int userIdPlaceId(CUser* user)
{
    return user->GetAssaultPlace();
}

// expert_job::CAlchemist::GetAssaultPrevent 为 static（无 this 槽，
// CExpertJob.h:272 权威声明，ORIG T 0x085d137c）。§4.5 正解：直接静态调用
// `CAlchemist::GetAssaultPrevent(user, flag)`，无需成员函数指针 ABI 转接。
// （DisPatcherCore.cpp 已同型直呼。）

// PowerWarOn：ORIG 各调用点均为 3 条内联形（s_power_manager 装载+call
// IsPowerWarEventOn），无独立符号；P49 —— -O0 下 inline 函数仍发真 call，
// 必须宏展开（[b23S3C]）。
#define PowerWarOn() (GlobalData::s_power_manager->IsPowerWarEventOn())

// CDataManager 内尚未具名化的三个 int 标量（+0x600/+0x604/+0x608，
// importBloodDungeon 区；字段化归 CDataManager 域批次，本批按具名视图读取）。
// 视图成员访问让 4.4 折叠单条 disp（ORIG `mov 0x604(%eax),%eax`）；inline helper
// 在 -O0 发真 call（P49）、裸 char* 偏移多发 add，均不可用。
struct CDataManagerTailView
{
    unsigned char m_reserved[0x600];
    int m_field600;
    int m_field604;
    float m_field608;                                   // ORIG `flds 0x608(%eax)`
};
#define DM_FIELD(n) \
    (reinterpret_cast<const CDataManagerTailView*>(G_CDataManager())->m_field##n)

}  // unnamed namespace

// ===========================================================================
// Reset —— ORIG T 0x082ed966 (0x0f)：清 id 计数器。
// ===========================================================================
#include "W6_AssaultMgrDeps/Reset.inc"
{
    m_nextAssaultPlaceId = 0;
}

// ===========================================================================
// _GetNextAssaultPlaceId —— ORIG T 0x082ed976 (0x87)
// 自增回卷时借静态 count 以 cMyTrace 报警并再 +1。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（P227 全表达式临时链：ORIG ctor 前 `mov count,%ebx` 跨 ctor
// 保活 0x082ed9a3 逐条复现，39=39；旧 N7/N82 判词作废）；ORIG 0x082ed976。
#include "W6_AssaultMgrDeps/_GetNextAssaultPlaceId.inc"
{
    static int count = 0;                       // ORIG _ZZN…_GetNextAssaultPlaceIdE5count

    ++m_nextAssaultPlaceId;
    if (m_nextAssaultPlaceId == 0)
    {
        ++count;
        // [b28d-solo] P227 全表达式临时链：ORIG 在 ctor 前 `mov count,%ebx`
        // 跨 ctor call 保活（0x082ed9a3），具名 trace 形 4.4 恒重读 count。
        cMyTrace(__PRETTY_FUNCTION__, 0x1085, 0)(
            "NEXT_ASSAULT_PLACE_ID overflow count(%d)", count);
        ++m_nextAssaultPlaceId;
    }
    return m_nextAssaultPlaceId;
}

// ===========================================================================
// _GetAssaultPlace(unsigned int, int) —— ORIG T 0x082eda94 (0xd6)
// map 未命中抛 ENUM_ERROR(0x15)；authenValue 不匹配抛 ENUM_ERROR(0x8)。
// ===========================================================================
#include "W6_AssaultMgrDeps/_GetAssaultPlace.inc"
{
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
    if (it == m_places.end())
    {
        throw static_cast<ENUM_ERROR>(0x15);
    }
    CAssaultPlace* place = it->second;
    if (place->GetAuthenValue() != authenValue)
    {
        throw static_cast<ENUM_ERROR>(0x8);
    }
    return place;
}

// ===========================================================================
// FrocedCloseAssaults —— ORIG T 0x082ed5f4 (0x7d)
// 遍历全部场次逐一 EndAssault（非空指针才调）。
// ===========================================================================
#include "W6_AssaultMgrDeps/FrocedCloseAssaults.inc"
{
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.begin();
    while (it != m_places.end())
    {
        CAssaultPlace* place = it->second;
        if (place != 0)
        {
            place->EndAssault();
        }
        ++it;
    }
}

// ===========================================================================
// OnChangeHp —— ORIG T 0x082ef570 (0x9f)
// 取用户所在场次转派 ChangeHp；异常路径 SendCmdErrorPacket(0x98,err&0xff)。恒返 1。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnChangeHp.inc"
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->ChangeHp(user, hp);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x98),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}

// ===========================================================================
// OnAbuseUser —— ORIG T 0x082efc54 (0x5d)：catch 空吞，恒返 1。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnAbuseUser.inc"
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->AbuseUser(user);
    }
    catch (ENUM_ERROR)
    {
        // ORIG catch 仅 __cxa_begin/end_catch 吞异常。
    }
    return 1;
}

// ===========================================================================
// OnConnectP2P —— ORIG T 0x82ef27a (0xa5)：错误码包 0x95。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnConnectP2P.inc"
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->ConnectP2P(user, ip, len);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x95),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}

// ===========================================================================
// OnRevivalUser —— ORIG T 0x082ef464 (0x10b)
// local result=0；static RevivalUser(user,result)；若中途异常：
// PacketGuard(header(0,0x97)/byte(1)/byte(0)/byte(err)/finalize(true)) 发给该用户。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnRevivalUser.inc"
{
    int result = 0;
    try
    {
        pvp_assault::CAssaultPlace::RevivalUser(user, result);   // ORIG：static 两参符号 _ZN…RevivalUserEP5CUserRi
    }
    catch (ENUM_ERROR e)
    {
        // ORIG 0x082ef49f：header(1,0x97)/byte(0)/byte(e)/int(result=0)
        PacketGuard packet;
        packet.put_header(1, 0x97);
        packet.put_byte(0);
        packet.put_byte(static_cast<int>(e));
        packet.put_int(result);
        packet.finalize(true);
        user->Send(packet);
    }
    return 1;
}

// ===========================================================================
// OnCompleteLoad —— ORIG T 0x082ef1e2 (0x97)：错误码包 0x94。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnCompleteLoad.inc"
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->CompleteLoad(user);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x94),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}

// ===========================================================================
// OnDieUser —— ORIG T 0x082ef3be (0xa5)：错误码包 0x96。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnDieUser.inc"
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->DieUser(user, index, msg);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x96),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}

// ===========================================================================
// AvailableAssault —— ORIG T 0x082f0022 (0x219)：全程为可攻击性校验，失败即抛。
// ===========================================================================
#include "W6_AssaultMgrDeps/AvailableAssault.inc"
{
    // [b23S3C] P0/P183 槽映射：ORIG 仅 3 槽 -0x14(errorOut，lea 直发三处 out 参、
    // get_unique_id 复用)/-0x10(party)/-0xc(ageLimit=0xf)；删 minAgeOut/
    // partyAgeOut/partyLevelOut/ageOk/partyAgeOk/partyLevelOk/vill 缓存槽
    // （P48 调用点级直传+直判），bool 汇聚改嵌套 if。
    int errorOut = 0;                                   // -0x14
    CParty* party = 0;                                  // -0x10
    int ageLimitFlag = 0xf;                             // -0xc

    // [b23S3C] 短路 && 直跳形（嵌套 if 反产 setcc 物化）。
    if (PowerWarOn() && user->getCurCharacVill() == 7)
    {
        ageLimitFlag = 0xc;
    }

    if (user->CheckInParty() != 0)
    {
        party = static_cast<CParty*>(user->GetParty());
    }

    if (user->CheckLimitMiniumAge(ageLimitFlag, errorOut) == 0)
    {
        throw static_cast<ENUM_ERROR>(0x5f);
    }

    // 等级门槛：level>29 才放行，否则同源/异源两种错误码。
    if (user->get_charac_level() <= 0x1d)
    {
        errorOut = user->get_unique_id();
        if (user == source)
        {
            throw static_cast<ENUM_ERROR>(0xd);
        }
        throw static_cast<ENUM_ERROR>(0xe);
    }

    if (party != 0)
    {
        // [b28c-L7] P168a①：is_limit_minimum_age/level 已按 Main 授权 bool 化
        // （CParty.h:680 判词）⇒ 裸形直产 ORIG `call; xor $1,%eax; test %al,%al;
        // je`（探针 /tmp/xprobe.cpp b/j 形 = bool callee 唯一可翻形）。
        if (!party->is_limit_minimum_age(ageLimitFlag, errorOut))
        {
            throw static_cast<ENUM_ERROR>(0x5f);
        }
        if (!party->is_limit_minimum_level(0x1e, errorOut))
        {
            if (user == source)
            {
                throw static_cast<ENUM_ERROR>(0xfe);
            }
            throw static_cast<ENUM_ERROR>(0xff);
        }
    }
    // ORIG 函数自然收尾（leave/ret），无返回值语义。
}

// ===========================================================================
// OnEvadeAssaultPlace —— ORIG T 0x082efbf6 (0x5d)：catch 空吞，恒返 1。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnEvadeAssaultPlace.inc"
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->EvadeAssaultPlace(user);
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;
}

// ===========================================================================
// 四个超时入口——模式均为取场次转派 + catch 吞 + 恒返 1。
// ORIG 地址随注释；place 具名局部（P0 槽往返：mov -0x10）后转派。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnTimeoutConnectP2P.inc"
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutConnectP2P();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efa52
}

#include "W6_AssaultMgrDeps/OnTimeoutEndAssault.inc"
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutEndAssault();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efaa8
}

#include "W6_AssaultMgrDeps/OnTimeoutCloseAssault.inc"
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutCloseAssault();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efafe
}

#include "W6_AssaultMgrDeps/OnTimeoutCompleteLoad.inc"
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutCompleteLoad();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082ef818
}

// ===========================================================================
// OnConnectP2PAfterAssault —— ORIG T 0x082ef320 (0x9d)
// 会战结束后阶段：P2P 直连改经队伍转发；失败错误码包 0xf9。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnConnectP2PAfterAssault.inc"
{
    try
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->ConnectP2P(user, ip, len);
        }
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf9),
                                 static_cast<unsigned char>(static_cast<int>(e)));
    }
    return 1;
}

// ===========================================================================
// OnTimeoutConnectP2PAfterAssault —— ORIG T 0x082ef9d8 (0x79)
// [b28d-solo-W6_AssaultMgrDeps] AE（P214① 正极性短路 &&：4.4 直产 ORIG `je;je;mov $1,%eax;jmp;
// mov $0,%eax;test;je` 物化岛，40=40；旧「bool ABI 跨版本」判词作废）；ORIG 0x082ef9d8。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnTimeoutConnectP2PAfterAssault.inc"
{
    (void)timerKey;                                     // ORIG 第 2 参未消费
    try
    {
        CParty* party = G_CGameManager()->GetParty(static_cast<int>(partyIndex));
        // [b28d-solo] P214① 正极性短路：ORIG 短路 && 后 bool 物化
        // `mov $1,%eax/jmp; mov $0,%eax; test %al,%al; je`（0x082efa0b..0x082efa19）。
        if (party != 0 && party->TimeoutCompleteAfterAssault())
        {
            party->ClearBadP2PUser();
        }
    }
    catch (int)                             // ORIG landing pad `cmp $0x1,%edx; je`+
                                            // _Unwind_Resume 分支 = 定类型 catch(int)，
                                            // 非 catch(...)（其无 selector 匹配）。
    {
        // ORIG 仅 begin_catch → 存值到槽 → end_catch，不消费。
    }
    return 1;   // ORIG 所有路径 eax=1
}

// ===========================================================================
// OnTimeoutCompleteLoadAfterAssault —— ORIG T 0x082ef86e (0x169)
// TimerCompleteLoadAfterAssaultTimeout 回调主体（LfAssaultPlace 只交付了 registNextTimer）。
// [b28d-solo-W6_AssaultMgrDeps] AE（95=95）：catch(...) 改 catch(int) 补 `cmp $0x1,%edx` landing；
// 返回类型 void→int（P159：ORIG 尾 `mov $0x1,%eax` 0x082ef9cb）；旧 N93 判词作废。
// ===========================================================================
#include "W6_AssaultMgrDeps/OnTimeoutCompleteLoadAfterAssault.inc"
{
    try
    {
        CParty* party = G_CGameManager()->GetParty(static_cast<int>(partyIndex));
        if (party == 0)
        {
            throw static_cast<ENUM_ERROR>(0x1);
        }
        if (party->check_timer_key(static_cast<TIMER_MESSAGE>(0x2a),
                                   timerKey) == false)
        {
            throw static_cast<ENUM_ERROR>(0x17);
        }
        if (party->TimeoutCompleteAfterAssault())       // ORIG 返回值为包触发条件（test %al; je 跳过）
        {
            PacketGuard packet;
            packet.clear();                             // InterfacePacketBuf::clear @080cb8e6
            packet.put_header(1, 0xf8);
            packet.put_byte(1);
            packet.finalize(true);
            party->send_to_party(packet);
        }
    }
    catch (int)                             // [b28d-solo] ORIG landing pad
                                            // `cmp $0x1,%edx; je` + 存值槽 -0xc
                                            // ⇒ 定类型 catch(int)，非 catch-all。
    {
    }
    return 1;                               // [b28d-solo] P159：ORIG 尾 `mov $0x1,%eax`
                                            // （0x082ef9cb）⇒ 返回类型非 void。
}

// ===========================================================================
// _SendVictoryInfo —— ORIG T 0x082efcb2 (0x161)
// 包 header(0,0x10a)；PowerSide==1 时 [胜方连数 A][B]，否则反向；
// finalize(true) 后两队分别 send_to_party 或单人 Send。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（105=105）：单 party 槽 -0xc 复用两处 GetParty + P297 反条件
// `je <Send>` 布局；旧 N93 判词作废。ORIG 0x082efcb2。
#include "W6_AssaultMgrDeps/_SendVictoryInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x10a);
    if (userA->getPowerSide() == 1)
    {
        packet.put_int(userA->GetPowerWarPartyVictory());
        packet.put_int(userB->GetPowerWarPartyVictory());
    }
    else
    {
        packet.put_int(userB->GetPowerWarPartyVictory());
        packet.put_int(userA->GetPowerWarPartyVictory());
    }
    packet.finalize(true);

    // [b28d-solo] ORIG 单槽 -0xc 复用两处 GetParty（0x082efd7c/0x082efdb6）
    // + 反条件 `je <Send>` 布局（party!=0 直落 send_to_party）⇒ P297 反写。
    CParty* party = static_cast<CParty*>(userA->GetParty());
    if (party != 0)
    {
        party->send_to_party(packet);
    }
    else
    {
        userA->Send(packet);
    }
    party = static_cast<CParty*>(userB->GetParty());
    if (party != 0)
    {
        party->send_to_party(packet);
    }
    else
    {
        userB->Send(packet);
    }
}

// ===========================================================================
// check_party_teleport_user —— ORIG T 0x082efe14 (0x12d)
// 双方队伍传送状态 ∈[0,3] 时分别记 0x9c/0x9d；notifyErr 时回错包。返回错误码。
// ===========================================================================
#include "W6_AssaultMgrDeps/check_party_teleport_user.inc"
                                           CUser* destUser, bool notifyErr)
{
    // [b23S3C] P0/P48 槽映射：ORIG errCode→-0x10、party→-0xc（第二块 party
    // 清零复用同槽，0x9d 直覆 errCode，无独立 errDest 局部）；每块 state 判定
    // && 短路双臂各自重取 GetPartyTelePort()+get_state()（4 call/块），bool
    // 物化岛由 && 形复现。4.1 实参 spill（slotno→-0x1c/notify→-0x20，尾段
    // movsbl/cmpb 槽读）为跨版本形态（具名复制局部实测产双槽+截断对，更差）。
    int errCode = 0;
    CParty* party = 0;

    if (srcUser->CheckInParty() != 0)
    {
        party = static_cast<CParty*>(srcUser->GetParty());
    }
    if (party != 0)
    {
        if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                    ->get_teleport_state() >= 0
            && static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() <= 0x3)
        {
            errCode = 0x9c;
        }
    }
    party = 0;
    if (destUser->CheckInParty() != 0)
    {
        party = static_cast<CParty*>(destUser->GetParty());
    }
    if (party != 0)
    {
        if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                    ->get_teleport_state() >= 0
            && static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() <= 0x3)
        {
            errCode = 0x9d;
        }
    }
    if (notifyErr && errCode != 0)
    {
        srcUser->SendCmdPeerErrorPacket(
            static_cast<ENUM_CMDPACKET>(0xa),
            static_cast<ENUM_PEER_REQUEST_TYPE>(static_cast<int>(slotno)),
            static_cast<unsigned char>(errCode));
    }
    return errCode;
}

// ===========================================================================
// GetRequestAssaultNeedMoney —— ORIG T 0x082edc86 (0x333)
// 价格表 + chaos 时效折算 + 混沌点权重 + 等级差系数（浮点公式逐指令转写，
// ROUND 采用 ORIG fstcw(0xc)/fistpl 截断形态，等价 lrintf）。
// ===========================================================================
// ORIG 的浮点→整型截断序列（fnstcw/fldcw RC=0x0c/fistpl）由 4.4 -O0 的
// `static_cast<int>(float)` 直接产出；lrintf 形会发真 call。
#define ROUND_NEAREST_EVEN(v) (static_cast<int>(v))

// [b28d-solo-W6_AssaultMgrDeps] 封顶 N85（253/257 insn）：已修 GetAutoClientRate 取 target（真 bug）、
// 去 event 判空 / powerWarOn 局部 / chaosCapSec、ROUND 改 static_cast<int> 截断内联、
// CDataManager+0x600/604/608 具名视图（+0x608 为 float）。残余 = 4 处 x87 比较
// lowering（ORIG fucompp/fnstsw/test$5,ah/sete 各多 1 条）+ x87 求值序临时槽
// （-0x50/-0x4c/-0x48 vs -0x40 区；具名 float 临时探针反致浅槽漂移 0x0c）。
// [S2W6Deps 复核·负结论] gap=+4 归因确认 = **编译器中端固有差，源码不可造**：
// ORIG（原厂旧 gcc）每处浮点比较发 4 条 `fucompp; fnstsw %ax; test $0x45,%ah;
// sete %al`（5 处 ×4=20），我方 gcc11 发 3 条 `fucomip %st(1),%st; fstp %st(0);
// setae/seta %al`（5 处 ×3=15）⇒ 净 +4~+5。探针 /tmp/fpcmp/t.cpp（gcc11 -m32 -O0
// 编译 `a<=0.0f / a>=b / a>b / a-b<=0.0f` 四式）实测恒出 `fcomip/fucomip+setcc`，
// 不因源码改写（`>=`↔`<=` 翻转、具名 float 临时、`(double)`/`(float)` 混型）而变；
// 与 N97「有符号除法 lowering cmovs」同族。已由父泳道 N3Free171to190 复核接受。
// ORIG 0x082edc86。
#include "W6_AssaultMgrDeps/GetRequestAssaultNeedMoney.inc"
{
    // ORIG 无 event 具名槽（P0：指针留 %eax 直传虚调）+ 无判空（P171 照抄）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing(0))
    {
        return 0;
    }

    // ORIG 无具名 powerWarOn 局部（P0：&& 短路值仅存寄存器）。
    if (PowerWarOn() && attacker->getCurCharacVill() == 0x7)
    {
        return 0;
    }

    const int basePrice =
        G_CDataManager()->m_serverParameter.getAssaultPrice(
            attacker->get_charac_level());

    // 目标混沌点==0：主价格公式分支。
    if (target->GetCurCharacChaosPoint() == 0)
    {
        // ORIG 取 target 的 AutoClientRate（0x082edd3a `mov 0x10(%ebp),%eax`）。
        const float autoClientRate =
            static_cast<float>(target->GetAutoClientRate()) / (-100.0f) + 1.0f;
                                                            // _DAT_08c1d18c=-100.0f
        int one = 1;                                        // ORIG -0x24 `movl $0x1`
        float decaySec = static_cast<float>(
            GlobalData::s_systemTime_.getCurSec()
            - attacker->GetCurCharacChaosKillTime());
        if (decaySec <= 0.0f)
        {
            decaySec += 1.0f;
        }
        else
        {
            if (ROUND_NEAREST_EVEN(decaySec) > DM_FIELD(604))
            {
                decaySec = static_cast<float>(DM_FIELD(604));
            }
        }
        float upperBound =
            static_cast<float>(DM_FIELD(604)) * autoClientRate;
        if (upperBound <= 0.0f)
        {
            upperBound += 1.0f;
        }
        float factor = 1.0f;
        if (decaySec - upperBound <= 0.0f)
        {
            factor = (static_cast<float>(DM_FIELD(600))
                      - decaySec / upperBound) * one;
        }
        if (factor >= 1.0f)
        {
            if (factor > static_cast<float>(DM_FIELD(600)))
            {
                factor = static_cast<float>(DM_FIELD(600));
            }
        }
        else
        {
            factor = 1.0f;
        }

        // 等级差惩罚（CDataManager+0x608 系数）。
        const int levelDiff =
            attacker->get_charac_level() - target->get_charac_level();
        float levelPenalty = 1.0f;
        if (levelDiff > 8)
        {
            levelPenalty = DM_FIELD(608)
                               * static_cast<float>(levelDiff - 8) + 1.0f;
        }

        const float priced = static_cast<float>(ROUND_NEAREST_EVEN(
            static_cast<float>(basePrice) * factor * levelPenalty * autoClientRate));
        return ROUND_NEAREST_EVEN(
            static_cast<float>(
                G_CDataManager()->m_serverParameter
                    .getAssaultPriceChaospointWeight(
                        attacker->GetCurCharacChaosPoint()))
            * priced);
    }

    // 目标混沌点!=0：只按目标混沌权重缩放基础价。
    const float priceF = static_cast<float>(basePrice);
    return ROUND_NEAREST_EVEN(
        G_CDataManager()->m_serverParameter.getChaosPointWeight(
            target->GetCurCharacChaosPoint())
        * priceF);
}

// ===========================================================================
// OnRequestAssault —— ORIG T 0x082edfba (0x11bb)：袭击请求全量校验 + 创建场次。
// ===========================================================================
// [b28c-L7][SIGFIX:pvp_assault::CAssaultMgr::OnRequestAssault] int→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...(CUser*, CUser*, int*)"，tsv 第 14 行；
// mangled 不含返回类型⇒符号不变；体内 return 值仅 0/1）。
// [b28d-solo-W6_AssaultMgrDeps] 封顶 N82/N86（1156/1097，gap +59 = 4.4 帧/临时槽形态差）：
// §6.2 三件套①p167 CLEAR 202/202（0 missing/0 extra）；②逐块对齐 Ghidra C 并修
// 2 个真语义 bug（0x082ee06e attacker 的 AssaultedUserId、0x082ef0ce 错误包
// 漏 put_byte(4)）；③帧 sub $0x120 vs ORIG $0xdc + 缺 push %edi（4.4 临时槽更多）。
// [S2W6Deps 收敛记录] 1156 → **1108**（gap −59 → **−11**，共 −48 条），逐项（ORIG 地址）：
//   ① 082ee7be..082ee82c 曼哈顿距离：删 dx/distX/absDx/dy/distY/absDy 6 具名 + bool
//      tooFar 槽（-0x64/-0x60/-0x5c/-0x38/-0x34/-0x30/-0x55）→ 单条短路或 +
//      __builtin_abs（P0/P329，−20）；
//   ② 082ee88d..082ee8b5 vill/area：tooFar 三元嵌套 → 短路或（P324，−8）；
//   ③ 082ee61b..082ee64a blackCount：去具名槽 + signed→unsigned 比较（P0+类型轴；
//      ORIG `ja` 无符号，旧形 signed `jg` 在负值域运行行为不同 = 真语义修，−4）；
//   ④ 082ee1fb 删 s_villageMonsterMgr 多余 `!= 0` 判空（发明防御，−4）；
//   ⑤ 082eecb0/082eecbc authen/placeId 具名 → 实参内联（−4）；
//   ⑥ 082eeeb1 joinCount 具名 → 实参内联（−2）；⑦ 082eeec2 invenW → 直调（−2）；
//   ⑧ 082ee19a/082ee1a1/082ee1dd preventPeer 段：`!PowerWarOn()+else-if` → 真块
//      先写 + `if (flag) 0xfd else 0xfb`（P324，−6）；
//   ⑨ 082ee32b/082ee33c/082ee343 PowerWar 大块：then/else 交换 + ORIG `sete` 形
//      `if (vill == 0x7) {...} else throw 0xfc`（P324，−10）；
//   ⑩ 082ee971..082ee9b5 store 两守卫：`&&` → 嵌套 if（消 4.4 整体 bool 物化，−4）。
// 余 **11 条 = N 封顶**：push/pop %edi（ORIG 082ee369 用 %edi 保活 vill 跨
//   G_GameWorld() call，我方 %esi ⇒ N7 寄存器角色）+ 帧 0xe0 vs 0xdc（槽基址整体
//   平移 0x3c，N82/N95）+ `test %eax,%eax; setne %al` ×2（= GetAssaultPrevent
//   头签名 int，ORIG 8 位 `test %al,%al`，已报父泳道按 .h 项裁决）+ ORIG 独有的
//   movl $0 预清/物化段（我方缺，属 gcc 中端差）。
// ORIG 0x082edfba。
#include "W6_AssaultMgrDeps/OnRequestAssault.inc"
{
    CAssaultPlace* acquiredPlace = 0;                   // -0x94(%ebp)
    unsigned int nextPlaceId = 0;                       // -0x98(%ebp)
    int chargeMoney = 0;                                // -0x9c(%ebp)

    try
    {
        if (attacker->isCompetitionMercenary() != 0)
        {
            throw static_cast<ENUM_ERROR>(0xea);
        }
        if (target->isCompetitionMercenary() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x15);
        }

        if (!PowerWarOn())
        {
            // ORIG 082ee06e/082ee07b：attacker 的 AssaultedUserId vs target 的 unique_id
            // （旧实现两侧都取 target，真语义 bug）。
            if (attacker->GetCurCharacAssaultedUserId()
                != (static_cast<unsigned short>(target->get_unique_id())))
            {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0x1291,
                    "CAssaultMgr::OnRequestAssault ERROR - pAssaultedUser Unique ID is invalid");
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        bool preventPeerFlag = false;
        if (expert_job::CAlchemist::GetAssaultPrevent(attacker, &preventPeerFlag))
        {
            // ORIG 082ee19f..082ee1f6 / 082ee2xx 同形：`movzbl -0x9d; test; je 0xfb段`
            // 紧接 `throw 0xfd` ⇒ 原码书写序 = `if (preventPeerFlag) throw 0xfd;
            // throw 0xfb;`（P324 叶块书写序；我方旧序为 0xfb 先，两段各多 1 条 jmp）。
            // ORIG 082ee198 `test %al,%al; je 082ee198`（PowerWarOn()==0 跳到 else）
            // + then 块 `cmp $0x7,%al; setne; test; je 082ee1fb`（vill==7 跳过）
            // ⇒ 原码 = `if (PowerWarOn()) { if (vill != 0x7) {flag} } else {flag}`
            //（P324：真分支先写；我方旧形 !PowerWarOn+else-if 会多 1 条 xor + 块序反转）。
            if (PowerWarOn())
            {
                if (attacker->getCurCharacVill() != 0x7)
                {
                    if (preventPeerFlag)
                    {
                        throw static_cast<ENUM_ERROR>(0xfd);
                    }
                    throw static_cast<ENUM_ERROR>(0xfb);
                }
            }
            else
            {
                if (preventPeerFlag)
                {
                    throw static_cast<ENUM_ERROR>(0xfd);
                }
                throw static_cast<ENUM_ERROR>(0xfb);
            }
        }

        // ORIG 082ee1fb 直接把全局对象地址当 this 传入（`mov &s_villageMonsterMgr,
        // %eax; mov %eax,(%esp); call OnEvent`，无判空分支）⇒ 原码无防御。
        if (GlobalData::s_villageMonsterMgr->OnEvent())
        {
            throw static_cast<ENUM_ERROR>(0xd9);
        }

        // P0：ORIG 082ee238/082ee331 各自独立 call IsPowerWarEventOn/getCurCharacVill
        // （无槽往返 ⇒ 原码无具名 bool 局部）。
        if (PowerWarOn() && attacker->getCurCharacVill() == 0x7 && result == 0)
        {
            GlobalData::s_power_manager->GetPowerWarLog()->IncAssaultAttemptCount();
        }

        // ORIG 082ee298 直接解引用返回值（无判空、无 event 具名槽，P171/P0）。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x2a)->IsEventing(0))
        {
            throw static_cast<ENUM_ERROR>(0xb2);
        }

        if (attacker->getCurCharacVill() == 0x8)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        // ORIG 082ee32b `je 082ee4f4`（PowerWarOn()==0 → 尾随 else 块）+ 082ee343
        // `je 082ee4c8`（vill!=7 → 尾随 throw 0xfc 段）⇒ 原码书写序 = `if
        // (PowerWarOn()) { ...大块... } else { vill==7 → 0x13 }`（P324 叶块序）。
        if (PowerWarOn())
        {
            // ORIG 082ee33c `cmp $0x7,%al; sete %al; test %al,%al; je 082ee4c8`
            // （082ee4c8 = throw 0xfc 段）⇒ 原码 = `if (vill == 0x7) {...} else
            // throw 0xfc;`（sete 而非 setne，P324 叶块书写序）。
            if (attacker->getCurCharacVill() == 0x7)
            {
                // ORIG 082ee349/082ee35e：实参内联（area 留 %ebx 无槽，P0/P183）。
                if (G_GameWorld()->IsPowerWarSafeZone(
                        static_cast<int>(attacker->getCurCharacVill()),
                        attacker->get_area(false)))
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
                if (attacker->getPowerSide() == target->getPowerSide()
                    || attacker->isJoinPowerWar() == 0
                    || target->isJoinPowerWar() == 0)
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
                CParty* party = static_cast<CParty*>(attacker->GetParty());
                if (party != 0 && party->CanDoPartyActionAssalutState() == 0)
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
                if (target->GetCurCharacSuperState() != 0)
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
            }
            else
            {
                throw static_cast<ENUM_ERROR>(0xfc);
            }
        }
        else
        {
            if (attacker->getCurCharacVill() == 0x7)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        if (expert_job::CAlchemist::GetAssaultPrevent(
                target, static_cast<bool*>(0)))
        {
            if (!PowerWarOn())
            {
                throw static_cast<ENUM_ERROR>(0x11);
            }
        }

        if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 3)
        {
            // ORIG 082ee5b6/082ee5c8 两次独立 getCurCharacNo（无 targetNo 局部）。
            if (target->getCurCharacNo() == 0x1eaa0d
                || target->getCurCharacNo() == 0x2c55a8)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        if (!m_bAssaultChannel)
        {
            // ORIG 082ee62b `ja`（无符号比较）+ 值直留 %eax（无槽，P0/P197）：
            // 原码为 unsigned 比较、无具名局部。
            if (static_cast<unsigned int>(target->getCurrCharacBlackCount()) < 100
                && target->GetCurCharacChaosPoint() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x8);
            }
        }

        if (attacker->GetAssaultPlace() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (target->GetAssaultPlace() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x12);
        }
        if (attacker->getCurCharacR() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (attacker->getCurCharacInvenR() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (attacker->getCurCharacNo() == target->getCurCharacNo())
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        // ORIG 082ee7be..082ee82c：单条短路或 + abs builtin（dx/absDx/dy/absDy
        // 全无具名槽：X 段 `cmp $0x64,%eax; jg`、Y 段 `jle`，值只在寄存器里
        // 走 sar/xor/sub；P0/P329）。
        if (__builtin_abs(static_cast<int>(attacker->get_posX())
                          - static_cast<int>(target->get_posX())) > 100
            || __builtin_abs(static_cast<int>(attacker->get_posY())
                             - static_cast<int>(target->get_posY())) > 100)
        {
            throw static_cast<ENUM_ERROR>(99);          // ORIG movl $0x63
        }

        // ORIG 082ee88d..082ee8b5：同形短路或（vill 用 `jne` 到 TRUE、area 用
        // `je` 到 FALSE），无 tooFar 槽复用。
        if (attacker->getCurCharacVill() != target->getCurCharacVill()
            || attacker->get_area(false) != target->get_area(false))
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        if (!PowerWarOn())
        {
            if (IsStayGate(attacker) || IsStayGate(target))
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        // ORIG 082ee95a/082ee9ba 两次独立 GetInstancePrivateStoreMgr（无 storeMgr 局部）。
        private_store::CPrivateStore* store =
            private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(attacker);
        // ORIG 082ee971..082ee9b5：`cmpl $0x0,-0x38; setne %al; test %al,%al; je
        // 082ee9ba` + 第二守卫同落 082ee9ba（**无** `mov $1/jmp/mov $0/test` 整体
        // 物化）⇒ 原码为嵌套 if（P324 叶块书写序；`&&` 直写令 4.4 物化整体多 4 条）。
        if (store != 0)
        {
            if (store->isDollCreated() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x3c);
            }
        }
        store = private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(target);
        if (store != 0)
        {
            if (store->isDollCreated() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x3c);
            }
        }

        CParty* attackerParty = 0;
        if (attacker->CheckInParty() != 0)
        {
            attackerParty = static_cast<CParty*>(attacker->GetParty());
        }
        if (attackerParty != 0 && attackerParty->IsExistUser(target) != 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (PowerWarOn()
            && target->getCurCharacVill() == 0x7)
        {
            CParty* targetParty = static_cast<CParty*>(target->GetParty());
            if (targetParty != 0 && targetParty->IsThereGhostInParty() != 0)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        AvailableAssault(attacker, attacker);
        AvailableAssault(target, attacker);

        chargeMoney = attacker->GetCurCharacAssaultMoney();
        // ORIG 082eeb34/082eeb62 两次 getCurCharacInvenR + get_money（差额在异常路径重算）。
        if (static_cast<const CInventory*>(attacker->getCurCharacInvenR())->get_money()
            < chargeMoney)
        {
            chargeMoney =
                chargeMoney
                - static_cast<const CInventory*>(attacker->getCurCharacInvenR())->get_money();
            throw static_cast<ENUM_ERROR>(0xa);
        }

        // param_3 非空＝纯校验模式（DryRun）：通过全部校验后立即成功返回。
        if (result != 0)
        {
            return true;                                 // ORIG +0xbf3 jne +0x11a9
        }

        CAssaultPlaceFactory* factory = GetInstanceAssaultPlaceFactory();
        acquiredPlace = factory->AcquireAssaultPlace();
        if (acquiredPlace == 0)
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }
        acquiredPlace->Reset();
        nextPlaceId = _GetNextAssaultPlaceId();
        acquiredPlace->SetAssaultPlaceId(nextPlaceId);
        acquiredPlace->MakeTeam(attacker, target, chargeMoney);

        std::pair<std::map<unsigned int, CAssaultPlace*>::iterator, bool> inserted =
            m_places.insert(std::make_pair(nextPlaceId, acquiredPlace));
        (void)inserted;

        // ORIG 082eecb0..082eece6：两结果只在 %ebx/%eax 跨 call 保活（GetAuthenValue
        // →%ebx、GetAssaultPlaceId→%eax 直落实参，无槽往返）⇒ 原码实参内联（P0/P329）。
        TimerCompleteLoadAssaultTimeout::registNextTimer(
            0x3c, acquiredPlace->GetAssaultPlaceId(),
            acquiredPlace->GetAuthenValue());

        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7)
        {
            chargeMoney = 0;
        }

        if (chargeMoney > 0)
        {
            Inven_Item equipSlot =
                static_cast<const CInventory*>(attacker->getCurCharacInvenR())->GetInvenSlot(0, 0xb);
            const int itemIdx = equipSlot.m_addInfo;
            CDataManager* cdm = G_CDataManager();
            const CItem* discountItem =
                (cdm != 0) ? cdm->find_item(itemIdx) : 0;
            if (discountItem != 0)
            {
                const float discount =
                    static_cast<const CEquipItem*>(discountItem)
                        ->getUpgradeCostDiscount();
                const float pricedF =
                    ((discount * static_cast<float>(chargeMoney)) / (-100.0f)
                     + static_cast<float>(chargeMoney));
                chargeMoney = static_cast<int>(pricedF);    // ORIG fstcw/fistpl 截断
            }
            // ORIG 082eeec2：getCurCharacInvenW 结果直留 %eax 当 this（无具名槽）。
            attacker->getCurCharacInvenW()->use_money(
                chargeMoney, static_cast<eMoneySubReason>(0x15), true);
            attacker->SetCurCharacAssaultMoney(0);
            attacker->SetCurCharacAssaultedUserId(0);
        }
        attacker->IncCurCharacAssaultCount();

        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7)
        {
            // ORIG 082eeeb1/082eeecd 两次独立 GetPowerWarLog（无 log 局部）；
            // GetAssaulterCount 结果直落 0x4(%esp)（无 joinCount 槽，P0/P329）。
            GlobalData::s_power_manager->GetPowerWarLog()->IncTotalJoinAssaultUser(
                acquiredPlace->GetAssaulterCount());
            GlobalData::s_power_manager->GetPowerWarLog()->IncAssaultAttemptSuccessCount();
        }

        _SendVictoryInfo(attacker, target);

        {
            PacketGuard packet;
            packet.put_header(0, 0x8);
            packet.put_short(
                static_cast<int>(
                    static_cast<unsigned short>(target->get_unique_id())));
            packet.put_byte(4);
            packet.put_int(chargeMoney);
            packet.finalize(true);
            attacker->Send(packet);
        }
        return true;
    }
    catch (ENUM_ERROR err)
    {
        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7 && result == 0)
        {
            // ORIG catch 内计数用 IncAssaultAttemptFailureCount（0x82f0850）
            GlobalData::s_power_manager->GetPowerWarLog()
                ->IncAssaultAttemptFailureCount();
        }
        if (result != 0)
        {
            *result = static_cast<int>(err);
            return false;
        }
        FreeAssaultPlace(static_cast<int>(nextPlaceId), acquiredPlace);
        {
            PacketGuard packet;
            packet.put_header(1, 0xa);
            packet.put_byte(0);
            packet.put_byte(static_cast<int>(err));
            packet.put_byte(4);                     // ORIG 082ef0ce（旧实现漏写，真语义缺口）
            packet.put_int(chargeMoney);
            packet.finalize(true);
            attacker->Send(packet);
        }
        return true;
    }
}

}  // namespace pvp_assault

// ===========================================================================
// IsStayGate —— ORIG T 0x082edc05 (0x81)，全局自由函数（被本 TU 使用）。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（40=40）：P225 `while (x==0) { return true; break; }` 复现 4.1
// 槽比零 sete/test/je 物化；P212 全头部声明对齐双预清；area 内联保 %ebx 跨 call。
// 旧 P197④「不可翻」判词作废。ORIG 0x082edc05。
#include "W6_AssaultMgrDeps/IsStayGate_1.inc"
{
    // ORIG 0x082edc05：入口 `movl $0x0,-0x10/-0xc(%ebp)` 双预清（P198① 两句式）
    // ⇒ 原码为「声明=0 + 独立赋值」两段。
    // [b28d-solo] P212 全头部声明：ORIG 两条预清 `movl $0,-0x10/-0xc` 连续
    // 落函数头（0x082edc0c/0x082edc13）⇒ 两个指针局部都声明在函数头。
    GameWorld* world = 0;
    Village* village = 0;
    world = G_GameWorld();
    // [b28d-solo] P225：ORIG 槽比零物化 `cmpl $0,-0x10; sete %al; test %al,%al;
    // je`（0x082edc22..0x082edc2b）⇒ while(cond){return X; break;} 形可翻。
    while (world == 0)
    {
        return true;
        break;
    }
    village = world->getUserVillage(user);
    while (village == 0)
    {
        return true;
        break;
    }
    // [b28d-solo] P0：ORIG get_area 结果留 %ebx 跨 get_gate_area call（无槽往返）
    // ⇒ 原码无具名 area 局部，使用点内联表达式。
    return user->get_area(false) == village->get_gate_area();
}

// ===========================================================================
// CPowerWarLog —— 生命周期与统计小方法族（ORIG 符号地址逐一标注）。
// ===========================================================================
#include "W6_AssaultMgrDeps/CPowerWarLog.inc"
{
    Reset();
}

// [b28d-19] AE（P54 逐字段具名赋值修复，34=34）；ORIG 0x08480232。
#include "W6_AssaultMgrDeps/Reset_1.inc"
{
    // ORIG 0x08480232：15 条独立 `mov 0x8(%ebp),%eax; movl $0x0,disp(%eax)`
    // （逐字段重读 this、无循环变量/无栈帧）⇒ 原码是逐成员赋值（P54/P171）。
    m_attemptCount = 0;
    m_successCount = 0;
    m_failureCount = 0;
    m_loadingTimeout = 0;
    m_loadingTimeoutSecede = 0;
    m_p2pTimeoutFailure = 0;
    m_p2pTimeoutSecede = 0;
    m_p2pBadFailure = 0;
    m_p2pBadSecede = 0;
    m_startSuccess = 0;
    m_startFailure = 0;
    m_startSuccessCount = 0;
    m_field30 = 0;
    m_totalJoinAssaultUser = 0;
    m_totalLeaveAssaultUser = 0;
}

#include "W6_AssaultMgrDeps/IncAssaultAttemptCount.inc"
{
    ++m_attemptCount;
}
#include "W6_AssaultMgrDeps/IncAssaultAttemptFailureCount.inc"
{
    ++m_failureCount;
}
#include "W6_AssaultMgrDeps/IncAssaultAttemptSuccessCount.inc"
{
    ++m_successCount;
}
#include "W6_AssaultMgrDeps/IncLoadingTimeOutCountSecede.inc"
{
    ++m_loadingTimeoutSecede;
}
#include "W6_AssaultMgrDeps/IncP2PTimeOutCountSecede.inc"
{
    ++m_p2pTimeoutSecede;
}
#include "W6_AssaultMgrDeps/IncP2PBadCountFailure.inc"
{
    ++m_p2pBadFailure;
}
#include "W6_AssaultMgrDeps/IncP2PBadCountSecede.inc"
{
    ++m_p2pBadSecede;
}
#include "W6_AssaultMgrDeps/IncStartSuccessCount.inc"
{
    ++m_startSuccess;
}
#include "W6_AssaultMgrDeps/IncStartFailureCount.inc"
{
    ++m_startFailure;
}
#include "W6_AssaultMgrDeps/IncTotalJoinAssaultUser.inc"
{
    m_totalJoinAssaultUser += n;
}
#include "W6_AssaultMgrDeps/IncTotalLeaveAssaultUser.inc"
{
    ++m_totalLeaveAssaultUser;
}

#include "W6_AssaultMgrDeps/GetPowerWarLog.inc"
{
    return &m_powerWarLog;   // +0x1b4 嵌入对象
}

// [b28d-solo-W6_AssaultMgrDeps] 封顶 N82（275/278）：p167 CLEAR 40/40
// （0 missing/0 extra）；残余 = 帧 sub $0x178 vs ORIG push %ebx+sub $0x174、
// cMyTrace 兄弟块临时槽方向（ORIG -0x158→-0xa8 深→浅 vs 4.4 -0x28→-0xe8 浅→深，
// P221）与 ORIG 字段值 %ebx 跨 ctor 保活（P227）。ORIG 0x084802cc。
#include "W6_AssaultMgrDeps/PrintPowerWarAssultReport.inc"
{
    // docs/class_func_reports/CPowerWarLog/PrintPowerWarAssultReport.md
    // 常量 .rodata@0x8c72fd8 = double 100.0（objdump 实测 00005940）。
    // P221/P227：ORIG 的 20 个 sibling 块 cMyTrace 槽深→浅（-0x158→-0x28，
    // 4.1「先声明更深」），4.4 对具名对象恒浅→深 ⇒ 改全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x32f, 0)(
        "-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x330, 0)(
        "|                                  POWAR WAR ASSAULT RESULT REPORT                                          |");
    cMyTrace(__PRETTY_FUNCTION__, 0x331, 0)(
        "-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x334, 0)("1. POWER WAR ATTEMPT REPORT");
    cMyTrace(__PRETTY_FUNCTION__, 0x335, 0)(
        "- POWER WAR ATTEMPT                : %d", m_attemptCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x336, 0)(
        "- POWER WAR ATTEMPT SUCCESS        : %d", m_successCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x337, 0)(
        "- POWER WAR ATTEMPT FAILURE        : %d", m_failureCount);
    double failureRatio = 0.0;
    if (m_attemptCount != 0)
        failureRatio = 100.0 * ((double)m_failureCount / (double)m_attemptCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x340, 0)(
        "- POWER WAR ATTEMPT FAILURE RATIO  : %.2f%%", failureRatio);
    cMyTrace(__PRETTY_FUNCTION__, 0x343, 0)(
        "2. POWER WAR LOADING & P2P FAILURE REPORT");
    cMyTrace(__PRETTY_FUNCTION__, 0x344, 0)(
        "- POWER WAR LOADING TIMEOUT        : %d", m_loadingTimeout);
    cMyTrace(__PRETTY_FUNCTION__, 0x345, 0)(
        "- POWER WAR P2P TIMEOUT            : %d", m_p2pTimeoutFailure);
    cMyTrace(__PRETTY_FUNCTION__, 0x346, 0)(
        "- POWER WAR P2P BAD                : %d", m_p2pBadFailure);
    double startFailureRatio = 0.0;
    if (m_successCount != 0)
        startFailureRatio = 100.0 * ((double)m_startFailure / (double)m_successCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x34f, 0)(
        "- POWER WAR FAILURE RATIO         : %.2f%%", startFailureRatio);
    cMyTrace(__PRETTY_FUNCTION__, 0x352, 0)("3. POWER WAR START SECEDE REPORT");
    cMyTrace(__PRETTY_FUNCTION__, 0x353, 0)(
        "- POWER WAR LOADING SECEDE        : %d", m_loadingTimeoutSecede);
    cMyTrace(__PRETTY_FUNCTION__, 0x354, 0)(
        "- POWER WAR P2P SECEDE            : %d", m_p2pTimeoutSecede);
    cMyTrace(__PRETTY_FUNCTION__, 0x355, 0)(
        "- POWER WAR P2P Bad               : %d", m_p2pBadSecede);
    cMyTrace(__PRETTY_FUNCTION__, 0x357, 0)(
        "4. POWER WAR SUCCESS/FAILURE INFOMATION");
    cMyTrace(__PRETTY_FUNCTION__, 0x358, 0)(
        "- SUCCESS COUNT                   : %d", m_startSuccessCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x359, 0)(
        "-------------------------------------------------------------------------------------------------------------");
}
