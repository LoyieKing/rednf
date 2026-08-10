#ifndef DNF_SYSTEM_FAIL_SIG_H_
#define DNF_SYSTEM_FAIL_SIG_H_

#include "DNFSignal.h"

class CSystemFailSig : public CSignal
{
public:
    CSystemFailSig() throw();
    virtual ~CSystemFailSig() throw();
    virtual void handle(int sig);
};

#endif  // DNF_SYSTEM_FAIL_SIG_H_
