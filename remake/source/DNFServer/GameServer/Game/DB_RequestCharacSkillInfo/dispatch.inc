// ORIG 0x084474f2：从 stream 的 out buffer 取 SIG，先取技能树索引，再取技能表，
// 成功才 sendResult(slot, sig)；任一失败返回 0。
bool DB_RequestCharacSkillInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_REQUEST_CHARAC_SKILL_INFO* sig = stream->GetOutBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
    bool ok = _GetSkillTreeIndex(sig->m_characNo, &sig->m_skillTreeIndex);
    if (!ok)
    {
        return 0;
    }
    ok = _GetSkillInfo(sig->m_characNo, sig->m_skillTreeIndex, sig->m_skills);
    if (!ok)
    {
        return 0;
    }
    sendResult(slot, sig);
    return 1;
}