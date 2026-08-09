#include "ChannelService.h"
#include "ChannelServiceApp.h"
#include "Socket.h"
#include "Rijndael.h"
#include "ScriptData.h"
#include "GlobalInstance.h"
#include <time.h>

BOOL CMsgCell::PAD()
{
    int nSize;
    tagPacketHeader* pPCK = (tagPacketHeader*)m_bBuf;
    pPCK->setSize(pPCK->isVariableLength() ? m_wPos : pPCK->getSize());
    switch (pPCK->isVariableLength())
    {
    default:
        m_wSize = m_wPos;
        break;
    case 0:
        m_wSize = pPCK->getSize();
        break;
    }
    return (unsigned int)(pPCK->getSize() > m_nBufLen);
}

bool ChannelServiceApp::bReadyToChangeScript = false;

void ITextOutputDevice::get_time(char* todaystr, char* timestr)
{
    time_t tval;
    tm t;
    time(&tval);
    localtime_r(&tval, &t);
    if (todaystr != NULL)
    {
        sprintf(todaystr, "%04d%02d%02d", t.tm_year + 0x76c, t.tm_mon + 1, t.tm_mday);
    }
    if (timestr != NULL)
    {
        sprintf(timestr, "%02d%02d%02d", t.tm_hour, t.tm_min, t.tm_sec);
    }
}

ChannelServiceApp::Protocols::Protocols()
{
}

DWORD ChannelServiceApp::Protocols::registerProtocols()
{
    return 0;
}

DWORD ChannelServiceApp::Protocols::registerProtocolsExtra()
{
    return 0;
}

ChannelServiceApp::PortInfo::PortInfo()
{
    nTCPPort_ = 0;
    nUDPPort_ = 0;
}

ChannelServiceApp::PortInfo::~PortInfo()
{
}

ChannelServiceApp::Handlers::Handlers()
{
    handlerUDPRelay_ = NULL;
    handlerTCPRelay_ = NULL;
}

ChannelServiceApp::Threads::Threads()
{
    threadTCPAccept_ = NULL;
    threadTCP_ = NULL;
    threadUDP_ = NULL;
    threadCheck_ = NULL;
}

ChannelServiceApp::Threads::~Threads()
{
}

template <class T, int Size, class Q>
TMemoryPoolStatic<T, Size, Q>::TMemoryPoolStatic()
{
    repository_ = NULL;
}

template <class T, int Size, class Q>
TMemoryPoolStatic<T, Size, Q>::~TMemoryPoolStatic()
{
    if (repository_ != NULL)
    {
        delete[] repository_;
    }
}

template <class T, int Size, class Q>
void TMemoryPoolStatic<T, Size, Q>::startup()
{
    if (repository_ != NULL)
    {
        puts("repository_ not NULL");
        throw Exception("repository_ not NULL");
    }
    try
    {
        repository_ = new T[Size];
    }
    catch (...)
    {
        puts("momory alloc failed");
        throw Exception("memory alloc failed");
    }
    printf("%d th allocated Success\n", Size);
    int i;
    for (i = 0; i < Size; i++)
    {
        freeq_.push(repository_ + i);
    }
}

template <class T, int Size, class Q>
void TMemoryPoolStatic<T, Size, Q>::shutdown()
{
    if (repository_ != NULL)
    {
        delete[] repository_;
        repository_ = NULL;
    }
}

template <class T, int Size, class Q>
T* TMemoryPoolStatic<T, Size, Q>::alloc()
{
    if (freeq_.empty())
    {
        return NULL;
    }
    T* t = freeq_.front();
    freeq_.pop();
    if (t == NULL)
    {
        throw Exception("impossible");
    }
    return t;
}

ChannelServiceApp::UserPools::UserPools()
{
}

ChannelServiceApp::UserPools::~UserPools()
{
}

TCPSocket* ChannelServiceApp::UserPools::createTCPSocket(char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPSocket);
    TCPSocket* r = poolTCPSockets_.alloc();
    if (r == NULL)
    {
        gFileLogCri.Lock();
    }
    gFileLogCri << "poolTCPSockets_ is empty : " << poolTCPSockets_.getRemain() << " " << file << ", " << line << endl;
    gFileLogCri.Unlock();
    return r;
}

void ChannelServiceApp::UserPools::destroyTCPSocket(TCPSocket* pTCPSocket)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPSocket);
    poolTCPSockets_.free(pTCPSocket);
}

ChannelServiceApp::TCPUser* ChannelServiceApp::UserPools::createTCPUser(char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPUser);
    TCPUser* r = poolTCPUsers_.alloc();
    return r;
}

void ChannelServiceApp::UserPools::destroyTCPUser(TCPUser* pTCPUser, char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPUser);
    GLOG(gFileLogInfo, "call destroyTCPUser from " << file << ", " << line);
    TCPSocket* s = pTCPUser->getSocket();
    if (s != NULL)
    {
        TReactor<EpollReactor<TCPUser>, TCPUser>* r = pTCPUser->getManager()->getReactor();
        r->unregistHandle(pTCPUser);
        s->close();
        destroyTCPSocket(s);
    }
    pTCPUser->setSocket(NULL);
    poolTCPUsers_.free(pTCPUser);
}

ChannelServiceApp::ServerGroup::ServerGroup()
{
    nServerCount_ = 0;
    use = false;
    for (int i = 0; i <= 0xfff; i = i + 1)
    {
        ServerInfo[i].use = false;
        ServerInfo[i].gc_no = -1;
        ServerInfo[i].tic = -1;
    }
}

