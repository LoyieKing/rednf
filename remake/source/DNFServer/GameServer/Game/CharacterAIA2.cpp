// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CharacterAIA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CharacterAIA2.cpp —— 角色/AI 脚本依赖恢复（CharacterAIA2 批次独占 TU）。
// ---- round5 判词汇解（本 TU 2 件非 identical，均 4.1/4.4 codegen 族）----
// STAIInfoScript::destroy 0x0836753a：**round5 已修**（去具名 `elem`，改
//   `if (m_patterns[p][i] != 0) delete m_patterns[p][i];` ⇒ 3 次 ix() 直读 +
//   `mov (%eax),%ebx` callee-saved 保活）⇒ 86/86 指令数全等、DIFF 列表移除；
//   旧 b28d-L16「去具名反劣 90→102」判词系试验形偏差（下标写多一次），作废。
// importAIInfoScript 0x08a390db：round5 共四处修复 —— ①tok/isType/ok 三个函数头
//   局部（复现入口 `call _ZNSsC1Ev; movb $0x0,-0x21` 槽图基线，P198① 预清）；
//   ②byLevel 分支块序交换（直落块 = !byLevel/makeAIFullPath）；③[targeting bonus]
//   两处 `while (ok = ScanStr(&s))` 复用函数级 ok（ORIG `mov %al,-0x21` 实证）
//   ⇒ our 461→**473**（ORIG 475，gap 14→**2**）。取反形 `while(true){if(!ok)break;}`
//   实测劣化（478，gap −3）已回退留痕。余 2 条 = 4.1 `movzbl/xor $1/test/je`
//   取反物化族（N122）⇒ §6.2 判语义完成、byte 层 N 封顶。
//   [新轴复核·Main 配方(A)const/(B)循环符号性] 均**不适用**：本函数无 `const` 局部
//   参与比较（无 `cmpl $imm,-N` 常量传播点可去 const）；`for (int i = 0; i <= 21; ++i)`
//   已是 int 有符号（ORIG 侧亦同）。余 2 条实证 = ORIG `cmp $0x14,%eax`（idx205/207
//   寄存器比较）vs 我方 `cmpl $imm,-0x1c`（内存直比）⇒ **N18b** + 上述 N122。
// importAICharacterCommonScript 0x088c9ccc：round5 把 [on attack]/[on key] 两段改
//   `switch (ScanFloatStr(fv, tok))`（P251）并删多余 `float fv = 0.0f`（ORIG 全
//   函数无该初始化写）⇒ our_insns 2111→2107；再补 [hell monster] 段具名 int
//   中转槽（ORIG 0x088cbfc3 `mov %eax,-0x10; cmpl $0x0,-0x10; setne %dl`）⇒
//   2107→2108，全函数 13 处「非分支比较」与 ORIG **逐条同形（含槽号 -0x10）**。
//   余差 = ORIG 函数级共享 int 槽（-0xa4 族，4.1 不按 lifetime 复用）与帧
//   352 vs 304 ⇒ N82/N95 封顶；§6.2 三件套：callsite 320/320 CLEAR + gap −3 +
//   槽位集逐段核（N95：4.4 局部槽基恒 ebp-8，全帧偏移差的根）。
//
// 本 TU 提供的 ORIG df_game_r 真实符号（nm 核验）：
//   STAICharacterCommonScript C1 T 0x088c8a78 / clear T 0x088c9592 /
//     D1 T 0x088c903c / D0 T 0x088c9574 / destroy T 0x088c98bc
//     （C1/D1 报告 docs/class_func_reports/STAICharacterCommonScript/ 完整转写；
//       clear/destroy/D0 报告缺失 → objdump 转写）
//   STAIInfoScript C1 W 0x08366f48 / D1 W 0x083671a4 / clear W 0x083673d4 /
//     destroy W 0x0836753a
//     （copy-ctor 报告 docs/class_func_reports/STAIInfoScript/STAIInfoScript.md
//       完整转写成员集；默认 C1/D1/clear/destroy 报告缺失 → objdump 转写）
//   CAICharacter C1 W 0x08373a9a / D1 W 0x08373b32（报告缺失 → objdump 转写）
//   STAIInfoScript C1ERKS T 0x08a38bc6 / aSERKS_ T 0x08a38e34
//     （报告 STAIInfoScript/STAIInfoScript.md 与 operator=.md 完整转写）
//   isExistAiPattern T 0x08a2232f / exportAIPatternPath T 0x08a223b0
//     （报告 _lt_global_gt_/isExistAiPattern.md 与 exportAIPatternPath.md 完整转写）
//   getRandomChampionElement(STAICharacterCommonScript) T 0x088b9d54
//     （报告 _lt_global_gt_/getRandomChampionElement_088b9d54.md 完整转写）
//   CNPCDynamicInfo C1 W 0x0858191c（报告缺失 → objdump 转写）
//   CNPCDynamicInfo C1 W 0x0858191c（报告缺失 → objdump 转写）
//   PowerParameterScript::clear W 0x0837187a（报告缺失 → objdump 转写）
//   isKeyFile T 0x088c9a4c / getFileName T 0x088c9a98 / makeKeyFullPath T 0x08a38b5d
//     makeAIFullPath T 0x08a38af4 / makeFullPath T 0x088c9990 / stringToInt T 0x08a38a8b
//     （各 _lt_global_gt_/*.md 报告完整转写）
//   ACTIVEOBJECT_STATE_STRING D 0x093f4d80 / ACTIVESTATUS_ATTACK_TAG D 0x093f54e0
//     （ORIG .data 指针表逐项解引用 .rodata 字符串提取，NULL 终止实证）
//
// 残余 DIFF 归因（batch-11 §6.3 口径）：
//   _Z41__static_initialization_and_destruction_0ii —— 全镜像 493 个同名 t 符号，
//   game_func_report 二进制切片撞名假阳性（§6.3）。本 TU 真实体 =
//   0x088cc086（26 insn：ios_base::Init + atexit + STScriptFileList
//   g_aiCharacterScriptFileList@0x94f6a20 ctor）；后者按 P186 归属迁移至
//   InitScriptProducers_5.cpp:68 定义，本 TU _Z41 仅余 __ioinit 链（16 insn），
//   语义覆盖等价，勿在本 TU 重复定义该对象（ODR 双定义禁止）。
//   importAIInfoScript / importAICharacterCommonScript / STAIInfoScript::destroy /
//   STRDCharacterStatusRate::Clear / _Vector_base/uninitialized_copy 模板族 ——
//   N7/N82/常量折叠/libstdc++ 内部结构跨版本产物，逐条标注见各函数头注释。
// ============================================================================

