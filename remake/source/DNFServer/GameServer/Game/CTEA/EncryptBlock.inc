// ---- ORIG _ZN4CTEA12EncryptBlockEPKhPh（0x080c3724）TEA 分组加密核心 ----
// [b24DBG · DB 1045 根因修正] 密钥字必须取 **m_iv**（+0x28）而非 m_key（+0x18）：
//   · ORIG Initialize 的四次 BytesToWord(buf+i*4) 目标 = this+0x28/0x2c/0x30/0x34
//     （0x080c3538-0x080c3595 实证）；EncryptBlock 读密钥亦为 [this+0x28+(sum&3)*4]
//     （0x080c378c / 0x080c37b6，gcc 折成 [edx+eax*4+0x8], eax=idx+8 —— 即源码直接
//     写 m_iv[...]，未走本地点针：ORIG 循环内每次都重载 [ebp+0x8]）。
//   · m_key 是 **char 缓冲**，按 (uint*) 解释在小端机上字节反序
//     （"qort..."→0x74726f71…），而 m_iv 经大端 BytesToWord 生成后恰好等于权威密钥字
//     KEY="troqkddmtroqkcdm" 的小端 unpack（0x716f7274,0x6d64646b,0x716f7274,0x6d64636b）。
//   · 旧形另用标准 TEA 混合式，亦与 ORIG 不符。正确式（0x080c3772-0x80c37be 逐指令）：
//       混合 = (((v<<4) ^ (v>>5)) + v)，自变量 v 为**被更新方之外**的那个字；
//       轮内 = v0 += 混合(v1) ^ (m_iv[sum & 3] + sum);  sum += delta;
//              v1 += 混合(v0) ^ (m_iv[(sum >> 11) & 3] + sum);
//     （sum 先用于 v0、+delta 后才供 v1 使用；键索引 sum&3 / (sum>>11)&3 次序不可交换。）
// 修正前 master_db_pwd 解出乱码 ⇒ MySQL error 1045。
int CTEA::EncryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0;
    unsigned int v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    unsigned int sum = 0;
    const unsigned int delta = 0x9e3779b9;
    for (int n = 0; n < 32; ++n)
    {
        v0 += ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (m_iv[sum & 3] + sum));
        sum += delta;
        v1 += ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (m_iv[(sum >> 11) & 3] + sum));
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
    return 0x6fffffff;
}
