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
void CAssaultPlace::_MakeCompleteLoad(PacketGuard* p, int idx)
{
    p->put_header(0, 0x81);
    // ORIG 无 uid 局部(P0 右值直传): GetUserUniqueId 结果 movzwl 直入 put_short
    p->put_short(m_assaulters[idx].GetUserUniqueId() & 0xffff);
    p->finalize(true);
}

// ORIG T 0x82e886a _MakeStartAssault(PacketGuard*)
void CAssaultPlace::_MakeStartAssault(PacketGuard* p)
{
    p->put_header(0, 0x82);
    p->finalize(true);
}

// ORIG T 0x82e891e _MakeDieAssaultPlayer(PacketGuard*, int)
void CAssaultPlace::_MakeDieAssaultPlayer(PacketGuard* p, int idx)
{
    p->put_header(0, 0x83);
    p->put_short(m_assaulters[idx].GetUserUniqueId() & 0xffff);
    p->finalize(true);
}

// ORIG T 0x82e8978 _MakeRevivalAssaultPlayer(PacketGuard*, int)（ORIG 未用 this）
void CAssaultPlace::_MakeRevivalAssaultPlayer(PacketGuard* p, int idx)
{
    p->put_header(0, 0x85);
    p->put_short(idx);
    p->finalize(true);
}

