// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_AssaultPlaceDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// W6_AssaultPlaceDepsB.cpp —— Wave6 后半组 30 个 pvp_assault::CAssaultPlace
// ORIG-only 方法（/tmp/only_orig/pvp_assault.txt 54-83 行）。
// 全部语义取自 docs/class_func_reports/pvp_assault__CAssaultPlace/ 对应报告，
// 汇编为准；控制流 / 局部变量 / 调用序 / 常量逐字节转写。
#include "CAssaultMgr.h"

#include <string.h>

#include "CPowerManager.h"          // IsPowerWarEventOn / GetPowerWarLog /
                                    // IncreaePowerWarPointInChannel / ENUM_POWER_SIDE_TYPE
#include "CUser.h"                  // Send / GetParty / get_area / get_posX /
                                    // get_unique_id / get_acc_id / get_charac_no /
                                    // getHackAnalyzer / getPowerSide / cUserHistoryLog
#include "CUserCharacInfo.h"
#include "CParty.h"                 // send_to_party / get_user / get_member_count /
                                    // SetAssaultState / straight-victories 族
#include "PacketGuard.h"            // PacketGuard : InterfacePacketBuf
#include "GlobalData.h"             // s_power_manager / s_systemTime_ / s_guild_proxy_mgr
#include "GameWorld.h"              // G_GameWorld / send_to_vill / send_all / get_user_id_list
#include "CGuildServerProxy.h"      // SendPowerWarPoint / CServerProxyMgr 使用点
#include "CServerProxyMgr.h"        // GetServerProxy 完整类型
#include "CHackAnalyzer.h"          // WongWork::CHackAnalyzer::addServerHackCnt
#include "MSG_DIE_ASSAULT_PLAYER.h"
#include "LogManager.h"
#include "GameTypes.h"              // cMyTrace

#include "Packet_Guild_Change_Power_War_Point.h"   // 本批新建权威头（广播过）

// W7_BuildHarmonize 去重：pvp_assault::GetDirectionByTeam（ORIG T 0x82e564c，
// team 0→6、1→4、其他→0）唯一 owner 为 CAssaultMgr.cpp。
namespace pvp_assault { int GetDirectionByTeam(ENUM_ASSAULT_TEAM team); }

namespace {

}  // namespace

using pvp_assault::CAssaulter;

namespace pvp_assault
{

// ---------------------------------------------------------------------------
// 包构造 helper 组
// ---------------------------------------------------------------------------

// ORIG T 0x82e8810 _MakeCompleteLoad(PacketGuard*, int)
#include "W6_AssaultPlaceDepsB/_MakeCompleteLoad.inc"
{
    p->put_header(0, 0x81);
    // ORIG 无 uid 局部(P0 右值直传): GetUserUniqueId 结果 movzwl 直入 put_short
    p->put_short(m_assaulters[idx].GetUserUniqueId() & 0xffff);
    p->finalize(true);
}

// ORIG T 0x82e886a _MakeStartAssault(PacketGuard*)
#include "W6_AssaultPlaceDepsB/_MakeStartAssault.inc"
{
    p->put_header(0, 0x82);
    p->finalize(true);
}

// ORIG T 0x82e891e _MakeDieAssaultPlayer(PacketGuard*, int)
#include "W6_AssaultPlaceDepsB/_MakeDieAssaultPlayer.inc"
{
    p->put_header(0, 0x83);
    p->put_short(m_assaulters[idx].GetUserUniqueId() & 0xffff);
    p->finalize(true);
}

// ORIG T 0x82e8978 _MakeRevivalAssaultPlayer(PacketGuard*, int)（ORIG 未用 this）
#include "W6_AssaultPlaceDepsB/_MakeRevivalAssaultPlayer.inc"
{
    p->put_header(0, 0x85);
    p->put_short(idx);
    p->finalize(true);
}

// ORIG T 0x82e88a0 ... 见 A 组 _MakeEndAssault —— 此处不重复。
// ORIG T 0x82e852e _MakeMoveAssaultPlace(PacketGuard*)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 200/200（p167 30/30 CLEAR）
//   修法：m_field104 直读、i 改 unsigned（setbe/ja/jbe）、P191 双初始化、P198③ 双槽
//   预清、continue 形；P0 槽-局部律——第二段复用 count/countIdx（ORIG 仅 5 个标量
//   槽：leaderUid0/1 + i + count + countIdx，listCount/listIdx 为多余局部 ⇒ 帧 +0x10）。
//   残差：N82/N95 槽平移（4.4 标量槽整体深 4）。
#include "W6_AssaultPlaceDepsB/_MakeMoveAssaultPlace.inc"
{
    p->put_header(0, 0x80);
    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82e8554 直读 +0x104（无 getter call）。
    p->put_int(m_field104);                     // +0x104
    p->put_byte(m_pos.m_x);                     // +0x118 vill（既沿头字段名）
    p->put_byte(m_pos.m_y);                     // +0x11c area
    unsigned short leaderUid0 = 0;
    if (m_checkUser != 0)                                       // +0x134
        leaderUid0 = m_checkUser->get_unique_id();
    p->put_short(leaderUid0);
    unsigned short leaderUid1 = 0;
    if (m_field138 != 0)                                        // +0x138
        leaderUid1 = m_field138->get_unique_id();
    p->put_short(leaderUid1);    // +0x138

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82e8611..0x82e86d0 槽序：
    //   i / count / countIdx；P191 双初始化形（声明=0 + for 初始化各一条）
    //   + P198③ 双槽置 0 两段式；i 为 unsigned（setbe/ja/jbe）。
    // 第一段：场内成员列表
    unsigned int i = 0;
    int count = 0;
    int countIdx = 0;
    countIdx = p->get_index();                  // PacketGuard 局部 idx 补丁槽
    p->put_byte(count);
    for (i = 0; i <= 7; ++i) {                  // cmpl $0x7 … setbe（无符号 ≤7）
        // P223：正极性 guard-continue 复现 ORIG `test %al,%al; jne <nop 岛>`。
        if (m_assaulters[i].Empty())
            continue;
        p->put_byte(i);
        p->put_byte(m_assaulters[i].GetTeam() + 1);       // lea 0x1(%eax)
        // P0：ORIG 无 uid 槽（`call; movzwl %ax,%edx` 直传）——不设具名局部。
        p->put_short(m_assaulters[i].GetUserUniqueId());
        count++;
    }
    p->put_byte(countIdx, count);

    // 第二段：同 vill/area 在线用户 id 列表（上限 10）
    // [b28d-solo-W6_AssaultPlaceDepsB] P0：ORIG 复用 count/countIdx 两槽（P198③
    //   双槽置 0 两段式），不新增 listCount/listIdx；置 0 顺序 countIdx 先于
    //   count（ORIG 0x82e86xx 清零序列 -0x18 → -0xc）。
    countIdx = 0;
    count = 0;
    countIdx = p->get_index();
    p->put_byte(count);
    std::vector<unsigned short> idList;
    G_GameWorld()->get_user_id_list(m_pos.m_x, m_pos.m_y, idList);
    for (i = 0; i < 10 && i < idList.size(); ++i) {
        p->put_short(idList[i]);
        count++;
    }
    p->put_byte(countIdx, count);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// 发送组
// ---------------------------------------------------------------------------

// ORIG T 0x82e6d92 _SendPacketToVill(PacketGuard&)
#include "W6_AssaultPlaceDepsB/_SendPacketToVill.inc"
{
    if (m_pos.m_x != 0)                         // +0x118 vill id 非零才发
        G_GameWorld()->send_to_vill(m_pos.m_x, packet);
}

// ORIG T 0x82e7986 _SendUserMiniInfo()
#include "W6_AssaultPlaceDepsB/_SendUserMiniInfo.inc"
{
    PacketGuard packet;
    _MakeUserInfo(&packet, static_cast<ENUM_USERINFO>(0));
    G_GameWorld()->send_all(packet);
}

// ORIG T 0x82e7a5e _SendUserUdpIpPort()
#include "W6_AssaultPlaceDepsB/_SendUserUdpIpPort.inc"
{
    PacketGuard packet;
    _MakeUserUdpIpPort(&packet);
    SendPacket(packet);
}

// ORIG T 0x82e8de2 _MakeUserUdpIpPort(PacketGuard*)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 166/166（p167 33/33 CLEAR）
//   get_mtu 段（同型 int 实参）4.4 恒 packet 先载，ORIG 为 `mov %eax,%edx` 值先
//   中转 ⇒ P226 裸语句表达式 `p->put_int(({ get_mtu(); }))` 复现（cast/双 cast
//   两形实测不翻），ndiff 7→0。
#include "W6_AssaultPlaceDepsB/_MakeUserUdpIpPort.inc"
{
    // b28 BM4 判词（N18a/N7）：fresh .o 实残 1 条 = ORIG +0x175 stamina 段
    // `movsbl %al,%edx` 拓宽即落第二寄存器 + receiver 重载（4.4 恒 `%eax`
    // 单段直落参数槽，冗余 regmove 不可造；P211 异型化探针 pbq 同形已证）。
    // 其余 6 行 difflib 连锁移位由该 ±1 行产生。无源码解，语义等价封顶。
    p->put_header(0, 0xb);
    int countIdx = 0;
    int count = 0;
    countIdx = p->get_index();
    p->put_byte(count);
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        p->put_short(m_assaulters[i].GetUser()->get_unique_id() & 0xffff);
        p->put_int(static_cast<int>(
            m_assaulters[i].GetUser()->m_network.get_inner_ip()));
        p->put_int(static_cast<int>(
            m_assaulters[i].GetUser()->m_network.get_ip()));
        p->put_short(m_assaulters[i].GetUser()->m_network.get_port() & 0xffff);
        p->put_int(m_assaulters[i].GetUser()->get_acc_id());
        p->put_byte(static_cast<int>(
            m_assaulters[i].GetUser()->m_network.get_nat_type()));
        // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82e8f9c 为 `mov %eax,%edx;
        // mov 0xc(%ebp),%eax` 值先装载形；4.4 对同型 int 实参恒折 packet 先载
        // （enum/unsigned cast 两形实测不翻）⇒ P226 裸语句表达式复现值先中转。
        p->put_int(({ m_assaulters[i].GetUser()->m_network.get_mtu(); }));
        // [b23S3C] P48 去缓存 + P191 极性：ORIG 每用点重取 getCurCharacR()（无 ch 槽），
        // 且以指针真值判定（test;setne;test;je），m_stamina 落 fall-through。
        if (m_assaulters[i].GetUser()->getCurCharacR())
            p->put_byte(
                m_assaulters[i].GetUser()->getCurCharacR()->m_stamina);
        else
            p->put_byte(0);
        count++;
    }
    p->put_byte(countIdx, count);
    p->finalize(true);
}

// ORIG T 0x82ecdbc _SendVictoryBonus(CUser*, int, int, int, int)
#include "W6_AssaultPlaceDepsB/_SendVictoryBonus.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x108);
    packet.put_int(a + 1);
    packet.put_int(b);
    packet.put_int(c);
    packet.put_int(d);
    packet.finalize(true);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
        party->send_to_party(packet);
    else
        user->Send(packet);
}

