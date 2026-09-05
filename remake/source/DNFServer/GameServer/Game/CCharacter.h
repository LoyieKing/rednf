// df_game_r 还原 —— CCharacter 类声明（独立 TU）。
// 权威符号（ORIG df_game_r）：
//   CCharacter::CCharacter()      T @0x0834828c
//   CCharacter::~CCharacter()     T @0x0834842a
//   CCharacter::get_give_skill…  T @0x08348798
// 布局依据 CCharacter/CCharacter.md 与 ~CCharacter.md 反编译：
//   +0x000 _Additioal_info[7]（各 0x52）
//   +0x240 vector<pair<int,int>>[7]（各 0xc）
//   +0x294 STSecondGrowTypeInfo[7]（步长 0xc0）
#ifndef DNF_GAME_CCHARACTER_H_
#define DNF_GAME_CCHARACTER_H_

#include <string.h>
#include <utility>
#include <vector>

// ORIG 各 TU 一份 _ZL21MAX_FIRST_GROWTH_TYPE（r 0x1）：成长类型槽位数（m_growTypeFlag 维度）。
static const unsigned char MAX_FIRST_GROWTH_TYPE = 8;

// ORIG df_game_r 符号：C1/C2 T 0x8348254（→clear）；clear T 0x8348268（memset(this,0,0x52)）。
// 字段布局由 _ZN15_Additioal_info3addERKS_（W 0x08146300）逐字段算术偏移与
// WongWork::CCalcCharacStat::InitializeStat（T 0x0831e9ca）的写入偏移共同证实；
// ORIG int 字段位于 0x3a/0x42/0x4e 非自然对齐位置（add/subtract 以未对齐 movl 访问），
// 必须 pack(1) 才能精确镜像 0x52 布局（仓库惯例同 AvatarItem.h/CBoosterGage.h）。
#pragma pack(push, 1)
struct _Additioal_info
{
    _Additioal_info() { clear(); }   // ORIG C1 调 clear()（0x8348254→clear 0x8348268）
    void clear() { memset(this, 0, 0x52); }

    // ORIG W 内联三件套；本工程收敛为单点强符号，定义见 CUserPremium.cpp。
    void add(const _Additioal_info& other);         // ORIG W 0x08146300
    void subtract(const _Additioal_info& other);    // ORIG W 0x08693d04
    void operator+=(const _Additioal_info& other);  // ORIG W 0x08693cea（仅转发 add）

    // u16 字段在 add/subtract 中为零扩展后 mod 2^16 运算，
    // 用 unsigned short 保证无符号回绕良构、与 movzwl 指令语义一致。
    // 2026-09-05 §4.3（DBCharac 批次）：charac_info DB 列绑定别名——get_* 引用
    // 形参类型即 ORIG 源字段类型实证（DB_LoadCharac::_getCharacInfo/_GetTagCharacInfo
    // 报告 callee：get_uint@0x080e22f2 / get_short@0x080fddec）；旧名旧类型保留，
    // add/subtract/InitializeStat 消费者零改动，union 两侧同宽 sizeof 不变。
    union { int            m_field00;      // +0x00（InitializeStat 基础值，随职业变化）
                   unsigned int    m_maxHP;   // +0x00 = charac_info.maxHP（get_uint）
    };
    union { int            m_field04;      // +0x04
                   unsigned int    m_maxMP;   // +0x04 = charac_info.maxMP（get_uint）
    };
    unsigned short m_field08;      // +0x08
    unsigned short m_field0a;      // +0x0a
    unsigned short m_field0c;      // +0x0c
    unsigned short m_field0e;      // +0x0e
    union { unsigned short m_field10[4];   // +0x10..+0x16（add/subtract 循环 i=0..3）
                   char            m_elementResist[8]; // +0x10 element_resist（get_binary 8）
    };
    union { unsigned short m_field18[17];  // +0x18..+0x38（循环 j=0..16）
                   char            m_specProperty[0x22]; // +0x18 spec_property（get_binary 0x22）
    };
    int            m_field3a;      // +0x3a
    union { unsigned short m_field3e;      // +0x3e
                   short            m_hpRegen;  // +0x3e hp_regen（get_short short&）
    };
    union { unsigned short m_field40;      // +0x40
                   short            m_mpRegen;  // +0x40 mp_regen（get_short short&）
    };
    union { int            m_field42;      // +0x42
                   unsigned int     m_moveSpeed; // +0x42 move_speed（get_uint）
    };
    unsigned short m_field46;      // +0x46
    unsigned short m_field48;      // +0x48
    union { unsigned short m_field4a;      // +0x4a
                   short            m_hitRecovery; // +0x4a hit_recovery（get_short short&）
    };
    union { unsigned short m_field4c;      // +0x4c（subtract 中 ORIG 仍执行加法）
                   short            m_jump;      // +0x4c jump（get_short short&）
    };
    int            m_field4e;      // +0x4e（subtract 中 ORIG 仍执行加法）
};
#pragma pack(pop)

// 布局自检（C++98）：锁死 0x52 ABI——嵌入 CCharacter[7]/CUser +0x90f/SPremiumInfo +0xc8。
typedef char _Additioal_info_size_0x52[(sizeof(_Additioal_info) == 0x52) ? 1 : -1];

