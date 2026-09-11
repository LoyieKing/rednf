// ---------------------------------------------------------------------------
// ORIG W 0x08234516（objdump：mov 0x1ac(%eax),%eax; ret）：
// m_serverEnvir(+0x198) + 0x14 = m_processSequence。
int CEnvironment::getProcessSequence() const
{
    return m_serverEnvir.m_processSequence;
}