void CSession::SetTriggerSessionEventType(int type)   // ORIG 0x8490038
{
    // int 字段 + int 形参同型直赋（无转换节点）：4.4 LHS 先载=ORIG 实证。
    m_triggerEventType = type;
}