#include <cstdlib>

#include "CDataManager.h"            // PowerParameterScript 完整声明 + stMonsterIndependentDrop_t
#include "STCharacterScript.h"       // STRDCharacterStatus / ActiveActionFileSet 权威布局
#include "EquipmentParameterInfo.h"  // EquipmentSkillDataUp + importSkillDataUp 声明
#include "DNFLexWrapper.h"           // loadRDARScriptFile/getDirectoryPath/Scan*
#include "CreatureScript.h"          // getElementName(ENUM_ELEMENT)

// ScriptDeps 域提供的全局（ORIG T 0x088bfd97）；无公共头声明，此处补一致声明。
#include "CharacterAIA2/getItemRarity.inc"
extern char g_aiCharacterScriptBaseDirectory[0x100];

// ============================================================================
// ORIG 字符串表数据（extern "C"，nm: D 符号无 mangled 前缀）
// ============================================================================

// [L32 GI 调序] 定义序按 ORIG 键名：ORIG `_GLOBAL__I_ACTIVESTATUS_ATTACK_TAG`
//   0x0810e4xx（CU=RDARScriptServerParameter.cpp）⇒ 本 TU 首个强符号须为
//   ACTIVESTATUS_ATTACK_TAG；仅交换两块定义的先后，零语义/零布局改动（P327：
//   移的是常量静态数组，不做动态初始化，构造序不变）。
// D 0x093f54e0：19 项 + NULL 终止。
extern "C" const char* ACTIVESTATUS_ATTACK_TAG[20];
const char* ACTIVESTATUS_ATTACK_TAG[20] = {
    "[slow]",   "[freeze]", "[poison]", "[stun]",
    "[curse]",  "[blind]",  "[lightning]", "[stone]",
    "[sleep]",  "[burn]",   "[weapon break]", "[bleeding]",
    "[haste]",  "[bless]",  "[element]", "[confuse]",
    "[hold]",   "[armor break]", "[none]", 0,
};

// D 0x093f4d80：15 项 + NULL 终止（objdump -s .data/.rodata 逐指针提取）。
extern "C" const char* ACTIVEOBJECT_STATE_STRING[16];
const char* ACTIVEOBJECT_STATE_STRING[16] = {
    "[stand]",       "[sit]",         "[sit forever]", "[damage]",
    "[down]",        "[die]",         "[jump]",        "[jump attack]",
    "[attack]",      "[hold]",        "[throw]",       "[dash]",
    "[dash attack]", "[get item]",    "[buff]",        0,
};

