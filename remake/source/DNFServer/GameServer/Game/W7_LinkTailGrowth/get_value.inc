// stSeriaBlessingScript::get_value(int, Value&) const
// ORIG W 0x8693ad6：FindOneByLv(lv)（C1 0x822aebe / operator() 0x822aecc）
// 谓词 std::find_if 遍历 m_map0；谓词条件 = (m_minLv==m_maxLv && m_minLv==lv)
// 或 (m_minLv<=lv && lv<m_maxLv)。命中把 Value 五 int 拷入 out 返回 true。
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N93/N98，gap=-1/blocks=4）→ 试=①prett2/q3_gap2
//   缺口 0 ②strall 串 CLEAR（仅 base64 假阳性）③4.4.6 后端 overlay 复编（`-B cc1plus446bin`）
//   四数逐项等同 447（13/12/2/2/5）⇒ 非后端路由可治 ④n1_v 残差 = 两 Value 对象槽号互换
//   （ORIG pred@-0x18/begin@-0x10，4.4 恒反；声明序翻转已实测劣化，见块内注释）
//   ⇒ 维持 N（4.1 槽分配方向 vs 4.4 声明序，源码不可兼得）。
bool stSeriaBlessingScript::get_value(int lv, Value& out)
{
    // [b28d-solo-W7_LinkTailGrowth 判词·NEAR] ORIG 0x08693ad6（62/62）：pred/end/begin
    // 三对象的槽号与构造序不可兼得——ORIG pred@-0x18、end@-0x14、begin@-0x10 且构造序
    // pred→end→begin（4.1 先声明深）；4.4 恒先声明浅，故按 ORIG 构造序声明
    // （pred、itEnd、it）后 pred@-0x10/end@-0x14/begin@-0x18，仅 pred↔begin 槽号互换。
    // N93/N98 封顶：「先声明后赋值」形引入迭代器 C1Ev 默认构造 call + 临时槽拷贝
    // （72 insn，P50/N86），已实测回退；声明序再换会翻构造序（call 块序）反劣。
    // [N1B 2026-09-11 复核 n1_v 口径] 倒置声明序（it/itEnd/pred）需重排三声明，
    // 4.4 槽分配=声明序递减（it→-0x10/itEnd→-0x14/pred→-0x18 = ORIG begin/end/pred）
    // 但构造序随之翻成 begin→end→pred（ORIG pred→end→begin）⇒ 槽序/块序不可兼得。
    // [N1B 2026-09-11 实测·真倒置] 声明序改 it→itEnd→pred（构造序随之翻）已试：
    // 四数由 near=2/diff=2 **劣化**为 near=1/diff=3（get_value 转 DIFF，62/63 gap −1
    // 不变）⇒ 回滚，维持 pred→itEnd→it 声明序。
    FindOneByLv pred(lv);                                   // ORIG 构造序第 1（+0x13）
    // [M2S1·P331 复核] 把三者**全倒置**（it/itEnd/pred）能把槽号对齐 ORIG
    //   （4.4 先声明浅：it→-0x10、itEnd→-0x14、pred→-0x18，恰为 ORIG begin/end/pred），
    //   但 4.4 槽分配与求值序绑定 ⇒ call 块序会由 ORIG 的 pred→end→begin
    //   （0x08693ae9 pred、0x08693afb end、0x08693b10 begin）翻成 begin→end→pred：
    //   槽序与块序不可兼得 ⇒ N 封顶（与 L27-r3 实测同结论）。
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
}