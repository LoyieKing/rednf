// ============================================================================
// CEgg::Hatch @ 0x0833912a（objdump 逐条转写）：蛋物品 find_item 非空且
//   非 stackable → *out = CEquipItem::get_hatch_item_id() 返回 1；否则 0
//   （ORIG 0x8339181 为外联 call 0x833eeda；nm -S 核验
//   _ZNK10CEquipItem18get_hatch_item_idEv W 0x0833eeda size 0xe）。
// ============================================================================
bool user_creature::CEgg::Hatch(int* out)
{
    CItem* item = G_CDataManager()->find_item(
        reinterpret_cast<user_creature::CCreatureItem*>(this)->GetItemId());
    if (item == 0)
        return false;
    if (item->is_stackable())
        return false;
    // ORIG 0x8339175..0x8339181：先物化 CEquipItem* 局部（mov -0x10(%ebp),%eax;
    // mov %eax,-0xc(%ebp)），再以该局部为 this 外联调用 get_hatch_item_id。
    CEquipItem* equip = static_cast<CEquipItem*>(item);
    *out = equip->get_hatch_item_id();
    return true;
}