ChannelServiceApp::ServerGroup::~ServerGroup()
{
}

ChannelServiceApp::Channel::Channel()
{
    ServerGroupCount = 0;
}

ChannelServiceApp::Channel::~Channel()
{
}

ChannelServiceApp::ChannelScript::ChannelScript()
{
    ReloadScript();
}

ChannelServiceApp::ChannelScript::~ChannelScript()
{
}

void ChannelServiceApp::ChannelScript::ReloadScript()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockFile);
    fp = fopen("channel_info/channel_info.etc", "rb");
    if (fp == NULL)
    {
        puts("[ERROR] Script Load : File Open fail");
    }
    else
    {
        fseek(fp, 0, 2);
        lSize = ftell(fp);
        rewind(fp);
        memset(buffer, 0, 0x100000);
        fread(buffer, lSize, 1, fp);
        buffer[lSize] = '\0';
        if ((buffer[lSize - 1] == '\n') && (buffer[lSize - 2] == '\r'))
        {
            buffer[lSize - 2] = '\0';
            buffer[lSize - 1] = '\0';
        }
        else if (buffer[lSize - 1] == '\n')
        {
            buffer[lSize - 1] = '\0';
        }
        fclose(fp);
    }
}

// ORIG DWARF：返回 unsigned int。
unsigned int ChannelServiceApp::ChannelScript::getScriptFileSize()
{
    return lSize;
}

ChannelServiceApp::EncyptTool::EncyptTool()
{
    memset(EncKey, 0, 0x20);
    pcryptRijndael = new CRijndael;
    EncKey[strlen(EncKey)] = '\0';
    pcryptRijndael->Initialize(EncKey, 0x20, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    createEncKey();
}

ChannelServiceApp::EncyptTool::~EncyptTool()
{
}

void ChannelServiceApp::EncyptTool::createEncKey()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockEnc);
    puts("========== Key Change =========");
    time_t tval;
    tm t;
    time(&tval);
    localtime_r(&tval, &t);
    sprintf(EncKey, "%04d%02d%02d", t.tm_year + 0x76c, t.tm_mon + 1, t.tm_mday);
    sprintf(EncKey + 8, "%02d%02d%02d", t.tm_hour, t.tm_min, t.tm_sec);
    EncKey[0xe] = '\0';
    pcryptRijndael->Initialize(EncKey, 0x20, "", 0x10, 0x10, 0, 0);
}

char* ChannelServiceApp::EncyptTool::getEncKey()
{
    return EncKey;
}

CRijndael* ChannelServiceApp::EncyptTool::getEncInc()
{
    return pcryptRijndael;
}

int ChannelServiceApp::EncyptTool::wrapEncrypt(char* pIn, int nInSize, char* pOut)
{
    int iBlockSize = pcryptRijndael->GetBlockSize();
    int nEncLen = 0;
    if (nInSize % iBlockSize != 0)
    {
        nEncLen = (nInSize / iBlockSize + 1) * iBlockSize;
    }
    else
    {
        nEncLen = nInSize;
    }
    pcryptRijndael->Pad(pIn, nInSize);
    pcryptRijndael->Encrypt(pIn, pOut, nEncLen);
    pOut[nEncLen] = '\0';
    return nEncLen;
}

void ChannelServiceApp::ServerGroup::increseServerCount()
{
    nServerCount_ = nServerCount_ + 1;
}

char* ChannelServiceApp::ChannelScript::getScriptFromFile()
{
    return buffer;
}

void ChannelServiceApp::UDPThread::setHandler(UDPHandler* in_pHandler)
{
    pHandler_ = in_pHandler;
}

void ChannelServiceApp::UDPThread::setPort(int nPort)
{
    nPort_ = nPort;
}

template <class T>
CMsgCell& CMsgCell::operator<<(const TSerializer<T>& obj)
{
    memcpy(&m_bBuf[m_wPos], obj.getBytePtr(), sizeof(T));
    m_wPos = m_wPos + sizeof(T);
    return *this;
}

template CMsgCell& CMsgCell::operator<<(const TSerializer<tpServerInfo>&);
template CMsgCell& CMsgCell::operator<<(const TSerializer<tServerGroupInfo>&);
template TSerializer<tpServerInfo>::TSerializer(const tpServerInfo&);
template TSerializer<tServerGroupInfo>::TSerializer(const tServerGroupInfo&);
template class TMsgCell<358400>;
template class TMsgCell<389120>;
template class TMsgCell<409600>;

template <>
void TGlobalInstance<TextOutputDevice_stdout>::create()
{
    if (m_p == 0 && m_p == 0)
    {
        try
        {
            register void* pvMem = operator new(sizeof(TextOutputDevice_stdout));
            memset(pvMem, 0, sizeof(TextOutputDevice_stdout));
            new (pvMem) TextOutputDevice_stdout();
            m_p = (TextOutputDevice_stdout*)pvMem;
        }
        catch (...)
        {
            printf("cannot allocate memory in TGlobalInstance.! cannot continue\n");
            exit(-1);
        }
    }
}

ChannelServiceApp::ChannelService::ChannelService()
{
    Mode_ = UDP2TCP;
    registerProtocols();
    registerProtocolsExtra();
}

ChannelServiceApp::ChannelService::ChannelService(char* service_identify)
{
    Mode_ = UDP2TCP;
    strncpy(serviceName, service_identify, 0x14);
    registerProtocols();
    registerProtocolsExtra();
}

ChannelServiceApp::ChannelService::~ChannelService()
{
}

