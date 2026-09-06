// GmCmds_C.cpp — game_master 命令体系批次 C（df_game_r ORIG T 符号族还原）。
// 负责类（10）：
//   CCraneNeedMaterial        ctor 0x082a86aa / execute 0x084b0626
//   CCreateDisjointStoreCmd   ctor 0x082a8108 / execute 0x084a9720
//   CDeleteAccountCargo       ctor 0x082a84ce / execute 0x084abdb4
//   CDeleteGuild              ctor 0x082a83e0 / execute 0x084ab29c
//   DungeonDifficulty         ctor 0x082a8892 / parse 0x084b2110 / execute 0x084b2152
//   CEndEvent                 ctor 0x082a845e / parse 0x084ab914 / execute 0x084ab94c
//   CEnduranceCmd             ctor 0x082a83fc / parse 0x084aa73a / execute 0x084aa858
//   CGrowhWeaponEvent         ctor 0x082a88e6 / parse 0x084b28d6 / execute 0x084b293c
//   CGuildDebug               ctor 0x082a8592 / parse 0x084ac720 / execute 0x084ac76c
//   CGuildExpBookDelete       ctor 0x082a8506 / execute 0x084abf2c
// 对照 docs/class_func_reports/game_master__<Class>/{parse,execute}.md。
//
// 头文件占位字段失配说明：GameMasterCmd.h 对本批各类统一给出 m_str0(string@+0x08)
// 等占位名，但 ORIG 真实布局按各类不同：
//   CEndEvent          +0x08 int（事件代码）
//   CEnduranceCmd      +0x08 int（背包槽位）+0x0c unsigned short（耐久值）
//   CGrowhWeaponEvent  +0x08 byte（"gw" 重置标记）+0x0c int（时计数值）
//   DungeonDifficulty  +0x08 byte（难度 1..3）
//   CGuildDebug        +0x08 int +0x0c int +0x10 unsigned int（m_f1 占位即此字段）
//   （gcc4.4 32 位 std::string 仅 4 字节，故占位 string 与上述标量布局同尺寸。）
// 本 TU 一律按偏移访问真实标量字段，不向占位 std::string 写入。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CInventory.h"
#include "CServerProxyMgr.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "CDungeonClear.h"
#include "CDataManager.h"
#include "CExpertJob.h"
#include "CGuildServerProxy.h"
#include "CMonitorServerProxy.h"
#include "STItemScript.h"
#include "Packet_Monitor_Notice_Guild_Dismiss.h"
#include "Packet_Monitor_Event_End.h"
#include "Packet_Guild_Debug.h"
#include "CEnvironment.h"
#include "DB_EventGrowthWeaponReset.h"            // makeRequest（桥清零）
#include "DB_EventUpdateGrowthWeaponTimepiece.h"  // makeRequest（桥清零）

#include <stdlib.h>

