// ORIG _ZN13user_creature9CCreature16IsCreatureScriptEv W 0x0833f0f2【objdump】：
// setne +0x44(m_script!=0)。
bool CCreature::IsCreatureScript()
{
    return m_script != 0;
}