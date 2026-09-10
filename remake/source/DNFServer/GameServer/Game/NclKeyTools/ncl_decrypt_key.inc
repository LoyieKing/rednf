int ncl_decrypt_key(char* key, char* mac, char* date1, char* date2)
{
    if (ncl_check_digit_str(key) != 0x28)
    {
        return -1;
    }
    int expected;
    int sum;
    int first = key[0] - '0';
    int from;
    int to;
    int macIdx;
    switch (first)
    {
    case 0:
        from = 1;
        to = 9;
        expected = key[9] - '0';
        break;
    case 1:
        from = 0xa;
        to = 0x12;
        expected = key[0x12] - '0';
        break;
    case 2:
        from = 0x13;
        to = 0x1b;
        expected = key[0x1b] - '0';
        break;
    case 3:
        from = 0x1c;
        to = 0x24;
        expected = key[0x24] - '0';
        break;
    case 4:
        from = 0x25;
        to = 0x27;
        expected = key[0x27] - '0';
        break;
    default:
        return -1;
    }
    sum = 0;
    for (; from < to; ++from)
    {
        sum += key[from] - '0';
    }
    if (sum % 10 != expected)
    {
        return -1;
    }

    from = 1;
    to = 0;
    macIdx = 0;
    for (; to < 8; ++to)
    {
        mac[macIdx++] = static_cast<char>(
            ncl_char_plus(key[from++], 9) + '0');
        date1[to] = static_cast<char>(
            ncl_char_plus(key[from++], 8) + '0');
        mac[macIdx++] = static_cast<char>(
            ncl_char_plus(key[from++], 7) + '0');
        date2[to] = static_cast<char>(
            ncl_char_plus(key[from++], 9) + '0');
        if (to & 1)
        {
            ++from;
        }
    }
    mac[macIdx++] = static_cast<char>(
        ncl_char_plus(key[from++], 5) + '0');
    mac[macIdx++] = static_cast<char>(
        ncl_char_plus(key[from++], 4) + '0');
    mac[macIdx] = '\0';
    date1[to] = '\0';
    date2[to] = '\0';
    return static_cast<int>(std::strlen(key));
