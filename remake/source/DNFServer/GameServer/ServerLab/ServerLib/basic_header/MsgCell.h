#ifndef NSL_MSGCELL_H_
#define NSL_MSGCELL_H_

#include <string.h>

#include "PacketDesign.h"
#include "InternalMsgDesign.h"
#include "DBTransactionDesign.h"

namespace nsl {

typedef char* LPSTR;
typedef const char* LPCSTR;
typedef unsigned short WORD;
typedef long LONG;
typedef int BOOL;

struct PACKET_HEADER;
struct INTERNALMSG_HEADER;
struct DBTR_HEADER;

class CMsgCell
{
public:
    friend class Message;
    template <int N>
    friend class TMsgCell;
    CMsgCell();
    virtual ~CMsgCell();
    int GetBufSize();
    void Clear();
    void Clear(int n);
    void Shift(int n);
    void SetPos(int pos);
    void ShiftPos(int pos);
    int GetPos();
    int GetSize() const;
    char* GetBuf() const;
    char* GetCurPos();
    void SetBuffer(LPSTR buffer);
    void SetSize(int wSize);
    PACKET_HEADER* GetPacket();
    INTERNALMSG_HEADER* GetInternalMsg();
    DBTR_HEADER* GetDBTr();

    CMsgCell& operator<<(unsigned char value);
    CMsgCell& operator<<(short value);
    CMsgCell& operator<<(unsigned short value);
    CMsgCell& operator<<(int value);
    CMsgCell& operator<<(unsigned int value);
    CMsgCell& operator<<(const long long& value);
    CMsgCell& operator<<(const double& value);
    CMsgCell& operator<<(const char* value);
    CMsgCell& operator<<(const short* value);
    CMsgCell& operator<<(const bool& value);
    CMsgCell& operator<<(PACKET_HEADER* pPacket);
    CMsgCell& operator<<(INTERNALMSG_HEADER* pInternalMsg);
    CMsgCell& operator<<(DBTR_HEADER* pDBTr);
    CMsgCell& operator<<(const CMsgCell& value);

    void LoadSizeAndBinary(void* pData);
    void LoadByteStream(unsigned char* pByte, int nSize);
    void AttachStream(LPSTR pBuf, int wSize);

    CMsgCell& operator>>(unsigned int& value);
    CMsgCell& operator>>(unsigned char& value);
    CMsgCell& operator>>(short& value);
    CMsgCell& operator>>(unsigned short& value);
    CMsgCell& operator>>(int& value);
    CMsgCell& operator>>(unsigned long& value);
    CMsgCell& operator>>(long& value);
    CMsgCell& operator>>(long long& value);
    CMsgCell& operator>>(double& value);
    CMsgCell& operator>>(char* value);
    CMsgCell& operator>>(short* value);
    CMsgCell& operator>>(bool& value);
    CMsgCell& operator>>(PACKET_HEADER* pPacket);
    CMsgCell& operator>>(DBTR_HEADER* pDBTr);

    void DetachStream(LPSTR pBuf, int wSize);
    int ID();
    int GETSIZE();
    int GETCATEGORY();
    BOOL PAD();
    void AddRef();
    void AddRef2(const long& n);
    void ReleaseRef();

private:
    LPSTR m_bBuf;
    int m_nBufLen;
    int m_wSize;
    int m_wPos;
    int m_rPos;
    bool m_bPacketData;
    LONG m_nRefCount;
};

} // namespace nsl

#include "MsgCell.inl"

#endif // NSL_MSGCELL_H_
