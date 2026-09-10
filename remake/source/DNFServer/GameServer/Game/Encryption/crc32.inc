unsigned int Encryption::crc32(unsigned int seed, const char* data, unsigned int len)
{
    // [b28d-solo-Encryption] verdict=AE 29=29。
    // ORIG 0x0848d472：仅查 crc_table[255]（&crc_table+0x3fc）；
    // 累加器直接用形参 seed 槽（notl 0x8(%ebp)/mov %eax,0x8(%ebp)，无 crc 局部）；
    // 索引用 (signed char)*data（movzbl+movsbl 两段）。
    if (crc_table[255] == 0)
        make_crc_table();
    seed = ~seed;
    while (len-- != 0)
    {
        // ORIG 先算 seed>>8 到 %edx 再算索引（`xor %edx,%eax`）⇒ 左操作数放 seed>>8。
        seed = (seed >> 8) ^ crc_table[((signed char)*data ^ seed) & 0xff];
        ++data;
    }
    return ~seed;
