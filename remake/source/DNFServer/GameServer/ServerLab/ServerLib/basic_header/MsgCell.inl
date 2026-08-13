#ifndef NSL_MSGCELL_INL_
#define NSL_MSGCELL_INL_

namespace nsl {

inline CMsgCell::CMsgCell()
{
    m_wSize = 0;
    m_wPos = 0;
    m_rPos = 0;
    m_nRefCount = 0;
    m_bPacketData = false;
}

inline CMsgCell::~CMsgCell()
{
}

inline int CMsgCell::GetBufSize()
{
    return m_nBufLen;
}

inline void CMsgCell::Clear()
{
    m_wSize = 0;
    m_wPos = 0;
    m_rPos = 0;
    m_nRefCount = 0;
    m_bPacketData = false;
}

inline void CMsgCell::Clear(int n)
{
    memset(m_bBuf + m_rPos, 0, n);
}

inline void CMsgCell::Shift(int n)
{
    memmove(m_bBuf, m_bBuf + n, m_wSize - n);
    m_wSize = m_wSize - n;
    m_wPos = m_wPos - n;
}

inline void CMsgCell::SetPos(int pos)
{
    m_wPos = pos;
}

inline void CMsgCell::ShiftPos(int pos)
{
    m_wPos = m_wPos + pos;
}

inline int CMsgCell::GetPos()
{
    return m_wPos;
}

inline int CMsgCell::GetSize() const
{
    return m_wSize;
}

inline char* CMsgCell::GetBuf() const
{
    return m_bBuf;
}

inline char* CMsgCell::GetCurPos()
{
    return m_bBuf + m_wPos;
}

inline void CMsgCell::SetBuffer(LPSTR buffer)
{
    m_bBuf = buffer;
}

inline void CMsgCell::SetSize(int wSize)
{
    m_wSize = wSize;
}

inline PACKET_HEADER* CMsgCell::GetPacket()
{
    return (PACKET_HEADER*)m_bBuf;
}

inline INTERNALMSG_HEADER* CMsgCell::GetInternalMsg()
{
    return (INTERNALMSG_HEADER*)m_bBuf;
}

inline DBTR_HEADER* CMsgCell::GetDBTr()
{
    return (DBTR_HEADER*)m_bBuf;
}

inline CMsgCell& CMsgCell::operator<<(unsigned char value)
{
    *(unsigned char*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 1;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(short value)
{
    *(short*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 2;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(unsigned short value)
{
    *(unsigned short*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 2;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(int value)
{
    *(int*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 4;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(unsigned int value)
{
    *(unsigned int*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 4;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(const long long& value)
{
    *(long long*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 8;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(const double& value)
{
    *(double*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 8;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(const char* value)
{
    int nSize = (int)strlen(value);
    memcpy(m_bBuf + m_wPos, value, nSize);
    m_wPos = m_wPos + nSize;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(const short* value)
{
    int nSize = (int)strlen((const char*)value) * 2;
    memcpy(m_bBuf + m_wPos, value, nSize);
    m_wPos = m_wPos + nSize;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(const bool& value)
{
    *(bool*)(m_bBuf + m_wPos) = value;
    m_wPos = m_wPos + 1;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(PACKET_HEADER* pPacket)
{
    memcpy(m_bBuf + m_wPos, pPacket, pPacket->getSize());
    m_wPos = m_wPos + pPacket->getSize();
    m_bPacketData = true;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(INTERNALMSG_HEADER* pInternalMsg)
{
    memcpy(m_bBuf + m_wPos, pInternalMsg, pInternalMsg->getSize());
    m_wPos = m_wPos + pInternalMsg->getSize();
    m_bPacketData = true;
    m_wSize = m_wPos;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(DBTR_HEADER* pDBTr)
{
    memcpy(m_bBuf + m_wPos, pDBTr, pDBTr->sLength);
    m_wPos = m_wPos + pDBTr->sLength;
    return *this;
}

inline CMsgCell& CMsgCell::operator<<(const CMsgCell& value)
{
    memcpy(m_bBuf + m_wPos, value.m_bBuf, value.m_wSize);
    m_wPos = m_wPos + value.m_wSize;
    return *this;
}

inline void CMsgCell::LoadSizeAndBinary(void* pData)
{
    int nSize = *(int*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 4;
    memcpy(pData, m_bBuf + m_rPos, nSize);
    m_rPos = m_rPos + nSize;
}

inline void CMsgCell::LoadByteStream(unsigned char* pByte, int nSize)
{
    memcpy(pByte, m_bBuf + m_rPos, nSize);
    m_rPos = m_rPos + nSize;
}

inline void CMsgCell::AttachStream(LPSTR pBuf, int wSize)
{
    memcpy(m_bBuf + m_wPos, pBuf, wSize);
    m_wPos = m_wPos + wSize;
}

inline CMsgCell& CMsgCell::operator>>(unsigned int& value)
{
    value = *(unsigned int*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 4;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(unsigned char& value)
{
    value = *(unsigned char*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 1;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(short& value)
{
    value = *(short*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 2;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(unsigned short& value)
{
    value = *(unsigned short*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 2;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(int& value)
{
    value = *(int*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 4;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(unsigned long& value)
{
    value = *(unsigned long*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 4;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(long& value)
{
    value = *(long*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 4;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(long long& value)
{
    value = *(long long*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 8;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(double& value)
{
    value = *(double*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 8;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(char* value)
{
    int nSize = (int)strlen(m_bBuf + m_rPos);
    memcpy(value, m_bBuf + m_rPos, nSize);
    m_rPos = m_rPos + nSize;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(short* value)
{
    int nSize = (int)strlen(m_bBuf + m_rPos) * 2;
    memcpy(value, m_bBuf + m_rPos, nSize);
    m_rPos = m_rPos + nSize;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(bool& value)
{
    value = *(bool*)(m_bBuf + m_rPos);
    m_rPos = m_rPos + 1;
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(PACKET_HEADER* pPacket)
{
    memcpy(pPacket, m_bBuf + m_rPos, sizeof(PACKET_HEADER));
    m_rPos = m_rPos + sizeof(PACKET_HEADER);
    return *this;
}

inline CMsgCell& CMsgCell::operator>>(DBTR_HEADER* pDBTr)
{
    memcpy(pDBTr, m_bBuf + m_rPos, sizeof(DBTR_HEADER));
    m_rPos = m_rPos + sizeof(DBTR_HEADER);
    return *this;
}

inline void CMsgCell::DetachStream(LPSTR pBuf, int wSize)
{
    memcpy(pBuf, m_bBuf + m_rPos, wSize);
    m_rPos = m_rPos + wSize;
}

inline int CMsgCell::ID()
{
    return ((PACKET_HEADER*)m_bBuf)->getPacketID();
}

inline int CMsgCell::GETSIZE()
{
    return ((PACKET_HEADER*)m_bBuf)->getSize();
}

inline int CMsgCell::GETCATEGORY()
{
    return ((PACKET_HEADER*)m_bBuf)->getCategory();
}

inline BOOL CMsgCell::PAD()
{
    PACKET_HEADER* pPCK = (PACKET_HEADER*)m_bBuf;
    if (m_bPacketData != false)
    {
        pPCK->setSize(m_wPos);
    }
    m_wSize = m_wPos;
    return pPCK->getSize();
}

inline void CMsgCell::AddRef()
{
    m_nRefCount = m_nRefCount + 1;
}

inline void CMsgCell::AddRef2(const long& n)
{
    m_nRefCount = m_nRefCount + n;
}

inline void CMsgCell::ReleaseRef()
{
    m_nRefCount = m_nRefCount - 1;
}

} // namespace nsl

#endif // NSL_MSGCELL_INL_
