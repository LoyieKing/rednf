void CSession::AddTriggerSessionEventType(int type)   // ORIG 0x849004a（|=）
{
    // int 字段 |= int 形参（无转换节点）：4.4 与 ORIG 逐条同形（原 N18a 判词
    // 在 unsigned 字段+cast 异型下成立；同型化后翻 identical）。
    m_triggerEventType |= type;
