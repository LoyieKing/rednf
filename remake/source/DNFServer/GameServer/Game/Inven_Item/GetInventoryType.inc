int Inven_Item::GetInventoryType()
{
    int result;
    register unsigned char t = (unsigned char)m_field1;
    if (t < 0xb)
    {
        // [b28d-solo-Inven_Item] N 封顶（语义等价，仅跨版本 codegen 差）：
        // ①开头 ORIG `movzbl 0x1(%eax),%eax; movzbl %al,%eax; cmp $0xa,%eax`
        //   （4.1 保留 (unsigned char) 转换的二次零扩展）；4.4 恒折 8 位比较
        //   `cmp $0xa,%al`（探针 f_ucast/f_uicast/f_plain 三形同）。
        // ②三次掩码测试 ORIG `mov %ebx,%eax; mov %eax,%edx; and $imm,%edx`
        //   （4.1 把 u 留在 %eax、用 %edx 作 and 临时），4.4 恒
        //   `mov %ebx,%eax; and $imm,%eax`（每次自 %ebx 重载，N18a/N7 族）。
        // 已试 12 形（register 体内/体外、int/unsigned、嵌套 if、!=0、三元、
        // 双 register 变量、具名槽）+ 4.4.6 后端复测，全部同形 ⇒ 不可造。
        // P231 纯 register 仍必要（免 u 栈槽，否则帧/槽全错位）。
        // [N2-151to170] register uchar t 单次读取（比较与 shift 共用）+ 三段独立 if
        // 各自 goto 共享尾出口（default 块 fall-through）⇒ 41→38 条 = ORIG 38（gap 0）。
        // 残余为跨版本寄存器/块序：①t 装载 4.4 落 %ebx / ORIG 落 %eax；②掩码 and 的
        // 临时 4.1 走 %edx 双指令、4.4 自 %ebx 重载折叠单指令（P248 族）；③叶块 4.1
        // 尾部集中（jne 跳叶块）/ 4.4 内联（je 跳过）⇒ 指令数已齐平，DIFF 封顶。
        register unsigned int u = 1u << t;
        if (u & 0x61f)
        {
            result = 1;
            goto done;
        }
        if (u & 0x100)
        {
            result = 2;
            goto done;
        }
        if (u & 0xe0)
        {
            result = 3;
            goto done;
        }
    }
    result = 1;
done:
    return result;
}