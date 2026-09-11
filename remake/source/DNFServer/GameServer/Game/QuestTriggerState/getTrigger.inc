// ORIG 0x0836dd68：每字段 `movzwl; andw $0x1ff,%ax; movzwl %ax,%eax`（位域读），
// 后两字段 `shl` 后 `or %eax,-0x4(%ebp)` ⇒ 具名 int r + |= 形（探针 23=23）。
int stQuestTriggerState_t::getTrigger()
{
    int r = m_trigger0;
    r |= (m_trigger1 << 9);
    r |= (m_trigger2 << 18);
    return r;
}