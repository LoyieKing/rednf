// ============================================================================
// df_game_r 还原 —— user_creature::CCreatureMgr 收尾批（CreatureOnlyMgrRest）。
// 工作清单 /tmp/only_orig/user_creature.txt 中 _ZN...12CCreatureMgr 27 符号，
// 报告 docs/class_func_reports/user_creature__CCreatureMgr/<Method>.md 全文
// （## 汇编 + ## 反编译 C）逐个转写；报告缺失者 objdump 标【objdump】。
//
// 本批另代管四个被本批方法调用、当前全仓无人提供定义的 user_creature::
// CDBMsgSender W 内联方法（ChangeOwner/ChangeSlotNo/AuctionRegist/Cancel），
// 均按 ORIG 逐指令转写【objdump】；归口收编时请宿主迁移后删除此处定义。
//
// 声明追加需求见 /tmp/df_batches/CreatureJob_decl_UCMgr.txt。
// ============================================================================
#include <string.h>

#include <map>

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CPacketRespondent.h"
#include "SigTypes.h"
#include "CSystemTime.h"
// ============================================================================
// CreatureOnlyMgrRest.cpp —— user_creature::CCreatureMgr worklist 收尾
// （OnlyOrig_CreatureJob 批；SIG_CREATURE_ITEM_DETAIL_INFO 权威定义已提升至
//   CInventory.h 顶部，本 TU 不再携带局部副本。）
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr10SetStomachEi T 0x08339ee0（SetStomach.md）：
// m_equippedCreature 非空 → CCreature::SetStomach(value) 后经 CPacketRespondent
// 广播 MakeNotipacketCreatureState(uid, GetStomach())；返回恒 1。
// ============================================================================

