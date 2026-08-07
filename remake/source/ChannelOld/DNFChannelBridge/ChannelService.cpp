#include "ChannelService.h"
#include "ChannelServiceApp.h"
#include "Socket.h"
#include "TCPThread.h"
#include "ScriptThread.h"
#include "ScriptData.h"
#include "GlobalInstance.h"
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
    for (int i = 0; i <= Size - 1; i = i + 1)
    {
        T* t = repository_ + i;
        freeq_.push(t);
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
    TCPSocket* r = m_poolTCPSocket.alloc();
    gFileLogInfo.Lock();
    gFileLogInfo << "create tcp socket " << m_poolTCPSocket.getRemain() << ", from=" << file << ", " << line << endl;
    gFileLogInfo.Unlock();
    return r;
}

void ChannelServiceApp::UserPools::destroyTCPSocket(TCPSocket* pTCPSocket)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPSocket);
    m_poolTCPSocket.free(pTCPSocket);
    gFileLogInfo.Lock();
    gFileLogInfo << "destroy tcp socket " << m_poolTCPSocket.getRemain() << endl;
    gFileLogInfo.Unlock();
}

ChannelServiceApp::TCPUser* ChannelServiceApp::UserPools::createTCPUser(char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPUser);
    TCPUser* r = m_poolTCPUser.alloc();
    gFileLogInfo.Lock();
    gFileLogInfo << "create tcp user =" << m_poolTCPUser.getRemain() << ", from=" << file << ", " << line << endl;
    gFileLogInfo.Unlock();
    return r;
}

void ChannelServiceApp::UserPools::destroyTCPUser(TCPUser* pTCPUser, char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockTCPUser);
    gFileLogInfo.Lock();
    gFileLogInfo << "call destroyTCPUser from " << file << ", " << line << endl;
    gFileLogInfo.Unlock();
    TCPSocket* s = pTCPUser->getSocket();
    if (s != NULL)
    {
        TReactor<EpollReactor<TCPUser>, TCPUser>* r = pTCPUser->getManager()->getReactor();
        r->unregistHandle(pTCPUser);
        s->close();
        destroyTCPSocket(s);
    }
    pTCPUser->setSocket(NULL);
    m_poolTCPUser.free(pTCPUser);
    gFileLogInfo.Lock();
    gFileLogInfo << "destroy tcp user " << m_poolTCPUser.getRemain() << endl;
    gFileLogInfo.Unlock();
}

ChannelServiceApp::ServerGroup::ServerGroup()
{
    nServerCount_ = 0;
    use = false;
    for (int i = 0; i < 0x1000; i = i + 1)
    {
        ServerInfo[i].use = false;
        ServerInfo[i].gc_no = -1;
        ServerInfo[i].tic = -1;
    }
}

ChannelServiceApp::ServerGroup::~ServerGroup()
{
}

void ChannelServiceApp::ServerGroup::increseServerCount()
{
    nServerCount_ = nServerCount_ + 1;
}

ChannelServiceApp::Channel::Channel()
{
    ServerGroupCount = 0;
}

ChannelServiceApp::Channel::~Channel()
{
}

