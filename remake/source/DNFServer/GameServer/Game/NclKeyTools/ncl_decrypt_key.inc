// ncl_decrypt_key(char* key, char* mac, char* date1, char* date2)
// ORIG（0x085729ce）：
//   key 须 40 位数字（ncl_check_digit_str == 0x28），否则 -1。
//   first = key[0]-'0' 须 ≤4（跳表 0x8ca6a28），否则 -1。
//   first==k 时保留区间 [from, to) 与期望 checksum 位：
//     k=0: [1,9)  位 9；k=1: [0xa,0x12) 位 0x12；k=2: [0x13,0x1b) 位 0x1b；
//     k=3: [0x1c,0x24) 位 0x24；k=4: [0x25,0x27) 位 0x27。
//   区间内数字累加 sum，sum%10 != 期望值 → -1。
//   8 轮解密：mac[i2++]←key[k++] -9、date1[i]←-8、mac[i2++]←-7、date2[i]←-9，
//   i 奇数时 k++（跳过 checksum 位）；循环后 mac←-5、mac←-4；各串补 '\0'。
//   返回 strlen(key)。
// 判例（ADeepY4[1]）：IDENTICAL_AE。槽-局部双向对应律（P0）——原码仅 6 具名局部
//   （macIdx/to/from/first/sum/expected），译出确定性变量时复用 from→k、to→循环计数
//   （Ghidra local_18/local_14 复用），删多余具名 k/i 局部与双初始化；尾串终止位 ORIG
//   用变量 to(=8)/macIdx，非常量下标（Ghidra param_3/param_4[local_14]）；初始化序
//   from→to→macIdx。跳表 rodata 地址经 AE 归一，判 identical_ae。
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
}