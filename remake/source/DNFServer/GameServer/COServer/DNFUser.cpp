// df_coserver_r — CUser（ORIG DNFUser.cpp）

#include "DNFUser.h"
#include "DNFGameServer.h"

CUser::CUser()
{
    m_gs = 0;
}

CUser::CUser(CGameServer* gs)
{
    m_gs = gs;
}

CUser::~CUser()
{
    m_gs = 0;
}

CGameServer* CUser::GetGameServer()
{
    return m_gs;
}

void CUser::SendToGameserver(char* buf, int len)
{
    if (m_gs != 0)
    {
        m_gs->SendToGameServer(buf, len);
    }
}
