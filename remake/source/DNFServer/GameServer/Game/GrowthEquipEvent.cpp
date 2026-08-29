// df_game_r Game/ GrowthEquipEvent 实现。
// 依据 docs/class_func_reports/GrowthEquipEvent/*.md + ORIG elf 反汇编：
//   ctor 0x080f6ef0：CEventBase() + 本类 vptr（无自有成员）
//   D1 0x080f6f0c / D0 0x080f6f3c：恢复 vptr 后调基类析构
//   StartEvent 0x080f6f5a / StartEvent(Word_Param) 0x080f6fac /
//   EndEvent 0x080f6ffe：SetEventFlag + cMyTrace
//   StartAction 0x080f7710：遍历世界用户表，state>2 者补发活动信息
//   IsEventing 0x080f7820（W）：转发 CEventBase::IsEventing
//   SelectGrowthWeapon 0x080f73be / SendGrowthWeaponEventInfo 0x080f7226
//   GiftGrowthWeaponBox 0x080f7050（礼包邮件 + DB_EventSaveGrowthWeapon）
#include "GrowthEquipEvent.h"

#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"
#include "DB_EventUpdateGrowthWeaponCharac.h"
#include "DB_EventSaveGrowthWeapon.h"
#include "Inven_Item.h"
#include "RDARScriptStringManager.h"
#include "CUserCharacInfo.h"
#include "CMailBoxHelper.h"

#include <cstring>
#include "cMyTraceNoop.h"

// ============================================================================
// GrowthEquipEvent::SelectGrowthWeapon @ 0x080f73be
// 使用成长武器道具（Dispatcher_UseBoosterItem::_onBoosterItemUseResult_OnlyOne）：
// ORIG 守卫比较 CDataManager+0x4bc4 == itemId。该字段即 m_growthWeapon.m_giftBox
// （get_GiftGrowthWeaponBox 0x836561c 读同一偏移，CDataManager.h 布局 +0x4bac+0x18），
// 用具名字段替代裸偏移。
// ============================================================================
void GrowthEquipEvent::SelectGrowthWeapon(CUser& user, int itemId, int addInfo)
{
    // 非本活动道具直接返回（ORIG +0x1b je 跳过全部逻辑）。
    if (G_CDataManager()->m_growthWeapon.m_giftBox != itemId)
        return;

    // 用户活动数据块（CUser::getGrowthWeaponEventdata 返回 void*，内部布局：
    // +0x00 状态字节 / +0x04 已选角色 charac_no / +0x08 已选道具 idx /
    // +0x0c 礼包已领取标志 / +0x10 short 沙漏相关 / +0x12 short serverGroup）。
    char* data = static_cast<char*>(user.getGrowthWeaponEventdata());

    *(int*)(data + 0x04) = user.get_charac_no(-1);      // +0x2c get_charac_no(-1)
    *(int*)(data + 0x08) = addInfo;

    // 渠道组：整合 PvP 基础渠道用用户所属 server group，否则取环境
    // m_serverEnvir.m_serverGroup（+0x378，经 get_server_group()）
    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = G_CEnvironment()->get_server_group();
    else
        serverGroup = (int)user.GetServerGroup();
    *(short*)(data + 0x12) = (short)serverGroup;

    DB_EventUpdateGrowthWeaponCharac::makeRequest(
        user.get_acc_id(), (unsigned int)user.get_charac_no(-1),
        addInfo, (short)serverGroup);

    SendGrowthWeaponEventInfo(user);
}

// ============================================================================
// GrowthEquipEvent::SendGrowthWeaponEventInfo @ 0x080f7226
// 包头 0x1b9：byte 状态（2=礼包已领 / 1=当前角色已选且组一致 / 0=未选）、
// int 已选道具 idx、short +0x10 字段。
// ============================================================================
void GrowthEquipEvent::SendGrowthWeaponEventInfo(CUser& user)
{
    PacketGuard packet;
    packet.put_header(0, 0x1b9);

    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = G_CEnvironment()->get_server_group();
    else
        serverGroup = (int)user.GetServerGroup();

    char* data = static_cast<char*>(user.getGrowthWeaponEventdata());
    if (*(int*)(data + 0x0c) != 0)
    {
        packet.put_byte(2);
    }
    else
    {
        bool selected =
            (user.get_charac_no(-1) == *(int*)(data + 0x04)) &&
            (*(short*)(data + 0x12) == (short)serverGroup);
        packet.put_byte(selected ? 1 : 0);
    }
    packet.put_int(*(int*)(data + 0x08));
    packet.put_short((int)*(short*)(data + 0x10));
    packet.finalize(true);
    user.Send(packet);
}

