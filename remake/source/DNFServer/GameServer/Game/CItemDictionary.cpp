#include "CItemDictionary.h"
#include <cstring>

namespace {
const unsigned int kBinaryLength = 5000;
const unsigned int kHeaderLength = 5;
inline bool valid(unsigned int item) { return item < kBinaryLength * 8; }
}

CItemDictionary::CItemDictionary() { _reset(); }
CItemDictionary::~CItemDictionary() {}
void CItemDictionary::_reset() { std::memset(m_data + kHeaderLength, 0, kBinaryLength); m_gainItemList.clear(); }
int CItemDictionary::_saveData(CUser*) { return 1; }
void CItemDictionary::getData(char* out) const { if (out) std::memcpy(out + 4, m_data + kHeaderLength, kBinaryLength); }
int CItemDictionary::getGainItemCount() const {
    int count = 0;
    for (unsigned int i = 0; i < kBinaryLength; ++i)
        for (unsigned int bit = 0; bit < 8; ++bit) count += (m_data[kHeaderLength + i] >> bit) & 1;
    return count;
}
std::vector<int>& CItemDictionary::getGainItemList() {
    m_gainItemList.clear();
    for (unsigned int i = 0; i < kBinaryLength; ++i)
        for (unsigned int bit = 0; bit < 8; ++bit)
            if ((m_data[kHeaderLength + i] >> bit) & 1) m_gainItemList.push_back(static_cast<int>(i * 8 + bit));
    return m_gainItemList;
}
bool CItemDictionary::getItemCheckFlag(unsigned int item, unsigned int flag) const {
    return valid(item) && flag < 8 && ((m_data[kHeaderLength + item / 8] >> flag) & 1) != 0;
}
int CItemDictionary::getLengthBinary() const { return kBinaryLength; }
int CItemDictionary::loadData(CUser*, char* data) { if (!data) return 0; std::memcpy(m_data + kHeaderLength, data + 4, kBinaryLength); return 1; }
int CItemDictionary::sendItemDictionaryInfo(CUser*, unsigned int) const { return 1; }
void CItemDictionary::sendItemDictionaryInfoTotal(CUser*) const {}
void CItemDictionary::setGainItemFlag(CUser*, unsigned int item) { if (valid(item)) m_data[kHeaderLength + item / 8] |= static_cast<unsigned char>(1u << (item & 7)); }
void CItemDictionary::setItemCheckFlag(CUser*, unsigned int item, unsigned int flag) { if (valid(item) && flag < 8) m_data[kHeaderLength + item / 8] |= static_cast<unsigned char>(1u << flag); }
