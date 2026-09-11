void base64_decode(std::string const& in, std::vector<unsigned char>& out)
{
    int in_len = static_cast<int>(in.size());
    int i = 0;
    int in_ = 0;
    // ORIG 0x0807ec17 `movl $0x0,-0xc(%ebp)`（第三枚函数级 int 清零）：原码遗留
    // 死变量（P171 照抄；4.4 对带初始化的未使用局部照样发 store，P232）。
    int j = 0;
    (void)j;
    unsigned char c4[4];
    unsigned char c3[3];
    while (in_len-- != 0)
    {
        // b28d-lane25：P167 差集实证 ORIG 每次迭代读 in[in_] 多次（不缓存 ch）。
        if (in[in_] == '=')
            break;
        if (!is_base64(static_cast<unsigned char>(in[in_])))
            break;
        c4[i++] = static_cast<unsigned char>(in[in_++]);
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                c4[i] = static_cast<unsigned char>(
                    base64_chars.find(static_cast<char>(c4[i]), 0));
            c3[0] = static_cast<unsigned char>((c4[0] << 2) + ((c4[1] & 0x30) >> 4));
            c3[1] = static_cast<unsigned char>((c4[1] << 4) + ((c4[2] & 0x3c) >> 2));
            c3[2] = static_cast<unsigned char>((c4[2] << 6) + c4[3]);
            for (i = 0; i < 3; i++)
                out.push_back(c3[i]);
            i = 0;
        }
    }
    if (i != 0)
    {
        // ORIG 0x0807ed93..0x0807ee6f：三处尾循环的计数均落在 -0x10(in_) 槽
        // （`mov -0x14,%eax; mov %eax,-0x10` = in_=i 起、`addl $1,-0x10` 自增）
        // ⇒ 原码复用函数级 in_ 作循环变量，非块内新声明。
        for (in_ = i; in_ <= 3; in_++)
            c4[in_] = 0;
        for (in_ = 0; in_ < 4; in_++)
            c4[in_] = static_cast<unsigned char>(
                base64_chars.find(static_cast<char>(c4[in_]), 0));
        c3[0] = static_cast<unsigned char>((c4[0] << 2) + ((c4[1] & 0x30) >> 4));
        c3[1] = static_cast<unsigned char>((c4[1] << 4) + ((c4[2] & 0x3c) >> 2));
        c3[2] = static_cast<unsigned char>((c4[2] << 6) + c4[3]);
        for (in_ = 0; in_ < i - 1; in_++)
            out.push_back(c3[in_]);
    }
}