void WongWork::CBlackIPMonitor::reset()
{
    // ORIG 0x82a6512：multimap::clear()。
    m_monitorMap.clear();
}