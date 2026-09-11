// ORIG W 0x08134838（objdump）：return *(int*)(this+4)。
int ProcStage::getState() const
{
    return m_state;
}