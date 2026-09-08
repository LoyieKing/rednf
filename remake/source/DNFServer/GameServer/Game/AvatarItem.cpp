// df_game_r Game/ WongWork::Avatar_Item 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。
#include "AvatarItem.h"
#include <cstddef>
#include <cstring>

static_assert(sizeof(stAvatarEmblemInfo_t) == 0x1e, "avatar emblem ABI layout changed");
static_assert(offsetof(stAvatarEmblemInfo_t, stEmblemSocket) == 0, "avatar emblem socket offset changed");
static_assert(sizeof(WongWork::Avatar_Item) == 0x44, "Avatar_Item ABI size changed (ORIG packed 0x44, DynamicPool MoreAlloc 步长 0x4c)");
static_assert(offsetof(WongWork::Avatar_Item, m_emblemInfo) == 0x21, "Avatar_Item emblem offset changed");
static_assert(offsetof(WongWork::Avatar_Item, m_color1) == 0x3f, "Avatar_Item color1 offset changed (ORIG 0x3f)");
static_assert(offsetof(WongWork::Avatar_Item, m_color2) == 0x41, "Avatar_Item color2 offset changed (ORIG 0x41)");

namespace WongWork
{

// ORIG _ZN8WongWork11Avatar_Item4initEv W 0x82fa35a：仅清 +0x00/+0x04 两个 int、
// memset(m_ipgAgencyNo,0,0x19)、memset(+0x3f,0,4)；emblem 区与 +0x43 之后不清。
void Avatar_Item::init()
{
    m_expireDate = 0;
    m_registTime = 0;
    memset(m_ipgAgencyNo, 0, 0x19);
    memset(&m_color1, 0, 4);
}

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
    // ORIG 0x82fa326：4 字节整读 info（m_color[0..1] 连续）整写 this+0x3f
    //（覆盖 m_color1/+0x3f 与 m_color2/+0x41）。4.4-O0 对两次 ushort 赋值
    // 恒拆 movzwl+mov %dx（14 条），整型写复现 ORIG 单 mov %edx,0x3f(%eax)（8 条）。
    // 复现 ORIG 单 `mov %edx,0x3f(%eax)`（无 add 折合）：P82 pack(1) 尾部视图
    // 具名成员访问保留 disp 寻址（探针实证；cast 指针 store 会折 add $0x3f）。
    struct __attribute__((packed)) V { char m_pad[0x3f]; unsigned int m_dw; };
    ((V*)this)->m_dw = *(const unsigned int*)&info.m_color[0];
}

void Avatar_Item::SetData(int expireDate, int registTime, const char* ipgAgency)
{
    m_expireDate = expireDate;
    m_registTime = registTime;
    strncpy(m_ipgAgencyNo, ipgAgency, 0x18);
}

void Avatar_Item::setEmblemSocketData(const stAvatarEmblemInfo_t& emblem)
{
    // ORIG 0x1d 字节 = 7×mov 链 + 尾 ushort（4.1 分块启发式）；结构整体赋值
    // 试 rep movsl 对比（N90/N84 族）。
    m_emblemInfo = emblem;
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
