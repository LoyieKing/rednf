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

// ---- WarningDistance（ORIG C1/C1Eiii T 0x08a3aeca/0x08a3aeec；
//      docs/class_func_reports/WarningDistance.md：三个 int 直赋）----

class WarningDistance
{
public:
    WarningDistance();                    // ORIG T 0x08a3aeca（cpp 定义）
    WarningDistance(int a, int b, int c); // ORIG T 0x08a3aeec（cpp 定义）

    int m_a;   // +0x00
    int m_b;   // +0x04
    int m_c;   // +0x08
};  // sizeof == 0xc（ORIG 相邻实例步进 0x1d4→0x1e0）

// ---- SpeedCtrlPassiveObjInfo（ORIG ctor T 0x08a3ad5a：+0=-1、+4 byte=0；
//      D1/D2 W 0x08a4eab2 trivial——编译器隐式生成）----
class SpeedCtrlPassiveObjInfo
{
public:
    SpeedCtrlPassiveObjInfo();         // ORIG T 0x08a3ad5a（cpp 定义）

    int m_field0;         // +0x00（ctor 初始化 -1）
    unsigned char m_b4;   // +0x04（ctor 初始化 0）
    // 无显式尾填充：尾部为编译器隐式 padding（ORIG _Vector_base D1
    // `(end-start) sar $3` → stride 8 = sizeof 实证；export 08a2bc5c 以
    // `memcpy(&info, &vec[i], 5)` 拷贝 41-bit 类对象，声明显式 m_tail_ 会使
    // GCC 改发 8 字节逐字段 mov，故留隐式）。
};
static_assert(sizeof(SpeedCtrlPassiveObjInfo) == 8,
              "SpeedCtrlPassiveObjInfo ABI drift (ORIG stride 8)");

// ---- PassiveIObjSniperInfo（ORIG ctor 0x08a3af0c / clear 0x08a3afc0，
//      docs/class_func_reports/PassiveIObjSniperInfo/：4 × vector<int> +
//      int 标量区至 +0x74）----
// ---- PassiveIObjSniperInfo（ORIG C1/clear T 0x08a3af0c/0x08a3afc0，
//      D1/op= W 0x08a3e1c4/0x08a3e76a；
//      docs/class_func_reports/PassiveIObjSniperInfo/：4 × vector<int> +
//      int 标量区至 +0x7c；C1/clear 为 T 强符号 → cpp 定义）----
class PassiveIObjSniperInfo
{
public:
    void clear();                        // ORIG T 0x08a3afc0（cpp 定义）
    PassiveIObjSniperInfo();             // ORIG T 0x08a3af0c（cpp 定义）
    ~PassiveIObjSniperInfo() {}          // ORIG W 0x08a3e1c4：4 × vector dtor
    PassiveIObjSniperInfo& operator=(const PassiveIObjSniperInfo& o)
    {
        m_vecMinDist = o.m_vecMinDist;
        m_vecMaxDist = o.m_vecMaxDist;
        m_vecField18 = o.m_vecField18;
        m_vecField24 = o.m_vecField24;
        m_i30 = o.m_i30;
        m_i34 = o.m_i34;
        m_b38 = o.m_b38;
        m_i3c = o.m_i3c;
        m_b40 = o.m_b40;
        m_ints44[0] = o.m_ints44[0];
        m_ints44[1] = o.m_ints44[1];
        m_ints44[2] = o.m_ints44[2];
        m_ints44[3] = o.m_ints44[3];
        m_ints44[4] = o.m_ints44[4];
        m_ints44[5] = o.m_ints44[5];
        m_i5c = o.m_i5c;
        m_i60 = o.m_i60;
        m_i64 = o.m_i64;
        m_i68 = o.m_i68;
        m_i6c = o.m_i6c;
        m_i70 = o.m_i70;
        m_ints74[0] = o.m_ints74[0];
        m_ints74[1] = o.m_ints74[1];
        m_ints74[2] = o.m_ints74[2];
        return *this;
    }

    std::vector<int> m_vecMinDist;       // +0x00
    std::vector<int> m_vecMaxDist;       // +0x0c
    std::vector<int> m_vecField18;       // +0x18
    std::vector<int> m_vecField24;       // +0x24
    int m_i30;                           // +0x30（clear 清零）
    int m_i34;                           // +0x34（clear 清零）
    unsigned char m_b38;                 // +0x38（clear movb 清零）
    int m_i3c;                           // +0x3c（clear 清零）
    unsigned char m_b40;                 // +0x40（clear movb 清零）
    int m_ints44[6];                     // +0x44..+0x58（clear 循环 i=0..5 清零）
    int m_i5c;                           // +0x5c（clear 清零）
    int m_i60;                           // +0x60（clear 清零）
    int m_i64;                           // +0x64（clear 清零）
    int m_i68;                           // +0x68（clear 未清，op= 拷贝）
    int m_i6c;                           // +0x6c（clear 清零）
    int m_i70;                           // +0x70（clear 清零）
    int m_ints74[3];                     // +0x74..+0x7c（clear 循环 i=0..2 清零）

private:
    char m_tail_[4];                     // [推断] 主类下一个成员 +0x270 int 紧随
                                         // 其后，即本子对象占 0x1ec..0x270（0x84）
};