ChannelServiceApp::ChannelServer::ChannelServer()
{
    ChannelServerNumber = 0;
    for (int i = 0; i < 0x100; i = i + 1)
    {
        CServers[i].id = -1;
        CServers[i].uTCP = NULL;
    }
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
    fp = fopen("cfg/channel_info.etc", "rb");
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
        printf("ScriptSize = '%d'\n");
        printf("ScriptSize = '%d'\n", lSize);
        printf("ScriptSize = '%d'\n", lSize);
        printf("ScriptSize = '%d'\n", lSize);
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

int ChannelServiceApp::ChannelScript::getScriptFileSize()
{
    return lSize;
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

template CMsgCell& CMsgCell::operator<<(const TSerializer<tServerGcInfo>&);
template TSerializer<tServerGcInfo>::TSerializer(const tServerGcInfo&);
template class TMsgCell<128>;
template class TMsgCell<4096>;
template class TMsgCell<1048576>;

template <class T>
void TDebugTrace<T>::setOutputDevice(ITextOutputDevice* in_device)
{
    pDevice_ = in_device;
}

template void TDebugTrace<char>::setOutputDevice(ITextOutputDevice*);

template <>
void TGlobalInstance<TextOutputDevice_stdout>::create()
{
    if (m_p == 0 && m_p == 0)
    {
        void* pvMem = operator new(sizeof(TextOutputDevice_stdout));
        memset(pvMem, 0, sizeof(TextOutputDevice_stdout));
        new (pvMem) TextOutputDevice_stdout();
        m_p = (TextOutputDevice_stdout*)pvMem;
    }
}

template <>
void TGlobalInstance<TextOutputDevice_FILE>::create()
{
    if (m_p == 0 && m_p == 0)
    {
        m_p = new TextOutputDevice_FILE;
    }
}

ChannelServiceApp::ChannelService::ChannelService()
    : PortInfo(), Handlers(), Protocols(), Threads(), UserPools(), Channel(), ChannelServer(), Reactor(), ChannelScript()
{
    Mode_ = UDP2TCP;
    registerProtocols();
    registerProtocolsExtra();
}

ChannelServiceApp::ChannelService::ChannelService(char* service_identify)
    : PortInfo(), Handlers(), Protocols(), Threads(), UserPools(), Channel(), ChannelServer(), Reactor(), ChannelScript()
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
    WORD wIndex = 5;
    m_pfnMsg[wIndex] = &ChannelService::onCS_CHECK_SCRIPT_VERSION;
    wIndex = 9;
    m_pfnMsg[wIndex] = &ChannelService::onCS_GET_SCRIPT;
    wIndex = 0xd;
    m_pfnMsg[wIndex] = &ChannelService::onCS_GET_GC_INFO;
    wIndex = 4;
    m_pfnMsg[wIndex] = &ChannelService::onCS_NOTICE_CHANNEL_SERVER;
    return 0;
}

DWORD ChannelServiceApp::ChannelService::registerProtocolsExtra()
{
    for (int i = 0; i < 0x200; i = i + 1)
    {
        m_pfnMsgExtra[i] = NULL;
    }
    WORD wIndex = 2;
    m_pfnMsgExtra[wIndex] = &ChannelService::onCS_UPDATE_CHANNEL_INFO;
    return 0;
}

void ChannelServiceApp::ChannelService::setTick()
{
    tick_ = get_ms_tick();
}

__int64 ChannelServiceApp::ChannelService::getTick() const
{
    return tick_;
}

char* ChannelServiceApp::ChannelService::getServiceName()
{
    return serviceName;
}

SOCKET ChannelServiceApp::TCPUser::getHandle()
{
    return pSock_->getHandle();
}

void ChannelServiceApp::ChannelService::shutdown()
{
}

void ChannelServiceApp::ChannelService::startup()
{
    puts("ChannelService is begining..");
    createFileLogInfo();
    createFileLogWarn();
    createFileLogError();
    createFileLogCri();
    createLogInfo();
    createLogCri();
    createLogWarn();
    createLogError();
    gFileLogInfo.setOutputDevice(getFileLogInfo());
    gFileLogWarn.setOutputDevice(getFileLogWarn());
    gFileLogError.setOutputDevice(getFileLogError());
    gFileLogCri.setOutputDevice(getFileLogCri());
    gLogInfo.setOutputDevice(getLogInfo());
    gLogWarn.setOutputDevice(getLogWarn());
    gLogError.setOutputDevice(getLogError());
    gLogCri.setOutputDevice(getLogCri());

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

    m_poolTCPSocket.startup();
    m_poolTCPUser.startup();

    TCPAcceptThread* pAccept = new TCPAcceptThread;
    threadTCPAccept_ = pAccept;
    pAccept->setManager(this);
    pAccept->setPort(getTCPPort());

    CheckThread* pCheck = new CheckThread;
    threadCheck_ = pCheck;
    pCheck->setManager(this);

    ScriptThread* pScript = new ScriptThread;
    threadScript_ = pScript;
    pScript->setManager(this);

    TCPThread* pTCP = new TCPThread;
    threadTCP_ = pTCP;
    pTCP->setManager(this);

    UDPThread* pUDP = new UDPThread;
    threadUDP_ = pUDP;
    pUDP->setManager(this);
    pUDP->setPort(getUDPPort());

    TCPHandlerRelay* pTCPHandler = new TCPHandlerRelay;
    handlerTCPRelay_ = pTCPHandler;
    pTCPHandler->setManager(this);

    UDPHandlerRelay* pUDPHandler = new UDPHandlerRelay;
    handlerUDPRelay_ = pUDPHandler;
    pUDPHandler->setManager(this);

    threadTCP_->setHandler(handlerTCPRelay_);
    threadUDP_->setHandler(handlerUDPRelay_);

    threadScript_->begin();
    threadUDP_->begin();
    threadTCPAccept_->begin();
    threadTCP_->begin();
    TSystem<LinuxSystem>::usleep(1000000);
    threadCheck_->begin();

    gLogInfo << "----------------------------------------------------------" << endl;
    gLogInfo << "-             Channel Bridge Start Ver1.0              -" << endl;
    gLogInfo << "----------------------------------------------------------" << endl;
}

DWORD ChannelServiceApp::ChannelService::onCS_UPDATE_CHANNEL_INFO(LPPACKET_HEADER pPCK)
{
    int ServerGroupIndex = -1;
    int count = 0;
    tagCS_UPDATE_CHANNEL_INFO* _pPCK = (tagCS_UPDATE_CHANNEL_INFO*)pPCK;
    int gc_no = _pPCK->gc_no;
    if (gc_no < 1)
    {
        return 0;
    }
    for (std::map<char*, int>::iterator iter = gc_map.begin(); iter != gc_map.end(); iter++)
    {
        if ((0 < iter->second - gc_no) && (iter->second - gc_no <= 0x3e7))
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
    for (int j = 0; j < 0x100; j = j + 1)
    {
        if (Servers[j].use != false)
        {
            ServerGroupCount = ServerGroupCount + 1;
        }
    }
    int i;
    for (i = 0; (i < 0x1000) && (Servers[ServerGroupIndex].ServerInfo[i].gc_no != gc_no); i = i + 1)
    {
    }
    gFileLogInfo.Lock();
    gFileLogInfo << "update ?" << i << endl;
    gFileLogInfo.Unlock();
    if (i == 0x1000)
    {
        for (i = 0; (i < 0x1000) && (Servers[ServerGroupIndex].ServerInfo[i].use == true); i = i + 1)
        {
        }
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockChannel);
        Servers[ServerGroupIndex].ServerInfo[i].nMaxUserCount_ = _pPCK->max_user_num;
        Servers[ServerGroupIndex].ServerInfo[i].nCurrentUserCount_ = _pPCK->cur_user_num;
        Servers[ServerGroupIndex].ServerInfo[i].port = _pPCK->port;
        Servers[ServerGroupIndex].ServerInfo[i].gc_no = _pPCK->gc_no;
        strcpy(Servers[ServerGroupIndex].ServerInfo[i].IP, _pPCK->server_ip);
        Servers[ServerGroupIndex].ServerInfo[i].channel_no = _pPCK->channel_no;
        sprintf(Servers[ServerGroupIndex].ServerInfo[i].ChannelName, "#ch.%d", _pPCK->channel_no);
        Servers[ServerGroupIndex].ServerInfo[i].tic = time(NULL);
        Servers[ServerGroupIndex].ServerInfo[i].use = true;
        Servers[ServerGroupIndex].listServerInfo_[gc_no] = &Servers[ServerGroupIndex].ServerInfo[i];
        Servers[ServerGroupIndex].increseServerCount();
    }
    else
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockChannel);
        Servers[ServerGroupIndex].ServerInfo[i].nMaxUserCount_ = _pPCK->max_user_num;
        Servers[ServerGroupIndex].ServerInfo[i].nCurrentUserCount_ = _pPCK->cur_user_num;
        Servers[ServerGroupIndex].ServerInfo[i].port = _pPCK->port;
        Servers[ServerGroupIndex].ServerInfo[i].gc_no = _pPCK->gc_no;
        strcpy(Servers[ServerGroupIndex].ServerInfo[i].IP, _pPCK->server_ip);
        Servers[ServerGroupIndex].ServerInfo[i].channel_no = _pPCK->channel_no;
        sprintf(Servers[ServerGroupIndex].ServerInfo[i].ChannelName, "#ch.%d", _pPCK->channel_no);
        Servers[ServerGroupIndex].ServerInfo[i].tic = time(NULL);
    }
    return 1;
}

