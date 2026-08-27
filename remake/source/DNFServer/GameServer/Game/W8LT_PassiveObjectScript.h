// ============================================================================
// W8LT_PassiveObjectScript.h — STPassiveObjectScript 及其子对象权威布局
//
// 来源（df_game_r，elf32-i386）：
//   STPassiveObjectScript::STPassiveObjectScript()  T/W 0x08a3a538
//     （docs/class_func_reports/STPassiveObjectScript/STPassiveObjectScript.md）
//   STPassiveObjectScript::clear()                  W    0x08a3a9e6
//     （docs/class_func_reports/STPassiveObjectScript/clear.md）
// 各子对象构造地址按 ctor 内 call 序逐项核对（nm 符号别名已解析）。
// 本头为本批次（W8_LongTail LT06 巨型脚本导入）新建，此前源码无任何
// STPassiveObjectScript / 子类型声明（grep Game/*.h *.cpp 为空）。
//
// 注意：本工程 32 位 gcc4.4 COW STL——sizeof(std::string)==4、
// sizeof(std::vector<T>)==0xc；ORIG clear() 中相邻写入（如 +0x17c string 与
// +0x180 vector）互不重叠，即以此为前提推导的全部偏移自洽。
// ============================================================================
#ifndef W8LT_PASSIVE_OBJECT_SCRIPT_H_
#define W8LT_PASSIVE_OBJECT_SCRIPT_H_

#include <string>
#include <vector>

// ---- WarningDistance（ORIG ctor 0x08a3aeca / ctor(i,i,i) 0x08a3aeec；
//      docs/class_func_reports/WarningDistance.md：三个 int 直赋）----
class WarningDistance
{
public:
    int m_a;   // +0x00
    int m_b;   // +0x04
    int m_c;   // +0x08
};  // sizeof == 0xc（ORIG 相邻实例步进 0x1d4→0x1e0）

// ---- SpeedCtrlPassiveObjInfo（ORIG ctor 0x08a3ad5a：+0=-1、+4 byte=0）----
class SpeedCtrlPassiveObjInfo
{
public:
    int m_field0;         // +0x00（ctor 初始化 -1）
    unsigned char m_b4;   // +0x04（ctor 初始化 0）

private:
    char m_tail_[7];      // [推断] ORIG 元素尺寸下界 5，按容器 stride 推得 0xc

public:
    SpeedCtrlPassiveObjInfo() : m_field0(-1), m_b4(0) {}
};

// ---- PassiveIObjSniperInfo（ORIG ctor 0x08a3af0c / clear 0x08a3afc0，
//      docs/class_func_reports/PassiveIObjSniperInfo/：4 × vector<int> +
//      int 标量区至 +0x74）----
class PassiveIObjSniperInfo
{
public:
    void clear();                        // ORIG W 0x08a3afc0
    PassiveIObjSniperInfo();             // ORIG W 0x08a3af0c

    std::vector<int> m_vecMinDist;       // +0x00
    std::vector<int> m_vecMaxDist;       // +0x0c
    std::vector<int> m_vecField18;       // +0x18
    std::vector<int> m_vecField24;       // +0x24
    int m_i30;                           // +0x30（clear 清零）
    int m_i34;                           // +0x34
    int m_i38;                           // +0x38（clear 循环 (i+0x10)*4+4 写段内含）
    int m_i44;                           // +0x44
    int m_i48;                           // +0x48
    int m_i4c;                           // +0x4c
    int m_i50;                           // +0x50
    int m_i54;                           // +0x54
    int m_i58;                           // +0x58
    int m_i5c;                           // +0x5c

    int m_i60;                           // +0x60
    int m_i64;                           // +0x64
    int m_i68;                           // +0x68
    int m_i6c;                           // +0x6c
    int m_i70;                           // +0x70

private:
    char m_tail_[0x10];                  // [推断] 使 sizeof==0x84（主类下一个
                                         // 成员 +0x270 int 紧随其后于 +0x270，
                                         // 即本子对象占 0x1ec..0x270）
};

