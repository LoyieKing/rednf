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