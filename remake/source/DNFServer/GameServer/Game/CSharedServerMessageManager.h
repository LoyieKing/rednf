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
    // [b28d-S112] ORIG 0x086015b9/0x08600f?? 对 `m_lenMsg = strlen(pMsg)` 发 LHS-first 直存
    // `mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)`（无 eax→edx 中转）⇒ 字段与 size_t 同型
    // （P211/P254）；消费点 ORIG 均 32 位直载，偏移/尺寸不变。
    unsigned int m_lenMsg;
    char m_msg[52];
    unsigned int m_lenMsg2;
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
    stSERVER_MESSAGE m_msg[0x32];
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
    // [b28d-S112] P292/P248 档①：ORIG 调用点 0x08601628/0x0860109? 为
    // `call; xor $0x1,%eax; test %al,%al; je`（无 sete 归一）⇒ 返回类型 bool；
    // 非 bool 返回恒产 `test; sete; test; je` 四段（探针 /tmp/gs_probe2.cpp call1/call2）。
    // mangled 不含返回类型，头内联，布局零影响。
    bool IsSetServerMessage() { return m_isSet; }
    EVENT_SERVER_MESSAGE* GetCurServerMessage() { return &m_event; }
    void setLoadingComplete() { m_isSet = 1; }

    CSharedServerMessageAdapter m_adapter;
    // b28c-L20: unsigned char→bool（ORIG 证据 0x08600b83 `movzbl 0x4(%eax),%eax` +
    // 0x08600b87 `xor $0x1,%eax; test %al,%al; je` ⇒ 该字段按 bool 物化；同族判例见
    // C28L12 的 CUser.h m_active224/225 与 C28L14 的 CVillageMonster::m_field10）。
    // +0x04 偏移、sizeof 0x18、m_pad[3] 空洞、IsSetServerMessage 的 unsigned char 返回
    // （P9：ORIG 0x084ed182 W sz 0xc 为 movzbl 直返无归一）全部不变；本 TU 全消费段
    // fresh 联判 ndiff=0。IsSetServerMessage/setLoadingComplete 为头内联 ⇒ 消费 TU 需重编自查。
    bool m_isSet;
    char m_pad[3];
    EVENT_SERVER_MESSAGE m_event;
};

#endif
