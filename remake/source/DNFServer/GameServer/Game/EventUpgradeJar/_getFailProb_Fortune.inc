unsigned int Event_Upgrade_Jar::_getFailProb_Fortune(int materialIdx, unsigned int upgrade) const
{
    // ORIG：i 为 int（size() 与 i 有符号 setg 比较）。
    for (int i = 0; i < static_cast<int>(m_materialIndices.size()); ++i)
    {
        // [b28d-solo-EventUpgradeJar] P231/P248：ORIG 0x080f4bf0 每轮循环体首条
        // `mov 0xc(%ebp),%ebx` 把形参升格 callee-saved 跨 operator[] 保活 ⇒ 纯
        // register 存储类提示、声明在循环体内（无 asm 绑定，P0 合规）。残余
        // `cmp %eax,%ebx` vs `cmp %ebx,%eax` 操作数序属 N18b。
        register int midx = materialIdx;
        if (midx == (int)m_materialIndices[i])
        {
            // [b28d-solo-EventUpgradeJar] P157/P183：ORIG 0x080f4c13/0x080f4c3e 两次
            // 重算 this+i*0xc+0x40 全链（无引用缓存槽），逐次表达式访问。
            if (upgrade >= m_failRateByFortune[i].size())
                return 100000;
            return m_failRateByFortune[i][upgrade];
        }
    }
    return 100000;
