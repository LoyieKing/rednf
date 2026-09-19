// ---- ORIG _ZN4CTEA12DecryptBlockEPKhPh（0x080c3804）TEA 分组解密核心 ----
// [b24DBG · DB 1045 根因修正] 与 EncryptBlock 同源：密钥字取 **m_iv**（+0x28，
// 见 EncryptBlock.inc 抬头注释的 ORIG 实证），混合式 = (((v<<4)^(v>>5))+v)。
//   轮内 = v1 -= 混合(v0) ^ (m_iv[(sum >> 11) & 3] + sum);  sum -= delta;
//          v0 -= 混合(v1) ^ (m_iv[sum & 3] + sum);
//   sum 初值 = delta<<5 = 0xc6ef3720（0x080c383d 立即数实证）。
// 自检（对本工程密钥 "qortmddkqortmdck" 的 m_iv 字）：
//   DecryptBlock(20e35501e56fcedb) == "uu5!^%jg"  ← TeaEncrypt.java 的 GETKEY 输出；
//   DecryptBlock(e8b10c1f8bc3595b) == 全 0        ← POSTFIX 块，故 DecryptValue 取前 0x14 字节即口令。
// ⚠ ORIG 本函数**无 return 语句**：0x080c38dc 起直接 add esp/pop/ret，无 mov eax,imm
//   （对比 EncryptBlock 0x080c37fc 有 mov eax,0x6fffffff）。为逐字节对齐 ORIG，此处同样
//   省略 return（gcc -O0 落空，eax 为上一调用残留）；调用方 Encrypt/Decrypt/DecryptValue
//   均丢弃该返回值 ⇒ 语义不受影响。
int CTEA::DecryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0;
    unsigned int v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    unsigned int sum = 0xc6ef3720;
    const unsigned int delta = 0x9e3779b9;
    for (int n = 0; n < 32; ++n)
    {
        v1 -= ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (m_iv[(sum >> 11) & 3] + sum));
        sum -= delta;
        v0 -= ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (m_iv[sum & 3] + sum));
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}
