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