// ORIG T 0x82e88a0 ... 见 A 组 _MakeEndAssault —— 此处不重复。
// ORIG T 0x82e852e _MakeMoveAssaultPlace(PacketGuard*)
void CAssaultPlace::_MakeMoveAssaultPlace(PacketGuard* p)
{
    p->put_header(0, 0x80);
    p->put_int(GetAssaultPlaceId());            // +0x104
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

    // 第一段：场内成员列表
    int countIdx = p->get_index();              // PacketGuard 局部 idx 补丁槽
    int count = 0;
    p->put_byte(count);
    for (int i = 0; i <= 7; ++i) {              // cmpl $0x7 … setbe（无符号 ≤7）
        if (!m_assaulters[i].Empty()) {
            p->put_byte(i);
            p->put_byte(m_assaulters[i].GetTeam() + 1);   // lea 0x1(%eax)
            unsigned short uid = m_assaulters[i].GetUserUniqueId();
            p->put_short(uid & 0xffff);
            count++;
        }
    }
    p->put_byte(countIdx, count);

    // 第二段：同 vill/area 在线用户 id 列表（上限 10）
    int listIdx = p->get_index();
    int listCount = 0;
    p->put_byte(listCount);
    std::vector<unsigned short> idList;
    G_GameWorld()->get_user_id_list(m_pos.m_x, m_pos.m_y, idList);
    for (int i = 0; i < 10 && i < static_cast<int>(idList.size()); ++i) {
        p->put_short(idList[i]);
        listCount++;
    }
    p->put_byte(listIdx, listCount);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// 发送组
// ---------------------------------------------------------------------------

// ORIG T 0x82e6d92 _SendPacketToVill(PacketGuard&)
void CAssaultPlace::_SendPacketToVill(PacketGuard& packet)
{
    if (m_pos.m_x != 0)                         // +0x118 vill id 非零才发
        G_GameWorld()->send_to_vill(m_pos.m_x, packet);
}

// ORIG T 0x82e7986 _SendUserMiniInfo()
void CAssaultPlace::_SendUserMiniInfo()
{
    PacketGuard packet;
    _MakeUserInfo(&packet, static_cast<ENUM_USERINFO>(0));
    G_GameWorld()->send_all(packet);
}

// ORIG T 0x82e7a5e _SendUserUdpIpPort()
void CAssaultPlace::_SendUserUdpIpPort()
{
    PacketGuard packet;
    _MakeUserUdpIpPort(&packet);
    SendPacket(packet);
}

// ORIG T 0x82e8de2 _MakeUserUdpIpPort(PacketGuard*)
void CAssaultPlace::_MakeUserUdpIpPort(PacketGuard* p)
{
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
        p->put_int(m_assaulters[i].GetUser()->m_network.get_mtu());
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
void CAssaultPlace::_SendVictoryBonus(CUser* user, int a, int b, int c, int d)
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
void CAssaultPlace::_SendPowerWarAssaultResultToUser(CUser* user, int point)
{
    PacketGuard packet;
    packet.put_header(0, 200);
    packet.put_byte(point);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG T 0x82e8300 _MoveAssaultPlace()
void CAssaultPlace::_MoveAssaultPlace()
{
    PacketGuard packet;
    _MakeMoveAssaultPlace(&packet);
    SendPacket(packet);
}

// ---------------------------------------------------------------------------
// 连接 P2P 统计组
// ---------------------------------------------------------------------------

// ORIG T 0x82e927e _IsCannotConnectP2P(int, int)
int CAssaultPlace::_IsCannotConnectP2P(int a, int b)
{
    // asm：offset = this + a*0x20 + b + 0x15 的单字节（a<<5 后按字节加 b，
    //   并非第二个槽基址）；该字节非零且 m_assaulters[b] 非空 → 判"无法连接"。
    const char* p =
        reinterpret_cast<const char*>(this) + a * 0x20 + b + 0x15;
    if (static_cast<unsigned char>(*p) == 0)
        return 1;
    if (!m_assaulters[b].Empty())
        return 1;
    return 0;
}

// ORIG T 0x82e93bc _GetConnectP2PPoint(int)
int CAssaultPlace::_GetConnectP2PPoint(int idx)
{
    int points = 0;                             // movl $0x0,-0x10(%ebp)
    for (int i = 0; i <= 7; ++i) {              // cmpl $0x7 … setle
        if (idx != i) {
            // X2: ORIG `call _IsCannotConnectP2P; xor $1,%eax; test %al`（int 返回
            // 不自救不行，绑 %eax 具名 uchar + ^=1 复现单段——技巧 22 精化）
            register unsigned char ic asm("eax") =
                (unsigned char)_IsCannotConnectP2P(idx, i);
            ic ^= 1;
            if (ic)
                points++;
        }
    }
    return points;
}

// ORIG T 0x82e9412 _GetConnectP2PMaxMinPoint(int&, int&)
void CAssaultPlace::_GetConnectP2PMaxMinPoint(int& maxPt, int& minPt)
{
    maxPt = 0;                                  // 第 1 引用参 init 0 → 最大值
    minPt = 8;                                  // 第 2 引用参 init 8 → 最小值
    for (int i = 0; i <= 7; ++i) {
        if (!m_assaulters[i].Empty()) {
            int pt = _GetConnectP2PPoint(i);
            if (pt < minPt)
                minPt = pt;
            if (maxPt < pt)
                maxPt = pt;
        }
    }
}

// ORIG T 0x82e92c6 _ClearConnectP2PAbsolute(int)
int CAssaultPlace::_ClearConnectP2PAbsolute(int idx)
{
    for (int i = 0; i <= 7; ++i) {
        if (idx != i) {
            // _IsCannotConnectP2P int 返回→窄化 %eax 具名 uchar 得 ORIG `test %al; je`
            register unsigned char ic asm("eax") =
                (unsigned char)_IsCannotConnectP2P(idx, i);
            if (ic) {
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
int CAssaultPlace::_ClearConnectP2PRelative(int idx)
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
void CAssaultPlace::_PowerWarAssaultCheckIn()
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
bool CAssaultPlace::_CheckPowerWarInPlayer(ENUM_POWER_SIDE_TYPE side,
                                           ENUM_POWER_WAR_WINNER_OR_LOSER winner,
                                           int& winCnt, int& loseCnt,
                                           int& aliveCnt)
{
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
void CAssaultPlace::_SendStraightVictories(CUser* user)
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
void CAssaultPlace::_SendPowerWarPointToGuildServer(CUser* user,
                                                    ENUM_POWER_WAR_WINNER_OR_LOSER winner,
                                                    int winCnt, int loseCnt,
                                                    int winAliveCnt)
{
    if (user == 0) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf4c, "Invalid User pointer income (%d, %d, %d, %d)",
            winner, winCnt, loseCnt, winAliveCnt);
        return;
    }

    Packet_Guild_Change_Power_War_Point pkt;     // ORIG ctor W 0x082f070c
    char side;
    if (winner == 1) {
        side = static_cast<char>(user->getPowerSide());
    } else {
        side = (user->getPowerSide() == 1) ? 2 : 1;
    }
    pkt.m_powerSide = side;

    int aliveCnt = winAliveCnt;
    if (winCnt - aliveCnt < 0) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf5c, "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d )",
            winCnt, aliveCnt);
        aliveCnt = winCnt;                       // ORIG: param_6 = param_4
    }

    unsigned char winPoint;
    // P48/P183：ORIG 每次读取 multiplicand 均重调 G_CDataManager 并读
    // m_powerParameter(+0xa67c)（+0xf5/+0x11b/+0x16a/+0x187/+0x1a2/+0x1f1 六点），
    // 禁具名缓存。
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - aliveCnt) > 0xff) {                 // imul; cmp $0xff; setg
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf62,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d)",
            winCnt, aliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        winPoint = 0xff;
    } else {
        winPoint = static_cast<unsigned char>(
            static_cast<char>(G_CDataManager()->m_powerParameter.m_powerWarPointMultiple) *
            (static_cast<char>(winCnt) - static_cast<char>(aliveCnt)));
    }
    unsigned char losePoint;
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt > 0xff) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf6a,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d )",
            winCnt, aliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        losePoint = 0xff;                        // movb $0xff
    } else {
        losePoint = static_cast<unsigned char>(
            static_cast<char>(G_CDataManager()->m_powerParameter.m_powerWarPointMultiple) *
            static_cast<char>(loseCnt));
    }

    int bonus = _VictoryBonus(user, side);
    losePoint = static_cast<unsigned char>(losePoint + bonus);   // add (%edx,%eax)

    int winnerNo[4] = {0};
    int loserNo[4] = {0};                        // auStack_62 高半区（asm 同栈连续 [8]）
    int winnerIdx = 0;
    int loserIdx = 0;
    for (int i = 0; i <= 7; ++i) {
        if (!m_assaulters[i].Empty()) {
            CUser* u = m_assaulters[i].GetUser();
            if (u == 0)
                continue;
            int now = GlobalData::s_systemTime_.getCurSec();
            unsigned int playTime = static_cast<unsigned int>(now - m_abuseSec);   // +0x13c
            {
                cUserHistoryLog* hist =
                    &u->m_historyLog;                           // +0x79700
                bool winFlag =
                    static_cast<int>(static_cast<unsigned char>(side))
                    == static_cast<int>(static_cast<signed char>(u->getPowerSide()));
                hist->EndPvP((ENUM_PVP_HISTORY_REASON)1 /*ORIG 字面 1*/,
                             winFlag, playTime);
            }
            u->IncPowerWarAssaultCount();
            if (static_cast<int>(static_cast<unsigned char>(side))
                == static_cast<int>(static_cast<signed char>(u->getPowerSide()))) {
                u->IncPowerWarAssaultVictoryCount();
                u->IncreasePowerWarPoint(static_cast<short>(winPoint));
                if (winnerIdx > 3) {
                    LogManager::logFormat(
                        1, "Assault.cpp",
                        __PRETTY_FUNCTION__,
                        0xf94,
                        "Power War Point Computer Error( winner index : %d, loser index : %d )",
                        winnerIdx, loserIdx);
                } else {
                    winnerNo[winnerIdx] = u->get_charac_no(-1);
                    winnerIdx++;
                    _SendPowerWarAssaultResultToUser(u, winPoint);
                }
            } else {
                u->IncreasePowerWarPoint(static_cast<short>(losePoint));
                if (loserIdx > 3) {
                    LogManager::logFormat(
                        1, "Assault.cpp",
                        __PRETTY_FUNCTION__,
                        0xfa4,
                        "Power War Point Computer Error( winner index : %d, loser index : %d )",
                        winnerIdx, loserIdx);
                } else {
                    loserNo[loserIdx] = u->get_charac_no(-1);
                    loserIdx++;
                    _SendPowerWarAssaultResultToUser(u, losePoint);
                }
            }
        }
    }

    // 包内 characNo 数组（胜者前 4、败者后 4；仅在 loop 内逐槽写入，此处回填对齐）
    for (int k = 0; k < 4; ++k) {
        pkt.m_characNo[k]     = static_cast<unsigned int>(winnerNo[k]);
        pkt.m_characNo[k + 4] = static_cast<unsigned int>(loserNo[k]);
    }
    pkt.m_winPoint = winPoint;
    pkt.m_losePoint = losePoint;

    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xfad, 0);
        trace("win side(%d), win pp(%d, %d), lose pp(%d, %d), winAliveCnt(%d)",
              static_cast<int>(static_cast<unsigned char>(side)), winCnt,
              static_cast<int>(winPoint), loseCnt,
              static_cast<int>(losePoint), aliveCnt);
    }

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE);
    proxy->SendPowerWarPoint(pkt);

    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        side, static_cast<int>(winPoint));
    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        (side == 1) ? 2 : 1, static_cast<int>(losePoint));
}