namespace user_creature
{
int user_creature::CCreatureMgr::SetStomach(int stomachValue)
{
    if (m_equippedCreature != 0)
    {
        m_equippedCreature->SetStomach(stomachValue);
        CPacketRespondent pkt(m_user, 0, 0);
        // ORIG 0x08339ee0 求值序：ctor 后先 GetStomach()→%ebx，再
        // CCreatureItem::GetUid() call（+0x69，W 0x833efbe 外联）→参数 (uid,stomach)。
        pkt.MakeNotipacketCreatureState(
            reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid(),
            m_equippedCreature->GetStomach());
        pkt.Send();
    }
    return 1;
}


// ORIG _ZN13user_creature12CCreatureMgr11ChangeOwnerEP10Inven_Itemiiii
// T 0x0833ad34（ChangeOwner.md）：槽位/物品类型过滤后 CDBMsgSender(0x8a,m_user)，
// sender.ChangeOwner(item,characNo,slotFrom,mapSize,param_4,param_5)；
// 成功 → Send() + WriteCreatureLog(3,item idx,addInfo2,1,type,param_5)；
// 失败 → cMyTrace(...,0xc26,5) "cannot write db uid(%d)\n"。过滤分支均返 true。
bool CCreatureMgr::ChangeOwner(Inven_Item* item, int slotFrom, int mapSizeHint,
                               int param_4, int param_5)
{
    if (!IsCreatureEquipmentScope(slotFrom))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    int characNo = m_user->getCurCharacNo();
    // ORIG 实参序：(item, characNo, slotFrom, mapSizeHint, mapSize, p4, p5)；
    // sender 侧 +0x48=characNo、+0x2c=slotFrom、+0x51 低字节=mapSizeHint、
    // +0x68=mapSize、+0x6c=p4、+0x70=p5（汇编 ebp+0x10..0x24 对应）。
    if (!sender.ChangeOwner(item, characNo, slotFrom, mapSizeHint,
                            mapSize, param_4, param_5))
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::ChangeOwner("
            "Inven_Item*, int, int, int, int)", 0xc26, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    const CInventory* inv = m_user->getCurCharacInvenR();
    inv->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, (eItemAddReason)(unsigned char)param_5);
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr11ItemEvoluteEi T 0x0833d6c2（ItemEvolute.md）：
// 装备宠存在则**直通透传** CCreature::ItemEvolute(m_user, itemIdx) 的 %eax
// （call 后无 test/movzbl，直接 jmp 尾 leave;ret），否则尾块 mov $0。
int CCreatureMgr::ItemEvolute(int itemIdx)
{
    if (m_equippedCreature != 0)
        return m_equippedCreature->ItemEvolute(m_user, itemIdx);
    return 0;
}

// ORIG _ZN13user_creature12CCreatureMgr12ChangeSlotNoEP10Inven_Itemii
// T 0x0833aedc（ChangeSlotNo.md）：FindCreatureItem(uid) 为空返 false；先本地
// SetSlotNo(newSlot)，再 CDBMsgSender(0x8a).ChangeSlotNo(item,characNo,...)；
// 成功 Send + WriteCreatureLog(3,item idx,addInfo2,1,type,param_3)；失败 trace 0xc44。
bool CCreatureMgr::ChangeSlotNo(Inven_Item* item, int newSlot, int param_3)
{
    if (!IsCreatureEquipmentScope(newSlot))
        return true;
    if (item->m_field1 != 5)
        return true;

    CCreatureItem* creatureItem =
        FindCreatureItem(static_cast<int>(item->m_addInfo2));
    if (creatureItem == 0)
        return false;

    creatureItem->SetSlotNo(newSlot);   // ORIG T 0x08337050（写 +0x10）

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int characNo = m_user->getCurCharacNo();
    if (!sender.ChangeSlotNo(item, characNo, newSlot, param_3))
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::ChangeSlotNo("
            "Inven_Item*, int, int)", 0xc44, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    const CInventory* inv = m_user->getCurCharacInvenR();
    inv->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, (eItemAddReason)(unsigned char)param_3);
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr12LoadArtifactEiP10Inven_Item
// T 0x0833be7c（LoadArtifact.md）：item 非空转 EquipArtifact(slot,item)，空返 1。
bool CCreatureMgr::LoadArtifact(int slot, Inven_Item* item)
{
    // ORIG 0x0833be8d：`test %al,%al; je <Equip>`（isEmpty 直测不取反，空→mov$1
    // 返 1，非空→EquipArtifact）。`!isEmpty()` 被 4.4 折散 xor$1 物化 → 改正向
    // `if (isEmpty()) return true;`（P22/P194 直测不取反形）。
    if (item->isEmpty())
        return true;
    return EquipArtifact(slot, item);
}

// ORIG _ZN13user_creature12CCreatureMgr13AuctionRegistEP10Inven_Itemii
// T 0x0833b09c（AuctionRegist.md）：sender.AuctionRegist(item,characNo,p2,p3,size)
// 成功 → Send() + DeleteCreatureItem(addInfo2)；失败 trace 0xc5c。
bool CCreatureMgr::AuctionRegist(Inven_Item* item, int param_2, int param_3)
{
    if (!IsCreatureEquipmentScope(param_2))
        return true;
    if (item->m_field1 != 5)
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    int characNo = m_user->getCurCharacNo();
    if (!sender.AuctionRegist(item, characNo, param_2, param_3, mapSize))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xc5c, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    return DeleteCreatureItem(static_cast<int>(item->m_addInfo2),
                              static_cast<int>(item->m_addInfo2));
}

// ORIG _ZN13user_creature12CCreatureMgr16DenySwapArtifactEP10Inven_ItemiiS2_ii
// T 0x0833989c（DenySwapArtifact.md，条件序以汇编为准）：
// 有装备宠：slot 组 (s1,b)/(s3,a) 任一满足 IsArtifactEquipSlot && !b.isEmpty()
// 且装备宠 IsAvailableArtifact(s1) 为假 → deny(1)；否则 0。
// 无装备宠：任一组 IsArtifactEquipSlot && !isEmpty(b/a) → allow(0)；否则 1。
bool CCreatureMgr::DenySwapArtifact(Inven_Item* a, int s1, int s2,
                                    Inven_Item* b, int s3, int s4)
{
    (void)s2;   // ORIG 未读 +0x14
    (void)s4;   // ORIG 未读 +0x20

    if (m_equippedCreature != 0)
    {
        bool ok1 = IsArtifactEquipSlot(s1) && !b->isEmpty();
        if (ok1 && !m_equippedCreature->IsAvailableArtifact(s1))
            return true;
        bool ok2 = IsArtifactEquipSlot(s3) && !a->isEmpty();
        if (ok2 && !m_equippedCreature->IsAvailableArtifact(s1))
            return true;   // ORIG 两处 IsAvailableArtifact 均传 +0x10(s1)
        return false;
    }
    else
    {
        if (IsArtifactEquipSlot(s1) && !b->isEmpty())
            return false;
        if (IsArtifactEquipSlot(s3) && !a->isEmpty())
            return false;
        return true;
    }
}

// ORIG _ZN13user_creature12CCreatureMgr16GetCreatureIndexEv T 0x08338f36
// （GetCreatureIndex.md）：尾调 CCreature::GetCreatureId(W 0x833f0da,+0x48)，
// eax 直通故为 int 返回；ORIG 不判空（忠实转写）。
int CCreatureMgr::GetCreatureIndex()
{
    return m_equippedCreature->GetCreatureId();
}

// ORIG _ZN13user_creature12CCreatureMgr16LoadCreatureItemEP10Inven_ItemP29SIG_...
// T 0x0833b68c（LoadCreatureItem.md）：工厂按 detail type 取项；临时到期换算；
// 清临时租期记录后虚槽+8 分派（蛋→CEgg::LoadCreatureItem / 宠→CCreature::...）；
// 失败回收对象回池；成长宠刷 isMaxLevel 标记。
bool CCreatureMgr::LoadCreatureItem(Inven_Item* item,
                                    SIG_CREATURE_ITEM_DETAIL_INFO* detail)
{
    // [b23S3C] P48 去缓存：ORIG 0x833b7be 第二次独立 call
    // GetInstanceCreatureItemFactory（FreeCreatureItem 前），工厂指针无具名槽
    // ⇒ 两调用点各自直调（缓存局部会少一次 call，p167 差集可见）。
    CCreatureItem* newItem =
        GetInstanceCreatureItemFactory()->AcquireCreatureItem(
            (user_creature::CREATURE_ITME_TYPE)
                (int)(signed char)detail->m_creatureItemType);   // detail+0x24
    if (newItem == 0)
        return false;

    if (m_tempExpireDelta != -1)
    {
        if (m_tempExpireDelta == 0)
        {
            // [b23S3C] ORIG 0x833b6d8：+0x28 写入值取自 this+0x2c（四条形），
            // 非立即数 0 ⇒ 直赋成员。
            detail->m_expireTime = m_tempExpireDelta;              // detail+0x28
        }
        else
        {
            // [b23S3C] P0 律：ORIG `call; mov %eax,-0x1c(%ebp)` 后 `add -0x1c`
            // ⇒ tick 为具名局部（合写表达式则成员先载、无槽，形序不符）。
            unsigned int tick = OS_API::GetDateTimeTick();
            detail->m_expireTime = m_tempExpireDelta + tick;
        }
        setTempExpireTime(-1);
    }
    UnRegistTempPeriod(detail->m_uid);   // ORIG ebp+0x10→detail+0x04

    // [b23S3C] ORIG 0x833b727：`mov (%eax),%eax; add $0x8,%eax; mov (%eax),%edx;
    // …call *%edx` 单次 vtable 槽 +8 间接分派（蛋/宠由对象自身 vptr 决定），
    // 原码无 type==1 判定、无 bool ok 槽（返回寄存器 xor 直测）。本镜像无虚表，
    // 按 §4.5 先例（CCreatureMgr.cpp 0x833f5bc / CParty.cpp 0x85b9ffd 同形）
    // 用 typedef 槽指针直调；实参右起（m_user→detail→item→mgr→newItem）与
    // ORIG 装槽序逐条一致。
    typedef bool (*LoadCreatureItemSlot)(CCreatureItem*, CCreatureMgr*,
                                         Inven_Item*,
                                         SIG_CREATURE_ITEM_DETAIL_INFO*,
                                         CUser*);
    if ((*reinterpret_cast<LoadCreatureItemSlot*>(
              *reinterpret_cast<int*>(newItem) + 8))(
            newItem, this, item, detail, m_user) != 1)
    {
        // [b23S3C] 临时对象直调形：ORIG 0x833b774 起 vararg（item+7→%edi、
        // item+2→%esi、uid→%ebx）在 ctor call 之前求值，trace 对象落 -0x30 匿名槽。
        cMyTrace(__PRETTY_FUNCTION__, 0xd36, 5)(
            "LoadCreatureItem() failed %d,%d,%d\n",
            m_user->get_unique_id() & 0xffff,
            static_cast<int>(item->m_addInfo),
            item->m_addInfo2);
        GetInstanceCreatureItemFactory()->FreeCreatureItem(newItem);
        return false;
    }

    // P194 X1 探针实测：写 `((uchar)flag ^ 1) != 0` 4.4 仍出 `cmp $0x1,%al`
    // （ndiff 不变），xor 形不可源码复现 ⇒ 保留自然 `!= 1` 形（记 N）。
    if (detail->m_growExpFlag != 1 &&          // detail+0x30
        detail->m_creatureItemType == 1)       // detail+0x24
    {
        m_isMaxLevelGrowCreature = detail->m_isMaxLevel;   // detail+0x31
    }
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr16SaveCreatureItemEv T 0x0833bbbe
// （SaveCreatureItem.md，条目寻址以汇编 lea 复核）：
// CDBMsgSender(0x89) 取 SIG_CREATURE_ITEM_LIST(0x4078)；count@+0；
// 遍历宠物：IsSave() 为真 → SaveCreatureItem(item 区 @cnt*0x3d+4（Inven_Item 数组）、
// detail 区 @cnt*0x38+0x21a0、m_user)，detail.m_uid(+4) 非 0 则 count++；
// count==0 → Cancel 否则 Send。
bool CCreatureMgr::SaveCreatureItem()
{
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x89, m_user);
    SIG_CREATURE_ITEM_LIST* list =
        sender.GetInBuffer<SIG_CREATURE_ITEM_LIST>();
    if (list == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xd90, 5);
        trace("cannot write db uid(%d)\n",
              m_user->get_unique_id() & 0xffff);
        return false;
    }

    memset(list, 0, sizeof(SIG_CREATURE_ITEM_LIST));   // 0x4078
    *(int*)(void*)list = 0;                            // count

    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); ++it)
    {
        CCreatureItem* entry = it->second;
        if (entry == 0 || !reinterpret_cast<CCreature*>(entry)->IsSave())
            continue;

        int cnt = *(int*)(void*)list;
        char* base = (char*)(void*)list;
        reinterpret_cast<CCreature*>(entry)->SaveCreatureItem(
            (Inven_Item*)(void*)(base + cnt * 0x3d + 4),               // 条目物品区
            (SIG_CREATURE_ITEM_DETAIL_INFO*)(void*)(base + cnt * 0x38 + 0x21a0),  // 条目详情区
            m_user);
        // detail.m_uid（详情区 +4）非 0 视为写入成功。
        if (*(int*)(base + cnt * 0x38 + 0x21a4) != 0)
            *(int*)(void*)list = cnt + 1;
    }

