#include "ChannelServiceApp.h"
#include "ScriptData.h"
#include "Script.h"
#include "GlobalInstance.h"
#include <time.h>
#include <sys/stat.h>

ChannelServiceApp::CheckThread::CheckThread()
{
}

ChannelServiceApp::CheckThread::~CheckThread()
{
}


ScriptData* G_ScriptData()
{
    return GlobalInstance<ScriptData>::inst_ptr();
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(const char* in_Str)
{
    return *putText((char*)in_Str);
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
        putValue(1);
    }
    else
    {
        putValue(0);
    }
    return *this;
}

template <class T>
TDebugTrace<T>& TDebugTrace<T>::operator<<(TDebugTrace& (*in_Pfn)(TDebugTrace&))
{
    return (*in_Pfn)(*this);
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putText(char* s)
{
    size_t sVar2 = strlen(s);
    if ((int)(mPos + sVar2) > 0x18fff)
    {
        return this;
    }
    mPos = mPos + snprintf(m_FormatBuf + mPos, sVar2 + 1, "%s", s);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putText(const char* s)
{
    size_t sVar2 = strlen(s);
    if ((int)(mPos + sVar2) > 0x18fff)
    {
        return this;
    }
    mPos = mPos + snprintf(m_FormatBuf + mPos, sVar2 + 1, "%s", s);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putValue(int n)
{
    if (mPos + 0xc > 0x18fff)
    {
        return this;
    }
    char fmt[12];
    char tmp[16];
    memset(fmt, 0, 0xc);
    sprintf(fmt, "%d", n);
    if (hexadecimal_ != false)
    {
        sprintf(tmp, "%%-%dx", strlen(fmt));
    }
    else
    {
        sprintf(tmp, "%%-%dd", strlen(fmt));
    }
    mPos = mPos + snprintf(&m_FormatBuf[mPos], 0xd, tmp, n);
    return this;
}

template <class T>
TDebugTrace<T>* TDebugTrace<T>::putValue(unsigned int n)
{
    if (mPos + 0xc > 0x18fff)
    {
        return this;
    }
    char fmt[12];
    char tmp[16];
    memset(fmt, 0, 0xc);
    sprintf(fmt, "%d", n);
    if (hexadecimal_ != false)
    {
        sprintf(tmp, "%%-%dx", strlen(fmt));
    }
    else
    {
        sprintf(tmp, "%%-%dd", strlen(fmt));
    }
    mPos = mPos + snprintf(&m_FormatBuf[mPos], 0xd, tmp, n);
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
        memset(&in_Str, 0, 0x19000);
    }
    else
    {
        in_Str.pDevice_->serialize((char*)&in_Str);
        in_Str.flush();
        in_Str.mPos = 0;
        memset(&in_Str, 0, 0x19000);
    }
    return in_Str;
}

template TDebugTrace<char>& TDebugTrace<char>::operator<<(const char*);
template TDebugTrace<char>& TDebugTrace<char>::operator<<(char*);
template TDebugTrace<char>& TDebugTrace<char>::operator<<(int);
template TDebugTrace<char>& TDebugTrace<char>::operator<<(unsigned int);
template TDebugTrace<char>& TDebugTrace<char>::operator<<(bool);
template TDebugTrace<char>& TDebugTrace<char>::operator<<(TDebugTrace<char>& (*)(TDebugTrace<char>&));
template TDebugTrace<char>* TDebugTrace<char>::putText(char*);
template TDebugTrace<char>* TDebugTrace<char>::putText(const char*);
template TDebugTrace<char>* TDebugTrace<char>::putValue(int);
template TDebugTrace<char>* TDebugTrace<char>::putValue(unsigned int);
template TDebugTrace<char>* TDebugTrace<char>::flush();
template void TDebugTrace<char>::Lock();
template void TDebugTrace<char>::Unlock();
template TDebugTrace<char>& endl<char>(TDebugTrace<char>&);
template class GlobalInstance<GLOBAL>;
template class GlobalInstance<Script>;
template class GlobalInstance<ScriptData>;

void tagPacketHeader::setCategory(int n)
{
    classification = (char)n;
}

void tagPacketHeader::setPacketID(int n)
{
    msg_no = (char)n;
}

int tagPacketHeader::getSize()
{
    return sLength;
}

void tagPacketHeader::setSize(int n)
{
    sLength = n;
}

BOOL tagPacketHeader::isVariableLength()
{
    return 1;
}

tagPacketHeader::tagPacketHeader()
{
}

void ChannelServiceApp::ServerGroup::decreseServerCount()
{
    nServerCount_ = nServerCount_ + -1;
}

template <class T, int Size, class Q>
int TMemoryPoolStatic<T, Size, Q>::getRemain()
{
    return freeq_.size();
}

tagCS_GET_GC_INFO::tagCS_GET_GC_INFO()
{
    memset(this, 0, 0xb);
    setCategory(0x7b);
    setPacketID(0xd);
    setSize(0xb);
}

tagCS_CHECK_SCRIPT_VERSION::tagCS_CHECK_SCRIPT_VERSION()
{
    memset(this, 0, 0x1b);
    setCategory(0x7b);
    setPacketID(5);
    setSize(0x1b);
}

tagCS_NOTICE_CHANNEL_SERVER::tagCS_NOTICE_CHANNEL_SERVER()
{
    memset(this, 0, 0x23);
    setCategory(0x7b);
    setPacketID(4);
    setSize(0x23);
}

void ChannelServiceApp::CheckThread::loop(void* temp)
{
    puts("Start up CheckThread");
    ChannelService* pApp = TManager<ChannelService>::getManager();
    char timeofday[9];
    pApp->getTCPThread()->lockPushRequestConnect(1, "CheckThread.cpp", 0x1d);
    TCPUser* acUser = NULL;
    do
    {
        acUser = pApp->getTCPThread()->lockPopConnectedUser();
        if (acUser == NULL)
        {
            TSystem<LinuxSystem>::sleep(1);
        }
    } while (acUser == NULL);

    // version check block
    {
        time_t tval;
        tm t;
        time(&tval);
        localtime_r(&tval, &t);
        snprintf(timeofday, 9, "%04d%02d%02d", t.tm_year + 0x76c, t.tm_mon + 1, t.tm_mday);
        tval = 0;
        char logdname[20];
        memset(logdname, 0, 0x14);
        strncpy(logdname, "channel_info", 0x14);
        struct stat st;
        int iRet = stat(logdname, &st);
        if (iRet < 0)
        {
            if (*__errno_location() != 2)
            {
                gFileLogError << "[ERROR] cant execute stat func" << endl;
                exit(1);
            }
            iRet = mkdir(logdname, 0x1c0);
            if (iRet < 0)
            {
                gFileLogError << "[ERROR] cant execute mkdir func" << endl;
                exit(1);
            }
        }
        FILE* pFile = fopen("channel_info/version", "rb+");
        if (pFile == NULL)
        {
            gFileLogError << "[ERROR] cannt open channel_script_versioin file" << endl;
            exit(1);
        }
        char buf[1024];
        memset(buf, 0, 0x400);
        int index = 0;
        char* pcVar10;
        while ((pcVar10 = fgets(buf, 0x400, pFile)) != NULL)
        {
            char* str = buf;
            char* start = str;
            char c;
            do
            {
                while (true)
                {
                    if ((*str == ' ') || (*str == '\t'))
                    {
                        str = str + 1;
                        continue;
                    }
                    break;
                }
                *start = *str;
                start = start + 1;
                c = *str;
                str = str + 1;
            } while (c != '\0');
            if ((buf[index] == '\n') || (buf[index] == '#'))
            {
                index = 0;
            }
        }
        fclose(pFile);
        printf("[[[[script version=\'%s\']]]]\n", buf);
        strncpy(G_ScriptData()->channel_script_version, buf, 0x13);
        tagCS_GET_GC_INFO pGCInfo;
        TMsgCell<128> buffer;
        buffer << &pGCInfo;
        buffer.PAD();
        acUser->onWrite2Buffer(&buffer);
    }

    while (true)
    {
        gFileLogWarn << "Check Thread Start" << endl;
        gFileLogInfo << "start check script version ~~" << endl;
        tagCS_CHECK_SCRIPT_VERSION pCCheck;
        TMsgCell<128> buffer;
        strncpy(pCCheck.channel_script_version, G_ScriptData()->channel_script_version, 0x10);
        buffer << &pCCheck;
        buffer.PAD();
        int ret = acUser->onWrite2Buffer(&buffer);
        if (ret < 0)
        {
            if ((ret == -5) || !acUser->isDisconnected())
            {
                pApp->getTCPThread()->lockPushRequestConnect(1, "CheckThread.cpp", 0xa2);
                TCPUser* nUser = NULL;
                bool bInnerLoop = true;
                while (bInnerLoop)
                {
                    gFileLogError << "ReTry connect to the Bridge Server ret=" << ret << endl;
                    nUser = pApp->getTCPThread()->lockPopConnectedUser();
                    if (nUser != NULL)
                    {
                        bInnerLoop = false;
                        break;
                    }
                    TSystem<LinuxSystem>::sleep(1);
                }
                acUser = nUser;
                gFileLogError << "ReTry Success" << endl;
            }
        }
        time_t cur_time = time(NULL);
        gFileLogCri << "Come" << endl;
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(pApp->LockChannel);
            for (int i = 0; i < 0x80; i = i + 1)
            {
                gFileLogCri << "AAAAA=" << i << ", " << pApp->Servers[i].use << endl;
                if (pApp->Servers[i].use == true)
                {
                    gFileLogCri << "BBBBB=" << pApp->Servers[i].listServerInfo_.size() << endl;
                    if (pApp->Servers[i].listServerInfo_.size() == 0)
                    {
                        pApp->Servers[i].use = false;
                        pApp->ServerGroupCount = 0;
                        for (int j = 0; j < 0x80; j = j + 1)
                        {
                            if (pApp->Servers[j].use != false)
                            {
                                pApp->ServerGroupCount = pApp->ServerGroupCount + 1;
                            }
                        }
                    }
                    gFileLogCri << "Start=" << pApp->Servers[i].listServerInfo_.size() << endl;
                    bool deleted = false;
                    std::map<int, ChannelServiceApp::tServerInfo*>::iterator it;
                    it = pApp->Servers[i].listServerInfo_.begin();
                    while (it != pApp->Servers[i].listServerInfo_.end())
                    {
                        gFileLogCri << "gc_no=" << (*it).second->gc_no << ", Cur=" << (unsigned int)cur_time
                                    << ", tic=" << (unsigned int)(*it).second->tic
                                    << ", result=" << (unsigned int)(cur_time - (*it).second->tic) << endl;
                        deleted = false;
                        if (((*it).second->use != false) && (0x28 < (int)(cur_time - (*it).second->tic)))
                        {
                            gFileLogCri << "delete gc_no=" << (*it).second->gc_no << ", tic=" << (unsigned int)(cur_time - (*it).second->tic)
                                        << ", cur=" << (unsigned int)cur_time << ", sav=" << (unsigned int)(*it).second->tic << endl;
                            for (int k = 0; k < 0x1000; k = k + 1)
                            {
                                if (pApp->Servers[i].ServerInfo[k].gc_no == (*it).second->gc_no)
                                {
                                    pApp->Servers[i].ServerInfo[k].use = false;
                                    pApp->Servers[i].ServerInfo[k].gc_no = -1;
                                }
                            }
                            pApp->Servers[i].listServerInfo_.erase(it);
                            pApp->Servers[i].decreseServerCount();
                            it = pApp->Servers[i].listServerInfo_.begin();
                            deleted = true;
                        }
                        if (deleted != true)
                        {
                            it++;
                        }
                    }
                    gFileLogCri << "End" << endl;
                    gFileLogCri << "ABCD *************************************************************\n"
                                << "ABCD * " << i << " * " << pApp->Servers[i].listServerInfo_.size() << "\n"
                                << "ABCD *************************************************************" << endl;
                }
            }
        }
        time_t tval;
        tm t;
        time(&tval);
        localtime_r(&tval, &t);
        char timeofcurday[9];
        snprintf(timeofcurday, 9, "%04d%02d%02d", t.tm_year + 0x76c, t.tm_mon + 1, t.tm_mday);
        pCCheck.classification = 0;
        if (strncmp(timeofday, timeofcurday, 8) != 0)
        {
            pApp->createEncKey();
            strcpy(timeofday, timeofcurday);
        }
        tagCS_NOTICE_CHANNEL_SERVER pck;
        TMsgCell<128> buffer2;
        pck.id = G_ScriptData()->id;
        strcpy(pck.server_ip, G_ScriptData()->ip);
        pck.port = (int)G_ScriptData()->udp_port;
        buffer2 << &pck;
        buffer2.PAD();
        ret = acUser->onWrite2Buffer(&buffer2);
        gFileLogInfo << "notice channel info send" << endl;
        if (ret < 0)
        {
            gFileLogInfo << "Notice Send Fail=" << ret << endl;
        }
        gFileLogInfo << "ret=" << ret << ", NOTICE id=" << pck.id << ", server_ip=" << pck.server_ip << ", port=" << pck.port << endl;
        usleep(20000000);
        gFileLogInfo << "Current Remain TCP Sockets =" << pApp->m_poolTCPSocket.getRemain() << endl;
        gFileLogInfo << "Current Remain TCP Users =" << pApp->m_poolTCPUser.getRemain() << endl;
        gFileLogWarn << "Check Thread end" << endl;
    }
}

CMsgCell::CMsgCell()
{
    m_wSize = 0;
    m_wPos = 0;
    m_nRefCount = 0;
}

CMsgCell::~CMsgCell()
{
}

CMsgCell& CMsgCell::operator<<(tagPacketHeader* pPacket)
{
    if ((m_wSize == 0) && (m_wPos == 0))
    {
        m_wSize = pPacket->getSize();
        memcpy(&m_bBuf[m_wPos], pPacket, pPacket->getSize());
        m_wPos = pPacket->getSize();
    }
    else
    {
        memcpy(&m_bBuf[m_wPos], pPacket, pPacket->getSize());
        m_wPos = m_wPos + pPacket->getSize();
    }
    return *this;
}

BOOL CMsgCell::PAD()
{
    tagPacketHeader* pPCK = (tagPacketHeader*)m_bBuf;
    int nSize;
    if (pPCK->isVariableLength() == 0)
    {
        nSize = pPCK->getSize();
    }
    else
    {
        nSize = m_wPos;
    }
    pPCK->setSize(nSize);
    if (pPCK->isVariableLength() == 0)
    {
        m_wSize = pPCK->getSize();
    }
    else
    {
        m_wSize = m_wPos;
    }
    nSize = pPCK->getSize();
    return (unsigned int)(m_nBufLen < nSize);
}

template class TMsgCell<128>;

extern "C" int __xstat(int ver, const char* path, struct stat* buf);

extern "C" __attribute__((weak)) int stat(const char* __path, struct stat* __statbuf)
{
    return __xstat(3, __path, __statbuf);
}
