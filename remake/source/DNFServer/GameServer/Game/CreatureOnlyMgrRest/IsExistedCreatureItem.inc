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