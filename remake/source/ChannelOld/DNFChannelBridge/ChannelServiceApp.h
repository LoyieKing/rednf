#ifndef CHANNELSERVICEAPP_H_
#define CHANNELSERVICEAPP_H_

#include "stdafx.h"
#include "ThreadLock.h"
#include "DebugLog.h"
#include "GlobalInstance.h"
#include "Thread.h"
#include "System.h"
#include "Reactor.h"
#include "Exception.h"
#include <vector>
#include <map>
#include <queue>
#include <deque>

#define PACKET_CTOR_BODY(cat, pid, sz) \
    memset(this, 0, sz); \
    setCategory(cat); \
    setPacketID(pid); \
    setSize(sz)

#define PACKET_HEADER_SET(cat, pid, sz) \
    setCategory(cat); \
    setPacketID(pid); \
    setSize(sz)

#define GLOG(stream, ...) \
    (stream).Lock(); \
    (stream) << __VA_ARGS__ << endl; \
    (stream).Unlock()

#define DNF_LOG_IN() GLOG(gFileLogInfo, "In  " << __FUNCTION__)
#define DNF_LOG_OUT() GLOG(gFileLogInfo, "Out " << __FUNCTION__)

#define CHANNEL_HANDLER_BEGIN(name) \
    DWORD ChannelServiceApp::ChannelService::on##name(LPPACKET_HEADER pPCK, TCPUser* u) \
    { \
        DNF_LOG_IN();

#define CHANNEL_HANDLER_END() \
        DNF_LOG_OUT(); \
        return 1; \
    }

#pragma pack(push, 1)

class CMsgCell;
class TCPSocket;
class CRijndael;
template <class T> class TSerializer;

struct tagPacketHeader
{
    char classification;
    char msg_no;
    unsigned int sLength;
    unsigned int check_sum;
    char ack;

    void setAckOk();
    void setAckFail();
    DWORD getPacketID();
    void setPacketID(int n);
    int getSize();
    void setSize(int n);
    int getCategory();
    void setCategory(int n);
    void setCheckSum(unsigned int sum);
    unsigned int getCheckSum();
    void setVariableLength(bool bVariable);
    BOOL isVariableLength();
    operator DWORD();
    char* _copyData();
    char* _getData();
    char* _getHeader();
    tagPacketHeader();
};

typedef tagPacketHeader* LPPACKET_HEADER;

CMsgCell* GetMessageBuffer(int nSize);

struct tpServerInfo
{
    char channel_name[20];
    int max_user_num;
    int cur_user_num;
    char server_ip[16];
    int port;
};

struct tServerGroupInfo
{
    char server_group_name[20];
    int server_count;
};

struct tServerGcInfo
{
    char server_group_name[20];
    int gc_no;
};

struct tagCS_UPDATE_CHANNEL_INFO : public tagPacketHeader
{
    tagCS_UPDATE_CHANNEL_INFO();
    int gc_no;
    int group;
    int channel_no;
    int max_user_num;
    int cur_user_num;
    char server_ip[16];
    int port;
};

struct tagSC_GET_GC_INFO : public tagPacketHeader
{
    tagSC_GET_GC_INFO();
    int count;
};

struct tagCS_CHECK_SCRIPT_VERSION : public tagPacketHeader
{
    char channel_script_version[0x10];
};

struct tagSC_CHECK_SCRIPT_VERSION : public tagPacketHeader
{
    tagSC_CHECK_SCRIPT_VERSION();
    int is_valid_version;
    char channel_script_version[0x10];
};

struct tagCS_NOTICE_CHANNEL_SERVER : public tagPacketHeader
{
    int id;
    char server_ip[16];
    int port;
};

struct tagSC_GET_SCRIPT : public tagPacketHeader
{
    tagSC_GET_SCRIPT();
};

#pragma pack(pop)

