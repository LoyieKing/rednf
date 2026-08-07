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
    Message* msg = 0;
    while (true)
    {
        while (true)
        {
            mThreadLock.lock();
            if (!mLogMessagePool.empty())
            {
                break;
            }
            mThreadLock.unlock();
            TSystem<LinuxSystem>::sleep(1000);
        }
        msg = mLogMessagePool.front();
        mLogMessagePool.pop_front();
        mThreadLock.unlock();
        bool bDisc = false;
        if (mLogUser->isAboutToDisconnect() || mLogUser->isDisconnected())
        {
            bDisc = true;
        }
        else
        {
            bDisc = false;
        }
        if (bDisc)
        {
            break;
        }
        CMsgCell* cell = msg->getCellFromMessage();
        int ret = mLogUser->onWriteByCMsg(cell);
        if (ret == 0)
        {
            msg->setOffDataTypeMask(0);
            msg->setOnDataTypeMask(0);
            int size = cell->GetSize();
            G_TraceLog()->sysLog(7, "Would block \xC0\xCC\xB9\xC7\xB7\xCE \xB4\xD9\xBD\xC3 \xC5\xA5\xBF\xA1 \xB3\xD6\xB4\xC2\xB4\xD9. size=%d", size);
            mLogMessagePool.push_front(msg);
            TSystem<LinuxSystem>::sleep(1000);
        }
        else
        {
            pApp->super_DataPools.getDataPool()->destroyLogMessage(msg);
        }
    }
    pApp->super_DataPools.getDataPool()->destroyLogMessage(msg);
    TSystem<LinuxSystem>::sleep(1000);
}

bool LogSendThread::CheckLogServer()
{
    assert(false);
    return false;
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
