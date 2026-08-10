// df_relay_r — TCPHandler/TCPHandlerRelay（GCC 4.1.2, 无 DWARF）
#include "TCPHandler.h"
#include "TCPUser.h"
#include "RelayService.h"
#include "Helper.h"

namespace RelayServiceApp
{
TCPHandler::TCPHandler()
{
}

TCPHandlerRelay::TCPHandlerRelay()
{
}

TCPHandlerRelay::~TCPHandlerRelay()
{
}

void TCPHandlerRelay::dispatch(TCPUser* user, char* buf, int size, int flag)
{
    if (*(short*)buf == 0)
    {
        if (user->getACCID() == 0)
        {
            unsigned int new_acc = *(unsigned int*)(buf + 4);
            if (new_acc == 0)
            {
                user->onClose();
            }
            else
            {
                TCPUser* old = getManager()->m_users.getTCPUser(new_acc);
                if (old != 0)
                {
                    old->onClose();
                }
                user->setACCID(new_acc);
                getManager()->m_users.setTCPUser(new_acc, user);
                getManager()->setAuthenticated(new_acc);
            }
        }
    }
    else if (*(short*)buf == 1)
    {
        if (user->getACCID() == 0)
        {
            user->onClose();
        }
        else
        {
            user->setLastAccessTime();
            getManager()->relayToTCP((PacketHeader*)buf);
        }
    }
}


} // namespace RelayServiceApp
