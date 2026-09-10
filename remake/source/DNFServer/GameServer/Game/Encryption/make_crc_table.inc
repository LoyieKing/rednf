void Encryption::make_crc_table()
{
    // [b28d-solo-Encryption] verdict=AE 42=42。
    // ORIG 0x0848d3e4 槽序 crc=-0x4/j=-0x8/bit=-0xc：
    // 4.4 按**声明序深→浅**分配（与 4.1 相反），故声明序须倒排为
    // bit→j→crc，语句序仍保持 crc=1、crc_table[0]=0、bit=0x80（照 ORIG）。
    unsigned int bit;
    unsigned int j;
    unsigned int crc = 1;
    crc_table[0] = 0;
    bit = 0x80;
    for (; bit != 0; bit >>= 1)
    {
        // ORIG：crc>>1 先落 %edx、三元值后落 %eax（`xor %edx,%eax`）且无 v 槽（P0）
        // ⇒ 三元内联且 crc>>1 置左（4.4 先算左操作数）。
        crc = (crc >> 1) ^ ((crc & 1) ? 0x4db89129U : 0);
        for (j = 0; j < 0x100; j += bit * 2)
        {
            crc_table[bit + j] = crc_table[j] ^ crc;
        }
    }
