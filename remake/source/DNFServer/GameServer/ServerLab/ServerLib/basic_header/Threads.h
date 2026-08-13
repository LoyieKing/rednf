#ifndef NSL_THREADS_H_
#define NSL_THREADS_H_

namespace nsl {

class TCPThread;
class UDPThread;
class TimerThread;
class WorkThread;
class NSLDBThread;
class TCPSendThread;
class LogSendThread;

class Threads
{
public:
    Threads();
    inline TCPThread* getTCPThread()
    {
        return threadTCP_;
    }
    inline TCPSendThread* getTCPSendThread()
    {
        return threadTCPSend_[mMostLittleSendQueue];
    }
    inline WorkThread* getWorkThread(int idx)
    {
        return threadWork_[idx];
    }
    inline TimerThread* getTimerThread()
    {
        return threadTimer_;
    }
    inline WorkThread* getLittleWorkThread()
    {
        return threadWork_[mMostLittleOrderQueue];
    }
    inline int getWorkThreadNum()
    {
        return mWorkThreadNum;
    }
    inline NSLDBThread* getDBThread(int idx)
    {
        return threadDB_[idx];
    }
    void setMostLittleQueue();
    int getScopedLittleWorkIdx(int idx, int end);

    TCPThread* threadTCP_;
    UDPThread* threadUDP_;
    TimerThread* threadTimer_;
    WorkThread* threadWork_[5];
    NSLDBThread* threadDB_[4];
    TCPSendThread* threadTCPSend_[1];
    LogSendThread* logSendThread_;
    int mMostLittleOrderQueue;
    int mMostLittleSendQueue;
    int mWorkThreadNum;
};

} // namespace nsl

#endif // NSL_THREADS_H_
