// ============================================================================
// UserCoreDepsB2.cpp —— 用户/安全核心叶子恢复（独占 TU；不触碰 CUser.cpp）。
//
// 主函数均逐指令对照 docs/class_func_reports/<类>/<函数>.md 的
// ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   1. CUser::MakeTagCharacInfo            ORIG T 0x08690776（CUser/MakeTagCharacInfo.md）
//   2. AvatarRechargeServer::CalcDurabilityChangeEquip
//                                          ORIG T 0x081905b4（AvatarRechargeServer/CalcDurabilityChangeEquip.md）
//   3. online_preliminary::COnlinePreliminary::CheckUnusableEquipment
//                                          ORIG T 0x08560920（online_preliminary__COnlinePreliminary/CheckUnusableEquipment.md）
//   4. _checkItemMovable                   ORIG T 0x0865f1e1（_lt_global_gt_/_checkItemMovable.md）
//   5. Secu_HackLogCheck::MoveMap          ORIG T 0x0827a7f4（Secu_HackLogCheck/MoveMap.md）
//   6. CSpecialItemRoutingManager::ProcessFailEtc
//                                          ORIG T 0x0860b332（CSpecialItemRoutingManager/ProcessFailEtc.md）
//   7. CSpecialItemRoutingManager::processRouting
//                                          ORIG T 0x0860ac6a（CSpecialItemRoutingManager/processRouting.md）
//
// 直接依赖且当前无任何定义的 ORIG 叶子（有报告者读报告；无报告者以 objdump
// 反汇编逐指令转写并在注释标注地址；全部符号经 ORIG nm 核实存在）：
//   - Secu_MonsterCheck::Reset                    T 0x0827ac7a（Secu_MonsterCheck/Reset.md）
//   - WongWork::CHackAnalyzer::getServerHackAccumulatedCnt
//                                                 W 0x0822eef6（无报告，objdump）
//   - CParty::IsUseFatigueDungeon                 T 0x085be9c8（无报告，objdump）
//   - CParty::CheckMapClear                       T 0x085bea4a（无报告，objdump；转发 check_grid_clear）
//   - CBattle_Field::check_grid_clear             T 0x0830a0e8（无报告，objdump）
//   - CEquipItem::GetUsableEquipmentType          W 0x0832e036（无报告，objdump；返回 +0x1bc）
//   - _Additioal_info::operator+=                 W 0x08693cea——已收敛至唯一定义
//     CUserPremium.cpp（add/subtract 同处），本 TU 不再重复定义
//
// 注：CUser::Send(PacketGuard&) 已由 CUser.cpp:2361 按报告实现；
//     CUser::getCurCharacInvenCheckSum/getCurCharacSkillCheckSum 已由
//     CUserDepsB.cpp 实现——本 TU 不重复定义。
// ============================================================================

#include <string.h>
#include <stdlib.h>

#include "CUser.h"                    // CUser/_Charac_info/Secu_HackLogCheck/
                                      // Secu_MonsterCheck/WongWork::CHackAnalyzer/
                                      // ENUM_HACKTYPE/WongWork::CUserPremium(m_premium)
#include "CUserCharacInfo.h"          // getCurCharacR/getTagCharacR/getTagCharacSkillW/
                                      // getTagCharacInvenR/getCurCharacInvenW/
                                      // getCurCharacNo/get_charac_10_level_section/
                                      // getCurCharacStrikerSkillIndex/
                                      // online_preliminary::COnlinePreliminary
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "AvatarRechargeServer.h"     // CalcDurabilityChangeEquip/CalcDurabilityDetail 声明
#include "CInventory.h"               // MakeEquipList/GetInvenSlot/insertItemIntoInventory/
                                      // GetInvenTypeFromItemSpace/_checkItemMovable 声明
