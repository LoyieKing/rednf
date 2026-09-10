int CBlowFish::Decrypt(unsigned char* buf, unsigned int len, int mode)
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
            BytesToBlock(buf, block);
            prev = block;
            Decrypt(block);
            block ^= link;
            link = prev;
            buf += 8;
            BlockToBytes(block, buf);
        }
    }
    else if (mode == 2)
    {
        SBlock prev(0, 0);
        SBlock link(m_right);
        for (; len > 7; len -= 8)
        {
            BytesToBlock(buf, block);
            Encrypt(link);
            prev = block;
            block ^= link;
            link = prev;
            buf += 8;
            BlockToBytes(block, buf);
        }
    }
    else
    {
        for (; len > 7; len -= 8)
        {
            BytesToBlock(buf, block);
            Decrypt(block);
            buf += 8;
            BlockToBytes(block, buf);
        }
    }
    return 0x6fffffff;
