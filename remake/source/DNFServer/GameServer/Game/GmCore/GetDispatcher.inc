IDispatcher* CGameMasterMgr::GetDispatcher(int code)   // 0x084a8a9a
{
    return G_PacketDispatcher()->get_dispatcher(code);
