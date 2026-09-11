// GetServerPORT ORIG 0x848f2a3：G_CEnvironment()->get_exchange_server_port()
// （int 截 ushort 返回，ORIG movswl/mov %dx 同宽度语义）。
int GetServerPORT()
{
    return G_CEnvironment()->get_exchange_server_port();
}