template <class T>
class TSerializer
{
public:
    TSerializer(const T& data)
    {
        m_Data = &data;
    }
    const unsigned char* getBytePtr() const
    {
        return (const unsigned char*)m_Data;
    }

private:
    const T* m_Data;
};

class CMsgCell
{
public:
    CMsgCell();
    virtual ~CMsgCell();
    BOOL PAD();
    void AttachStream(char* szBuf, int nSize);
    CMsgCell& operator<<(tagPacketHeader* pHeader);
    template <class T>
    CMsgCell& operator<<(const TSerializer<T>& obj);
    char* GetBuf() const;
    int GetSize() const;

protected:
    char* m_bBuf;
    int m_nBufLen;
    int m_wSize;
    int m_wPos;
    int m_nRefCount;
};

template <int BufSize>
class TMsgCell : public CMsgCell
{
public:
    TMsgCell()
        throw()
    {
        m_nBufLen = BufSize;
        m_bBuf = m_bData;
    }
    virtual ~TMsgCell()
        throw()
    {
    }

private:
    char m_bData[BufSize];
};

template <unsigned int Size>
class TCircularQueueBuffer
{
public:
    TCircularQueueBuffer()
    {
        m_nPopIndex = 0;
        m_nPushIndex = 0;
        memset(&m_buffer[0], 0, sizeof(m_buffer));
    }
    void clear()
    {
        m_nPopIndex = 0;
        m_nPushIndex = m_nPopIndex;
        memset(&m_buffer[0], 0, sizeof(m_buffer));
    }
    int push(char* in_block, int in_nSize, char* file, int line);
    int pop(int nSize);
    char* peekPop()
    {
        return m_buffer + m_nPopIndex;
    }
    bool popCopy(int in_nSize, char* pCopyee);
    bool peekCopy(int in_nSize, char* pCopyee);
    bool isPopStraight(int nSize);
    int getPushedLength() const
    {
        if (m_nPushIndex >= m_nPopIndex)
        {
            return m_nPushIndex - m_nPopIndex;
        }
        return m_nPushIndex - m_nPopIndex + Size;
    }
    int getPopLengthToEnd() const
    {
        return Size - m_nPopIndex;
    }
    bool isPushGreaterThanPop() const
    {
        return m_nPushIndex >= m_nPopIndex;
    }
    int isEmpty() const
    {
        return getPushedLength() == 0;
    }

private:
    char m_buffer[Size];
    unsigned int m_nPushIndex;
    unsigned int m_nPopIndex;
};

template <class T>
class TManager
{
public:
    TManager()
    {
    }
    void setManager(T* pManager)
    {
        pManager_ = pManager;
    }
    T* getManager()
    {
        return pManager_;
    }
    const T* getManager() const
    {
        return pManager_;
    }

private:
    T* pManager_;
};

template <class T, int Size, class Q>
class TMemoryPoolStatic
{
public:
    TMemoryPoolStatic();
    ~TMemoryPoolStatic();
    void startup();
    void shutdown();
    T* alloc();
    void free(T* t)
    {
        if (t == NULL)
        {
            throw Exception("cannot free NULL");
        }
        if (repository_ == NULL)
        {
            throw Exception("repository NULL");
        }
        __int64 r = (__int64)t - (__int64)repository_;
        int idx = 0;
        if (r > 0)
        {
            idx = (int)(r / sizeof(T));
        }
        if (!((idx < Size) && (-1 < idx)))
        {
            throw Exception("Not alloc from this pool");
        }
        freeq_.push(t);
    }
    int getRemain();
    int getIndex();

private:
    T* repository_;
    Q freeq_;
};

namespace ChannelServiceApp
{
    struct tServerInfo
    {
        int nMaxUserCount_;
        int nCurrentUserCount_;
        char IP[16];
        int port;
        int gc_no;
        bool use;
        int tic;
        char ChannelName[10];
        int channel_no;
    };

