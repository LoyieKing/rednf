// ---- GetCharacterJob —— T 0x082712a6（同名报告全文转写）--------------------
bool GetCharacterJob(CUser& user, char& outJob)
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }
    outJob = cur->m_job;  // job 字节（+0x26）
    return true;
}