// ORIG T 0x82ec808 _SendPowerWarAssaultResultToUser(CUser*, int)
#include "W6_AssaultPlaceDepsB/_SendPowerWarAssaultResultToUser.inc"
{
    PacketGuard packet;
    packet.put_header(0, 200);
    packet.put_byte(point);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG T 0x82e8300 _MoveAssaultPlace()
#include "W6_AssaultPlaceDepsB/_MoveAssaultPlace.inc"
{
    PacketGuard packet;
    _MakeMoveAssaultPlace(&packet);
    SendPacket(packet);
}

// ---------------------------------------------------------------------------
// 连接 P2P 统计组
// ---------------------------------------------------------------------------

// ORIG T 0x82e927e _IsCannotConnectP2P(int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 26/26（p167 1/1 CLEAR）
//   修法：bool 返回 + 合并短路（byte!=0 && !Empty()）+ 嵌套 if 复现两 je 共享尾岛；
//   P311 成员数组视图 `m_assaulters[a].m_connectIp[b]` 复现 ORIG 装载序（ndiff 11→1，
//   唯一残差 +0x15 常量折叠由 AE 口径归一）。
// [b28c-L7] 返回类型 int→bool（判词见 CAssaultMgr.h）：ORIG 0x82e927e 体尾
// `mov $0x0/$0x1,%eax` 与 bool 归一形一致；改型使两处调用点复现
// `xor $1,%eax` / `test %al,%al` 直测形。
#include "W6_AssaultPlaceDepsB/_IsCannotConnectP2P.inc"
{
    // asm：offset = this + a*0x20 + b + 0x15 的单字节（a<<5 后按字节加 b，
    //   并非第二个槽基址）；该字节非零且 m_assaulters[b] 非空 → 判"无法连接"。
    // [b28d-26] P183：去 p 局部缓存（ORIG 0x82e927e 全表达式直算，无 -0xc 槽，
    //   帧 sub $0x18 vs 我们 $0x28）。
    // ORIG 0x82e927e 偏移拆分：`add $0x10,%eax; movzbl 0x5(%eax)`。
    // [b28d-solo-W6_AssaultPlaceDepsB] P133/P168a①：ORIG 为「byte!=0 && !Empty()」
    // 合并短路——两条件各以 je 直跳共享 mov $1 尾岛（call 后 xor $1,%eax）。
    // 独立早返形（if(byte==0)return true; if(Empty)return false;）反产 jne+早岛。
    // [b28d-solo-W6_AssaultPlaceDepsB] P311 成员数组视图：CAssaulter 的
    //   m_connectIp[9] 位于 +0x15 ⇒ 直写 `m_assaulters[a].m_connectIp[b]`
    //   复现 ORIG 装载序（a→%ecx / b→%eax / this→%edx / shl / add / lea）。
    //   残差：ORIG 把 +0x15 拆成 `add $0x10` + disp `0x5`，4.4 恒折单条
    //   `add $0x15`（P30/N4/N102 常量折叠族，源形不可择）。
    // [b28d-solo-W6_AssaultPlaceDepsB] P225 推广：`&&` 合并形被 4.4 折成
    //   bool 归一（mov $1/test/je/mov $0 尾段，+5 条）；拆嵌套 if 复现 ORIG
    //   两条件各自 je 直跳共享 mov $1 尾岛形。
    if (static_cast<unsigned char>(m_assaulters[a].m_connectIp[b]) != 0) {
        if (!m_assaulters[b].Empty())
            return false;
    }
    return true;
}

// ORIG T 0x82e93bc _GetConnectP2PPoint(int)
#include "W6_AssaultPlaceDepsB/_GetConnectP2PPoint.inc"
{
    int points = 0;                             // movl $0x0,-0x10(%ebp)
    for (int i = 0; i <= 7; ++i) {              // cmpl $0x7 … setle
        if (idx != i) {
            // [b28c-L7] P168a①：callee 改 bool ⇒ 裸形 `if (!call())` 直产
            // ORIG 0x82e93c0 `call; xor $1,%eax; test %al,%al; je`。
            if (!_IsCannotConnectP2P(idx, i))
                points++;
        }
    }
    return points;
}

// ORIG T 0x82e9412 _GetConnectP2PMaxMinPoint(int&, int&)
#include "W6_AssaultPlaceDepsB/_GetConnectP2PMaxMinPoint.inc"
{
    maxPt = 0;                                  // 第 1 引用参 init 0 → 最大值
    minPt = 8;                                  // 第 2 引用参 init 8 → 最小值
    // [b28d-26] ORIG 0x82e9412：pt 声明于循环外（-0x10 槽预清 0），循环变量 i 落
    //   -0xc；Empty() 早返 continue 形产 `test %al,%al; jne <continue>`（无 xor）。
    int pt = 0;
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        pt = _GetConnectP2PPoint(i);
        if (pt < minPt)
            minPt = pt;
        if (maxPt < pt)
            maxPt = pt;
    }
}

