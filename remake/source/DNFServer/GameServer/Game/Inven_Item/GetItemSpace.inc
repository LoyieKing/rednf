int Inven_Item::GetItemSpace() const
{
    int result;
    if ((unsigned char)m_field1 < 0xb)
    {
        // [b28d-solo-Inven_Item] 同 GetInventoryType：N 封顶（跨版本冗余 zext
        // `movzbl %al,%eax` 被 4.4 折叠 + and 临时寄存器选择 4.1 %edx / 4.4 %eax）；
        // P231 register 免 u 槽，控制流与返回值语义一致。
        register unsigned int u = 1u << m_field1;
        if (u & 0x61f)
        {
            result = 0;
        }
        else if (u & 0x100)
        {
            result = 1;
        }
        else if (u & 0xe0)
        {
            result = 7;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        result = 0;
    }
    return result;
