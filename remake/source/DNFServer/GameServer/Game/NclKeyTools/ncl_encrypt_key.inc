// ncl_encrypt_key(char* out, char* mac, char* date1, char* date2)
// ORIG（0x085725bd）：
//   校验 mac=12 位 / date1=8 位 / date2=8 位数字，否则 -1。
//   out[0]='0'；8 轮 × 4 位：mac[j++]→+1、date1[2i]→+2、date1[2i+1]→+3、
//   date2[i]→+1，每轮累加 sum 并输出 v+'0'；i 为奇数时输出 sum%10+'0' 并清零。
//   循环后 mac[8]→+5、mac[9]→+6、尾 checksum=sum%10+'0'、out[40]='\0'。
//   随机化：srand(time)；pos=rand()%10；out[0]=pos+'0'；5 个 checksum 位
//   （偏移 9/0x12/0x1b/0x24/0x27 = 9/18/27/36/39）中 pos==k 者保留原值，
//   其余 4 位用 rand()%10+'0' 覆盖（ORIG 原样，含越界位 36/39 写入）。
//   返回 strlen(out)。
// [b28d-D43] N82/N89 大函数（344/344）：槽序与寄存器分配跨版本差（ORIG
// 0x085725bd；控制流/调用点逐条同构，语义闭合）。
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
}