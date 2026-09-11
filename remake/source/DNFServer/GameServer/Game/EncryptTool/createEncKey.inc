// [b28d-lane6] DIFF·豁免：加密/密钥生成类按用户规则只要求算法正确（不强制 identical）；258 vs 238，语义经 decrypt 往返验证。
// [P6AuditOldN.P6S2 2026-09-11 · 本波复核] n1_v DIFF 258/238 gap=+20（本片最大缺口）。
//   ORIG 0x0848d5b0 索引/常量计算段：`mov 0x200(%eax),%ecx; …; movzbl 0x9(%eax),%eax;
//   mov %eax,%edx; …; lea (%edx,%eax,1),%eax` vs 我方 `mov 0x200(%eax),%edx; …;
//   movzbl 0x9(%eax),%ecx; …; lea (%ecx,%eax,1),%eax; lea 0x5(%eax),%ecx; shl $0x5,%edx;
//   mov %cl,(%edx,%eax,1)`。新钥匙判定：P339（无槽序入口）/P59（无守卫）/真 switch
//   （无 cmp 链）/类型轴三式（掩码 cast 形未现）均不适用，属 4.1↔4.4 索引计算与
//   装载序；未落地，维持 N（未决）。
void EncryptTool::createEncKey()
{
    unsigned int local_10 = time(0);
    unsigned int local_14 = local_10;
    m_keyIndex = local_14 & 0xf;
    local_14 = m_keyIndex;
    Encryption::SeedRand(local_14);

    m_key[m_keyIndex][0]  = (char)(m_key[0][9]  + m_keyIndex + 0x05);
    m_key[m_keyIndex][1]  = (char)(m_key[1][8]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][2]  = (char)(m_key[2][7]  + m_keyIndex + 0x04);
    m_key[m_keyIndex][3]  = (char)(m_key[3][6]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][4]  = (char)(m_key[4][5]  + m_keyIndex);
    m_key[m_keyIndex][5]  = (char)(m_key[5][4]  + m_keyIndex + 0x07);
    m_key[m_keyIndex][6]  = (char)(m_key[6][3]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][7]  = (char)(m_key[7][2]  + m_keyIndex + 0x06);
    m_key[m_keyIndex][8]  = (char)(m_key[8][1]  + m_keyIndex + 0x03);
    m_key[m_keyIndex][9]  = (char)(m_key[9][0]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][10] = (char)(m_key[10][5] + m_keyIndex + 0x02);
    m_key[m_keyIndex][11] = (char)(m_key[11][4] + m_keyIndex);
    m_key[m_keyIndex][12] = (char)(m_key[12][3] + m_keyIndex + 0x07);
    m_key[m_keyIndex][13] = (char)(m_key[13][2] + m_keyIndex + 0x02);
    m_key[m_keyIndex][14] = (char)(m_key[14][1] + m_keyIndex + 0x06);
    m_key[m_keyIndex][15] = (char)(m_key[15][0] + m_keyIndex + 0x03);

    m_rijndael->Initialize((const char*)m_key[m_keyIndex], 0x20,
                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}