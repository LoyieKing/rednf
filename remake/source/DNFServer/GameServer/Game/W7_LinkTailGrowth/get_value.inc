bool stSeriaBlessingScript::get_value(int lv, Value& out)
{
    // [b28d-solo-W7_LinkTailGrowth 判词·NEAR] ORIG 0x08693ad6（62/62）：pred/end/begin
    // 三对象的槽号与构造序不可兼得——ORIG pred@-0x18、end@-0x14、begin@-0x10 且构造序
    // pred→end→begin（4.1 先声明深）；4.4 恒先声明浅，故按 ORIG 构造序声明
    // （pred、itEnd、it）后 pred@-0x10/end@-0x14/begin@-0x18，仅 pred↔begin 槽号互换。
    // N93/N98 封顶：「先声明后赋值」形引入迭代器 C1Ev 默认构造 call + 临时槽拷贝
    // （72 insn，P50/N86），已实测回退；声明序再换会翻构造序（call 块序）反劣。
    FindOneByLv pred(lv);                                   // ORIG 构造序第 1（+0x13）
    // [L27-r3·判据⑩ 已实测] 声明序翻转（it 先 / itEnd 后）对本件**无效**：两槽仍互换
    //   （ORIG itEnd=-0x18/it=-0x10 vs 我方恒反）且 call 块序受影响 ⇒ 维持原声明序、判 N。
    std::map<int, Value>::iterator itEnd = m_map0.end();    // ORIG 构造序第 2（+0x25）
    std::map<int, Value>::iterator it = m_map0.begin();     // ORIG 构造序第 3（+0x3a）
    std::map<int, Value>::iterator found = std::find_if(it, itEnd, pred);  // ORIG：返回值独立变量
    if (found == m_map0.end())   // ORIG：if 条件中再次调用 end()
        return false;
    // P167 新判据：ORIG 发 `_ZNK…deEv`（operator*）而非 `ptEv`（operator->）
    // ⇒ 源形必须写 `(*it).second`；`it->second` 会多一条 EXTRA ptEv 实例化。
    out = (*found).second;       // ORIG：Value 整体赋值（五 int 逐字段展开）
    return true;
