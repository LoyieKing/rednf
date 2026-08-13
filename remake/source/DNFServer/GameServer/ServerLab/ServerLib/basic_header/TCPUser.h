#ifndef NSL_TCPUSER_H_
#define NSL_TCPUSER_H_

#include <deque>

#include "Message.h"
#include "RecvBuffer.h"
#include "ThreadLock.h"

namespace nsl {

class TCPSocket;
class ISession;
class WorkThread;
class RecvBuffer;

extern int ddebug;

class TCPUser
{
public:
    enum ENUM_DATA_TYPE
    {
        SEND_DATA_NORMAL = 0,
        SEND_DATA_ENCRYPT = 1,
        SEND_DATA_COMPRESS = 2,
        SEND_DATA_ENCRYPT_COMPRESS = 3,
        RECV_DATA_NORMAL = 4,
        RECV_DATA_ENCRYPT = 5,
        RECV_DATA_COMPRESS = 6,
        RECV_DATA_ENCRYPT_COMPRESS = 7
    };

    TCPUser();
    ~TCPUser();
    int onRead();
    int onWriteByCMsg(CMsgCell* cell);
    void onError();
    bool onPassiveClose(char* file, int line);
    bool onActiveClose(unsigned int key);

    void setSendDataType(ENUM_DATA_TYPE type);
    void setRecvDataType(ENUM_DATA_TYPE type);
    ENUM_DATA_TYPE getSendDataType();
    ENUM_DATA_TYPE getRecvDataType();
    void setSocket(TCPSocket* pSock);
    TCPSocket* getSocket();
    int getHandle();
    void setNeedReconnect(bool bNeed);
    bool getNeedReconnect();
    void initialize();
    void shutdown();
    void startupAfterSetSocket();
    void onPopForTerminated();
    void setLastAccessTime();
    __int64 getLastAccessTime();
    bool isAboutToDisconnect() const;
    bool isDisconnected() const;
    void postDisconnected(int reason);
    void setSession(ISession* pSession);
    inline void setWorkThread(WorkThread* pWorkThread)
    {
        pmWorkThread = pWorkThread;
    }
    WorkThread* getWorkThread();
    ISession* getSession();
    inline unsigned int getUserId()
    {
        return mUserId;
    }
    inline bool isBindedSession()
    {
        return mbBindedSession;
    }
    void setBindedSession(bool bBinded);
    bool isPassiveCloseSyncByWorker();
    void setPassiveSyncByWorker(bool bSync);
    bool isActiveCloseSyncByWorker();
    void setActiveSyncByWorker(bool bSync);
    Message* PopSendMessage();
    inline void PushSendMessage(Message* pMsg)
    {
        mSendMessageQueue.push_back(pMsg);
    }
    inline void PushWouldBlockMessage(Message* pMsg)
    {
        mSendMessageQueue.push_front(pMsg);
    }
    void ClearRecvMsgs();
    void IncPendingWorkNum();
    void DecPendingWorkNum();
    void IncPendingSendNum();
    void DecPendingSendNum();
    inline unsigned int GetPendingWorkNum()
    {
        return mPendingWork;
    }
    inline unsigned int GetPendingSendNum()
    {
        return mPendingSend;
    }
    void SetSending(bool bSending);
    bool IsSending() const;
    void SetWorking(bool bWorking);
    bool IsWorking() const;
    inline int GetMaxPacketSize()
    {
        return mMaxPacketSize;
    }

    ENUM_DATA_TYPE mSendDataType;
    ENUM_DATA_TYPE mRecvDataType;
    int nReason_;
    int m_PosByWouldBlock;
    int m_SizeByWouldBlock;
    bool bDisconnected_;
    bool bAboutToDisconnect_;
    bool needReconnect_;
    TCPSocket* pSock_;
    ISession* pSession;
    __int64 tickLast_;
    __int64 DisLast_;
    unsigned int mUserId;
    WorkThread* pmWorkThread;
    bool mbPassiveCloseSyncByWorker;
    bool mbActiveCloseSyncByWorker;
    std::deque<Message*> wouldBlockQueue;
    std::deque<Message*> mSendMessageQueue;
    RecvBuffer mRecvBuffer;
    int mMaxPacketSize;
    ThreadLock mPendingSendLock;
    bool mbBindedSession;
    unsigned int mPendingSend;
    unsigned int mPendingWork;
    bool mbSending;
    bool mbWorking;
};

} // namespace nsl

#endif // NSL_TCPUSER_H_
