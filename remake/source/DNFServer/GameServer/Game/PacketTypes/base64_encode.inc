void base64_encode(const unsigned char* data, unsigned int len,
                   std::string& out)
{
    int i = 0;
    int j = 0;                       // ORIG 0x0807ea3b `movl $0,-0xc`（函数级 j=0）
    unsigned char b3[3];
    // ORIG 0x0807eafd..0x0807eb0a：`cmpl $0,0xc(%ebp); setne %al; subl $0x1,0xc(%ebp);
    // test %al,%al; jne <body>` ⇒ 原码是 `while (len-- != 0)`（形参自减，退出后为 -1）。
    while (len-- != 0)
    {
        b3[i] = *data;
        ++i;
        ++data;
        if (i == 3)
        {
            unsigned char b4[4];
            b4[0] = static_cast<unsigned char>(b3[0] >> 2);
            b4[1] = static_cast<unsigned char>((b3[0] & 3) * 16 + (b3[1] >> 4));
            b4[2] = static_cast<unsigned char>((b3[1] & 0xf) * 4 + (b3[2] >> 6));
            b4[3] = static_cast<unsigned char>(b3[2] & 0x3f);
            // ORIG 0x0807eaae：清零/自增均作用于 -0x10(i) ⇒ 循环变量复用 i。
            for (i = 0; i < 4; ++i)
                out += base64_chars[b4[i]];
            i = 0;
        }
    }
    if (i > 0)
    {
        // ORIG 0x0807eb1a..0x0807eb37：`mov i,%eax; mov %eax,-0xc(j)` + j<=2 计数
        // ⇒ 原码用函数级 j 作循环变量（非块内新变量）。
        for (j = i; j < 3; ++j)
            b3[j] = 0;
        unsigned char b4[4];
        b4[0] = static_cast<unsigned char>(b3[0] >> 2);
        b4[1] = static_cast<unsigned char>((b3[0] & 3) * 16 + (b3[1] >> 4));
        b4[2] = static_cast<unsigned char>((b3[1] & 0xf) * 4 + (b3[2] >> 6));
        b4[3] = static_cast<unsigned char>(b3[2] & 0x3f);
        // ORIG 0x0807ebbe：`mov i,%eax; add $0x1,%eax; cmp j,%eax; setg` ⇒ j < i+1
        for (j = 0; j < i + 1; ++j)
            out += base64_chars[b4[j]];
        // ORIG 0x0807ebe3：`cmpl $0x2,-0x10(i); setle %al; addl $0x1,-0x10;
        // test %al,%al; jne <body>`（比较先于自增）⇒ 原码 `while (i++ <= 2)`；
        // 自增后 i 出函数即被修改为 4（照抄原码行为，P171）。
        while (i++ <= 2)
            out += '=';
    }
}