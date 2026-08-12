#ifndef DBMW_DNFSYSTEMFAILSIG_H_
#define DBMW_DNFSYSTEMFAILSIG_H_

#include "DBMWCommon.h"


class CSystemFailSig : public CSignal
{
public:
    CSystemFailSig() {}
    virtual ~CSystemFailSig() {}
    virtual void handle(int sig);
};

#endif  // DBMW_DNFSYSTEMFAILSIG_H_
