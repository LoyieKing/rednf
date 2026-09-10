bool GetCharacterJob(CUser& user, char& outJob)
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }
    outJob = cur->m_field26;  // job 字节（+0x26）
    return true;
