// ORIG 0x084b9718：缓存(m_connSec) 与 proxy 当前连接秒数不一致则重登并同步 cate=3。
bool CHades::needLogin()
{
    // b28d-solo-CHades verdict=NEAR [N22] ORIG 0x084b9718 orig/ours=28/28
    // （check_tu_routed 口径 NEAR，残差 1 处）。
    // [L6S4 round5b 复核] 读数维持 28/28；新探针 /tmp/l6s4b/probe_n2.o `_Z3nl3P1S`
    // （`register int cached = s->m_connSec; if (GP()->m_connSec != cached)`）逐条复现
    // 我方形 `mov (%eax),%ebx; …call…; mov (%eax),%eax; cmp %ebx,%eax; setne`
    // ⇒ 4.4 恒把 call 结果当 dst，ORIG 的 `cmp %eax,%ebx`（dst=缓存）不可翻，维持封顶。
    // 唯一残差 = cmp 操作数序：ORIG `cmp %eax,%ebx`（dst=缓存局部）vs 4.4
    // `cmp %ebx,%eax`（dst=call 结果）——N22「4.4 恒把 call 结果当 dst」；
    // 探针六形（操作数交换 / unsigned / short / !(==) / 具名 cur）全产
    // `cmpl %ebx,%eax`，不可翻。
    // setne 目标寄存器差（%al↔%bl）已消：删「register bool differ 具名局部」、
    // 条件直写 if（探针 fA setne %bl → fC setne %al）。register int cached 为
    // P231 纯存储类提示（非 asm 绑定），保 callee-saved 免槽跨 call 保活。
    register int cached = m_connSec;
    // [M4FreeNext15C.M4S1 round5 复验 · 维持 N22] ORIG 0x084b9732 `cmp %eax,%ebx`
    // （dst = 缓存局部 %ebx、src = call 结果 %eax，即后求值者不作 dst）。
    // 旧探针用成员访问形（`GP()->m_connSec`）；本轮改用**实调用**并交换源码
    // 操作数序（`cached != …GetHadesSocketConnSec()`）实编 —— 读数不变：
    // 仍 `cmp %ebx,%eax`（ndiff=2 ↔ 改前同值）⇒ 4.4 恒按“后求值者为 dst”生成，
    // 源码不可控，回滚原形。N22「4.4 恒把 call 结果当 dst」维持。
    if (GlobalData::s_hades_proxy->GetHadesSocketConnSec() != cached)
    {
        Send_Login();
        StdCateUpdate(3, 0);
        return true;
    }
    return false;
}