// [b28d-solo-ChecksumCrypto] DES_Decrypt N 封顶（算法族豁免，guide §6）：ORIG 与
// DES_Encrypt 同族 des_SPtrans 16 轮完全展开（1058 条，报告 0x080acf59）；重建为
// FIPS 表 + 轮循环（80 条）。NIST 单块向量 ✓（/tmp/d41_crypto_test.cpp）。
void DES_Decrypt(void* rkArea, unsigned char* block)
{
    DES_BLOCK_BODY(0);
}