#include "CExpandEquipslot.h"         // GetCharacExpandDataR(9) 返回类型与 GetInvenSlot
#include "CEquipItem.h"               // CEquipItem 视图（GetUsableEquipmentType/m_field1bc）
#include "CItem.h"                    // is_stackable/get_index/getUsableLevel/GetItemType
#include "CDataManager.h"             // G_CDataManager/find_item/m_serverParameter(+0x68)
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "CCharacter.h"               // _Additioal_info（GetAdditionalInfo 返回值）
#include "CBattle_Field.h"            // check_grid_clear（CheckMapClear 转发目标）
#include "CDungeon.h"                 // isNoFatigueDungeon/m_villageAttackEventFlag/m_bloodDungeonType
#include "CParty.h"                   // 路由 API/IsUseFatigueDungeon/CheckMapClear/
                                      // DeleteDungeonDropItem/send_to_party
#include "CSpecialItemRoutingManager.h"
#include "CBossTower.h"               // WongWork::CBossTower（getBossTower 返回值）
#include "WarField.h"                 // MapInfo::m_itemMap（+0xa18 = CBossStage+0x9f4+0x24）
#include "CGenUniqueNo.h"             // WongWork::CGenUniqueNo::genIPGNo
#include "GameRand.h"                 // get_rand_int
#include "CStatisticServerProxy.h"    // statistc_proxy::add
#include "CSystemTime.h"              // getCurDate
#include "GlobalData.h"               // s_systemTime_
#include "LogManager.h"               // logFormat