// ORIG T 0x82e92c6 _ClearConnectP2PAbsolute(int)
#include "W6_AssaultPlaceDepsB/_ClearConnectP2PAbsolute.inc"
{
    for (int i = 0; i <= 7; ++i) {
        if (idx != i) {
            // [b28c-L7] P168a①：callee bool ⇒ 裸形直产 ORIG 0x82e92e6
            // `call; test %al,%al; je`。
            if (_IsCannotConnectP2P(idx, i)) {
                if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                    GlobalData::s_power_manager->GetPowerWarLog()->IncP2PBadCountSecede();
                }
                LeaveAssaultPlace(i, true, false);
            }
        }
    }
    return 1;
}

// ORIG T 0x82e9358 _ClearConnectP2PRelative(int)
#include "W6_AssaultPlaceDepsB/_ClearConnectP2PRelative.inc"
{
    for (int i = 2; i <= 7; ++i) {
        if (_GetConnectP2PPoint(i) == idx)
            LeaveAssaultPlace(i, true, false);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// PowerWar 组
// ---------------------------------------------------------------------------

// ORIG T 0x82e72fa _PowerWarAssaultCheckIn()
#include "W6_AssaultPlaceDepsB/_PowerWarAssaultCheckIn.inc"
{
    for (int i = 0; i <= 7; ++i) {
        // b28 BL6: P0——ORIG 无 now 槽往返（getCurSec 结果寄存器直传出参槽）；
        // 正向 Empty continue 岛形复现 ORIG 0x82e72fa test/jne+jmp 尾岛。
        if (m_assaulters[i].Empty())
            continue;
        CUser* u = m_assaulters[i].GetUser();
        u->SetPowerWarLastPlayTime(GlobalData::s_systemTime_.getCurSec());
    }
}

// ORIG T 0x82e7362 _CheckPowerWarInPlayer(...) 返回恒 1
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 186/187（p167 22/22 CLEAR）
//   残差：N104 EH 尾块相对序——ORIG 正常路径 PacketGuard D1 块落在 _Unwind_Resume
//   之前（`call _SendPacketToVill; lea -0x1c; call D1; jmp`），4.4 把 D1 移到
//   unwind 段之后；块内容逐条全等（ndiff 7 全为该 3 行块 + jmp 的 rotate）。
//   §6.2 三件套：p167 22/22 CLEAR、gap=1、块内容全等 ⇒ 语义完成，byte 层封顶。
#include "W6_AssaultPlaceDepsB/_CheckPowerWarInPlayer.inc"
                                           ENUM_POWER_WAR_WINNER_OR_LOSER winner,
                                           int& winCnt, int& loseCnt,
                                           int& aliveCnt)
{
    // b28 BM4 判词（jump-threading 跨版本负例族）：fresh .o 实残 1 条 = 块尾
    // PacketGuard 段前 4.4 多发 `jmp <T>` 中继（ORIG 4.1 直落）。助记符序列
    // 其余逐条对齐（含空块 `{}` 限定 u 作用域形），语义完成，byte 层封顶。
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        {
            CUser* u = m_assaulters[i].GetUser();
            if (u != 0 && u->isJoinPowerWar()) {
                if (winner == 1) {
                    if (u->getPowerSide() == side) {
                        if (m_assaulters[i].GetState() == 5)
                            aliveCnt++;
                        winCnt++;
                        m_assaulters[i].SetPowerWarWinner(true);
                        u->SetCurCharacSuperState(true);
                    } else {
                        loseCnt++;
                        m_assaulters[i].DieUser();
                        u->setPowerWarHP(100);              // 0x64
                    }
                } else {
                    if (u->getPowerSide() == side) {
                        loseCnt++;
                        m_assaulters[i].DieUser();
                        u->setPowerWarHP(100);
                    } else {
                        if (m_assaulters[i].GetState() == 5)
                            aliveCnt++;
                        winCnt++;
                        m_assaulters[i].SetPowerWarWinner(true);
                        u->SetCurCharacSuperState(true);
                    }
                }
                PacketGuard packet;
                _MakeChangeHp(&packet, i, u->getPowerWarHP() & 0xffff);
                _SendPacketToVill(packet);
            }
        }
    }
    return 1;
}

// ORIG T 0x82ec558 _SendStraightVictories(CUser*)
#include "W6_AssaultPlaceDepsB/_SendStraightVictories.inc"
{
    PacketGuard packet;
    unsigned short victories = 0;
    int threshold = G_CDataManager()->m_powerParameter.m_straightVictories;

    if (user != 0) {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0) {
            party->incStraightVictories();
            victories = party->getStraightVictories();
            if (threshold <= static_cast<int>(victories)) {
                packet.put_header(0, 0xe0);
                packet.put_byte(party->get_member_count());
                for (int i = 0; i <= 3; ++i) {
                    CUser* member = party->get_user(i);
                    // b28 BL6: guard-continue 岛形——ORIG 0x82ec61f je→nop 岛 +
                    // 体尾前向 jmp 统一尾（4.4 直落形被折叠，continue 岛复现）。
                    if (member == 0)
                        continue;
                    const char* name = member->getCurCharacName();
                    int len = static_cast<int>(strlen(name));
                    packet.put_int(len);
                    packet.put_str(name, len);
                }
                packet.put_int(victories & 0xffff);
                packet.put_byte(static_cast<int>(static_cast<signed char>(user->getPowerSide())));
                packet.finalize(true);
                G_GameWorld()->send_to_vill(7, packet);
            }
        } else {
            user->incStraightVictories();
            victories = user->getStraightVictories();
            if (threshold <= static_cast<int>(victories)) {
                packet.put_header(0, 0xe0);
                packet.put_byte(1);
                const char* name = user->getCurCharacName();
                int len = static_cast<int>(strlen(name));
                packet.put_int(len);
                packet.put_str(name, len);
                packet.put_int(victories & 0xffff);
                packet.put_byte(static_cast<int>(static_cast<signed char>(user->getPowerSide())));
                packet.finalize(true);
                G_GameWorld()->send_to_vill(7, packet);
            }
        }
    }
}

