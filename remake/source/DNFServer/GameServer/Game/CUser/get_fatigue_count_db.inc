// ORIG 0x0818d4c4：若当前疲劳 DB 类型(+0x8eb4d) == type 返回 (+0x8eb4c) 否则 0。
int CUser::get_fatigue_count_db(int type)
{
    // ORIG 0x868d4ca-0x868d4d4：双零扩展 + `cmp 0xc(%ebp),%eax` arg 槽直折、**无 cur 槽**
    // （sub 无、pop ebp 直返）。b28d-solo-CUser AE：内联条件（具名 cur 落 -0x4 致帧 0x10）。
    if ((unsigned char)m_serverFatigueDbType == type)
        return (unsigned char)m_serverFatigueValue;
    return 0;
}