// ---- PassiveObjGeneratorObject（ORIG C1 T 0x08a3ae56 / C1ERKS_ W 0x08a47834 /
//      D1 W 0x08a3e6f4 / aSERKS_ W 0x08a478ee；sizeof==0xc）----
// 布局证据：ctor 逐字段清零 + string("") 构造；拷贝构造/operator= 为编译器
// 隐式生成（int,float,string 声明序逐字段，D1 仅析构 string）——ORIG 三者为
// 弱符号且形态与隐式生成一致，故不声明用户版本。
class PassiveObjGeneratorObject
{
public:
    PassiveObjGeneratorObject();         // ORIG T 0x08a3ae56
    // ORIG W 0x08a478ee：+0/+4 直拷 + string@8 拷贝。
    PassiveObjGeneratorObject& operator=(const PassiveObjGeneratorObject& o)
    {
        m_i00 = o.m_i00;
        m_f04 = o.m_f04;
        m_s08 = o.m_s08;
        return *this;
    }

    int m_i00;                           // +0x00（ctor=0，movl 直写）
    float m_f04;                         // +0x04（ctor=0.0f，mov $0,%edx 中转实证）
    std::string m_s08;                   // +0x08（ctor=""）
};  // sizeof == 0xc（vector<PassiveObjGeneratorObject> _M_insert_aux
    // 步进 sub/lea $0xc 实证）
static_assert(sizeof(PassiveObjGeneratorObject) == 0xc,
              "PassiveObjGeneratorObject ABI drift (ORIG 0xc)");

// ---- PassiveObjGeneratorPattern（ORIG C1 T 0x08a3ad70 / C1ERKS_ W 0x08a47c6c /
//      D1 W 0x08a3e70a / aSERKS_ W 0x08a47d72；sizeof==0x1c）----
// 布局证据：ctor 顺序 m_b00=1 → string 默认构造 → m_i08=0 → m_i0c=0 →
// vector<unsigned char> 构造，函数体再 clear() 两个成员；隐式拷贝构造/赋值/
// 析构按声明序与 ORIG 弱符号形态一致。
class PassiveObjGeneratorPattern
{
public:
    PassiveObjGeneratorPattern();        // ORIG T 0x08a3ad70
    // ORIG W 0x08a47d72：byte@0 + string@4 + int@8 + int@0xc + vector<uchar>@0x10。
    PassiveObjGeneratorPattern& operator=(const PassiveObjGeneratorPattern& o)
    {
        m_b00 = o.m_b00;
        m_s04 = o.m_s04;
        m_i08 = o.m_i08;
        m_i0c = o.m_i0c;
        m_v10 = o.m_v10;
        return *this;
    }

    unsigned char m_b00;                 // +0x00（ctor=1，movb 实证）
    std::string m_s04;                   // +0x04
    int m_i08;                           // +0x08（ctor=0）
    int m_i0c;                           // +0x0c（ctor=0）
    std::vector<unsigned char> m_v10;    // +0x10
};  // sizeof == 0x1c（new_allocator construct movl $0x1c 实证）
static_assert(sizeof(PassiveObjGeneratorPattern) == 0x1c,
              "PassiveObjGeneratorPattern ABI drift (ORIG 0x1c)");

// ---- PassiveObjGeneratorSequence（ORIG C1 T 0x08a3ae0a / C1ERKS_ W 0x08a4812a /
//      D1 W 0x08a3e756 / aSERKS_ W 0x08a481d4；sizeof==0x8）----
// 布局证据：ctor 顺序 string 默认构造 → m_i04=0 → 函数体 string::clear；
// 隐式拷贝构造/赋值/析构与 ORIG 弱符号形态一致。
class PassiveObjGeneratorSequence
{
public:
    PassiveObjGeneratorSequence();       // ORIG T 0x08a3ae0a
    // ORIG W 0x08a481d4：string@0 拷贝 + int@4 直拷。
    PassiveObjGeneratorSequence& operator=(const PassiveObjGeneratorSequence& o)
    {
        m_s00 = o.m_s00;
        m_i04 = o.m_i04;
        return *this;
    }

