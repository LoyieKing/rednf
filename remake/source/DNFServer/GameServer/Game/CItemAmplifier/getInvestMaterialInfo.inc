// ORIG 0x08234d5e：按 type 索引 m_script.m_investMap[type]，查找 level。
// 命中则把 value.{m_amount,m_value} 写入 info，返回 1；未命中返回 0。
// [b28d-20] 返回类型 int→bool：ORIG dispatch_sig 0x08219cd4 为 xor $1（bool callee）；
// 返回类型不入 mangling，无 sizeof/偏移/vtable 变化。
// [b28d-solo-CItemAmplifier] verdict=NEAR（60/60，归一 diff 4）：N98 封顶。
//   ORIG 两个 hash_map 迭代器 it(-0x1c)/last(-0x14) 为「声明序=先声明深」，
//   4.4 按首次使用序「先用浅」⇒ 仅两处 end() 的 lea 槽号互换；两序互试
//   （it 先/8 行差、last 先/4 行差）已取优。
// [W15ANearHunt1 2026-09-11 收尾复核] routed 复编（10:27）n1_v=NEAR 60/60 gap+0
//   ndiff2，残差**仅**两处 end() 的 sret 槽互换：ORIG 0x08234d7f `lea
//   -0x1c(%ebp),%eax`（首次 end）/ 0x08234d94 `lea -0x14(%ebp),%eax`（二次 end），
//   我方顺序相反；0x08234dd1/0x08234dd8 的比较用 lea 随动。4.4 按「首次使用序」
//   先浅(-0x14)后深(-0x1c)，4.1 按「声明序」先深后浅 ⇒ 交换声明序两向均不可翻转
//   （本批 W13G `CalculateExpertJobMaterials`、W13A `get_limit_cube_item` 同族件
//   独立复现同一结论）⇒ 维持 N98 封顶，未改动代码。
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
    // [PoolH 2026-09-11 两试均回滚] ① P339 全头部声明（it 先/last 后）⇒ 迭代器
    // 先声明后赋值引入 2 次默认 ctor call，60→74 反劣；② P326 形（第二个迭代器
    // 不命名、`table.end()` 内联进比较）⇒ 60/60 但 near→diff 劣化。维持 N98 封顶
    // （本 TU 2026-09-11 复测：ndiff=2，两处 end() 的 lea 槽号互换）。
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
}