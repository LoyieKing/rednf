// ORIG 0x080c8446：直接 call CEventBase::GetAddInfo()（非虚，sret 写 out
// 指针；GetAddInfo 返回 stADDINFO，2026-08-28 OrigEventLeaf8f 修正 ABI），
// 返回 m_info0 符号扩展（movzwl+cwtl，与 ORIG 逐条同形；AE 实证）。
int AttendanceEvent::getRewordItem()
{
    return (short)CEventBase::GetAddInfo().m_info0;
}