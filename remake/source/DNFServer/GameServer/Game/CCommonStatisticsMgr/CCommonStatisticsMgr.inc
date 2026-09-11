// ORIG C1 0x86104e8 W：operator new(0x1c) × 3，各容器构造后依次存
// m_container[0..2]，每个 new 失败路径 delete 回滚。
CCommonStatisticsMgr::CCommonStatisticsMgr()
{
    m_container[0] = new CHelpedPartyCount;
    m_container[1] = new CPVPPlayerRate;
    m_container[2] = new CPVPMapCount;
}