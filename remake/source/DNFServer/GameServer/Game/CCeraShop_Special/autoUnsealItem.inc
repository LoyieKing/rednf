// ============================================================================
// autoUnsealItem —— ORIG 0x08327f10 (T)
// 自动解封：商品为随机选项物品（IsRandomOption）且原商品
// （getOriginalItem→find_item）IsRandomOptionUnseal 时，经
// GlobalData::g_randomOptionHandle->give_option 写入 invItem 的随机选项区
// （m_random.m_random，ORIG &invItem+0x25）。失败记 logFormat。
// ============================================================================
// b28d-D42 verdict=DIFF [N18a] ORIG 0x08327f10 orig/ours=103/105 ndiff=78。
// xor/sete 物化方向与寄存器分配差；语义等价封顶。
// b28d-solo-CCeraShop_Special verdict=NEAR [N7] 105/105 等长，仅 give_option 实参 edx/ecx 角色互换
void CCeraShop::autoUnsealItem(const CItem* item, Inven_Item& invItem)
{
    // b28d-solo P223 多岛形：ORIG 两守卫各落独立 nop 岛（item==0 → nop;jmp；
    // !IsRandomOption → nop），合并 && 会被 4.4 归一成 bool 岛 ⇒ 拆两条独立早返。
    if (item == 0)
        return;
    if (!item->IsRandomOption())
        return;
    int original = getOriginalItem();
    if (original != 0)
    {
        const CItem* origItem = G_CDataManager()->find_item(original);
        if (origItem != 0 && origItem->IsRandomOptionUnseal())
        {
            // ORIG 实参序 vtable 槽核对 —— get_rarity/getUsableLevel/
            // getItemGroupName/GetRandomOptionGrade(vtable+0x50)；
            // options = invItem.m_random.m_random（ORIG &invItem+0x25）。
            if (!GlobalData::g_randomOptionHandle->give_option(
                    invItem.m_addInfo,
                    (ENUM_RARITY)item->get_rarity(), item->getUsableLevel(),
                    item->getItemGroupName(), item->GetRandomOptionGrade(),
                    &invItem.m_random.m_random))
            {
                LogManager::logFormat(1, "CeraShop.cpp", __PRETTY_FUNCTION__,
                                      0x2131, "autoUnsealItem : %d",
                                      invItem.m_addInfo);
            }
        }
    }
}