// ---------------------------------------------------------------------------
// 1. CUser::MakeTagCharacInfo —— ORIG T 0x08690776（报告 CUser/MakeTagCharacInfo.md）
// tag 角色不存在 → 0；否则写入：uid 低 16 位、名字、等级/job/+0x29、striker 技能
// 序号、附加信息 0x52 字节（premium 时叠加 CUserPremium::GetAdditionalInfo）、
// 装备列表、技能强化循环、求购单包。成功返回 1。
// ---------------------------------------------------------------------------
bool CUser::MakeTagCharacInfo(PacketGuard& packet)
{
    if (getCurCharacR() == 0)
        return false;
    const _Charac_info* tag = getTagCharacR();
    if (tag == 0)
        return false;

    packet.put_short(get_unique_id() & 0xffff);

    int nameLen = strlen(tag->m_name);
    packet.put_int(nameLen & 0xff);
    packet.put_str(tag->m_name, nameLen & 0xff);

    packet.put_byte((int)(short)tag->m_level);      // movswl +0x27 → byte
    packet.put_byte((int)(char)tag->m_field26);     // movsbl +0x26
    packet.put_byte((int)(char)tag->m_field29);     // movsbl +0x29

    packet.put_byte(getCurCharacStrikerSkillIndex() & 0xff);

    char infoBuf[0x52];
    if (!IsHavePremiumAdvantage())
    {
        packet.put_int(0x52);
        packet.put_binary((const char*)getTagCharacAddInfoR(), 0x52);
    }
    else
    {
        memcpy(infoBuf, getTagCharacAddInfoR(), 0x52);           // 内联 memcpy 0x52
        _Additioal_info additional = m_premium.GetAdditionalInfo();  // sret @-0x7b
        *(_Additioal_info*)infoBuf += additional;                // ORIG 0x08693cea
        packet.put_int(0x52);
        packet.put_binary(infoBuf, 0x52);
    }

    // ORIG：getTagCharacInvenR() 返回 CInventory*（_Charac_info.m_inven 同址；
    // 本源码声明返回 Inven_Item*，沿用 CUser.cpp:7541 的同址转换惯例）。
    const CInventory* inven =
        reinterpret_cast<const CInventory*>(getTagCharacInvenR());
    inven->MakeEquipList(&packet, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);

    int idxPos = packet.get_index();   // 占位字节位置（回填强化条目数）
    int count = 0;
    packet.put_byte(0);
    for (int i = 0; i <= 0xcb; ++i)    // cmp $0xcb; setle
    {
        // 每轮重取 tagR（ORIG 循环体内重复调用 getTagCharacR）。
        const _Charac_info* tagLoop = getTagCharacR();
        bool baseSmall =
            ((char)tagLoop->m_field1239 == (char)-1) || (tagLoop->m_field1239 == 0);
        char* skillW = getTagCharacSkillW();
        char* base = skillW + (baseSmall ? 0x46 : 0x1de);
        if (*(unsigned char*)(base + i * 2) != 0)
        {
            packet.put_byte(i);
            packet.put_byte(*(unsigned char*)(base + i * 2));
            packet.put_byte((int)*(char*)(base + i * 2 + 1));
            ++count;
        }
    }
    packet.put_byte(idxPos, count);    // put_byte(int&, int) 回填占位

    CQuestShop* questShop =
        (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    questShop->makePacket_BuyingStatusList(packet, true);
    return true;
}

// ---------------------------------------------------------------------------
// 2. AvatarRechargeServer::CalcDurabilityChangeEquip —— ORIG T 0x081905b4
// （报告 AvatarRechargeServer/CalcDurabilityChangeEquip.md）
// 状态 5 且组队中、疲劳副本、未清图时，槽位 0..9 执行 CalcDurabilityDetail。
// ---------------------------------------------------------------------------
void AvatarRechargeServer::CalcDurabilityChangeEquip(CUser* user, int equipSlot)
{
    if (user->get_state() != 5)
        return;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return;
    if (!party->IsUseFatigueDungeon())
        return;
    if (party->CheckMapClear())
        return;

    std::vector<int> rates;            // ORIG 栈 vector（空构造即传入）
    if (-1 < equipSlot && equipSlot < 10)
        CalcDurabilityDetail(user, equipSlot, rates);
}

// ---------------------------------------------------------------------------
// 4. _checkItemMovable —— ORIG T 0x0865f1e1（报告 _lt_global_gt_/_checkItemMovable.md）
// exchange_interspace(_expand) 前置校验：源/目标空间为 2 或 0xc 直接放行；
// 否则按 INVEN_TYPE 取槽内 item idx（5=扩充栏 9 号页 / 6=金库期望清零 /
// 其余走背包），两侧 idx 均与期望一致才允许移动。
// ---------------------------------------------------------------------------
bool _checkItemMovable(const CUser* user, ENUM_ITEMSPACE fromSpace, int fromSlot,
                       unsigned long fromItemIdx, ENUM_ITEMSPACE toSpace, int toSlot,
                       unsigned long toItemIdx)
{
    if (fromSpace == (ENUM_ITEMSPACE)2 || toSpace == (ENUM_ITEMSPACE)2)
        return true;
    if (fromSpace == (ENUM_ITEMSPACE)0xc || toSpace == (ENUM_ITEMSPACE)0xc)
        return true;

    int fromType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)fromSpace);
    int toType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)toSpace);

    int foundFrom = 0;
    if (fromType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)fromType, fromSlot);
        foundFrom = slot.m_addInfo;                     // 读 sret 结果 +2
    }
    else if (fromType == 6)
    {
        fromItemIdx = 0;                                // ORIG 清参数槽 0x14(%ebp)
    }
    else
    {
        const CInventory* inven = user->getCurCharacInvenR();
        Inven_Item slot = inven->GetInvenSlot(fromType, fromSlot);
        foundFrom = slot.m_addInfo;
    }

    int foundTo = 0;
    if (toType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)toType, toSlot);
        foundTo = slot.m_addInfo;
    }
    else if (toType == 6)
    {
        toItemIdx = 0;                                  // ORIG 清参数槽 0x20(%ebp)
    }
    else
    {
        const CInventory* inven = user->getCurCharacInvenR();
        Inven_Item slot = inven->GetInvenSlot(toType, toSlot);
        foundTo = slot.m_addInfo;
    }

    return foundFrom == (int)fromItemIdx && foundTo == (int)toItemIdx;
}

