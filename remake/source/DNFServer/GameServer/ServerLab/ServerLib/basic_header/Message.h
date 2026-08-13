#ifndef NSL_MESSAGE_H_
#define NSL_MESSAGE_H_

#include "SendBuffer.h"
#include "MsgCell.h"

namespace nsl {

class TCPUser;

typedef unsigned long long __int64;

enum ENUM_DATA_TYPE
{
    SEND_DATA_NORMAL = 0,
    SEND_DATA_ENCRYPT = 1,
    SEND_DATA_COMPRESS = 2,
    SEND_DATA_ENCRYPT_COMPRESS = 3,
    RECV_DATA_NORMAL = 4,
    RECV_DATA_ENCRYPT = 5,
    RECV_DATA_COMPRESS = 6,
    RECV_DATA_ENCRYPT_COMPRESS = 7
};

enum ENUM_BUFFER_TYPE
{
    BUFFER_TYPE_NOT_SETTED = 0,
    BUFFER_TYPE_SENDBUFFER = 1,
    BUFFER_TYPE_DBBUFFER = 2
};

class IMessageStruct
{
public:
    IMessageStruct();
    virtual ~IMessageStruct();

    bool bActMsg;
    bool mbInter;
    char mMsgType;
};

class Message : public IMessageStruct
{
public:
    Message();
    virtual ~Message();
    void initialize();
    void SetActMsg(bool bAct);
    void setWorkId(unsigned int workId);
    void setStringToMessage(SendBuffer* pSendBuffer);
    void setStringToMessage(DbBuffer* pDbBuffer);
    char* getStringFromMessage();
    SendBuffer* getSendBufferFromMessage();
    DbBuffer* getDbBufferFromMessage();
    void setUserToMessage(TCPUser* pUser);
    void setCellToMessage(char* Msg, int nOriSize, int nModSize);
    void setUse(bool bUse);
    bool getUse();
    TCPUser* getUserFromMessage();
    CMsgCell* getCellFromMessage();
    void setOnDataTypeMask(int bit);
    void setOffDataTypeMask(int bit);
    int getDataTypeMask(int bit);
    void SetWouldBlock();
    bool GetWouldBlock();
    ENUM_BUFFER_TYPE GetBufferType();

    static __int64 ident;
    int mOwnerWorkId;
    ENUM_BUFFER_TYPE mBufferType;
    TCPUser* acUser;
    SendBuffer* mpSendBuffer;
    DbBuffer* mpDbBuffer;
    CMsgCell pMsg;
    int mSize;
    long dataTypeMask;
    bool bUse;
    bool mbWouldBlock;
};

inline IMessageStruct::IMessageStruct()
{
    mMsgType = '\0';
}

inline IMessageStruct::~IMessageStruct()
{
}

inline Message::Message()
    : acUser(0)
{
    mSize = 0;
    dataTypeMask = 0;
    bUse = false;
    mbWouldBlock = false;
    ident = ident + 1;
    initialize();
}

inline Message::~Message()
{
}

inline void Message::initialize()
{
    acUser = 0;
    mSize = 0;
    bUse = false;
    dataTypeMask = 0;
    pMsg.Clear();
    bActMsg = false;
    mbWouldBlock = false;
    mBufferType = BUFFER_TYPE_NOT_SETTED;
}

inline void Message::SetActMsg(bool bAct)
{
    bActMsg = bAct;
}

inline void Message::setWorkId(unsigned int workId)
{
    mOwnerWorkId = workId;
}

inline void Message::setStringToMessage(SendBuffer* pSendBuffer)
{
    mpSendBuffer = pSendBuffer;
    pMsg.SetBuffer(pSendBuffer->buf);
    mBufferType = BUFFER_TYPE_SENDBUFFER;
}

inline void Message::setStringToMessage(DbBuffer* pDbBuffer)
{
    mpDbBuffer = pDbBuffer;
    pMsg.SetBuffer(pDbBuffer->buf);
    mBufferType = BUFFER_TYPE_DBBUFFER;
}

inline char* Message::getStringFromMessage()
{
    return pMsg.GetBuf();
}

inline SendBuffer* Message::getSendBufferFromMessage()
{
    return mpSendBuffer;
}

inline DbBuffer* Message::getDbBufferFromMessage()
{
    return mpDbBuffer;
}

inline void Message::setUserToMessage(TCPUser* pUser)
{
    acUser = pUser;
}

inline void Message::setCellToMessage(char* Msg, int nOriSize, int nModSize)
{
    mSize = nOriSize;
    pMsg.m_bBuf = Msg;
    pMsg.m_wPos = pMsg.m_wPos + nModSize;
    pMsg.SetSize(nModSize);
}

inline void Message::setUse(bool bUse)
{
    this->bUse = bUse;
}

inline bool Message::getUse()
{
    return bUse;
}

inline TCPUser* Message::getUserFromMessage()
{
    return acUser;
}

inline CMsgCell* Message::getCellFromMessage()
{
    return &pMsg;
}

inline void Message::setOnDataTypeMask(int bit)
{
    dataTypeMask = dataTypeMask | (1 << bit);
}

inline void Message::setOffDataTypeMask(int bit)
{
    dataTypeMask = dataTypeMask & ~(1 << bit);
}

inline int Message::getDataTypeMask(int bit)
{
    // ORIG：dataTypeMask 先载 edx，1<<bit 走 ebx/esi（callee-saved）——
    // 操作数顺序 + if/else 分支物化（test; jle; mov $1; jmp; mov $0）。
    if (0 < (dataTypeMask & (1 << bit)))
    {
        return 1;
    }
    return 0;
}

inline void Message::SetWouldBlock()
{
    mbWouldBlock = true;
}

inline bool Message::GetWouldBlock()
{
    return mbWouldBlock;
}

inline ENUM_BUFFER_TYPE Message::GetBufferType()
{
    return mBufferType;
}

} // namespace nsl

#endif // NSL_MESSAGE_H_