DWORD ChannelServiceApp::ChannelService::onCS_NOTICE_CHANNEL_SERVER(LPPACKET_HEADER pPCK, TCPUser* u)
{
    gFileLogInfo.Lock();
    gFileLogInfo << "In  " << "onCS_NOTICE_CHANNEL_SERVER" << endl;
    gFileLogInfo.Unlock();
    tagCS_NOTICE_CHANNEL_SERVER* _pPCK = (tagCS_NOTICE_CHANNEL_SERVER*)pPCK;
    gFileLogInfo.Lock();
    gFileLogInfo << "==> ChannelServerID=" << _pPCK->id << ", IP=" << _pPCK->server_ip << ", PORT=" << _pPCK->port << endl;
    gFileLogInfo.Unlock();
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockChannel);
    if (ChannelServerNumber == 0)
    {
        CServers[0].id = _pPCK->id;
        CServers[0].port = _pPCK->port;
        strcpy(CServers[0].IP, _pPCK->server_ip);
        ChannelServerNumber = ChannelServerNumber + 1;
        CServers[0].tic = time(NULL);
        CServers[0].uTCP = u;
        gFileLogInfo.Lock();
        gFileLogInfo << "***************************************************************" << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "INDEX=" << 0 << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "TCP USER=" << (int)CServers[0].uTCP << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "TCP SOCK=" << (int)CServers[0].uTCP->getSocket() << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "***************************************************************" << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "***********************************************************" << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "First incomming ChannelServer IP =" << _pPCK->server_ip << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "***********************************************************" << endl;
        gFileLogInfo.Unlock();
    }
    else
    {
        bool isFound = false;
        int i;
        for (i = 0; i < ChannelServerNumber; i = i + 1)
        {
            if (_pPCK->id == CServers[i].id)
            {
                isFound = true;
                break;
            }
        }
        if (i == 0x100)
        {
            gFileLogError.Lock();
            gFileLogError << "Invalid number of ChannelServer" << endl;
            gFileLogError.Unlock();
            return 0;
        }
        if (isFound != true)
        {
            ChannelServerNumber = ChannelServerNumber + 1;
        }
        CServers[i].uTCP = u;
        gFileLogInfo.Lock();
        gFileLogInfo << "***************************************************************" << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "INDEX=" << i << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "TCP USER=" << (int)CServers[i].uTCP << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "TCP SOCK=" << (int)CServers[i].uTCP->getSocket() << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "***************************************************************" << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "***********************************************************" << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "ChannelServer IP =" << _pPCK->server_ip << endl;
        gFileLogInfo.Unlock();
        gFileLogInfo.Lock();
        gFileLogInfo << "***********************************************************" << endl;
        gFileLogInfo.Unlock();
        CServers[i].id = _pPCK->id;
        CServers[i].port = _pPCK->port;
        strcpy(CServers[i].IP, _pPCK->server_ip);
        CServers[i].tic = time(NULL);
    }
    gFileLogInfo.Lock();
    gFileLogInfo << "Out " << "onCS_NOTICE_CHANNEL_SERVER" << endl;
    gFileLogInfo.Unlock();
    return 1;
}

