// [b28d-solo-CDataManagerScripts] AE：P310 —— `for (int i = 0; (unsigned int)i <= 199; ++i)` 复现 ORIG 两段寄存器尾测（`mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe`）；fresh clean 42=42；ORIG 0x08370b02。
void stBloodDungeon_t::clear()
{
    // [b28c-27] N：ORIG 循环条件 `mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe`
    // （局部先载寄存器）；4.4 O0 恒 `cmpl $0xc7,-0xc(%ebp)` mem 直比。unsigned/
    // while 两形实测同折，属局部比较 codegen 跨版本差（N-86b 族），语义全等。
    // ORIG 0x8370b02：循环(unsigned≤199)置 1.0f → +0x334 clear → +0x320=1 →
    // +0x330=1.0f → memset(+0x324,0,0xc) → memset(+0x340,0,0x18) → +0x358 clear。
    //   【N】循环条件 ORIG `mov -0xc,%eax; cmp $0xc7,%eax`（先载入寄存器），
    //   4.4 O0 恒折 `cmpl $0xc7,-0xc(%ebp)`（mem 直比），语义全等，register 形差。
    // [b28d-solo-CDataManagerScripts] P310：局部改 int + 显式 (unsigned int) 比较
    // 复现 ORIG 两段寄存器尾测（mov -0xc,%eax; cmp $0xc7,%eax; setbe）。
    for (int i = 0; (unsigned int)i <= 199; ++i)
        m_rate[i] = 1.0f;
    m_vec334.clear();
    m_field320 = 1;
    m_field330 = 1.0f;
    memset(m_field324, 0, sizeof(m_field324));
    memset(m_pad340, 0, 0x18);  // 连续覆盖 +0x340..+0x357（含 344/348 两 int）
    m_map358.clear();
}