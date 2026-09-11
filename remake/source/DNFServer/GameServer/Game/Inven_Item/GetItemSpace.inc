int Inven_Item::GetItemSpace() const
{
    int result;
    register unsigned char t = (unsigned char)m_field1;
    if (t < 0xb)
    {
        // [b28d-solo-Inven_Item] 同 GetInventoryType：N 封顶（跨版本冗余 zext
        // `movzbl %al,%eax` 被 4.4 折叠 + and 临时寄存器选择 4.1 %edx / 4.4 %eax）；
        // P231 register 免 u 槽，控制流与返回值语义一致。
        // [N2-151to170] 同 GetInventoryType：register uchar t 单次读取 + 三段独立 if
        // 各自 goto 共享尾出口 ⇒ 41→38 条 = ORIG 38（gap 0）；残余同上（跨版本
        // t/u 寄存器分配、and 临时折叠、叶块内联 vs 尾部集中）⇒ DIFF 封顶。
        register unsigned int u = 1u << t;
        if (u & 0x61f)
        {
            result = 0;
            goto done;
        }
        if (u & 0x100)
        {
            result = 1;
            goto done;
        }
        if (u & 0xe0)
        {
            result = 7;
            goto done;
        }
    }
    result = 0;
done:
    return result;
}