namespace game_master
{

// ========================================================================
// CCraneNeedMaterial —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a86aa：基类 vptr → 自身 vptr。

// ORIG execute 0x084b0626：G_GameWorld()->UpdateMiniCraneSeed()。
void CCraneNeedMaterial::execute()
{
    G_GameWorld()->UpdateMiniCraneSeed();
}

// ========================================================================
// CCreateDisjointStoreCmd —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8108。

// ORIG execute 0x084a9720：取分解师职业槽挂到 GM 角色，随后在其脚下
// 以固定名 "test"/容量 500 创建分解商店（坐标取 GM 角色当前位置低 16 位）。
void CCreateDisjointStoreCmd::execute()
{
    expert_job::CExpertJob* job =
        GlobalData::s_ExpertJobMgr->AcquireExpertJob(
            static_cast<ENUM_EXPERT_JOB_TYPE>(3));
    CUser* user = GetUser();
    user->SetCurCharacExpertJob(job);

    unsigned int x = user->get_posX() & 0xffff;
    unsigned int y = user->get_posY() & 0xffff;
    static_cast<expert_job::CDisjointer*>(job)->OnCreateDisjointStore(
        user, "test", 500, static_cast<int>(x), static_cast<int>(y));
}

// ========================================================================
// CDeleteAccountCargo —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a84ce。

// ORIG execute 0x084abdb4：CAccountCargo::DeleteAccountCargo(GetUser())。
// ORIG 该函数为静态语义直调（cdecl，无 this 使用）；此处经 CUser 内嵌实例
// （布局注记 CUser.h：+0x6effc CAccountCargo）走同一实现路径。
void CDeleteAccountCargo::execute()
{
    CUser* user = GetUser();
    CAccountCargo* cargo = &user->m_accountCargo;
    cargo->DeleteAccountCargo(user);
}

// ========================================================================
// CDeleteGuild —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a83e0。

// ORIG execute 0x084ab29c：GM 校验通过后向 guild 服务器发解散请求（0x1df 流）、
// 清本地公会键并回发 0x3d 包，同时向 monitor 广播 Packet_Monitor_Notice_Guild_Dismiss。
void CDeleteGuild::execute()
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }
    if (user->get_state() != 3)
    {
        return;
    }
    // P48 no-CSE：ORIG 三处各自 fresh get_charac_guildkey()（if 判零 / guard<< /
    // notice.m_guildKey）。
    if (static_cast<unsigned int>(user->get_charac_guildkey()) == 0)
    {
        return;
    }

    // 向 DB 队列发解散流：0x1df + (uid, characNo, guildkey)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0x68d), true);
    // ORIG：`(*guard)->operator<<(x)`（operator* de()）标识调用形态。
    (*guard)->operator<<(0x1df);
    (*guard)->operator<<(user->GetUID());
    (*guard)->operator<<(user->getCurCharacNo());
    (*guard)->operator<<(static_cast<unsigned int>(user->get_charac_guildkey()));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    user->set_guild_create_right(0);
    user->set_charac_guildkey(0);

    PacketGuard packet;
    packet.put_header(0, 0x3d);
    packet.finalize(true);
    user->Send(packet);

    Packet_Monitor_Notice_Guild_Dismiss notice;
    notice.m_guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&notice), 0xe);
}

// ========================================================================
// DungeonDifficulty —— sizeof=0xc；+0x08 byte 难度（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a8892。

// ORIG parse 0x084b2110：难度字节清零后 atoi(args) 写入 +0x08。
void DungeonDifficulty::parse()
{
    trim(m_parser->m_args);
    m_difficulty = 0;
    m_difficulty = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084b2152：遍历全地下城索引表，为非特殊地下城写入通关难度记录。
void DungeonDifficulty::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    unsigned char difficulty = m_difficulty;
    if (difficulty == 0 || difficulty > 3)
    {
        return;
    }

    CDungeonList* list = G_CDataManager()->getDungeonIndexList();  // 真实定义 UserPartyLink4.cpp
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it =
        list->m_dungeonMap.begin();
    for (; it != list->m_dungeonMap.end(); ++it)
    {
        CDungeon* dungeon = it->second;
        if (dungeon == 0)
        {
            continue;
        }
        // 血色地狱(1/2)、绝望之塔、锦标赛、kind==1 的地下城不写入。
        bool skip = dungeon->get_blood_dungeon_type() == 1 ||
                    dungeon->get_blood_dungeon_type() == 2 ||
                    dungeon->isTowerOfDespairDungeon() ||
                    dungeon->isTournamentDungeon() ||
                    dungeon->getDungeonKind() == 1;
        if (!skip)
        {
            WongWork::CDungeonClear* clear = user->getDungeonClearRefW();
            clear->addClearedDungeon(static_cast<short>(dungeon->get_index()),
                                     static_cast<char>(difficulty));
        }
    }
}

// ========================================================================
// CEndEvent —— sizeof=0xc；+0x08 int 事件代码（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a845e。

// ORIG parse 0x084ab914：atoi(args) → +0x08。
void CEndEvent::parse()
{
    trim(m_parser->m_args);
    m_eventCode = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084ab94c：GM 校验后向 monitor 发 Packet_Monitor_Event_End。
void CEndEvent::execute()
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }
    Packet_Monitor_Event_End packet;
    packet.m_eventCode = static_cast<unsigned int>(m_eventCode);
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0xe);
}

// ========================================================================
// CEnduranceCmd —— sizeof=0x10；+0x08 int 槽位、+0x0c unsigned short 耐久
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a83fc。

