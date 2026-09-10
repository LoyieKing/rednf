bool CItemAmplifier::getInvestMaterialInfo(
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type,
    unsigned long level,
    stInvestAmplifyOption_t& info) const
{
    // ORIG 0x08234d64..0x08234e1b 逐条转写（无 md 报告，二进制切片）：
    //   ① 表址 = this + ((type*4+type)*4 + 0xa0) + 0xc（无边界检查，P140）；
    //   ② hash_map::end() **两次**（-0x1c 先作 it 初值、-0x14 作 last），
    //      find 结果以 POD 双 dword 覆盖 -0x1c（4.1 迭代器赋值发形）；
    // [b28d-34] N98 封顶（ndiff 8→2 NEAR）：4.1 槽序=声明序「先声明深」
    // （it=-0x1c/last=-0x14），4.4 按首次使用序「先用浅」且不可由声明序翻
    // （两序互试实测）。交换声明序（last 先）后仅剩两处 end() 的 lea 槽号
    // 互换（-0x14↔-0x1c），find 结果槽与后段逐条全等，语义等价。
    //   ③ it == last → 0；
    //   ④ 命中段**逐成员**赋值：两次 _Hashtable_const_iterator::operator->
    //      分别取 pair+4 / pair+8（⇒ `info = it->second` 整体拷贝会丢一次
    //      operator-> 并改走结构复制路径）。
    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>&
        table = m_script.m_investMap[(int)type];
    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator
        last = table.end();
    __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator it =
        table.end();
    it = table.find(level);
    if (it == last)
    {
        return 0;
    }
    info.m_amount = it->second.m_amount;
    info.m_value = it->second.m_value;
    return 1;
