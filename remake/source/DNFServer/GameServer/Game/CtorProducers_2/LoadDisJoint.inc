bool DisJoint::LoadDisJoint()
{
    // [SweepCtorProd2] gap −1→+1（78/77 条），jcc 多重集全等（call 6/je 1/jmp 3/
    // jne 2/setg 1/setle 1，逐条核对落点体一致），p167 CLEAR（6=6）。
    // 残余 1 条 = 循环尾基址/索引装载寄存器角色互换（ORIG `mov i,%edx; mov this,%eax;
    // add $0x5c,%edx; shl $4,%edx; add %edx,%eax` vs 4.4 `mov i,%eax; … add this,%eax`）
    // ⇒ N7 族（间接/地址装载寄存器角色，非源形可控），语义等价。
    // 已修：删 `const int count` 缓存（ORIG 内层条件每轮重算 m_additional[i].m_count，
    // 缓存版多发一段循环外计算并多占一槽使全帧深 4）。
    // ORIG 0x0847300c：import 成功则把 [cube index] 第 5 组首值与其
    // [additional result] 各组元素聚合到 +0x628 起的 6 个 vector。
    if (!importDisjointScript(this, "Etc/Disjoint.etc"))
    {
        std::puts("DisJoint Script Error: Etc/Disjoint.etc");
        return false;
    }

    for (int i = 0; i < 6; ++i)
    {
        int value = m_cubeIndexVecs[4].at(0);        // at(this+0x30, 0)；ORIG 显式 _ZNSt6vectorIiSaIiEE2atEj
        m_resultVecs[i].push_back(value);
        // [SweepCtorProd2] ORIG 0x084730xx 内层条件每轮重算（`mov i,%edx; add $0x5c;
        // shl $4; add this; add $8; mov (%eax),%eax; cmp j,%eax; setg`），无 count
        // 缓存局部；原 `const int count` 缓存多发一段循环外计算并多占一个槽
        // （全帧槽深 +4）。表达式内联进 for 条件即复现。
        // [M5F_CtorDB 2026-09-11] 唯一残差（ORIG 78 / 我方 77）= 循环尾基址装载：
        // ORIG `mov -0x10,%edx; mov 0x8(%ebp),%eax; add $0x5c,%edx; shl $4,%edx;
        // add %edx,%eax`（**索引先落 %edx、this 后落 %eax 再 add**），我方 4.4 折成
        // `mov i,%eax; …; add 0x8(%ebp),%eax`（内存操作数直加，少 1 条 mov）。
        // 本轮换「成员访问形」试造 ORIG 的装载序：m_additional[i].m_count 走
        // 索引强度削减 `(i+0x5c)<<4 + 8`，与 ORIG 的 add $0x5c/shl $4 逐条同源；
        // 语义等价、布局/mangled 零变化。若读数仍 77 ⇒ 归 N7（寄存器角色，4.1 独有）。
        for (int j = 0; j < m_additional[i].m_count; ++j)
        {
            m_resultVecs[i].push_back(m_additional[i].m_items[j]);
        }
    }
    return true;
}