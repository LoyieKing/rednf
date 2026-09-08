// ============================================================================
// ExpertJobOnlyEnchanter.cpp —— expert_job::CEnchanter 附魔师 15 符号独占 TU。
#include "InstanceRentalSystem.h"   // [EJEnchanter] 先行引入带 CheckRentableItem(int,uint) 的版本
//（该成员声明为 decl 追加项；ORIG T 0x082702dc）。
// 来源：docs/class_func_reports/expert_job__CEnchanter/ 全部报告逐份转写；
// IsUseOtherUserMonsterCard 无独立报告，按 objdump 反汇编转写并标注【objdump】。
// 布局依据 CExpertJob.h 现有 CEnchanter 字段：
//   +0x08/+0x18 ExpertJobMaterial m_materials[2]（步长 0x10）
//   +0x28 m_user +0x2c m_peer +0x30 uid +0x34 peerUid
//   +0x38 m_field38（附魔费用）+0x3c m_field3c（已登记怪物卡物品，
//   = m_materials[1].m_item）+0x40 m_field40（附魔配方/目标物品）+0x44 m_peerRequestType
// 头部声明追加需求见 /tmp/df_batches/CreatureJob_decl_EJEnchanter.txt。
// ============================================================================
#include "CUser.h"                 // ENUM_PEER_REQUEST_TYPE（先于本头）
#include "CExpertJob.h"
#include "PacketGuard.h"        // PacketGuard : InterfacePacketBuf（put_header/put_byte…）
#include "CItem.h"              // get_index / is_stackable / GetItemType / getUsableItemType
#include "CUser.h"              // Send/SendCmdErrorPacket/get_unique_id/get_acc_name/m_historyLog
#include "CInventory.h"         // GetInvenRef/GetInvenData/MakeItemPacket/gain_money/use_money/delete_item/get_money + GetInvenTypeFromItemSpace
#include "Inven_Item.h"         // Inven_Item（ctor/m_addInfo/m_fieldd）
#include "CDataManager.h"       // G_CDataManager() / find_item(int)
#include "GlobalData.h"         // GlobalData::s_rentalSystem
#include "InstanceRentalSystem.h"
#include "CUserProc.h"          // APSystem::CUserProc::ClearActionAndSendtoUser

