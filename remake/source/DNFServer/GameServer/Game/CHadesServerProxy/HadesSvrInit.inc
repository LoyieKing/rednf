bool CHadesServerProxy::HadesSvrInit()
{
    ConnTcpHadesSvr();
    InitUdp();
    return 1;
}