// ORIG T 0x82ec558/0x82eceb2 直读补充：_SendStraightVictories 末尾 endp。
// （注：上面的 EndPvP reason 参数按 ORIG mangled 为字面 1（0x082ed187 前 mov $1）。）

// ---------------------------------------------------------------------------
// TimeoutCompleteLoad
// ---------------------------------------------------------------------------

// ORIG T 0x82ea31a TimeoutCompleteLoad()（ORIG 返回 undefined4：尾部 1/0，
// mangled 不含返回类型；按头声明 void 落地，返回值语义已并入路径注释）
void CAssaultPlace::TimeoutCompleteLoad()
{
    if (m_state != 2)
        throw (ENUM_ERROR)0x13;
    for (int i = 0; i <= 7; ++i) {
        if (!m_assaulters[i].Empty() && m_assaulters[i].GetState() != 2) {
            if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                GlobalData::s_power_manager->GetPowerWarLog()->IncLoadingTimeOutCountSecede();
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0xa18, 0);
                    trace("POWER WAR LOADING SECEDE");
                }
                for (int j = 0; j <= 7; ++j) {
                    if (!m_assaulters[j].Empty()) {
                        // ORIG 蓝/红两分支各调一次 GetUser（P48 静态调用点各一）
                        if (m_assaulters[j].GetTeam() == 0) {
                            cMyTrace t(__PRETTY_FUNCTION__, 0xa20, 0);
                            t("BLUE TEAM : %s",
                              m_assaulters[j].GetUser()->getCurCharacName());
                        } else {
                            cMyTrace t(__PRETTY_FUNCTION__, 0xa22, 0);
                            t("RED TEAM : %s",
                              m_assaulters[j].GetUser()->getCurCharacName());
                        }
                    }
                }
                cMyTrace secede(__PRETTY_FUNCTION__, 0xa25, 0);
                // ORIG 直接把 GetUser(i) 指针作为 %s 实参传给 trace（保留原样）。
                secede("SECEDE : %s", reinterpret_cast<const char*>(m_assaulters[i].GetUser()));
            }
            CUser* u = m_assaulters[i].GetUser();
            bool pwVill = GlobalData::s_power_manager->IsPowerWarEventOn()
                          && u != 0
                          && u->getCurCharacVill() == 7;
            if (pwVill) {
                PacketGuard packet;
                packet.put_header(0, 0xe6);
                packet.put_byte(1);
                packet.finalize(true);
                u->Send(packet);
            }
        }
    }
    for (int i = 0; ; ++i) {
        if (i > 7) {
            DoConnectP2P();
            // ORIG 直读 m_parties[i].m_field00（空判定 +0x12c/0x130 指针），不走 Get()
            if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
                (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x03');
            if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
                (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x03');
            return;                              // ORIG return 1（void 化丢弃）
        }
        if (!m_assaulters[i].Empty() && m_assaulters[i].GetState() != 2) {
            LeaveAssaultPlace(i, true, false);
            if (m_state == 6)
                break;                           // ORIG 提前返回 0
        }
    }
    // ORIG return 0：state==6 提前退出路径
}


// ---------------------------------------------------------------------------
// ChangeHp
// ---------------------------------------------------------------------------

// ORIG T 0x82ed752 ChangeHp(CUser*, int)
// [b21 N·P198-②/P197-④] 语义已闭合：`pwVill = IsPowerWarEventOn() && getCurCharacVill()==7`
//   短路判定 ORIG(4.1) 直测 `test %al,%al; je`（无槽），4.4-O0 恒把中间结果
//   `mov %al,-off; cmpb` 物化（P197-④ 源形不可择）；末段 allDead 循环 `GetState()!=8`
//   反亦 `setne` 物化 vs ORIG `cmp $8; jne`；循环回边 `jmp;nop` 合并差（N82）。逐调用
//   逐分支语义逐一相等（_GetUserIndex/m_assaulters[idx].ChangeHp/_MakeChangeHp/Send 三叉/
//   CloseAssault 段全对齐），封 N。
void CAssaultPlace::ChangeHp(CUser* user, int hp)
{
    int idx = _GetUserIndex(user, true);
    // P198-②：ORIG 对 CAssaulter::ChangeHp 返回 `test %al,%al; je` 寄存器直测（无槽）；
    // `bool changed` 具名会额外 `mov %al,-off; cmpb $0,-off`，内联直测对齐。
    if (m_assaulters[idx].ChangeHp(hp)) {
        PacketGuard packet;
        _MakeChangeHp(&packet, idx, hp);
        bool pwVill = GlobalData::s_power_manager->IsPowerWarEventOn()
                      && user->getCurCharacVill() == 7;
        if (pwVill) {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party == 0)
                user->Send(packet);
            else
                party->send_to_party(packet);
        } else {
            _SendPacketToArea(packet);
        }
    }
    if (m_state == 5 || m_state == 6) {
        m_assaulters[idx].SetState(static_cast<ENUM_ASSAULT_STATE>(8));
        bool allDead = true;
        for (int i = 0; i <= 7; ++i) {
            if (!m_assaulters[i].Empty()) {
                if (m_assaulters[i].GetState() != 8)
                    allDead = false;
            }
        }
        if (allDead) {
            if ((reinterpret_cast<CParty*>(m_parties[0].Get())) != 0)
                (reinterpret_cast<CParty*>(m_parties[0].Get()))->SetAssaultState('\a');
            if ((reinterpret_cast<CParty*>(m_parties[1].Get())) != 0)
                (reinterpret_cast<CParty*>(m_parties[1].Get()))->SetAssaultState('\a');
            m_state = 7;
            CloseAssault();
        }
    }
}

