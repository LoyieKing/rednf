// ============================================================================
// 2) _Z24isValidAttackInfoElementP23STComputeDamageVariablei @0x0809576e (0x55)
//    elem==4：依次 test 0xa/0xb/0xc/0xd（m_elemEnable[0..3]），任一非零返 0，否则 1；
//    其它：movzbl 0xa(%edx,%eax) → m_elemEnable[elem] != 0。
// ============================================================================
// 返回 bool：ORIG 尾 setne %al 直返无 movzbl 规范化（P9）
// [b28 BM2 判词·AE] 清单 NEAR(2) 为 jmp 目标偏移标签伪差（ORIG 双 epilogue 岛
// +0x41/+0x53 vs 4.4 共享尾，跨版本 epilogue merge 族）；fresh /tmp/tu_SDCLink.o
// fn_diff_tu 归一 0 差（31=31 逐条）。
// [b28d-lane15] check_tu_routed 复核：本件不在 near/diff 列表（identical/ae）；
// game_func_report 的 DIFF 系 jmp 目标标签归一伪差，维持 AE。
bool isValidAttackInfoElement(STComputeDamageVariable* v, int elem)
{
    if (elem == 4)
    {
        if (!(v->m_elemEnable[0] || v->m_elemEnable[1] ||
              v->m_elemEnable[2] || v->m_elemEnable[3]))
            return true;
        return false;
    }
    return v->m_elemEnable[elem] != 0;
}