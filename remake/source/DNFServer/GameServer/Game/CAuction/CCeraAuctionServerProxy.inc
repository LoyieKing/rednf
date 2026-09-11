CCeraAuctionServerProxy::CCeraAuctionServerProxy(char* ip, int port, char* name)
    : BaseServerProxy(ip, port, name)
{
    m_running = 0;
    Init();
}