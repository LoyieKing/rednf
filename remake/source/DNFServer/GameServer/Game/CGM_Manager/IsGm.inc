bool CGM_Manager::IsGm(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    // ORIG 0x082994ee：`test %al,%al; je +0x6b(mov $1)` ⇒ al==0（find 未命中）
    // 返回 **1**；命中返回 0。原实现 `if(it==end()) return true;` 语义与 ORIG
    // **相反**（真 bug：命中 GM 反而返回 false）——按 ORIG 汇编改正极性。
    if (it == m_gmMap.end())
        return false;
    return true;
