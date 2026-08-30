// ORIG 域常量（nm：_ZN12InformNoticeL18kInformNoticeIndexE / L16kInformNoticeBitE，
// r 段内部链接；m_flags[16] × 8bit 标志板）
namespace InformNotice
{
static const int kInformNoticeIndex = 16;
static const int kInformNoticeBit = 8;
}
#include "CCharacInformNotice.h"
#include "CUser.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include <string.h>



// ORIG C1 0x080e4790：基类 CData ctor（隐式）后 _reset。
CCharacInformNotice::CCharacInformNotice()
{
    _reset();
}

CCharacInformNotice::~CCharacInformNotice()
{
}

// ORIG T 0x080e4ab8：memset(this+5, 0, 0x10)
void CCharacInformNotice::_reset()
{
    memset(m_flags, 0, sizeof(m_flags));
}

// ORIG T 0x080e4892：bit>7 或 byte>0xf → 0；否则返回 !(flag bit set)
bool CCharacInformNotice::checkInformNoticeFlag(unsigned int bit, unsigned int byteIdx)
{
    if (bit > 7)
        return false;
    if (byteIdx >= InformNotice::kInformNoticeIndex)
        return false;
    return ((m_flags[byteIdx] >> bit) & 1) == 0;
}

// ORIG T 0x080e4806：flag&7=bit，flag>>3=byte；check 真则置位并 alter。
void CCharacInformNotice::handleInformNoticeFlag(unsigned int flag)
{
    unsigned int bit = flag & 7;
    if (bit > 7)
        return;
    unsigned int byteIdx = flag >> 3;
    if (byteIdx >= InformNotice::kInformNoticeIndex)
        return;
    if (!checkInformNoticeFlag(bit, byteIdx))
        return;
    m_flags[byteIdx] = (unsigned char)(m_flags[byteIdx] + (1u << bit));
    alter();
}

// ORIG T 0x080e48ec：movzbl 0x5(%edx,%eax,1),%eax 直返（P9：返回 unsigned char，单条零扩展）。
unsigned char CCharacInformNotice::getInformNoticeFlag(int idx)
{
    return m_flags[idx];
}

// ORIG T 0x080e48fc：位展开为索引清单（clear + 双层 setle 循环 + push_back）。
void CCharacInformNotice::getInformNoticeFlagIndex(std::vector<unsigned char>& out)
{
    out.clear();
    for (int byteIdx = 0; byteIdx < InformNotice::kInformNoticeIndex; ++byteIdx)
    {
        unsigned char bits = static_cast<unsigned char>(getInformNoticeFlag(byteIdx));
        for (int bitIdx = 0; bitIdx < InformNotice::kInformNoticeBit; ++bitIdx)
        {
            if (((bits >> bitIdx) & 1) != 0)
            {
                out.push_back(static_cast<unsigned char>(
                    byteIdx * InformNotice::kInformNoticeBit + bitIdx));
            }
        }
    }
}

// ORIG T 0x080e4994：vector 索引清单 + 0x1b8 包（put_header/put_byte/finalize + CUser::Send）。
void CCharacInformNotice::sendInformNoticeFlag(CUser* user)
{
    std::vector<unsigned char> indexes;
    getInformNoticeFlagIndex(indexes);

    PacketGuard packet;
    packet.put_header(0, 0x1b8);
    packet.put_byte(static_cast<int>(indexes.size()));
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        packet.put_byte(static_cast<int>(indexes[i]));
    }
    packet.finalize(true);
    user->Send(packet);
}

// ORIG T 0x080e4ade：vtable+0x1c 虚调用 _reset → memcpy(this+5,data,0x10) → sendInformNoticeFlag → 1。
bool CCharacInformNotice::loadData(CUser* user, char* data)
{
    _reset();
    memcpy(m_flags, data, sizeof(m_flags));
    sendInformNoticeFlag(user);
    return true;
}

bool CCharacInformNotice::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacInformNotice.cpp", 0x77), true);
    guard << 0x311;
    guard << user->GetUID();
    SIG_SAVE_CHARAC_INFORM_NOTICE* sig =
        guard.GetInBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>();
    if (sig == 0)
        return false;
    memset(sig, 0, 0x14);
    sig->m_characNo = user->get_charac_no(-1);
    memcpy(sig->m_flags, m_flags, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

void CCharacInformNotice::getData(char* data) const
{
    memcpy(data, m_flags, sizeof(m_flags));
}
