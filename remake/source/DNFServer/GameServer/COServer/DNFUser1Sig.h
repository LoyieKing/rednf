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

// CUser2Sig：coserver 原版 ORIG .o 清单中无独立 DNFUser2Sig.cpp，
// 与 CUser1Sig 同源文件（DNFSignal.cpp 内共同定义）
class CUser2Sig : public CSignal
{
public:
    CUser2Sig() throw();
    virtual ~CUser2Sig() throw();
    virtual void handle(int sig);
};

#endif // DNF_USER1_SIG_H_