// ============================================================================
// 路径/表工具函数（各 _lt_global_gt_/*.md 报告全文转写）
// ============================================================================

// isKeyFile —— 报告 _lt_global_gt_/isKeyFile.md：非空且以 ".key" 结尾。
#include "CharacterAIA2/isKeyFile.inc"
}

// stringToInt —— 报告 _lt_global_gt_/stringToInt.md：线性查表，max==-1 视为 999。
#include "CharacterAIA2/stringToInt.inc"
}

// getFileName —— 报告 _lt_global_gt_/getFileName.md：拷贝入串后从尾向前找
// '/' 或 '\\' 截掉目录前缀，再找 '.' 截掉扩展名。sret 返回。
std::string getFileName(const std::string& path)
{
    std::string out(path);

    std::string::iterator it = out.end() - 1;
    while (it != out.begin())
    {
        if (*it == '/' || *it == '\\')
        {
            out.erase(out.begin(), it + 1);
            break;
        }
        it--;
    }

    std::string::iterator it2 = out.end() - 1;
    while (it2 != out.begin())
    {
        if (*it2 == '.')
        {
            out.erase(it2, out.end());
            break;
        }
        it2--;
    }
    return out;
}

// makeFullPath —— 报告 _lt_global_gt_/makeFullPath.md：path 不含 '/'/'\\' 时
// dir+path 回写 path 并返 true；否则不动返 false。首参按值传递（mangled Ss）。
#include "CharacterAIA2/makeFullPath.inc"
}

// makeAIFullPath —— 报告 _lt_global_gt_/makeAIFullPath.md：dir+"ai/" 后接 makeFullPath。
// ORIG 0x08a38af4：临时直接作为按值参数（无额外拷贝构造）。
#include "CharacterAIA2/makeAIFullPath.inc"
}

// makeKeyFullPath —— 报告 _lt_global_gt_/makeKeyFullPath.md：dir+"key/" 后接 makeFullPath。
// ORIG 0x08a38b5d：临时直接作为按值参数（无额外拷贝构造）。
#include "CharacterAIA2/makeKeyFullPath.inc"
}

// ============================================================================
// 子类型（生命周期符号均为 ORIG W；报告缺失，objdump 转写）
// ============================================================================

// MinimumInfo：+0x00 std::string / +0x04 int（sizeof 8）。
// C1 W 0x088cc3e2 = string ctor；D1 W 0x088cc3f6 = string dtor；
// clear W 0x088cc190 = 仅置 (+this)+4 = 0。
struct MinimumInfo
{
    MinimumInfo() {}
    ~MinimumInfo() {}
    void clear() { m_field4 = 0; }

    std::string m_name;     // +0x00（[minimum info] 首个 ScanStr）
    int         m_field4;   // +0x04（第 5 个 ScanInt）
};

// APCSpeech（sizeof 0x18）：+0x00 vector<string> / +0x0c vector<int>。
// C1 W 0x088cc328 = 两 vector ctor + clear()；D1 W 0x088cc40a = 逆序 vector dtor；
// clear W 0x088cc390；add W 0x088cc3b2 = lines.push_back(s); probs.push_back(i)。
struct APCSpeech
{
    // ORIG C1 W 0x088cc328：两 vector 默认构造 + call clear()（非内联 clear 体）。
    APCSpeech() { clear(); }
    ~APCSpeech() {}
    void clear()
    {
        m_lines.clear();
        m_probs.clear();
    }
    // ORIG W 0x088cc3b2 _ZN9APCSpeech3addERSsi：非 const std::string& 形参。
    void add(std::string& line, int prob)
    {
        m_lines.push_back(line);
        m_probs.push_back(prob);
    }

    std::vector<std::string> m_lines;  // +0x00
    std::vector<int>         m_probs;  // +0x0c
};
typedef char assert_apc_speech_size[(sizeof(APCSpeech) == 0x18) ? 1 : -1];

// STRDCharacterStatusRate（sizeof 0xa4 = 41 float）。
// C1 W 0x088cc2fa = Clear()；Clear W 0x088cc1a0：
//   +0x00..+0x24 十个 1.0f；循环 i=0..3 写 +0x28+4i；循环 k=0..16 写 +0x38+4k
//   （k=16 → +0x78）；尾部 +0x7c..+0xa0 十个 1.0f。41 float 全覆盖。
struct STRDCharacterStatusRate
{
    STRDCharacterStatusRate() { Clear(); }

