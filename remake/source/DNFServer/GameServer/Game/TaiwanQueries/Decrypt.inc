// ORIG T 0x0816c532：rsaSize=RSA_size(key)；pos=0 起按块
// min(len-pos, rsaSize) RSA_public_decrypt(...,RSA_PKCS1_PADDING=1)，块失败
// 即整体失败；成功块 insert 到 out 尾部，直至 pos>=len。
bool COpensslRSA::Decrypt(const char* in, int len, std::vector<unsigned char>& out)
{
    int rsa_size = RSA_size(m_rsa);
    int pos = 0;
    // [b28d-solo-D02] ORIG 0x0816c552..0x0816c567 为 MIN_EXPR 展开形（三目
    // `(len - pos > rsa_size) ? rsa_size : (len - pos)` + i486 后端 ⇒ 寄存器比较 +
    // jle + 单条 store）。Main 已按 D02 A/B 落表本 TU `-march=i486 -mtune=generic`
    // 路由（缺该旗标时 4.4 对同一三目发 cmovle，探针实测多 10 条）；
    // A/B 证据见 /tmp/ab_TaiwanQueries.compile.log。**verdict=AE（137≈归一）**。
    int chunk = (len - pos > rsa_size) ? rsa_size : (len - pos);
    while (chunk > 0)
    {
        std::vector<unsigned char> block;
        block.resize(static_cast<size_t>(rsa_size), 0);
        int written = RSA_public_decrypt(chunk,
                                         reinterpret_cast<const unsigned char*>(in) + pos,
                                         &block[0], m_rsa, 1);
        if (written <= 0)
        {
            return false;
        }
        // [L10-2] P167 实证：ORIG 0x0816c5ff..0x0816c644 为
        // `operator[](0)`×2（first/last 各一次）+ `insert<unsigned char*>`（指针版）
        // ⇒ 源形 `&block[0], &block[0] + written`；`.begin()+n` 会发 `begin()`×2 +
        // `normal_iterator::operator+` + 迭代器版 insert（p167 EXTRA/MISSING 差集）。
        out.insert(out.end(), &block[0], &block[0] + written);
        pos += chunk;
        chunk = (len - pos > rsa_size) ? rsa_size : (len - pos);
    }
    return true;
}