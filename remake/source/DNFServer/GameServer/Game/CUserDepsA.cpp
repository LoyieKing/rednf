// ============================================================================
// CUserDepsA.cpp —— CUser 第一组恢复（独占 TU；不触碰 CUser.cpp）。
//
// 六个方法均逐指令对照 docs/class_func_reports/CUser/<方法>.md 的
// ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   1. get_character_job        ORIG T 0x0864e024 _ZNK5CUser17get_character_jobEj
//   2. CheckQuestMap            ORIG T 0x0866cac6 _ZN5CUser13CheckQuestMapEi
//   3. IsCompleteGuildFund      ORIG T 0x0866c1ca _ZN5CUser19IsCompleteGuildFundEv
//   4. LoadCharacTypeHotKey     ORIG T 0x08692d2a _ZN5CUser20LoadCharacTypeHotKeyEii
//   5. SendNotiPacketMessage    ORIG T 0x086886ce _ZN5CUser21SendNotiPacketMessageEPKc17ENUM_MESSAGE_TYPE
//   6. change_equip             ORIG T 0x0865eed2 _ZN5CUser12change_equipEisi
// ============================================================================

#include <stddef.h>
#include <string.h>

#include "CUser.h"                    // CUser/UserQuest/_Charac_info/STGuildDBInfoOnly/
                                      // ENUM_MESSAGE_TYPE/INVEN_TYPE/CCharacterView +
                                      // online_preliminary::COnlinePreliminary（经 CUserCharacInfo.h）
#include "CInventory.h"               // GetInvenRef(int,int) const / ChangeEquip(INVEN_TYPE,int,int)
#include "Inven_Item.h"               // Inven_Item 完整类型（m_addInfo +0x02）
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "CGameOption.h"              // get_load_hotkey_flag/get_hotkey_option（CHotkeyOption 0xbc）
#include "GameWorld.h"                // G_GameWorld/IsCheckUnusableEquipmentChannel
#include "CDataManager.h"             // G_CDataManager/CDataManager::find_item(int) const
#include "CItem.h"                    // ENUM_WORLD_TYPE/CItem::IsEnableWorld(ENUM_WORLD_TYPE)
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString(int,const char*,bool*)
#include "AvatarRechargeServer.h"     // ARAD::Singleton<AvatarRechargeServer>::Get +
                                      // CalcDurabilityChangeEquip(CUser*,int)
#include "DB_LoadCharacHotKeyOption.h"

