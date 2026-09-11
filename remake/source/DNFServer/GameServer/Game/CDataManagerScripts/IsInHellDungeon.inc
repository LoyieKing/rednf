// ---- CWorldMap 访问器（ORIG 无独立报告；按 objdump 直读字节字段实现） ----
bool CWorldMap::IsInHellDungeon()   // ORIG W 0x830e6ba：movzbl 0x4(%eax)（非 const，直读 bool）
{
    return m_isHellDungeon;
}