    void Clear()
    {
        // ORIG 0x088cc1a0：head/tail 各 10 条独立写（每次重装载 this），
        // mid28/mid38 为循环（setle）。4.4 常量折叠省 %edx 中转属版本差（P182）。
        m_head[0] = 1.0f;
        m_head[1] = 1.0f;
        m_head[2] = 1.0f;
        m_head[3] = 1.0f;
        m_head[4] = 1.0f;
        m_head[5] = 1.0f;
        m_head[6] = 1.0f;
        m_head[7] = 1.0f;
        m_head[8] = 1.0f;
        m_head[9] = 1.0f;
        // P191：4.1 循环变量双初始化（`movl $0x0,-0x4`×2）⇒ `int i=0;`+`for(i=0;…)` 两段。
        // [b28c-31] AE：两循环共用单变量 i 复现 ORIG 0x088cc1e3/0x088cc24b 同槽
        // -0x4 复用（两个独立变量让 4.4 各占一槽，实测 i=-0x8/k=-0x4 DIFF）；90=90。
        int i = 0;
        for (i = 0; i <= 3; ++i) m_mid28[i] = 1.0f;
        for (i = 0; i <= 16; ++i) m_mid38[i] = 1.0f;
        m_tail[0] = 1.0f;
        m_tail[1] = 1.0f;
        m_tail[2] = 1.0f;
        m_tail[3] = 1.0f;
        m_tail[4] = 1.0f;
        m_tail[5] = 1.0f;
        m_tail[6] = 1.0f;
        m_tail[7] = 1.0f;
        m_tail[8] = 1.0f;
        m_tail[9] = 1.0f;
    }

    float m_head[10];   // +0x00..+0x24
    float m_mid28[4];   // +0x28..+0x34
    float m_mid38[17];  // +0x38..+0x78（k=16 → 0x78）
    float m_tail[10];   // +0x7c..+0xa0
};  // sizeof == 0xa4
typedef char assert_strd_rate_size[(sizeof(STRDCharacterStatusRate) == 0xa4) ? 1 : -1];

// ---- vector 元素 POD（元素步长由各自 vector<T>::push_back 的 construct 步长
//      objdump 实证；字段语义为【推断】，仅按解析写入顺序命名）----
struct STAICharacterEquipmentData  // push_back 步长 0xc（W 0x088cc8cc 区）
{
    int m_a;  // ScanInt 第 1 值
    int m_b;  // ScanInt 第 2 值
    int m_c;  // ScanInt 第 3 值
};  // 0xc
typedef char assert_eqdata_size[(sizeof(STAICharacterEquipmentData) == 0xc) ? 1 : -1];

struct STAICharacterSkillData  // 步长 0x8（W 0x088cc940 区）
{
    int m_a;  // ScanInt(&ok)
    int m_b;  // ScanInt(NULL)
};  // 0x8

struct STAICharacterItemData  // 步长 0x8（W 0x088cc9b4 区）
{
    int m_a;
    int m_b;
};  // 0x8

struct STAICharacterCustomSkillLevel  // 步长 0x10（W 0x088ccb62 区）
{
    int m_a;  // ScanInt(&ok)
    int m_b;  // ScanInt(NULL)
    int m_c;  // ScanInt(NULL)
    int m_d;  // ScanInt(NULL)
};  // 0x10

struct STAICharacterSkillCorrection  // C1 W 0x088cc30e：{0, 1.0f}
{
    int   m_skill;
    float m_rate;
    STAICharacterSkillCorrection() : m_skill(0), m_rate(1.0f) {}
};  // 0x8

// stEnamyDropAvatarElement_t：push_back 步长 0xc（W 0x088ccbd6 区）。
// 解析写入 {ScanInt, ScanInt, ScanInt 低字节}；第三字段高 3 字节 ORIG 为栈垃圾，
// 此处建模 unsigned char + pad（同样不初始化 pad，行为一致）。
struct stEnamyDropAvatarElement_t
{
    int           m_itemId;
    int           m_total;
    unsigned char m_flag;
    unsigned char m_pad[3];
};  // 0xc
typedef char assert_enamy_elem_size[(sizeof(stEnamyDropAvatarElement_t) == 0xc) ? 1 : -1];

// stEnamyDropAvatarInfo_t（sizeof 0x40）：C1 W 0x08368de8 = 构造 +0x30 vector；
// D1 W 0x08368dfe；clear W 0x088cc10c：
//   +0x00=0；+0x04=0；循环 i=0..3 清 +0x08+4i；循环 k=0..4 清 +0x18+4k；
//   +0x2c=0；+0x30 vector clear。
struct stEnamyDropAvatarInfo_t
{
    // ORIG C1 W 0x08368de8：仅 +0x30 vector 默认构造；标量字段不初始化（P131）。
    stEnamyDropAvatarInfo_t() {}
    ~stEnamyDropAvatarInfo_t() {}

