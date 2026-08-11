#ifndef DNF_USER1_SIG_H_
#define DNF_USER1_SIG_H_

#include "DNFSignal.h"

class CUser1Sig : public CSignal
{
public:
    CUser1Sig() throw();
    virtual ~CUser1Sig() throw();
    virtual void handle(int sig);
};

class CUser2Sig : public CSignal
{
public:
    CUser2Sig() throw();
    virtual ~CUser2Sig() throw();
    virtual void handle(int sig);
};

#endif // DNF_USER1_SIG_H_
