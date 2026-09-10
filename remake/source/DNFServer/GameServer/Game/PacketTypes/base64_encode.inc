void base64_encode(const unsigned char* data, unsigned int len,
                   std::string& out)
{
    int i = 0;
    unsigned char b3[3];
    while (len != 0)
    {
        --len;
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
            for (int k = 0; k < 4; ++k)
                out += base64_chars[b4[k]];
            i = 0;
        }
    }
    if (i > 0)
    {
        for (int k = i; k < 3; ++k)
            b3[k] = 0;
        unsigned char b4[4];
        b4[0] = static_cast<unsigned char>(b3[0] >> 2);
        b4[1] = static_cast<unsigned char>((b3[0] & 3) * 16 + (b3[1] >> 4));
        b4[2] = static_cast<unsigned char>((b3[1] & 0xf) * 4 + (b3[2] >> 6));
        b4[3] = static_cast<unsigned char>(b3[2] & 0x3f);
        for (int k = 0; k < i + 1; ++k)
            out += base64_chars[b4[k]];
        for (int k = i + 1; k < 4; ++k)
            out += '=';
    }