    void clear()
    {
        m_field0 = 0;
        m_field4 = 0;
        for (int i = 0; i <= 3; ++i) m_rates08[i] = 0.0f;
        for (int k = 0; k <= 4; ++k) m_rates18[k] = 0.0f;
        m_total = 0;
        m_elements.clear();
    }

    int                                     m_field0;          // +0x00
    int                                     m_field4;          // +0x04
    float                                   m_rates08[4];      // +0x08..+0x14
    float                                   m_rates18[5];      // +0x18..+0x28
    int                                     m_total;           // +0x2c
    std::vector<stEnamyDropAvatarElement_t> m_elements;        // +0x30
};  // sizeof == 0x3c
typedef char assert_enamy_info_size[(sizeof(stEnamyDropAvatarInfo_t) == 0x3c) ? 1 : -1];

// ============================================================================
// STAIInfoScript（多态：ORIG _ZTV/_ZTI/_ZTS14STAIInfoScript 存在；sizeof 0x224）
// copy-ctor 报告 STAIInfoScript/STAIInfoScript.md 完整转写成员集；
// 默认 C1/D1/clear/destroy 报告缺失 → objdump 转写。
class STAIInfoScript
{
public:
    STAIInfoScript();
    STAIInfoScript(const STAIInfoScript& other);      // ORIG T 0x08a38bc6（C1/C2 同址）
    virtual ~STAIInfoScript();          // 发 D1/D0/vtable，匹配 ORIG 符号集
    STAIInfoScript& operator=(const STAIInfoScript& other);  // ORIG T 0x08a38e34
    void clear();                       // ORIG W 0x083673d4
    void destroy();                     // ORIG W 0x0836753a

    int m_thinkTerm;                    // +0x04
    int m_destinationChangeTerm;        // +0x08
    std::string m_dir;                  // +0x0c
    std::vector<std::string*> m_patterns[5];  // +0x10 easy/medium/hard/ultimate/hero
    int m_targetingBonus[22];           // +0x4c
    std::vector<int> m_targetingBonusStr[22]; // +0xa4
    int m_keepDistance;                 // +0x1ac
    // ORIG C1 0x08366f48（+0xe7..）对 +0x1b0..0x1e0 发射 5 个独立 C1Ev call
    // （非数组构造循环；同族数组 patterns/bonusStr 均为倒计数循环）→ 权威建模
    // 为 5 个独立命名成员。
    std::vector<int> m_keepRange0;      // +0x1b0
    std::vector<int> m_keepRange1;      // +0x1bc
    std::vector<int> m_keepRange2;      // +0x1c8
    std::vector<int> m_keepRange3;      // +0x1d4
    std::vector<int> m_keepRange4;      // +0x1e0
    int m_intelligence;                 // +0x1ec
    int m_field1f0;                     // +0x1f0
    int m_field1f4;                     // +0x1f4
    int m_vision;                       // +0x1f8
    int m_backVision;                   // +0x1fc
    bool m_noChangeTarget;              // +0x200
    char m_pad201[3];
    int  m_pod204[8];                   // +0x204..+0x220（外层 clear 清零）
};  // sizeof == 0x224
typedef char assert_stai_info_size[(sizeof(STAIInfoScript) == 0x224) ? 1 : -1];

// STAICharacterCommonScript（多态；sizeof 0x1014）。
// C1/D1 报告 docs/class_func_reports/STAICharacterCommonScript/ 完整转写；
// clear/destroy 报告缺失 → objdump 转写。
class STAICharacterCommonScript
{
public:
    STAICharacterCommonScript();           // ORIG T 0x088c8a78
    virtual ~STAICharacterCommonScript();  // D1 T 0x088c903c / D0 T 0x088c9574
    void clear();                          // ORIG T 0x088c9592
    void destroy();                        // ORIG T 0x088c98bc