    if (*(int*)(void*)list == 0)
        sender.Cancel();
    else
        sender.Send();
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr16SetOverSkillTimeERKi T 0x0833c41e
// （SetOverSkillTime.md）：死亡宠免检返 1；匹配装备宠 item 时校验脚本/等级/
// 冷却窗（cur+2 < last+st/1000+ovst/1000 → trace 0xe6c 返 0x13），通过则
// COverSkillTime::SetTime() 刷新；其余路径 0。
int CCreatureMgr::SetOverSkillTime(const int& itemIdx)
{
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsDieCreature())
            return 1;

        if (*(&itemIdx) == m_equippedCreature->m_itemId)   // CCreatureItem 基区 +0x04
        {
            if (!m_equippedCreature->IsCreatureScript())
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xe62, 0);
                trace("SetOverSkillTime::creature_script is null(creature_itemno=%u)",
                      itemIdx);
                return 0x13;
            }

            int learnLevel = m_equippedCreature->GetLearnOverSkillLevel();
            int expLevel = m_equippedCreature->GetExpLevel(
                m_equippedCreature->GetExp());
            if (expLevel < learnLevel)
                return 0x13;

            int lastUsed = *m_equippedCreature->GetOverSkillTime()->GetTime();
            int stMs = m_equippedCreature->GetSkillRecoveryTime();
            int ovstMs = m_equippedCreature->GetOverSkillRecoveryTime();
            int curSec = GlobalData::s_systemTime_.getCurSec();
            if (curSec + 2 < lastUsed + stMs / 1000 + ovstMs / 1000)
            {
                unsigned int accId = m_user->get_acc_id();
                char* idStr = NumberToString(accId, 0);
                int ovstR = m_equippedCreature->GetOverSkillRecoveryTime();
                int stR = m_equippedCreature->GetSkillRecoveryTime();
                curSec = GlobalData::s_systemTime_.getCurSec();
                int last = *m_equippedCreature->GetOverSkillTime()->GetTime();
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xe6c, 0);
                trace("fail:cur_time(%u),last_used_skilltime(%u),interval(%d) "
                      "st(%d),ovst(%d),m_id(%s),ch_no(%u)",
                      (unsigned int)curSec, (unsigned int)last,
                      curSec - last, stR / 1000, ovstR / 1000,
                      idStr,
                      (unsigned int)m_user->getCurCharacNo());
                return 0x13;
            }
            m_equippedCreature->GetOverSkillTime()->SetTime();
        }
    }
    return 0;
}

