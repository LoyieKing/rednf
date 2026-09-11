// ORIG _ZN13user_creature9CCreature19IsAvailableArtifactEi T 0x83376f6：
// m_script==0 → false；否则委托 CCreatureScript::IsAvailableArtifactSlot(slot)。
bool CCreature::IsAvailableArtifact(int slot)
{
    if (m_script == 0)
        return false;
    // ORIG 0x833771d：call 后无 test/setne，直落 %eax。
    return m_script->IsAvailableArtifactSlot(slot);
}