    MinimumInfo m_minimumInfo;                     // +0x004
    STRDCharacterStatus m_status;                  // +0x00c（0x57 packed）
    char m_pad63[1];                               // +0x063
    std::vector<STAICharacterEquipmentData> m_equipment;       // +0x064
    std::vector<STAICharacterSkillData> m_skills[2];           // +0x070/+0x07c
    std::vector<STAICharacterSkillCorrection> m_corrections;   // +0x088
    std::vector<STAICharacterItemData> m_items[2];             // +0x094/+0x0a0
    STAIInfoScript m_info;                         // +0x0ac（0x224）
    std::vector<std::string> m_keyStreamFiles;     // +0x2d0
    std::vector<std::string> m_keyFiles;           // +0x2dc
    std::map<std::string, int> m_keyCooltimes;     // +0x2e8（sizeof 0x18 @gcc4.4）
    APCSpeech m_speeches[111];                     // +0x300（111×0x18 = 0xa68）
    std::vector<APCSpeech> m_onAttackSpeech;       // +0xd68
    std::vector<APCSpeech> m_onKeySpeech;          // +0xd74
    int m_rarity;                                  // +0xd80（clear=0）
    int m_appearancePoint;                         // +0xd84（clear=0）
    std::vector<std::pair<int, int> > m_deathTowerItems; // +0xd88
    std::vector<std::pair<int, int> > m_partyMembers;    // +0xd94
    int m_farAttackReactionRate;                   // +0xda0（clear=0）
    int m_pvpWinPointLow;                          // +0xda4（clear=0）
    int m_pvpWinPointHigh;                         // +0xda8（clear=0）
    STRDCharacterStatusRate m_statusRate;          // +0xdac（0xa4）
    std::vector<STAICharacterCustomSkillLevel> m_customSkills; // +0xe50
    float m_attackDamageRate;                      // +0xe5c（clear=1.0f）
    int m_addEquipStatusFromLevel;                 // +0xe60（clear=0）
    int m_armorSubtype;                            // +0xe64（clear=2）
    int m_defaultEquipment;                        // +0xe68（clear=1）
    std::vector<int> m_dollSpecialDungeons;        // +0xe6c
    STRDCharacterStatusRate m_specialStatusRate;   // +0xe78（0xa4）
    float m_attackDamageRateSpecial;               // +0xf1c（clear=1.0f）
    std::vector<int> m_championElements;           // +0xf20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops; // +0xf2c
    std::vector<int> m_limitPassiveObjects;        // +0xf38
    int m_overheadGaugeType;                       // +0xf44（clear=0）
    int m_overheadGaugePosX;                       // +0xf48（clear=0）
    int m_overheadGaugePosY;                       // +0xf4c（clear=0）
    stEnamyDropAvatarInfo_t m_avatarDrop;          // +0xf50（0x3c）
    int m_getupInvincible;                         // +0xf8c（clear=1）
    std::vector<EquipmentSkillDataUp> m_skillDataUps; // +0xf90
    std::vector<float> m_summonStatusRates;        // +0xf9c
    bool m_leaveBody;                              // +0xfa8（clear=true）
    char m_padfa9[3];
    ActiveActionFileSet m_actionSet;               // +0xfac（0x40）
    std::string m_lastAction;                      // +0xfec
    std::string m_procAction;                      // +0xff0
    bool m_addGravityInAerial;                     // +0xff4（clear=false）
    char m_padff5[3];
    float m_gravityAerialA;                        // +0xff8（clear=20.0f）
    float m_gravityAerialB;                        // +0xffc（clear=-300.0f）
    int m_noCooltime;                              // +0x1000（clear=0）
    bool m_checkAccountItem;                       // +0x1004（clear=true）
    char m_pad1005[3];
    int m_npcId;                                   // +0x1008（clear=-1）
    bool m_isHellMonster;                          // +0x100c（clear=false）
    char m_pad100d[3];
    int m_animationJob;                            // +0x1010（clear=11）
};  // sizeof == 0x1014
typedef char assert_stai_script_size[
    sizeof(STAICharacterCommonScript) == 0x1014 ? 1 : (int)-sizeof(STAICharacterCommonScript)];

#include "CharacterAIA2/STAIInfoScript.inc"
}

#include "CharacterAIA2/STAIInfoScript_1.inc"
}

