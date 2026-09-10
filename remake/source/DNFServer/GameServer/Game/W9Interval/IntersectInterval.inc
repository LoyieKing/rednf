bool IntersectInterval(Interval* result, const Interval& lhs, const Interval& rhs) // T @0x08573060
{
    // [b28d-lane15] ORIG 全寄存器、零局部槽（无 lhsEnd/rhsEnd 具名局部，P0/P183）：
    // 每个使用点重算 `start+size`；两个 early-return 独立（P133），两分支各带内层
    // `if (rhs.start < lhs.start+lhs.size)`（P225 拆嵌套 if，禁 && 合并）。
    // ORIG 两条件共享同一 FAIL 块（+0x2f `mov $0,%eax`）⇒ 合并短路（P133 反向用例：
    // 此处 ORIG 失败块唯一，独立 if 会生两个 FAIL 块各 +2 条）。
    if (lhs.m_start + lhs.m_size <= rhs.m_start ||
        rhs.m_start + rhs.m_size <= lhs.m_start)
        return false;

    // ORIG 两分支内层假路都落同一 ELSE1（+0xc6）⇒ 原码是 && 合并的 else-if 链
    // （4.4 对 `a && b` 直跳不物化，P225 边界）。
    if (lhs.m_start <= rhs.m_start &&
        rhs.m_start < lhs.m_start + lhs.m_size)
    {
        result->m_start = rhs.m_start;                  // 左端取 max(start)
        // ORIG 0x085730e4 `jge +0xa7`：真臂（rhsEnd-start）在远处、假臂紧跟
        // ⇒ 条件取反 + 两臂交换（P47/P59 块排布）。
        if (lhs.m_start + lhs.m_size < rhs.m_start + rhs.m_size)
            result->m_size = lhs.m_start + lhs.m_size - result->m_start;
        else
            result->m_size = rhs.m_start + rhs.m_size - result->m_start;
    }
    else if (rhs.m_start <= lhs.m_start &&
             lhs.m_start < rhs.m_start + rhs.m_size)
    {
        result->m_start = lhs.m_start;
        // ORIG 0x08573171 `jge +0x131`：真臂（lhsEnd-start）在远处 ⇒ 同法取反。
        if (rhs.m_start + rhs.m_size < lhs.m_start + lhs.m_size)
            result->m_size = rhs.m_start + rhs.m_size - result->m_start;
        else
            result->m_size = lhs.m_start + lhs.m_size - result->m_start;
    }
    // 未命中重叠窗口时 ORIG 落到 ret 1 且不写 result；此处与之一致。
    return true;
