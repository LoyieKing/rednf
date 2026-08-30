#include "Dispatchers.h"

// ============================================================================
// Dispatcher_ModItemAttr::dispatch_sig  —— 还原 ORIG 0x08200b08 T（约 829 insn）
//
// 语义（依据 docs/class_func_reports/Dispatcher_ModItemAttr/dispatch_sig.md）：
//   客户端请求「修改装备属性（封印再铸 / 一次性再铸）」。包格式：
//     get_short  → equipSlot（目标装备槽，INVEN_TYPE_EQUIP=1）
//     get_int    → itemIdx （目标装备 item idx）
//     get_short  → matSlot （消耗材料槽：封印卷 / 再铸材料）
//   校验链：state==3 且当前角色存在 → ServiceRestrict(1,0xf) → 安全保护(0x26)
//           → CheckInTrade → 目标装备非空/ItemType 10..0x15/匹配 idx →
//           → 材料非空 → CheckItemLock(1,equipSlot) → 按材料 item idx 选再铸模式
//           → 扣材料（delete_item）→ 修改装备（update_item）→ _SendResult 回包
//           → 会员返还材料（GetReturnItemRate / insertItemIntoInventory）。
//
// ORIG 的全部校验失败均为「抛出 int / ENUM_ERROR 异常 → 函数尾部 catch」：
//   ENUM_ERROR（正值）→ catch1：SendCmdErrorPacket(0x54, 码&0xff) 后 return 0；
//   int(0)            → catch2：e>=0 → return 0（不发包）；
//   int(-1/-2/-3)     → catch2：e<0 → LineFunc(0x9706,__PRETTY_FUNCTION__,0,0)（仅日志）。
//   本实现用等价的 SendCmdErrorPacket / return / LineFunc 直接表达。
// ============================================================================

#include "InterfacePacketBuf.h"      // InterfacePacketBuf（PacketGuard 基类，put_*）
#include "PacketGuard.h"             // PacketGuard 权威声明（CUser::Send(PacketGuard&) 参数类型）
#include "CDataManager.h"            // G_CDataManager、find_item、reseal_table_t、CItem
#include "CInventory.h"              // INVEN_TYPE / eItemDelReason / eItemAddReason、
                                     // CSecu_ProtectionField、GlobalData::s_pSecuProtectionField
#include "GlobalData.h"              // s_pSecuProtectionField
#include "ServiceRestrictManager.h"  // ServiceRestrictManager
#include "Arad_DataManager.h"        // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "CItemList.h"               // get_rand_int(int)
#include <algorithm>                 // std::min<int>
// LineFunc（包分发断言日志；权威声明 CompoundEmblemDeps.h:35）
extern int LineFunc(int line, const char* func, int errNo, unsigned int mid);


namespace
{
// ---- 还原 Dispatcher_ModItemAttr::_SendResult（ORIG 0x08201938 T，独立成员函数，
//      Dispatchers.h 类声明未含该方法，故以 TU 局部静态函数表达相同包行为）----
//   参数：(resultType, matSlot, equipSlot) 对应 ORIG _SendResult(this,user,param_2,param_3,param_4)。
//   包：header(1,0x54) | byte(1) | short(matSlot) | int(该槽 item.get_add_info()) |
//       short(resultType)，随后 CUser::Send + SendUpdateItemList(1,0,equipSlot)。
void sSendResult(CUser* user, unsigned short resultType,
                 unsigned short matSlot, unsigned short equipSlot)
{
    Inven_Item item;                                           // ORIG 先 C1 默认构造
    item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)matSlot);  // 再拷贝赋值
    PacketGuard packet;
    packet.put_header(1, 0x54);
    packet.put_byte(1);
    packet.put_short((int)matSlot);
    packet.put_int(item.get_add_info());
    packet.put_short((int)resultType);
    packet.finalize(true);
    user->Send(packet);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)equipSlot);
}
}  // namespace

