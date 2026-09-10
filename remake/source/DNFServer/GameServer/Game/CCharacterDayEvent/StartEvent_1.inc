void CCharacterDayEvent::StartEvent(Word_Param param)
{
    // [b28c-27] N18a：ORIG `movzwl 0xc(%ebp),%eax; mov %eax,%edx; ...; mov %dx,0xa`
    // （零扩展经 eax 中转再落 edx）；4.4 恒折 `movzwl 0xc(%ebp),%edx` 单段。
    // ORIG 0x0810a58e：SetEventFlag(true)；movzwl 0xc(%ebp),%eax → mov %eax,%edx
    // → mov %dx,0xa 写 m_eventJob（unsigned ushort 目标，零扩展直写 16 位）；
    // cMyTrace(__FUNCTION__,0x1f,0)(...)。残差 N18a：4.4 恒折
    // movzwl 0xc(%ebp),%edx 单段（两段中转不可复现）。
    SetEventFlag(true);
    m_eventJob = param.m_duration;
    cMyTrace(__FUNCTION__, 0x1f, 0)("Character Day Event Start!");
