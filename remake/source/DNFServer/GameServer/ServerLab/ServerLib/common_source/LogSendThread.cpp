#include <assert.h>
#include <string.h>

#include "LogSendThread.h"
#include "Message.h"
#include "TCPUser.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "DataPool.h"
#include "TActiveConnect.h"
#include "DNFPacket.h"
#include "System.h"

namespace nsl {

LogSendThread::LogSendThread()
{
    mLogCon = new ConInterface;
}

LogSendThread::~LogSendThread()
{
}

void LogSendThread::loop(void* temp)
{
    Message* msg;
    while (true)
    {
        mThreadLock.lock();
        // ORIG: empty(); xor $1; je empty_path  => if (!empty()) process else empty
        if (!mLogMessagePool.empty())
        {
            msg = mLogMessagePool.front();
            mLogMessagePool.pop_front();
            mThreadLock.unlock();
            // ORIG || expansion: about? -> true; else check disconnected -> 0/1; test
            if (mLogUser->isAboutToDisconnect() || mLogUser->isDisconnected())
            {
                pApp->super_DataPools.getDataPool()->destroyLogMessage(msg);
                TSystem<LinuxSystem>::sleep(1000);
                return;
            }
            CMsgCell* cell = msg->getCellFromMessage();
            if (mLogUser->onWriteByCMsg(cell) == 0)
            {
                msg->setOffDataTypeMask(0);
                msg->setOnDataTypeMask(0);
                G_TraceLog()->sysLog(7, "Would block \xC0\xCC\xB9\xC7\xB7\xCE \xB4\xD9\xBD\xC3 \xC5\xA5\xBF\xA1 \xB3\xD6\xB4\xC2\xB4\xD9. size=%d", cell->GetSize());
                mLogMessagePool.push_front(msg);
                TSystem<LinuxSystem>::sleep(1000);
            }
            else
            {
                pApp->super_DataPools.getDataPool()->destroyLogMessage(msg);
            }
        }
        else
        {
            mThreadLock.unlock();
            TSystem<LinuxSystem>::sleep(1000);
        }
    }
}

void LogSendThread::CheckLogServer()
{
    assert(false);
}

void LogSendThread::SendLogMsg(char* buf, int len)
{
    PCK_CS_POSTING_LOG pck;
    mThreadLock.lock();
    TCPUser* u = mLogUser;
    Message* msg = pApp->super_DataPools.getDataPool()->getLogMessage(u);
    mThreadLock.unlock();
    CMsgCell* pMsg = msg->getCellFromMessage();
    *pMsg << &pck;
    pMsg = msg->getCellFromMessage();
    pMsg->AttachStream(buf, len);
    pMsg->PAD();
    mThreadLock.lock();
    mLogMessagePool.push_back(msg);
    mThreadLock.unlock();
}

void LogSendThread::SetLogServerIp(char* ip)
{
    strcpy(mLogServerIp, ip);
}

char* LogSendThread::GetLogServerIp()
{
    return mLogServerIp;
}

void LogSendThread::SetLogServerPort(int port)
{
    mLogServerPort = port;
}

int LogSendThread::GetLogServerPort()
{
    return mLogServerPort;
}

} // namespace nsl