namespace expert_job
{

// ---------------------------------------------------------------------------
// IsUseOtherUseMonsterCard（ORIG W 0x849f324 sz 0x27）【objdump】：
// 读 m_peerRequestType(+0x44)，==7 或 ==8 时返真。
// ---------------------------------------------------------------------------
bool CEnchanter::IsUseOtherUserMonsterCard()
{
    return (m_peerRequestType == 7) || (m_peerRequestType == 8);
}

// ---------------------------------------------------------------------------
// IsRightMaterial（报告 IsRightMaterial.md，ORIG T 0x849c9b2 sz 0x53）：
// 两槽依次 IsRight()，首个非 0 值即返回；全 0 返 0。
// ---------------------------------------------------------------------------
int CEnchanter::IsRightMaterial()
{
    for (int i = 0; i <= 1; ++i)
    {
        int right = 0;
        if ((right = m_materials[i].IsRight()) != 0)
            return right;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// IsRecipeRight（报告 IsRecipeRight.md，ORIG T 0x849ca06 sz 0x63）：
// m_field40 非空、非 stackable、虚槽 vtable+0xc（GetItemType）== 3 才为真。
// ---------------------------------------------------------------------------
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
bool CEnchanter::IsRecipeRight()
{
    if (reinterpret_cast<CItem*>(m_field40) == 0)
        return 0;
    if (!reinterpret_cast<CItem*>(m_field40)->is_stackable())
        return 0;
    if (reinterpret_cast<CItem*>(m_field40)->GetItemType() != 3)
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// IsEqualMonsterCad（报告 IsEqualMonsterCad.md，ORIG T 0x849ca6a sz 0x5b）：
// FindMonsterCard(m_field40) 结果的 get_index 与 m_field3c 的 get_index 相等。
// ---------------------------------------------------------------------------
bool CEnchanter::IsEqualMonsterCad()
{
    CItem* card = FindMonsterCard(reinterpret_cast<CItem*>(m_field40));
    if (card == 0)
        return 0;
    if (card->get_index() != reinterpret_cast<CItem*>(m_field3c)->get_index())
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// _DrawEnchant（报告 _DrawEnchant.md，ORIG T 0x849cac6 sz 0x65）：
// IsSuccess 判定，成功则把结果经验并入当前职业经验。
// ---------------------------------------------------------------------------
bool CEnchanter::_DrawEnchant()
{
    int resultExp = 0;
    if (IsSuccess(m_user,
                  static_cast<CStackableItem const*>(
                      reinterpret_cast<CStackableItem*>(m_field40)),
                  resultExp))
    {
        IncreaseExpertJobExp(m_user, m_user->GetCurCharacExpertJobExp() +
                                         resultExp);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// _TryEnchantMine（报告 _TryEnchantMine.md，ORIG T 0x849d7b6 sz 0x287）
// ---------------------------------------------------------------------------
int CEnchanter::_TryEnchantMine(CUser* user, stItemPos_t* pos)
{
    int ret = 0;
    if ((ret = m_materials[0].SetData(user, (unsigned short)pos->m_itemSpace,
                                      pos->m_slot)) != 0)
        return ret;

    CItem* card = FindMonsterCard(reinterpret_cast<CItem*>(m_field40));
    if (card == 0)
        return 0x11;

    Inven_Item invenItem;
    int slot = -1;
    if ((slot = user->getCurCharacInvenR()->GetInvenData(card->get_index(),
                                                         invenItem)) == -1)
        return 0x11;

    if ((ret = m_materials[1].SetData(user, 0, (unsigned short)slot)) != 0)
        return ret;

    if (!IsRecipeRight())
        return 0x11;

    if (!IsLearnRecipe(
            user, static_cast<CStackableItem const*>(
                      reinterpret_cast<CStackableItem*>(m_field40))))
        return 0x11;

    if ((ret = m_materials[0].CheckType(false, 0)) != 0)
        return ret;

    if ((ret = m_materials[1].CheckType(true, 2)) != 0)
        return ret;

    m_field3c = m_materials[1].m_item;
    if (reinterpret_cast<CItem*>(m_field3c) == 0)
        return 0x11;

    if (!IsEqualMonsterCad())
        return 0x11;

    if (!::EnableWorld(reinterpret_cast<CItem*>(m_materials[0].m_item),
                       reinterpret_cast<CItem*>(m_field40)))
        return 0x11;

    // ORIG 虚链：target 卡对象 vtable+0x20 槽（getUsableItemType(int)），实参取
    // 材料 0 物品 vtable+0xc 槽（GetItemType()）返回值。
    if (!reinterpret_cast<CItem*>(m_field3c)->getUsableItemType(
            reinterpret_cast<CItem*>(m_materials[0].m_item)->GetItemType()))
        return 0x17;

    if ((ret = _DoEnchant(ENUM_TRY_ENCHANT_TYPE_0)) != 0)
        return ret;
    return 0;
}

// ---------------------------------------------------------------------------
// _TryEnchantOther（报告 _TryEnchantOther.md，ORIG T 0x849da3e sz 0x285）
// ---------------------------------------------------------------------------
// b28c-1 N7: _TryEnchantOther 虚调用 callee %eax↔%edx 互换。
int CEnchanter::_TryEnchantOther(CUser* user, stItemPos_t* pos)
{
    (void)pos;   // ORIG 形参 pos 未被函数体消费（Ghidra 同）

    if (m_materials[0].Empty())
        return 0x15;

    if (IsRecipeRight() != 1)
        return 0x11;

    if (IsLearnRecipe(user,
                      static_cast<CStackableItem const*>(
                          reinterpret_cast<CStackableItem*>(m_field40))) != true)
        return 0x11;

    if (m_materials[1].Empty())
        return 0x14;

    m_field3c = m_materials[1].m_item;
    if (reinterpret_cast<CItem*>(m_field3c) == 0)
        return 0x11;

    if (IsEqualMonsterCad() != 1)
        return 0x11;

    if (!::EnableWorld(reinterpret_cast<CItem*>(m_materials[0].m_item),
                     reinterpret_cast<CItem*>(m_field40)))
        return 0x11;

    // 与 _TryEnchantMine 相同的双虚槽链（getUsableItemType(GetItemType())）。
    if (!reinterpret_cast<CItem*>(m_field3c)->getUsableItemType(
            reinterpret_cast<CItem*>(m_materials[0].m_item)->GetItemType()))
        return 0x17;

    int variation = 0;
    stCompoundRate* rate = GetSuccessRate(user,
                                          static_cast<CStackableItem const*>(
                                              reinterpret_cast<CStackableItem*>(m_field40)),
                                          variation);
    if (rate == 0)
        return 0x17;

    int percent = rate->m_successRate + variation;
    if (percent > 100)
        percent = 100;

    if (m_peer != 0 && m_peer->getCurCharacR() != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x9c);
        packet.put_byte(1);
        packet.put_short(percent);
        packet.put_byte(1);
        packet.put_int(reinterpret_cast<CItem*>(m_field40)->get_index());
        packet.finalize(true);
        m_peer->Send(packet);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// _PutItemForEnchant（报告 _PutItemForEnchant.md，ORIG T 0x849d344 sz 0x423）
// kind：0=登记费用、1=主槽材料、2=对家槽怪物卡。money 仅 kind==0 使用。
// ---------------------------------------------------------------------------
int CEnchanter::_PutItemForEnchant(CUser* user, int kind, stItemPos_t* pos,
                                   int money)
{
    if (user->getCurCharacR() == 0)
        return 3;

    // ORIG 全部 unique_id 比较为 16 位（cmp %ax）；allowed 不落栈（寄存器物化）
    if ((unsigned short)user->get_unique_id() !=
        (unsigned short)m_peer->get_unique_id())
    {
        bool allowed =
            (kind == 2 && IsUseOtherUserMonsterCard() != true);  // ORIG bVar1
        if (!allowed)
            return 3;
        if ((unsigned short)user->get_unique_id() !=
            (unsigned short)m_user->get_unique_id())
            return 3;
    }

    // ORIG kind 分发：mov type→eax + cmp $1/je、cmp $2/je、test/jne → switch
    // （叶块排放序 0、1、2，P46）
    switch (kind)
    {
    case 0:
        if (money < 0)
            return 0x5f;
        if (m_field38 > 0)
            return 0x14;
        if (!m_user->CheckMoney(money))
            return 0x5f;
        if (user->getCurCharacInvenR()->get_money() < money)
            return 0x16;
        m_field38 = money;
        break;
    case 1:
        if (!m_materials[0].Empty())
            return 0x14;
        {
            int err = 0;
            if ((err = m_materials[0].SetData(user, pos->m_itemSpace,
                                              pos->m_slot)) != 0)
                return err;
            if ((err = m_materials[0].CheckType(false, 0)) != 0)
                return err;
        }
        break;
    case 2:
        if (!m_materials[1].Empty())
            return 0x14;
        {
            int err = 0;
            if ((err = m_materials[1].SetData(user, pos->m_itemSpace,
                                              pos->m_slot)) != 0)
                return err;
            if ((err = m_materials[1].CheckType(true, 2)) != 0)
                return err;
        }
        break;
    default:
        break;
    }

    if (!(m_user != 0 && m_user->getCurCharacR() != 0))
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0x9d);
    packet.put_byte(kind);
    switch (kind)
    {
    case 0:
        packet.put_int(m_field38);
        break;
    case 1:
        packet.put_int((int)m_materials[0].m_itemIndex);
        break;
    case 2:
        packet.put_int((int)m_materials[1].m_itemIndex);
        break;
    default:
        break;
    }
    packet.finalize(true);

    register int result = 0;     // ORIG unaff_EBX（先声明 → ebx）
    register int notified = 0;   // ORIG esi（后声明 → esi）
    if ((unsigned short)user->get_unique_id() ==
        (unsigned short)m_user->get_unique_id())
    {
        if (m_peer != 0 && m_peer->getCurCharacR() != 0)
        {
            m_peer->Send(packet);
            notified = 1;
        }
        else
        {
            result = 3;
        }
    }
    else
    {
        m_user->Send(packet);
        notified = 1;
    }
    return notified ? 0 : result;
}

// ---------------------------------------------------------------------------
// OnPutItemForEnchant（报告 OnPutItemForEnchant.md，ORIG T 0x849df5e sz 0x6d）
// msg+0xd byte = kind；msg+0xe 同时被解释为 stItemPos_t 内联结构与首 dword。
// ---------------------------------------------------------------------------
void CEnchanter::OnPutItemForEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;
    if ((err = _PutItemForEnchant(
             user, (int)(unsigned char)msg->m_type0d,
             reinterpret_cast<stItemPos_t*>((char*)msg + 0xe),
             msg->m_value0e)) != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xad,
                                 (unsigned char)(err & 0xff));
}

// ---------------------------------------------------------------------------
// OnTryEnchant（报告 OnTryEnchant.md，ORIG T 0x849dfcc sz 0xb1）
// ---------------------------------------------------------------------------
// b28c-1 N18a: 字段装载 4.4 单段直落 %ebx vs ORIG mov mem,%eax; mov %eax,%ebx（P226 实测无效）。
void CEnchanter::OnTryEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;   // ORIG 首条 movl $0,-0xc：err 声明置函数头
    // b28c-1：ORIG `mov 0xe(%eax),%eax; mov %eax,%ebx` 两段升格 vs 4.4 单段
    //  直落 %ebx（N18a 族）；P226 语句表达式实测无效，维持自然形。
    m_field40 = G_CDataManager()->find_item(msg->m_value0e);
    if ((err = _TryEnchant(
             user, reinterpret_cast<stItemPos_t*>((char*)msg + 0x13),
             (ENUM_TRY_ENCHANT_TYPE)(unsigned char)msg->m_type0d)) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac,
                                 (unsigned char)(err & 0xff));
    }
    if (err == 0)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, APSystem::_ActionGroupIndex(0x1f), 0,
            (ENUM_PACKETCLASS)0);
    }
}

// ---------------------------------------------------------------------------
// _AgreeEnchant（报告 _AgreeEnchant.md，ORIG T 0x849dd94 sz 0x1c9）
// ---------------------------------------------------------------------------
// b28c-1 N7: 两处虚调用 callee %eax↔%edx 互换。
int CEnchanter::_AgreeEnchant(CUser* user, ENUM_AGREE_ENCHANT_TYPE type)
{
    int ret = 0;   // -0xc：ORIG movl $0 开头
    if ((unsigned short)user->get_unique_id() !=
        (unsigned short)m_peer->get_unique_id())   // ORIG cmp %ax,%bx（16 位）
        return 3;

    // ORIG 汇编：type 加载一次入 eax 后 cmp $1/je、cmp $2/je、test/jne
    // （线性比较链）→ switch，case 书写序 = 叶块排放序（P46）。
    switch (type)
    {
    case ENUM_AGREE_ENCHANT_TYPE_0:
        if (m_materials[1].Empty())
            return 0x15;
        m_materials[1].Reset();
        m_field40 = 0;

        if (m_user != 0 && m_user->getCurCharacR() != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0xac);
            packet.put_byte(1);
            packet.put_byte(3);
            packet.finalize(true);
            m_user->Send(packet);
        }
        break;
    case ENUM_AGREE_ENCHANT_TYPE_1:
        ret = _DoEnchant(ENUM_TRY_ENCHANT_TYPE_1);
        OnCloseMission(user, ENUM_CLOSE_MISSION_TYPE_0);
        return ret;
    case ENUM_AGREE_ENCHANT_TYPE_2:
        OnCloseMission(user, ENUM_CLOSE_MISSION_TYPE_2);
        break;
    default:
        break;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// OnAgreeEnchant（报告 OnAgreeEnchant.md，ORIG T 0x849e07e sz 0x83）
// ---------------------------------------------------------------------------
void CEnchanter::OnAgreeEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;
    if ((err = _AgreeEnchant(
             user,
             (ENUM_AGREE_ENCHANT_TYPE)(unsigned char)msg->m_type0d)) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xab,
                                 (unsigned char)(err & 0xff));
        if (m_user != 0)
            m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0xab,
                                       (unsigned char)(err & 0xff));
    }
}

