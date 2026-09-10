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