// ---- PassiveObjGeneratorInfo（ORIG ctor 0x08a3e8ae；占 0x1b0..0x1d4=0x24）----
// 内部含三个容器起点由主类 ctor 的 vector 构造序列给出（pattern/sequence/
// object 三 vector 分别位于主类 +0x18c/+0x198/+0x1a4），此处仅承载
// GeneratorInfo 自身标量。内部布局 [推断]（标量 + 两个 int）。
class PassiveObjGeneratorPattern;
class PassiveObjGeneratorSequence;
class PassiveObjGeneratorObject;

class PassiveObjGeneratorInfo
{
public:
    PassiveObjGeneratorInfo();           // ORIG W 0x08a3e8ae

private:
    struct PatternStub { int m_v[3]; };  // 占位推导用 [推断]
    PatternStub m_stub_;                 // [推断] 对齐 0x24 总尺寸

public:
    int m_i20;                           // +0x20
};

// ---- STPassiveObjectAdditionalParticleInfo（仅前向声明，动态分配）----
struct STPassiveObjectAdditionalParticleInfo;

// ---- STPassiveObjectDestroy（ORIG ctor 0x08a3d926 / clear 0x08a3d8c2 /
//      dtor 0x08a3e04c；占 +0xd8..+0x128 = 0x50）----
class STPassiveObjectDestroy
{
public:
    void clear();                        // ORIG W 0x08a3d8c2
    STPassiveObjectDestroy();            // ORIG W 0x08a3d926
    ~STPassiveObjectDestroy();           // ORIG W 0x08a3e04c

    int m_type;                          // +0x00（clear=-1）
    int m_flag04;                        // +0x04（clear=0）
    std::vector<int> m_ints08;           // +0x08
    std::vector<std::string> m_strs14;   // +0x14
    std::vector<std::string> m_strs20;   // +0x20
    std::vector<int> m_ints2c;           // +0x2c
    int m_i38;                           // +0x38（clear=-1）
    int m_i3c;                           // +0x3c（clear=-1）
    int m_i40;                           // +0x40（clear=-1）
    std::vector<int> m_ints44;           // +0x44（ctor 构造，clear 未清）
};  // sizeof == 0x50

// ---- STPassiveObjectAddEffect（ORIG ctor 0x08a3da0c / clear 0x08a3da06 空体；
//      占 +0x128..+0x140 = 0x18）----
class STPassiveObjectAddEffect
{
public:
    void clear() {}                      // ORIG W 0x08a3da06 空体（忠实转写）
    STPassiveObjectAddEffect();          // ORIG W 0x08a3da0c

    std::vector<std::string> m_strs00;   // +0x00
    std::vector<int> m_ints0c;           // +0x0c
};  // sizeof == 0x18

// ---- RandomItemFairy（ORIG vector<RandomItemFairy> ctor 0x08a41110；
//      完整定义由 W8LT_PassiveObjectImport.cpp 承载，本头仅前置声明）----
class RandomItemFairy;

// ---- STPassiveObjectScript 主类 ----
// 字段语义待 Import/exportPassiveObjectScript token 分支进一步具名化；
// 当前名称按偏移命名（m_b* byte / m_f* float / m_v* 容器），语义可证处换名。
class STPassiveObjectScript
{
public:
    STPassiveObjectScript();             // ORIG W 0x08a3a538
    void clear();                        // ORIG W 0x08a3a9e6

