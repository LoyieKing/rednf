#ifndef NSL_ISESSION_H_
#define NSL_ISESSION_H_

namespace nsl {

class TCPUser;

class ISession
{
public:
    ISession()
    {
    }
    // Virtual order must match auction DWARF/vtable: onClose, onDoClose, then dtor.
    virtual void onClose(bool bActiveClosing) = 0;
    virtual void onDoClose() = 0;
    virtual ~ISession()
    {
    }
    inline void setTCPUser(TCPUser* pUser)
    {
        pTCPUser = pUser;
    }
    inline TCPUser* getTCPUser()
    {
        return pTCPUser;
    }

    TCPUser* pTCPUser;
};

} // namespace nsl

#endif // NSL_ISESSION_H_