// ORIG _ZN13user_creature12CCreatureMgr18IsSaveCreatureItemEv T 0x0833ba30
// （IsSaveCreatureItem.md）：map 非空时任一条目 IsSave()（vt+4==+0x40 flag）
// 为真即 1；否则 0。
bool CCreatureMgr::IsSaveCreatureItem()
{
    if ((int)m_creatureItems.size() != 0)
    {
        for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
             it != m_creatureItems.end(); ++it)
        {
            CCreatureItem* entry = it->second;
            if (entry != 0 &&
                reinterpret_cast<CCreature*>(entry)->IsSave())
                return true;
        }
    }
    return false;
}

// ORIG _ZN13user_creature12CCreatureMgr18UnRegistTempPeriodEi T 0x0833d962
// （UnRegistTempPeriod.md）：m_field30.find(uid)!=end 则 erase 并返 true。
bool CCreatureMgr::UnRegistTempPeriod(int uid)
{
    std::map<int, int>::iterator it = m_field30.find(uid);
    if (it != m_field30.end())
    {
        m_field30.erase(uid);
        return true;
    }
    return false;
}

// ORIG _ZN13user_creature12CCreatureMgr20EventEvoluteCreatureEiii T 0x0833da1e
// （EventEvoluteCreature.md）：等级门槛不足 trace 0x10c2；进化成功广播列表包
// MakeNotipacketCreatureItemList(map*,0xee) 后返 1。
bool CCreatureMgr::EventEvoluteCreature(int creatureId, int minLevel, int slot)
{
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->GetLevel() < minLevel)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x10c2, 0);
            trace("event evolute creature error : low creature level.");
        }
        else
        {
            if (m_equippedCreature->EventEvolute(m_user, creatureId, slot))
            {
                CPacketRespondent pkt(m_user, 0, 0);
                pkt.MakeNotipacketCreatureItemList(&m_creatureItems, 0xee);
                return true;
            }
        }
    }
    return false;
}

