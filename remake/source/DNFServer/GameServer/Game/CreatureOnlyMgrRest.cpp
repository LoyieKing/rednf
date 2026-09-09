// ============================================================================
// df_game_r 还原 —— user_creature::CCreatureMgr 收尾批（CreatureOnlyMgrRest）。
// ---- b28d-61 判词汇总（本 TU 14 件非 identical）----
// 已修 AE：UpdateCreatureItemResult（显式指针算术 `(char*)sig+4` 复现 ORIG lea）、
//   EventEvoluteCreature（P222 正向包裹翻 setge 极性）。
// N 封顶（ORIG 地址见各函数头/报告）：
//   checkMaxCreatureLevel 0x0833baee（GetItemId 已补；余 N89 STL 调用+帧）
//   LoadCreatureItem 0x0833b68c（N89 内联差异 1 处）
//   IsExistedCreatureItem 0x0833a942（N104 分支/trace 块布局）
//   IsSaveCreatureItem 0x0833ba30（setcc-fold：ORIG sete vs 4.4 setne）
//   C1ERKS0_ 0x0833b6xx（N84/N86 拷贝构造整体形态）
//   SendCreatureItemInfo 0x0833b498（P183 槽/局部缓存）
//   AuctionRegist 0x0833b09c / ChangeOwner 0x0833ad34 / ChangeSlotNo 0x0833aedc
//     / SaveCreatureItem 0x0833bbbe（N80 cMyTrace 变参求值序+GetItemId 块）
//   InsertCreatureItemResult 0x0833b4d4（N82 寄存器保存 push edi/esi）
//   Evolute 0x0833beb4（N85 bool 物化岛）
//   DenySwapArtifact 0x0833989c（N93 块序）
//   SetOverSkillTime 0x0833c41e（P211 求值序，GetItemId 已补）
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：**真语义 bug 修正**
// （WriteCreatureLog 末实参 ORIG 0x0833ae4e 读 0x14(%ebp)=mapSizeHint，
// 旧码误传 param_5）+ P212/P309 reason 全头部声明占最深槽 + P0 内联 characNo
// + P227 临时链 trace + P52 链式 receiver。残差：槽号分配（N82）。
bool CCreatureMgr::ChangeOwner(Inven_Item* item, int slotFrom, int mapSizeHint,
                               int param_4, int param_5)
{
    if (!IsCreatureEquipmentScope(slotFrom))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP
        return true;

    // b28d-solo-CreatureOnlyMgrRest [P212/P309]：ORIG reason 占最深槽 -0x3c
    // （sender -0x34 / mapSize -0x1c）⇒ 全头部声明、先声明深。
    eItemAddReason reason;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // ORIG 实参序：(item, characNo, slotFrom, mapSizeHint, mapSize, p4, p5)；
    // sender 侧 +0x48=characNo、+0x2c=slotFrom、+0x51 低字节=mapSizeHint、
    // +0x68=mapSize、+0x6c=p4、+0x70=p5（汇编 ebp+0x10..0x24 对应）。
    // b28d-solo-CreatureOnlyMgrRest [P0]：ORIG 0x0833ada9 getCurCharacNo 结果
    // 无槽（直接 0x8(%esp)）⇒ 内联实参。
    if (!sender.ChangeOwner(item, m_user->getCurCharacNo(), slotFrom, mapSizeHint,
                            mapSize, param_4, param_5))
    {
        // b28d-solo-CreatureOnlyMgrRest [P227]：ORIG uid 在 ctor call 前
        // movzwl→%ebx 保活 ⇒ 全表达式临时链。
        cMyTrace(
            "bool user_creature::CCreatureMgr::ChangeOwner("
            "Inven_Item*, int, int, int, int)", 0xc26, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正 + P52]：ORIG 0x0833ae4e
    // 的 WriteCreatureLog 末实参读 0x14(%ebp)=mapSizeHint（旧码误传 param_5）；
    // 该值落具名槽 -0x3c，field1/addInfo2/addInfo 先物化 edi/esi/ebx 再 call
    // getCurCharacInvenR ⇒ receiver 单语句链式。
    reason = (eItemAddReason)mapSizeHint;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, reason);
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P212/P309 reason 全头部声明
// + P0 内联 characNo + P227 临时链 trace + P52 链式 receiver（reason 实参
// 经复核 = param_3 = ORIG 0x14(%ebp)，无 bug）。残差：槽号分配（N82）。
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

    // b28d-solo-CreatureOnlyMgrRest [P212/P309]：ORIG reason 占最深槽 -0x3c
    // （sender -0x34 / mapSize -0x1c）⇒ 全头部声明、先声明深。
    eItemAddReason reason;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // b28d-solo-CreatureOnlyMgrRest [P0/P227]：ORIG 0x0833af74 getCurCharacNo
    // 结果无槽（直接 0x8(%esp)）⇒ 内联；失败 trace uid 在 ctor 前
    // movzwl→%ebx 保活 ⇒ 全表达式临时链。
    if (!sender.ChangeSlotNo(item, m_user->getCurCharacNo(), newSlot, param_3))
    {
        cMyTrace(
            "bool user_creature::CCreatureMgr::ChangeSlotNo("
            "Inven_Item*, int, int)", 0xc44, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    // b28d-solo-CreatureOnlyMgrRest [P52]：ORIG 0x0833b00d reason 落槽 -0x3c、
    // field1/addInfo2/addInfo 先物化 edi/esi/ebx 再 call getCurCharacInvenR。
    reason = (eItemAddReason)param_3;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, reason);
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：**真语义 bug 修正**
// （DeleteCreatureItem 第 2 实参 ORIG 读 0x14(%ebp)=param_3，旧码误传两次
// item->m_addInfo2）+ P0/P227 内联 characNo 与临时链 trace + 返回归一化
// `if(!call()) return false; return true;`。
bool CCreatureMgr::AuctionRegist(Inven_Item* item, int param_2, int param_3)
{
    if (!IsCreatureEquipmentScope(param_2))
        return true;
    if (item->m_field1 != 5)
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // b28d-solo-CreatureOnlyMgrRest [P0/P227]：ORIG 0x0833b110 的
    // getCurCharacNo 结果无槽（直接 0x8(%esp)）⇒ 内联实参；失败 trace 的
    // uid 在 ctor call 前求值并入 %ebx（P227 全表达式临时链）。
    if (!sender.AuctionRegist(item, m_user->getCurCharacNo(), param_2, param_3,
                              mapSize))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc5c, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正]：ORIG 0x0833b1a4 第二实参
    // 读 0x14(%ebp)=param_3（旧码误传两次 item->m_addInfo2）；返回归一化形
    // `xor $1; test; je <mov $1>` = `if(!call()) return false; return true;`。
    if (!DeleteCreatureItem(static_cast<int>(item->m_addInfo2), param_3))
        return false;
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr16DenySwapArtifactEP10Inven_ItemiiS2_ii
// T 0x0833989c（DenySwapArtifact.md，条件序以汇编为准）：
// 有装备宠：slot 组 (s1,b)/(s3,a) 任一满足 IsArtifactEquipSlot && !b.isEmpty()
// 且装备宠 IsAvailableArtifact(s1) 为假 → deny(1)；否则 0。
// 无装备宠：任一组 IsArtifactEquipSlot && !isEmpty(b/a) → allow(0)；否则 1。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：**真语义 bug 修正**
// （无装备宠分支 ORIG 0x08339990 无 `xor $1` ⇒ 条件为 b->isEmpty()/a->isEmpty()，
// 旧码 `!isEmpty()` 极性写反）+ P226 语句表达式零槽复现 `&&` 物化岛 +
// N93附 反向共享尾（block A return false 提为函数尾，复现 ORIG
// `mov $1,%eax; jmp; mov $0,%eax` 尾块序）。
bool CCreatureMgr::DenySwapArtifact(Inven_Item* a, int s1, int s2,
                                    Inven_Item* b, int s3, int s4)
{
    (void)s2;   // ORIG 未读 +0x14
    (void)s4;   // ORIG 未读 +0x20

    if (m_equippedCreature != 0)
    {
        // b28d-solo-CreatureOnlyMgrRest [P226/P0]：ORIG ok1/ok2 无栈槽
        // （`mov $1/jmp` + `mov $0` + `test %al,%al; je` 寄存器归一化）
        // ⇒ 语句表达式包 `&&` 短路，勿用具名 bool（4.4 槽化 + cmpb）。
        if (({ IsArtifactEquipSlot(s1) && !b->isEmpty(); }))
        {
            if (!m_equippedCreature->IsAvailableArtifact(s1))
                return true;
        }
        if (({ IsArtifactEquipSlot(s3) && !a->isEmpty(); }))
        {
            if (!m_equippedCreature->IsAvailableArtifact(s1))
                return true;   // ORIG 两处 IsAvailableArtifact 均传 +0x10(s1)
        }
    }
    else
    {
        // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正]：ORIG 0x08339990
        // 无 `xor $1`（0x083398d1 block A 才有）⇒ 无装备宠分支条件为
        // `b->isEmpty()`/`a->isEmpty()`，旧码 `!isEmpty()` 极性写反。
        if (({ IsArtifactEquipSlot(s1) && b->isEmpty(); }))
            return false;
        if (({ IsArtifactEquipSlot(s3) && a->isEmpty(); }))
            return false;
        return true;
    }
    // b28d-solo-CreatureOnlyMgrRest [N93附 反向]：ORIG 尾块序为
    // `mov $1,%eax; jmp 尾; mov $0,%eax; leave; ret`（block B ret1 在前、
    // block A 的 ret0 落共享尾）⇒ 把 block A 的 return false 提为共享尾。
    return false;
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P248 档② m_growExpFlag
// uchar→bool + 裸 `if(!field)`（GamePODTypes.h）+ 删 m_addInfo 冗余 cast。
// 残差 N7：4.4.7 间接 call 目标恒 %eax，ORIG 用 %edx（P230 换后端可治）。
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
            item->m_addInfo,
            item->m_addInfo2);
        GetInstanceCreatureItemFactory()->FreeCreatureItem(newItem);
        return false;
    }

    // b28d-solo-CreatureOnlyMgrRest [P248 档②]：ORIG 0x0833b7dc
    // `movzbl 0x30(%eax),%eax; xor $1,%eax; test %al,%al; je` ⇒ 字段 bool 化
    // （GamePODTypes.h m_growExpFlag uchar→bool）+ 裸 `!field`。
    if (!detail->m_growExpFlag &&              // detail+0x30
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=N 封顶（DIFF，语义完整）：§4.5
// 虚槽直调（IsSave vptr+4 / SaveCreatureItem vptr+0xc，ORIG 0x0833bc9b/bcb1）
// + P183 count 每处重读（删 cnt/base 具名局部）+ P28 `it++`（ppEi）+ P225
// 拆嵌套 if。残差：帧 0x4c vs 0x40 与多 push %edi、槽整体深移 0x10
// （临时链 cMyTrace 在 4.4 的聚合槽分配差异；具名形实测同样深移，N82/P112）。
bool CCreatureMgr::SaveCreatureItem()
{
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x89, m_user);
    SIG_CREATURE_ITEM_LIST* list =
        sender.GetInBuffer<SIG_CREATURE_ITEM_LIST>();
    if (list == 0)
    {
        // b28d-solo-CreatureOnlyMgrRest [P227 两形互试取优]：ORIG uid 在
        // ctor call 前 movzwl→%ebx 保活 ⇒ 全表达式临时链（具名形改求值序、
        // 槽同样深移 0x10，实测无收益）。残差帧/槽分配记 N82/P112。
        cMyTrace(__PRETTY_FUNCTION__, 0xd90, 5)(
            "cannot write db uid(%d)\n",
            m_user->get_unique_id() & 0xffff);
        return false;
    }

    memset(list, 0, sizeof(SIG_CREATURE_ITEM_LIST));   // 0x4078
    *(int*)(void*)list = 0;                            // count

    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)
    {
        CCreatureItem* entry = it->second;
        // b28d-solo-CreatureOnlyMgrRest [§4.5 槽指针直调 + P183/P28]：ORIG
        // 0x0833bc9b 起 `mov (%eax),%eax; add $4; mov (%eax),%edx; …call *%edx`
        // = IsSave 虚槽；0x0833bcb1 `add $0xc` = SaveCreatureItem 虚槽；
        // count 每处重读（无 cnt/base 具名局部）；增量段调 ppEi ⇒ `it++`。
        if (entry != 0)
        {
            typedef bool (*IsSaveSlotFn)(void*);
            if ((*reinterpret_cast<IsSaveSlotFn*>(
                     *reinterpret_cast<int*>(entry) + 4))(entry))
            {
                typedef bool (*SaveItemSlotFn)(void*, Inven_Item*,
                                               SIG_CREATURE_ITEM_DETAIL_INFO*,
                                               CUser*);
                (*reinterpret_cast<SaveItemSlotFn*>(
                    *reinterpret_cast<int*>(entry) + 0xc))(
                    entry,
                    (Inven_Item*)(void*)((char*)(void*)list +
                                         *(int*)(void*)list * 0x3d + 4),
                    (SIG_CREATURE_ITEM_DETAIL_INFO*)(void*)(
                        (char*)(void*)list + *(int*)(void*)list * 0x38 + 0x21a0),
                    m_user);
                // detail.m_uid（详情区 +4）非 0 视为写入成功。
                if (*(int*)((char*)(void*)list +
                            *(int*)(void*)list * 0x38 + 0x21a4) != 0)
                    *(int*)(void*)list = *(int*)(void*)list + 1;
            }
        }
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=N 封顶（DIFF，语义完整）：P196
// 比较方向翻正（ORIG `cmp %eax,%ebx; setg` = 累加值在左）+ P0 去 learnLevel/
// expLevel/lastUsed/stMs/ovstMs/curSec 具名局部 + P23/P24 求值序（itemIdx
// 写左侧）+ P227 临时链 trace1。残差：帧 0x8c vs 0x6c 与 trace 段局部槽
// 数量（4.1 用 %edi/%esi/%ebx 缓存，4.4 落槽；N82/N7）。
int CCreatureMgr::SetOverSkillTime(const int& itemIdx)
{
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsDieCreature())
            return 1;

        // b28d-61 [P167 MISSING `CCreatureItem::GetItemId()`]：ORIG 0x0833c5xx
        // `call GetItemId; cmp %eax,%ebx; sete` 为真实调用，旧形直读 +0x04 字段。
        // CCreature 为镜像类（基区 +0x00..+0x1b 与 CCreatureItem 一致），
        // 按 CCreature.cpp 既有 P201 做法经 reinterpret_cast 调用权威方法。
        // b28d-solo-CreatureOnlyMgrRest [P23/P24 求值序]：ORIG 0x0833c451 先
        // `mov 0xc(%ebp),%eax; mov (%eax),%ebx` 载 itemIdx，后 call GetItemId，
        // 尾 `cmp %eax,%ebx; sete` ⇒ 源码把 itemIdx 写左侧。
        if (itemIdx == reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetItemId())
        {
            if (!m_equippedCreature->IsCreatureScript())
            {
                // [P227]：ORIG 0x0833c486 在 ctor 前把 itemIdx 载入 %ebx 保活。
                cMyTrace(__PRETTY_FUNCTION__, 0xe62, 0)(
                    "SetOverSkillTime::creature_script is null(creature_itemno=%u)",
                    itemIdx);
                return 0x13;
            }

            // b28d-solo-CreatureOnlyMgrRest [P0/P196]：ORIG 0x0833c4d8
            // learnLevel→%ebx、expLevel 无槽（`cmp %eax,%ebx; setg`）⇒ 内联
            // 表达式且左操作数先求值。
            if (m_equippedCreature->GetLearnOverSkillLevel() >
                m_equippedCreature->GetExpLevel(
                    m_equippedCreature->GetExp()))
                return 0x13;

            // b28d-solo-CreatureOnlyMgrRest [P196/P0]：ORIG 0x0833c528 起
            // lastUsed→%ebx 后 `add %eax,%ebx` 连加两次，尾 `cmp %eax,%ebx;
            // setg`（累加值在左）⇒ 累加表达式写左、内联无局部。
            if (*m_equippedCreature->GetOverSkillTime()->GetTime() +
                    m_equippedCreature->GetSkillRecoveryTime() / 1000 +
                    m_equippedCreature->GetOverSkillRecoveryTime() / 1000 >
                GlobalData::s_systemTime_.getCurSec() + 2)
            {
                unsigned int accId = m_user->get_acc_id();
                char* idStr = NumberToString(accId, 0);
                int ovstR = m_equippedCreature->GetOverSkillRecoveryTime();
                int stR = m_equippedCreature->GetSkillRecoveryTime();
                int curSec = GlobalData::s_systemTime_.getCurSec();
                int last = *m_equippedCreature->GetOverSkillTime()->GetTime();
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xe6c, 0);
                // b28d-solo-CreatureOnlyMgrRest [P167/P44 变参即时求值]：ORIG
                // trace 实参内联重读 getCurSec 与 GetOverSkillTime()->GetTime()
                // （各第 2 次调用；p167 差集 MISSING×3 实证）。
                trace("fail:cur_time(%u),last_used_skilltime(%u),interval(%d) "
                      "st(%d),ovst(%d),m_id(%s),ch_no(%u)",
                      (unsigned int)GlobalData::s_systemTime_.getCurSec(),
                      (unsigned int)*m_equippedCreature->GetOverSkillTime()->GetTime(),
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P292/P16 `!size()` 早返形
// （ORIG `test; sete; test; je`）+ P225 拆嵌套 if + P28 `it++` + §4.5 IsSave
// 虚槽（vptr+4）直调。残差 N7：虚槽 call 目标 %eax vs ORIG %edx。
bool CCreatureMgr::IsSaveCreatureItem()
{
    // b28d-solo-CreatureOnlyMgrRest [P292/P16/P225/P28]：ORIG 0x0833ba41
    // `test %eax,%eax; sete %al; test %al,%al; je <循环>` = `!size()` 早返形
    // （`!=0` 包裹产 setne 反形）；循环体两条件 ORIG 各发独立 `je 增量`
    // ⇒ 拆嵌套 if；增量段调 ppEi ⇒ `it++`。
    if (!m_creatureItems.size())
        return false;

    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)
    {
        CCreatureItem* entry = it->second;
        if (entry != 0)
        {
            // b28d-solo-CreatureOnlyMgrRest [§4.5 槽指针直调先例]：ORIG
            // 0x0833ba52 `mov (%eax),%eax; add $0x4,%eax; mov (%eax),%edx;
            // …call *%edx` = _ZTV CCreature word3（vptr+4）= IsSave 虚槽。
            typedef bool (*IsSaveSlotFn)(void*);
            if ((*reinterpret_cast<IsSaveSlotFn*>(
                     *reinterpret_cast<int*>(entry) + 4))(entry))
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
        // b28d-61 [P222 正向包裹翻极性，ORIG 0x0833da3x `cmp 0x10(%ebp),%eax;
        // setge %al; test %al,%al; je +0xcc`]：旧「GetLevel()<minLevel 早判」
        // 产 setl 且 trace 块落 fall-through；改正向 if 后 trace 归 else。
        if (m_equippedCreature->GetLevel() >= minLevel)
        {
            if (m_equippedCreature->EventEvolute(m_user, creatureId, slot))
            {
                CPacketRespondent pkt(m_user, 0, 0);
                pkt.MakeNotipacketCreatureItemList(&m_creatureItems, 0xee);
                return true;
            }
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x10c2, 0);
            trace("event evolute creature error : low creature level.");
        }
    }
    return false;
}

// ORIG _ZN13user_creature12CCreatureMgr20SendCreatureItemInfoEP17SIG_CREATURE_ITEM
// T 0x0833b498（SendCreatureItemInfo.md）：SIG.m_slot(+0x2c) 槽物存在且非空 →
// 广播列表包后 SendUpdateItemList(1,ITEMSPACE 7,slot)；返回恒 1。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P197③ slotItem
// 两段式（ORIG `movl $0,-0xc`）+ P52 `sig->m_slot` 内联实参跨 receiver call
// 保活 + 去 inv/slot 具名局部（三处使用点各自重读 sig->m_slot）。
bool CCreatureMgr::SendCreatureItemInfo(SIG_CREATURE_ITEM* sig)
{
    // b28d-solo-CreatureOnlyMgrRest [P52/P197③/P0]：ORIG 0x0833b4a0
    // `movl $0x0,-0xc` 两段式初始化；0x0833b4aa `mov 0x2c(%eax),%ebx` 把
    // sig->m_slot 落 callee-saved 跨 getCurCharacInvenW call 保活（无具名局部），
    // 后续两处使用点各自 `mov 0x2c(%eax),%edx` 重读 ⇒ 全部内联 sig->m_slot。
    Inven_Item* slotItem = 0;
    slotItem = m_user->getCurCharacInvenW()->GetInvenRef(3, sig->m_slot);
    if (slotItem != 0 && slotItem->m_addInfo != 0)   // +0x02
    {
        CPacketRespondent pkt(m_user, 0, 0);
        pkt.MakeNotipacketCreatureItemList(&m_creatureItems, sig->m_slot);
        pkt.Send();
        m_user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)7,
                                   sig->m_slot);
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
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：删多余 memset（ORIG 仅
// Inven_Item::C1）+ P235 具名成员访问（m_reason/m_addInfo2/m_itemView.m_itemIdx/
// m_item[1] 复现 disp 折叠）+ P52 单语句链式 receiver。残差 N18a（ORIG
// `mov 0x8(%eax),%eax; mov %eax,%ebx` 两段 vs 4.4 单段）与帧 +0x10。
bool CCreatureMgr::InsertCreatureItemResult(SIG_CREATURE_ITEM* sig)
{
    Inven_Item localItem;
    eItemAddReason reason[1];
    // b28d-solo-CreatureOnlyMgrRest [ORIG 0x0833b395/N107]：ORIG 仅
    // `lea -0x55; call Inven_Item::C1`（清零由 ctor reset 完成，无独立
    // memset）；`lea 0x4(%eax),%edx` = 显式指针算术 `(char*)sig + 4`。
    reinterpret_cast<Inven_Item_Compatible*>((char*)sig + 4)->makeInvenItem(localItem);

    if (!LoadCreatureItem(&localItem,
                          reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                              (char*)sig + 0x2c)))
        return false;

    if (sig->m_field64 == 6)
        SendCreatureItemInfo(sig);

    m_user->set_cera_item_order_no((unsigned int)sig->m_ceraOrderNo);   // +0x60

    // b28d-solo-CreatureOnlyMgrRest [P52/P0]：ORIG 0x0833b417 先把 reason
    // 落具名槽 -0x5c、field1/addInfo2/addInfo 物化 edi/esi/ebx，之后才 call
    // getCurCharacInvenR ⇒ receiver 必须写成单语句链式，inv 不得具名。
    // b28d-solo-CreatureOnlyMgrRest [P235/P211]：ORIG 0x0833b417 全部为
    // disp 折叠成员访问（`movzbl 0x51(%eax)` / `mov 0x30(%eax),%esi` /
    // `mov 0x8(%eax),%eax` / `movzbl 0x5(%eax)`）⇒ 用具名字段而非指针算术。
    reason[0] = (eItemAddReason)(signed char)sig->m_reason;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3,
        sig->m_itemView.m_itemIdx,                            // +0x08
        sig->m_addInfo2,                                      // +0x30
        1,
        (int)sig->m_item[1],                                  // +0x05
        reason[0]);
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
    // b28d-61 [N107 反向可翻，ORIG 0x833b5a6 `lea 0x4(%eax),%edx`]：数组退化形
    // `(void*)sig->m_item` 在 4.4 恒折 `add $0x4,%eax`（探针 /tmp/b61_probe2
    // gB..gI + /tmp/b61_probe3 gJ 全 add）；改显式指针算术 `(char*)sig + 4`
    // 产 ORIG 的 lea 形（探针 gK/gL/gM）。+4 = m_item 区偏移（SigTypes_B.h）。
    reinterpret_cast<Inven_Item_Compatible*>((char*)sig + 4)->makeInvenItem(localItem);

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
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P226 语句表达式复现两处
// `||` 的 mov$1/mov$0 物化岛（ORIG 0x0833bedc）+ P0 去 GetLevel 具名局部。
// 残差：trace 对象槽分配方向（N82/P112）。
bool CCreatureMgr::Evolute(int creatureId, int minLevel, bool force)
{
    if (m_equippedCreature != 0)
    {
        // b28d-solo-CreatureOnlyMgrRest [P226/P0]：ORIG 0x0833bedc
        // `cmp 0x10(%ebp),%eax; jge L1; cmpb $0,-0x2c; je L0; L1: mov $1;
        // jmp; L0: mov $0; test; je` = `||` 短路物化岛且 GetLevel() 无槽
        // ⇒ 语句表达式包条件（内不含具名变量）。
        if (({ m_equippedCreature->GetLevel() >= minLevel || force; }))
        {
            if (({ m_equippedCreature->IsEvolutionCreature(creatureId) || force; }))
                return m_equippedCreature->Evolute(m_user);

            if (({ m_equippedCreature->IsEvolutionCreatureMulty(creatureId) || force; }))
                return m_equippedCreature->EvoluteMulty(m_user, creatureId);

            // b28d-solo-CreatureOnlyMgrRest [P221]：ORIG 兄弟块槽向
            // then=-0x28 / else=-0x18（后声明占浅槽）；4.4 具名对象恒反向
            // ⇒ 改全表达式临时链复现。
            cMyTrace(__PRETTY_FUNCTION__, 0xdd6, 0)(
                "evolute error : different creature kind.");
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xddb, 0)(
                "evolute error : low creature level.");
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

// ORIG _ZN13user_creature12CCreatureMgrC1ERKS0_ W 0x86992cc【objdump】：
// map 拷贝 → +0x18/+0x1c → m_artifacts[3] 内联 3×mov → +0x2c → map30 →
// +0x48 bool；无 CArtifact::C1 调用 ⇒ 原码是**编译器隐式合成拷贝构造**。
// [b28d-solo-CreatureOnlyMgrRest] verdict=N84 封顶（DIFF，语义已修正）：
// 删用户定义后 GCC 4.4.7 不发射隐式版本（nm 恒 U）；类外 `= default` 报
// 「数组被用作初始值设定」；含用户默认构造元素数组的隐式拷贝构造不可合成。
// 保留用户定义形（T 符号 + 3×CArtifact::C1 默认构造，ORIG 为 W 无默认构造）；
// 真 bug 修正：旧码 `i<2` 漏拷 m_artifacts[2]，ORIG 循环 3 次（初值 2→-1）。

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
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P272 反写 if/else
// （ORIG `je <trace 块>`，else 体直落）+ P28 `it++` + P221 全表达式临时链
// 翻 trace 对象槽（ORIG -0x20 浅于 begin -0x28）。
bool CCreatureMgr::IsExistedCreatureItem(int slot) const
{
    for (std::map<int, CCreatureItem*>::const_iterator it =
             m_creatureItems.begin();
         it != m_creatureItems.end(); it++)
    {
        CCreatureItem* entry = it->second;
        // b28d-solo-CreatureOnlyMgrRest [P272 块序反写 + P28 增量侧]：ORIG
        // 0x0833a98c `je <+0x56 trace 块>`（else 体直落、trace 块在后），
        // 增量段调 ppEi（后置 `it++`）⇒ 反写 if/else 并后置。
        if (entry != 0)
        {
            if (entry->GetSlotNo() == slot)
                return true;
        }
        else
        {
            // b28d-solo-CreatureOnlyMgrRest [P221 全表达式临时链]：ORIG
            // trace 对象槽 -0x20（浅于 begin -0x28/end -0x24），具名对象在
            // 4.4 恒占最低槽 -0x28 ⇒ 改构造即调用的临时链翻槽方向。
            cMyTrace(__PRETTY_FUNCTION__, 0xbda, 5)(
                "[%s][%d]", __PRETTY_FUNCTION__, 0xbda);
        }
    }
    return false;
}

// ORIG _ZNK13user_creature12CCreatureMgr21checkMaxCreatureLevelEib T 0x0833baee
// （checkMaxCreatureLevel.md）：命中 itemId 且成长标志一致的首个条目上取
// CCreature::isMaxLevel()；否则 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P28 增量侧 `it++`
// （ORIG 增量段 call ppEi，具 sret 临时 -0x10）。
bool CCreatureMgr::checkMaxCreatureLevel(int itemId, bool growFlag) const
{
    for (std::map<int, CCreatureItem*>::const_iterator it =
             m_creatureItems.begin();
         it != m_creatureItems.end(); it++)
    {
        // b28d-61 [P167 MISSING GetItemId + P225 拆嵌套 if]：ORIG 0x0833bb0x
        // `call GetItemId; cmp 0xc(%ebp),%eax; sete; test; je` 与
        // `cmp -0x1c(%ebp),%al; sete; test; je` 两段独立物化 ⇒ 嵌套 if。
        // b28d-solo-CreatureOnlyMgrRest [P28 增量侧]：ORIG 增量段
        // `call ppEi`（后置版，sret 临时 -0x10）⇒ 源形必写 `it++`。
        CCreature* entry = reinterpret_cast<CCreature*>(it->second);
        if (entry != 0)
        {
            if (reinterpret_cast<CCreatureItem*>(entry)->GetItemId() == itemId)
            {
                if (entry->IsGrowCreature() == growFlag)
                    return entry->isMaxLevel();
            }
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

// b28d-solo-CreatureOnlyMgrRest [N84 封顶]：ORIG 0x86992cc 是编译器隐式合成
// 拷贝构造（map 拷贝 → +0x18/+0x1c → m_artifacts[3] 内联 3×mov → +0x2c →
// map30 → +0x48 bool，无 CArtifact::C1 调用）。GCC 4.4.7 无法合成含用户
// 默认构造元素数组的隐式拷贝构造：删用户定义后 nm 恒 U（不发射），类外
// `= default` 直接报「数组被用作初始值设定」（实测）⇒ 只能保留用户定义形。
// 用户定义形必然先默认构造 m_artifacts（3×CArtifact::C1 W 调用，ORIG 无）
// 且符号为 T 非 W ⇒ DIFF 封顶；同时修正旧码漏拷 m_artifacts[2] 的真 bug
// （ORIG 循环初值 2 递减到 -1 = 3 次拷贝）。
user_creature::CCreatureMgr::CCreatureMgr(
    const user_creature::CCreatureMgr& src)
    : m_creatureItems(src.m_creatureItems),
      m_equippedCreature(src.m_equippedCreature),
      m_user(src.m_user),
      m_tempExpireDelta(src.m_tempExpireDelta),
      m_field30(src.m_field30),
      m_isMaxLevelGrowCreature(src.m_isMaxLevelGrowCreature)
{
    for (int i = 0; i < 3; ++i)
        m_artifacts[i] = src.m_artifacts[i];
}