// ORIG STSecondGrowTypeInfoC1 W 0x8373934 / D1 W 0x8376324：
//   2 个 _Additioal_info（0xa4）+ 2 个 vector（+0xa4/+0xb0，dtor 界 +0xbc）
//   + int @+0xbc（CCharacter 数组步长 0xc0 的收尾字段，推断为 POD 计数/标志）。
struct STSecondGrowTypeInfo
{
    STSecondGrowTypeInfo();
    ~STSecondGrowTypeInfo();
    void clear();   // ORIG _ZN20STSecondGrowTypeInfo5clearEv W 0x08373a1e

    _Additioal_info m_addInfo[2];                  // +0x00（0xa4）
    std::vector<std::pair<int, int> > m_skills[2]; // +0xa4（各 0xc）
    int m_fieldbc;                                 // +0xbc → 共 0xc0
};

struct STCharacterScript;  // 完整类型见 STCharacterScript.h（set_data 源表）

class CCharacter
{
public:
    CCharacter();
    ~CCharacter();

    void destroy();   // ORIG _ZN10CCharacter7destroyEv T 0x083485c4 → clear()
    void clear();     // ORIG _ZN10CCharacter5clearEv  T 0x0834852e
    void set_data(STCharacterScript* script);
                   // ORIG _ZN10CCharacter8set_dataEP17STCharacterScript T 0x08348894

    char get_give_skill(int firstGrow, int secondGrow,
                        std::vector<std::pair<int, int> >& out,
                        unsigned int flag) const;

    char get_give_skill_firstGrowtype(int grow,
                                      std::vector<std::pair<int, int> >& out) const;
    // secondGrow 1..2 → m_secondGrowTypes[grow+1].m_skills[secondGrow-1]。
    char get_give_skill_secondGrowtype(int grow, int secondGrow,
                                       std::vector<std::pair<int, int> >& out) const;
    // ORIG T 0x0834883e：按 (firstGrow,secondGrow) 返回成长属性块指针。
    // 返回块地址恒为 0x52 步长的 _Additioal_info（CalcStat.md 调用点直接以
    // _Additioal_info::add 消费；ORIG asm call 08146300 add 实参即此返回值），
    // 故原型具型为 const _Additioal_info*（mangled 不含返回类型，ABI 不变）。
    const _Additioal_info* get_growtype_stat(int firstGrow, int secondGrow) const;

public:  // ORIG SetSkillTree 0x84ef8cd 直访 m_characs[job]+0x240（m_giveSkillList[0]）
    _Additioal_info m_baseInfo[7];                 // +0x000..+0x23f
    std::vector<std::pair<int, int> > m_giveSkillList[7];  // +0x240..+0x293
private:
    STSecondGrowTypeInfo m_secondGrowTypes[7];     // +0x294..+0x7d3

    // +0x7d4..+0x7db：set_data（ORIG 0x08348ac7/0x08348e0c）按 grow 类型索引逐字节写入
    // this[idx + 0x7d4]（源 STCharacterScript 行内 +0x76）。CCharacList 以 0x7dc 步长
    // 遍历本类数组（CCharacList.md ctor/init/destroy），对象必须补足到 0x7dc。
    char m_growTypeFlag[MAX_FIRST_GROWTH_TYPE];
};

// 布局自检：锁死 ORIG 数组步长 ABI（CCharacList = CCharacter[10]，步长 0x7dc；
// CDataManager::m_characList 指向该数组基址，_update_charac_stat 按下标取行）。
namespace WongWork
{
// WongWork::CCalcCharacStat —— 全静态角色属性计算器（ORIG 7 方法均 T）。
// mangled（无 const/static 标记差异，返回类型不参与 mangling）：
//   _ZN8WongWork15CCalcCharacStat14InitializeStatEP15_Additioal_infoi      T 0x0831e9ca
//   _ZN8WongWork15CCalcCharacStat15ApplyElixerItemEP15_Additioal_infoii    T 0x0831e85c
//   _ZN8WongWork15CCalcCharacStat8CalcStatEP15_Additioal_infoiiiii         T 0x0831ec4a
//   _ZN8WongWork15CCalcCharacStat13_CalcSwordmanEP15_Additioal_infoii      T 0x0831ed6c
//   _ZN8WongWork15CCalcCharacStat12_CalcFighterEP15_Additioal_infoii       T 0x0831f282
//   _ZN8WongWork15CCalcCharacStat9_CalcMageEP15_Additioal_infoii           T 0x0831fc3a
//   _ZN8WongWork15CCalcCharacStat11_CalcGunnerEP15_Additioal_infoii        T 0x0831f75c
// 定义见 WongWorkMisc_A.cpp。
class CCalcCharacStat
{
public:
    static void InitializeStat(_Additioal_info* out, int job);

    static void ApplyElixerItem(_Additioal_info* out, int statIdx, int value);

    static _Additioal_info* CalcStat(_Additioal_info* out, int job,
                                     int firstGrow, int secondGrow,
                                     int level);

    // 第二参数为成长系类型（2nd grow subtype），第三参数为目标等级。
    static _Additioal_info* _CalcSwordman(_Additioal_info* out,
                                          int secondType, int level);
    static _Additioal_info* _CalcFighter(_Additioal_info* out,
                                         int secondType, int level);
    static _Additioal_info* _CalcMage(_Additioal_info* out,
                                      int secondType, int level);
    static _Additioal_info* _CalcGunner(_Additioal_info* out,
                                        int secondType, int level);
};
}  // namespace WongWork

typedef char CCharacter_size_0x7dc[(sizeof(CCharacter) == 0x7dc) ? 1 : -1];

#endif  // DNF_GAME_CCHARACTER_H_
