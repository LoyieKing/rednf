// ---- _get_option_value —— T 0x085f2b02（同名报告全文转写）----------------
int CRandomOptionItemHandle::_get_option_value(int minV, int maxV)
{
    // [Lane17 G 判词] ORIG 0x085f2b02：**槽位集已逐值全等**（判据⑪ 直方图实测
    // {−0xc…−0x70} 两侧一致，含 x87 临时 −0x6c/−0x6e/−0x70），残余仅 1 条
    // `add -0xc(%ebp),%eax`（我方）vs ORIG 085f2c3d `mov -0xc(%ebp),%edx;
    // add %edx,%eax` 两段 ⇒ **N86b 封顶**（4.4 恒折内存直算，probe 已证源形不可择；
    // P243 求值序修法已于本轮落地）。语义完成。
    // [Lane17 L1] ORIG 085f2c40 把最终结果写回 -0x14（= 首段 r1 槽，085f2b34 同址）
    // ⇒ 原码单变量承载两段结果；我方新开 result 槽使全帧槽基深移 4（已修）。
    // [b28d-solo-RandomRentalA2 P0] bound 匿名临时形。
    int r1 = m_rand[7].randInt(
        (unsigned long)reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_ratioE4.m_c);

    // [Lane13] P278/P221：ORIG 085f2b49 把 CRandomListEx 构到 **-0x38**（浅槽）、
    // for_each 返回值 sret -0x50、get_value 直落 sel 槽 -0x60（两次 flds 读 sel
    // 内部 +0/+4）；具名 chooser 被 4.4 放深槽并顶浅全帧。
    OptionValueRatio sel =
        std::for_each(
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
                ->m_ratioE4.m_vec0.begin(),
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
                ->m_ratioE4.m_vec0.end(),
            CRandomListEx<OptionValueRatio>((unsigned int)r1))
            .get_value();

    // [Lane13] ORIG 无 fstps/flds 中间往返（085f2bf0 `flds -0x60` 直用 sel 槽、
    // 085f2c0e `flds -0x5c` 直读）⇒ 去具名 float 中间量，直接 fistpl 落 span/base。
    // [Lane17 G1] P243：ORIG 先 `fildl -0x6c`(maxV-minV) 后 `flds -0x60`(sel)
    // ⇒ 书写序「右式在前」（Lane17 已在 ROI::RegenVer_get_option_value 同族验证）。
    int span = (int)((float)(maxV - minV) * sel.m_scaleRange0);  // fistpl 截断（fnstcw 0xc）
    int base = (int)((float)minV * sel.m_scaleBase4);            // fistpl 截断
    // [b28f-RR 实测回退] base 改 unsigned（P274 档③）无效：4.4 仍折 `add mem,%eax`
    // 一条（ORIG 085f2c3d 为两段）⇒ 该段维持 N86b 封顶。
    // [K2ERandomBoss 2026-09-11 探针复核] /tmp/rr_probe_add.cpp 12 形（base+g()、
    // g()+base、(int)ug()、unsigned base、具名 r=g() 中转、(unsigned)+(unsigned)、
    // 三目/加 0 等）在 446 后端下**全部**产 `add 0x8(%ebp),%eax` 单条 ⇒ ORIG
    // 085f2c3d `mov -0xc(%ebp),%edx; add %edx,%eax` 两段为 4.1 独有装载形，N 封顶。

    // [Lane17 L1] ORIG 085f2c40 把最终结果写回 **-0x14 = 首段 r1 槽**
    // （085f2b34/085f2c42 两处同址）⇒ 原码用同一变量承载两段结果，
    // 我方新开 result 槽（-0x1c 于浅端）使全帧槽基深移 4（-0x54→-0x58 等）。
    // 复用 r1 消掉该槽。
    // [Lane17 L1b] ORIG 085f2c3d `mov -0xc(%ebp),%edx; add %edx,%eax`（左操作数
    // base 先物化）⇒ 源码写 `base + randInt(...)` 而非 `randInt(...) + base`
    // （4.4 对 call 结果在左的写法恒折 `add mem,%eax` 一条）。
    r1 = base + (int)m_rand[8].randInt((unsigned long)span);
    return r1;
}