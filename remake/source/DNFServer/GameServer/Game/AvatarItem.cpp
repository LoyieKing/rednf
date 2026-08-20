// df_game_r Game/ WongWork::Avatar_Item 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。
#include "AvatarItem.h"
#include <cstddef>
#include <cstring>

static_assert(sizeof(stAvatarEmblemInfo_t) == 0x1e, "avatar emblem ABI layout changed");
static_assert(offsetof(stAvatarEmblemInfo_t, stEmblemSocket) == 0, "avatar emblem socket offset changed");
static_assert(sizeof(WongWork::Avatar_Item) == 0x104, "Avatar_Item ABI size changed");
static_assert(offsetof(WongWork::Avatar_Item, m_emblemInfo) == 0x21, "Avatar_Item emblem offset changed");

namespace WongWork
{

void Avatar_Item::init() { memset(this, 0, sizeof(*this)); }

int Avatar_Item::GetExpireDate() const { return m_expireDate; }

int Avatar_Item::GetRegistTime() const { return m_registTime; }

int Avatar_Item::GetRemainDate(int tick) const
{
    if (m_expireDate <= 0)
        return m_expireDate;
    int remain = m_expireDate - tick;
    if (remain == 0)
        return 1;
    return remain;
}

const char* Avatar_Item::GetIPGAgencyNo() const { return m_ipgAgencyNo; }

const char* Avatar_Item::getJewelSocketData() const
{
    return (const char*)&m_emblemInfo;
}

void* Avatar_Item::getExpansionInfo() const { return (void*)&m_color1; }

int* Avatar_Item::getExnansionInfoRef() const { return (int*)&m_color1; }

void Avatar_Item::setColorEss(short color1, short color2)
{
    m_color1 = (unsigned short)color1;
    m_color2 = (unsigned short)color2;
}

void Avatar_Item::setExpansionInfo(const ::stAvatarExpansionInfo_t& info)
{
    m_color1 = info.m_color[0];
    m_color2 = info.m_color[1];
}

void Avatar_Item::SetData(int expireDate, int registTime, const char* ipgAgency)
{
    m_expireDate = expireDate;
    m_registTime = registTime;
    strncpy(m_ipgAgencyNo, ipgAgency, 0x18);
}

void Avatar_Item::setEmblemSocketData(const stAvatarEmblemInfo_t& emblem)
{
    memcpy(&m_emblemInfo, &emblem, sizeof(stAvatarEmblemInfo_t));
}

int Avatar_Item::updateEmblemSocket(int grade, unsigned long itemId)
{
    // 报告中的 grade * 6 + 2 对应 packed 槽的具名成员，保持原 ABI 与布局。
    if (m_emblemInfo.stEmblemSocket[grade].emblemSocketType_ == 0)
        return 0;
    m_emblemInfo.stEmblemSocket[grade].emblemItemIdx_ = (int)itemId;
    return 1;
}

}  // namespace WongWork