// ORIG _ZN13user_creature12CCreatureMgr20SendCreatureItemInfoEP17SIG_CREATURE_ITEM
// T 0x0833b498（SendCreatureItemInfo.md）：SIG.m_slot(+0x2c) 槽物存在且非空 →
// 广播列表包后 SendUpdateItemList(1,ITEMSPACE 7,slot)；返回恒 1。
bool CCreatureMgr::SendCreatureItemInfo(SIG_CREATURE_ITEM* sig)
{
    int slot = sig->m_slot;                    // +0x2c
    CInventory* inv = m_user->getCurCharacInvenW();
    Inven_Item* slotItem = inv->GetInvenRef(3, slot);
    if (slotItem != 0 && slotItem->m_addInfo != 0)   // +0x02
    {
        CPacketRespondent pkt(m_user, 0, 0);
        pkt.MakeNotipacketCreatureItemList(&m_creatureItems, slot);
        pkt.Send();
        m_user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)7, slot);
    }
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr22LoadCreatureTempPeriodEP31SIG_POSTAL_...
// T 0x0833d9d8（LoadCreatureTempPeriod.md）：+4 非 -1 时 RegistTempPeriod(uid,t,0)；
// 返回 t!=-1。（结构 SIG_POSTAL_CREATURE_TEMP_PERIOD 由 decl 收编，仅证 +0/+4）
bool CCreatureMgr::LoadCreatureTempPeriod(
    SIG_POSTAL_CREATURE_TEMP_PERIOD* sig)
{
    // ORIG T 0x0833d9d8：每处槽位均原地重取形参（`mov 0xc(%ebp),%eax` +
    // `mov 0x4(%eax),%eax` 读 m_tempPeriod、`mov (%eax),%eax` 读 m_uid），
    // true/false 在分支内直接置 %eax ⇒ 不设中间局部（O0 下多占栈槽会偏离）。
    if (sig->m_tempPeriod != -1)
    {
        RegistTempPeriod(sig->m_uid, sig->m_tempPeriod, 0);
        return true;
    }
    return false;
}

