bool CompiledDNFLex::PopInputStream(bool pop_all)
{
    // [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：本 TU 路由 4.4.6 -O2，迭代器 16B
    // 与 ORIG 一致（0x2030/0x2034/0x203c 偏移全等，旧 N108「12B vs 16B」不适用）。
    // 实测 ORIG 0x8ad0c80 134 条 vs 我们 172 条，差异三类：
    // ①ORIG 的 `_M_pop_back_aux` 路径 call out-of-line `_ZN14CompiledDNFLex8stream_tD1Ev`
    //   （ORIG nm 为 W 0x8d @0x8ad1320），4.4.6 内联该隐式析构（两棵 COW string 的
    //   引用计数递减，~30 条/处）；用户声明 ~stream_t() 已证更劣（check_dnflex_set.py
    //   :182 注释：会内联 ~deque 进 Pop/D1），属编译器内联决策差；
    // ②prologue：ORIG `sub $0x48,%esp` + 栈槽保存 ebx/esi/edi，我们 push×3+sub 0x3c（N7 族 RA）；
    // ③`cmp %eax,%edx` vs `cmp %edx,%eax` 操作数序（N18b）。
    // 栈空/单元素/多元素三分支、delete[] buffer、pop_back、offset_ 回写语义全等 ⇒ N 封顶。
    // [b28d-solo 复验 2026-09-09] gnu++0x 路由后 fresh .o：134 vs 172，ndiff 83。
    // p167 差集：MISSING `_ZN14CompiledDNFLex8stream_tD1Ev` ×1、EXTRA
    // `_ZNSs4_Rep10_M_destroyERKSaIcE` ×2（=①内联展开的可见后果，语义同）。
    // 试形：`~stream_t() __attribute__((noinline)) {}`（头内）⇒ D1Ev/deque D1Ev 两件
    // 转 DIFF（ae 25→23），已回退 ⇒ 内联决策差不可源码控，维持 N。
    if (streamStack_.empty())
        return false;
    if (!pop_all)
        goto check_size;
do_pop:
    {
        stream_t& top = streamStack_.back();
        if (top.buffer != 0)
            delete[] top.buffer;
        streamStack_.pop_back();
        if (!streamStack_.empty())
            offset_ = streamStack_.back().offset;
        return true;
    }
check_size:
    if (streamStack_.size() > 1)
        goto do_pop;
    return false;
