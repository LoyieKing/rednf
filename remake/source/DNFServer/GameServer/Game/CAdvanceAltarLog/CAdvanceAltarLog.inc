// ----------------------------------------------------------------------------
// ORIG T 0x08156aaa（ctor）：Mutex() → map(+0x18)/map(+0x30) 构造 → initStatistics。
// ----------------------------------------------------------------------------
CAdvanceAltarLog::CAdvanceAltarLog()
    : m_mutex(), m_entrances(), m_layers()
{
    initStatistics();
}