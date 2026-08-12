#ifndef DNFSYSTEMFAILSIG_H_
#define DNFSYSTEMFAILSIG_H_


#include "PacketHeader.h"
#include "DNFSignal.h"

class CSystemFailSig;

// from GuildSignal.h
class CSystemFailSig : public CSignal
{
public:
    CSystemFailSig() throw();
    virtual ~CSystemFailSig();
    virtual void handle(int sig);
};

#endif
