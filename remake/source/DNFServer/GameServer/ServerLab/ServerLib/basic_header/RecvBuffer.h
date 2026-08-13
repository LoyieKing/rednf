#ifndef NSL_RECVBUFFER_H_
#define NSL_RECVBUFFER_H_

#include <deque>

#include "ThreadLock.h"

namespace nsl {

class Message;
class TCPUser;

class RecvBuffer
{
public:
    RecvBuffer(int queueSize, int maxPacketSize);
    ~RecvBuffer();
    bool IsEmpty();
    bool IsFull();
    int AvailableSize();
    bool Parse(TCPUser* pUser);
    bool ClearUsedMsgs();
    char* GetFront();
    void AdjustRear(int size);
    char* GetRear();

    char* mQueue;
    std::deque<Message*> mRecvMsgs;
    int mFrontIdx;
    int mRearIdx;
    int mParseIdx;
    int mParseStatus;
    int mQueueSize;
    int mPartialQueueSize;
    int mMaxPacketSize;
};

} // namespace nsl

#endif // NSL_RECVBUFFER_H_
