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
