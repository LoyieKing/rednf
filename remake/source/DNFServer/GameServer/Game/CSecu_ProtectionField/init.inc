void CSecu_ProtectionField::init()
{
    // b28d-L3: DIFF/N18b — ORIG 0x082882b9 段 `mov -0x28(%ebp),%edx; mov -0x24,%eax; mov -0x14c(%eax,4),%eax; cmp %eax,%edx`（左操作数先物化）
    // ORIG T 0x0828824e（报告 init.md）：表为局部数组（GCC 生成 .rodata 模板 +
    // rep movsl 拷贝），长度走局部变量；第二轮前整表重置回 0x1ff（第一轮结果
    // 被覆盖，ORIG 原样保留）；插入使用 make_pair(enum, unsigned int& ) 实例。
    // [b28c-30] N18b 封顶：内层比较 ORIG 为 `mov i,-0x28,%edx; mov j,%eax;
    // mov tableA[j],%eax; cmp %eax,%edx`（左操作数先物化寄存器）；4.4 恒
    // `mov tableA[j],%eax; cmp i_mem,%eax`（mem 直比）。探针 /tmp/l30_probe3.cpp
    // 5 形（i==t[j]/t[j]==i/unsigned 双 cast/int cast/(t[j]==unsigned i)）全折
    // mem 直比；余差 2 条 mov（150 vs 152）。
    // b28d-L3 实测：i 声明 unsigned 使循环尾测 setle→setbe（残 6→16，劣化），
    // 回退 int；内层比较 mem 直比差仍为 N18b（见上）。
    int i = 0;
    int j = 0;
    int lenA;
    int lenB;

    unsigned int values[0x47];
    for (i = 0; i < 0x47; ++i)
        values[i] = 0x1ff;

    int tableA[37] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
        0x22, 0x24, 0x1f, 0x27, 0x25, 0x26, 0x2c, 0x2d, 0x2f, 0x37,
        0x41, 0x42, 0x43, 0x3a, 0x3b, 0x20, 0x0b };
    lenA = 37;
    for (i = 0; i < 0x47; ++i)
    {
        for (j = 0; j < lenA; ++j)
        {
            // [b28c-30] ORIG 0x082882b9 段：mov -0x28(%ebp),%edx（i 先载寄存器）
            // + cmp %eax,%edx ⇒ 原码左操作数是数组元素（tableA[j] == i）。
            if (tableA[j] == i)
                values[i] = ExceptionProtectionType(values[i], (SECURITY_PROTCTION_TYPE)0,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0x10,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0,
                    (SECURITY_PROTCTION_TYPE)0x80, (SECURITY_PROTCTION_TYPE)0);
        }
    }

    // 第二轮（ORIG 在此把整表重置回 0x1ff，忠实保留）
    for (i = 0; i < 0x47; ++i)
        values[i] = 0x1ff;

    int tableB[29] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x11, 0x14,
        0x16, 0x17, 0x18, 0x1d, 0x22, 0x24, 0x2c, 0x2d, 0x1f, 0x20,
        0x2f, 0x41, 0x42, 0x43, 0x38, 0x39, 0x3a, 0x3b, 0x0b };
    lenB = 29;
    for (i = 0; i < 0x47; ++i)
    {
        for (j = 0; j < lenB; ++j)
        {
            if (tableB[j] == i)   // [b28c-30] 同 tableA：ORIG 寄存器比较方向
                values[i] = ExceptionProtectionType(values[i], (SECURITY_PROTCTION_TYPE)1,
                    (SECURITY_PROTCTION_TYPE)2, (SECURITY_PROTCTION_TYPE)4,
                    (SECURITY_PROTCTION_TYPE)8, (SECURITY_PROTCTION_TYPE)0x10,
                    (SECURITY_PROTCTION_TYPE)0x20, (SECURITY_PROTCTION_TYPE)0x40,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0x100);
        }
    }

    for (i = 0; i < 0x47; ++i)
        m_typeMap.insert(std::make_pair((SECURITY_PROTCTION)i, values[i]));
