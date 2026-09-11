void CAchievement::getData(char* pData) const
{
    stAchievement_DBData* out = (stAchievement_DBData*)pData;
    int i = 0;
    for (std::map<unsigned int, stAchievement>::const_iterator it =
             m_achievements.begin();
         it != m_achievements.end(); ++it, ++i)
    {
        // ORIG 0x828c6e0：按 12 字节整块复制 value（3 个 dword），非逐字段
        out->m_entries[i] = it->second;
    }
    out->m_count = (int)m_achievements.size();
    // b28c-L10 判词（AE）：m_saveTime 声明 unsigned int（CAchievement.h）后，与
    // getCurSec() int 返回值异型，4.4 store 翻为 ORIG 的 RHS-first 形
    // `call; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,0x1804(%eax)`（P211②）。
    out->m_saveTime = GlobalData::s_systemTime_.getCurSec();
}