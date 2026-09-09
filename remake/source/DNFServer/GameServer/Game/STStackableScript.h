#ifndef GAME_STSTACKABLESCRIPT_H_
#define GAME_STSTACKABLESCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STStackableScript（G5 堆叠物脚本表）
// 布局依据 ORIG Clear（0x0899087a）/ ctor（0x0898f218）/ copy（0x0898f8e6）/
// 析构（0x0899013c）逐成员推导；基类 STItemScript（0x178）。
// Arad_STStackableScript 为 +0x178 内嵌类。
// 子对象（STRecovery/STStatusRecovery/STBuff/STSummonMonster/STSummonApc/
// CountinuousItemOption/AvatarOptionChangeType）与 STStackableScript 同 TU
// （ORIG 0x089c30xx..0x089c33xx 弱符号紧邻本类），方法实现在 STStackableScript.cpp。
// CItemList.h 中的简化声明由本头替换。
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "STItemScript.h"

// StackableAction 枚举 / stStackableBooster_t / stLimitCubeInfo /
// BoosterSelectInfo / RecipeInfo 的唯一声明点在 CStackableItem.h
// （ORIG 同一二进制类型，禁止镜像重复定义）。
#include "CStackableItem.h"

// CRabbitBox 权威定义收敛到 AradRabbitBox.h（多态兔盒，W6_ARAD_ItemBonus 批次），
// 本头仅保留 m_rabbit19c 容器成员（@+0x19c）。
// STRecovery 权威定义（ORIG W 0x089c3098 C1 / 0x089c30ac clear；实现 STRecovery.cpp）。
#include "STRecovery.h"

// ---- STPassiveObjectInStackable

// ---- STPassiveObjectInStackable（ORIG C1/C2 W 0x089c34e8；无 D1/D2 →
//      析构 trivial，P12；sizeof 0x10）----
// C1 函数体逐句清零 +0x00/+0x04/+0x08/+0x0c（每字段重读 this，勿用初始化列表 P29）。
struct STPassiveObjectInStackable
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
    int m_fieldC;   // +0x0c
    int m_field10;  // +0x10

    STPassiveObjectInStackable()
    {
        m_field0 = 0;
        m_field4 = 0;
        m_field8 = 0;
        m_fieldC = 0;
        m_field10 = 0;
    }
};

// ---- STStatusGenerate（ORIG C1/clear W 0x089c3124/0x089c3166、
//      D1 W 0x089c3aba / op= W 0x089c40bc，全 W → 头内联）----
// 布局：vector<float>@+0x00 + int@+0x0c + int@+0x10 + byte@+0x14，sizeof 0x18。
// C1：vector ctor 后调 clear；clear：+0x0c/+0x10 清零 → vector::clear → +0x14 清零；
// D1 仅 vector<float> 析构；op= = vector op= + 三标量拷贝，返回 this。
struct STStatusGenerate
{
    std::vector<float> m_vec0;   // +0x00
    int m_fieldC;                // +0x0c
    int m_field10;               // +0x10
    unsigned char m_field14;     // +0x14

    STStatusGenerate() : m_vec0() { clear(); }

    void clear()
    {
        m_fieldC = 0;
        m_field10 = 0;
        m_vec0.clear();
        m_field14 = 0;
    }

    ~STStatusGenerate() {}

    STStatusGenerate& operator=(const STStatusGenerate& o)
    {
        m_vec0 = o.m_vec0;
        m_fieldC = o.m_fieldC;
        m_field10 = o.m_field10;
        m_field14 = o.m_field14;
        return *this;
    }
};   // sizeof 0x18

// ---- STStatChange（ORIG C1 0x089c328a / clear 0x089c329e / D1 0x089d3f1a）----
// clear 次序：+0x0c int=0 → +0x00=0 → +0x04=0 → +0x08 字节=0。sizeof 0x10。
class STStatChange
{
public:
    STStatChange();
    void clear();

    int m_field0;       // +0x00
    int m_field4;       // +0x04
    unsigned char m_field8;   // +0x08
    char m_pad9[3];     // +0x09..+0x0b
    int m_fieldC;       // +0x0c
};   // sizeof 0x10

// ---- STStatChangeVector（ORIG C1 0x089c32c8 / clear 0x089c330a /
//      D1 0x089c3ace / operator= 0x089c40fe）----
// C1 = vector ctor 后 clear；clear：vector.clear、+0x0c/+0x10 int=0、
// +0x14 字节=0；operator= = vector operator= + 三标量拷贝。sizeof 0x18。
class STStatChangeVector
{
public:
    STStatChangeVector();
    ~STStatChangeVector();
    STStatChangeVector& operator=(const STStatChangeVector& other);
    void clear();

    std::vector<STStatChange> m_vec0;   // +0x00
    int m_fieldC;                       // +0x0c
    int m_field10;                      // +0x10
    unsigned char m_field14;            // +0x14
    char m_pad15[3];                    // +0x15..+0x17
};   // sizeof 0x18


