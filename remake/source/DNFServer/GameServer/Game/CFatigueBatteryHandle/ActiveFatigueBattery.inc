// ORIG 0x84a83da：minLevel <= level 且 level < endLevel 时走 NotEndLevel 分支。
short CFatigueBatteryHandle::ActiveFatigueBattery(CUser* user)
{
    // b28d-20: ORIG 0x084a83da — sp = G_CDataManager()+0x68 存 -0xc 槽（ServerParameterScript&
    // 引用形，`add $0x68` 非折叠 +0x628）；两次 get_charac_level 结果均无栈槽（P0
    // 表达式右值直用，ORIG `call; cmp %eax,%ebx` 直接比较），min 字段先物化 %ebx 跨 call。
    // 守卫用合并 `&&`（P225：4.4 对 && 直产 `jg skip; jge skip` 两直跳；拆嵌套 if
    // 反产 setle/setl 物化，实测 40 vs ORIG 41 且多 4 条 setcc）。
    int result = 0;
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;
    if (static_cast<int>(sp.m_fatigueTable5c0.m_minChargeLevel9) <= user->get_charac_level() &&
        user->get_charac_level() < static_cast<int>(sp.m_fatigueTable5c0.m_endLevel10))
        result = ActiveFatigueBattery_NotEndLevel(user);  // ORIG 结果经 cwtl 截 short
    return static_cast<short>(result);
}