DWORD ChannelServiceApp::ChannelService::registerProtocols()
{
    for (int i = 0; i < 0x200; i = i + 1)
    {
        m_pfnMsg[i] = NULL;
    }
    WORD wIndex = 3;
    m_pfnMsg[wIndex] = &ChannelService::onSC_ASK_CHANNEL_INFO;
    wIndex = 1;
    m_pfnMsg[wIndex] = &ChannelService::onCS_ASK_CHANNEL_INFO;
    wIndex = 5;
    m_pfnMsg[wIndex] = &ChannelService::onCS_CHECK_SCRIPT_VERSION;
    wIndex = 9;
    m_pfnMsg[wIndex] = &ChannelService::onCS_GET_SCRIPT;
    wIndex = 0xb;
    m_pfnMsg[wIndex] = &ChannelService::onCS_CONNECT;
    wIndex = 0xe;
    m_pfnMsg[wIndex] = &ChannelService::onSC_GET_GC_INFO;
    wIndex = 6;
    m_pfnMsg[wIndex] = &ChannelService::onSC_CHECK_SCRIPT_VERSION;
    wIndex = 10;
    m_pfnMsg[wIndex] = &ChannelService::onSC_GET_SCRIPT;
    wIndex = 2;
    m_pfnMsg[wIndex] = &ChannelService::onCS_UPDATE_CHANNEL_INFO;
    return 0;
}

DWORD ChannelServiceApp::ChannelService::registerProtocolsExtra()
{
    for (int i = 0; i < 0x200; i = i + 1)
    {
        m_pfnMsgExtra[i] = NULL;
    }
    return 0;
}

DWORD ChannelServiceApp::ChannelService::onSC_ASK_CHANNEL_INFO(LPPACKET_HEADER pPCK, TCPUser* u)
{
    return 1;
}

CHANNEL_HANDLER_BEGIN(SC_GET_SCRIPT)
    LPPACKET_HEADER pSGet = pPCK;
    if (bReadyToChangeScript != false)
    {
        FILE* pFile = fopen("channel_info/channel_info.etc", "w+");
        if (pFile == NULL)
        {
            GLOG(gFileLogInfo, "[ERROR] : cannt open channel_script_version file");
        }
        int ret = fwrite(pSGet + 1, 1, pSGet->getSize() - 0xb, pFile);
        fflush(pFile);
        fclose(pFile);
        ReloadScript();
        bReadyToChangeScript = false;
    }
CHANNEL_HANDLER_END()

CHANNEL_HANDLER_BEGIN(SC_CHECK_SCRIPT_VERSION)
    tagSC_CHECK_SCRIPT_VERSION* pSCheck = (tagSC_CHECK_SCRIPT_VERSION*)pPCK;
    if (pSCheck->is_valid_version == 0)
    {
        char tmpversion[16];
        memset(tmpversion, 0, 0x10);
        strcpy(tmpversion, pSCheck->channel_script_version);
        strncpy(G_ScriptData()->channel_script_version, tmpversion, 0x10);
        FILE* pFile = fopen("channel_info/version", "wb+");
        if (pFile == NULL)
        {
            GLOG(gFileLogInfo, "[ERROR] : cannt open channel_script_version file");
        }
        fputs(tmpversion, pFile);
        fclose(pFile);
        tagCS_GET_SCRIPT pCGet;
        TMsgCell<512> buffer;
        CMsgCell* pMsg = &buffer;
        *pMsg << &pCGet;
        pMsg->PAD();
        bReadyToChangeScript = true;
        u->onWrite2Buffer(pMsg);
    }
CHANNEL_HANDLER_END()

CHANNEL_HANDLER_BEGIN(CS_CONNECT)
    LPPACKET_HEADER _pPCK = pPCK;
    tagSC_CONNECT pck;
    CMsgCell* pMsg = GetMessageBuffer(0x2f);
    char test[10] = "dnf_game";
    memcpy(test, "dnf_game", 9);
    char decryptresult[32];
    memset(decryptresult, 0, 0x20);
    GLOG(gFileLogInfo, "Key " << getEncKey());
    getEncInc()->Decrypt((const char*)(_pPCK + 1), decryptresult, (size_t)(_pPCK->getSize() - 0xb));
    if (strncmp(test, decryptresult, strlen(test)) != 0)
    {
        pck.is_valid_key = 0;
        strncpy(pck.key, getEncKey(), 0x20);
    }
    else
    {
        pck.is_valid_key = 1;
    }
    pck.setAckOk();
    *pMsg << &pck;
    pMsg->PAD();
    u->onWrite2Buffer(pMsg);
    delete pMsg;
    return 1;
}

CHANNEL_HANDLER_BEGIN(CS_GET_SCRIPT)
    tagSC_GET_SCRIPT pck;
    char* script = getScriptFromFile();
    script[getScriptFileSize()] = '\0';
    TMsgCell<409600> buffer;
    TMsgCell<409600> encbuffer;
    TMsgCell<1048576> tmpbuffer;
    TMsgCell<409600> zipbuffer;
    CMsgCell* pMsg = &buffer;
    CMsgCell* encMsg = &encbuffer;
    CMsgCell* zipMsg = &zipbuffer;
    pck.setAckOk();
    *pMsg << &pck;
    *encMsg << &pck;
    *zipMsg << &pck;
    int len = strlen(script);
    GLOG(gFileLogInfo, "Script File Size = " << getScriptFileSize());
    GLOG(gFileLogInfo, "Script File len = " << len);
    if ((int)getScriptFileSize() != len)
    {
        if ((int)getScriptFileSize() > len)
        {
            int remain = (int)getScriptFileSize() - len;
            for (int i = 0; i < remain; i = i + 1)
            {
                script[len + i + 1] = ' ';
                if (len + i + 1U == (unsigned int)getScriptFileSize())
                {
                    script[len + i + 1] = '\0';
                }
            }
        }
    }
    pMsg->AttachStream(script, getScriptFileSize());
    pMsg->PAD();
    int enc_len = wrapEncrypt(pMsg->GetBuf() + 0xb, pMsg->GetSize() - 0xb, (char*)&tmpbuffer);
    encMsg->AttachStream((char*)&tmpbuffer, enc_len);
    encMsg->PAD();
    unsigned int CompressLen = enc_len + 0xd;
    compress2((unsigned char*)&tmpbuffer, (unsigned long*)&CompressLen, (unsigned char*)(encMsg->GetBuf() + 0xb), enc_len, -1);
    zipMsg->AttachStream((char*)&tmpbuffer, CompressLen);
    zipMsg->PAD();
    u->onWrite2Buffer(zipMsg);
