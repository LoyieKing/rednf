// ORIG 0x084a84b6：
//   idx = level-1；idx > (int)m_endLevel10（字节零扩展）→ 返回 0；
//   level <= 0 → 返回 0；
//   否则返回 *(u16*)(&m_field2 + idx + 8)，与 ORIG 的
//   movzwl 0x2(this,idx+8,2) 同址。
unsigned short stFatigueBatteryConstantTable_t::getLimitGrownupBuffCntFatigueBattery(
    int level) const
{
    // [b28d-solo-CFatigueBatteryHandle] verdict = N1 封顶（跨版本常量折叠），
    // 非 identical 唯一成因；语义等价（下标恒等），p167 CLEAR（0 call，gap=1）。
    // [L6S4 round5b 复核] 读数维持 20/19；新探针 /tmp/l6s4b/probe_n.o `_Z3limPK1Ti`
    // 逐条复现我方形（`lea 0x7(%eax),%edx` 单步），ORIG 0x084a84dd 为
    // `lea -0x1(%eax),%edx; add $0x8,%edx` 两步 —— 4.4 常量折叠不可逆（N1），
    // 视图/拆形（含 P311 成员数组视图）均不改变索引寄存器的常量拆分 ⇒ 维持封顶。
    // b28d-solo 复证（19 形探针 /tmp/s99/p1.cpp+p2.cpp+p3.cpp，4.4.7 -m32 -O0）：
    // ORIG 尾段 `lea -0x1(%eax),%edx; add $0x8,%edx; movzwl 0x2(%eax,%edx,2),%eax`
    // 为 (level-1)+8 两步；数组下标形（A/B/D/E/G/J/M/N/O/P/Q/R 共 12 形，含
    // 8u 混合类型 / (8,8) 逗号 / 成员指针差 / sizeof 常量源 / (int)8L）恒折
    // `lea 0x7(%eax),%edx`；指针算术形（C/F/H/I/K/L 6 形）反产 add/add/lea 四段；
    // (unsigned char) 截断形（T）产 subl+movzbl+lea 三段，均不闭合。
    // b28d-20: ORIG 0x084a84b6 双守卫合并短路形——ORIG `jg fail; cmpl $0,level;
    // jg ok; fail: mov$0; jmp; ok: body` 的唯一 4.4 复现形是
    // `if (a > end || level <= 0) return 0;`（正极性 ||，fail 块 fall-through、
    // ok 块直落，无 goto 的 jmp/nop 中继）。探针 /tmp/p_fat1.cpp getD 19 条与
    // ORIG 逐条同序（jg fail / jg body），残 1 条 = 4.4 常量折叠
    // `(level-1)+8`→`lea 0x7(%eax)`（ORIG `lea -0x1`+`add $0x8` 两步）。
    // [M4S3 round5c 追加试形，全部不闭合 ⇒ 维持 N1]
    //   直证（本件无 docs/class_func_reports 报告，证据取自 ORIG 二进制 nm+objdump）：
    //   ORIG 0x084a84dd `lea -0x1(%eax),%edx` → 0x084a84e0 `mov 0x8(%ebp),%eax`
    //   → 0x084a84e3 `add $0x8,%edx` → 0x084a84e6 `movzwl 0x2(%eax,%edx,2),%eax`；
    //   我方 `lea 0x7(%eax),%edx` + `movzwl 0x2(%eax,%edx,2),%eax`（19 条）。
    //   /tmp/m4s3/p9.cpp b1/b3/b4（`(&m_lt[i-1])[8]` / `(m_lt)[i-1+8]` / `m_lt[i+7]`，
    //   视图偏移 0x18）⇒ 4.4 仍折第一步为 `lea 0x7`，仅把第二级 8 挪进索引并让
    //   disp 吸收 −0x10（`add $0x8` 后接 `movzwl 0x8(...)`），非 ORIG 的
    //   `lea -0x1`+`add $0x8`+`disp 0x2`；p10.cpp c1/c2（真实 m_field2 偏移 +0x02）
    //   ⇒ disp 吸收失败，4.4 直接合成 `lea 0x7` 单步（19 条）；b2/c3 指针算术形
    //   ⇒ add/add/lea 四段；p11.cpp c4/c5（`const int kOff` / enum 常量）⇒ 前端
    //   常量传播后同样折 `lea 0x7`。⇒ 4.1 不折叠 `(i-1)+8`、4.4 折叠属跨版本
    //   lowering，源码层不可复现（N1）。
    if (level - 1 > static_cast<int>(m_endLevel10) || level <= 0)
        return 0;
    // [P6S6 2026-09-11 复核·仍 N] 补试 unsigned 索引形（`level - 1u + 8u`，
    //   历史 19 形未含）实测仍折叠 `lea 0x7(%eax),%edx`（19/20，blocks=2）
    //   ⇒ 与 ORIG 0x084a84dd 的 `lea -0x1(%eax),%edx` + 0x084a84e3 `add $0x8,%edx`
    //   两步形不可复现 ⇒ 维持 N1（4.1 不折叠 (i−1)+8，4.4 折叠属跨版本 lowering）。
    return m_limitTable[level - 1 + 8];
}