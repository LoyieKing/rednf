int reseal_table_t::get_oneshot_reseal_cost(unsigned char param_1,
                                            unsigned char param_2,
                                            unsigned char param_3)
{
    // 汇编仅校验 param_1<=5 与 param_2 范围，忽略 param_3（无 -0xc(%ebp) 读取）
    (void)param_3;
    if (param_2 == 0)
        return -1;
    if (param_1 > 5)
        return -2;
    if (param_2 > 99)
        return -3;
    return 1;
