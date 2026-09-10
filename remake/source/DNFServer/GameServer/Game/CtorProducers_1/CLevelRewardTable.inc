CLevelRewardTable::CLevelRewardTable()
{
    // [b28d-S435 修复·T1 二维化 ⇒ 归 AE] ORIG 0x086975fe：67 vs 67，DIFF 12→0。
    // 头 DataManagerInitDepends.h m_rewardPrimary/Secondary 改 `int[9][0x46]`
    // （Main 代落，同尺寸/偏移/mangled 不变），本处写 `m_rewardPrimary[j][l]`：
    // 4.4.6 逐条复现 ORIG `mov -0x10,%ecx; mov -0xc,%edx; mov 0x8,%eax;
    // imul $0x46,%ecx,%ecx; lea (%ecx,%edx,1),%edx; add $0x44,%edx;
    // movl $0x0,0x8(%eax,%edx,4)`（探针 /tmp/p_lrt.cpp f3 vs 一维 f1/f2/f4/f5/f6/f7）。
    // 旧「N4/N102 封顶」判词作废（一维形恒出 `imul; mov %eax,%edx; add 内存`）。
    // [b28d-solo-CtorProducers_1 修复①] ORIG 0x086975fe：67 vs 67。
    // 本轮修：①成员数组**取址形** `&m_nameStrings[21*i]` / `&m_messages[0x100*i]`
    // 复现 ORIG 的 `add $0x14c0; add this; add $8` 三步地址链（旧 `arr + 21*i`
    // 退化形被 4.4 折 `lea 0x14c8(%eax)`）；②零初始化/声明序不变。
    // ORIG 0x086975fe：i∈[0,0x45] 清 levelKeys[i]，名字槽(+0x14c8+21i)/留言槽
    // (+0x1a86+0x100i) 首字节 memcpy '1'（0x31，来自 rodata "127.0.0.1" 首字符，
    // 非 '\0'）；双层清空 primary(@+0x118)/secondary(@+0xaf0) 两块。
    // [b23S3C] 名字/留言槽 ORIG 出 call memcpy(len 1)（4.1 对 1 字节常量拷贝
    // 不内联；`= '1'` 直存产 movb 缺两条 call 形），改 memcpy("1",1) 直调。
    for (int i = 0; i <= 0x45; ++i)
    {
        m_levelKeys[i] = 0;
        memcpy(&m_nameStrings[21 * i], "1", 1);
        memcpy(&m_messages[0x100 * i], "1", 1);
    }
    for (int j = 0; j < 9; ++j)
        for (int l = 0; l <= 0x45; ++l)
        {
            m_rewardPrimary[j][l] = 0;
            m_rewardSecondary[j][l] = 0;
        }
    m_jobAffection = false;   // ORIG 0x086975fe 尾部：movb $0x0,0x6086(%eax)
