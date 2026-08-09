#ifndef AUCTION_CHARACTER_H_
#define AUCTION_CHARACTER_H_

#include "ISession.h"
#include "IState.h"

class Zone;
class InterHandler;

namespace nsl {
class ConInterface;
class TCPUser;
}

namespace auction {

class Character : public nsl::ISession
{
public:
    Character();
    // Virtual order matches ISession/DWARF: onClose, onDoClose, then dtor.
    virtual void onClose(bool bActiveClosing);
    virtual void onDoClose();
    virtual ~Character();
    bool setArea(int areaIndex);
    void setCharacKey(unsigned int characKey);
    unsigned int getCharacKey();
    void setActiveTCPUser(nsl::ConInterface* info);
    void setPassiveTCPUser(nsl::TCPUser* u);
    void setInterHandler(InterHandler* pHandler);
    void setState(IState::STATE_COMMON in);
    void onMove();
    void onStand();
    void onAttack();
    void onCast();
    bool isActiveTCPUser()
    {
        return bActiveUser;
    }

    unsigned int xPos;
    unsigned int yPos;
    unsigned int otp;
    bool bActiveUser;
    IState* currentState;
    Zone* mZone;
    int mAreaIndex;
    unsigned int mCharacKey;
    InterHandler* TInterHandler;
    nsl::ConInterface* conInfo;
};

} // namespace auction

#endif // AUCTION_CHARACTER_H_