// ORIG T 0x82eceb2 _SendPowerWarPointToGuildServer(CUser*, ENUM_POWER_WAR_WINNER_OR_LOSER, int, int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 333/329（p167 36/36 CLEAR）
//   修法：①删独立 side 局部（P0：ORIG 直写 pkt.m_powerSide@-0x5f=pkt+0xa，
//   后续一律读该成员）；②删 playTime 具名局部（内联为 EndPvP 第三实参，
//   ORIG 经 %ebx 直传）；③winPoint/losePoint 改 int 乘法取低字节（消 4 条
//   movsbl 截断链，ORIG 0x82ed0xx 为 `imul; mov %al`）；④winAliveCnt 形参写回、
//   直写 pkt.m_characNo[idx]、P227 trace 临时链、u 函数头预清（ndiff 260→202）。
//   残差：N95/N82 槽序（4.4 按首次使用序给 u 最深 -0x34 vs ORIG -0x2c，且
//   winPoint/losePoint 与 u 的相对序相反）+ N86b（`losePoint += bonus` 4.4 恒
//   折内存 add，ORIG 为 movzbl/lea/mov 四条形）。
// [E02·R5 fresh 实测] verdict=DIFF 335/329（our 331→329）；p167 调用点差集
//   36=36 CLEAR（MISSING/EXTRA 仅 Packet_Guild_Change_Power_War_Point 与
//   cMyTrace 的 C1/C2 同址别名 = P93 良性）⇒ 语义缺口 0，残差纯形态（槽序）。
//   本轮修法⑤（P0/P52/P134）：删具名 `CGuildServerProxy* proxy` 局部，改链式
//   `GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
//    ->SendPowerWarPoint(pkt);` —— ORIG 0x82ed2xx 为 `call GetServerProxy;
//   lea -0x69(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call
//   SendPowerWarPoint`（结果 %eax 直通 receiver 实参，无栈往返）；我方旧形多
//   `mov %eax,-0x20(%ebp)` + 回读一对（−2 insn）。
//   反劣实验记录（勿重试）：删 cMyTrace 实参位 `static_cast<int>(winPoint)` /
//   `static_cast<int>(losePoint)` 显式提升 ⇒ our 329→331（+2）；此处 cast 是
//   uchar→int 提升必需形（ORIG `movzbl -0x3d,%eax; movzbl %al,%edi`），与
//   C28L0「vararg 冗余 cast 反劣」方向相反，非冗余。
// [E11·语义修复 + codegen] verdict=DIFF 335/335（改前 335/329，gap −6→0）；
//   语义：ORIG 的配对是 m_losePoint(+0x2c) ← multiple×(winCnt−winAliveCnt)
//   （0x082ed036 段，日志行 0xf62；败者分支 0x082ed25f 读取）、m_winPoint(+0x2b)
//   ← multiple×loseCnt（0x082ed0b4 段，日志行 0xf6a；胜者分支 0x082ed1c4 读取，
//   且 _VictoryBonus 结果加于其上 0x082ed0e1）。改前重建把两式互换 ⇒ 发给胜者/
//   败者的积分值与包内 +0x2b/+0x2c 内容均与 ORIG 相反（真 bug，由整函数逐条对齐
//   反证）。字段锁定：ORIG ctor 0x082f070c = PacketHeader(this,0x6a4,0x2d) 且
//   movb $0 写 +0xa/+0x2b/+0x2c ⇒ 对象 0x2d 字节、栈 [-0x69,-0x3d] ⇒ -0x3e/-0x3d
//   即成员（无独立局部）。同刻删两个具名积分局部（P0/P183）⇒ our 329→335。
//   残差：仅败者分支 `pkt.m_characNo[loserIdx + 4]` 的 lea 时机（ORIG 先提升
//   loserIdx 入 %ebx、call 后 `lea 0x4(%ebx),%edx`；4.4 提前 lea 入 %ebx）⇒
//   P226/P331 书写序族，全函数唯一 1 处。
#include "W6_AssaultPlaceDepsB/_SendPowerWarPointToGuildServer.inc"
                                                    ENUM_POWER_WAR_WINNER_OR_LOSER winner,
                                                    int winCnt, int loseCnt,
                                                    int winAliveCnt)
{
    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ecebe 函数最头 `movl $0x0,-0x2c`
    //   预清 u 槽（在 user==0 检查之前）。
    CUser* u = 0;
    if (user == 0) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf4c, "Invalid User pointer income (%d, %d, %d, %d)",
            winner, winCnt, loseCnt, winAliveCnt);
        return;
    }

    Packet_Guild_Change_Power_War_Point pkt;     // ORIG ctor W 0x082f070c
    // [b28d-solo-W6_AssaultPlaceDepsB] P0：ORIG 无独立 side 局部——直接写
    //   pkt.m_powerSide（-0x5f = pkt 基址 -0x69 + 0xa），后续一律读该成员。
    if (winner == 1) {
        pkt.m_powerSide = static_cast<char>(user->getPowerSide());
    } else {
        pkt.m_powerSide = (user->getPowerSide() == 1) ? 2 : 1;
    }

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ecfa1 直接把 winCnt 写回形参
    //   winAliveCnt 槽（`mov 0x14(%ebp),%eax; mov %eax,0x1c(%ebp)`）——无 aliveCnt
    //   具名局部（我们多一个 -0x28 槽使帧 +0x24、ndiff 24）。
    if (winCnt - winAliveCnt < 0) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf5c, "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d )",
            winCnt, winAliveCnt);
        winAliveCnt = winCnt;                    // ORIG: param_6 = param_4
    }

    // [E11·语义修正] ORIG 的「公式 ↔ 字段/用途」配对与此前重建相反（真 bug）：
    //   m_losePoint(+0x2c) ← multiple × (winCnt - winAliveCnt)（0x082ed036 段：
    //     `mov 0xa67c(%eax),%eax; mov 0x14(%ebp),%ecx; sub 0x1c(%ebp),%ecx;
    //      imul; mov %al,-0x3d`，日志行 0xf62），败者分支
    //     （0x082ed25f `movzbl -0x3d(%ebp),%eax`）读取；
    //   m_winPoint(+0x2b) ← multiple × loseCnt（0x082ed0b4 段：`imul 0x18(%ebp),%eax;
    //     mov %al,-0x3e`，日志行 0xf6a），胜者分支（0x082ed1c4 `movzbl -0x3e`）读取，
    //     且 _VictoryBonus 结果经 `movzbl -0x3e,%edx; lea (%edx,%eax,1); mov %al,-0x3e`
    //     （0x082ed0d7..0x082ed0e1）加在其上。
    //   字段锁定：ORIG ctor 0x082f070c = PacketHeader(this,0x6a4,0x2d) + movb $0 写
    //     +0xa/+0x2b/+0x2c ⇒ 对象 0x2d 字节，栈 [-0x69,-0x3d] ⇒ pkt+0x2b=-0x3e、
    //     pkt+0x2c=-0x3d；无独立积分局部，故直接写成员。
    //   P48/P183：ORIG 每次读取 multiplicand 均重调 G_CDataManager 并读
    //     m_powerParameter(+0xa67c)（六点），禁具名缓存。
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - winAliveCnt) > 0xff) {              // imul; cmp $0xff; setg
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf62,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d)",
            winCnt, winAliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        pkt.m_losePoint = 0xff;                      // ORIG 0x082ed016 movb $0xff,-0x3d
    } else {
        // ORIG 0x082ed036：int 乘法后取低字节（低 8 位等价，无 movsbl 截断链）。
        pkt.m_losePoint = static_cast<unsigned char>(
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - winAliveCnt));
    }
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt > 0xff) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf6a,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d )",
            winCnt, winAliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        pkt.m_winPoint = 0xff;                   // ORIG 0x082ed09d movb $0xff,-0x3e
    } else {
        pkt.m_winPoint = static_cast<unsigned char>(
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt);
    }

    int bonus = _VictoryBonus(user, pkt.m_powerSide);
    // [b28d-L28 round2 · P322② 实测负例] ORIG 0x82ed0d7 段为读-加-写四条形
    // （`movzbl -0x3e,%edx; mov -0x28,%eax; lea (%edx,%eax,1),%eax; mov %al,-0x3e`），
    // 但加 `volatile` 后全部使用点（5 处读 + pkt 写入）都强制重载 ⇒ ourN 331→339
    // （gap +4→−4、块 5→11）反劣，已回退。P322② 仅适用于「该累加器无其它使用点」场景。
    pkt.m_winPoint = pkt.m_winPoint + bonus;

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ed235 直接写 pkt.m_characNo[idx]
    //   （`mov %eax,-0x5e(%ebp,%ebx,4)` = pkt 基址 -0x69 + 0xb）——无 winnerNo/
    //   loserNo 局部数组、无末尾回填循环（我们此前多 8 清零 + 拷贝段）。
    int winnerIdx = 0;
    int loserIdx = 0;
    for (int i = 0; i <= 7; ++i) {
        // P223：ORIG `test %al,%al; jne <skip>` 正极性 guard-continue。
        if (m_assaulters[i].Empty())
            continue;
        u = m_assaulters[i].GetUser();
        if (u == 0)
            continue;
        // P0/P227：ORIG 无 now/hist/winFlag/playTime 具名局部（值经 %ebx/%eax
        //   直传）；playTime 内联为 EndPvP 第三实参（右起求值先算）。
        u->m_historyLog.EndPvP(
            (ENUM_PVP_HISTORY_REASON)1 /*ORIG 字面 1*/,
            static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide))
                == static_cast<int>(static_cast<signed char>(u->getPowerSide())),
            static_cast<unsigned int>(
                GlobalData::s_systemTime_.getCurSec() - m_abuseSec));  // +0x13c
        u->IncPowerWarAssaultCount();
        if (static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide))
            == static_cast<int>(static_cast<signed char>(u->getPowerSide()))) {
            u->IncPowerWarAssaultVictoryCount();
            u->IncreasePowerWarPoint(static_cast<short>(pkt.m_winPoint));
            if (winnerIdx > 3) {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0xf94,
                    "Power War Point Computer Error( winner index : %d, loser index : %d )",
                    winnerIdx, loserIdx);
            } else {
                pkt.m_characNo[winnerIdx] =
                    static_cast<unsigned int>(u->get_charac_no(-1));
                winnerIdx++;
                _SendPowerWarAssaultResultToUser(u, pkt.m_winPoint);
            }
        } else {
            u->IncreasePowerWarPoint(static_cast<short>(pkt.m_losePoint));
            if (loserIdx > 3) {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0xfa4,
                    "Power War Point Computer Error( winner index : %d, loser index : %d )",
                    winnerIdx, loserIdx);
            } else {
                pkt.m_characNo[loserIdx + 4] =
                    static_cast<unsigned int>(u->get_charac_no(-1));
                loserIdx++;
                _SendPowerWarAssaultResultToUser(u, pkt.m_losePoint);
            }
        }
    }

    // P227 全表达式临时链：ORIG 0x82ed30e 在 ctor 前把 6 个 vararg 求值入
    //   %edi/%esi/%ebx 跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0xfad, 0)(
        "win side(%d), win pp(%d, %d), lose pp(%d, %d), winAliveCnt(%d)",
        static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide)), winCnt,
        // [E02·R5 实验已回退] 删 (int) cast 实测反劣（our 329→331，+2 条）：
        //   ORIG `movzbl -0x3d,%eax; movzbl %al,%edi` 的提升直入形由**显式**
        //   窄化 cast 链复现（与 C28L0 的「vararg 冗余 cast 反劣」方向相反，
        //   因本处 cast 是 uchar→int 提升必需形，非冗余）。
        static_cast<int>(pkt.m_winPoint), loseCnt,
        static_cast<int>(pkt.m_losePoint), winAliveCnt);

    // [E02·R5 修·P0/P52/P134] 删具名 proxy 局部：ORIG 0x82ed2xx 段为
    //   `call GetServerProxy; lea -0x69(%ebp),%edx; mov %edx,0x4(%esp);
    //    mov %eax,(%esp); call SendPowerWarPoint`（结果 %eax 直通 receiver，
    //   无槽往返）。我方具名局部多 `mov %eax,-0x20(%ebp)` + 回读一对。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
        ->SendPowerWarPoint(pkt);

    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        pkt.m_powerSide, static_cast<int>(pkt.m_winPoint));
    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        (pkt.m_powerSide == 1) ? 2 : 1, static_cast<int>(pkt.m_losePoint));
}