// ---------------------------------------------------------------------------
// SetPeerRequestType（ORIG W 0x849f316：写 +0x44）
// 头文件现签名为 (int)；ORIG mangled 为
// _ZN10expert_job10CEnchanter18SetPeerRequestTypeE22ENUM_PEER_REQUEST_TYPE，
// 以枚举重载为准（decl 要求删除旧 (int) 定义于 ExpertJobLink4.cpp:703）。
// ---------------------------------------------------------------------------
void CEnchanter::SetPeerRequestType(ENUM_PEER_REQUEST_TYPE type)
{
    m_peerRequestType = type;
}

// ---------------------------------------------------------------------------
// _DoEnchant（报告 _DoEnchant.md，ORIG T 0x849cb2c sz 0x817）：
// 前置费用门禁 → 材料/配方校验 → _DrawEnchant 结算 → 目标物品 addInfo 写
// 怪物卡索引 → 双向结果包 → 费用转移/材料消耗 → 结算包与历史日志。
// 返回 0=成功，其余为 ORIG 错误码（0x11/0x15/0x14 等）。
// §6.2 巨件核销（b25 复验）：P167 76/76 [0缺0多]；gap 0→0（orig 553 /
// our 553 指令数相等）；残差归因 N82（帧/槽号偏移：ORIG sub $0x60 vs OURS
// +32B 位移）+ 分支极性/寄存器物化控制流形状差（详见
// docs/asm_translation_patterns.md 负例表）。语义完成、byte 层 N 封顶。
// ---------------------------------------------------------------------------
int CEnchanter::_DoEnchant(ENUM_TRY_ENCHANT_TYPE type)
{
    // ---- 门禁块 1：对家有效且双方不同时校验登记费用 ----
    if (m_peer != 0 && m_peer->getCurCharacR() != 0 && m_user != m_peer)
    {
        // ORIG 条件照写：此处复核的是 m_peer 的当前角色（非 m_user）
        if (m_user == 0 || m_peer->getCurCharacR() == 0)
        {
            /* bVar4=false → 落入下方继续 */
        }
        else if (m_user->CheckMoney(m_field38) != 1)
        {
            return 0x11;
        }
        else if (m_peer->getCurCharacInvenR()->get_money() < m_field38)
        {
            return 0x11;
        }
    }

    // ---- 材料槽与配方校验 ----
    int ret = IsRightMaterial();
    if (ret != 0)
        return ret;

    bool useOther = IsUseOtherUserMonsterCard();
    if (!IsExistRecipeItem(
            m_user, reinterpret_cast<CItem*>(m_field40), useOther))
        return 0x15;

    bool successSent = false;
    int fee = 0;                     // -0x1c：materials[0] 物品空间→INVEN_TYPE
    PacketGuard packet;

    CItem* addedCard = 0;            // -0x10：本次写入的怪物卡物品
    int invenType =
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)m_materials[0].m_field0e);
    unsigned short slot0 = m_materials[0].m_slot;

    Inven_Item* ref = ((CUser*)m_materials[0].m_user)->getCurCharacInvenW()
                          ->GetInvenRef(invenType, slot0);
    if (ref == 0)
        return 0x11;

    if (GlobalData::s_rentalSystem->CheckRentableItem(1,
            (unsigned int)ref->m_addInfo))
        return 0x11;
    if (GlobalData::s_rentalSystem->CheckRentableItem(2,
            (unsigned int)ref->m_addInfo))
        return 0x11;

    int itemIndex0 = (int)m_materials[0].m_itemIndex;   // -0x18：历史日志参数
    int oldCardRef = ref->m_fieldd;                     // -0x14：写卡前快照

    if (_DrawEnchant())
    {
        addedCard = reinterpret_cast<CItem*>(m_field3c);
        if (addedCard != 0)
            ref->m_fieldd = addedCard->get_index();

        packet.clear();
        packet.put_header(0, 0xe);
        packet.put_byte((int)m_materials[0].m_field0e);
        packet.put_short(1);

        unsigned short slot0b = m_materials[0].m_slot;
        bool ok = ((CUser*)m_materials[0].m_user)->getCurCharacInvenR()
                      ->MakeItemPacket((INVEN_TYPE)invenType, slot0b, packet);
        if (!ok)
            return 0;   // ORIG：MakeItemPacket 失败直接返 0（packet 析构）

        packet.finalize(true);
        m_peer->Send(packet);
        successSent = true;
    }

    // ---- 统计：卡被消耗计数 ----
    if (addedCard != 0 && m_user != 0)
    {
        int cardIdx = addedCard->get_index();
        int ejType = m_user->GetCurCharacExpertJobType();
        GetInstanceExpertJobStatistic()->increaseItemCount(
            (ENUM_EXPERT_JOB_TYPE)ejType, (unsigned long)cardIdx, 1);
    }

    // ---- 主体验收包 + 配方材料扣除（含 exp/金钱回显）----
    if (m_user != 0 && m_user->getCurCharacR() != 0)
    {
        if (m_field38 > 0)
        {
            m_user->getCurCharacInvenW()->gain_money(m_field38,
                                                     (eMoneyAddReason)0x10, true,
                                                     0);
            m_user->SendUpdateItemList(CUser::eSendTarget_1,
                                       (ENUM_ITEMSPACE)0, 0);
        }

        packet.clear();
        packet.put_header(1, 0xac);
        packet.put_byte(1);
        packet.put_byte((int)type);
        packet.put_byte((int)(unsigned char)successSent);
        packet.put_int(m_user->GetCurCharacExpertJobExp());
        packet.put_int(m_user->getCurCharacInvenR()->get_money());

        useOther = IsUseOtherUserMonsterCard();
        ret = UseMaterialOfRecipe(m_user, reinterpret_cast<CItem*>(m_field40),
                                  &packet, useOther);
        if (ret != 0)
        {
            if (ref != 0)
                ref->m_fieldd = 0;
            return ret;
        }
        packet.finalize(true);
        m_user->Send(packet);
    }

    // ---- 对家结算：扣费/卡片删除/终止包 ----
    if (m_peer != 0 && m_peer->getCurCharacR() != 0 && m_user != m_peer)
    {
        if (m_field38 > 0)
        {
            m_peer->getCurCharacInvenW()->use_money(m_field38,
                                                    (eMoneySubReason)0x17,
                                                    true);
            m_peer->SendUpdateItemList(CUser::eSendTarget_1,
                                       (ENUM_ITEMSPACE)0, 0);
        }

        if (IsUseOtherUserMonsterCard())
        {
            unsigned short slot1 = m_materials[1].m_slot;
            bool delOk = m_peer->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot1, 1, (eItemDelReason)8, true);
            if (!delOk)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x3d9, 5);
                trace("CUser::compound_item, getCurCharacInvenW()->delete_item failed , %d %d",
                      m_peer->getCurCharacNo(), (int)slot1);
            }

            packet.clear();
            packet.put_header(0, 0xe);
            packet.put_byte(0);
            packet.put_short(1);
            m_peer->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot1,
                                                         packet);
            packet.finalize(true);
            m_peer->Send(packet);
        }

        packet.clear();
        packet.put_header(1, 0xab);
        packet.put_byte(1);
        packet.put_byte((int)(unsigned char)successSent);
        packet.finalize(true);
        m_peer->Send(packet);
    }

    // ---- 历史日志（CUser +0x79700 即 cUserHistoryLog 内嵌成员）----
    const char* characName = m_peer->getCurCharacName();
    const char* accName = m_peer->get_acc_name();
    m_peer->m_historyLog.EnchantItem(itemIndex0, oldCardRef, ref->m_fieldd,
                                     successSent, accName, characName);
    return 0;
}

}  // namespace expert_job
