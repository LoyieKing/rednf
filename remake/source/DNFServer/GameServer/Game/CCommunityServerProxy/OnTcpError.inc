// ORIG T 0x0846c962：errno!=0 → 报错返 0；否则返 1。
bool CCommunityServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x10f, 0)
            ("CommunityServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}