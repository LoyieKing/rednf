// ORIG T 0x08ac34a4：线性 i=0..12，双构串后 operator==（name 串 vs getString 串），
// 命中返回 i；穷尽未命中返回 0。
// b28d-solo-EventRewardQuickPartyB2 判词：DIFF（N93/N96 族封顶，语义等价）。
// 残差三源均跨版本结构性：①4.1 把 string 单参构造展开的 allocator 临时析构推迟
// 到块尾（槽 -0x15/-0xd），4.4 在 string 构造后立即 ~allocator（显式 allocator
// 局部会新增 ORIG 无的具名槽，P0 禁）；②if(current==expected)return i; 的 CFG：
// ORIG 把 operator== 结果留 %ebx、析构两串后 test %bl；4.4 在测试点即分支并以
// %ebx 作 cleanup 路径标志（多 mov $0/$1,%ebx + 两处 test/reload）；
// ③块级对象槽方向相反（N93）：ORIG expected@-0x1c/current@-0x14，4.4 相反
// （current@-0x18/expected@-0x14），帧 0x30 vs 0x20。
// 实测：continue 形 / 函数级 i / 临时表达式三写法 our_insns 恒 109（ORIG 96）；
// p167 15 vs 16 调用点、0 MISSING（仅 +1 _Unwind_Resume，EH 共享差）。
int ContentsType::getType(const char* name)
{
    // [N2-S4 2026-09-11] 4.4 bool 物化消解（109→99，ORIG 96，gap -13→-3）：
    // 把两个 string 的作用域收进内层块、bool 提到块外 ⇒ `if (same)` 落在析构
    // 之后，return 不再跨 cleanup —— 4.4 不再把 operator== 结果物化到
    // callee-saved(%ebx) 并在 landing pad 后重整 0/1（ORIG 0x08ac35a4 仅一条
    // `test %bl,%bl`），也不再需要 `mov $0,%esi; mov %esi,%eax` 的返回值中转。
    // 负结论（同一函数实测 our_insns 不变或变差，故不采用）：声明序交换
    // current/expected(109)、goto 共享出口(109)、while 手写 ++i(109)、
    // 具名 hit+result+break(111)、break+守卫返回 i>12(111)；三元/`==0`/
    // 具名 bool 已由 b28d 判词覆盖（恒 109）。残 -3 为 4.4 的 landing pad
    // 分组（我方 12+12 条 pad + 2 条跳过 jmp vs ORIG 9+14 条）——跨版本结构。
    // [P6S6 2026-09-11] 追加钥匙：`register bool same;`（ORIG 0x08ac35a4
    //   `test %bl,%bl` 直证 same 常驻 callee-saved 字节寄存器）。实测
    //   our_insns 99 不变、gap −3 不变，但 difflib 差异块 14→**12**（消去
    //   `cmpb $0x0,-0x9(%ebp)` 槽物化与 %bl 往返两项）⇒ 按 codegen 轴
    //   「四数不劣 + ndiff 严格改善」保留。
    //   剩余 12 块 = 4.4 landing pad 分组 + string 对象槽深（跨版本结构）。
    for (int i = 0; i <= 12; ++i)
    {
        register bool same;
        {
            std::string expected(getString((ContentsType::T)i));
            std::string current(name);
            same = (current == expected);
        }
        if (same)
            return i;
    }
    return 0;
}