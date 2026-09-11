// ORIG W _ZN27CInGameAdvertisementManager7RandNumEi @0x080fdd34：
// m_rand->randInt() 无符号 % range（xor edx / div 实证）。
unsigned int CInGameAdvertisementManager::RandNum(int range)
{
    return static_cast<unsigned int>(m_rand->randInt()) %
           static_cast<unsigned int>(range);
}