unsigned char CBitManager::GetBit(int idx, int bitLo, int bitHi) // 0x0831c2fc
{
    // ORIG 0x0831c2fc：int 循环槽先置 0（-0x8）、acc/ret 置 0（-0x2/-0x1），
    // 栈掩码表逐元素赋值，(int)b<=bitHi setle 形循环。
    int b = 0;
    unsigned char acc = 0;
    unsigned char ret = 0;
    unsigned char masks[8];
    masks[0] = 0x01;
    masks[1] = 0x02;
    masks[2] = 0x04;
    masks[3] = 0x08;
    masks[4] = 0x10;
    masks[5] = 0x20;
    masks[6] = 0x40;
    masks[7] = 0x80;
    for (b = bitLo; b <= bitHi; ++b)
    {
        acc = static_cast<unsigned char>(acc | masks[b]);
    }
    ret = static_cast<unsigned char>(m_data[idx] & acc);
    return ret;