STAIInfoScript& STAIInfoScript::operator=(const STAIInfoScript& other)
{
    // ORIG T 0x08a38e34（objdump 逐条转写）：
    // destroy() → m_thinkTerm/m_destinationChangeTerm → m_dir →
    // m_patterns[5] 深拷贝（new string）→ m_targetingBonus[22] 与
    // m_targetingBonusStr[22] → m_keepDistance → m_keepRange[5] →
    // m_vision/m_backVision/m_intelligence/m_field1f0/m_field1f4 →
    // m_noChangeTarget。【+0x204..+0x220 m_pod204 不触碰】（ORIG 固有）
    destroy();
    m_thinkTerm = other.m_thinkTerm;
    m_destinationChangeTerm = other.m_destinationChangeTerm;
    m_dir = other.m_dir;
    for (int p = 0; p < 5; ++p)
    {
        for (std::size_t i = 0; i < other.m_patterns[p].size(); ++i)
        {
            std::string* elem = other.m_patterns[p][i];
            std::string* created = new std::string(*elem);
            m_patterns[p].push_back(created);
        }
    }
    for (int i = 0; i <= 21; ++i)
    {
        m_targetingBonus[i] = other.m_targetingBonus[i];
        m_targetingBonusStr[i] = other.m_targetingBonusStr[i];
    }
    m_keepDistance = other.m_keepDistance;
    // ORIG 0x08a38fd2..0x08a3905e：5 个独立 aSERKS call（逐条）。
    m_keepRange0 = other.m_keepRange0;
    m_keepRange1 = other.m_keepRange1;
    m_keepRange2 = other.m_keepRange2;
    m_keepRange3 = other.m_keepRange3;
    m_keepRange4 = other.m_keepRange4;
    m_vision = other.m_vision;
    m_backVision = other.m_backVision;
    m_intelligence = other.m_intelligence;
    m_field1f0 = other.m_field1f0;
    m_field1f4 = other.m_field1f4;
    m_noChangeTarget = other.m_noChangeTarget;
    return *this;
}

#include "CharacterAIA2/dtor_STAIInfoScript.inc"
}

#include "CharacterAIA2/destroy.inc"
}

#include "CharacterAIA2/clear.inc"
}

#include "CharacterAIA2/STAICharacterCommonScript.inc"
}

#include "CharacterAIA2/dtor_STAICharacterCommonScript.inc"
}

#include "CharacterAIA2/destroy_1.inc"
}

#include "CharacterAIA2/clear_1.inc"
}

// ============================================================================
// importAIInfoScript —— ORIG T 0x08a390db
// 报告 _lt_global_gt_/importAIInfoScript.md 完整转写。
// 调用点差集：MISS=0；EXTRA 仅 _Unwind_Resume×2（EH landing pad 版本差）+
// C1↔C2 / D1↔D2 同址别名对（P93 伪差）。
// [round5] tok/isType 提升到函数头（复现 ORIG 入口 C1 + `movb $0,-0x21` 槽图基线）
// + byLevel 分支块序交换（ORIG 直落块 = !byLevel / makeAIFullPath）⇒ our 461→462。
// 残余 13 条 = 4.1 `movzbl 槽/xor $1/test %al,%al/je` bool 物化族（N122：4.4 对
// bool 局部恒 cmpb $0 直比，源形不可造）与 `while (ScanStr(&s))` 落槽形
// ⇒ 判语义完成、byte 层 N 封顶（§6.2 同口径）。
// ============================================================================
#include "CharacterAIA2/importAIInfoScript.inc"

#include "CharacterAIA2/importAIInfoScript_1.inc"
}

// ============================================================================
// importAICharacterCommonScript —— ORIG T 0x088c9ccc
// 报告 _lt_global_gt_/importAICharacterCommonScript.md 完整转写。
// §6.2 超大函数（2105 insns）验收：调用点差集 320/320 全命中（MISS/EXTRA=0）；
// gap 186 → −5 → **−3**（round5：两处 `switch(ScanFloatStr)` + 删 fv 初值 +
// [hell monster] 补具名 int 中转槽，our 2111→2108）；13 处非分支比较已与 ORIG
// 逐条同形（含 -0x10 槽号）。残余为 N82 帧槽错位（ORIG 0x130 vs NEW 0x160，
// P182 不强凑）与 ORIG 共享 int 槽（-0xa4 族）⇒ 判语义完成、byte 层 N 封顶。
// 返回 bool：文件加载失败 / [rarity] 解析失败或=6 / [pvp win point] 逆序 /
//   [drop avatar item] 任一扫描失败 / [animation job] 失败 /
//   兜底 importAIInfoScript 失败 → false；ScanType 终结 → true。
// ============================================================================
#include "CharacterAIA2/importAIInfoScript_2.inc"

#include "CharacterAIA2/importAICharacterCommonScript.inc"
}
// ============================================================================
// isExistAiPattern —— ORIG T 0x08a2232f
// 报告 _lt_global_gt_/isExistAiPattern.md 完整转写。
// bucket==-1 遍历 5 个难度桶；否则只查指定桶；任一非空返 1，否则 0。
#include "CharacterAIA2/isExistAiPattern.inc"
}

