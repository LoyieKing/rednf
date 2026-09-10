int CDataManager::VerifyDungeon(const CDungeon& dungeon) const
{
    // ORIG T 0x08362b44：①后置 `it++`（ORIG @+0x45 发 iterator::operator++(int)
    // 带 0 实参 + 返回临时槽 -0x10）；②it->second 先落 -0xc 槽再回读（P0 具名
    // 局部）；③find_item 结果 `test %eax,%eax` 无消费者（P184 死测，4.4 需
    // goto 岛形才发射，P223）；④循环条件 op!=(it, end)（end 临时 -0x14）。
    for (std::map<int, int>::const_iterator it = dungeon.m_clearItemMap.begin();
         it != dungeon.m_clearItemMap.end(); it++)
    {
        int itemIdx = it->second;
        if (find_item(itemIdx))
            goto ITEM_FOUND;
    ITEM_FOUND:
        ;
    }
    dungeon.VerifyMaze();
    return 1;
