void CSecialIMonsterDropItemGenRateTable::generateTable(stGenItemRange_t* range)
{
    // [SweepDropLayout 2026-09-09] AE（ORIG 220 = 我方 220，ndiff=0），p167 CLEAR（17=17）。
    // 旧判词「已试 callee bool 化→回退」被推翻：真因是**字段类型**未 bool 化。
    // ORIG 谓词定义体 0x8539e10/0x8539e24 `movzbl 0xb5/0xba(%edx,%eax),%eax; ret`
    // （7 条无 setne）+ 调用点 0x8534ee4/0x8534ee7 `call; xor $1; test %al,%al; jne`
    // （P292 ⇒ callee bool）⇒ 原码字段即 bool（bool 字段+bool 返回=7 条；char 字段+
    // bool 返回=9 条带 test/setne，探针 /tmp/p_boolf.cpp f/g/h/k）。
    // 落地：STItemScript.h m_difficulty[5]/m_dungeonType[3] char→bool +
    // CItem.h 两谓词返回 unsigned char→bool（Main 批准 + 全广播）。
    // sizeof(STSpecialMonsterDrop)=9、偏移/mangled 零变化；写入点仍 movb 常量。
    // 已修：同 CItemGenRateTable::generateTable（去局部缓存、end 条件块、直接构造
    // pair、const grades、m_blocks 用 level）+ 数组指针解引用视图复现 ORIG 0x08534d2f
    // `add %eax,%eax; add 0xc(%ebp),%eax; movzbl (%eax)`/`0x1(%eax)` disp 折叠。
    // 报告 generateTable.md（T 0x08534cd0）：五重循环 a(0..4) b(0..2) type(1..4)
    // rarity(0..5) level(1..200)；槽址 a*0x69ff0+b*0x23550+type*0x7110+
    // rarity*0x12d8+level*24。过滤要求 isSpecialMonsterDropItem /
    // isDropItemDungeonDifficulty(a) / isDropItemDungeonType(b) 全真，无随机选项分支。
    const __gnu_cxx::hash_map<int, CItem*>* grades =
        G_CDataManager()->m_itemList->getItemGradeList();
    for (int a = 0; a <= 4; ++a)
    {
        for (int b = 0; b <= 2; ++b)
        {
            for (int type = 1; type <= 4; ++type)
            {
                for (int rarity = 0; rarity <= 5; ++rarity)
                {
                    for (int level = 1; level <= 200; ++level)
                    {
                        unsigned int cumulative = 0;
                        int offset = -(int)(*reinterpret_cast<
                                          const unsigned char(*)[2]>(
                                          &range->m_range[level]))[0];
                        for (; offset < (int)(*reinterpret_cast<
                                             const unsigned char(*)[2]>(
                                             &range->m_range[level]))[1]; ++offset)
                        {
                            if (level + offset > 200)
                                break;
                            for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                                     it = grades[level + offset].begin();
                                 it != grades[level + offset].end(); ++it)
                            {
                                CItem* item = it->second;
                                if (item->get_gen_rate() == 0)
                                    continue;
                                if (item->get_rarity() != rarity)
                                    continue;
                                if (!_itemTypeVerify(type, item))
                                    continue;
                                if (!item->isSpecialMonsterDropItem())
                                    continue;
                                if (!item->isDropItemDungeonDifficulty(a))
                                    continue;
                                if (!item->isDropItemDungeonType(b))
                                    continue;
                                cumulative += (unsigned int)item->get_gen_rate();
                                m_blocks[a][b][type][rarity][level].insert(
                                    std::pair<const unsigned int, int>(
                                        cumulative, item->get_index()));
                            }
                        }
                    }
                }
            }
        }
    }