// ============================================================================
// exportAIPatternPath —— ORIG T 0x08a223b0
// 报告 _lt_global_gt_/exportAIPatternPath.md 完整转写。
// 遍历指定桶内模式名，截掉 m_dir 目录前缀后 fprintf 每行 "`%s`\n"。
// ============================================================================
#include "CharacterAIA2/exportAIPatternPath.inc"
}

// ============================================================================
// getRandomChampionElement —— ORIG T 0x088b9d54（STAICharacterCommonScript 版；
// STMonsterScript 版在 STMonsterScript.cpp）。报告
// _lt_global_gt_/getRandomChampionElement_088b9d54.md 完整转写。
// m_championElements(+0xf20) 非空 → rand()%size 取元素；空 → rand()%4+1。
// ============================================================================
#include "CharacterAIA2/getRandomChampionElement.inc"
}
// CNPCDynamicInfo —— C1 W 0x0858191c（报告缺失 → objdump 转写）：
//   m_script = a; +0x04 = 3; +0x08 = 0x7fffffff; +0x0c = 0。
// 完整类型归 NPC 动态域；本 TU 仅需产生该 ctor 符号
// （CtorProducers_1.cpp 的 CNPCDynamicInfoManager::add 经 asm 标签引用）。
// ============================================================================
class CNPCScript;  // 仅持指针，前置声明足够

class CNPCDynamicInfo
{
public:
    CNPCDynamicInfo(const CNPCScript* script) throw();   // ORIG W 0x0858191c（nothrow，见 NPCDynamicInfo.h）

    const CNPCScript* m_script;  // +0x00（changeMood/giveGiftItem 等报告域使用）
    int m_moodState;             // +0x04（ctor 置 3）
    int m_field8;                // +0x08（ctor 置 0x7fffffff）
    int m_fieldc;                // +0x0c（ctor 置 0）
};

#include "CharacterAIA2/CNPCDynamicInfo.inc"
}

// ============================================================================
// PowerParameterScript::clear —— ORIG W 0x0837187a
// 报告缺失 → objdump 转写（字段名映射 CDataManager.h 权威声明）。
// 序列：+0x00..+0x14 六个 int 清零；+0x1c=0；vec20.clear；
//   +0x30/+0x34/+0x48/+0x4c/+0x50/+0x54=0；+0x58=0；+0x5c=5；+0x60=5；
//   vec70.clear；+0x7c..+0x98 八个 int 清零；vec38.clear；+0x44=0；
//   +0x64/+0x68=0；+0xa8=0；vecAC.clear；+0xb8=0；vecBC.clear；vecC8.clear；
//   +0xd4=0。【+0x18/+0x2c/+0x6c 与 +0x9c powerWarServerList 不触碰】（ORIG 固有）
// ============================================================================
#include "CharacterAIA2/clear_2.inc"
}

// ============================================================================
// CAICharacter —— C1 W 0x08373a9a / D1 W 0x08373b32
// 报告缺失 → objdump 转写。布局与 CAICharacterList.cpp 本地镜像 ABI 一致
// （sizeof 0x74：init 的 operator new $0x74 实证）。
// C1：+0x10 vector<pair<int,int>> ctor / +0x24 vector<stMonsterIndependentDrop_t>
//   ctor / +0x30 stEnamyDropAvatarInfo_t ctor / +0x6c std::string ctor。
// D1：逆序析构。其余 POD 字段不初始化（ORIG 同）。
// ============================================================================
class CAICharacter
{
public:
    CAICharacter();    // ORIG W 0x08373a9a
    ~CAICharacter();   // ORIG W 0x08373b32

    int m_idx;                                                    // +0x00
    int m_level;                                                  // +0x04
    int m_field8;                                                 // +0x08（getRarity 取值处）
    int m_appearancePoint;                                        // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;                // +0x10
    int m_pvpWinLow;                                              // +0x1c
    int m_pvpWinHigh;                                             // +0x20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;   // +0x24
    stEnamyDropAvatarInfo_t m_avatarDrops;                        // +0x30（0x40）
    std::string m_name;                                           // +0x6c
    unsigned char m_rarity;                                       // +0x70（set 写入）
    char m_pad71[3];
};  // sizeof == 0x74
typedef char assert_cai_character_size[(sizeof(CAICharacter) == 0x74) ? 1 : -1];

#include "CharacterAIA2/CAICharacter.inc"
}

#include "CharacterAIA2/dtor_CAICharacter.inc"
}

// ---- force 发射 APCSpeech::add（ORIG W 0x088cc3b2）----
namespace {
#include "CharacterAIA2/forceApcSpeechAddEmission.inc"
}
}  // namespace