// ORIG T 0x82ec558/0x82eceb2 直读补充：_SendStraightVictories 末尾 endp。
// （注：上面的 EndPvP reason 参数按 ORIG mangled 为字面 1（0x082ed187 前 mov $1）。）

// ---------------------------------------------------------------------------
// TimeoutCompleteLoad
// ---------------------------------------------------------------------------

// ORIG T 0x82ea31a TimeoutCompleteLoad()
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 262/262（p167 39/39 CLEAR）
//   修法：continue 形、P227 cMyTrace 全表达式临时链、删 u!=0 过度防御（P171）、
//   第二段循环底部条件（i<=7 + 循环后 DoConnectP2P）。残差仅 .o 重定位（AE）。
// [b28c-L7][SIGFIX:pvp_assault::CAssaultPlace::TimeoutCompleteLoad] void→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...TimeoutCompleteLoad()"，tsv 第 15 行；
// ORIG 两出口分别落 1 / 0，此前 void 化把返回值整体丢弃 = 建模缺失）。
#include "W6_AssaultPlaceDepsB/TimeoutCompleteLoad.inc"
{
    if (m_state != 2)
        throw (ENUM_ERROR)0x13;
    for (int i = 0; i <= 7; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsB] P223/P292：ORIG 0x82ea3xx 对 Empty 用
        //   `test %al,%al; jne <continue>`（正极性），`!Empty()` 在 4.4 恒多 xor $1。
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetState() != 2) {
            if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                GlobalData::s_power_manager->GetPowerWarLog()->IncLoadingTimeOutCountSecede();
                // [b28d-solo-W6_AssaultPlaceDepsB] P227 全表达式临时链：ORIG 在
                //   cMyTrace ctor 前求 vararg 并以 %ebx 跨 ctor 保活。
                cMyTrace(__PRETTY_FUNCTION__, 0xa18, 0)(
                    "POWER WAR LOADING SECEDE");
                for (int j = 0; j <= 7; ++j) {
                    if (m_assaulters[j].Empty())
                        continue;
                    // ORIG 蓝/红两分支各调一次 GetUser（P48 静态调用点各一）
                    if (m_assaulters[j].GetTeam() == 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0xa20, 0)(
                            "BLUE TEAM : %s",
                            m_assaulters[j].GetUser()->getCurCharacName());
                    } else {
                        cMyTrace(__PRETTY_FUNCTION__, 0xa22, 0)(
                            "RED TEAM : %s",
                            m_assaulters[j].GetUser()->getCurCharacName());
                    }
                }
                // ORIG 直接把 GetUser(i) 指针作为 %s 实参传给 trace（保留原样）。
                cMyTrace(__PRETTY_FUNCTION__, 0xa25, 0)(
                    "SECEDE : %s",
                    reinterpret_cast<const char*>(m_assaulters[i].GetUser()));
            }
            CUser* u = m_assaulters[i].GetUser();
            // P0：ORIG 0x82ea56x 短路汇聚值直测（无 pwVill 槽）；ORIG 无 u!=0
            // 判空（P171 过度防御禁止——直接解引用 u）。
            if (GlobalData::s_power_manager->IsPowerWarEventOn()
                && u->getCurCharacVill() == 7) {
                PacketGuard packet;
                packet.put_header(0, 0xe6);
                packet.put_byte(1);
                packet.finalize(true);
                u->Send(packet);
            }
        }
    }
    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ea63b：底部条件循环
    //   （`i=0; jmp cond; body; ++i; cmpl $7; setle; jne body`），i>7 分支在
    //   循环之后——写 `for(;;){ if(i>7){...return true;} }` 4.4 恒条件置顶。
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetState() != 2) {
            LeaveAssaultPlace(i, true, false);
            if (m_state == 6)
                return false;                    // ORIG 提前返回 0
        }
    }
    DoConnectP2P();
    // ORIG 直读 m_parties[i].m_field00（空判定 +0x12c/0x130 指针），不走 Get()
    if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x03');
    if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x03');
    return true;                                 // ORIG mov $0x1,%eax
}


