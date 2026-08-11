#ifndef DNF_TERMINATE_SIG_H_
#define DNF_TERMINATE_SIG_H_

#include "DNFSignal.h"

class CTerminateSig : public CSignal
{
public:
    CTerminateSig() throw();
    virtual ~CTerminateSig() throw();
    virtual void handle(int sig);
};

#endif // DNF_TERMINATE_SIG_H_
