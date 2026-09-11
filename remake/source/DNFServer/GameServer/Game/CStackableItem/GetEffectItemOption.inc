// ORIG 0x086946f4 (W 非 const)：读 +0x2e4(+0x2e6)，effect 时长(秒)/标志
// +0x2e4 为 CountinuousItemOption 子对象（m_option2e4：3 独立字节）。
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [S1Charac 2026-09-11 复核 · 与 GetCoolTimeOption 同族同判] 本波对 GetCoolTimeOption
// 实测 5 形（ptr-arith 视图 13 / 3B 子对象整体拷贝 14——唯一复现 ORIG `%ecx` 首读形但
// gcc 多保存 sret 指针到 %ebx 4 条 / 逗号表达式 11=基线 / 同型 aggregate 赋值 16 /
// 具名 const 引用 15）全部不可翻 ⇒ 本件（+0x2e4 同构）维持 10/11 N7，未重复试形。
// §6.5：本函数无 rodata 串引用 ⇒ CLEAR。
CStackableItem::CoolTimeOption CStackableItem::GetEffectItemOption()
{
    CoolTimeOption opt;
    opt.sec  = m_option2e4.m_sec16;    // b28c-L8：同 GetCoolTimeOption（ORIG 0x086946f4）
                                       // [b28d-solo-N7] fresh 10/11 维持 N7（同族同判词）。
    opt.flag = m_option2e4.m_field2;
    return opt;
}