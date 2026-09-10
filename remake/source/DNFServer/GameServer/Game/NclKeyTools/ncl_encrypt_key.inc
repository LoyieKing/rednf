int ncl_encrypt_key(char* out, char* mac, char* date1, char* date2)
{
    if (ncl_check_digit_str(mac) != 0x12)
    {
        return -1;
    }
    if (ncl_check_digit_str(date1) != 0x8)
    {
        return -1;
    }
    if (ncl_check_digit_str(date2) != 0x8)
    {
        return -1;
    }

    int outIdx = 0;
    int sum = 0;
    int macIdx = 0;
    out[outIdx++] = '0';
    for (int i = 0; i <= 7; ++i)
    {
        int v;
        v = ncl_char_plus(mac[macIdx++], 1);
        sum += v;
        out[outIdx++] = static_cast<char>(v + '0');
        v = ncl_char_plus(date1[i * 2], 2);
        sum += v;
        out[outIdx++] = static_cast<char>(v + '0');
        v = ncl_char_plus(date1[i * 2 + 1], 3);
        sum += v;
        out[outIdx++] = static_cast<char>(v + '0');
        v = ncl_char_plus(date2[i], 1);
        sum += v;
        out[outIdx++] = static_cast<char>(v + '0');
        if (i & 1)
        {
            out[outIdx] = static_cast<char>(sum % 10 + '0');
            ++outIdx;
            sum = 0;
        }
    }
    int v = ncl_char_plus(mac[macIdx++], 5);
    sum += v;
    out[outIdx++] = static_cast<char>(v + '0');
    v = ncl_char_plus(mac[macIdx++], 6);
    sum += v;
    out[outIdx++] = static_cast<char>(v + '0');
    out[outIdx] = static_cast<char>(sum % 10 + '0');
    ++outIdx;
    out[outIdx] = '\0';

    std::srand(static_cast<unsigned int>(std::time(0)));
    int pos = std::rand() % 10;
    out[0] = static_cast<char>(pos + '0');
    if (pos != 0)
    {
        out[9] = static_cast<char>(std::rand() % 10 + '0');
    }
    if (pos != 1)
    {
        out[18] = static_cast<char>(std::rand() % 10 + '0');
    }
    if (pos != 2)
    {
        out[27] = static_cast<char>(std::rand() % 10 + '0');
    }
    if (pos != 3)
    {
        out[36] = static_cast<char>(std::rand() % 10 + '0');
    }
    if (pos != 4)
    {
        out[39] = static_cast<char>(std::rand() % 10 + '0');
    }
    return static_cast<int>(std::strlen(out));
