int Inven_Item::GetInventoryType()
{
    int result;
    if ((unsigned char)m_field1 < 0xb)
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
        register unsigned int u = 1u << m_field1;
        if (u & 0x61f)
        {
            result = 1;
        }
        else if (u & 0x100)
        {
            result = 2;
        }
        else if (u & 0xe0)
        {
            result = 3;
        }
        else
        {
            result = 1;
        }
    }
    else
    {
        result = 1;
    }
    return result;