// ============================================================================
// GrowthEquipEvent::GiftGrowthWeaponBox @ 0x080f7050（const）
// 活动礼包箱领取（每角色一次，data+0x00 状态字节守卫）：按脚本配置的礼包箱
// idx（m_growthWeapon.m_giftBox，ORIG CDataManager+0x4bc4）构造 Inven_Item，
// 经 RDAR 字符串 game_server_msg_277/278 走系统邮件，再发 DB 存档请求并刷新
// 邮件提醒 tooltip。
// ============================================================================
void GrowthEquipEvent::GiftGrowthWeaponBox(CUser& user) const
{
    char* data = static_cast<char*>(user.getGrowthWeaponEventdata());
    if (*data == '\x01')            // 已领取过：直接返回
        return;
    *data = '\x01';

    const int giftBoxIdx = G_CDataManager()->m_growthWeapon.m_giftBox;

    Inven_Item item;                // ORIG Inven_Item::Inven_Item() 0x080cb854
    CItem* itemBase = G_CDataManager()->find_item(giftBoxIdx);

    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = G_CEnvironment()->get_server_group();
    else
        serverGroup = (int)user.GetServerGroup();

    if (itemBase == 0)
        return;

    // vtable+0x08 = CItem::make_item(Inven_Item&) const
    itemBase->make_item(item);
    item.set_add_info(1);           // ORIG Inven_Item::set_add_info(item,1)

    // 邮件正文：RDAR 字符串 game_server_msg_278（strncpy 0xff 忠实保留）。
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(
                4, "game_server_msg_278", static_cast<bool*>(0)),
            0xff);

    if (giftBoxIdx != 0)
    {
        const int titleLen = static_cast<int>(strlen(title));
        const unsigned int characNo =
            static_cast<unsigned int>(user.getCurCharacNo());
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(
                4, "game_server_msg_277", static_cast<bool*>(0)),
            item, 0u, characNo, title, titleLen, 0u,
            static_cast<ENUM_SERVER_GROUP>(serverGroup), false, false);
    }

    DB_EventSaveGrowthWeapon::makeRequest(user.get_acc_id(),
                                          static_cast<short>(serverGroup));
    user.SendEventMailAlaramTooltip();
}

// ============================================================================
// 生命周期（vtable 全覆盖，保证本 TU 发出的 _ZTV16GrowthEquipEvent 可解析）
// ============================================================================
GrowthEquipEvent::GrowthEquipEvent()
{
    // ORIG：CEventBase() 置基类 vptr 与 stADDINFO/+0x08 flag 后写本类 vptr；
    // 无自有成员，编译器生成的基类调用 + vptr 赋值即为忠实还原。
}

GrowthEquipEvent::~GrowthEquipEvent()
{
    // ORIG D1：恢复本类 vptr 后调 CEventBase::~CEventBase()。
}

void GrowthEquipEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x18, 0);
    tr("GrowthEquipEvent Event start!");
}

void GrowthEquipEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("GrowthEquipEvent Event start! [monitor request]");
}

void GrowthEquipEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x24, 0);
    tr("GrowthEquipEvent Event end!");
}

void GrowthEquipEvent::StartAction()
{
    // ORIG：遍历 GameWorld 用户表（map<ushort,CUser*>），对 get_state()>2 的
    // 在线用户逐个补发 SendGrowthWeaponEventInfo。
    GameWorld* world = G_GameWorld();
    for (std::map<unsigned short, CUser*>::const_iterator it =
             world->m_UserInWorld.begin();
         it != world->m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
            SendGrowthWeaponEventInfo(*user);
    }
}

bool GrowthEquipEvent::IsEventing(CUser* user) const
{
    // ORIG W 0x080f7820：尾调 CEventBase::IsEventing（编译器弱拷贝）。
    return CEventBase::IsEventing(user);
}
