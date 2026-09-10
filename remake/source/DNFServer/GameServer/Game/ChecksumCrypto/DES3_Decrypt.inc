void DES3_Decrypt(void* rkArea, unsigned char* block)
{
    // [b28d-solo-ChecksumCrypto] DES3_Decrypt IDENTICAL（ndiff 7→0，ORIG=OUR=24 条全等）：
    // 同 DES3_Encrypt——P210 定义后置翻正帧 0x28 + 槽 -0xc，`unsigned char* volatile rk`
    // 翻正末段实参装载序。P167 CLEAR（ORIG 3 = OUR 3）。
    unsigned char* volatile rk = (unsigned char*)rkArea;
    DES_Decrypt(rk + 0x100, block);   // D(K3)
    DES_Encrypt(rk + 0x80, block);    // E(K2)
    DES_Decrypt(rk, block);           // D(K1)
