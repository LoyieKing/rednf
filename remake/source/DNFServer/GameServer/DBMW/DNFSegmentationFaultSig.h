#ifndef DBMW_DNFSEGMENTATIONFAULTSIG_H_
#define DBMW_DNFSEGMENTATIONFAULTSIG_H_

#include "DBMWCommon.h"


class CSegmentationFaultSig : public CSignal
{
public:
    CSegmentationFaultSig();
    virtual ~CSegmentationFaultSig();
    virtual void handle(int sig);
};

#endif  // DBMW_DNFSEGMENTATIONFAULTSIG_H_
