// ============================================================================
// stMapPlayInfo_t::getPlayTick() —— ORIG W 0x830e7ea
// [_ZN15stMapPlayInfo_t11getPlayTickEv]（objdump 转写，见文件头标记；ORIG 非 const）
//   return m_clearTick - m_startTick;（unsigned 减法回绕）
// ============================================================================
unsigned int stMapPlayInfo_t::getPlayTick()
{
    return m_clearTick - m_startTick;
}