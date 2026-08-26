// GmCmds_M.cpp — game_master::CMoveCharacCmd（id=0x01 "move"，样板实现）
// + 9 个命令类的 execute/parse 补齐（vtable key function 落地 TU）。
// 对照 docs/class_func_reports/game_master__{CPvPStartCmd,CItemDropRateCmd,
// CResetTutorialFlagCmd,CRunMacroCmd,CAchievementReset,CAchievementReport,
// CAchievementClear,CAchievementAllClear,CAchievementNoClear}/ 逐函数转写。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CEnvironment.h"          // trim(std::string&)
#include "CTitleBook.h"            // CTitleBook::_gm_clear/sendList
#include "CAchievement.h"          // CAchievement::_gm_*/setTrigger/sendList
#include "PvP_Room.h"             // PvP_Room（GetPVPRoom 返回类型）
#include "MatchRoomList.h"         // IMatch::OnRegisterMatch
#include "LogManager.h"            // LogManager::logFormat

#include <cstdlib>                 // atoi
#include <cstdio>                  // sprintf
#include <cstring>                 // memset/strcpy/strlen

namespace game_master
{

// ORIG ctor 0x082a7ffe：基类 vptr → 自身 vptr(0x8c81ca0)；string@+0x08 默认构造。
CMoveCharacCmd::CMoveCharacCmd()
    : m_str0()
{
}

// ORIG parse 0x084a8cce：trim(parser->args)；m_str0 = parser->args（目标角色名）。
void CMoveCharacCmd::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}

