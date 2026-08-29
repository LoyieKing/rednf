// ============================================================================
// df_game_r 还原 —— PremiumServiceData（OrigPremiumData 批独占）
// 布局依据（objdump 实证，报告缺失按 §3.2 兜底）：
//   C1 0x0891bb5e：pair<int,int> C1 @+0x04 → Ss C1 @+0x14 →
//                  vector<int> C1 @+0x18 → map<int,pair<bool,int>> C1 @+0x24
//   随后 +0x00=0 / +0x04..+0x08=-1 / +0x0c=-1 / +0x10=-1 / +0x14="" /
//   +0x18 vector.clear()。
//   new_allocator::construct 0x08931864：placement new 大小 0x3c → sizeof=0x3c。
//   字段语义见 importPremiumServiceData 报告（Etc/PremiumServiceEffect.etc）：
//   [type]0x3059→+0x00、[main premium image index]0x306e→+0x04/+0x08、
//   [additional premium image index]0x3072→+0x0c、[icon index]0x305c→+0x10、
//   [icon explain]0x305f→+0x14(string)、[add equipment list]0x3062→+0x18(vector<int>)、
//   [add selectAble equipment list]0x3076→+0x24(map<int,pair<bool,int>>)。
// ORIG 符号：C1/C2 0x0891bb5e、C1RKS/C2RKS 0x089317a0、D1/D2 0x0891ca96、
//   aS 0x089318e8、clear 0x0891bc4a（均 W）。
// ============================================================================
#ifndef DNF_GAME_PREMIUMSERVICEDATA_H_
#define DNF_GAME_PREMIUMSERVICEDATA_H_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <cstddef>

class PremiumServiceData
{
public:
    PremiumServiceData();                                     // W 0x0891bb5e
    PremiumServiceData(PremiumServiceData const&);            // W 0x089317a0
    ~PremiumServiceData();                                    // W 0x0891ca96
    PremiumServiceData& operator=(PremiumServiceData const&); // W 0x089318e8
    void clear();                                             // W 0x0891bc4a

    int m_type;                                    // +0x00 [type]
    std::pair<int, int> m_mainImageIndex;          // +0x04/+0x08 [main premium image index]
    int m_additionalImageIndex;                    // +0x0c [additional premium image index]
    int m_iconIndex;                               // +0x10 [icon index]
    std::string m_iconExplain;                     // +0x14 [icon explain]
    std::vector<int> m_addEquipmentList;           // +0x18 [add equipment list]
    std::map<int, std::pair<bool, int> >
        m_selectAbleEquipmentList;                 // +0x24 [add selectAble equipment list]
};

static_assert(sizeof(PremiumServiceData) == 0x3c, "PremiumServiceData size mismatch");
#endif  // DNF_GAME_PREMIUMSERVICEDATA_H_