// ORIG _ZN13user_creature12CCreatureMgr22SetCreatureItemAddInfoEii T 0x0833b620
// （SetCreatureItemAddInfo.md）：背包区 3 槽物存在且 idx(+0x02) 非空 →
// set_add_info(value) 返 1；否则 0。
bool CCreatureMgr::SetCreatureItemAddInfo(int slot, int value)
{
    // ORIG 0x833b620+7：entry `movl $0,-0xc`（item 两段式 P197③）；refW 结果
    // 无槽往返（P0）⇒ 嵌套实参直传 GetInvenRef 首参。
    Inven_Item* item = 0;
    item = m_user->getCurCharacInvenRefW()->GetInvenRef(3, slot);
    if (item == 0)
        return false;
    if (item->m_addInfo == 0)   // +0x02
        return false;
    item->set_add_info(value);
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr24InsertCreatureItemResultEP17SIG_...
// T 0x0833b38c（InsertCreatureItemResult.md）：load 后 field64==6 广播列表；
// set/order 记录 ceraOrderNo(+0x60) 前后置/清零；WriteCreatureLog 五值直取 SIG
// （idx +0x8 / addInfo2 +0x30 / 类型 byte +0x5 / reason +0x51）。
bool CCreatureMgr::InsertCreatureItemResult(SIG_CREATURE_ITEM* sig)
{
    Inven_Item localItem;
    memset(&localItem, 0, sizeof(localItem));
    reinterpret_cast<Inven_Item_Compatible*>((void*)sig->m_item)->makeInvenItem(localItem);

    if (!LoadCreatureItem(&localItem,
                          reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                              (char*)sig + 0x2c)))
        return false;

    if (sig->m_field64 == 6)
        SendCreatureItemInfo(sig);

    m_user->set_cera_item_order_no((unsigned int)sig->m_ceraOrderNo);   // +0x60

    const CInventory* inv = m_user->getCurCharacInvenR();
    inv->WriteCreatureLog(
        (INVEN_TYPE)3,
        *(const int*)((const char*)sig + 8),                  // compat.m_addInfo
        *(const int*)((const char*)sig + 0x30),               // m_addInfo2
        1,
        (int)*(const unsigned char*)((const char*)sig + 5),   // compat.m_field1
        (eItemAddReason)*(const signed char*)((const char*)sig + 0x51));  // ORIG movsbl 符号扩展
    m_user->set_cera_item_order_no(0);
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr24UpdateCreatureItemResultEP17SIG_...
// T 0x0833b592（UpdateCreatureItemResult.md）：同载入链；成功且 field64==4
// 广播 SendCreatureItemInfo。
bool CCreatureMgr::UpdateCreatureItemResult(SIG_CREATURE_ITEM* sig)
{
    // ORIG 0x833b592：仅 `lea -0x45,%eax; mov (%esp); call Inven_Item::Inven_Item`
    // 一条——清零由 ctor（Inven_Item.cpp reset()）完成，无独立 memset。
    Inven_Item localItem;
    reinterpret_cast<Inven_Item_Compatible*>((void*)sig->m_item)->makeInvenItem(localItem);

    // ORIG tail `call; xor $0x1,%eax(dead flags-consumed); …mov$0`＝P168a② 直分支
    // 负判形（==0/! 早返失败），成功路径 fall-through 共享尾 mov$1。
    if (!LoadCreatureItem(&localItem,
                          reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                              (char*)sig + 0x2c)))
        return false;
    if (sig->m_field64 == 4)
        SendCreatureItemInfo(sig);
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr7EvoluteEiib T 0x083beb4
// （Evolute.md）：等级达标或 force 时；单线/多线进化分别判定后执行，
// 各失败 path trace（0xdd6 kind / 0xddb level）；空宠/不满足返 0。
bool CCreatureMgr::Evolute(int creatureId, int minLevel, bool force)
{
    if (m_equippedCreature != 0)
    {
        int level = m_equippedCreature->GetLevel();
        if (level >= minLevel || force)
        {
            if (m_equippedCreature->IsEvolutionCreature(creatureId) || force)
                return m_equippedCreature->Evolute(m_user);

            if (m_equippedCreature->IsEvolutionCreatureMulty(creatureId) || force)
                return m_equippedCreature->EvoluteMulty(m_user, creatureId);

            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xdd6, 0);
            trace("evolute error : different creature kind.");
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xddb, 0);
            trace("evolute error : low creature level.");
        }
    }
    return false;
}

