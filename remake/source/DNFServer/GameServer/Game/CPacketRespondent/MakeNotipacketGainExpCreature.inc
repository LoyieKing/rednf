void CPacketRespondent::MakeNotipacketGainExpCreature(int exp, int level,
                                                      bool growFlag)
{
    clear();
    put_header(0, 0x66);
    put_byte(level);
    if (growFlag)
    {
        put_byte(1);
        // ORIG 0x83407bd：int 全宽落槽 -0xc 后，movzwl -0xa(%ebp)（同址 u.w[1]）
        // + cwtl + 写回参数槽 0xc ⇒ 推断源为 union 拆半惯用形（P0：shr/独立 16 位槽均形不符）。
        union { int all; unsigned short w[2]; unsigned char b[4]; } u;
        u.all = exp;
        exp = (int)(short)u.w[1];
        put_int(exp);
        put_byte(0xa);
        put_byte((int)u.b[0]);
    }
    else
    {
        put_byte(0);
        put_int(exp);
    }
    finalize(true);
