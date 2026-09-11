// C1 ORIG 0x8493e36（disasm 5169-5200）：ThreadInterface()（vptr 派生）→
// 构造 +0x0c 处 m_exchangeServer → 将其地址写入静态全局 exchange_server_。
CExchangeServerThread::CExchangeServerThread()
{
    CSession::exchange_server_ = static_cast<IExchangeServer*>(&m_exchangeServer);
}