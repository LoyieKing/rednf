// ORIG ~CCreatureMgr @ 0x08339226（~CCreatureMgr.md）：
// 成员逆序析构（先 m_field30 map<int,int>，再 m_creatureItems
// map<int,CCreatureItem*>）；指针/标量成员无析构。用户定义使 D1/D2 为 T。
CCreatureMgr::~CCreatureMgr()
{
}
