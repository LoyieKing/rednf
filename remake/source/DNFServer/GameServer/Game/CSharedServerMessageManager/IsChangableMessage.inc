bool CSharedServerMessageManager::IsChangableMessage(int nIndex, int nTime)
{
    // [N6S2NegB 2026-09-11] ORIG 0x08600cc8：槽往返仅 pMsg 一处（ORIG -0xc(%ebp)
    // `mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp)`）；pMsg->m_endTime(+0x54)+0x78
    // 与 nTime 的**有符号严格小于** `cmp 0x10(%ebp),%eax; jl <RET1 岛>` 走 RHS 值
    // 先物化（P0/P85）。tail 形判定（/tmp/probe_icm p1..p3 同旗标探针）：
    //   源码 `if (cond) goto ret_true;` + `ret_true: return true;` ⇒ 4.4.7 在
    //   jl 目标前补 1 条对齐 nop（24 条，DIFF +1，即本件原残差）；
    //   `if (cond) return true; return false;` ⇒ 反向 `jge` 跳 return-0 岛（DIFF）；
    //   **反向早返 `if (cond) return false; return true;` ⇒ `jl` 直跳尾岛
    //   `mov $1,%eax`、fall-through 为 `mov $0,%eax; jmp`（23 条，与 ORIG 逐条一致）**。
    // 故取反向早返形；`>=` 保持有符号（改 unsigned 会变 jae 反劣）。
    stSERVER_MESSAGE* pMsg = getCurMessage(nIndex);
    if (pMsg == 0)
        return true;
    if (pMsg->m_endTime + 0x78 >= nTime)
        return false;
    return true;
}