CHANNEL_HANDLER_END()

DWORD ChannelServiceApp::ChannelService::onSC_GET_GC_INFO(LPPACKET_HEADER pPCK, TCPUser* u)
{
    tagSC_GET_GC_INFO* pSGCInfo = (tagSC_GET_GC_INFO*)pPCK;
    int offs = 0xf;
    int blocklen = 0x18;
    printf("Server Group Count = \'%d\'\n", pSGCInfo->count);
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockChannel);
    for (int i = 0; i < pSGCInfo->count; i = i + 1)
    {
        tServerGcInfo* pGcInfo = (tServerGcInfo*)malloc(0x18);
        memcpy(pGcInfo, (char*)pSGCInfo + offs, blocklen);
        gc_map[(char*)pGcInfo] = pGcInfo->gc_no;
        offs = offs + blocklen;
    }
    isReadyToStart = true;
    return 1;
}

CHANNEL_HANDLER_BEGIN(CS_ASK_CHANNEL_INFO)
    tagSC_ASK_CHANNEL_INFO pck;
    pck.setAckOk();
    TMsgCell<131072> buffer;
    TMsgCell<131072> encbuffer;
    TMsgCell<131072> tmpbuffer;
    TMsgCell<131072> zipbuffer;
    CMsgCell* pMsg = &buffer;
    CMsgCell* encMsg = &encbuffer;
    CMsgCell* zipMsg = &zipbuffer;
    pck.server_group_count = ServerGroupCount;
    *pMsg << &pck;
    int CurrentConnectedUserForGroup = 0;
    int TotalConnectedUser = 0;
    GLOG(gFileLogInfo, "*****************************************************************************************");
    GLOG(gFileLogInfo, pck.server_group_count << "\xb0\xb3 ServerGroupNum");
    for (int i = 0; i < 0x80; i = i + 1)
    {
        CurrentConnectedUserForGroup = 0;
        tServerGroupInfo Ginfo;
        TSerializer<tServerGroupInfo> Ginfo_ser(Ginfo);
        Ginfo.server_count = Servers[i].getServerCount();
        if (Ginfo.server_count != 0)
        {
            GLOG(gFileLogInfo, "*****************************************************************************************");
            GLOG(gFileLogInfo, i << "th SG");
            GLOG(gFileLogInfo, "channel count=" << Ginfo.server_count);
            strncpy(Ginfo.server_group_name, Servers[i].ServerName, 0x14);
            *pMsg << Ginfo_ser;
            GLOG(gFileLogInfo, Ginfo.server_group_name);
            for (std::map<int, tServerInfo*>::iterator it = Servers[i].listServerInfo_.begin(); it != Servers[i].listServerInfo_.end(); it++)
            {
                tpServerInfo info;
                TSerializer<tpServerInfo> Sinfo(info);
                strcpy(info.channel_name, it->second->ChannelName);
                info.max_user_num = it->second->nMaxUserCount_;
                info.cur_user_num = it->second->nCurrentUserCount_;
                strcpy(info.server_ip, it->second->IP);
                info.port = it->second->port;
                GLOG(gFileLogInfo, info.channel_name);
                GLOG(gFileLogInfo, "IP    " << it->second->IP);
                GLOG(gFileLogInfo, "POPT  " << it->second->port);
                GLOG(gFileLogInfo, "MAX  " << it->second->nMaxUserCount_);
                GLOG(gFileLogInfo, "CUR  " << it->second->nCurrentUserCount_);
                CurrentConnectedUserForGroup = CurrentConnectedUserForGroup + it->second->nCurrentUserCount_;
                *pMsg << Sinfo;
            }
            GLOG(gFileLogInfo, " CurrentConnectedUserForGroup = " << CurrentConnectedUserForGroup);
            TotalConnectedUser = TotalConnectedUser + CurrentConnectedUserForGroup;
        }
    }
    GLOG(gFileLogInfo, " TotalConnectedUser = " << TotalConnectedUser);
    GLOG(gFileLogInfo, "*****************************************************************************************");
    pMsg->PAD();
    int enc_len = wrapEncrypt(pMsg->GetBuf() + 0xb, pMsg->GetSize() - 0xb, (char*)&tmpbuffer);
    encMsg->AttachStream(pMsg->GetBuf(), 0xb);
    encMsg->AttachStream((char*)&tmpbuffer, enc_len);
    encMsg->PAD();
    zipMsg->AttachStream(pMsg->GetBuf(), 0xb);
    unsigned int CompressLen = enc_len + 0xd;
    compress2((unsigned char*)&tmpbuffer, (unsigned long*)&CompressLen, (unsigned char*)(encMsg->GetBuf() + 0xb), enc_len, 1);
    zipMsg->AttachStream((char*)&tmpbuffer, CompressLen);
    zipMsg->PAD();
    u->onWrite2Buffer(zipMsg);
