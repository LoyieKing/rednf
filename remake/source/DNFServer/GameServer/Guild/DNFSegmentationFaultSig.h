#ifndef DNFSEGMENTATIONFAULTSIG_H_
#define DNFSEGMENTATIONFAULTSIG_H_


#include "PacketHeader.h"
#include "DNFSignal.h"

class CSegmentationFaultSig;

// from GuildSignal.h
class CSegmentationFaultSig : public CSignal
{
public:
    CSegmentationFaultSig();
    virtual ~CSegmentationFaultSig();
    virtual void handle(int sig);
};

#endif
