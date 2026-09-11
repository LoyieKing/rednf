// ============================================================================
// advancealtar::ConfigParameter::importConfigParameter（ORIG 0x0889b6cc T）
// 逐段还原 ORIG ## 汇编 + ## 反编译 C：
//  · [gage up rate] 行 (idx, P, Q)：0<=idx<=3 时 this+8(idx*4)=Q(m_gageUpRate)、
//    this+0x1c(idx*4)=P(m_gageUpAux)（ORIG 0x0889b742/0x0889b757：+8 存第 3 个
//    ScanInt=local_18，+0x1c 存第 2 个=local_1c，与直觉相反，原实现写反已修）；
//    循环带一个 ORIG dead 计数 local_24（0..每行 ++，从不读）。P 失败→0x3e9、
//    Q 失败→0x3ec。
//  · [gage upgrade cost percent]：(float)(long double)ScanFloat 存 m_4(+4)，失败 0x3f8；
//    [daily refill ticket]：ScanInt 存 m_0(+0)，失败 0x3ff。
//  · 三个开标签均不匹配时（ORIG 最内 else 0x0889b84f..）短路比对
//    [/gage up rate]、[/gage upgrade cost percent]、[/daily refill ticket]（结果丢弃）。
//    原实现缺这三条比较 → we-smaller 主因。成功/文件打开失败均返 0（非零=Fail）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（17/17）。本轮加 P232
// `char framePad[4]`（首声明）⇒ 帧 sub $0x40=ORIG、token -0x28 / ok -0x29 /
// lineNo -0x20 / idx -0x1c / p -0x18 / q -0x14 / fv -0x10 / tv -0xc 全部与 ORIG 对齐；
// 残余 = 尾部闭标签 6 条（ORIG `test %al,%al; jne <循环头岛>` 正极性 vs 4.4
// `test; je 下一块; jmp 岛` 反极性——continue/嵌套包裹/goto 尾标签三形实测均不翻，
// P222/P223 边界）；ORIG 0x0889b6cc。
int advancealtar::ConfigParameter::importConfigParameter(const char* path)
{
    std::string token;
    int unkC = 0;          // ORIG 0x0889b6df `movl $0x0,-0x24(%ebp)`：token 后 dead int
    (void)unkC;
    bool ok = false;
    // [X2 形·技巧22精化（DispC8 probe 实证，Main 批准）] ORIG 对 bool callee 返回一律
    // `xor $0x1,%eax; test %al,%al; j<cc>`（0x0889b6fd/0x0889b725/0x0889b764/…），4.4
    // 直写 !call()/!ok 折成 test/sete 两段。[b28c-18-unbind] 绑定已全部清除
    // （N96+N18a+P168a④，判词见 DeleteInvalidItemScript::loadScript 头）。
    if (!(loadRDARScriptFile("", path)))
        return 0;

    // [本波] P223 多岛形（不同落点物理连续标签）：ORIG 0x0889b70e/0x0889b711
    // 为两个相邻岛（`nop; jmp 0x0889b712` 与 `nop`），分别被 0x0889b887/0x0889b8a2
    // 的 `jne` 后跳命中（目标 +0x42/+0x45 不同）⇒ 源码写两条独立 `goto` 到
    // 循环头前的连续空标签（标签序 = 守卫序），替代 `continue`（4.4 恒 je+jmp）。
    LoopIsland1:;
    LoopIsland2:;
    for (;;)
    {
        if (!(ScanType(token, true)))
            break;                               // ORIG 0x0889b72a jne 出口 → 返 0
        if (token == "[gage up rate]")
        {
            int lineNo = 0;                      // ORIG local_24：dead 行计数
            for (;;)                              // ORIG 头：idx=ScanInt 于循环顶
            {
                int idx = ScanInt(&ok);
                if (!(ok))
                    break;                        // ORIG：行首失败仅结束本段(0x…76c)
                int p = ScanInt(&ok);            // ORIG local_1c（第 2 个）
                if (!(ok))
                    return 0x3e9;
                int q = ScanInt(&ok);            // ORIG local_18（第 3 个）
                if (!(ok))
                    return 0x3ec;
                if (idx >= 0 && idx <= 3)        // ORIG cmpl $0/js + cmpl $3/jg
                {
                    m_gageUpRate[idx] = q;       // this+8   = 第 3 值
                    m_gageUpAux[idx] = p;        // this+0x1c = 第 2 值
                }
                ++lineNo;                        // ORIG local_24++
            }
        }
        else if (token == "[gage upgrade cost percent]")
        {
            float fv = ScanFloat(&ok);           // ORIG：fstps 暂存，校验后才写 this+4
            if (!(ok))
                return 0x3f8;
            m_4 = fv;
        }
        else if (token == "[daily refill ticket]")
        {
            int tv = ScanInt(&ok);               // ORIG：暂存 -0xc，校验后才写 this+0
            if (!(ok))
                return 0x3ff;
            m_0 = tv;
        }
        else
        {
            // ORIG 0x0889b885/0x0889b8a2：`test %al,%al; jne <循环头岛>` 正极性
            // ⇒ 用 goto 到循环头前的连续空标签（P223 多岛：不同落点可造）。
            if (token == "[/gage up rate]")
                goto LoopIsland1;
            if (token == "[/gage upgrade cost percent]")
                goto LoopIsland2;
            (void)(token == "[/daily refill ticket]");
        }
    }
    return 0;
}