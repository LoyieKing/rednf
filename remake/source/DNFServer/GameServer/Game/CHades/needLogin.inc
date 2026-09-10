bool CHades::needLogin()
{
    // b28d-solo-CHades verdict=NEAR [N22] ORIG 0x084b9718 orig/ours=28/28
    // （check_tu_routed 口径 NEAR，残差 1 处）。
    // 唯一残差 = cmp 操作数序：ORIG `cmp %eax,%ebx`（dst=缓存局部）vs 4.4
    // `cmp %ebx,%eax`（dst=call 结果）——N22「4.4 恒把 call 结果当 dst」；
    // 探针六形（操作数交换 / unsigned / short / !(==) / 具名 cur）全产
    // `cmpl %ebx,%eax`，不可翻。
    // setne 目标寄存器差（%al↔%bl）已消：删「register bool differ 具名局部」、
    // 条件直写 if（探针 fA setne %bl → fC setne %al）。register int cached 为
    // P231 纯存储类提示（非 asm 绑定），保 callee-saved 免槽跨 call 保活。
    register int cached = m_connSec;
    if (GlobalData::s_hades_proxy->GetHadesSocketConnSec() != cached)
    {
        Send_Login();
        StdCateUpdate(3, 0);
        return true;
    }
    return false;