// ORIG parse 0x084aa73a：trim 后以首个空格切分 args；
// 前段 atoi → 槽位(+0x08)，后段 atoi → 耐久(+0x0c)。ORIG 未校验 find 失配。
void CEnduranceCmd::parse()
{
    trim(m_parser->m_args);
    int pos = static_cast<int>(m_parser->m_args.find(" ", 0));
    std::string slotStr = m_parser->m_args.substr(0, pos);
    std::string enduranceStr = m_parser->m_args.substr(pos + 1, std::string::npos);

    m_slot = atoi(slotStr.c_str());
    m_endurance = static_cast<unsigned short>(atoi(enduranceStr.c_str()));
}

// ORIG execute 0x084aa858：GM 且槽位 ∈ (9,0x15]（cmp $0x15;jg 允许 ==0x15）时改写背包物品耐久并回发更新包。
void CEnduranceCmd::execute()
{
    CUserCharacInfo* ci = GetUser();
    CUser* user = static_cast<CUser*>(ci);
    if (!user->isGMUser())
    {
        return;
    }
    int slot = m_slot;
    if (!(slot > 9 && slot <= 0x15))
    {
        return;
    }

    Inven_Item* item =
        const_cast<CInventory*>(ci->getCurCharacInvenW())->GetInvenRef(0, slot);
    if (item == 0 || item->m_addInfo == 0)
    {
        return;
    }

    item->m_fieldb = m_endurance;

    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(3);
    packet.put_short(1);
    ci->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_INVEN, slot, packet);
    packet.finalize(true);
    user->Send(packet);
}

// ========================================================================
// CGrowhWeaponEvent —— sizeof=0x10；+0x08 byte "gw" 重置标记、+0x0c int 时计值
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a88e6。

// ORIG parse 0x084b28d6：args=="gw" 时置重置标记，否则标记清零并 atoi 存时计值。
void CGrowhWeaponEvent::parse()
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "gw")
    {
        m_resetFlag = 1;
    }
    else
    {
        m_resetFlag = 0;
        m_timepiece = atoi(m_parser->m_args.c_str());
    }
}

// ORIG execute 0x084b293c：改写成长武器事件数据并落库——
//   标记置位：整体清零事件数据 + DB_EventGrowthWeaponReset(accId)；
//   否则：写时计值/清计数 + DB_EventUpdateGrowthWeaponTimepiece(accId, value, 0)。
void CGrowhWeaponEvent::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    GrowthWeaponEvent* data =
        static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata());
    unsigned char resetFlag = m_resetFlag;
    if (resetFlag != 0)
    {
        data->m_field8 = 0;
        data->m_field4 = 0;
        data->m_eventFlag = 0;
        data->m_field10 = 0;
        data->m_fieldc = 0;
        DB_EventGrowthWeaponReset::makeRequest(user->get_acc_id());
    }
    else
    {
        int timepiece = m_timepiece;
        data->m_field10 = static_cast<short>(timepiece);
        data->m_fieldc = 0;
        DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
            user->get_acc_id(),
            static_cast<short>((unsigned short)data->m_field10), 0);
    }
}

// ========================================================================
// CGuildDebug —— sizeof=0x14；+0x08/+0x0c int 参数、+0x10 unsigned int（m_f1 占位）。
// ========================================================================

// ORIG ctor 0x082a8592。

// ORIG parse 0x084ac720：atoi(args) → +0x08；+0x0c、+0x10 显式清零。
void CGuildDebug::parse()
{
    trim(m_parser->m_args);
    m_param1 = atoi(m_parser->m_args.c_str());
    m_param2 = 0;
    m_f1 = 0;  // +0x10（ORIG 无其它写点，恒为 0）
}

// ORIG execute 0x084ac76c：组 Packet_Guild_Debug 经 guild 代理 SendTcpPacket
//（ORIG 无 isGMUser 校验）。size 取包内 0x1e。
void CGuildDebug::execute()
{
    CUser* user = GetUser();
    Packet_Guild_Debug packet;
    packet.m_charNo = static_cast<unsigned int>(user->get_charac_no(-1));
    packet.m_guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    packet.m_mode = m_param1;
    packet.m_field16 = m_param2;
    packet.m_field1a = m_f1;  // +0x10

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x1e);
}

// ========================================================================
// CGuildExpBookDelete —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8506。

// ORIG execute 0x084abf2c：GetUser()->deleteGuildExpBook()。
void CGuildExpBookDelete::execute()
{
    CUser* user = GetUser();
    user->deleteGuildExpBook();   // 真实定义 UserPartyLink4.cpp（T 0x0867e898）
}

} // namespace game_master