#ifndef NSL_INTERNALMSG_H_
#define NSL_INTERNALMSG_H_

#include "MsgCell.h"
#include "Thread.h"

namespace nsl {

class TCPUser;
class ConInterface;

class InternalMsg : public CMsgCell
{
public:
    InternalMsg()
    {
        bActiveJob = false;
        bWillDelete = false;
        workIndex = -1;
        SetBuffer(buf);
        mCharacKey = 0;
        mOwnerWorkId = tlsThreadId;
    }
    virtual ~InternalMsg()
    {
    }
    void setDestCharacKey(int n)
    {
        mDestCharacKey = n;
    }
    int getDestCharacKey()
    {
        return mDestCharacKey;
    }
    bool isActiveJob()
    {
        return bActiveJob;
    }
    void setActiveJob(bool b)
    {
        bActiveJob = b;
    }
    void setConInfo(ConInterface* p)
    {
        pConInfo = p;
    }
    ConInterface* getConInfo()
    {
        return pConInfo;
    }
    void setNextJob(void* p)
    {
        pNextJob = p;
    }
    void* getNextJob()
    {
        return pNextJob;
    }
    void setTCPUser(TCPUser* p)
    {
        pTCPUser = p;
    }
    TCPUser* getTCPUser()
    {
        return pTCPUser;
    }
    void setCharacKey(unsigned int k)
    {
        mCharacKey = k;
    }
    unsigned int getCharacKey()
    {
        return mCharacKey;
    }

    void* pNextJob;
    ConInterface* pConInfo;
    char buf[128];
    char data[1920];
    bool bActiveJob;
    int workIndex;
    int mDestCharacKey;
    TCPUser* pTCPUser;
    unsigned int mCharacKey;
    bool bWillDelete;
    int mOwnerWorkId;
};

} // namespace nsl

#endif // NSL_INTERNALMSG_H_
