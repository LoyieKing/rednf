// ORIG 0x084474f2：从 stream 的 out buffer 取 SIG，先取技能树索引，再取技能表，
// 成功才 sendResult(slot, sig)；任一失败返回 0。
bool DB_RequestCharacSkillInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_REQUEST_CHARAC_SKILL_INFO* sig = stream->GetOutBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
    bool ok = _GetSkillTreeIndex(*(unsigned int*)&sig->m_pad[0], &sig->m_pad[0x19c]);
    if (!ok)
    {
        return 0;
    }
    ok = _GetSkillInfo(*(unsigned int*)&sig->m_pad[0], *(char*)&sig->m_pad[0x19c],
                       (_Mastered_skill*)((char*)sig + 4));
    if (!ok)
    {
        return 0;
    }
    sendResult(slot, sig);
    return 1;
}