// ---- ORIG _ZN13user_creature12CCreatureMgrC1ERKS0_ W 0x86992cc 【objdump】----
// （nm 弱别名 C2 同址）：按成员序拷贝 map<int,CCreatureItem*> → +0x18 equipped →
// +0x1c user → +0x20..+0x2b artifacts（int×3 循环）→ +0x2c tempExpireDelta →
// map<int,int> field30 → movzbl +0x48 isMaxLevel 标记。用户定义拷贝构造同时
// 产出 C1/C2 两符号。
// ORIG T 0x08339196：成员 map/CArtifact[3] 隐式默认构造后调 Reset()
// （置 equipped=0/user=0/tempExpireDelta=-1/artifacts Reset/isMaxLevel=1）。
CCreatureMgr::CCreatureMgr()
{
    Reset();
}

// ORIG 0x86992cc 拷贝构造：map/equipped/user/tempExpireDelta/map30/isMaxLevel
// 复制后仅循环复制 +0x20 起 2 个 dword（m_artifacts[0..1]）；m_artifacts[2]
// 与 +0x28 区间不复制（保持目标默认状态）。
CCreatureMgr::CCreatureMgr(const CCreatureMgr& src)
    : m_creatureItems(src.m_creatureItems),
      m_equippedCreature(src.m_equippedCreature),
      m_user(src.m_user),
      m_tempExpireDelta(src.m_tempExpireDelta),
      m_field30(src.m_field30),
      m_isMaxLevelGrowCreature(src.m_isMaxLevelGrowCreature)
{
    for (int i = 0; i < 2; ++i)
        m_artifacts[i] = src.m_artifacts[i];
}