    class Protocols;
    class PortInfo;
    class Handlers;
    class Threads;
    class ChannelServer;
    class ScriptThread;
    class TCPHandler;
    class UDPHandler;
    class TCPHandlerRelay;
    class UDPHandlerRelay;
    class TCPUserStates;
    class UDPUserStates;
    class UDPUser;
    class TCPUser;
    class TCPAcceptThread;
    class UDPThread;
    class ServerGroup;
    class CheckThread;
    class Channel;
    class ChannelScript;
    class TCPThread;
    class Reactor;
    class ChannelService;
    class UserPools;

    extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogInfo;
    extern TDebugTrace<char> gFileLogInfo;
    extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogWarn;
    extern TDebugTrace<char> gFileLogWarn;
    extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogError;
    extern TDebugTrace<char> gFileLogError;
    extern TGlobalInstance<TextOutputDevice_FILE> g_FileLogCri;
    extern TDebugTrace<char> gFileLogCri;
    extern TGlobalInstance<TextOutputDevice_stdout> g_LogInfo;
    extern TDebugTrace<char> gLogInfo;
    extern TGlobalInstance<TextOutputDevice_stdout> g_LogCri;
    extern TDebugTrace<char> gLogCri;
    extern TGlobalInstance<TextOutputDevice_stdout> g_LogWarn;
    extern TDebugTrace<char> gLogWarn;
    extern TGlobalInstance<TextOutputDevice_stdout> g_LogError;
    extern TDebugTrace<char> gLogError;
    void createFileLogInfo();
    void createFileLogWarn();
    void createFileLogError();
    void createFileLogCri();
    TextOutputDevice_FILE* getFileLogInfo();
    TextOutputDevice_FILE* getFileLogWarn();
    TextOutputDevice_FILE* getFileLogError();
    TextOutputDevice_FILE* getFileLogCri();
    void createLogInfo();
    void createLogCri();
    void createLogWarn();
    void createLogError();
    TextOutputDevice_stdout* getLogInfo();
    TextOutputDevice_stdout* getLogCri();
    TextOutputDevice_stdout* getLogWarn();
    TextOutputDevice_stdout* getLogError();

    class Protocols
    {
    public:
        Protocols();
        virtual DWORD registerProtocols();
        virtual DWORD registerProtocolsExtra();
    };

    class PortInfo
    {
    public:
        PortInfo();
        ~PortInfo();
        void setTCPPort(int nPort);
        void setUDPPort(int nPort);
        int getTCPPort() const
        {
            return nTCPPort_;
        }
        int getUDPPort() const
        {
            return nUDPPort_;
        }

        int nTCPPort_;
        int nUDPPort_;
        std::vector<int> vPorts_;
    };

    class Handlers
    {
    public:
        Handlers();
        TCPHandlerRelay* getTCPHandlerRelay()
        {
            return handlerTCPRelay_;
        }

        UDPHandlerRelay* handlerUDPRelay_;
        TCPHandlerRelay* handlerTCPRelay_;
    };

    class Threads
    {
    public:
        Threads();
        ~Threads();
        TCPThread* getTCPThread()
        {
            return threadTCP_;
        }
        TCPAcceptThread* getTCPAcceptThread()
        {
            return threadTCPAccept_;
        }
        ScriptThread* getScriptThread()
        {
            return threadScript_;
        }

        TCPAcceptThread* threadTCPAccept_;
        TCPThread* threadTCP_;
        UDPThread* threadUDP_;
        CheckThread* threadCheck_;
        ScriptThread* threadScript_;
        std::vector<UDPThread*> arrayUDPThreads_;
    };

    class TCPHandler
    {
    public:
        TCPHandler();
        virtual void dispatch(TCPUser* u, char* szBlock, int nSize, int Catagory) = 0;
    };

    class UDPHandler
    {
    public:
        UDPHandler();
        virtual void dispatch(char* szBlock, int nSize, int Catagory) = 0;
    };