int Dispatcher_ModItemAttr::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ---- 1. 状态检查：state==3 且当前角色存在，否则错误包(0x54,0) 后 return ----
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        char* idStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace tr(__PRETTY_FUNCTION__,
                    0x95db, 5);
        tr("Dispatcher_ModItemAttr => User State : %d, m_id : %s",
           user->get_state(), idStr);
        return 0;   // ORIG throw int(0) → catch2 e>=0 → return 0（不发包）
    }

    // ---- 2. 服务限制 (RestrictCategory=1, v=0xf) ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()
            ->isRestricted(user, (RestrictCategory::Enum)1, 0xf))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0xd1);
        return 0;
    }

    // ---- 3. 安全保护 CSecu_ProtectionField::Check(0x26) ----
    unsigned int secuRet = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x26);
    if (secuRet != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, (unsigned char)secuRet);
        return 0;
    }

    // ---- 4. 交易中则直接忽略 ----
    if (user->CheckInTrade())
        return 0;

    // ---- 5. 解析包 ----
    unsigned short equipSlot = 0;
    unsigned long itemIdx = 0;
    unsigned short matSlot = 0;
    if (!packet.get_short(equipSlot) ||
        !packet.get_int(itemIdx) ||
        !packet.get_short(matSlot))
    {
        // ORIG throw int(-1) → catch2 e<0 → LineFunc(0x9706)（仅日志）
        return LineFunc(0x9706, __PRETTY_FUNCTION__, 0, 0);
    }

    // ---- 6. 取目标装备槽项目并校验 ----
    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item target = inven->GetInvenSlot((INVEN_TYPE)1, (int)equipSlot);
    if (target.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 4);   // ORIG ENUM_ERROR(4)
        return 0;
    }
    if ((unsigned int)(unsigned short)target.m_addInfo != (unsigned int)itemIdx)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 4);   // ORIG ENUM_ERROR(4)
        return 0;
    }
    if (target.m_field1 != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x13);
        return 0;
    }

    CItem* citem = G_CDataManager()->find_item((int)itemIdx);
    if (citem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 4);   // ORIG ENUM_ERROR(4)
        return 0;
    }
    {
        int itype = citem->GetItemType();
        if (itype < 10 || itype > 0x15)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x13);
            return 0;
        }
    }

    // ---- 7. 取材料槽项目 ----
    Inven_Item material = inven->GetInvenSlot((INVEN_TYPE)1, (int)matSlot);
    if (material.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x11);
        return 0;
    }

    // ---- 8. 装备槽加锁检查 ----
    if (user->CheckItemLock(1, (int)equipSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0xd5);
        return 0;
    }

    // ---- 9. 按材料 item idx 选择再铸模式并计算消耗 cost ----
    int resultType = 2;          // local_36：默认「一次性(oneshot)」模式
    int cost = 1;                // local_34：消耗数量（cast 前初值 1）
    const int matIdx = material.m_addInfo;
    if (matIdx == 0xe || matIdx == 0x292090f1)
    {
        // 封印再铸（reroll）：材料=封印卷，逐次加封印次数
        resultType = 1;
        if (target.GetReSealCount() > 6)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0xd);
            return 0;
        }
        if (citem->GetItemType() == 0xb)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x13);
            return 0;
        }
        cost = G_CDataManager()->m_resealTable.get_reseal_cost(
                   (unsigned char)citem->get_rarity(),
                   (unsigned char)citem->get_grade(),
                   target.GetReSealCount());
        if (cost < 0)
        {
            // ORIG throw int(-2) → catch2 e<0 → LineFunc(0x9706)（仅日志）
            return LineFunc(0x9706, __PRETTY_FUNCTION__, 0, 0);
        }
        if (citem->GetAttachType() != 3)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x13);
            return 0;
        }
    }
    else if (matIdx == 0x28978e)
    {
        // 一次性再铸（oneshot）
        resultType = 1;
        if (citem->GetItemType() == 0xb)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x13);
            return 0;
        }
        if (citem->GetAttachType() != 3)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x13);
            return 0;
        }
        cost = G_CDataManager()->m_resealTable.get_oneshot_reseal_cost(
                   (unsigned char)citem->get_rarity(),
                   (unsigned char)citem->get_grade(),
                   target.GetReSealCount());
        if (cost < 0)
        {
            // ORIG throw int(-2) → catch2 e<0 → LineFunc(0x9706)（仅日志）
            return LineFunc(0x9706, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    else if (matIdx != 0x28d14e && matIdx != 0xf &&
             matIdx != 0x381 && matIdx != 0x292090f2)
    {
        // 未知材料 idx → ORIG ENUM_ERROR(0x11)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x11);
        return 0;
    }
    // 其余 (0x28d14e / 0xf / 0x381 / 0x292090f2) 保持 resultType=2（一次性）

    // ---- 10. 校验材料数量并扣除 ----
    if (material.get_add_info() < cost)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x16);
        return 0;
    }
    if (!inven->delete_item((INVEN_TYPE)1, (int)matSlot, cost,
                            (eItemDelReason)3, true))
    {
        // ORIG throw int(-3) → catch2 e<0 → LineFunc(0x9706)（仅日志）
        return LineFunc(0x9706, __PRETTY_FUNCTION__, 0, 0);
    }

    // ---- 11. 修改装备并落库 ----
    if (resultType == 1)
    {
        // 封印再铸：标记已用 + 封印次数 +1（上限 7）
        if (target.m_field0 != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0x12);
            return 0;
        }
        target.m_field0 = 1;
        target.SetReSealCount(
            (unsigned char)std::min<int>(target.GetReSealCount() + 1, 7));
    }
    else
    {
        // 一次性再铸：随机重置附加信息
        target.set_add_info(get_rand_int(0));
    }
    inven->update_item((INVEN_TYPE)1, (int)equipSlot, target);

    // ---- 12. 回包（修改结果） ----
    sSendResult(user, (unsigned short)resultType,
                (unsigned short)matSlot, (unsigned short)equipSlot);

    // ---- 13. 会员返还材料（GetReturnItemRate(matIdx) / insertItemIntoInventory） ----
    int rate = 0;
    rate = user->GetPremiumInfo()->GetReturnItemRate(matIdx);
    if (rate != 0)
    {
        int returnQty = (cost * rate) / 100;
        if (returnQty > 0)
        {
            Inven_Item ret = material;
            ret.set_add_info(returnQty);
            int reslot = inven->insertItemIntoInventory(ret, (eItemAddReason)0xe,
                                                        true, false);
            if (reslot > -1)
                user->SendUpdateItemList(CUser::eSendTarget_1,
                                         (ENUM_ITEMSPACE)0, reslot);
        }
    }
    return 0;
}

// ============================================================================
// ORIG 0x082335da W _ZN22Dispatcher_ModItemAttr7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x08200b08，本文件上方)
//   与 _SendResult(0x08201938)。忠实转写空返回，勿臆造。
int Dispatcher_ModItemAttr::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                    ParamBase& /*param*/)
{
    return 0;
}

// ORIG 0x082335d0 W _ZN22Dispatcher_ModItemAttr4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x082335d0..0x082335da）；ORIG 的包读取在
//   dispatch_sig 内完成（get_short/get_int 直读 PacketBuf），read 不承担职责。
int Dispatcher_ModItemAttr::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;
}