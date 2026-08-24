#include "CGM_Manager.h"

// ORIG 符号: _ZN11CGM_Manager4IsGmEj @ 0x829948c
// readGuard(m_lock)；m_gmMap.find(accId) != end() 即 GM。
bool CGM_Manager::IsGm(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    return m_gmMap.find(accId) != m_gmMap.end();
}

// ORIG 符号: _ZN11CGM_Manager10TurnGmModeEj @ 0x829917c
// readGuard(m_lock)；找到记录则翻转其模式位（0<->1）并返回新值，否则返回 0。
unsigned char CGM_Manager::TurnGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    char& mode = *reinterpret_cast<char*>(&it->second.m_mode);
    mode = (mode == 0) ? 1 : 0;
    return static_cast<unsigned char>(mode);
}

// ORIG 符号: _ZN11CGM_Manager16GetCurrentGmModeEj @ 0x8299276
// readGuard(m_lock)；找到记录返回其模式字节，否则 0。
char CGM_Manager::GetCurrentGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    return *reinterpret_cast<const char*>(&it->second.m_mode);
}

// ORIG 符号: _ZN11CGM_Manager10GetGmLevelEj @ 0x082993d8
// readGuard(m_lock)；m_gmMap.find(accId)==end() → 0；否则返回记录 +0x04（m_level）。
int CGM_Manager::GetGmLevel(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    return it->second.m_level;
}
