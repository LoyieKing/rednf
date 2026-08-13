#ifndef NSL_INTERNALMSGDESIGN_H_
#define NSL_INTERNALMSGDESIGN_H_

#include <string.h>

#include "PacketDesign.h"

namespace nsl {

class TCPUser;
class ConInterface;
extern __thread unsigned int tlsThreadId;  // 与 Thread.h 一致，供 ctor 初始化 mOwnerWorkId

#pragma pack(push, 1)
struct INTERNALMSG_HEADER
{
    void* pNextJob;
    bool bActiveJob;
    int workIndex;
    int mDestCharacKey;
    TCPUser* pTcpUser;
    unsigned int characKey;
    bool bWillDelete;
    int mOwnerWorkId;
    char classification;
    char msg_no;
    unsigned int sLength;

    INTERNALMSG_HEADER()
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

    unsigned int getInternalMsgID()
    {
        return (unsigned int)msg_no;
    }

    void setInternalMsgID(int n)
    {
        msg_no = (char)n;
    }

    int getSize()
    {
        return sLength;
    }

    void setSize(int n)
    {
        sLength = n;
    }

    unsigned int getCategory()
    {
        return (unsigned int)classification;
    }

    void setCategory(int n)
    {
        classification = (char)n;
    }
};

#pragma pack(pop)

} // namespace nsl

#pragma pack(push, 1)
struct INTERNALMSG_SERVICE_UNAVAILABLE : public nsl::INTERNALMSG_HEADER
{
    int reason;

    INTERNALMSG_SERVICE_UNAVAILABLE()
    {
        memset(this, 0, sizeof(INTERNALMSG_SERVICE_UNAVAILABLE));
        setCategory(0);
        setInternalMsgID(0);
        setSize(sizeof(INTERNALMSG_SERVICE_UNAVAILABLE));
        // ORIG：setSize 后按序显式清 bWillDelete(0x15)/workIndex(0x5)/mOwnerWorkId(0x16)，
        // 再置 bActiveJob(0x4)（memset 已清零，冗余但机器码可见，缺则差 6 条）。
        bWillDelete = false;
        workIndex = 0;
        mOwnerWorkId = 0;
        bActiveJob = false;
    }
};

struct INTERNALMSG_DESTROY_CHARACTER : public nsl::INTERNALMSG_HEADER
{
    bool bActiveClosing;
    nsl::ConInterface* pConInfo;
    int areaIndex;

    INTERNALMSG_DESTROY_CHARACTER()
    {
        memset(this, 0, sizeof(INTERNALMSG_DESTROY_CHARACTER));
        setCategory(1);
        setInternalMsgID(2);  // ORIG：internal msg ID = 2（原误写 0）
        setSize(sizeof(INTERNALMSG_DESTROY_CHARACTER));
        // ORIG：setSize 后还有 4 个成员初始化（bWillDelete/workIndex/mOwnerWorkId/bActiveJob）
        bWillDelete = false;
        workIndex = 0;
        mOwnerWorkId = nsl::tlsThreadId;
        bActiveJob = false;
    }
};

#pragma pack(pop)

#endif // NSL_INTERNALMSGDESIGN_H_
