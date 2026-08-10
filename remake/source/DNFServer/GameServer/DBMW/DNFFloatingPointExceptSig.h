#ifndef DBMW_DNFFLOATINGPOINTEXCEPTSIG_H_
#define DBMW_DNFFLOATINGPOINTEXCEPTSIG_H_

#include "DBMWCommon.h"


class CFloatingPointExceptSig : public CSignal
{
public:
    CFloatingPointExceptSig();
    virtual ~CFloatingPointExceptSig();
    virtual void handle(int sig);
};

#endif  // DBMW_DNFFLOATINGPOINTEXCEPTSIG_H_
