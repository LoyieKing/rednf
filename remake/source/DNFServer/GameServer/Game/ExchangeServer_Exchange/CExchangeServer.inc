// ---------------------------------------------------------------------------
// C1 / D1                                  ORIG 0x8493ae6 / 0x8493b82
// 成员按地址序构造：sessionMgr(+0x0c) → localInfo(+0x40) → map(+0x50)
// → pool(+0x6c)，尾调 Reset。析构逆序由默认成员析构承担。
// ---------------------------------------------------------------------------
// ORIG 0x8493ae6：仅 IExchangeServer 基类 + 成员按序默认构造（sessionMgr
// → localInfo → map → pool）→ Reset()。标量成员（+0x04/+0x08/+0x0a/+0x38/
// +0x39/+0x3c/+0x68）不初始化（ORIG 无清零指令）。
CExchangeServer::CExchangeServer()
    : IExchangeServer(),
      m_sessionMgr(),
      m_localInfo(),
      m_serverMap(),
      m_sessionPool()
{
    Reset();
}