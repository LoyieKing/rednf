#ifndef DBMW_DNFUSER1SIG_H_
#define DBMW_DNFUSER1SIG_H_

#include "DBMWCommon.h"


class CUser1Sig : public CSignal
{
public:
    CUser1Sig() {}
    virtual ~CUser1Sig() {}
    virtual void handle(int sig);
};

class CUser2Sig : public CSignal
{
public:
    CUser2Sig() {}
    virtual ~CUser2Sig() {}
    virtual void handle(int sig);
};

#endif  // DBMW_DNFUSER1SIG_H_