// ============================================================================
// const 侧查询
// ============================================================================

// ORIG _ZNK13user_creature12CCreatureMgr13getExpireDateEi T 0x0833d758
// （getExpireDate.md）：FindCreatureItem 空 → -1；否则 getExpireTime()。
int CCreatureMgr::getExpireDate(int uid) const
{
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    return item->getExpireTime();
}

// ORIG _ZNK13user_creature12CCreatureMgr19GetCreatureItemSizeEv W 0x8531fbe
//【objdump】：单 call m_creatureItems.size() 后直返。
int CCreatureMgr::GetCreatureItemSize() const
{
    return (int)m_creatureItems.size();
}

// ORIG _ZNK13user_creature12CCreatureMgr21IsExistedCreatureItemEi T 0x0833a942
// （IsExistedCreatureItem.md）：遍历 map；条目空 → cMyTrace(...,0xbda,5)+
// "[%s][%d]"；GetSlotNo()==slot 即返 1；否则 0。
bool CCreatureMgr::IsExistedCreatureItem(int slot) const
{
    for (std::map<int, CCreatureItem*>::const_iterator it =
             m_creatureItems.begin();
         it != m_creatureItems.end(); ++it)
    {
        CCreatureItem* entry = it->second;
        if (entry == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xbda, 5);
            trace("[%s][%d]",
                  __PRETTY_FUNCTION__,
                  0xbda);
        }
        else
        {
            if (entry->GetSlotNo() == slot)
                return true;
        }
    }
    return false;
}

// ORIG _ZNK13user_creature12CCreatureMgr21checkMaxCreatureLevelEib T 0x0833baee
// （checkMaxCreatureLevel.md）：命中 itemId 且成长标志一致的首个条目上取
// CCreature::isMaxLevel()；否则 0。
bool CCreatureMgr::checkMaxCreatureLevel(int itemId, bool growFlag) const
{
    for (std::map<int, CCreatureItem*>::const_iterator it =
             m_creatureItems.begin();
         it != m_creatureItems.end(); ++it)
    {
        CCreature* entry = reinterpret_cast<CCreature*>(it->second);
        if (entry != 0 &&
            entry->m_itemId == itemId &&
            entry->IsGrowCreature() == growFlag)
        {
            return entry->isMaxLevel();
        }
    }
    return false;
}

// ORIG _ZNK13user_creature12CCreatureMgr22isGrowCreatureMaxLevelEv T 0x0833bbb2
// （isGrowCreatureMaxLevel.md）：movzbl +0x48 直返。
bool CCreatureMgr::isGrowCreatureMaxLevel() const
{
    return m_isMaxLevelGrowCreature;
}

}  // namespace user_creature

namespace user_creature
{

// ORIG _ZN13user_creature9CCreature16IsCreatureScriptEv W 0x0833f0f2【objdump】：
// setne +0x44(m_script!=0)。
bool CCreature::IsCreatureScript()
{
    return m_script != 0;
}

}  // namespace user_creature