// ----------------------------------------------------------------------------
// 1. ORIG T 0x0864e024 sz 0x91。
// 遍历 m_characList（+0x796e8 vector<_Charac_info>），首个 m_characNo（+0x00，
// 按 uint 比较）等于 characNo 的元素返回其 +0x26 字节（job，movsbl 符号扩展）；
// 未命中返回 -1。
// ----------------------------------------------------------------------------
int CUser::get_character_job(unsigned int characNo) const
{
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator mark = m_characList.end();
    while (it != mark)
    {
        if (static_cast<unsigned int>(it->m_characNo) == characNo)
            return static_cast<int>(it->m_field26);  // +0x26 job（movsbl）
        ++it;
    }
    return -1;
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x0866cac6 sz 0x3d。
// getCurCharacR() 为空返回 0；否则委托当前角色 UserQuest 的
// get_rescue_questmap(dungeonIdx)（ORIG 直调 _ZNK9UserQuest19get_rescue_questmapEi）。
// ----------------------------------------------------------------------------
int CUser::CheckQuestMap(int dungeonIdx)
{
    if (getCurCharacR() == 0)
        return 0;
    return getCurCharacQuestR()->get_rescue_questmap(dungeonIdx);
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0866c1ca sz 0x53。
// m_guildDB.m_fundLevel（+0x8d03b，无符号字节）>0xf 且 m_guildDB.m_fundMoney
// （+0x8d0c0，uint）>=10000000 即完整；否则 fundMoney >=20000000 判完整
// （汇编 jbe $0xf / jbe $0x98967f / jbe $0x1312cff，全部无符号比较）。
// ----------------------------------------------------------------------------
bool CUser::IsCompleteGuildFund()
{
    unsigned int* money = &m_guildDB.m_fundMoney;
    unsigned char* level = &m_guildDB.m_fundLevel;
    if (*level > 15 && *money >= 10000000)
        return true;
    if (*money > 19999999)
        return true;
    return false;
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x08692d2a sz 0x171。
// hotkeyType = GetCharacHotKeyType(characJob)（==10）。装了热键加载开关时：
//   CheckLoadHotKeyCashedCharacType(cashed, job) 通过才发包——
//   PacketGuard: put_header(0,0x1c4)/put_byte(type)/put_int(0xbc)/
//   put_binary(get_hotkey_option(type),0xbc)/finalize(true)，Send 后析构；
// 未开开关：DB_LoadCharacHotKeyOption::makeRequest(GetUID, acc_id, type)。
// ----------------------------------------------------------------------------
void CUser::LoadCharacTypeHotKey(int cashedCharacJob, int characJob)
{
    unsigned char hotkeyType = GetCharacHotKeyType(characJob);

    if (GetGameOptionRef()->get_load_hotkey_flag(hotkeyType))
    {
        if (CheckLoadHotKeyCashedCharacType(cashedCharacJob, characJob))
        {
            PacketGuard packet;
            packet.put_header(0, 0x1c4);
            packet.put_byte(hotkeyType);
            packet.put_int(0xbc);
            packet.put_binary(
                reinterpret_cast<char*>(GetGameOptionRef()->get_hotkey_option(
                    hotkeyType)),
                0xbc);
            packet.finalize(true);
            Send(packet);
        }
    }
    else
    {
        DB_LoadCharacHotKeyOption::makeRequest(GetUID(), get_acc_id(),
                                               GetCharacHotKeyType(characJob));
    }
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x086886ce sz 0xf5。
// 通知消息包：put_header(0,0xc)/put_byte(type)/put_short(0)/put_byte(0)/
// put_int(strlen(msg))/put_str(msg,strlen(msg))/finalize(true)，Send 后析构
// （strlen 与 put_str 均按 ORIG 各调一次）。
// ----------------------------------------------------------------------------
void CUser::SendNotiPacketMessage(const char* msg, ENUM_MESSAGE_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(static_cast<int>(type));
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(strlen(msg)));
    packet.put_str(msg, static_cast<int>(strlen(msg)));
    packet.finalize(true);
    Send(packet);
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x0865eed2 sz 0x30e。space 分派：
//   1 头像域（INVEN_TYPE_AVATAR=2）：频道不可用装备校验失败→msg_146 通知返 1；
//     isCompetitionMercenary→返 0xb3；ChangeEquip(AVATAR,slot,dst)==0 时回写
//     setAvatarTypeBonus(CalcMercenaryAvatarBonus)；>0 直接返；否则
//     AvatarRechargeServer::CalcDurabilityChangeEquip(this,dstSlot)。
//   2 交易域：CheckInTrade()→返 0x13（否则落尾部）。
//   0 装备域（INVEN_TYPE_EQUIP=1）：同款不可用装备校验；
//     getCurChannelEquipslotSwitch()==1 且 dstSlot!=0xb 时 find_item(m_addInfo)
//     非空且 !IsEnableWorld(WORLD_1) → 返 1；ChangeEquip(EQUIP,...)>0 直接返。
//   其它 space 返 1。
// 尾部：0<=dstSlot<0xc 时 m_characterView.enableSaveCharacView()；
// send_equip(dstSlot)；返 0。
// ----------------------------------------------------------------------------
int CUser::change_equip(int space, short slot, int dstSlot)
{
    if (space == 1)
    {
        if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
        {
            const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(2, slot);
            if (!online_preliminary::COnlinePreliminary::CheckUnusableEquipment(
                    item))
            {
                SendNotiPacketMessage(
                    g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                      (bool*)0),
                    (ENUM_MESSAGE_TYPE)0);
                return 1;
            }
        }
        if (isCompetitionMercenary())
            return 0xb3;

        int ret = getCurCharacInvenW()->ChangeEquip(INVEN_TYPE_AVATAR, slot,
                                                    dstSlot);
        if (ret == 0)
            setAvatarTypeBonus(
                static_cast<char>(CalcMercenaryAvatarBonus()));  // movsbl %al
        if (ret > 0)
            return ret;

        ARAD::Singleton<AvatarRechargeServer>::Get()->CalcDurabilityChangeEquip(
            this, dstSlot);
    }
    else if (space == 2)
    {
        if (CheckInTrade())
            return 0x13;
    }
    else if (space != 0)
    {
        return 1;
    }
    else
    {
        if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
        {
            const Inven_Item* item =
                getCurCharacInvenR()->GetInvenRef(1, slot);
            if (!online_preliminary::COnlinePreliminary::CheckUnusableEquipment(
                    item))
            {
                SendNotiPacketMessage(
                    g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                      (bool*)0),
                    (ENUM_MESSAGE_TYPE)0);
                return 1;
            }
        }
        if (getCurChannelEquipslotSwitch() == 1 && dstSlot != 0xb)
        {
            const Inven_Item* item =
                getCurCharacInvenR()->GetInvenRef(1, slot);
            CItem* itemInfo =
                G_CDataManager()->find_item(item->m_addInfo);  // +0x02 item idx
            if (!(itemInfo == 0 ||
                  itemInfo->IsEnableWorld((ENUM_WORLD_TYPE)1)))
                return 1;
        }

        int ret = getCurCharacInvenW()->ChangeEquip(INVEN_TYPE_EQUIP, slot,
                                                    dstSlot);
        if (ret > 0)
            return ret;
    }

    if (dstSlot >= 0 && dstSlot < 12)
        m_characterView.enableSaveCharacView();
    send_equip(dstSlot);
    return 0;
}