CHANNEL_HANDLER_END()

CHANNEL_HANDLER_BEGIN(CS_UPDATE_CHANNEL_INFO)
    int ServerGroupIndex = -1;
    int count = 0;
    LPPACKET_HEADER _pPCK = pPCK;
    int gc_no = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->gc_no;
    gFileLogInfo.Lock();
    unsigned int uVar3 = gc_no;
    gFileLogInfo << "ABCD update gc_no=" << uVar3 << endl;
    gFileLogInfo.Unlock();
    if (isReadyToStart != false)
    {
        if (gc_no < 1)
        {
            gFileLogError.Lock();
            unsigned int uErr = gc_no;
            gFileLogError << "Incomming invalid gc_no=" << uErr << endl;
            gFileLogError.Unlock();
            return 0;
        }
        std::map<char*, int>::iterator iter;
        for (iter = gc_map.begin(); iter != gc_map.end(); iter++)
        {
            bool bMatch = false;
            if ((iter->second == gc_no || iter->second - gc_no < 0) || 999 < iter->second - gc_no)
            {
                bMatch = false;
            }
            else
            {
                bMatch = true;
            }
            if (bMatch)
            {
                ServerGroupIndex = count;
                strncpy(Servers[ServerGroupIndex].ServerName, iter->first, 0x14);
                break;
            }
            count = count + 1;
        }
        if (ServerGroupIndex == -1)
        {
            gFileLogError.Lock();
            gFileLogError << "Invalid ServerGroupIndex" << endl;
            gFileLogError.Unlock();
            return 0;
        }
        if (Servers[ServerGroupIndex].use != true)
        {
            Servers[ServerGroupIndex].use = true;
        }
        ServerGroupCount = 0;
        for (int j = 0; j < 0x80; j = j + 1)
        {
            if (Servers[j].use != false)
            {
                ServerGroupCount = ServerGroupCount + 1;
            }
        }
        int k;
        for (k = 0; (k < 0x1000) && (Servers[ServerGroupIndex].ServerInfo[k].gc_no != gc_no); k = k + 1)
        {
        }
        GLOG(gFileLogInfo, "update ?" << k);
        gFileLogInfo.Lock();
        unsigned int uCnt = gc_no;
        int iSgc = ServerGroupCount;
        gFileLogInfo << "ServerGroupCount=" << iSgc << ", ServerGroupIndex=" << ServerGroupIndex << ", gc_no=" << uCnt << endl;
        gFileLogInfo.Unlock();
        if (k == 0x1000)
        {
            for (k = 0; (k < 0x1000) && (Servers[ServerGroupIndex].ServerInfo[k].use == true); k = k + 1)
            {
            }
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockChannel);
            Servers[ServerGroupIndex].ServerInfo[k].nMaxUserCount_ = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->max_user_num;
            Servers[ServerGroupIndex].ServerInfo[k].nCurrentUserCount_ = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->cur_user_num;
            Servers[ServerGroupIndex].ServerInfo[k].port = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->port;
            Servers[ServerGroupIndex].ServerInfo[k].gc_no = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->gc_no;
            strcpy(Servers[ServerGroupIndex].ServerInfo[k].IP, ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->server_ip);
            Servers[ServerGroupIndex].ServerInfo[k].channel_no = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->channel_no;
            sprintf(Servers[ServerGroupIndex].ServerInfo[k].ChannelName, "#ch.%d", ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->channel_no);
            Servers[ServerGroupIndex].ServerInfo[k].tic = time(NULL);
            gFileLogInfo.Lock();
            unsigned int uAdd = gc_no;
            int iPort = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->port;
            char* pIp = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->server_ip;
            gFileLogInfo << "Add SGI=" << ServerGroupIndex << ", IP= " << pIp << ", PORT= " << iPort << ", gc_no=" << uAdd << endl;
            gFileLogInfo.Unlock();
            Servers[ServerGroupIndex].ServerInfo[k].use = true;
            Servers[ServerGroupIndex].listServerInfo_[gc_no] = &Servers[ServerGroupIndex].ServerInfo[k];
            Servers[ServerGroupIndex].increseServerCount();
        }
        else
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockChannel);
            Servers[ServerGroupIndex].ServerInfo[k].nMaxUserCount_ = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->max_user_num;
            Servers[ServerGroupIndex].ServerInfo[k].nCurrentUserCount_ = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->cur_user_num;
            Servers[ServerGroupIndex].ServerInfo[k].port = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->port;
            Servers[ServerGroupIndex].ServerInfo[k].gc_no = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->gc_no;
            strcpy(Servers[ServerGroupIndex].ServerInfo[k].IP, ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->server_ip);
            Servers[ServerGroupIndex].ServerInfo[k].channel_no = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->channel_no;
            sprintf(Servers[ServerGroupIndex].ServerInfo[k].ChannelName, "#ch.%d", ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->channel_no);
            Servers[ServerGroupIndex].ServerInfo[k].tic = time(NULL);
            gFileLogInfo.Lock();
            unsigned int uAdd = gc_no;
            int iPort = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->port;
            char* pIp = ((tagCS_UPDATE_CHANNEL_INFO*)_pPCK)->server_ip;
            gFileLogInfo << "Add SGI=" << ServerGroupIndex << ", IP= " << pIp << ", PORT= " << iPort << ", gc_no=" << uAdd << endl;
            gFileLogInfo.Unlock();
            Servers[ServerGroupIndex].listServerInfo_[gc_no] = &Servers[ServerGroupIndex].ServerInfo[k];
            Servers[ServerGroupIndex].ServerInfo[k].use = true;
        }
        DNF_LOG_OUT();
        return 1;
    }
    return 0;
}