// ---------------------------------------------------------------------------
// DieUser 双版本
// ---------------------------------------------------------------------------

// ORIG T 0x82ebda6 DieUser(int, int, MSG_DIE_ASSAULT_PLAYER&)
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
void CAssaultPlace::DieUser(int victimIdx, int killerIdx, MSG_DIE_ASSAULT_PLAYER& msg)
{
    char killerName[0x15] = { 0 };               // ORIG 0x15 字节初始化清零展开（非 memset call）

    if (m_state != 4)
        throw (ENUM_ERROR)0x13;

    CUser* victim = m_assaulters[victimIdx].GetUser();
    if (m_assaulters[victimIdx].GetState() != 5)
        throw (ENUM_ERROR)0x12;

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

    m_state = 5;
    // ORIG 直读 m_parties[i].m_field00（+0x12c/+0x130），无 Get() call
    if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x05');
    if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x05');
    EndAssault();
}

// ORIG T 0x82ed3fe DieUser(CUser*, int, MSG_DIE_ASSAULT_PLAYER&)
void CAssaultPlace::DieUser(CUser* user, int killerIdx, MSG_DIE_ASSAULT_PLAYER& msg)
{
    // b28 BL6: P0——ORIG 切片无 idx 槽往返，返回值寄存器直传出参槽；
    // 具名局部引入 -0xc 双 store/load +帧差。嵌套实参复现 ORIG 0x82ed3fe 形态。
    DieUser(_GetUserIndex(user, true), killerIdx, msg);
}