    class TCPHandlerRelay : public TManager<ChannelService>, public TCPHandler
    {
    public:
        TCPHandlerRelay();
        ~TCPHandlerRelay();
        virtual void dispatch(TCPUser* u, char* szBlock, int nSize, int Catagory);
    };

    class UDPHandlerRelay : public TManager<ChannelService>, public UDPHandler
    {
    public:
        UDPHandlerRelay();
        virtual void dispatch(char* szBlock, int nSize, int Catagory);
    };

    class TCPUserStates
    {
    public:
        TCPUserStates();

    private:
        char m_state;
    };

    class UDPUserStates
    {
    public:
        UDPUserStates();
    };

    class UDPUser : public UDPUserStates
    {
    public:
        UDPUser();
        ~UDPUser();
    };

    class TCPUser : public TManager<ChannelService>, public TCPUserStates
    {
    public:
        TCPUser();
        ~TCPUser();
        void shutdown();
        void postDisconnected(int nReason);
        void setLastAccessTime();
        void startupAfterSetSocket();
        void onAccept();
        void onError();
        void onClose(char* file, int line);
        void onRead(char* file, int line);
        void onRead_();
        void onWrite(char* file, int line);
        void onWrite_();
        int onWrite2Buffer(CMsgCell* pMsgCell);
        int send(CMsgCell* pMsgCell);
        bool isAboutToDisconnect() const
        {
            return bAboutToDisconnect_;
        }
        bool isDisconnected() const
        {
            return bDisconnected_;
        }
        bool isToWrite() const;
        bool isIdle() const;
        int getACCID() const
        {
            return nAccID_;
        }
        void setACCID(unsigned int nAccID)
        {
            nAccID_ = nAccID;
        }
        TCPSocket* getSocket()
        {
            return pSock_;
        }
        SOCKET getHandle();
        void setSocket(TCPSocket* pSock)
        {
            pSock_ = pSock;
        }

    private:
        TThreadLock<ThreadLock_linux> lockRecv_;
        TThreadLock<ThreadLock_linux> lockSend_;
        int nAccID_;
        int nReason_;
        bool bDisconnected_;
        bool bAboutToDisconnect_;
        TCPSocket* pSock_;
        TCircularQueueBuffer<655360> bufferRecv_;
        TCircularQueueBuffer<655360> bufferSend_;
        __int64 tickLast_;
    };

    class TCPAcceptThread : public Thread, public TManager<ChannelService>
    {
    public:
        TCPAcceptThread();
        ~TCPAcceptThread();
        virtual void loop(void* temp);
        void setPort(int nPort)
        {
            nPort_ = nPort;
        }
        TCPUser* lockPopAcceptedUser();
        void lockPushAcceptedUser(TCPUser* in_pUser);
        void notifyCannotCreateUser(TCPSocket& s);
        void notifyCannotLoginByMaxUserCount(TCPSocket& s);
        unsigned short getPort() const;

    private:
        std::queue<TCPUser*> queueAcceptedUser_;
        TThreadLock<ThreadLock_linux> lockQueueAcceptedUser_;
        int nPort_;
    };

    class UDPThread : public Thread, public TManager<ChannelService>
    {
    public:
        UDPThread();
        ~UDPThread();
        virtual void loop(void* temp);
        void logError();
        void setHandler(UDPHandler* pHandler);
        void setPort(int nPort);

    private:
        UDPHandler* pHandler_;
        int nPort_;
    };

    class ServerGroup
    {
    public:
        ServerGroup();
        ~ServerGroup();
        void decreseServerCount();
        void increseServerCount();
        int getServerCount() const
        {
            return nServerCount_;
        }

        int nServerCount_;
        bool use;
        char ServerName[20];
        std::map<int, tServerInfo*> listServerInfo_;
        TThreadLock<ThreadLock_linux> Lock;
        tServerInfo ServerInfo[0x1000];
    };

    class CheckThread : public Thread, public TManager<ChannelService>
    {
    public:
        CheckThread();
        ~CheckThread();
        virtual void loop(void* temp);
    };