// ---------------------------------------------------------------------------
// 5a. Secu_HackLogCheck::MoveMap —— ORIG T 0x0827a7f4
// （报告 Secu_HackLogCheck/MoveMap.md）：三检查使能位置 1 后调基类 Reset。
// ---------------------------------------------------------------------------
void Secu_HackLogCheck::MoveMap()
{
    m_flag223 = 1;
    m_flag224 = 1;
    m_flag225 = 1;
    Reset();
}

// ---------------------------------------------------------------------------
// 5b. Secu_MonsterCheck::Reset —— ORIG T 0x0827ac7a
// （报告 Secu_MonsterCheck/Reset.md）：各检查 word==1 且用户在册时上报
// hacktype 0xdf/0xe0/0xe1 各 1 次，随后清三个 active 字节。
// ---------------------------------------------------------------------------
void Secu_MonsterCheck::Reset()
{
    if (m_field10 == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xdf, 1, 0, 0);
    if (m_field1a == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xe0, 1, 0, 0);
    if (m_field26 == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xe1, 1, 0, 0);
    m_active223 = 0;
    m_active224 = 0;
    m_active225 = 0;
}

// ---------------------------------------------------------------------------
// 6. CSpecialItemRoutingManager::ProcessFailEtc —— ORIG T 0x0860b332
// （报告 CSpecialItemRoutingManager/ProcessFailEtc.md）：路由开启且首个有效
// 成员状态 ∈ {5, 0xc} 时广播 0x11a 失败包（byte 0/失败码/路由 uniqueId）、
// 取消计时器并结束路由。
// [b22判词·N-EH布局] orig87/our86 唯一真差：ORIG 编译器在 SetEndRouting 之后为成功路径
// 生成「call ~PacketGuard; jmp <epilogue>」，故 _Unwind_Resume 前多一条独立 nop（87 条）；
// 4.4.7 让成功路径 jmp 到共享 epilogue、~PacketGuard 只落 epilogue 一次（86 条）。函数体
// 主调用序列逐条同 ORIG：IsRoutingState → GetFirstValidMemberState×2（5/0xc 短路，布尔留
// %al 无槽）→ PacketGuard::C1 → put_header(0,0x11a) → put_byte(0) → put_byte(failCode)
// → put_int(GetRoutingItemUniqueID) → finalize(true) → send_to_party → cancelTimer →
// SetEndRouting → ~PacketGuard（+EH 岛 _Unwind_Resume）。P167 实测 16=16 call 集归零。语义等价。
// ---------------------------------------------------------------------------
void CSpecialItemRoutingManager::ProcessFailEtc(CParty* party, int failCode)
{
    if (party != 0)
    {
        // ORIG（idx102 报告）：IsRoutingState 为假则跳过状态读取；5 或 0xc 任一
        // 命中即可路由。布尔全程寄存器（test %al,%al; je），故谓词内联不具名局部
        // （P0 无槽局部）。
        if (party->IsRoutingState() &&
            (party->GetFirstValidMemberState() == 5 ||
             party->GetFirstValidMemberState() == 0xc))
        {
            PacketGuard packet;
            packet.put_header(0, 0x11a);
            packet.put_byte(0);      // ORIG put_byte(local_18,0)
            packet.put_byte(failCode);
            packet.put_int((int)party->GetRoutingItemUniqueID());
            packet.finalize(true);
            party->send_to_party(packet);
            cancelTimer(party);
            party->SetEndRouting();
        }
    }
}

