// ============================================================================
// df_game_r MakePVPRoom 全局函数还原（2026-08-28）。
// ORIG T _Z11MakePVPRoomP5CUserR17MSG_MAKE_PVP_ROOM14ENUM_CMDPACKET15PVP_BATTLE_MODE
//   @0x081f793c（sz 0x701）。
// 返回类型：ORIG __PRETTY_FUNCTION__ @0x08bcff20 实证为 "bool MakePVPRoom(...)"
//   （mangled 不含返回类型，与 PVP_BATTLE_MODE 返回同符号；此处忠实写 bool）。
// 逐指令转写自 objdump 0x081f793c..0x081f803d；日志串原样：
//   0x8bc22a4 "pvp@log %s,%s, room(%d),%d,%d,%d"
//   0x8bc22c8 "create_room fail: PVP할 수 없는 지역 입니다. ID[%d]\n"
//   0x8bc1464 "PacketDispatcher_Impl_1.cpp"（logFormat file 参）
// 调用方：DisPatcherCore.cpp::DisPatcher_MakePVP::dispatch_sig（cmd=0x35/0x9f）。
// ============================================================================

#include "CDataManager.h"        // G_CDataManager / GetMaxGradePvPChannel / m_serverParameter
#include "CGameManager.h"        // G_CGameManager / GetPvp / CheckOutParty / PutPvp
#include "GameWorld.h"           // G_GameWorld / GetChannelType / IsPVPChannel / send_all / goto_pvp
#include "PvP_Room.h"            // create_room / get_index / make_room_info
#include "CParty.h"              // CParty::send_invite_pvp
#include "CFairPvPScore.h"       // fair_pvp::CFairPvPScore::GetGiveItem
#include "PacketGuard.h"         // PacketGuard : InterfacePacketBuf（put_header/put_byte/put_short/finalize/clear）
#include "CSwitchLog.h"          // CSwitchLog（ORIG 854fd3c/854fd64）
#include "LogManager.h"          // LogManager::logFormat
#include "GameEnums.h"           // ENUM_CMDPACKET / PVP_BATTLE_MODE / ENUM_CHARAC_EXPAND_TYPE

// MSG_MAKE_PVP_ROOM：权威定义当前在 DisPatcherCore.cpp wave1 声明区（sizeof 0x3d，
// dispatch_sig memset 0x3d 实证；DisPatcherCore.h 仅前置声明）。此处按同一布局
// 重声明（TU 内类型，无链接符号，mangled 参数名一致即得 ORIG 符号）。
struct MSG_MAKE_PVP_ROOM
{
    char m_pad0[0xd];        // +0x00..0x0c
    char m_field0d;          // +0x0d 门类型（<0 失败）
    char m_name[0x1e];       // +0x0e..0x2b 房名
    short m_field2c;         // +0x2c
    char m_field2e;          // +0x2e 模式门
    int m_field2f;           // +0x2f 密码长（<=8）
    char m_field33[9];       // +0x33..0x3b 密码/校验缓冲
    char m_battleMode;       // +0x3c battleMode
};

// 共用错误应答：put_byte(0), put_byte(err), finalize(1), Send —— ORIG 各失败段
// 0x820..0x813/0x7ac8..0x7b0e/0x7b31../0x7b9b../0x7c06.. 共形（仅 err 码不同）。
static void SendMakePVPRoomError(CUser* user, PacketGuard& packet, int errCode)
{
    packet.put_byte(0);
    packet.put_byte(errCode);
    packet.finalize(1);
    user->Send(packet);
}

