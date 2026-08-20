#ifndef DNF_GAME_CSHARED_SERVER_MESSAGE_MANAGER_H_
#define DNF_GAME_CSHARED_SERVER_MESSAGE_MANAGER_H_

#include <string>
#include <utility>
#include <vector>

class CUser;

struct stSERVER_MESSAGE
{
    unsigned char m_kind;
    unsigned char m_index;
    int m_lenMsg;
    char m_msg[52];
    int m_lenMsg2;
    char m_msg2[20];
    int m_endTime;
};

struct EVENT_SERVER_MESSAGE
{
    unsigned char m_count;
    std::vector<stSERVER_MESSAGE> m_vecMsg;
    void clear()
    {
        m_count = 0;
        m_vecMsg.clear();
    }
};

struct SIG_EVENT_SERVER_MESSAGE
{
    unsigned char m_count;
    stSERVER_MESSAGE m_msg[0x64];
};


struct stSharedServerMessageInfo;
class CSharedServerMessageManager
{
public:
    class CSharedServerMessageAdapter
    {
    public:
        CSharedServerMessageAdapter() : m_pInfo(0) {}
        ~CSharedServerMessageAdapter() {}
        void SetServerMessageAddInfo(stSharedServerMessageInfo* info) { m_pInfo = info; }
        int GetMappedEventID(int index);
        bool IsOnIndex(int index);
        int GetShareKindWithIndex(int index);
        stSharedServerMessageInfo* m_pInfo;
    };

    CSharedServerMessageManager();
    explicit CSharedServerMessageManager(stSharedServerMessageInfo* info);
    ~CSharedServerMessageManager();

    void SendServerMessageInfo(CUser*, SIG_EVENT_SERVER_MESSAGE*, char);
    void SetServerMessage(SIG_EVENT_SERVER_MESSAGE*);
    stSERVER_MESSAGE* getCurMessage(int);
    bool IsChangableMessage(int, int);
    void DeleteSpecificMessage(int);
    bool CheckUserCondition(CUser*, int, int&);
    void UpdateServerMessage(CUser*, int, char*, char*, int);
    void UpdateServerMessageByBroadcast(int, char*, char*, int);
    void LoadServerMessageFromDB();
    bool IsSetServerMessage() { return m_isSet != 0; }
    EVENT_SERVER_MESSAGE* GetCurServerMessage() { return &m_event; }
    void setLoadingComplete() { m_isSet = 1; }

    CSharedServerMessageAdapter m_adapter;
    unsigned char m_isSet;
    char m_pad[3];
    EVENT_SERVER_MESSAGE m_event;
};

#endif
