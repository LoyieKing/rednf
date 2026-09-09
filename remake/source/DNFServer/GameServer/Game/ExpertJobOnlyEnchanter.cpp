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
// [b28d-solo-ExpertJobOnlyEnchanter] AE（189/189 逐条全等，p167 14/14 CLEAR）。
// 修法：①补 ORIG 0x0849d83d 的死局部 `movl $0x0,-0xc(%ebp)`（P171 死写入照抄
// + P232 帧律，同时把 4.4 标量槽区补到 ORIG 同深：ret -0x18 / card -0x14 /
// slot -0x10）；②`slot = GetInvenData(...)` 与 `if (slot == -1)` 分两句（P15，
// 赋值即条件形多 sete;test 物化）。
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
    // [b28d-solo-ExpertJobOnlyEnchanter] P171/P232：ORIG 0x0849d83d 有死写入
    // `movl $0x0,-0xc(%ebp)`（Ghidra local_10 赋 0 后不再读）⇒ 照抄一个死局部，
    // 同时把 4.4 标量槽区补到与 ORIG 同深（ret -0x18 / card -0x14 / slot -0x10）。
    int dead = 0;
    (void)dead;
    // P15：赋值与条件分两句（ORIG 为 `cmpl $0xffffffff,-0x10; jne` 直跳，
    // 赋值即条件形会多 sete;test 物化）。
    slot = user->getCurCharacInvenR()->GetInvenData(card->get_index(),
                                                    invenItem);
    if (slot == -1)
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
    register int notified;
    register int result;
    // [b28d-solo-ExpertJobOnlyEnchanter] 判词·N（314 vs ORIG 312，prologue/帧 0x30
    // 与全部调用点已对齐；p167 见 TU 报告）。已修：①allowed 正条件包裹
    // （P214①，原负条件反产 jne 落 true 岛）；②m_user 守卫改正向 if + 块外 return
    // （P214①/h3 形，原负条件产 jne 落 true 岛）；③PacketGuard 内层块 + 统一
    // `return result`（P223 尾标签 + ORIG 0x416 共享 mov $0,%ebx）；④register 无初值
    // 声明 + else 臂 `notified = 0`（P231/P248④，原初始化形多两条 mov）。
    // 残余 N：①callee-saved 角色互换——4.4 分配器把 notified/result 固定为
    //   ebx/esi（ORIG 反之），声明序/声明位置（函数头↔使用点）/作用域四形实测均不翻
    //   （N7/N82附）；②4.4 -O0 无 tail-merge/crossjump——`notified=1` 两处与
    //   `result=0` 两处不合并且多 1 jmp+nop（N121 族）；③EH cleanup 块相对序反转
    //   （ORIG 正常尾在前、unwind 居中；4.4 恒正常尾内联、unwind 压最末，N104）。
    // 语义完整（p167 差集零）；以上均为 4.1/4.4 后端块布局差异。
    if (user->getCurCharacR() == 0)
        return 3;

    // ORIG 全部 unique_id 比较为 16 位（cmp %ax）；allowed 不落栈（寄存器物化）
    if ((unsigned short)user->get_unique_id() !=
        (unsigned short)m_peer->get_unique_id())
    {
        // [b28d-solo-ExpertJobOnlyEnchanter] P214①：ORIG 短路物化为正条件
        // `jne L_false; call; xor $1; test; je L_false; mov $1; jmp; L_false: mov $0;
        // test %al; je L_err`（无栈槽）⇒ 必须写「正向 if 包裹 + else 早返」，
        // 负极性 `if (!(A&&B)) return` 会反产 jne 落 true 岛（探针 /tmp/probe_allowed.cpp
        // g2 与 ORIG 逐条同构，g1/g3 均反）。
        if (kind == 2 && IsUseOtherUserMonsterCard() != true)  // ORIG bVar1
        {
            if ((unsigned short)user->get_unique_id() !=
                (unsigned short)m_user->get_unique_id())
                return 3;
        }
        else
        {
            return 3;
        }
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

    // [b28d-solo-ExpertJobOnlyEnchanter] P214①/h3 形：ORIG 两个短路 je 同落 false 岛
    // 后 `test %al,%al; je <尾部 return 0>`（尾部 return 岛直落）⇒ 必须写「正向 if
    // 包裹 + 块外 return」，负条件 `if(!(A&&B)) return 0;` 反产 jne 落 true 岛
    // （探针 /tmp/probe_guard.cpp h1/h2 反、h3 与 ORIG 逐条同构）。
    // [b28d-solo-ExpertJobOnlyEnchanter] P231/P248④：ORIG 尾段无 `mov $0` 预清，
    // result/notified 全程 callee-saved 无槽（ebx/esi），仅 else 臂显式
    // `mov $0x3,%ebx; mov $0x0,%esi` ⇒ 无初值 register 声明 + else 臂显式
    // `notified = 0;`。`return result;` 统一落函数尾出口（ORIG 0x41b）；
    // m_user 无效臂的 result=0 与 `if (notified) result = 0;` 共享
    // `mov $0x0,%ebx`（ORIG 0x416 单点）。
    if (m_user != 0 && m_user->getCurCharacR() != 0)
    {
    {
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

    if ((unsigned short)user->get_unique_id() ==
        (unsigned short)m_user->get_unique_id())
    {
        if (m_peer != 0 && m_peer->getCurCharacR() != 0)
        {
            m_peer->Send(packet);
            goto enchant_set_notified;
        }
        result = 3;
        notified = 0;
        goto enchant_notified_done;
    }
    m_user->Send(packet);
enchant_set_notified:
    notified = 1;
enchant_notified_done: ;
    }
    // ORIG 0x412-0x41b：`test %esi,%esi; je 0x41b; mov $0x0,%ebx; 0x41b: mov %ebx,%eax`
    // ⇒ 负条件 goto 跳过 result=0；m_user 无效臂与 notified!=0 臂共享同一
    // `result = 0`（P223 尾标签形；正条件 if/else 形 4.4 多 jmp 且两处 mov 不合并）。
    if (!notified)
        goto enchant_skip_zero;
    }
        result = 0;
enchant_skip_zero:
    return result;
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
// [b28d-solo-ExpertJobOnlyEnchanter] P254 档③ + P235 匿名 union 双视角：
// ORIG `mov 0xe(%eax),%eax; mov %eax,%ebx` 两段升格由「字段与 find_item(int) 实参
// 异型」触发 ⇒ 取 MSG_STATIC_COMMAND 的 unsigned 视角 m_value0eU（探针
// /tmp/probe_p254d.cpp f2 逐条实证；int 视角 f1 仍单段，OnPutItemForEnchant 不受影响）。
void CEnchanter::OnTryEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;   // ORIG 首条 movl $0,-0xc：err 声明置函数头
    m_field40 = G_CDataManager()->find_item(msg->m_value0eU);
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
// [b28d-solo-ExpertJobOnlyEnchanter] 判词：553/553 条、gap 0、帧 sub $0x60 与
// 槽位集全等、p167 CLEAR（§6.2 三件套满足），仅剩 1 处 3 条求值序差 ⇒ 语义完成、
// byte 层 N 封顶（4.1/4.4 短路物化位置差，N-88 族）。
// 本轮真语义修正：①门禁漏 `m_field38 > 0` 且 bVar4b=false 时 get_money 检查仍须
// 执行（原 else-if 链整段跳过）；②MakeItemPacket 的 receiver 应为 m_peer（0x2a0
// `mov 0x2c(%eax),%eax`），原误用 m_materials[0].m_user。
// 本轮形态修正：ret 预清+赋值即条件（P198①/P15）；UseMaterialOfRecipe 赋值即条件；
// useOther/slot0/slot0b/ok/cardIdx/ejType/slot1/characName/accName/fee 全部内联
// （P0/P134，具名局部各占槽）；局部声明序按 ORIG 槽位（successSent(-0x21) →
// ref=0(-0x20) → PacketGuard(-0x44) → invenType(-0x1c) → ref(-0x20) →
// itemIndex0(-0x18) → oldCardRef(-0x14) → addedCard(-0x10)）；trace 改全表达式
// 临时链（P227，ctor 前求值 varargs，槽 -0x38）；IsExistRecipeItem 返回类型
// int→bool（P292/P248 档①，CExpertJob.h 声明 + ExpertJobOnlyMaterial.cpp 定义
// 已由 S351 同步）。
// 残余 N：0x056-0x0a1 段 ORIG 先物化 `(m_user != 0 && m_peer->getCurCharacR() != 0)`
// 成 %al 岛再与 CheckMoney 短路；4.4 恒把 CheckMoney 提到物化之前。平铺三条件链、
// `!!`、`!= false`、德摩根四形探针（/tmp/probe_cond.cpp、probe_cond2.cpp）全产
// 同一 4.4 形，P226 语句表达式实测 553→558 劣化 ⇒ 判 N（跨版本短路物化位置差）。
// ---------------------------------------------------------------------------
int CEnchanter::_DoEnchant(ENUM_TRY_ENCHANT_TYPE type)
{
    // [b28d-56 判词·N] 封顶 N82/P182：ORIG 0x0849cb2c 帧 `sub $0x60` vs 我们
    // `add $<A>,%esp`（大帧 4.4 用 add 形）；553/552 条、助记符序列同，仅帧/槽号差。
    // ---- 门禁块 1：对家有效且双方不同时校验登记费用 ----
    if (m_peer != 0 && m_peer->getCurCharacR() != 0 && m_user != m_peer)
    {
        // [b28d-solo-ExpertJobOnlyEnchanter] 真语义修正：ORIG 0x048-0x0d5 的门禁是
        // `if (bVar4 && m_field38 > 0) { if (bVar4b && CheckMoney!=1) return 0x11;
        //   if (get_money() < m_field38) return 0x11; }`——原 else-if 链漏了
        // ①m_field38 > 0 前置门禁；②bVar4b 为假时 get_money 检查仍须执行。
        if (m_field38 > 0)
        {
            // ORIG 条件照写：此处复核的是 m_peer 的当前角色（非 m_user）
            // [b28d-solo-ExpertJobOnlyEnchanter] ORIG 0x056-0x0a1 先把
            // `m_user != 0 && m_peer->getCurCharacR() != 0` 物化成 %al 岛再与
            // CheckMoney 短路；平铺三条件链 4.4 把 CheckMoney 提到物化之前
            // （残余顺序差）。P226 语句表达式形实测 553→558 劣化（+5 条），已回退。
            if (m_user != 0 && m_peer->getCurCharacR() != 0 &&
                m_user->CheckMoney(m_field38) != 1)
                return 0x11;
            if (m_peer->getCurCharacInvenR()->get_money() < m_field38)
                return 0x11;
        }
    }

    // ---- 材料槽与配方校验 ----
    // [b28d-solo-ExpertJobOnlyEnchanter] P198①/P15：ORIG 0x0da 有预清
    // `movl $0x0,-0x28(%ebp)` 且 `cmpl $0x0,-0x28; setne %al; test %al; je`
    // ⇒ 「声明=0 + 赋值即条件」两句式。
    int ret = 0;
    if ((ret = IsRightMaterial()) != 0)
        return ret;

    // [b28d-solo-ExpertJobOnlyEnchanter] P0/P134：ORIG 该 bool 无槽（`call; movzbl
    // %al,%ecx` 直传）⇒ 内联实参，勿具名（具名多槽 + movb 往返，帧 +4）。
    if (!IsExistRecipeItem(m_user, reinterpret_cast<CItem*>(m_field40),
                           IsUseOtherUserMonsterCard()))
        return 0x15;

    // [b28d-solo-ExpertJobOnlyEnchanter] ORIG 初始化/声明序（槽位实证）：
    // successSent(-0x21) → ref=0 预清(-0x20) → PacketGuard(-0x44) → invenType(-0x1c)
    // → ref=GetInvenRef(-0x20) → itemIndex0(-0x18) → oldCardRef(-0x14)
    // → addedCard=0(-0x10)。ref 预清为 P198① 两句式。
    bool successSent = false;
    Inven_Item* ref = 0;
    PacketGuard packet;
    int invenType =
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)m_materials[0].m_field0e);

    // [b28d-solo-ExpertJobOnlyEnchanter] P0：ORIG slot 值经 callee-saved 直传
    // （`movzwl 0x14(%eax),%ebx`），无具名槽 ⇒ 实参内联成员读。
    ref = ((CUser*)m_materials[0].m_user)->getCurCharacInvenW()
              ->GetInvenRef(invenType, m_materials[0].m_slot);
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
    CItem* addedCard = 0;                               // -0x10：本次写入的怪物卡物品

    if (_DrawEnchant())
    {
        addedCard = reinterpret_cast<CItem*>(m_field3c);
        if (addedCard != 0)
            ref->m_fieldd = addedCard->get_index();

        packet.clear();
        packet.put_header(0, 0xe);
        packet.put_byte((int)m_materials[0].m_field0e);
        packet.put_short(1);

        // [b28d-solo-ExpertJobOnlyEnchanter] 真语义修正（数据源用错）：ORIG
        // 0x2a0-0x2cd receiver = this+0x2c（m_peer）的 getCurCharacInvenR，
        // 非 m_materials[0].m_user（+0x8，仅 GetInvenRef 段用）。
        if (!m_peer->getCurCharacInvenR()->MakeItemPacket(
                (INVEN_TYPE)invenType, m_materials[0].m_slot, packet))
            return 0;   // ORIG：MakeItemPacket 失败直接返 0（packet 析构）

        packet.finalize(true);
        m_peer->Send(packet);
        successSent = true;
    }

    // ---- 统计：卡被消耗计数 ----
    if (addedCard != 0 && m_user != 0)
    {
        GetInstanceExpertJobStatistic()->increaseItemCount(
            (ENUM_EXPERT_JOB_TYPE)m_user->GetCurCharacExpertJobType(),
            (unsigned long)addedCard->get_index(), 1);
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

        // P15 赋值即条件（ORIG 0x4c5 `cmpl $0,-0x28; setne %al; test %al; je`）。
        if ((ret = UseMaterialOfRecipe(
                 m_user, reinterpret_cast<CItem*>(m_field40), &packet,
                 IsUseOtherUserMonsterCard())) != 0)
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
            bool delOk = m_peer->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, m_materials[1].m_slot, 1, (eItemDelReason)8,
                true);
            if (!delOk)
            {
                // [b28d-solo-ExpertJobOnlyEnchanter] P227 全表达式临时链：ORIG
                // 0x615-0x668 在 ctor 前求值 varargs（slot1→%esi、getCurCharacNo→%ebx
                // 跨 ctor 保活）⇒ 具名 trace 两语句形会把它推到 ctor 之后。
                cMyTrace(__PRETTY_FUNCTION__, 0x3d9, 5)(
                    "CUser::compound_item, getCurCharacInvenW()->delete_item failed , %d %d",
                    m_peer->getCurCharacNo(), (int)m_materials[1].m_slot);
            }

            packet.clear();
            packet.put_header(0, 0xe);
            packet.put_byte(0);
            packet.put_short(1);
            m_peer->getCurCharacInvenR()->MakeItemPacket(
                (INVEN_TYPE)1, m_materials[1].m_slot, packet);
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
    m_peer->m_historyLog.EnchantItem(itemIndex0, oldCardRef, ref->m_fieldd,
                                     successSent, m_peer->get_acc_name(),
                                     m_peer->getCurCharacName());
    return 0;
}

}  // namespace expert_job