// ORIG execute 0x084a8d02。
void CMoveCharacCmd::execute()
{
    GameWorld* gw = G_GameWorld();
    CUser* target = gw->find_user_by_charac_name(m_str0.c_str());
    if (target == 0)
    {
        return;
    }
    if (target->getCurCharacR() == 0)                       // ORIG CUserCharacInfo::getCurCharacR 判 0
    {
        return;
    }
    int vill = static_cast<int>(target->getCurCharacVill());
    int area = target->get_area(false);
    unsigned int x = target->get_posX() & 0xffff;
    unsigned int y = target->get_posY() & 0xffff;

    // 与发起 GM 同 vill 且同 area 则原地 set_position，否则跨区 move_area。
    bool needMove = true;
    CUser* self = GetUser();
    if (static_cast<int>(self->getCurCharacVill()) == vill &&
        self->get_area(false) == area)
    {
        needMove = false;
    }
    if (needMove)
    {
        G_GameWorld()->move_area(self, vill, area,
                                 static_cast<int>(x), static_cast<int>(y),
                                 0, false, 0, 0, 0);
    }
    else
    {
        self->set_position(static_cast<unsigned short>(x),
                           static_cast<unsigned short>(y), 0);
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(static_cast<int>(self->get_unique_id() & 0xffff));
    packet.put_short(static_cast<int>(x));
    packet.put_short(static_cast<int>(y));
    packet.put_byte(0);
    packet.finalize(true);
    if (vill != 0 || area != 0)
    {
        G_GameWorld()->send_to_area(vill, area, packet);
    }
}

    // -------------------------------------------------------------------
    // 注册链 ctor 补齐（GmRegister.cpp new 调用；ORIG W 弱符号，基类
    // CCommand ctor @GmCore.cpp）。sizeof=0x8 类无自有成员，空体即由
    // 编译器生成「调基类 ctor + 设自身 vptr」，与 ORIG 一致。
    // -------------------------------------------------------------------

    CPvPStartCmd::CPvPStartCmd()            // ORIG W 0x082a8028
    {
    }

    CItemDropRateCmd::CItemDropRateCmd()    // ORIG W 0x082a807c
    {
    }

    CResetTutorialFlagCmd::CResetTutorialFlagCmd()   // ORIG W 0x082a80ec
    {
    }

    CRunMacroCmd::CRunMacroCmd()            // ORIG W 0x082a81da
    {
    }

    CAchievementReset::CAchievementReset()      // ORIG W 0x082a871a
    {
    }

    CAchievementReport::CAchievementReport()    // ORIG W 0x082a8736
    {
    }

    CAchievementClear::CAchievementClear()      // ORIG W 0x082a8752（sizeof=0xc）
    {
    }

    CAchievementAllClear::CAchievementAllClear()    // ORIG W 0x082a876e
    {
    }

    CAchievementNoClear::CAchievementNoClear()      // ORIG W 0x082a878a
    {
    }

    // -------------------------------------------------------------------
    // CPvPStartCmd — ORIG execute 0x084a8f6a。
    // 取 PVPRoom，空则记日志；否则对 +0x6e4 登记的 IMatch 匹配器
    // 虚调用 vtable+0x30 = OnRegisterMatch(user, pRoom)。
    // -------------------------------------------------------------------
    // PvP_Room +0x6e4 视图（PvP_Room.h 该槽现标 int；ORIG 实为 create_room
    // 时 CMatchingSystem::GetMatchingSystem 登记的匹配器指针）。
    struct PvpRoomMatcherSlot
    {
        char m_pad[0x6e4];
        IMatch* m_matcher;
    };

    void CPvPStartCmd::execute()
    {
        PvP_Room* pRoom = (PvP_Room*)GetUser()->GetPVPRoom();
        if (pRoom == 0)
        {
            LogManager::logFormat(1, "GameMaster.cpp",
                "virtual void game_master::CPvPStartCmd::execute()", 0x169,
                "PVPRoom pRoom is NULL");
            return;
        }
        ((PvpRoomMatcherSlot*)pRoom)->m_matcher
            ->OnRegisterMatch(GetUser(), pRoom);       // ORIG 虚表+0x30
    }

    // -------------------------------------------------------------------
    // CItemDropRateCmd — ORIG execute 0x084a9002。
    // 函数级静态 ItemDropRate 角色对象（ORIG guard 0x0943dc40/数据
    // 0x0943dc48），SetGameMasterCharacter 注入到当前 GM 用户。
    // -------------------------------------------------------------------
    void CItemDropRateCmd::execute()
    {
        static CGameMasterCharacterItemDropRate gameMasterCharacter;
        GetUser()->SetGameMasterCharacter(&gameMasterCharacter);
    }

    // -------------------------------------------------------------------
    // CResetTutorialFlagCmd — ORIG execute 0x084a94c6。
    // 重置教学标志后回读 0..0x7f 标志位拼接 "(n) " 列表，
    // put_header(0,0xc)/byte/short/byte/int(len)/str 后 Send 给用户。
    // -------------------------------------------------------------------
    void CResetTutorialFlagCmd::execute()
    {
        GetUser()->resetCharacTutorialFlag();

        std::string msg = "tutorial flag : ";
        char num[255] = { '\0' };          // ORIG 内联清零展开（0xff 字节）
        for (int i = 0; i <= 0x7f; ++i)    // ORIG cmpl $0x7f+setle（有符号）
        {
            // ORIG test %al：按单字节布尔读（头文件声明 int，此处收窄）。
            if ((unsigned char)GetUser()->getCurCharacEscaladeTutorialFlag(i))
            {
                sprintf(num, "(%d) ", i);
                msg += num;
            }
        }

        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int((int)msg.size());
        packet.put_str(msg.c_str(), (int)msg.size());
        packet.finalize(true);
        GetUser()->Send(packet);
    }

    // ORIG execute 0x084a9c06：纯 ret（宏回放入口由 CMacro::RudMacro 承担）。
    void CRunMacroCmd::execute()
    {
    }

    // -------------------------------------------------------------------
    // CAchievementReset — ORIG execute 0x084b076a。
    // 称号簿(扩展数据 0xe)_gm_clear + 成就(0xf)_gm_init，再双列表重发。
    // -------------------------------------------------------------------
    void CAchievementReset::execute()
    {
        ((CTitleBook*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_init();
        ((CTitleBook*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->sendList();
    }

    // GmCmds_A.cpp 提供的 ORIG 符号（CBoosterTest::printClient 静态桥接）。
    void gmM_booster_printClient(CUser*, bool, char const*, char const*)
        asm("_ZN11game_master12CBoosterTest11printClientEP5CUserbPKcS4_");

    // -------------------------------------------------------------------
    // CAchievementReport — ORIG execute 0x084b07fe。
    // 成就(0xf)_gm_report 到栈缓冲（95000 字节，先清零、限长 strcpy），
    // 经 CBoosterTest::printClient 以 achievement_report.txt 回显给 GM。
    // -------------------------------------------------------------------
    void CAchievementReport::execute()
    {
        char fileBuf[95000];
        memset(&fileBuf[0], 0, 95000);

        std::string out;
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_report(out);
        if (out.length() < sizeof(fileBuf))
            strcpy(fileBuf, out.c_str());

        gmM_booster_printClient(GetUser(), false,
                                "achievement_report.txt", fileBuf);
    }

    // -------------------------------------------------------------------
    // CAchievementClear — ORIG parse 0x084b08f0 / execute 0x084b092a。
    // parse：trim(args)+atoi 存 m_achievementId(+0x08)；
    // execute：成就 setTrigger(id, 0xffff, 0xffff, 0xffff) 全条件达成。
    // -------------------------------------------------------------------
    void CAchievementClear::parse()
    {
        trim(m_parser->m_args);
        m_achievementId =
            (unsigned int)atoi(m_parser->m_args.c_str());
    }

    void CAchievementClear::execute()
    {
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))
            ->setTrigger(m_achievementId, 0xffff, 0xffff, 0xffff);
    }

    // CAchievement 扩展数据 +0x24 标志字节视图（未具名字段，参照
    // GmCmds_G AchFlagView 同类处理）。
    struct AchNoClearFlagView
    {
        char m_pad[0x24];
        unsigned char m_flag;
    };

    // ORIG execute 0x084b097c：成就(0xf) _gm_clearAll。
    void CAchievementAllClear::execute()
    {
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_clearAll();
    }

    // ORIG execute 0x084b09a8：标志字节置 1（ORIG movb $1,0x24(%eax) 单指令）。
    void CAchievementNoClear::execute()
    {
        ((AchNoClearFlagView*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->m_flag = 1;
    }

} // namespace game_master


// =======================================================================
// CGameMasterCharacter 族（全局命名空间，ORIG W 弱符号族）。
// 基类仅设 vptr；sendServerState 基版为纯 ret。派生版把文本以
// put_header(0,0xc)/byte/short/byte/int(strlen)/str/finalize(true)
// 打包后 user->Send（ORIG 0x084b4330，0xf6 字节逐指令转写）。
// =======================================================================
CGameMasterCharacter::CGameMasterCharacter()                 // ORIG 0x084b4426
{
}

void CGameMasterCharacter::sendServerState(CUser*, char const*)   // 0x084b42b6
{
}

CGameMasterCharacterItemDropRate::CGameMasterCharacterItemDropRate()  // 0x084b4434
{
}

void CGameMasterCharacterItemDropRate::sendServerState(
    CUser* user, char const* msg)                            // 0x084b4330
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((int)strlen(msg));
    packet.put_str(msg, (int)strlen(msg));
    packet.finalize(true);
    user->Send(packet);
}