// ---- STStatusRecovery（ORIG W 0x089c30dc C1 / 0x089c30f0 clear；sizeof 0x14）----
// clear：4×int + byte 逐句清零（每字段重读 this）。trivial 析构（无 D1）。
struct STStatusRecovery
{
public:
    STStatusRecovery();
    void clear();

    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldC;       // +0x0c
    unsigned char m_field10;  // +0x10
};   // sizeof 0x14

// ---- STBuff（ORIG W 0x089c3194 C1 / 0x089c31a8 clear；sizeof 0x10）----
// clear：3×int + byte 清零。trivial 析构。
struct STBuff
{
public:
    STBuff();
    void clear();

    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    unsigned char m_fieldC;  // +0x0c
};   // sizeof 0x10

// ---- STSummonMonster（ORIG W 0x089c31d2 C1 / 0x089c31e6 clear；sizeof 0x1c）----
// clear：6×int（+0x00..+0x14，ORIG 对 +0x14 连写两次 movl $0）+ byte@+0x18。trivial 析构。
struct STSummonMonster
{
public:
    STSummonMonster();
    void clear();

    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldC;       // +0x0c
    int m_field10;      // +0x10
    int m_field14;      // +0x14
    unsigned char m_field18;  // +0x18
};   // sizeof 0x1c

// ---- STSummonApc（ORIG W 0x089c3238 C1 / 0x089c324c clear；sizeof 0x18）----
// clear：5×int + byte 清零。trivial 析构。
struct STSummonApc
{
public:
    STSummonApc();
    void clear();

    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldC;       // +0x0c
    int m_field10;      // +0x10
    unsigned char m_field14;  // +0x14
};   // sizeof 0x18

// ---- CountinuousItemOption / AvatarOptionChangeType / STEnchantSystemMapKey /
//      STEnchantSystemMapData 权威定义已迁至 CStackableItem.h（CStackableItem
//      +0x2d8/+0x2e4/+0x365/+0x2f4 成员子对象类型；本头 include CStackableItem.h
//      即可见），本处删除防重复定义。STEnchantSystemData 基类经
//      STEquipmentScript.h（本头 include STItemScript.h → EquipmentParameterInfo.h）可见。
enum ENUM_MODULE_TYPE
{
    ENUM_MODULE_TYPE_0 = 0
};

class EquipmentParameterInfo;
class STStackableScript;

class Arad_STStackableScript
{
public:
    Arad_STStackableScript();
    ~Arad_STStackableScript();
    Arad_STStackableScript& operator=(const Arad_STStackableScript& other);
    void AradStClear();

    int m_0;          // +0x00
    char m_4;         // +0x04（-1）
    std::string m_str8;   // +0x08
    std::string m_strc;   // +0x0c
    std::string m_str10;  // +0x10
    int m_field14;    // +0x14（AradStClear 不清、op= 不拷；由 STStackableScript::Clear 清）
};

class STStackableScript : public STItemScript
{
public:
    STStackableScript();
    STStackableScript(STStackableScript& other);
    ~STStackableScript();
    STStackableScript& operator=(const STStackableScript& other);
    void Clear();

