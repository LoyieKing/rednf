CPrivateStore::~CPrivateStore() {}

// ORIG W 0x82345e4：返回 +0x08（m_dollCreated）是否非 0。
// dispatch_sig 购买链中用于判定卖家 store 已创建 doll（私商店铺实体）。
__attribute__((weak)) bool CPrivateStore::isDollCreated()
{
    return m_dollCreated;  // ORIG：movzbl +0x08 直返，不做 0/1 归一化
}