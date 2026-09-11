// ORIG _ZN13user_creature4CEgg16SaveCreatureItemEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x083390c8（SaveCreatureItem.md 全汇编转写）：蛋条目序列化；恒返 1。
// 首写经 GetItemId()（外联 call，见 CCreatureItem::GetItemId 定义）。
// [b28d-26] AE。旧 [b22判词·N82]「movzbl;mov%eax,%edx 双寄存路由不可复现」作废：
//   ORIG 0x083390c8 +0x26 赋值段 `movzbl 0x14(%eax),%eax; mov %eax,%edx; mov %dl,0x26`
//   = uchar→signed char 转换两段；目标字段 GamePODTypes.h 改 signed char 后逐条全等
//   （探针 /tmp/d26p/p_route2.cpp：uchar→char 两段、同型单段）。
bool CEgg::SaveCreatureItem(Inven_Item* item,
                            SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user)
{
    (void)user;                     // ORIG 未读该实参
    item->m_addInfo = GetItemId();  // *(int*)(item+0x02) ← CCreatureItem::GetItemId()（ORIG call）
    info->m_uid = m_uid;            // info+0x04 ← this+0x0c
    info->m_slotNo = m_slotNo;      // info+0x00 ← this+0x10
    info->m_creatureItemType =
        (unsigned char)m_creatureItemType;                      // +0x24 低字节
    info->m_noCharge = m_noCharge;  // info+0x26 ← this+0x14（目标 signed char ⇒ ORIG 双寄存路由）
    info->m_stat = m_stat;          // +0x27 ← this+0x15
    return true;
}