void base64_decode(std::string const& in, std::vector<unsigned char>& out)
{
    int in_len = static_cast<int>(in.size());
    int i = 0;
    int in_ = 0;
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
        int j;
        for (j = i; j <= 3; j++)
            c4[j] = 0;
        for (j = 0; j < 4; j++)
            c4[j] = static_cast<unsigned char>(
                base64_chars.find(static_cast<char>(c4[j]), 0));
        c3[0] = static_cast<unsigned char>((c4[0] << 2) + ((c4[1] & 0x30) >> 4));
        c3[1] = static_cast<unsigned char>((c4[1] << 4) + ((c4[2] & 0x3c) >> 2));
        c3[2] = static_cast<unsigned char>((c4[2] << 6) + c4[3]);
        for (j = 0; j < i - 1; j++)
            out.push_back(c3[j]);
    }
