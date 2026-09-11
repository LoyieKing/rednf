// ORIG 0x085ca3d6（报告 private_store__CPrivateStoreMgr/CPrivateStoreMgr.md 逐指令）：
// IUserPos 基类构造（写基类 vptr）→ 派生 vptr → 构造 +0x04 store、+0x68 池、
// +0x98/+0xb0/+0xc8 三张表，函数体再对三表 clear()。析构序与成员声明逆序一致。
CPrivateStoreMgr::CPrivateStoreMgr()
{
    m_storeSellers.clear();
    m_storeBuyers.clear();
    m_zoneStores.clear();
}