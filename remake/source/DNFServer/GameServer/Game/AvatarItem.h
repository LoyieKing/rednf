#ifndef DNF_GAME_AVATARITEM_H_
#define DNF_GAME_AVATARITEM_H_

// df_game_r Game/ WongWork::Avatar_Item（独立 TU，从 GameStubs.cpp 拆出）。
// 布局：m_expireDate(+0x00) / m_registTime(+0x04) / m_ipgAgencyNo(+0x08, 25B)
//       / m_emblemInfo(+0x21, 0x1eB) / m_color1(+0x3f) / m_color2(+0x41) / pad(+0x43)。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。

#include "GamePODTypes.h"

namespace WongWork
{

class Avatar_Item
{
public:
    void init();
    int GetExpireDate() const;
    int GetRegistTime() const;
    int GetRemainDate(int tick) const;
    const char* GetIPGAgencyNo() const;
    const char* getJewelSocketData() const;
    void* getExpansionInfo() const;
    int* getExnansionInfoRef() const;
    void setColorEss(short color1, short color2);
    void setExpansionInfo(const ::stAvatarExpansionInfo_t& info);
    void SetData(int expireDate, int registTime, const char* ipgAgency);
    void setEmblemSocketData(const stAvatarEmblemInfo_t& emblem);
    int updateEmblemSocket(int grade, unsigned long itemId);

    int m_expireDate;                          // +0x00
    int m_registTime;                          // +0x04
    char m_ipgAgencyNo[25];                    // +0x08（0x19 字节）
    stAvatarEmblemInfo_t m_emblemInfo;         // +0x21（0x1e 字节）
    unsigned short m_color1;                   // +0x3f
    unsigned short m_color2;                   // +0x41
    char m_pad43[0xbd];                        // +0x43
};

}  // namespace WongWork

#endif  // DNF_GAME_AVATARITEM_H_