    Arad_STStackableScript m_arad178;                 // +0x178
    std::vector<int> m_vec190;                        // +0x190
    std::vector<CRabbitBox> m_rabbit19c;              // +0x19c
    int m_1a8;                                        // +0x1a8
    int m_1ac;                                        // +0x1ac
    int m_1b0;                                        // +0x1b0
    int m_1b4;                                        // +0x1b4（-1）
    char m_1b8;                                       // +0x1b8
    CountinuousItemOption m_option1b9;                // +0x1b9（3 字节）
    CountinuousItemOption m_option1bc;                // +0x1bc（3 字节）
    char m_1bf;                                       // +0x1bf
    char m_1c0;                                       // +0x1c0
    float m_1c4;                                      // +0x1c4（ORIG Clear 0x089909e1
                                                      //   `mov $0,%edx; mov %edx` 常量中转
                                                      //   = P238 float 字段形；且
                                                      //   ImportStackableScript 写
                                                      //   ScanFloat 返回值，真型为 float）
    char m_1c8;                                       // +0x1c8
    float m_1cc;                                      // +0x1cc（同 0x089909f9）
    char m_1d0;                                       // +0x1d0
    int m_1d4;                                        // +0x1d4
    std::string m_str1d8;                             // +0x1d8
    int m_1dc;                                        // +0x1dc
    int m_1e0;                                        // +0x1e0（0x27）
    int m_1e4;                                        // +0x1e4
    int m_1e8;                                        // +0x1e8
    std::vector<int> m_vec1ec;                        // +0x1ec
    std::vector<StackableAction::UsablePlace> m_place1f8;  // +0x1f8
    char m_204;                                       // +0x204
    char m_205;                                       // +0x205
    std::vector<STPassiveObjectInStackable> m_passive208;  // +0x208
    std::vector<StackableAction::UsableServerType> m_server214;  // +0x214
    int m_220;                                        // +0x220
    std::vector<int> m_vec224;                        // +0x224
    std::string m_str230;                             // +0x230
    std::string m_str234;                             // +0x234
    std::string m_str238;                             // +0x238
    int m_23c;                                        // +0x23c（1）
    int m_240;                                        // +0x240（-1）
    std::vector<std::vector<int> > m_vec244;          // +0x244
    std::vector<int> m_vec250;                        // +0x250
    std::vector<int> m_vec25c;                        // +0x25c
    std::vector<STStackablePackageData> m_package268; // +0x268
    std::vector<std::vector<STStackablePackageData> > m_package274;  // +0x274
    stStackableBooster_t m_booster280;                // +0x280
    std::vector<STStackablePackageData> m_package2a4; // +0x2a4
    STRecovery m_recovery2b0;                         // +0x2b0
    STRecovery m_recovery2c4;                         // +0x2c4
    STStatusRecovery m_statusRecovery2d8;             // +0x2d8
    STStatusGenerate m_status2ec;                     // +0x2ec
    STBuff m_buff304;                                 // +0x304
    STSummonMonster m_summonMonster314;               // +0x314
    STSummonApc m_summonApc330;                       // +0x330
    int m_348;                                        // +0x348（-1）
    STStatChangeVector m_statChange34c;               // +0x34c
    int m_364;                                        // +0x364（9999999）
    std::string m_str368;                             // +0x368
    int m_36c;                                        // +0x36c（-1）
    int m_370;                                        // +0x370（-1）
    std::vector<std::pair<int, int> > m_vec374;       // +0x374
    unsigned short m_380;                             // +0x380
    int m_384;                                        // +0x384
    char m_388;                                       // +0x388（1）
    int m_38c;                                        // +0x38c（-1）
    int m_390;                                        // +0x390（1）
    unsigned short m_394;                             // +0x394
    char m_396;                                       // +0x396
    unsigned short m_398;                             // +0x398
    char m_39a;                                       // +0x39a
    char m_39b;                                       // +0x39b
    int m_39c;                                        // +0x39c
    char m_3a0;                                       // +0x3a0
    char m_3a1;                                       // +0x3a1
    std::map<int, int> m_map3a4;                      // +0x3a4
    int m_3bc;                                        // +0x3bc
    std::vector<std::pair<int, int> > m_vec3c0;       // +0x3c0
    int m_3cc;                                        // +0x3cc
    char m_3d0;                                       // +0x3d0
    char m_3d1;                                       // +0x3d1（1）
    stLimitCubeInfo m_cube3d4;                        // +0x3d4
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData> m_enchant3f4;  // +0x3f4
    char m_40c;                                       // +0x40c（0xff）
    char m_40d[2];                                    // +0x40d（memset 0）
    std::string m_str410;                             // +0x410
    char m_414;                                       // +0x414
    int m_418;                                        // +0x418
    BoosterSelectInfo m_boosterSelect41c;             // +0x41c
    char m_438;                                       // +0x438
    std::pair<int, int> m_pair43c;                    // +0x43c
    int m_444;                                        // +0x444（0xb）
    int m_448;                                        // +0x448
    int m_44c;                                        // +0x44c（-1）
    int m_450;                                        // +0x450（-1）
    int m_454;                                        // +0x454（-1）
    int m_458;                                        // +0x458（-1）
    int m_45c;                                        // +0x45c（-1）
    int m_460;                                        // +0x460（-1）
    int m_464;                                        // +0x464（-1）
    int m_468;                                        // +0x468（-1）
    int m_46c;                                        // +0x46c
    int m_470;                                        // +0x470（-1）
    int m_474;                                        // +0x474
    int m_478;                                        // +0x478
    AvatarOptionChangeType m_avatar47c;               // +0x47c（3 字节）
    std::set<int> m_set480;                           // +0x480
    std::set<int> m_set498;                           // +0x498
    char m_4b0;                                       // +0x4b0
    std::string m_str4b4;                             // +0x4b4
    int m_4b8;                                        // +0x4b8
    std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*> m_module4bc;  // +0x4bc
    std::set<int> m_set4d4;                           // +0x4d4
    int m_4ec;                                        // +0x4ec
    int m_4f0;                                        // +0x4f0（RecipeInfo additionalInfo 源1）
    int m_4f4;                                        // +0x4f4（源2）
    int m_4f8;                                        // +0x4f8（源3）
    std::vector<int> m_vec4fc;                        // +0x4fc
    int m_508;                                        // +0x508
    std::set<int> m_set50c;                           // +0x50c
    // ORIG B 0x94fac20 [0x2ee0]：pricetable.tbl 30×100 int 静态表
    // （InitStackableScript 0x089ae72e 清零回填；定义 InitScriptProducers_4.cpp）。
    static int priceTable_[30][100];
};

bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx);
bool StackableScriptSpecialMonsterDrop(STStackableScript* script,
                                       std::string str, const char* path);

#endif  // GAME_STSTACKABLESCRIPT_H_
