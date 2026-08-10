#ifndef DNFTERMINATESIG_H_
#define DNFTERMINATESIG_H_


#include "PacketHeader.h"
#include "DNFSignal.h"

class CTerminateSig;

// from GuildSignal.h
class CTerminateSig : public CSignal
{
public:
    CTerminateSig();
    virtual ~CTerminateSig();
    virtual void handle(int sig);
};

#endif
