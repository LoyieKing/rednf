// ORIG 0x8288074：对 base 依次检查 9 个类型参数，为 0 则清对应位（bit0..bit8）。
int ExceptionProtectionType(int value, SECURITY_PROTCTION_TYPE t0,
                            SECURITY_PROTCTION_TYPE t1, SECURITY_PROTCTION_TYPE t2,
                            SECURITY_PROTCTION_TYPE t3, SECURITY_PROTCTION_TYPE t4,
                            SECURITY_PROTCTION_TYPE t5, SECURITY_PROTCTION_TYPE t6,
                            SECURITY_PROTCTION_TYPE t7, SECURITY_PROTCTION_TYPE t8)
{
    if (t0 == 0) value &= ~0x01;
    if (t1 == 0) value &= ~0x02;
    if (t2 == 0) value &= ~0x04;
    if (t3 == 0) value &= ~0x08;
    if (t4 == 0) value &= ~0x10;
    if (t5 == 0) value &= ~0x20;
    if (t6 == 0) value &= ~0x40;
    if (t7 == 0) value &= ~0x80;
    if (t8 == 0) value &= ~0x100;
    return value;
}