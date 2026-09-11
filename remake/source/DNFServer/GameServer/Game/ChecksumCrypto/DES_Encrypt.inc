// [b28d-41] 豁免（用户规则 §6：DES 通用算法类只保算法正确）：ORIG 4.1 将 16 轮
// 完全展开（1058 条），重建用 DES_BLOCK_BODY 宏内 16 轮循环（81 条）——语义等价。
// 实测（/tmp/d41_crypto_test.cpp 运行 tu_ChecksumCrypto.o）：NIST 单块向量
// key 133457799BBCDFF1 / pt 0123456789ABCDEF → ct 85E813540F0AB405 ✓、
// DES enc/dec roundtrip ✓、3DES EDE roundtrip ✓。
// [b28d-solo-ChecksumCrypto] DES_Encrypt N 封顶（算法族豁免，guide §6）：ORIG 为
// OpenSSL 风格 des_SPtrans 表驱动 16 轮完全展开（1058 条，报告 0x080ac0ba）；重建为
// FIPS 46-3 置换表 + desF helper 的 16 轮循环（80 条）。实现策略不同 ⇒ 源形不可复现。
void DES_Encrypt(void* rkArea, unsigned char* block)
{
    DES_BLOCK_BODY(1);
}