// ---------------------------------------------------------------------------
// 7. CSpecialItemRoutingManager::processRouting —— ORIG T 0x0860ac6a
// （报告 CSpecialItemRoutingManager/processRouting.md）：路由物品投递主流程。
// ---------------------------------------------------------------------------
void CSpecialItemRoutingManager::processRouting(CParty* party)
{
    Inven_Item* routingItem = party->GetRoutingInvenItem();
    if (routingItem->m_addInfo == 0)
    {
        ProcessFailEtc(party, 0);
        return;
    }

    int routingItemId = party->GetRoutingItemID();
    CDataManager* dataMgr = G_CDataManager();
    CItem* item = dataMgr->find_item(routingItemId);
    if (item == 0)
    {
        ProcessFailEtc(party, 0);
        return;
    }

    std::vector<std::pair<int, int> > diceMembers;    // (成员序号, 权重)
    party->GetRoutingDiceMemberList(diceMembers, item);

    CUser* winner = 0;                                // -0x28
    int winnerIdx = 0;                                // -0x24
    PacketGuard packet;
    packet.put_header(0, 0x11a);
    if (diceMembers.empty())
    {
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_int((int)party->GetRoutingItemUniqueID());
    }
    else if (diceMembers.size() == 1)
    {
        winnerIdx = diceMembers[0].first;
        winner = party->get_user(winnerIdx);
    }
    else
    {
        int bestRoll = -1;                            // -0x20
        for (std::vector<std::pair<int, int> >::iterator it = diceMembers.begin();
             it != diceMembers.end();)
        {
            int roll = get_rand_int(100);             // -0x1c
            if (!(roll <= bestRoll))                  // jle 跳过（== 也跳过）
            {
                bestRoll = roll;
                winnerIdx = it->first;
                winner = party->get_user(it->first);
            }
            ++it;                                     // ORIG：roll==best 时也前进
        }
    }

    if (winner != 0)
    {
        char space = 0;                               // -0x18 ENUM_ITEMSPACE 结果
        if (!item->is_stackable())
        {
            if (item->IsCreatureItem())               // vptr+0x14
                space = 7;
            else if (item->IsAvatarItem())            // vptr+0x10
                space = 1;
        }
        else
        {
            if (item->GetItemType() == 9)             // vptr+0x0c
                space = 7;
        }

        int result = -1;                              // -0x14
        if (!item->IsAvatarItem())
        {
            CInventory* inventory = winner->getCurCharacInvenW();
            result = inventory->insertItemIntoInventory(
                *routingItem, (eItemAddReason)4, true, true);
        }
        else
        {
            char ipgAgency[0x18];                     // genIPGNo 输出缓冲（Ghidra local_80[24]）
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                (unsigned int)winner->getCurCharacNo(), ipgAgency);
            result = winner->addAvatarItem(
                (unsigned long)routingItem->m_addInfo, 0,
                (char)*(short*)((const char*)routingItem + 0xb), ipgAgency,
                (eAvatarItemAddReason)0xf);
        }

        if (result == -1)
        {
            packet.put_byte(0);
            packet.put_byte(1);
            packet.put_int((int)party->GetRoutingItemUniqueID());
        }
        else
        {
            if (item->IsRandomOption())               // vptr+0x4c
            {
                char* curDate = GlobalData::s_systemTime_.getCurDate();
                int levelSection = winner->get_charac_10_level_section();
                statistc_proxy::add(winner, "log_random_option", "pick_cnt", 1,
                                    "%d,'%s'", levelSection, curDate);
            }
            unsigned int routingUniqueId = party->GetRoutingItemUniqueID(); // -0x80
            int deleteResult = 0;                     // -0x10
            int state = winner->get_state();
            if (state == 5)
            {
                deleteResult = party->DeleteDungeonDropItem((int)routingUniqueId);
            }
            else if (state == 0xc)
            {
                WongWork::CBossTower* tower = winner->getBossTower();
                if (tower != 0)
                {
                    // m_stageData.m_itemMap 位于 CBossTower+0xa18
                    // （CBossStage+0x9f4 的 MapInfo +0x24）。CBossStage 后段字段
                    // 为 private 且无具名访问器，按既有 TU 惯例以地址访问。
                    std::map<int, map_item>* itemMap =
                        (std::map<int, map_item>*)((char*)tower + 0xa18);
                    itemMap->erase((int)routingUniqueId);
                }
            }
            if (0 < deleteResult)
            {
                LogManager::logFormat(
                    1, "SpecialItemRoutingManager.cpp",
                    __PRETTY_FUNCTION__,
                    0x165, "[EPIC ROUTING] DELETE DUNGEON ITEM FAIL");
            }

            packet.put_byte(1);
            packet.put_byte(winnerIdx);
            packet.put_int((int)routingUniqueId);
            packet.put_byte((int)(char)space);
            packet.put_short(result);
            winner->SendUpdateItem((CUser::eSendTarget)1,
                                   (ENUM_ITEMSPACE)(int)space, result);
        }
    }

    packet.finalize(true);
    party->send_to_party(packet);
    party->SetEndRouting();
}

