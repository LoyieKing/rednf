unsigned int CCompound_ExtreamDun_Item::_FindItemRelateCreationRate(  // ORIG T 0x832d4b4
    std::vector<OUTPUT_ITEM_STR>::const_iterator begin,
    std::vector<OUTPUT_ITEM_STR>::const_iterator end) const
{
    if (begin == end)
    {
        return 0;
    }
    std::vector<std::pair<int, unsigned int> > entries;   // (权重, itemIdx)
    // ORIG 0x832d6e1 `mov $0x0,%edx; divl -0x1c(%ebp)` 直接内存操作数 ⇒
    // totalGen 声明为 unsigned int（int 时 4.4 先 `mov -0x1c,%ecx` 再 div）。
    unsigned int totalGen = 0;
    for (std::vector<OUTPUT_ITEM_STR>::const_iterator it = begin; it != end; ++it)
    {
        CItem* info = G_CDataManager()->find_item(it->m_index);
        if (info == 0)
        {
            continue;
        }
        // ORIG 0x832d52b 独立 `_Rb_tree_const_iterator::C1Ev`（rule 默认构造）
        // + 0x832d543 `end()`（具名 endIt 直接构造）+ 0x832d570 `find` 临时槽
        // -0x4c 再拷贝回 rule(-0x48) ⇒ 原码为「先声明后赋值」+ 具名 endIt
        // （P50/P28；初始化式会折叠 C1Ev 与临时槽）。
        std::map<int, forceRule_ResultItem>::const_iterator rule;
        std::map<int, forceRule_ResultItem>::const_iterator endIt =
            m_data.m_forceRules.end();
        rule = m_data.m_forceRules.find(it->m_index);
        if (rule != endIt)
        {
            // ORIG 0x832d5be 调 `pair<int,unsigned>::C1<int const&,int const&>`
            // （`_ZNSt4pairIijEC1IRKiS3_EEOT_OT0_`）——实参直接取
            // `&rule->second.m_value` / `&it->m_index` 引用，无 cast 临时物化；
            // make_pair + (unsigned)cast 会多 2 条 mov + 2 个局部槽（P61/P81）。
            entries.push_back(std::pair<int, unsigned int>(
                rule->second.m_value, it->m_index));
            totalGen += rule->second.m_value;
        }
        else
        {
            // ORIG 0x832d615 调 `pair<int,unsigned>::C1<int,int const&>`
            // （`_ZNSt4pairIijEC1IiRKiEEOT_OT0_`）：第一参按值（get_gen_rate
            // 右值）、第二参取 `&it->m_index` 引用。
            entries.push_back(std::pair<int, unsigned int>(
                info->get_gen_rate(), it->m_index));
            totalGen += info->get_gen_rate();
        }
    }
    unsigned int pick = 0;
    if (totalGen == 0)
    {
        // ORIG 0x832d67a `test %eax,%eax; setne %al; test; je <mov $0,%ebx>`
        // ⇒ 正极性（size()!=0 fall-through 到 randInt 路径），非 `==0` 早返
        //   （4.4 对 `==0` 生 sete 反向岛，P220/P47）。
        if (entries.size() != 0)
        {
            return entries[m_rand->randInt() % entries.size()].second;
        }
        return 0;
    }
    // ORIG 0x832d6dc `mov $0x0,%edx; divl -0x1c(%ebp)` 为**无符号**除法 ⇒
    // 两操作数均按 unsigned 取模（原 `(unsigned)(randInt() % totalGen)` 生
    // `sar $0x1f; idivl` 有符号除法，负数语义不同）。
    pick = (unsigned int)m_rand->randInt() % totalGen + 1;
    unsigned int acc = 0;
    for (unsigned int i = 0; i < entries.size(); ++i)
    {
        acc += (unsigned int)entries[i].first;
        if (pick <= acc)
        {
            return entries[i].second;
        }
    }
    return 0;