    std::string m_s00;                   // +0x00
    int m_i04;                           // +0x04（ctor=0）
};  // sizeof == 0x8（new_allocator construct movl $0x8 实证）
static_assert(sizeof(PassiveObjGeneratorSequence) == 0x8,
              "PassiveObjGeneratorSequence ABI drift (ORIG 0x8)");

// ---- PassiveObjGeneratorInfo（ORIG C1/C2/D1/D2 全 W 弱符号
//      0x08a3e8ae/0x08a3e156：编译器隐式生成——C1 按声明序构造
//      pattern → sequence → object 三 vector（带 unwind 清理），
//      D1 逆序析构 object → sequence → pattern）----
// 布局证据：C1 objdump 依次 call vector<Pattern>C1(this+0x0) /
// vector<Sequence>C1(this+0xc) / vector<Object>C1(this+0x18)；
// 主类 m_genInfo 占 +0x1b0..+0x1d4（0x24）= 3 × vector 0xc。
class PassiveObjGeneratorInfo
{
public:
    std::vector<PassiveObjGeneratorPattern> m_patterns;    // +0x00
    std::vector<PassiveObjGeneratorSequence> m_sequences;  // +0x0c
    std::vector<PassiveObjGeneratorObject> m_objects;      // +0x18
};  // sizeof == 0x24（ORIG C1 三 vector 构造实证）

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
    void clear();                        // ORIG W 0x08a3da06 空体（cpp 定义）
    STPassiveObjectAddEffect();          // ORIG W 0x08a3da0c
    ~STPassiveObjectAddEffect();         // ORIG W 0x08a3e110

    std::vector<std::string> m_strs00;   // +0x00
    std::vector<int> m_ints0c;           // +0x0c
};  // sizeof == 0x18

// ---- RandomItemFairy（ORIG C1/C2 W 0x8a3db5e：ctor 仅调 clear；
//      clear W 0x8a3db72：+0 movl $0 直写 + 循环 i=0..2 清 +4+i*4；
//      D1/D2 W 0x8a4f436 空体（push ebp/mov/pop ebp/ret 5 字节））----
// 布局证据：clear 单指令清 +0、`0x4(%eax,%edx,4)` 索引寻址清 +4/+8/+c；
// ImportPassiveObjectScript token 分支以 ScanInt 顺序填充 4 个 int
// （local_d44 → iStack_d40/d3c/d38 实证，push_back 基址即 +0）。
class RandomItemFairy
{
public:
    RandomItemFairy();             // ORIG W 0x8a3db5e（cpp 定义；P69：ctor 内 call clear）
    void clear();                  // ORIG W 0x8a3db72（cpp 定义）
    // ORIG 0x8a4f436 D1Ev 存在但 ImportPassiveObjectScript 内 0 调用点
    // → 隐式平凡析构（勿声明用户析构，否则 O0 在作用域出口生成 D1 调用）。

    int m_i00;                     // +0x00（clear movl $0 直写）
    int m_ints04[3];               // +0x04..+0x0c（clear 循环 i=0..2 清零）
};  // sizeof == 0x10

// ---- STPassiveObjectScript 主类 ----
// 字段语义待 Import/exportPassiveObjectScript token 分支进一步具名化；
// 当前名称按偏移命名（m_b* byte / m_f* float / m_v* 容器），语义可证处换名。
class STPassiveObjectScript
{
public:
    STPassiveObjectScript();             // ORIG W 0x08a3a538
    // ORIG D1/D2 W 0x08a3e25a：string(+0x284) → vector<RandomItemFairy>(+0x274) →
    // PassiveIObjSniperInfo(+0x1ec) → … 成员析构链（用户空析构让编译器生成）。
    ~STPassiveObjectScript() {}
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
    float m_fa8;                                // +0x0a8（clear 经 edx 中转 0.0f 实证）
    float m_fac;                                // +0x0ac（clear 经 edx 中转 0.0f 实证）
    int m_ib0;                                  // +0x0b0（clear=500）
    unsigned char m_bb4;                        // +0x0b4
    float m_fb8;                                // +0x0b8（clear 经 edx 中转 0.0f 实证）
    float m_fbc;                                // +0x0bc（clear 经 edx 中转 0.0f 实证）
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
    float m_f170;                               // +0x170（clear=-1.0f：mov $0xbf800000,%edx 实证）
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
    int m_i288;                                 // +0x288（ORIG import 0x8a35715 ScanInt 写入实证）
    int m_i28c;                                 // +0x28c（ORIG import 0x8a35725 ScanInt 写入实证；
                                                //   主类末 int；clear/ctor 均不清，POD 尾区）
};  // sizeof == 0x290（ORIG m_i28c 之后无更多成员证据）

#endif  // W8LT_PASSIVE_OBJECT_SCRIPT_H_