// ---------------------------------------------------------------------------
// 依赖叶子（无报告，objdump 直转；地址见各注释）。
// ---------------------------------------------------------------------------

// ORIG T 0x085be9c8（无报告，objdump）：+0xcd8==1 / 无副本(+0xcac) / m_bloodDungeonType!=0 /
// m_villageAttackEventFlag==1 / isNoFatigueDungeon 任一命中 → false。ORIG 为 && 短链形态。
bool CParty::IsUseFatigueDungeon()
{
    return m_battleField.m_bloodState != 1 && m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
           m_battleField.m_pDungeon->m_villageAttackEventFlag != 1 && !m_battleField.m_pDungeon->isNoFatigueDungeon();
}

// ORIG T 0x085bea4a（无报告，objdump）：al 直接透传 check_grid_clear。
bool CParty::CheckMapClear()
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->check_grid_clear();
}

// ORIG T 0x0830a0e8（无报告，objdump）：遍历当前图怪物表，存在 dropCnt==100
// 且 +0x13 标志为 0 的怪 → false，全部通过 → true。
bool CBattle_Field::check_grid_clear()
{
    MapInfo* mapInfo = GetCurrentMapInfo();
    std::map<int, map_monster>& monsters = mapInfo->m_monsterMap;  // +0xc
    for (std::map<int, map_monster>::iterator it = monsters.begin();
         it != monsters.end(); ++it)
    {
        map_monster mob = it->second;                 // operator= 拷贝
        if (mob.m_dropCnt == 0x64 && mob.m_field13 == 0)
            return false;
    }
    return true;
}

// ORIG W 0x0832e036（无报告，objdump）：返回 this+0x1bc。
int CEquipItem::GetUsableEquipmentType() const
{
    return m_field1bc;
}

// ORIG W 0x0822eef6（无报告，objdump）：
//   mov 0xc(%ebp),%eax ; lea -0x65(%eax),%edx ; add $0xf44,%edx
//   mov 0x8(%eax,%edx,4),%eax
// 即 B 表累计计数（与 CHackAnalyzer.cpp hackCntB 寻址一致；
// resetServerHackAccumulatedCnt 0x086950b6 清同一槽位）。按 ORIG 运算次序
// 书写以对齐指令选择。
int WongWork::CHackAnalyzer::getServerHackAccumulatedCnt(
    WongWork::ENUM_HACKTYPE type) const
{
    int slot = (int)type - 0x65;
    slot += 0xf44;
    return *(const int*)((const char*)this + 8 + slot * 4);
}

// ---------------------------------------------------------------------------
// Secu_HackLogCheck::Init —— ORIG T 0x0827a7d0：写基类 +0x34 = user，
// 再调基类 SetUserPtr(user)。Secu_MonsterCheck::SetUserPtr —— ORIG T
// 0x0827abec：m_user = user。
// ---------------------------------------------------------------------------
void Secu_HackLogCheck::Init(CUser* user)
{
    m_user34 = user;
    SetUserPtr(user);
}

void Secu_MonsterCheck::SetUserPtr(CUser* user)
{
    m_user = user;
}