    int m_idx;                                  // +0x000（clear=0）
    std::string m_name;                         // +0x004
    std::string m_layer;                        // +0x008
    int m_width;                                // +0x00c
    int m_height;                               // +0x010
    int m_i14;                                  // +0x014
    int m_i18;                                  // +0x018（clear=1）
    int m_i1c;                                  // +0x01c
    int m_i20;                                  // +0x020
    int m_i24;                                  // +0x024
    std::vector<int> m_category;                // +0x028
    std::string m_s34;                          // +0x034
    std::vector<std::string> m_basicActions;    // +0x038
    std::string m_s44;                          // +0x044
    std::vector<std::string> m_etcActions;      // +0x048
    std::string m_s54;                          // +0x054
    unsigned char m_b58;                        // +0x058
    unsigned char m_b59;                        // +0x059
    unsigned char m_b5a;                        // +0x05a
    int m_i5c;                                  // +0x05c（clear=-1）
    unsigned char m_b60;                        // +0x060
    unsigned char m_b61;                        // +0x061
    unsigned char m_b62;                        // +0x062
    std::string m_s64;                          // +0x064
    std::string m_s68;                          // +0x068
    std::vector<std::string> m_attackInfos;     // +0x06c
    std::vector<int> m_intData;                 // +0x078
    std::vector<std::string> m_stringData;      // +0x084
    std::string m_s90;                          // +0x090
    int m_i94;                                  // +0x094（clear=-1）
    int m_i98;                                  // +0x098
    unsigned char m_b9c;                        // +0x09c
    int m_ia0;                                  // +0x0a0（clear=1）
    int m_ia4;                                  // +0x0a4
    int m_ia8;                                  // +0x0a8
    int m_iac;                                  // +0x0ac
    int m_ib0;                                  // +0x0b0（clear=500）
    unsigned char m_bb4;                        // +0x0b4
    int m_ib8;                                  // +0x0b8
    int m_ibc;                                  // +0x0bc
    float m_fc0;                                // +0x0c0（clear=180.0f）
    int m_ic4;                                  // +0x0c4
    int m_ic8;                                  // +0x0c8
    int m_icc;                                  // +0x0cc（clear=-1）
    int m_id0;                                  // +0x0d0
    unsigned char m_bd4;                        // +0x0d4
    unsigned char m_bd5;                        // +0x0d5
    STPassiveObjectDestroy m_destroy;           // +0x0d8（0x50）
    STPassiveObjectAddEffect m_addEffect;       // +0x128（0x18）
    std::vector<STPassiveObjectAdditionalParticleInfo*> m_particles;  // +0x140
    unsigned char m_b14c;                       // +0x14c
    unsigned char m_b14d;                       // +0x14d
    unsigned char m_b14e;                       // +0x14e
    unsigned char m_ub14f;                      // +0x14f（clear=0xff）
    unsigned short m_us150;                     // +0x150（clear=0xffff）
    std::string m_s154;                         // +0x154
    unsigned char m_b158;                       // +0x158
    unsigned char m_b159;                       // +0x159
    int m_i15c;                                 // +0x15c
    std::vector<int> m_onAttackData;            // +0x160
    unsigned char m_b16c;                       // +0x16c
    float m_f170;                               // +0x170（clear=1.0f）
    int m_i174;                                 // +0x174
    int m_i178;                                 // +0x178
    std::string m_s17c;                         // +0x17c
    std::vector<SpeedCtrlPassiveObjInfo> m_speedCtrls;              // +0x180
    std::vector<PassiveObjGeneratorPattern> m_genPatterns;          // +0x18c
    std::vector<PassiveObjGeneratorSequence> m_genSequences;        // +0x198
    std::vector<PassiveObjGeneratorObject> m_genObjects;            // +0x1a4
    PassiveObjGeneratorInfo m_genInfo;                              // +0x1b0（0x24）
    WarningDistance m_warnDanger;               // +0x1d4
    WarningDistance m_warnRecovery;             // +0x1e0
    PassiveIObjSniperInfo m_sniper;             // +0x1ec（0x84）
    int m_i270;                                 // +0x270（clear=2）
    std::vector<class RandomItemFairy> m_itemFairies;               // +0x274
    unsigned char m_b280;                       // +0x280
    std::string m_s284;                         // +0x284
};

#endif  // W8LT_PASSIVE_OBJECT_SCRIPT_H_
