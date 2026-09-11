// ORIG execute 0x084abce8：可堆叠或 avatar/creature 类型(idx 0x16/0x17) 物品跳过，
// 其余交队伍 drop_item_gm。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR；P0 表达式直读（无 reqIdx/skip 槽）、
// party==0 正向包裹消尾岛（P285/P223 变体）均已按 ORIG 复现。
void CItemDropInDungeon::execute()
{
    // b28d-solo-GmCmds_D：ORIG 无 reqIdx 槽（P0 表达式直读；末段 drop_item_gm
    // 处重读 +0x08 实证），skip 亦无 bool 槽（`test %al,%al` 直测）。
    CItem* item = G_CDataManager()->find_item(((const GmDFld<int>*)this)->f08);
    if (item == 0)
    {
        return;
    }
    // ORIG 虚表第 4 槽(vtable+0xc)=GetItemType。
    if (item->is_stackable() || item->GetItemType() == 0x16 ||
        item->GetItemType() == 0x17)
    {
        return;
    }
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // ORIG party==0 的 `je` 直落 end（无 nop 岛，+0xc5）⇒ 正向包裹而非早返。
    if (party != 0)
    {
        party->drop_item_gm(
            user, static_cast<unsigned int>(((const GmDFld<int>*)this)->f08));
    }
}