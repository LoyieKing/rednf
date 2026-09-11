// === RSA_size @0x087eb2e0 ===================================================
// (BN_num_bits(rsa->n@+0x10)+7)/8（ORIG lea/sar 有符号除法展开一致）
int RSA_size(const void* rsa)
{
    const RSA_VIEW* r = static_cast<const RSA_VIEW*>(rsa);
    return (BN_num_bits(r->n) + 7) / 8;
}