// ---------------------------------------------------------------------------
// ChangeHp
// ---------------------------------------------------------------------------

// ORIG T 0x82ed752 ChangeHp(CUser*, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 155/155（p167 19/19 CLEAR，槽位集全等）
//   修法：删 pwVill 具名局部（内联短路直测）、party 判断改正极性、循环 continue 形、
//   m_parties[].Get()→m_field00 直读。残差仅 .o 重定位显示（AE 口径）。
// [b21 N·P198-②/P197-④] 语义已闭合：`pwVill = IsPowerWarEventOn() && getCurCharacVill()==7`
//   短路判定 ORIG(4.1) 直测 `test %al,%al; je`（无槽），4.4-O0 恒把中间结果
//   `mov %al,-off; cmpb` 物化（P197-④ 源形不可择）；末段 allDead 循环 `GetState()!=8`
//   反亦 `setne` 物化 vs ORIG `cmp $8; jne`；循环回边 `jmp;nop` 合并差（N82）。逐调用
//   逐分支语义逐一相等（_GetUserIndex/m_assaulters[idx].ChangeHp/_MakeChangeHp/Send 三叉/
//   CloseAssault 段全对齐），封 N。
#include "W6_AssaultPlaceDepsB/ChangeHp.inc"
{
    int idx = _GetUserIndex(user, true);
    // P198-②：ORIG 对 CAssaulter::ChangeHp 返回 `test %al,%al; je` 寄存器直测（无槽）；
    // `bool changed` 具名会额外 `mov %al,-off; cmpb $0,-off`，内联直测对齐。
    if (m_assaulters[idx].ChangeHp(hp)) {
        PacketGuard packet;
        _MakeChangeHp(&packet, idx, hp);
        // [b28d-solo-W6_AssaultPlaceDepsB] P0：ORIG 0x82ed7b6 短路汇聚值直测
        //   `test %al,%al; je`（无槽）——具名 bool 局部会多 store+cmpb 两条。
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            && user->getCurCharacVill() == 7) {
            CParty* party = static_cast<CParty*>(user->GetParty());
            // P59 正极性：ORIG `cmpl $0; je <Send 块>` ⇒ party!=0 走 fall-through。
            if (party != 0)
                party->send_to_party(packet);
            else
                user->Send(packet);
        } else {
            _SendPacketToArea(packet);
        }
    }
    if (m_state == 5 || m_state == 6) {
        m_assaulters[idx].SetState(static_cast<ENUM_ASSAULT_STATE>(8));
        bool allDead = true;
        for (int i = 0; i <= 7; ++i) {
            // P223：ORIG `test %al,%al; jne <nop 岛>` 正极性 guard-continue。
            if (m_assaulters[i].Empty())
                continue;
            if (m_assaulters[i].GetState() != 8)
                allDead = false;
        }
        if (allDead) {
            // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ed92a 直读 m_field00
            // （`mov 0x12c(%eax),%eax`，无 Get() call）。
            if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
                (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\a');
            if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
                (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\a');
            m_state = (ENUM_ASSAULT_STATE)7;
            CloseAssault();
        }
    }
}

// ---------------------------------------------------------------------------
// DieUser 双版本
// ---------------------------------------------------------------------------

// ORIG T 0x82ebda6 DieUser(int, int, MSG_DIE_ASSAULT_PLAYER&)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 528/529（p167 61/61 CLEAR）
//   残差：N82 帧槽（0xdc vs 0xcc，killerName -0x45 vs ORIG -0x61）+ P63 jmp/nop
//   中继；§6.2 三件套（gap=1 + 调用点差集归零 + 槽位集对齐）判语义完成。
//   本轮复核：ORIG 0x82ebda6 函数头有 killer(-0x2c)/victim(-0x28) 双预清 +
//   killerName(-0x61) 清零；三形实测（头部双预清 390 / 仅 killer 391 / 原位
//   347）以原位最优 ⇒ 保持原位，槽序差归 N82。
// §6.2 巨件核销（b25）：P167 修前 61/66 [多4 PartyInfo::Get 多1 memset]→
// 修后 61/61 [0缺0多]；gap 15→-2（orig 531 / our 533）；修法：①killerName
// memset 改 `= {0}` 初始化形（复现 ORIG 字节/字/双字对齐手工清零展开，
// N-78b 回扫）；②m_parties[i].Get() 改 m_parties[i].m_field00 直读（ORIG
// 内联解引用无 call）；③商比较加 (int) 复现 ORIG divl+jle 有符号比较
// （P196 无符号反）；④hack 命名局部删除（ORIG getHackAnalyzer 返回值
// 直传入参无槽）；⑤pw bool 局部删除（ORIG && 汇聚条件内联无槽）；
// ⑥side/winnerType 改 int 槽（ORIG movsbl 物化后 dword 存）。残差归因
// N82 帧槽（0xdc vs 0xcc）+ P63 jmp/nop 中继（详见
// docs/asm_translation_patterns.md 负例表）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①prett2/修正判据缺口 0、
//   strall 串差 0、p167 CLEAR(61=61)；②本 TU 已路由 4.4.6 全量（C6446R），本波
//   反向实测 4.4.7 全量变体 **劣化**（6/40/0/5/12 → 6/39/1/5/12，ae−1/near+1）
//   ⇒ 路由已最优、无反向旋钮；③本波复核 531/533 gap=-2：帧 0xcc vs 0xdc（我方大
//   0x10）+ killerName 槽（ORIG -0x61 vs 我方 -0x45）+ `movl $0x0,-0x2c` 预清位置
//   （ORIG 头部双预清 0x10 之外，三形历史实测以原位最优 ndiff 347）⇒ 维持
//   （新=DIFF 531/533 gap=-2, blocks=58）。
#include "W6_AssaultPlaceDepsB/DieUser.inc"
{
    char killerName[0x15] = { 0 };               // ORIG 0x15 字节初始化清零展开（非 memset call）

    if (m_state != 4)
        throw (ENUM_ERROR)0x13;

    CUser* victim = m_assaulters[victimIdx].GetUser();
    if (m_assaulters[victimIdx].GetState() != 5)
        throw (ENUM_ERROR)0x12;

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ebda6 在函数头预清 killer/victim
    //   两槽（-0x2c/-0x28）；实测三形（头部双预清 ndiff 390 / 仅 killer 391 /
    //   原位 347）以原位最优 ⇒ 保持原位（槽序差异归 N82）。
    CUser* killer = 0;
    if (killerIdx >= 0 && killerIdx <= 7) {
        killer = m_assaulters[killerIdx].GetUser();
        if (m_assaulters[killerIdx].GetState() != 5) {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xd37, 0);
            trace("CAssaultPlace::DieUser => killer is already died!");
        }
        strncpy(killerName, m_assaulters[killerIdx].GetCharacName(), 0x14);
        if (killer != 0 && msg.m_damage != 0) {
            if (msg.m_damage == 1) {
                killer->getHackAnalyzer()->addServerHackCnt(
                    killer, static_cast<WongWork::ENUM_HACKTYPE>(0x7d), 1, 0, 0);
            }
            if ((int)(msg.m_attackCount / msg.m_damage) > 100000) {  // ORIG divl + jle（有符号比较，P196）
                killer->getHackAnalyzer()->addServerHackCnt(
                    killer, static_cast<WongWork::ENUM_HACKTYPE>(0x7e), 1, 0, 0);
            }
        }
    } else {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd54, 0);
        trace("CAssaultPlace::DieUser invalid killer index income(%d,%d)", victimIdx, killerIdx);
    }

    if (victim != 0) {
        cUserHistoryLog* hist =
            &victim->m_historyLog;                             // +0x79700
        if (victim->getCurCharacVill() == 7
            && GlobalData::s_power_manager->IsPowerWarEventOn()) {
            hist->DiePvP((ENUM_PVP_HISTORY_REASON)2,
                         msg.m_damage, msg.m_attackCount,
                         (int)(short)msg.m_field08, (int)(short)msg.m_field0a,
                         (int)(short)msg.m_field0c, (int)(short)msg.m_field0e,
                         killerName);
        } else {
            hist->DiePvP((ENUM_PVP_HISTORY_REASON)6,
                         msg.m_damage, msg.m_attackCount,
                         (int)(short)msg.m_field08, (int)(short)msg.m_field0a,
                         (int)(short)msg.m_field0c, (int)(short)msg.m_field0e,
                         killerName);
        }
    }

    if (_IsChangeDieState())
        _ChangeDieState(victimIdx);

    if (victim != 0
        && !(victim->getCurCharacVill() == 7
             && GlobalData::s_power_manager->IsPowerWarEventOn())) {
        PacketGuard packet;
        _MakeUserState(&packet, victim);
        G_GameWorld()->send_all(packet);
    }

    if (killer != 0
        && !(killer->getCurCharacVill() == 7
             && GlobalData::s_power_manager->IsPowerWarEventOn())) {
        if (_IsProcsssChaos())
            _ProcessChaos(victimIdx, killerIdx);
    }

    if (!_CheckAssaultable())
        return;

    if (victim != 0 && killer != 0
        && victim->getCurCharacVill() == 7
        && GlobalData::s_power_manager->IsPowerWarEventOn()) {
        int winCnt = 0, loseCnt = 0, aliveCnt = 0;
        int side = 0;          // ORIG local_28 int 槽（movsbl 物化后 dword 存）
        int winnerType = 0;
        if (killerIdx >= 0 && killerIdx <= 7) {
            side = (int)(signed char)killer->getPowerSide();
            winnerType = 1;
        } else {
            side = (int)(signed char)victim->getPowerSide();
            winnerType = 2;
        }
        if (_CheckPowerWarInPlayer((ENUM_POWER_SIDE_TYPE)side,
                                   (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                   winCnt, loseCnt, aliveCnt)) {
            if (winnerType == 1)
                _SendPowerWarPointToGuildServer(killer,
                                                (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                                winCnt, loseCnt, aliveCnt);
            else
                _SendPowerWarPointToGuildServer(victim,
                                                (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                                winCnt, loseCnt, aliveCnt);
            PacketGuard packet;
            _MakeUserState(&packet);
            G_GameWorld()->send_all(packet);
            _SendStraightVictories(killer);
        }
    }

    if (victim != 0
        && victim->getCurCharacVill() == 7
        && GlobalData::s_power_manager->IsPowerWarEventOn()) {
        CParty* party = static_cast<CParty*>(victim->GetParty());
        if (party != 0)
            party->resetStraightVictories();
        else
            victim->resetStraightVictories();
    }

    m_state = (ENUM_ASSAULT_STATE)5;
    // ORIG 直读 m_parties[i].m_field00（+0x12c/+0x130），无 Get() call
    if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x05');
    if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x05');
    EndAssault();
}

// ORIG T 0x82ed3fe DieUser(CUser*, int, MSG_DIE_ASSAULT_PLAYER&)
#include "W6_AssaultPlaceDepsB/DieUser_1.inc"
{
    // b28 BL6: P0——ORIG 切片无 idx 槽往返，返回值寄存器直传出参槽；
    // 具名局部引入 -0xc 双 store/load +帧差。嵌套实参复现 ORIG 0x82ed3fe 形态。
    DieUser(_GetUserIndex(user, true), killerIdx, msg);
}

// ---------------------------------------------------------------------------
// MakeTeam / 组队操作组
// ---------------------------------------------------------------------------

// ORIG T 0x82e7d0a MakeTeam(CUser*, CUser*, int&)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 218/217（p167 20/20 CLEAR）
//   修法：①P226 裸语句表达式复现 get_posX 的 %ebx 跨调用保活；②真 bug——then
//   （posX1<posX2）应为 `_SeparateTeam(user1,user2)`+m_winTeam=0，原写 then/else
//   实参序颠倒；③P212 槽序——i 提至函数头并声明在 team 计数前、team0Count 在
//   team1Count 前，复现 ORIG names(-0x4c..-0x30)>i(-0x2c)>team0(-0x28)>team1(-0x24)
//   （ndiff 153→51）。
//   残差：N107（`&u->m_historyLog` 取址 4.4 恒 add vs ORIG lea；六形探针全败）
//   + N82附（StartPvP 9 实参的 callee-saved/栈临时分配对调）。
#include "W6_AssaultPlaceDepsB/MakeTeam.inc"
{
    // param_2 存 +0x134、param_3 存 +0x138——与既有头字段一一对应。
    m_pos.m_x = user1->getCurCharacVill();
    m_pos.m_y = user1->get_area(false);
    if (m_pos.m_x == 0 && m_pos.m_y == 0)
        throw (ENUM_ERROR)0x15;
    m_checkUser = user1;                                        // +0x134
    m_field138 = user2;                                         // +0x138

    // posX 比较：ORIG 首 get_posX 结果 `mov %eax,%ebx` 跨第二 call 存寄存器
    // （P197② 语句表达式）；随后 _SeparateTeam 两参顺序随比较翻转。
    // [b28d-solo-W6_AssaultPlaceDepsB] 真 bug 修正：ORIG then（posX1<posX2，
    //   fall-through）为 `_SeparateTeam(user1,user2)` + m_winTeam=0，else 为
    //   `_SeparateTeam(user2,user1)` + m_winTeam=1；原写 then/else 实参序颠倒。
    // [b28d-solo-W6_AssaultPlaceDepsB] P226：裸语句表达式（内无具名变量）才令
    //   值升格 callee-saved；带具名 r 会槽化（多 mov+movzwl 两条）。
    if (({ user1->get_posX(); }) < user2->get_posX()) {
        _SeparateTeam(user1, user2);
        m_winTeam = (ENUM_ASSAULT_TEAM)0;                       // +0x124
    } else {
        _SeparateTeam(user2, user1);
        m_winTeam = (ENUM_ASSAULT_TEAM)1;                       // +0x124
    }

    m_authenValue = GlobalData::s_systemTime_.getCurSec();       // +0x108 = getCurSec
    _SetReadyState();

    if (m_parties[0].m_field00 != 0)                            // +0x12c
        m_parties[0].m_field00->SetAssaultState('\x02');
    if (m_parties[1].m_field00 != 0)                            // +0x130
        m_parties[1].m_field00->SetAssaultState('\x02');

    // 8 个名字槽（.rodata 空串 DAT_08c1bca0 直存），team0 填 [0..]，team1 填 [4..]。
    // [b28d-solo-W6_AssaultPlaceDepsB] P212：ORIG 槽深序 names(-0x4c..-0x30) >
    //   i(-0x2c) > team1Count(-0x28) > team0Count(-0x24) ⇒ i 提至函数头并声明在
    //   team 计数之前（两个循环共用同一 i）。
    const char* names[8] = { "", "", "", "", "", "", "", "" };
    int i;
    int team0Count = 0;                          // ORIG -0x28
    int team1Count = 0;                          // ORIG -0x24
    for (i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetTeam() == 0) {
            names[team0Count] = m_assaulters[i].GetCharacName();
            team0Count++;
        } else if (m_assaulters[i].GetTeam() == 1) {
            names[team1Count + 4] = m_assaulters[i].GetCharacName();
            team1Count++;
        }
    }
    for (i = 0; i <= 7; ++i) {
        CUser* u = m_assaulters[i].GetUser();
        if (u != 0) {
            int reason;                                          // ORIG sete/test 形
            if (u->getCurCharacVill() == 7)
                reason = 0;
            else
                reason = 4;
            u->m_historyLog.StartPvP((ENUM_PVP_HISTORY_REASON)reason,
                                     names[0], names[1], names[2], names[3],
                                     names[4], names[5], names[6], names[7]);
        }
    }
}

// ORIG T 0x82e71c4 _GetTeamCountAliveUser(int*)
#include "W6_AssaultPlaceDepsB/_GetTeamCountAliveUser.inc"
{
    for (int i = 0; i <= 7; ++i) {
        CUser* u = m_assaulters[i].GetUser();
        if (u == 0)
            continue;
        if (u->IsCurCharacGhost())
            continue;
        counts[m_assaulters[i].GetTeam()]++;
    }
}

// ORIG T 0x82e7018 _AddUser(CUser*, ENUM_ASSAULT_TEAM)
#include "W6_AssaultPlaceDepsB/_AddUser.inc"
{
    // [b28d-26] ORIG 0x82e7018：`je <循环增量>` 跳过主体、主体以 ret 早返，
    //   throw 块在循环尾之后（无循环内 slot==7 判定）；SetAssaultPlace 实参直读
    //   this+0x104（无 GetAssaultPlaceId call）。
    for (int slot = 0; slot <= 7; ++slot) {
        if (m_assaulters[slot].IsAttachAble())
        {
            m_assaulters[slot].AttachUser(user, team);
            user->SetAssaultPlace(m_field104);   // ORIG 直读 +0x104 → CUserCharacInfo::SetAssaultPlace
            m_field100++;                        // GetAssaulterCount 计数域
            return;
        }
    }
    throw (ENUM_ERROR)0x04;                      // ORIG __cxa_throw ENUM_ERROR(4)
}

// ORIG T 0x82e70c8 _AddTeam(CUser*, ENUM_ASSAULT_TEAM)
#include "W6_AssaultPlaceDepsB/_AddTeam.inc"
{
    CParty* party = 0;
    if (user->CheckInParty())
        party = static_cast<CParty*>(user->GetParty());
    // b28 BL6: P59 双向布局——ORIG 0x82e70f2 `cmpl $0; je <else体后置>` 要求
    // 正极性 if(party!=0){for…}else{_AddUser}，体尾前向 jmp 共享出口。
    if (party != 0) {
        for (int i = 0; i <= 3; ++i) {           // cmpl $0x3 … setle
            CUser* member = party->get_user(i);
            if (member != 0)
                _AddUser(member, team);
        }
    } else {
        _AddUser(user, team);
    }
}

// ORIG T 0x82e782a _MoveUser(pvp_assault::CDisposition*, int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 70/70（p167 7/7 CLEAR，446 后端）
//   修法：continue 形复现 nop 岛。
//   [b28d-solo-W6_AssaultPlaceDepsB] 真 bug 修正：ORIG fm 基址 -0x34（GetTeam→
//   -0x34/+4=-0x30/GetUser=-0x2c 三证），param_2→-0x28=+0xc=m_destVill、
//   param_3→-0x24=+0x10=m_destArea；调用点 W6_AssaultPlaceDepsA.cpp:809 传
//   (m_pos.m_x=vill, m_pos.m_y=area) 与 ForcedMove 的 m_destVill/m_destArea 语义
//   自洽。原写 m_destArea/m_x 整体错位一格（m_destVill 未初始化 + 覆盖矩阵 x）。
//   残差：N7（间接 call 目标 4.4.7 恒 %eax，ORIG 用 %edx）。
#include "W6_AssaultPlaceDepsB/_MoveUser.inc"
{
    // [b28d-solo-W6_AssaultPlaceDepsB] P0 槽-局部律修正：ORIG -0x18 不是独立
    //   dir 局部，而是 fm.m_dir（fm 基址 -0x34 + 0x1c = -0x18；fm 占
    //   -0x34..-0x15，与独立 dir 槽重叠在物理上不可能）⇒ 原码写
    //   `fm.m_dir = GetDirectionByTeam(...)`。据此槽序自洽：
    //   fm(-0x34..-0x15) > counts(-0x14/-0x10) > i(-0xc)。
    int counts[2] = {0};
    for (int i = 0; i <= 7; ++i) {
        // P223：ORIG `test %al,%al; jne <+0xd8 nop 岛>` + 体尾 `jmp <addl>; nop`
        //   ——正极性 guard-continue 岛形（if(!Empty()){...} 反产 xor+je 无岛）。
        if (m_assaulters[i].Empty())
            continue;
        CForcedMoveCharacter fm;                 // 栈上结构，偏移对应见头
        fm.m_field00 = static_cast<int>(m_assaulters[i].GetTeam());
        fm.m_slotIdx = counts[static_cast<int>(m_assaulters[i].GetTeam())];
        fm.m_user    = m_assaulters[i].GetUser();
        // asm：param_2→+0x0c（m_destVill）、param_3→+0x10（m_destArea）。
        fm.m_destVill = x;
        fm.m_destArea = y;
        fm.m_dir = pvp_assault::GetDirectionByTeam(
            static_cast<ENUM_ASSAULT_TEAM>(m_assaulters[i].GetTeam()));
        disp->MoveUser(&fm);
        counts[static_cast<int>(m_assaulters[i].GetTeam())]++;
    }
}

}  // namespace pvp_assault
