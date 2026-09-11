// ---------------------------------------------------------------------------
// ORIG 0x0810ac04：SetEventFlag(true) + 整 4 字节写 param 到 +0x09 +
// cMyTrace(__FUNCTION__,0x41,0) 输出 4 个档位字节
// ("Coin Event Per Day Start! coin(%d) coin(%d) coin(%d) coin(%d) ")。
// b28d-76（N76 封顶，语义等价）：ORIG `lea 0xc(%ebp),%edx; mov 0x8(%ebp),%eax;
// mov (%edx),%edx; mov %edx,0x9(%eax)` = 4.1 求值序「RHS 地址 → LHS 地址 → RHS 值」；
// 4.4-O0 对 `m_coinPack = *(unsigned int*)&param;` 恒 RHS 完整求值先
// （`lea ...,%eax; mov (%eax),%edx; mov this,%eax`）。20 形探针（volatile 读/写、
// 视图/引用解引用、字段 int↔unsigned 同异型化、数组下标、语句表达式、具名中转）
// 只能产「this 先」或「RHS 值先」两族，无一复现 ORIG 前两条顺序 ⇒ N76。
// [N5C_EventChar 2026-09-11 复核] 维持 N76 封顶，新增负结论（fresh NEAR 48/49，残差仅
// `lea 0xc(%ebp),%eax; mov (%eax),%edx` vs ORIG `lea 0xc(%ebp),%edx; … mov (%edx),%edx`）：
// ① P226 裸语句表达式 `*({ (const unsigned int*)&param; })`、具名地址局部、union 视图、
//    char* 算术（探针 probe4.cpp H2/H3/H4/H5）与基线逐条同形；
// ② 字段/RHS 异型化（probe_hetero.cpp I/J：int 字段或 (const int*) 视图）反多 1 条
//    `mov %eax,%edx`（5 条 vs ORIG 4 条）⇒ 禁用；
// ③ 残差性质 = 4.1 用 edx 同时承载 RHS 地址与值、4.4 恒用 eax 做地址（N18a/N7 寄存器角色族）。
void CCoinEventPerDay::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_coinPack = *(const unsigned int*)&param;
    cMyTrace(__FUNCTION__, 0x41, 0)(
        "Coin Event Per Day Start! coin(%d) coin(%d) coin(%d) coin(%d) ",
        m_coin01, m_coin02, m_coin03, m_coin04);
}