// ---------------------------------------------------------------------------
// MakeTeam / 组队操作组
// ---------------------------------------------------------------------------

// ORIG T 0x82e7d0a MakeTeam(CUser*, CUser*, int&)
void CAssaultPlace::MakeTeam(CUser* user1, CUser* user2, int& outIdx)
{
    // param_2 存 +0x134、param_3 存 +0x138——与既有头字段一一对应。
    m_pos.m_x = user1->getCurCharacVill();
    m_pos.m_y = user1->get_area(false);
    if (m_pos.m_x == 0 && m_pos.m_y == 0)
        throw (ENUM_ERROR)0x15;
    m_checkUser = user1;                                        // +0x134
    m_field138 = user2;                                         // +0x138

    // posX 比较：ORIG 首 get_posX 结果 `mov %eax,%ebx` 跨第二 call 存寄存器
    // （P197② 语句表达式）；随后 _SeparateTeam 两参顺序随比较翻转，+0x124 记 0/1。
    if (({ unsigned short r = user1->get_posX(); r; }) < user2->get_posX()) {
        _SeparateTeam(user2, user1);
        m_winTeam = 0;                                          // +0x124
    } else {
        _SeparateTeam(user1, user2);
        m_winTeam = 1;                                          // +0x124
    }

    m_authenValue = GlobalData::s_systemTime_.getCurSec();       // +0x108 = getCurSec
    _SetReadyState();

    if (m_parties[0].m_field00 != 0)                            // +0x12c
        m_parties[0].m_field00->SetAssaultState('\x02');
    if (m_parties[1].m_field00 != 0)                            // +0x130
        m_parties[1].m_field00->SetAssaultState('\x02');

    // 8 个名字槽（.rodata 空串 DAT_08c1bca0 直存），team0 填 [0..]，team1 填 [4..]。
    const char* names[8] = { "", "", "", "", "", "", "", "" };
    int team0Count = 0;                          // local_2c
    int team1Count = 0;                          // local_28
    for (int i = 0; i <= 7; ++i) {
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
    for (int i = 0; i <= 7; ++i) {
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
void CAssaultPlace::_GetTeamCountAliveUser(int* counts)
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
void CAssaultPlace::_AddUser(CUser* user, ENUM_ASSAULT_TEAM team)
{
    int slot = 0;
    for (slot = 0; slot <= 7; ++slot) {
        if (m_assaulters[slot].IsAttachAble())
            break;
        if (slot == 7)
            throw (ENUM_ERROR)0x04;              // ORIG __cxa_throw ENUM_ERROR(4)
    }
    m_assaulters[slot].AttachUser(user, team);
    user->SetAssaultPlace(GetAssaultPlaceId());  // +0x104 → CUserCharacInfo::SetAssaultPlace
    m_field100++;                                // GetAssaulterCount 计数域
}

// ORIG T 0x82e70c8 _AddTeam(CUser*, ENUM_ASSAULT_TEAM)
void CAssaultPlace::_AddTeam(CUser* user, ENUM_ASSAULT_TEAM team)
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
void CAssaultPlace::_MoveUser(pvp_assault::CDisposition* disp, int x, int y)
{
    int counts[2] = {0};
    for (int i = 0; i <= 7; ++i) {
        if (!m_assaulters[i].Empty()) {
            CForcedMoveCharacter fm;             // 栈上结构，偏移对应见头
            fm.m_field00 = static_cast<int>(m_assaulters[i].GetTeam());
            fm.m_slotIdx = counts[static_cast<int>(m_assaulters[i].GetTeam())];
            fm.m_user    = m_assaulters[i].GetUser();
            // asm：param_2→+0x10、param_3→+0x14；+0x0c 在 ORIG 帧中未初始化。
            fm.m_destArea = x;
            fm.m_x        = y;
            int dir = pvp_assault::GetDirectionByTeam(
                static_cast<ENUM_ASSAULT_TEAM>(m_assaulters[i].GetTeam()));
            (void)dir;                           // ORIG 计算后未消费（保留调用序）
            disp->MoveUser(&fm);
            counts[static_cast<int>(m_assaulters[i].GetTeam())]++;
        }
    }
}

}  // namespace pvp_assault
