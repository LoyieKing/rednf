#ifndef DBMW_DNFTERMINATESIG_H_
#define DBMW_DNFTERMINATESIG_H_

#include "DBMWCommon.h"
#include "DNFSignal.h"


class CTerminateSig : public CSignal
{
public:
    CTerminateSig() {}
    virtual ~CTerminateSig() {}
    virtual void handle(int sig);
};

#endif  // DBMW_DNFTERMINATESIG_H_
