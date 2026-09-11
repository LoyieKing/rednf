// ---- RegenVer_get_option_value —— T 0x085f8da6（同名报告全文转写）---------
int RegenerationROI::RegenVer_get_option_value(int minV, int maxV)
{
    // [Lane17 G 判词] ORIG 0x085f8da6：槽位集已逐值全等（判据⑪ 直方图
    // {−0xc…−0x40} 两侧一致）。本轮命中：①判据⑩/P183 删 ratios 引用缓存
    // （ORIG 无该引用槽，每访问点 `mov 0x8(%ebp),%eax; lea 0x40(%eax),%edx` 全链，
    // -0x1c 槽在 ORIG 是 r）；②P243 乘法书写序（ORIG 085f8e6c 先 `fildl -0x3c`(maxV-minV)
    // 后 `flds -0x30`(sel)）；③语句序 acc=0 先于 sel C1Ev（ORIG 085f8dcc < 085f8dd3）；
    // ④P0 内联 bound2/r2（ORIG 无该两具名槽）。
    // [F2e-Singles2 C05② 复核 2026-09-10] 本件比较双方 r/acc 均为**具名 int 局部**
    // （C05② 命名命中形），但 int 态已是最优形（我方仍两载 `mov r,%edx; mov acc,%eax;
    // cmp`）且 unsigned 向已试反劣（b28f：`cmp -0x1c(%ebp),%eax; jb` 极性/方向双反）
    // ⇒ S1 一次性旋钮已用尽，维持 N86b（P209 unsigned 比较恒载寄存器；改 signed 会翻
    // ja→jg 触 P196）。our 92 = ORIG 92，四数不变。
    // **残余 N86b 封顶**：比较段 ORIG `mov -0x1c(%ebp),%eax; cmp -0x18(%ebp),%eax; ja`
    // 单段 vs 我方两载入；尾 `mov -0x10(%ebp),%edx; add %edx,%eax` 两段 vs 我方
    // `add -0x10(%ebp),%eax` 折叠（r/acc 改 unsigned 实测反劣已回退）。语义完成。
    // [K2ERandomBoss 2026-09-11 探针复核] /tmp/rr_probe_add.cpp 12 形在 446 后端
    // 下全部折 `add mem,%eax` 单条 ⇒ 尾 ADD 两段为 4.1 独有；比较段两载为 4.4
    // unsigned 槽比恒载寄存器（P209）⇒ 两段同判跨版本 N，维持封顶。
    // [SweepRandomRental 修正] ORIG 0x085f8e7b `mov $0xc,%ah; mov %ax,-0x40;
    // fldcw -0x40; fistpl`：控制字 RC=11 = **向零截断**（Ghidra 的 ROUND 系误标）。
    // 旧 ROUND(+0.5) 形在正/负小数上语义偏离 ⇒ 改回 (int) 截断。
    unsigned long bound = (unsigned long)m_regenerator.m_field48;
    // [Lane17 L5 已试并回退] r/acc 改 unsigned 后：`acc += (unsigned)…` 被 4.4 强度
    // 归约为 4 条 `mov %eax,%edx; mov -0x18,%eax; lea (%edx,%eax,1),%eax; mov %eax,-0x18`
    // （ORIG 单条 `add %eax,-0x18`），比较极性也翻成 `jb` ⇒ 净负，回退 int 形。
    // 残余比较段/尾加法的两段形记 N86b（4.4 恒折内存直算；probe 已证不可择）。
    int r = m_regeneratorPtr->randInt(bound);

    // [SweepRandomRental 修正] ORIG 0x085f8dd3 `lea -0x30(%ebp),%eax; mov %eax,(%esp);
    // call _ZN16OptionValueRatioC2Ev` ⇒ 原码是 OptionValueRatio 局部对象（三字清零），
    // 非两个独立 float 局部（旧形 p167 MISSING C2Ev ×1）。
    // 字段来源按 ORIG 逐条核对：本函数（ROI）与 handle::_get_option_value 的取用**相反** ——
    //   ROI 0x085f8e1f `mov 0x4(%eax),%eax; mov %eax,-0x30` ⇒ span 系数取元素 **+0x04**；
    //   ROI 0x085f8e3a `mov (%eax),%eax;   mov %eax,-0x2c` ⇒ base 系数取元素 **+0x00**。
    // （handle::_get_option_value 0x085f2bf0/0x085f2c0e 才是 span=+0/base=+4，两处不同。）
    // [Lane17 L3] ORIG 语句序为 r → `acc=0`(0x085f8dcc) → sel C1Ev(0x085f8dd3)
    // → i=0 ⇒ 原码 acc 声明在 sel 之前（我方旧序 sel→acc 使两语句次序相反、
    // 并把 acc 挤到 -0x1c）。判据⑩：槽序亦要求 acc 浅于 sel。
    int acc = 0;
    OptionValueRatio sel;
    // [Lane17 L2] 按判据⑩重核：ORIG 无 ratios 引用局部（每访问点 `mov 0x8(%ebp),%eax;
    // lea 0x40(%eax),%edx` 全链，-0x1c 槽在 ORIG 是 r），我方 `&ratios` 缓存使
    // bound/r/acc/sel 全体深移 4（-0x24→-0x28 等）。旧「删引用 96 条反劣」判词
    // 只按指令数、未按槽位归位口径 ⇒ 重做。
    for (std::size_t i = 0; i < m_regenerator.m_vec3c.size(); ++i)
    {
        acc += (int)m_regenerator.m_vec3c[i].m_weight8;
        // [PoolB b28f R2 实验·负结论] ORIG 0x085f8e05 比较段为「载 r →`cmp acc,mem`→`ja`
        // 跳 i++」；实测改为正极性 continue（`if ((unsigned)r > (unsigned)acc) continue;`）
        // 4.4 **仍产两载入 + jbe**，且块内多 2 条（92→93、ndiff 2→3）⇒ 已回退。
        if ((unsigned int)r <= (unsigned int)acc)
        {
            sel.m_scaleRange0 = m_regenerator.m_vec3c[i].m_scaleBase4;  // 对象+0 ← 元素+0x04
            sel.m_scaleBase4 = m_regenerator.m_vec3c[i].m_scaleRange0;  // 对象+4 ← 元素+0x00
            break;
        }
    }

    // [SweepRandomRental] P0：ORIG 0x085f8e6c..0x085f8e84 为 fildl/flds/fmulp/
    // fldcw/fistpl 直落（无 float 中间槽往返）⇒ 表达式内联，禁具名 spanf/basef。
    // [Lane17 L3] P243：ORIG 先 `fildl -0x3c`(maxV-minV) 后 `flds -0x30`(sel)
    // ⇒ 书写序为「右式在前」。我方旧式 sel*… 使 4.4 先 flds sel（求值序反）。
    int span = (int)((float)(maxV - minV) * sel.m_scaleRange0);  // ORIG fldcw RC=11：截断
    int base = (int)((float)minV * sel.m_scaleBase4);
    // [b28f-RR 实测回退] base 改 unsigned（P274 档③）无效：ORIG 085f8eb5 两段形
    // 不可由源侧类型旋钮复现 ⇒ 该段维持 N86b 封顶。

    // [Lane17 L4] ORIG 无 bound2/r2 具名槽（085f8e9b `mov -0x14,%eax; mov %eax,-0x20`
    // 只是 `(unsigned long)span` 转换临时；085f8eb5 结果直接落 r 槽 -0x1c）。
    // 我方具名 bound2 把 span/base/acc/r 挤深 4（-0x14→-0x18 等）⇒ 内联（P0）。
    // [SweepRandomRental] ORIG 0x085f8eba `mov %eax,-0x1c(%ebp); mov -0x1c(%ebp),%eax`
    // = 复用 r 槽的 store/reload 往返（原码有具名局部）⇒ 赋值回 r 再 return。
    r = base + (int)m_regeneratorPtr->randInt((unsigned long)span);
    return r;
    // [b28f-RR 实测] r 改 unsigned（P310 方向）后比较段翻为 `cmp -0x1c(%ebp),%eax; jb`
    // （ORIG 为 `mov -0x1c,%eax; cmp -0x18,%eax; ja`）—— 极性/方向双反，撤回首段
    // `mov -0x1c,%eax` 反而丢失 ⇒ 维持 N86b 封顶。
}