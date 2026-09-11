// ORIG _ZN5CUser24GenerateCleanPadVeriDataERtRhS0_ T 0x08649ac4：生成三段
// CleanPad 验证种子（ushort randInt(0xffff)+1 / uchar randInt(6) /
// ushort randInt(0xffff)+1）存 m_mcap 并回填出参。
void CUser::GenerateCleanPadVeriData(unsigned short& outA, unsigned char& outB,
                                     unsigned short& outC)
{
    m_mcap.m_cleanPadVeriA =
        static_cast<unsigned short>(get_rand_int(0xffff) + 1);
    m_mcap.m_cleanPadVeriB = static_cast<unsigned char>(get_rand_int(6));
    m_mcap.m_cleanPadVeriC =
        static_cast<unsigned short>(get_rand_int(0xffff) + 1);
    outA = m_mcap.m_cleanPadVeriA;
    outB = m_mcap.m_cleanPadVeriB;
    outC = m_mcap.m_cleanPadVeriC;
}