    class Channel
    {
    public:
        Channel();
        ~Channel();

        ServerGroup Servers[0x100];
        int ServerGroupIndex;
        int ServerGroupCount;
        std::map<char*, int> gc_map;
        TThreadLock<ThreadLock_linux> LockChannel;
    };

    class ChannelScript
    {
    public:
        ChannelScript();
        ~ChannelScript();
        void ReloadScript();
        char* getScriptFromFile();
        int getScriptFileSize();

        FILE* fp;
        long lSize;
        char buffer[0x100000];
        TThreadLock<ThreadLock_linux> lockFile;
    };

    struct tChannelServer
    {
        int id;
        char IP[16];
        int port;
        int tic;
        TCPUser* uTCP;
    };

    class ChannelServer
    {
    public:
        ChannelServer();

        int ChannelServerNumber;
        tChannelServer CServers[0x100];
    };

    class Reactor : public TReactor<EpollReactor<TCPUser>, TCPUser>
    {
    public:
        Reactor()
        {
        }
        ~Reactor()
        {
        }
        TReactor<EpollReactor<TCPUser>, TCPUser>* getReactor()
        {
            return this;
        }
    };

    typedef Reactor TCPReactor;

    class UserPools
    {
    public:
        UserPools();
        ~UserPools();
        TCPSocket* createTCPSocket(char* file, int line);
        void destroyTCPSocket(TCPSocket* pTCPSocket);
        TCPUser* createTCPUser(char* file, int line);
        void destroyTCPUser(TCPUser* pTCPUser, char* file, int line);

        TMemoryPoolStatic<TCPSocket, 1000, std::queue<TCPSocket*, std::deque<TCPSocket*> > > m_poolTCPSocket;
        TMemoryPoolStatic<TCPUser, 1000, std::queue<TCPUser*, std::deque<TCPUser*> > > m_poolTCPUser;
        TThreadLock<ThreadLock_linux> LockTCPUser;
        TThreadLock<ThreadLock_linux> LockTCPSocket;
    };

    class ChannelService : public PortInfo, public Handlers, public Protocols, public Threads, public UserPools, public Reactor, public Channel, public ChannelServer, public ChannelScript
    {
    public:
        typedef DWORD (ChannelService::*FPMessageHandler)(LPPACKET_HEADER, TCPUser*);
        typedef DWORD (ChannelService::*FPMessageHandlerExtra)(LPPACKET_HEADER);

        enum Mode
        {
            UDP2TCP = 0,
            Mode_Max
        };

        ChannelService();
        ChannelService(char* service_identify);
        ~ChannelService();
        virtual DWORD registerProtocols();
        virtual DWORD registerProtocolsExtra();

        FPMessageHandler GetMessageHandler(int n);
        FPMessageHandlerExtra GetMessageHandlerExtra(int n);
        void setMode(Mode mode);
        __int64 getTick() const;
        void setTick();
        DWORD onCS_GET_SCRIPT(LPPACKET_HEADER pPCK, TCPUser* u);
        DWORD onCS_GET_GC_INFO(LPPACKET_HEADER pPCK, TCPUser* u);
        DWORD onCS_UPDATE_CHANNEL_INFO(LPPACKET_HEADER pPCK);
        DWORD onCS_CHECK_SCRIPT_VERSION(LPPACKET_HEADER pPCK, TCPUser* u);
        DWORD onCS_NOTICE_CHANNEL_SERVER(LPPACKET_HEADER pPCK, TCPUser* u);
        void startup();
        void shutdown();
        char* getServiceName();

        Mode Mode_;
        __int64 tick_;
        char serviceName[0x14];
        FPMessageHandlerExtra m_pfnMsgExtra[0x400];
        FPMessageHandler m_pfnMsg[0x400];
    };

}

#include "Reactor.inl"

#endif // CHANNELSERVICEAPP_H_
