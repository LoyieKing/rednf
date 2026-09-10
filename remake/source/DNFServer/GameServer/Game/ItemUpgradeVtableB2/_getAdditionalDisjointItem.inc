void CItemUpgrade::_getAdditionalDisjointItem(unsigned int up,
                                              const CItem* itemScript,
                                              std::vector<std::pair<int, int> >& items,
                                              const upgrade_table_t& table)
{
    // 【切片转写 0x08546938..0x08546a95】表项全部走具名字段直访（inline
    // 访问器在 -O0 每点额外发 call）；**语义纠错**：pow 的底数是
    // `up - table.m_additionalMinUpgrade`（0x085469db..0x085469f8：先取 +0x258，
    // 与形参 up(0xc(%ebp)) 相减、符号扩展成 long long 后 fildll），
    // 此前误用钳零后的 grade 差。
    // [b28d-solo-ItemUpgradeVtableB2] P0/P48：ORIG 0x08546944 `mov %eax,%ebx`
    // ⇒ grade 无栈槽（值跨 get_rarity 调用由 %ebx 保活）⇒ 不设具名局部，
    // 直接内联进 diff 表达式。
    // 0x0854696f..0x0854698a：`lea -0x24(&diff); lea -0x20(&zero); call max;
    // mov (%eax)` ⇒ ORIG 用 std::max 模板（两实参各取地址，故 zero 必须是
    // 具名 int 局部）；写成 if 钳零会少 6 insn。
    // get_rarity **两次独立调用**（0x0854695a 算 grade 差、0x085469bf 选权重
    // 槽）⇒ 不命名 rarity 局部（P48/P183）。
    int diff =
        itemScript->get_grade() -
        table.m_bodyView.m_named.m_baseGrade[itemScript->get_rarity()];
    int zero = 0;
    // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x0854696f..0x0854698a 压参序：
    // 0x4(%esp)=&diff、(%esp)=&zero ⇒ 调用是 max(zero, diff)（4.4 右起求值
    // 时先取 &diff，与 ORIG 装载序一致；max 可交换，语义等价）。
    const int clamped = std::max(zero, diff);

    // ORIG 0x0854698d..0x085469a2：`mov -0xc,%edx; mov 0x23c(%eax),%eax;
    // cmp %eax,%edx; jb end` ⇒ 源码写 `clamped >= gap`（4.4 对 `a>=b` 发 jb）。
    if ((unsigned int)clamped >=
            (unsigned int)table.m_bodyView.m_named.m_additionalGradeGap &&
        (unsigned int)table.m_bodyView.m_named.m_additionalMinUpgrade < up)
    {
        // ORIG 的 make_pair 实参是 `Rm Rj`（unsigned long& / unsigned int&
        // 左值）⇒ pair 由转换 ctor C2<unsigned long,unsigned int> 落成
        // pair<int,int>（0x08546a5c/0x08546a71 两个 call）⇒ 两量必须是有类型
        // 的具名局部，不能是表达式直传。
        unsigned long itemId =
            (unsigned long)table.m_bodyView.m_named.m_additionalItemId;
        // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x085469bf..0x085469ca 为
        // `flds (%eax,%edx,4); fstps -0x4c(%ebp)`（4.1 的 float 拷贝经 x87），
        // 4.4 恒 mov 位直搬（volatile 形实测反而多 flds+fmulp 一条，且初始化
        // 仍为 mov）⇒ N85① 封顶（跨版本 float 拷贝 lowering 差异）。
        const float weight = table.m_bodyView.m_named
                                 .m_additionalWeight[itemScript->get_rarity()];

        // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x085469db..0x08546a01：
        // `mov 0x258(%eax),%eax; mov 0xc(%ebp),%edx; mov %edx,%ecx;
        //  sub %eax,%ecx; mov %ecx,%eax; mov $0x0,%edx; mov %eax,-0x40;
        //  mov %edx,-0x3c; fildll -0x40` = **unsigned int → double** 的零扩展
        // 转换（32 位 unsigned 不能走 fildl）⇒ 底数类型是 unsigned int，
        // 不是 (long long) 相减（那会出 sbb 链）。
        // ORIG 0x08546a02..0x08546a39：pow 结果直接 `fnstcw/fldcw(RC=trunc)`
        // 后 fistpl（截断，**无 +0.5**）；再 fildl + fmuls weight +
        // fistpll（64 位截断）⇒ 源码是两次 (int)/(long long) C cast。
        // span 无具名槽（ORIG 0x085469d1 用 %ecx/%eax 直传）⇒ 内联表达式。
        const long long scaled =
            (long long)((float)(int)std::pow(
                            (double)(unsigned int)(
                                up - table.m_bodyView.m_named
                                         .m_additionalMinUpgrade),
                            2.0) *
                        weight);
        unsigned int count = (unsigned int)scaled;
        if (count != 0)
        {
            // ORIG 0x08546a4a..0x08546a6e：make_pair<Rm,Rj>（两左值）→
            // pair<int,int> 转换 ctor → push_back(&&) 三连 call（P178 嵌套形；
            // 直写 make_pair 只发两 call）。
            items.push_back(
                std::pair<int, int>(std::make_pair(itemId, count)));
        }
    }