DWORD ChannelServiceApp::ChannelService::onCS_CHECK_SCRIPT_VERSION(LPPACKET_HEADER pPCK, TCPUser* u)
{
    gFileLogInfo.Lock();
    gFileLogInfo << "In  " << "onCS_CHECK_SCRIPT_VERSION" << endl;
    gFileLogInfo.Unlock();
    tagCS_CHECK_SCRIPT_VERSION* _pPCK = (tagCS_CHECK_SCRIPT_VERSION*)pPCK;
    tagSC_CHECK_SCRIPT_VERSION pck;
    gFileLogInfo.Lock();
    gFileLogInfo << "                                                    " << endl;
    gFileLogInfo.Unlock();
    gFileLogInfo.Lock();
    gFileLogInfo << "script version =" << _pPCK->channel_script_version
                 << ", cur version =" << G_ScriptData()->channel_script_version << endl;
    gFileLogInfo.Unlock();
    TMsgCell<128> buffer;
    CMsgCell* pMsg = &buffer;
    size_t nLen = strlen(G_ScriptData()->channel_script_version);
    pck.is_valid_version = (strncmp(G_ScriptData()->channel_script_version, _pPCK->channel_script_version, nLen) == 0);
    nLen = strlen(G_ScriptData()->channel_script_version);
    strncpy(pck.channel_script_version, G_ScriptData()->channel_script_version, nLen);
    nLen = strlen(G_ScriptData()->channel_script_version);
    pck.channel_script_version[nLen] = '\0';
    *pMsg << &pck;
    pMsg->PAD();
    u->onWrite2Buffer(pMsg);
    gFileLogInfo.Lock();
    gFileLogInfo << "Out " << "onCS_CHECK_SCRIPT_VERSION" << endl;
    gFileLogInfo.Unlock();
    return 1;
}

