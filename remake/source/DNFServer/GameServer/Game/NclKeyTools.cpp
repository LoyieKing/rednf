// ============================================================================
// df_game_r ncl_* license key 工具族（ORIG 自由函数，objdump 直转）。
// ----------------------------------------------------------------------------
// ORIG 符号（df_game_r，全部 T 强符号，同一工具区域 0x085723fc-0x08572e1c）：
//   ncl_make_mac(char*)                _Z12ncl_make_macPc    0x085723fc (0xd3)
//   ncl_char_plus(char, int)           _Z13ncl_char_plusci   0x08572581 (0x3c)
//   ncl_check_digit_str(char*)         _Z19ncl_check_digit_strPc 0x08572531 (0x50)
//   ncl_make_date(char*)               _Z13ncl_make_datePc   0x085724cf (0x62)
//   ncl_encrypt_key(char*,char*,char*,char*) _Z15ncl_encrypt_keyPcS_S_S_ 0x085725bd
//   ncl_decrypt_key(char*,char*,char*,char*) _Z15ncl_decrypt_keyPcS_S_S_ 0x085729ce
//   ncl_check_key(char*)               _Z13ncl_check_keyPc   0x08572cb5 (0x167)
//   ncl_check_date(char*, char*)       _Z14ncl_check_datePcS_ 0x08572e1c (0x7a)
// 加密/哈希类（用户规则 2026-08-16）只要求算法正确，不要求 identical。
// 原 ncl_check_key「非空即通过」骨架位于 GameMain.cpp，已删除并由本 TU 提供
// 真实实现（ORIG：decrypt → 长度 0x28 → MAC 等于本机 eth0 → 当日日期落入
// [date1, date2] 闭区间）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

// ncl_char_plus(char, int) —— 数字字符加增量后按 0-9 回绕。
// ORIG：movsbl 符号扩展 → sub $0x30 → add n → i586 除法魔法 %10。
int ncl_char_plus(char c, int n)
{
    return (c - '0' + n) % 10;
}

// ncl_check_digit_str(char*) —— 全数字返回 strlen，含非数字返回 -1。
// ORIG：len=strlen；逐字符 sub $0x30; cmp $0x9; jbe（无符号 ≤9 合法）→
// 非法时 len=-1 并跳出；jbe 对应 (unsigned)(c-'0') <= 9。
int ncl_check_digit_str(char* s)
{
    // ORIG 槽向：len@-0xc（浅）、i@-0x10（深）；4.4 先声明占深槽，
    // 故 i 声明前置（未初始化无指令序影响，P210 附/N93 平铺版复现）。
    int i;
    int len = static_cast<int>(std::strlen(s));
    for (i = 0; i < len; ++i)
    {
        if ((unsigned int)(s[i] - '0') > 9)
        {
            len = -1;
            break;
        }
    }
    return len;
}

// ncl_make_mac(char*) —— 本机 eth0 MAC，每字节 "%03d"（3 位十进制）拼接。
// ORIG：socket(AF_INET, SOCK_DGRAM, 0) → struct ifreq(栈 0x30) memset →
//   memcpy 设备名 "eth0"（5 字节含 NUL）→ ioctl(SIOCGIFHWADDR=0x8927) →
//   close → 6 字节各 sprintf("%03d") 到 out，每 3 字符后补 '\0'（rodata
//   0x8ca6a10="eth0" / 0x8ca6a15="%03d"）。
void ncl_make_mac(char* out)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::memcpy(ifr.ifr_name, "eth0", 5);
    ioctl(sock, SIOCGIFHWADDR, &ifr);
    close(sock);
    int i;
    int off = 0;
    for (i = 0; i <= 5; ++i)
    {
        std::sprintf(out + off, "%03d",
                     static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[i]));
        off += 3;
        out[off] = '\0';
    }
}

// ncl_make_date(char*) —— 当日 "YYYYMMDD"（8 位数字）。
// ORIG：time → localtime_r → sprintf("%04d%02d%02d", year+1900, mon+1, mday)
// （rodata 0x8ca6a1a="%04d%02d%02d"；tm 字段经返回指针访问，P43）。
void ncl_make_date(char* out)
{
    // ORIG：time(&t)（传指针，P43）；tm 字段直读结构槽（非经 p 指针）。
    time_t t;
    std::time(&t);
    struct tm tmBuf;
    struct tm* p = localtime_r(&t, &tmBuf);
    // ORIG 0x85724f9：`je <nop 岛>` + sprintf + 尾 `jmp` + nop（P223 空块占位岛）。
    if (p == 0)
        goto L_done;
    std::sprintf(out, "%04d%02d%02d",
                 tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday);
L_done:;
}

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

// ncl_check_key(char*) —— license key 校验（真实实现替换原「非空即通过」骨架）。
// ORIG（0x08572cb5）：三个 0x80 缓冲 memset → ncl_decrypt_key(key, mac, date1,
//   date2) == 0x28 才继续 → ncl_make_mac 与本机 MAC 相等 → ncl_make_date 的
//   当日字符串落入 [date1, date2] 闭区间（strcmp <0 / >0 判非法）。
int ncl_check_key(char* key)
{
    int ok = 0;
    char today[0x80];   // 提升到函数首部：ORIG 使 today 落栈顶 -0x8c（if 块内声明则落最深 -0x20c）
    char mac[0x80];
    char date1[0x80];
    char date2[0x80];
    std::memset(mac, 0, sizeof(mac));
    std::memset(date1, 0, sizeof(date1));
    std::memset(date2, 0, sizeof(date2));
    if (ncl_decrypt_key(key, mac, date1, date2) == 0x28)
    {
        ok = 1;
        std::memset(today, 0, sizeof(today));
        ncl_make_mac(today);
        if (std::strcmp(mac, today) != 0)
        {
            ok = 0;
        }
        std::memset(today, 0, sizeof(today));
        ncl_make_date(today);
        if (std::strcmp(today, date1) < 0 || std::strcmp(today, date2) > 0)
        {
            ok = 0;
        }
    }
    return ok;
}

// ncl_check_date(char*, char*) —— 当日字符串在 [a, b] 闭区间内返回 1，否则 0。
// ORIG（0x08572e1c）：memset(0x80) → ncl_make_date → strcmp(today,a) < 0 ||
//   strcmp(today,b) > 0 → 0。
int ncl_check_date(char* a, char* b)
{
    int ok = 1;
    char today[0x80];
    std::memset(today, 0, sizeof(today));
    ncl_make_date(today);
    if (std::strcmp(today, a) < 0 || std::strcmp(today, b) > 0)
    {
        ok = 0;
    }
    return ok;
}
