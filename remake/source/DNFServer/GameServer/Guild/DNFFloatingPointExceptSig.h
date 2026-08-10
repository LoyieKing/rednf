#ifndef DNFFLOATINGPOINTEXCEPTSIG_H_
#define DNFFLOATINGPOINTEXCEPTSIG_H_


#include "PacketHeader.h"
#include "DNFSignal.h"

class CFloatingPointExceptSig;

// from GuildSignal.h
class CFloatingPointExceptSig : public CSignal
{
public:
    CFloatingPointExceptSig() {}
    virtual ~CFloatingPointExceptSig();
    virtual void handle(int sig);
};

#endif