CHANNEL_HANDLER_BEGIN(CS_CHECK_SCRIPT_VERSION)
    LPPACKET_HEADER _pEPCK = pPCK;
    tagCS_CHECK_SCRIPT_VERSION _DPCK;
    tagSC_CHECK_SCRIPT_VERSION pck;
    pck.setAckOk();
    getEncInc()->Decrypt((const char*)_pEPCK->_getData(), (char*)_DPCK._getData(), _pEPCK->getSize() - 0xb);
    TMsgCell<128> tmpbuffer;
    CMsgCell* pMsg = &tmpbuffer;
    TMsgCell<128> encbuffer;
    TMsgCell<128> buffer;
    CMsgCell* encMsg = &encbuffer;
    int nCmp = strcmp(G_ScriptData()->channel_script_version, _DPCK.channel_script_version);
    if (nCmp == 0)
    {
        pck.setAckOk();
        pck.is_valid_version = 1;
    }
    else
    {
        pck.setAckOk();
        pck.is_valid_version = 0;
    }
    size_t nVerLen = strlen(G_ScriptData()->channel_script_version);
    strncpy(pck.channel_script_version, G_ScriptData()->channel_script_version, nVerLen);
    nVerLen = strlen(G_ScriptData()->channel_script_version);
    pck.channel_script_version[nVerLen] = '\0';
    *pMsg << &pck;
    pMsg->PAD();
    int nPckSize = pMsg->GetSize();
    char* pPckBuf = pMsg->GetBuf();
    int enc_len = wrapEncrypt(pPckBuf + 0xb, nPckSize - 0xb, (char*)&buffer);
    pPckBuf = pMsg->GetBuf();
    encMsg->AttachStream(pPckBuf, 0xb);
    encMsg->AttachStream((char*)&buffer, enc_len);
    encMsg->PAD();
    u->onWrite2Buffer(encMsg);
    GLOG(gFileLogInfo, "Out  " << "onCS_CHECK_SCRIPT_VERSION");
    return 1;
}

void ChannelServiceApp::ChannelService::startup()
{
    puts("ChannelService is beginning..");
    createFileLogInfo();
    createFileLogWarn();
    createFileLogError();
    createFileLogCri();
    gFileLogInfo.setOutputDevice(getFileLogInfo());
    gFileLogWarn.setOutputDevice(getFileLogWarn());
    gFileLogError.setOutputDevice(getFileLogError());
    gFileLogCri.setOutputDevice(getFileLogCri());
    createLogInfo();
    createLogCri();
    createLogWarn();
    createLogError();
    gLogInfo.setOutputDevice(getLogInfo());
    gLogWarn.setOutputDevice(getLogWarn());
    gLogError.setOutputDevice(getLogError());
    gLogCri.setOutputDevice(getLogCri());
    puts("ChannelService is beginning..");
    char nameoflog[0x1e];
    memset(nameoflog, 0, 0x1e);
    sprintf(nameoflog, "./log/Info_%s.log", getServiceName());
    getFileLogInfo()->open(nameoflog);
    memset(nameoflog, 0, 0x1e);
    sprintf(nameoflog, "./log/Warn_%s.log", getServiceName());
    getFileLogWarn()->open(nameoflog);
    memset(nameoflog, 0, 0x1e);
    sprintf(nameoflog, "./log/Error_%s.log", getServiceName());
    getFileLogError()->open(nameoflog);
    memset(nameoflog, 0, 0x1e);
    sprintf(nameoflog, "./log/Cri_%s.log", getServiceName());
    getFileLogCri()->open(nameoflog);
    puts("Loading Session Pool");
    poolTCPSockets_.startup();
    poolTCPUsers_.startup();
    puts("End Loading Session Pool");
    threadTCPAccept_ = new TCPAcceptThread;
    threadTCPAccept_->TManager<ChannelService>::setManager(this);
    threadTCPAccept_->setPort(PortInfo::getTCPPort());
    threadCheck_ = new CheckThread;
    threadCheck_->TManager<ChannelService>::setManager(this);
    threadTCP_ = new TCPThread;
    threadTCP_->TManager<ChannelService>::setManager(this);
    threadUDP_ = new UDPThread;
    threadUDP_->TManager<ChannelService>::setManager(this);
    threadUDP_->setPort(PortInfo::getUDPPort());
    handlerTCPRelay_ = new TCPHandlerRelay;
    handlerTCPRelay_->TManager<ChannelService>::setManager(this);
    handlerUDPRelay_ = new UDPHandlerRelay;
    handlerUDPRelay_->TManager<ChannelService>::setManager(this);
    threadTCP_->setHandler((TCPHandler*)handlerTCPRelay_);
    threadUDP_->setHandler((UDPHandler*)handlerUDPRelay_);
    gLogInfo << "Start Thread rasing" << endl;
    threadUDP_->begin();
    gLogInfo << "The UDP Thread rasing success" << endl;
    threadTCPAccept_->begin();
    gLogInfo << "The TCPAccept Thread rasing success" << endl;
    threadCheck_->begin();
    gLogInfo << "The Check Thread rasing success" << endl;
    threadTCP_->begin();
    gLogInfo << "The TCP Thread rasing success" << endl;
    gLogInfo << "My ID is=" << G_ScriptData()->id << endl;
    gLogInfo << "----------------------------------------------------------" << endl;
    gLogInfo << "-   XX            Channel Server Start Ver1.0              -" << endl;
    gLogInfo << "----------------------------------------------------------" << endl;
}

