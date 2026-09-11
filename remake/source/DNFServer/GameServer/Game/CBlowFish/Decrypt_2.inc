int CBlowFish::Decrypt(unsigned char const* in, unsigned char* out, unsigned int len, int mode)
{
    if (len == 0 || (len & 7) != 0)
        return 0x70000006;
    SBlock block(0, 0);
    if (mode == 1)
    {
        SBlock prev(0, 0);
        SBlock link(m_right);
        for (; len > 7; len -= 8)
        {
            BytesToBlock(in, block);
            prev = block;
            Decrypt(block);
            block ^= link;
            link = prev;
            out += 8;
            BlockToBytes(block, out);
            in += 8;
        }
    }
    else if (mode == 2)
    {
        SBlock prev(0, 0);
        SBlock link(m_right);
        for (; len > 7; len -= 8)
        {
            BytesToBlock(in, block);
            Encrypt(link);
            prev = block;
            block ^= link;
            link = prev;
            out += 8;
            BlockToBytes(block, out);
            in += 8;
        }
    }
    else
    {
        for (; len > 7; len -= 8)
        {
            BytesToBlock(in, block);
            Decrypt(block);
            out += 8;
            BlockToBytes(block, out);
            in += 8;
        }
    }
    return 0x6fffffff;
}