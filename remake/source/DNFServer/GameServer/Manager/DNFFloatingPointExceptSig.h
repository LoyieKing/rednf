#ifndef DNF_FLOATING_POINT_EXCEPT_SIG_H_
#define DNF_FLOATING_POINT_EXCEPT_SIG_H_

#include "DNFSignal.h"

class CFloatingPointExceptSig : public CSignal
{
public:
    CFloatingPointExceptSig() throw();
    virtual ~CFloatingPointExceptSig() throw();
    virtual void handle(int sig);
};

#endif  // DNF_FLOATING_POINT_EXCEPT_SIG_H_
