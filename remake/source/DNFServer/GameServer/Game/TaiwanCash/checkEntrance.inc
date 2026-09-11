// ---- checkEntrance(const char*, char&) —— ORIG T 0x081736ce ----
int TaiwanCash::checkEntrance(const char* account, char& entrance)
{
    (void)entrance;  // ORIG 未写回该出参（汇编无 store，仅 trace 结果码）——忠实转写

    EntranceQuery query;
    // EntranceQuery: m_resultCode@+0x04
    if (!processBilling<EntranceQuery>(query))
        return 4;
    if (!checkResultCode(query.m_resultCode))
        return query.m_resultCode;

    // [b28d-47] P227/P221：ORIG 0x0817371e `mov -0x1c(%ebp),%ebx` 把 m_resultCode
    // 在 cMyTrace ctor 前物化并跨 ctor 以 %ebx 保活 ⇒ 全表达式临时链形（具名 trace
    // 对象会令实参在 ctor 后求值且落槽，帧 -0x38 vs ORIG -0x34）。
    cMyTrace(__FUNCTION__, 0x34, 0)(
        "[Taiwan, Billing] EntranceQuery Entrance:%d", query.m_resultCode);
    return 0;
}