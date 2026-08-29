// df_game_r 还原 —— SPremiumInfo / g_SPremiumInfo 权威定义。
//
// ORIG 证据（df_game_r elf32-i386）：
//   g_SPremiumInfo                B 0x09483a40
//   _ZN12SPremiumInfoC1Ev         W 0x086b16d4（vector<uint>@+0x4c、
//                                 _Additioal_info@+0xc8、std::string@+0x11c）
//   _ZN12SPremiumInfoD1Ev         W 0x086b1732（逆序析构 string/vector/_Additioal_info）
//   _GLOBAL__I_g_SPremiumInfo     t 0x086b080a → __static_initialization_and_destruction_0
//                                 (0x86b06b7)：从 0x9483a40 起以步长 0x120 构造
//                                 0x6a 个元素（循环计数 0x69→-1），共 0x6a*0x120=0x7740 字节。
//
// 字段偏移证据（均为 g_SPremiumInfo[type] 元素内偏移，直接取自反汇编 disp）：
//   +0x00  int    类别标志：==1 计入 normal 池（RemovePremium 0x86adba6 /
//                 startPremium 0x86adc6a / ReCalcAdvantage 0x86ae10e cmp $1）
//   +0x04  int    时间窗检查门：非 0 时 startPremium/CheckPremiumTimeout(0x86add9c)
//                 校验用户 start/end 与 curSec（LoadPremiumInfo 写入）
//   +0x08  int    时间边界秒（premium_helper_function::cal_premium_term /
//                 CCeraShop_Special 以 start<val 截断 end）
//   +0x0c  5×8B   特权物品对 (itemIndex,count)：GetAdvantageItem 0x86adf7c
//                 读 a4c/a50 scaled（i=0..4 → +0xc/+0x10 + i*8）
//   +0x34  u16    疲劳加成：ReCalcAdvantage += user 0x850(outFatigueRate)
//   +0x38  int    ReCalcAdvantage += user 0x854(outPremiumCount)；
//                 CHandlePremium::handleSetUserPremium 作为加币值读取（双用途推断）
//   +0x3c  int    经验加成：ReCalcAdvantage += user 0x858(outExpRate)
//   +0x40  u8     每日检查标志：ReCalcAdvantage 非 0 → user 0x85c=1
//                 （LoadPremiumInfo movzbl/cl 写 a80）
//   +0x44  int    LoadPremiumInfo 写入（-0x7dc8 源，语义未知）
//   +0x48  int    LoadPremiumInfo 写入（-0x7dc4 源，语义未知）
//   +0x4c  vector<unsigned int> 独立掉落率表（GetAddIndependentDropRate 0x830ed48
//                 取 &元素+0x4c 传 vector operator[]；LoadPremiumInfo vector::operator= 拷贝）
//   +0x58  u16    overSkillLevel 来源：ReCalcAdvantage → user 0x85e
//   +0x5a  u16    ReCalcAdvantage → user 0x860
//   +0x5c  int    目标服务器开关：==1 时 serverCode = *(int*)(G_CEnvironment()+0x378)
//                 （get_target_server 0x827d0f5 / CCeraShop::_ClosePremium）
//   +0x60  u8     overEquip 标志：ReCalcAdvantage 非 0 → user 0x8bc=1
//   +0x64  int    LoadPremiumInfo 写入（语义未知）
//   +0x68  5×8B   LoadPremiumInfo 循环写入的 (int,int) 对（源 record+0x60..，
//                 语义推断：返还物品对，参照 GetReturnItemRate 域）
//   +0x90  int    LoadPremiumInfo 写入（语义未知）
//   +0x94  3×8B   LoadPremiumInfo 循环写入的 (int,int) 对（源 record+0x90..）
//   +0xac  7×int  生效 yday 掩码；[0]==-1 表示每天（_CheckApply/AddPremium 读 aec）
//   +0xc8  _Additioal_info(0x52)（LoadPremiumInfo 另有 +0xc8/+0xcc int、
//                 +0xd0..+0xd6/+0xd8..+0xdf/+0xe0..+0x100 u16、+0x102 int、
//                 +0x106..+0x10e u16 子字段写入，类型保持不透明）
//   +0x11c std::string 名称（ORIG 旧 ABI 4 字节 COW 指针）
//   总计 0x120。
//
// respond_special_item_premium(0x827d3dc) 本身经 g_map_premium_item_data
// （0x948b198, map<unsigned long,STPremiumItemData>）查询，不经本表直访。

#ifndef DNF_GAME_SPREMIUMINFO_H_
#define DNF_GAME_SPREMIUMINFO_H_

#include <string>
#include <vector>

#include "CCharacter.h"   // _Additioal_info（0x52，memset 清零）

class SPremiumInfo
{
public:
    // 8 字节 (int,int) 对（GetAdvantageItem 以 std::pair<int,int> 消费）。
    struct ItemPair
    {
        int first;
        int second;
    };

    int m_kind;                        // +0x00 类别标志（==1 → normal 池）
    int m_checkTerm;                   // +0x04 时间窗检查门
    int m_timeLimit;                   // +0x08 时间边界秒
    ItemPair m_advantageItems[5];      // +0x0c 特权物品 (itemIndex,count)
    unsigned short m_fatigueRate;      // +0x34
    int m_premiumCount;                // +0x38
    int m_expRate;                     // +0x3c
    unsigned char m_needDailyCheck;    // +0x40
    int m_field44;                     // +0x44
    int m_field48;                     // +0x48
    std::vector<unsigned int> m_independentDropRates;  // +0x4c
    unsigned short m_overSkillLevel;   // +0x58
    unsigned short m_field5a;          // +0x5a
    int m_useTargetServer;             // +0x5c
    unsigned char m_overEquipFlag;     // +0x60
    int m_field64;                     // +0x64
    ItemPair m_returnPairs68[5];       // +0x68
    int m_field90;                     // +0x90
    ItemPair m_returnPairs94[3];       // +0x94
    int m_applyDayMask[7];             // +0xac（[0]==-1 → 每天）
    _Additioal_info m_additionalInfo;  // +0xc8
    char m_pad11a[2];                  // +0x11a
    std::string m_name;                // +0x11c

};

// sizeof == 0x120 编译期断言（C++98，类外完成型检查）。
typedef char spremiuminfo_size_0x120[(sizeof(SPremiumInfo) == 0x120) ? 1 : -1];

// 元素数 0x6a（合法 type 下标 0..0x69，CheckPremiumTimeout 循环 1..0x69）。
extern SPremiumInfo g_SPremiumInfo[0x6a];   // B 0x09483a40；全局 C 名（无命名空间，ORIG B），共 0x7740 字节

#endif  // DNF_GAME_SPREMIUMINFO_H_