void ChannelServiceApp::ChannelService::shutdown()
{
}

void ChannelServiceApp::createFileLogInfo()
{
    g_FileLogInfo.create();
}

void ChannelServiceApp::createFileLogWarn()
{
    g_FileLogWarn.create();
}

void ChannelServiceApp::createFileLogError()
{
    g_FileLogError.create();
}

void ChannelServiceApp::createFileLogCri()
{
    g_FileLogCri.create();
}

TextOutputDevice_FILE* ChannelServiceApp::getFileLogInfo()
{
    return g_FileLogInfo.get();
}

TextOutputDevice_FILE* ChannelServiceApp::getFileLogWarn()
{
    return g_FileLogWarn.get();
}

TextOutputDevice_FILE* ChannelServiceApp::getFileLogError()
{
    return g_FileLogError.get();
}

TextOutputDevice_FILE* ChannelServiceApp::getFileLogCri()
{
    return g_FileLogCri.get();
}

void ChannelServiceApp::createLogInfo()
{
    g_LogInfo.create();
}

void ChannelServiceApp::createLogCri()
{
    g_LogCri.create();
}

void ChannelServiceApp::createLogWarn()
{
    g_LogWarn.create();
}

void ChannelServiceApp::createLogError()
{
    g_LogError.create();
}

TextOutputDevice_stdout* ChannelServiceApp::getLogInfo()
{
    return g_LogInfo.get();
}

TextOutputDevice_stdout* ChannelServiceApp::getLogCri()
{
    return g_LogCri.get();
}

TextOutputDevice_stdout* ChannelServiceApp::getLogWarn()
{
    return g_LogWarn.get();
}

TextOutputDevice_stdout* ChannelServiceApp::getLogError()
{
    return g_LogError.get();
}

__int64 ChannelServiceApp::ChannelService::getTick() const
{
    return tick_;
}

void ChannelServiceApp::ChannelService::setTick()
{
    tick_ = get_ms_tick();
}

void ChannelServiceApp::ChannelService::setLastTickForIdleSession()
{
    lastTickForIdleSession = get_ms_tick();
}

bool ChannelServiceApp::ChannelService::isIdleCheckTime() const
{
    if (get_ms_tick() - getLastTickForIdleSession() > 0x2bf20)
    {
        return true;
    }
    return false;
}

char* ChannelServiceApp::ChannelService::getServiceName()
{
    return serviceName;
}

void tagPacketHeader::setAckOk()
{
    ack = 1;
}

void tagPacketHeader::setAckFail()
{
    ack = 0;
}

int tagPacketHeader::getCategory()
{
    return classification;
}

void tagPacketHeader::setCheckSum(unsigned int sum)
{
    check_sum = sum;
}

unsigned int tagPacketHeader::getCheckSum()
{
    return check_sum;
}

void tagPacketHeader::setVariableLength(bool bVariable)
{
    classification = (char)bVariable;
}

tagPacketHeader::operator DWORD()
{
    return (DWORD)this;
}

char* tagPacketHeader::_copyData()
{
    return (char*)this + 0xb;
}

char* tagPacketHeader::_getData()
{
    return (char*)this + 0xb;
}

char* tagPacketHeader::_getHeader()
{
    return (char*)this;
}

template <class T>
void TDebugTrace<T>::setOutputDevice(ITextOutputDevice* in_device)
{
    pDevice_ = in_device;
}

CMsgCell* GetMessageBuffer(int nSize)
{
    if (!((nSize < 0x80001) && (-1 < nSize)))
    {
        throw "GetMessageBuffer";
    }
    CMsgCell* pCell = 0;
    if (nSize < 0x11)
    {
        pCell = new TMsgCell<16>;
    }
        else if (nSize < 0x21)
        {
            pCell = new TMsgCell<32>;
        }
        else if (nSize < 0x41)
        {
            pCell = new TMsgCell<64>;
        }
        else if (nSize < 0x81)
        {
            pCell = new TMsgCell<128>;
        }
        else if (nSize < 0x101)
        {
            pCell = new TMsgCell<256>;
        }
        else if (nSize < 0x201)
        {
            pCell = new TMsgCell<512>;
        }
        else if (nSize < 0x401)
        {
            pCell = new TMsgCell<1024>;
        }
        else if (nSize < 0x801)
        {
            pCell = new TMsgCell<2048>;
        }
        else if (nSize < 0x1001)
        {
            pCell = new TMsgCell<4096>;
        }
        else if (nSize < 0x2001)
        {
            pCell = new TMsgCell<8192>;
        }
        else if (nSize < 0x4001)
        {
            pCell = new TMsgCell<16384>;
        }
        else if (nSize < 0x8001)
        {
            pCell = new TMsgCell<32768>;
        }
        else if (nSize < 0x10001)
        {
            pCell = new TMsgCell<65536>;
        }
        else if (nSize < 0x20001)
        {
            pCell = new TMsgCell<131072>;
        }
        else if (nSize < 0x40001)
        {
            pCell = new TMsgCell<262144>;
        }
        else if (nSize < 0x57801)
        {
            pCell = new TMsgCell<358400>;
        }
        else if (nSize < 0x5f001)
        {
            pCell = new TMsgCell<389120>;
        }
        else if (nSize < 0x64001)
        {
            pCell = new TMsgCell<409600>;
        }
        else if (nSize < 0x80001)
        {
            pCell = new TMsgCell<524288>;
        }
        else if (nSize < 0x100001)
        {
            pCell = new TMsgCell<1048576>;
        }
        else
        {
            pCell = new TMsgCell<409600>;
        }
    return pCell;
}

