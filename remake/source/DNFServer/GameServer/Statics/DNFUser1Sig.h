#ifndef DNF_USER1_SIG_H_
#define DNF_USER1_SIG_H_

#include "DNFSignal.h"

class CUser1Sig : public CSignal
{
public:
    CUser1Sig();
    virtual ~CUser1Sig();
    virtual void handle(int sig);
};

class CUser2Sig : public CSignal
{
public:
    CUser2Sig();
    virtual ~CUser2Sig();
    virtual void handle(int sig);
};

#endif // DNF_USER1_SIG_H_