DWORD ChannelServiceApp::ChannelService::onCS_GET_SCRIPT(LPPACKET_HEADER pPCK, TCPUser* u)
{
    gFileLogInfo.Lock();
    gFileLogInfo << "In  " << "onCS_GET_SCRIPT" << endl;
    gFileLogInfo.Unlock();
    tagSC_GET_SCRIPT pck;
    char* script = getScriptFromFile();
    script[getScriptFileSize()] = '\0';
    TMsgCell<1048576> buffer;
    CMsgCell* pMsg = &buffer;
    *pMsg << &pck;
    pMsg->AttachStream(script, getScriptFileSize());
    pMsg->PAD();
    int ret = u->onWrite2Buffer(pMsg);
    gFileLogInfo.Lock();
    gFileLogInfo << "SendScript Fail?=" << ret << endl;
    gFileLogInfo.Unlock();
    gFileLogInfo.Lock();
    gFileLogInfo << "Out " << "onCS_GET_SCRIPT" << endl;
    gFileLogInfo.Unlock();
    return 1;
}

DWORD ChannelServiceApp::ChannelService::onCS_GET_GC_INFO(LPPACKET_HEADER pPCK, TCPUser* u)
{
    gFileLogInfo.Lock();
    gFileLogInfo << "In  " << "onCS_GET_GC_INFO" << endl;
    gFileLogInfo.Unlock();
    tagSC_GET_GC_INFO pck;
    TMsgCell<4096> buffer;
    CMsgCell* pMsg = &buffer;
    int count = 0;
    for (std::map<char*, int>::iterator iter = gc_map.begin(); iter != gc_map.end(); iter++)
    {
        count = count + 1;
    }
    pck.count = count;
    gFileLogInfo.Lock();
    gFileLogInfo << "COUNT : " << pck.count << endl;
    gFileLogInfo.Unlock();
    *pMsg << &pck;
    for (std::map<char*, int>::iterator iter = gc_map.begin(); iter != gc_map.end(); iter++)
    {
        tServerGcInfo ServerGcInfo;
        TSerializer<tServerGcInfo> GcInfo(ServerGcInfo);
        strncpy(ServerGcInfo.server_group_name, iter->first, 0x14);
        ServerGcInfo.gc_no = iter->second;
        gFileLogInfo.Lock();
        gFileLogInfo << ServerGcInfo.server_group_name << ", " << ServerGcInfo.gc_no << endl;
        gFileLogInfo.Unlock();
        *pMsg << GcInfo;
    }
    pMsg->PAD();
    u->onWrite2Buffer(pMsg);
    gFileLogInfo.Lock();
    gFileLogInfo << "Out " << "onCS_GET_GC_INFO" << endl;
    gFileLogInfo.Unlock();
    return 1;
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

void tagPacketHeader::setCategory(int n)
{
    classification = (char)n;
}

BOOL tagPacketHeader::isVariableLength()
{
    return 1;
}

tagPacketHeader::tagPacketHeader()
{
}

tagSC_GET_SCRIPT::tagSC_GET_SCRIPT()
{
    setCategory(0x7c);
    setPacketID(10);
    setSize(0xb);
}

tagSC_GET_GC_INFO::tagSC_GET_GC_INFO()
{
    setCategory(0x7c);
    setPacketID(0xe);
    setSize(0xf);
}

tagSC_CHECK_SCRIPT_VERSION::tagSC_CHECK_SCRIPT_VERSION()
{
    setCategory(0x7c);
    setPacketID(6);
    setSize(0x1f);
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

BOOL CMsgCell::PAD()
{
    LPPACKET_HEADER pPCK = (LPPACKET_HEADER)m_bBuf;
    int nSize;
    if (pPCK->isVariableLength() != 0)
    {
        nSize = m_wPos;
    }
    else
    {
        nSize = pPCK->getSize();
    }
    pPCK->setSize(nSize);
    if (pPCK->isVariableLength() != 0)
    {
        m_wSize = m_wPos;
    }
    else
    {
        m_wSize = pPCK->getSize();
    }
    nSize = pPCK->getSize();
    return m_nBufLen < nSize;
}

CMsgCell& CMsgCell::operator<<(LPPACKET_HEADER pPacket)
{
    if ((m_wSize == 0) && (m_wPos == 0))
    {
        m_wSize = pPacket->getSize();
        memcpy(m_bBuf + m_wPos, pPacket, pPacket->getSize());
        m_wPos = pPacket->getSize();
    }
    else
    {
        memcpy(m_bBuf + m_wPos, pPacket, pPacket->getSize());
        m_wPos = m_wPos + pPacket->getSize();
    }
    return *this;
}

void CMsgCell::AttachStream(char* pBuf, int wSize)
{
    memcpy(m_bBuf + m_wPos, pBuf, wSize);
    m_wPos = m_wPos + wSize;
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
    if (ftell(fp_) < 10000000)
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
    if (log_file_check() == false)
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

void ITextOutputDevice::serialize(char* s)
{
    puts("ERROR ERROR ");
}
