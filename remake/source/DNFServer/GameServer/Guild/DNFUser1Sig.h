#ifndef DNFUSER1SIG_H_
#define DNFUSER1SIG_H_


#include "PacketHeader.h"
#include "DNFSignal.h"

class CUser1Sig;
class CUser2Sig;

// from GuildSignal.h
class CUser1Sig : public CSignal
{
public:
    CUser1Sig();
    virtual ~CUser1Sig();
    virtual void handle(int sig);
};

// from GuildSignal.h
class CUser2Sig : public CSignal
{
public:
    CUser2Sig();
    virtual ~CUser2Sig();
    virtual void handle(int sig);
};

#endif
