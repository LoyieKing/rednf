// ORIG _ZN5CUser14setStdDropRateEi @ 0x0828715c (W)：
// *(int*)(this+0x79624) = rate，即 m_codeHackCheck(+0x795fc).m_field28。
void CUser::setStdDropRate(int rate)
{
    m_postChecksumData.m_field28 = rate;
}