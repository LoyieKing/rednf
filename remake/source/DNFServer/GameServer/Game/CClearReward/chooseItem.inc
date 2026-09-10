int CItemGenRateTable::chooseItem(eItemDropType_t type, ENUM_RARITY rarity, int level)
{
    // [b28d-solo-CClearReward] 逐条对齐 ORIG 0x085349a4：
    //  ①首判 `cmpl $0x2,0xc(%ebp)` 是 **type==2**（0xc=第一形参），且 choose_item
    //    第一参压 `lea 0x10(%ebp)`（形参 rarity 的引用）；itemIdx 预置 -1 再赋值
    //    （P198① 预清两句式）。
    //  ②`call size; test; sete; test; je` ⇒ 源形 `size()==0`（P197④），非 empty()。
    //  ③迭代器先默认构造（C1Ev）后赋 end()（P50 复活）；`it--` 为**后置**
    //    （ORIG 调 operator--(int)，带 0 哑元 + sret 临时）。
    //  ④槽址每次重算（三处 imul 链），故不缓存 map 引用（P183）。
    //  ⑤try 仅覆盖 lower_bound..it->second（ORIG .gcc_except_table 08534b11..08534b8b）；
    //    catch 用 __PRETTY_FUNCTION__ 恢复 `_ZZ...E19__PRETTY_FUNCTION__` 局部静态
    //    （P139/P118），格式串 "[%s][%d]"、行号 0x279。
    if (type == eItemDropType_2)
    {
        int itemIdx = -1;
        itemIdx = static_cast<int>(m_optHandle.choose_item(rarity, level));
        // P225：ORIG `cmpl $0xffffffff,-0xc; setne %al; test %al,%al; je` 的
        // 4.1 bool 物化形由 while+return 复现（普通 if 在 4.4 恒直跳 je）。
        while (itemIdx != -1)
        {
            return itemIdx;
        }
    }
    if (m_slots[type][rarity][level].size() == 0)
        return -1;
    std::map<unsigned int, int>::iterator it;
    it = m_slots[type][rarity][level].end();
    it--;
    unsigned int maxKey = it->first;
    unsigned int roll = static_cast<unsigned int>(m_rand->randInt(maxKey));
    int result = -1;
    try
    {
        it = m_slots[type][rarity][level].lower_bound(roll);
        if (it == m_slots[type][rarity][level].end())
            it--;
        result = it->second;
    }
    catch (...)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x279, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x279);
    }
    return result;
