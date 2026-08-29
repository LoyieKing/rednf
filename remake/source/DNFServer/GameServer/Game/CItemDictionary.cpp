#include "CItemDictionary.h"
#include <cstring>

#include "CUser.h"   // 经 SigTypes.h 提供权威 SIG_ITEM_DICTIONARY_FLAG（0x138c）
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "StreamPool.h"

// ORIG 少量 TU 各一份 _ZL18kItemDictionaryBit（r 0x1）：字节位图每字节位数。
static const unsigned char kItemDictionaryBit = 8;

namespace {
const unsigned int kBinaryLength = 5000;
inline bool valid(unsigned int item) { return item < kBinaryLength * kItemDictionaryBit; }
}

// ORIG 0x0811d5d4：仅 CDataC1（置基类 vptr）+ 编译器置派生 vptr，不调用 _reset。
CItemDictionary::CItemDictionary() {}
CItemDictionary::~CItemDictionary() {}
// ORIG 0x0811d774：memset(this+5,0,0x1388)；不触碰 m_gainItemList。
void CItemDictionary::_reset() { std::memset(m_data, 0, kBinaryLength); }
// ORIG 0x0811d63e：DB 保存（SIG_ITEM_DICTIONARY_FLAG = int characNo +
// 5000 字节位图，共 0x138c），队列 LOGDB_Q(4)。
bool CItemDictionary::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemDictionary.cpp", 0x18), true);
    **guard << 0x2b2;
    **guard << (int)user->GetUID();
    SIG_ITEM_DICTIONARY_FLAG* sig =
        guard->GetInBuffer<SIG_ITEM_DICTIONARY_FLAG>();
    std::memset(sig, 0, 0x138c);
    sig->m_characNo = user->get_charac_no(-1);
    std::memcpy(sig->m_data, m_data, kBinaryLength);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    return true;
}
// ORIG 0x0811d7e0：memcpy(out+4, this+5, 0x1388)，无空检查。
void CItemDictionary::getData(char* out) const { std::memcpy(out + 4, m_data, kBinaryLength); }
int CItemDictionary::getGainItemCount() const {
    int count = 0;
    for (unsigned int i = 0; i < kBinaryLength; ++i)
        for (unsigned int bit = 0; bit < kItemDictionaryBit; ++bit) count += (m_data[i] >> bit) & 1;
    return count;
}
std::vector<int>& CItemDictionary::getGainItemList() {
    m_gainItemList.clear();
    for (unsigned int i = 0; i < kBinaryLength; ++i)
        for (unsigned int bit = 0; bit < kItemDictionaryBit; ++bit)
            if ((m_data[i] >> bit) & 1) m_gainItemList.push_back(static_cast<int>(i * kItemDictionaryBit + bit));
    return m_gainItemList;
}
bool CItemDictionary::getItemCheckFlag(unsigned int item, unsigned int flag) const {
    return valid(item) && flag < kItemDictionaryBit && ((m_data[item / kItemDictionaryBit] >> flag) & 1) != 0;
}
// ORIG 0x0811db60：最后一个非零字节下标+1（无非零字节返回 0）。
int CItemDictionary::getLengthBinary() const
{
    int length = 0;
    for (unsigned int i = 0; i < kBinaryLength; ++i)
    {
        if (m_data[i] != 0)
            length = (int)i + 1;
    }
    return length;
}
// ORIG 0x0811d79a：先经虚表槽7 调 _reset()，再 memcpy(data+4 → this+5, 0x1388)，返回 true。
bool CItemDictionary::loadData(CUser*, char* data)
{
    _reset();
    std::memcpy(m_data, data + 4, kBinaryLength);
    return true;
}
// ORIG 0x0811dce6：向用户发送单项词典标记（接口包 0x181：byte=1 + int item）。
void CItemDictionary::sendItemDictionaryInfo(CUser* user, unsigned int item)
{
    if (!user) return;
    PacketGuard guard;
    guard.put_header(0, 0x181);
    guard.put_byte(1);
    guard.put_int(static_cast<int>(item));
    guard.finalize(true);
    user->Send(guard);
}
void CItemDictionary::sendItemDictionaryInfoTotal(CUser* user)
{
    if (user == 0)
        return;
    PacketGuard guard;
    guard.put_header(0, 0x18a);
    const int length = getLengthBinary();
    guard.put_int(length);
    if (length != 0)
        guard.put_binary(reinterpret_cast<const char*>(m_data), length);
    guard.finalize(true);
    user->Send(guard);
}
void CItemDictionary::setGainItemFlag(CUser*, unsigned int item) { if (valid(item)) m_data[item / kItemDictionaryBit] |= static_cast<unsigned char>(1u << (item & 7)); }
void CItemDictionary::setItemCheckFlag(CUser*, unsigned int item, unsigned int flag) { if (valid(item) && flag < kItemDictionaryBit) m_data[item / kItemDictionaryBit] |= static_cast<unsigned char>(1u << flag); }
