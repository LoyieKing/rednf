#include "CheckThread.h"
#include "TCPUser.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

ChannelServiceApp::CheckThread::CheckThread()
    : Thread(), TManager<ChannelService>()
{
}

ChannelServiceApp::CheckThread::~CheckThread()
{
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(const char* in_Str)
{
    return *putText(in_Str);
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(char* in_Str)
{
    return *putText(in_Str);
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(int in_Val)
{
    return *putValue(in_Val);
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(unsigned int in_Val)
{
    return *putValue(in_Val);
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(bool b)
{
    if (b)
    {
        return *putValue(1);
    }
    else
    {
        return *putValue(0);
    }
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(TDebugTrace& (*in_Pfn)(TDebugTrace&))
{
    return (*in_Pfn)(*this);
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putText(char* s)
{
    int sVar2 = strlen(s);
    if ((int)(mPos + sVar2) > 0x19000)
    {
        return this;
    }
    mPos = mPos + snprintf(m_FormatBuf + mPos, sVar2 + 1, "%s", s);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putText(const char* s)
{
    int sVar2 = strlen(s);
    if ((int)(mPos + sVar2) > 0x19000)
    {
        return this;
    }
    mPos = mPos + snprintf(m_FormatBuf + mPos, sVar2 + 1, "%s", s);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putValue(int n)
{
    if ((int)(mPos + 0xc) > 0x19000)
    {
        return this;
    }
    char tmp[12];
    char fmt[12];
    memset(fmt, 0, 0xc);
    sprintf(fmt, "%d", n);
    if (hexadecimal_)
    {
        sprintf(tmp, "%%-%dx", strlen(fmt));
    }
    else
    {
        sprintf(tmp, "%%-%dd", strlen(fmt));
    }
    mPos = mPos + snprintf(m_FormatBuf + mPos, 0xd, tmp, n);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putValue(unsigned int n)
{
    if ((int)(mPos + 0xc) > 0x19000)
    {
        return this;
    }
    char tmp[12];
    char fmt[12];
    memset(fmt, 0, 0xc);
    sprintf(fmt, "%d", n);
    if (hexadecimal_)
    {
        sprintf(tmp, "%%-%dx", strlen(fmt));
    }
    else
    {
        sprintf(tmp, "%%-%dd", strlen(fmt));
    }
    mPos = mPos + snprintf(m_FormatBuf + mPos, 0xd, tmp, n);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::flush()
{
    pDevice_->flush();
    return this;
}

template <class T>
void TDebugTrace<T>::Lock()
{
    m_Lock.lock();
}

template <class T>
void TDebugTrace<T>::Unlock()
{
    m_Lock.unlock();
}

template <class T>
TDebugTrace<T>& endl(TDebugTrace<T>& in_Str)
{
    if (in_Str.pDevice_ == NULL)
    {
        return in_Str;
    }
    if (in_Str.mPos > 0x19000)
    {
        FILE* log = fopen("error.txt", "a+");
        if (log != NULL)
        {
            int ret = fprintf(log, "DebugTrace error %d\n", in_Str.mPos);
            fclose(log);
        }
        in_Str.mPos = 0;
        memset(in_Str.m_FormatBuf, 0, 0x19000);
        return in_Str;
    }
    else
    {
        in_Str.pDevice_->serialize(in_Str.m_FormatBuf);
        in_Str.flush();
        in_Str.mPos = 0;
        memset(in_Str.m_FormatBuf, 0, 0x19000);
    }
    return in_Str;
}

template TDebugTrace<char>& endl<char>(TDebugTrace<char>&);
template TDebugTrace<char>* TDebugTrace<char>::putText(const char*);

template <class T, int Size, class Q>
int TMemoryPoolStatic<T, Size, Q>::getRemain()
{
    return freeq_.size();
}

template int TMemoryPoolStatic<TCPSocket, 1000, std::queue<TCPSocket*, std::deque<TCPSocket*> > >::getRemain();
template int TMemoryPoolStatic<ChannelServiceApp::TCPUser, 1000, std::queue<ChannelServiceApp::TCPUser*, std::deque<ChannelServiceApp::TCPUser*> > >::getRemain();

void ChannelServiceApp::ServerGroup::decreseServerCount()
{
    nServerCount_ = nServerCount_ - 1;
}

tagCS_UPDATE_CHANNEL_INFO::tagCS_UPDATE_CHANNEL_INFO()
{
    PACKET_HEADER_SET(0x7b, 2, 0x33);  // PACKETS::CS_UPDATE_CHANNEL_INFO
}

void ChannelServiceApp::CheckThread::loop(void* temp)
{
    puts("Start up CheckThread");
    ChannelService* pApp = getManager();
    do
    {
        time_t cur_time = time(NULL);
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(pApp->LockChannel);
            for (int i = 0; i < 0x100; i++)
            {
                if (pApp->Servers[i].use != false)
                {
                    gFileLogCri.Lock();
                    gFileLogCri << "AAAAA=" << pApp->Servers[i].use << endl;
                    gFileLogCri.Unlock();
                }
                if (pApp->Servers[i].use == true)
                {
                    gFileLogCri.Lock();
                    gFileLogCri << "BBBBB=" << pApp->Servers[i].listServerInfo_.size() << ", i = " << i << endl;
                    gFileLogCri.Unlock();
                    if (pApp->Servers[i].listServerInfo_.size() == 0)
                    {
                        pApp->Servers[i].use = false;
                        pApp->ServerGroupCount = 0;
                        for (int j = 0; j < 0x100; j++)
                        {
                            if (pApp->Servers[j].use != false)
                            {
                                pApp->ServerGroupCount = pApp->ServerGroupCount + 1;
                            }
                        }
                    }
                    gFileLogCri.Lock();
                    gFileLogCri << "Start=" << pApp->Servers[i].listServerInfo_.size() << endl;
                    gFileLogCri.Unlock();
                    int count = 0;
                    std::map<int, tServerInfo*>::iterator it = pApp->Servers[i].listServerInfo_.begin();
                    while (it != pApp->Servers[i].listServerInfo_.end())
                    {
                        gFileLogCri.Lock();
                        gFileLogCri << "gc_no=" << (*it).second->gc_no << ", Cur=" << (int)cur_time
                                    << ", tic=" << (int)(*it).second->tic << ", result=" << (int)(cur_time - (*it).second->tic) << endl;
                        gFileLogCri.Unlock();
                        if (((*it).second->use != false) && (0x14 < cur_time - (*it).second->tic))
                        {
                            gFileLogCri.Lock();
                            gFileLogCri << "delete gc_no=" << (*it).second->gc_no << ", tic="
                                        << (int)(cur_time - (*it).second->tic) << endl;
                            gFileLogCri.Unlock();
                            for (int k = 0; k < 0x1000; k++)
                            {
                                if (pApp->Servers[i].ServerInfo[k].gc_no == (*it).second->gc_no)
                                {
                                    pApp->Servers[i].ServerInfo[k].use = false;
                                    pApp->Servers[i].ServerInfo[k].gc_no = -1;
                                }
                            }
                            pApp->Servers[i].listServerInfo_.erase(it);
                            pApp->Servers[i].decreseServerCount();
                        }
                        it++;
                        count = count + 1;
                    }
                    gFileLogCri.Lock();
                    gFileLogCri << "End" << endl;
                    gFileLogCri.Unlock();
                    GLOG(gFileLogInfo, "ABCD *************************************************************");
                    GLOG(gFileLogInfo, "ABCD * " << i << "\xbc\xad\xb9\xf6\xb1\xba, \xbc\xad\xb9\xf6 \xb0\xb3\xbc\xf6 = " << (count + 1));
                    GLOG(gFileLogInfo, "ABCD *************************************************************");
                }
            }
        }
        for (int k = 0; k < pApp->ChannelServerNumber; k++)
        {
            if ((pApp->CServers[k].id == -1) || (pApp->CServers[k].uTCP->isDisconnected()))
            {
                continue;
            }
            gFileLogCri.Lock();
            gFileLogCri << "12345 START Send Channel Index = " << pApp->CServers[k].id
                        << "tic=" << (int)(cur_time - pApp->CServers[k].tic) << endl;
            gFileLogCri.Unlock();
            if (cur_time - pApp->CServers[k].tic < 0xc9)
            {
                for (int i = 0; i < 0x100; i++)
                {
                    std::map<int, tServerInfo*>::iterator it;
                    if (pApp->Servers[i].listServerInfo_.size() != 0)
                    {
                        gFileLogInfo.Lock();
                    }
                    gFileLogInfo << i << " th Number of Channel : " << pApp->Servers[i].listServerInfo_.size() << endl;
                    gFileLogInfo.Unlock();
                    it = pApp->Servers[i].listServerInfo_.begin();
                    while (it != pApp->Servers[i].listServerInfo_.end())
                    {
                        tagCS_UPDATE_CHANNEL_INFO pck;
                        TMsgCell<64> buffer;
                        CMsgCell* pMsg = &buffer;
                        pck.gc_no = (*it).second->gc_no;
                        pck.group = 0;
                        pck.channel_no = (*it).second->channel_no;
                        pck.max_user_num = (*it).second->nMaxUserCount_;
                        pck.cur_user_num = (*it).second->nCurrentUserCount_;
                        strcpy(pck.server_ip, (*it).second->IP);
                        pck.port = (*it).second->port;
                        *pMsg << (LPPACKET_HEADER)&pck;
                        pMsg->PAD();
                        GLOG(gFileLogInfo, "------------------------------------------------------");
                        GLOG(gFileLogInfo, "INDEX=" << k);
                        GLOG(gFileLogInfo, "TCP USER=" << (int)pApp->CServers[k].uTCP);
                        GLOG(gFileLogInfo, "TCP SOCK=" << (int)pApp->CServers[k].uTCP->getSocket());
                        GLOG(gFileLogInfo, "------------------------------------------------------");
                        int ret = pApp->CServers[k].uTCP->onWrite2Buffer(pMsg);
                        if (ret < 0)
                        {
                            GLOG(gFileLogInfo, "Update Send Fail=" << ret << ", error=" << strerror(errno));
                            printf("Update Send Fail");
                        }
                        if (ret == 0)
                        {
                            GLOG(gFileLogInfo, "ret = 0");
                        }
                        GLOG(gFileLogInfo, "\xc3\xa4\xb3\xce \xbc\xad\xb9\xf6\xbf\xa1 \xbe\xcb\xb8\xb0\xb4\xd9. "
                            << "gc_no=" << pck.gc_no << ", IP=" << pApp->CServers[k].IP
                            << ", PORT=" << pApp->CServers[k].port);
                        it++;
                    }
                }
                gFileLogCri.Lock();
                gFileLogCri << "12345 END Send Channel Index = " << pApp->CServers[k].id << endl;
                gFileLogCri.Unlock();
            }
            else
            {
                TScopedLock<TThreadLock<ThreadLock_linux> > slock(pApp->LockChannel);
                pApp->CServers[k].id = -1;
                pApp->ChannelServerNumber = pApp->ChannelServerNumber + -1;
                pApp->CServers[k].uTCP = NULL;
            }
        }
        gFileLogCri.Lock();
        gFileLogCri << "12345 LOOP END " << endl;
        gFileLogCri.Unlock();
        usleep(3000000);
    } while (true);
}