bool MakePVPRoom(CUser* user, MSG_MAKE_PVP_ROOM& msg, ENUM_CMDPACKET cmd, PVP_BATTLE_MODE mode)
{
    PacketGuard packet;                                  // -0x40
    packet.put_header((int)cmd, 1);                      // 0x81f7968

    // 最高段位频道的段位上限门（ORIG 0x81f796d..0x81f7a1d：ch==0xf || ch==0x10）
    int chType = G_GameWorld()->GetChannelType();
    bool maxGradeChannel = (chType == 0xf || chType == 0x10);
    if (maxGradeChannel)
    {
        if (G_CDataManager()->GetMaxGradePvPChannel() < user->get_pvp_grade())
        {
            SendMakePVPRoomError(user, packet, 0xd);     // 0x81f79c8..0x81f7a13
            return false;
        }
    }

    // 公平 PvP 给品计数门（ORIG 0x81f7a1d..0x81f7a62：give <= get_pvp_item_give → err 7）
    fair_pvp::CFairPvPScore* pvpScore =
        (fair_pvp::CFairPvPScore*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
    int giveItem = (int)pvpScore->GetGiveItem();
    int giveLimit =
        G_CDataManager()->m_serverParameter.get_pvp_item_give(user->get_pvp_grade());
    if (giveItem <= giveLimit)
    {
        SendMakePVPRoomError(user, packet, 0x7);         // 0x81f7a64..0x81f7aaf
        return false;
    }

    // 幽灵状态门（ORIG 0x81f7ab9..0x81f7b13）
    if (user->IsCurCharacGhost() != 0)
    {
        SendMakePVPRoomError(user, packet, 0x13);
        return false;
    }

    // 非 PvP 频道门（ORIG 0x81f7b1d..0x81f7b81：!IsPVPChannel() → err 0x13）
    if (!G_GameWorld()->IsPVPChannel())
    {
        SendMakePVPRoomError(user, packet, 0x13);
        return false;
    }

    // 用户状态门（ORIG 0x81f7b86..0x81f7be6：get_state() != 3 → err 0x13）
    if (user->get_state() != 3)
    {
        SendMakePVPRoomError(user, packet, 0x13);
        return false;
    }

    // PvP 管理器在位门（ORIG 0x81f7bf0..0x81f7c56：GetPvp()==0 → err 4）
    PvP_Room* pvp = G_CGameManager()->GetPvp();         // -0x24
    if (pvp == 0)
    {
        SendMakePVPRoomError(user, packet, 0x4);
        return false;
    }

    // 组队邀请转发 + 出队登记（ORIG 0x81f7c5b..0x81f7cb6）
    if (user->CheckInParty())
    {
        CParty* party = (CParty*)user->GetParty();       // -0x1c
        if (party != 0)
        {
            party->send_invite_pvp((int)(unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }

    // 建房（ORIG 0x81f7cb7..0x81f7d26）
    int roomNo = pvp->create_room(user, (char)msg.m_field0d, msg.m_name,
                                   (short)msg.m_field2c, msg.m_field2e != 1,
                                   msg.m_field33, msg.m_field2f, mode);   // -0x20

    // pvp@log（ORIG 0x81f7d26..0x81f7db8；0x8583=34179 行号）
    int roomIndex = pvp->get_index();
    const char* characName = user->getCurCharacName();
    const char* accName = user->get_acc_name();
    CSwitchLog pvpLog("bool MakePVPRoom(CUser*, MSG_MAKE_PVP_ROOM&, ENUM_CMDPACKET, PVP_BATTLE_MODE)", 0x8583, 0, 0);
    pvpLog("pvp@log %s,%s, room(%d),%d,%d,%d", accName, characName, (int)roomIndex,
           (int)mode, roomNo, (int)cmd);

    if (roomNo > 0)
    {
        // 成功应答（ORIG 0x81f7dc2..0x81f7e07）
        // 建房失败计费区日志（ORIG 0x81f7e0c..0x81f7e42；0x858c=34188；
        SendMakePVPRoomError(user, packet, roomNo);      // ORIG 0x81f7dc2..0x81f7e07：byte(0), byte(ret), finalize, Send
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__,
                              0x858c,
                              "create_room fail: PVP할 수 없는 지역 입니다. ID[%d]\n",
                              (int)pvp->get_index());
        G_CGameManager()->PutPvp(pvp);                   // 0x81f7e47..0x81f7e56
        return false;                                    // ebx=0 → 0x81f8025
    }

    // create_room 失败（ret <= 0）：
    // cmd==0x65 附加应答包（ORIG 0x81f7e65..0x81f7ee7）
    if ((int)cmd == 0x65)
    {
        PacketGuard failPacket;                          // -0x4c
        failPacket.put_header(0x65, 1);
        failPacket.put_byte(1);
        failPacket.finalize(1);
        user->Send(failPacket);
    }

    // 基本信息重发（ORIG 0x81f7ef2..0x81f7f78：header(0,2), byte 1, short 1, make_basic_info）
    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 1);
    packet.finalize(1);
    user->Send(packet);

    // 房间信息广播（ORIG 0x81f7f7d..0x81f7fea：header(0,0x29), short 1, make_room_info, send_all）
    packet.clear();
    packet.put_header(0, 0x29);
    packet.put_short(1);
    pvp->make_room_info((char*)&packet);
    packet.finalize(1);
    G_GameWorld()->send_all(packet);

    // 传送入 PvP（ORIG 0x81f7fef..0x81f7ffe）
    G_GameWorld()->goto_pvp(user);
    return true;                                         // ebx=1 → 0x81f8025
}
