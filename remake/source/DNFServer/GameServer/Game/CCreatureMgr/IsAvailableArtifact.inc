bool CCreature::IsAvailableArtifact(int slot)
{
    if (m_script == 0)
        return false;
    // ORIG 0x833771d：call 后无 test/setne，直落 %eax。
    return m_script->IsAvailableArtifactSlot(slot);
}