template void TDebugTrace<char>::setOutputDevice(ITextOutputDevice*);
template class TMsgCell<16>;
template class TMsgCell<32>;
template class TMsgCell<64>;
template class TMsgCell<256>;
template class TMsgCell<512>;
template class TMsgCell<1024>;
template class TMsgCell<2048>;
template class TMsgCell<4096>;
template class TMsgCell<8192>;
template class TMsgCell<16384>;
template class TMsgCell<32768>;
template class TMsgCell<65536>;
template class TMsgCell<131072>;
template class TMsgCell<262144>;
template class TMsgCell<524288>;
template class TMsgCell<1048576>;
template bool EpollReactor<ChannelServiceApp::TCPUser>::unregistHandle(ChannelServiceApp::TCPUser*);
template bool TReactor<EpollReactor<ChannelServiceApp::TCPUser>, ChannelServiceApp::TCPUser>::unregistHandle(ChannelServiceApp::TCPUser*);
template class GlobalInstance<GLOBAL>;
template class GlobalInstance<ScriptData>;

tagSC_CONNECT::tagSC_CONNECT()
{
    PACKET_CTOR_BODY(0x7c, 0xc, 0x2f);  // PACKETS::SC_CONNECT
}

tagCS_GET_SCRIPT::tagCS_GET_SCRIPT()
{
    PACKET_CTOR_BODY(0x7c, 9, 0xb);  // PACKETS::CS_GET_SCRIPT
}

tagSC_GET_SCRIPT::tagSC_GET_SCRIPT()
{
    PACKET_CTOR_BODY(0x7c, 10, 0xb);  // PACKETS::SC_GET_SCRIPT
}

tagSC_CHECK_SCRIPT_VERSION::tagSC_CHECK_SCRIPT_VERSION()
{
    PACKET_CTOR_BODY(0x7c, 6, 0x1f);  // PACKETS::SC_CHECK_SCRIPT_VERSION
}

tagSC_ASK_CHANNEL_INFO::tagSC_ASK_CHANNEL_INFO()
{
    PACKET_CTOR_BODY(0x7c, 3, 0xf);  // PACKETS::SC_ASK_CHANNEL_INFO
}

void CMsgCell::AttachStream(char* pBuf, int wSize)
{
    memcpy(&m_bBuf[m_wPos], pBuf, wSize);
    m_wPos = m_wPos + wSize;
}

template class TSerializer<tpServerInfo>;
template class TSerializer<tServerGroupInfo>;

ITextOutputDevice::ITextOutputDevice()
{
}

TextOutputDevice_FILE::TextOutputDevice_FILE()
{
    fp_ = NULL;
}

TextOutputDevice_stdout::TextOutputDevice_stdout()
{
}

bool TextOutputDevice_FILE::open(const TCHAR* s)
{
    fp_ = fopen(s, "at");
    if (fp_ == NULL)
    {
        printf("Can't open [%s] file : %s\n", s, strerror(*__errno_location()));
        return false;
    }
    strncpy(szFileName_, s, 0x104);
    printf("Success Log-File open : %s\n", szFileName_);
    return true;
}

bool TextOutputDevice_FILE::_reopen()
{
    if (fp_ != NULL)
    {
        return true;
    }
    FILE* fp = fopen(szFileName_, "at");
    if (fp == NULL)
    {
        puts("[ERROR] cant execut reopen func");
        return false;
    }
    fp_ = fp;
    return true;
}

bool TextOutputDevice_FILE::log_file_check()
{
    if (fp_ == NULL)
    {
        if (!_reopen())
        {
            return false;
        }
    }
    if (ftell(fp_) < 50000000)
    {
        return true;
    }
    memcpy(logdname, "./log/old_log", 0xe);
    if (stat(logdname, &st) < 0)
    {
        if (*__errno_location() != 2)
        {
            puts("[ERROR] cant execute stat func");
            return false;
        }
        if (mkdir(logdname, 0x1c0) < 0)
        {
            puts("[ERROR] cant execute mkdir func");
            return false;
        }
    }
    fclose(fp_);
    fp_ = NULL;
    now = time(NULL);
    tm = localtime_r(&now, &buf);
    sprintf(refname, "%s/%s", logdname, szFileName_ + 6);
    if (rename(szFileName_, refname) < 0)
    {
        puts("[ERROR] cant execut rename func");
        if (!_reopen())
        {
            return false;
        }
    }
    if (!_reopen())
    {
        return false;
    }
    return true;
}

void TextOutputDevice_FILE::serialize(char* s)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockFile);
    get_time(cur_date, cur_time);
    snprintf(out_buf, 0x19000, "%s %s : %s\n", cur_date, cur_time, s);
    if (!log_file_check())
    {
    }
    else
    {
        fputs(out_buf, fp_);
    }
}

void TextOutputDevice_FILE::flush()
{
}

void TextOutputDevice_stdout::serialize(char* s)
{
    get_time(cur_date, cur_time);
    snprintf(out_buf, 0x19000, "%s %s : %s", cur_date, cur_time, s);
    puts(out_buf);
}

void TextOutputDevice_stdout::flush()
{
}

template class TGlobalInstance<TextOutputDevice_FILE>;
template class TGlobalInstance<TextOutputDevice_stdout>;
