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
int getItemRarity(const char* name);

// ORIG B 0x094f6920（char[0x100]，定义在 InitScriptProducers_5.cpp）。
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
bool isKeyFile(const char* name)
{
    if (name == 0)
        return false;
    // ORIG 0x088c9a65：strlen 结果落命名槽；strcmp 命中/未命中走分支常量岛（P56）。
    unsigned int len = strlen(name);
    if (strcmp(name + (len - 4), ".key") == 0)
        return true;
    return false;
}

// stringToInt —— 报告 _lt_global_gt_/stringToInt.md：线性查表，max==-1 视为 999。
int stringToInt(const char* s, const char** table, int maxCount)
{
    if (maxCount == -1)
        maxCount = 999;
    // ORIG 0x08a38aa7..0x08a38ae8：loop 头只判 i<max（setl）；体内 table[i]
    // 两次独立装载（null 守卫 + strcmp 实参，O0 无 CSE，P48），空则跳出。
    for (int i = 0; i < maxCount; ++i)
    {
        if (table[i] == 0)
            break;
        if (strcmp(table[i], s) == 0)
            return i;
    }
    return -1;
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
bool makeFullPath(std::string directory, std::string& path)
{
    // ORIG 0x088c99d4..0x088c99de：两处 `cmpl $0x0; jns/js` 符号位测试
    // （4.1 把 ==npos 折成 <0 判）→ 用有符号比较复现 cmp 立即数形；
    // 第二分支 js→then 的前跳为 4.1 jump-threading（N 类残余）。
    const int slash = path.find('/', 0);
    const int backslash = path.find('\\', 0);
    if (slash >= 0 || backslash >= 0)
        return false;
    path = directory + path;
    return true;
}

// makeAIFullPath —— 报告 _lt_global_gt_/makeAIFullPath.md：dir+"ai/" 后接 makeFullPath。
// ORIG 0x08a38af4：临时直接作为按值参数（无额外拷贝构造）。
void makeAIFullPath(std::string& dir, std::string& name)
{
    makeFullPath(dir + "ai/", name);
}

// makeKeyFullPath —— 报告 _lt_global_gt_/makeKeyFullPath.md：dir+"key/" 后接 makeFullPath。
// ORIG 0x08a38b5d：临时直接作为按值参数（无额外拷贝构造）。
void makeKeyFullPath(std::string& dir, std::string& name)
{
    makeFullPath(dir + "key/", name);
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

STAIInfoScript::STAIInfoScript()
{
    // ORIG C1 W 0x08366f48：vptr + 成员默认构造（编译器生成）→ 调 clear()。
    // 强符号覆盖 STMonsterScript.h 的 inline 弱定义（后者内联 clear 展开，
    // 与 ORIG 的“成员构造 + call clear”结构不一致）。
    clear();
}

STAIInfoScript::STAIInfoScript(const STAIInfoScript& other)
{
    // ORIG C1ERKS T 0x08a38bc6：vptr + 成员默认构造（编译器生成）→ clear() → operator=()。
    clear();
    *this = other;
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

STAIInfoScript::~STAIInfoScript()
{
    // ORIG D1 W 0x083671a4：写 vtable 后先 destroy() 释放 pattern 桶，
    // 随后编译器按声明逆序析构成员。
    destroy();
}

void STAIInfoScript::destroy()
{
    // ORIG 0x0836753a 逐块转写：外层 p≤4（setle）；内层每轮重取 size()
    // （setg 有符号比较）；body 内 ix() 三次独立装载（P48 无 CSE）：
    // 判空 → 取值 → 置 0；死块重复判空照抄（P184）。
    for (int p = 0; p <= 4; ++p)
    {
        for (int i = 0; i < (int)m_patterns[p].size(); ++i)
        {
            if (m_patterns[p][i] != 0)
                delete m_patterns[p][i];   // ORIG 0x083675a1 第 2 次 ix() 直读实参
            m_patterns[p][i] = 0;
        }
        m_patterns[p].clear();
    }
}

void STAIInfoScript::clear()
{
    // ORIG 0x083673d4（objdump 逐条转写）。
    m_thinkTerm = 0x3e8;
    m_destinationChangeTerm = 0x3e8;
    m_keepRange0.clear();               // ORIG 0x083673f9..0x08367439 逐条
    m_keepRange1.clear();
    m_keepRange2.clear();
    m_keepRange3.clear();
    m_keepRange4.clear();
    m_dir.clear();
    for (int i = 0; i <= 4; ++i)
        m_patterns[i].clear();
    for (int i = 0; i <= 21; ++i)
    {
        // ORIG 0x08367487..0x083674b0：两分支各自重装载 this+下标写常量
        // （三目会被 4.4 合并公共地址，-2 insn）。
        if (i == 0)
            m_targetingBonus[i] = 0x32;
        else
            m_targetingBonus[i] = 0;
        m_targetingBonusStr[i].clear();
    }
    m_keepDistance = 0x64;
    m_intelligence = 0x64;
    m_field1f0 = 0x64;
    m_field1f4 = 0x1388;
    m_vision = 0x5dc;
    m_backVision = 0x5dc;
    m_noChangeTarget = true;
}

STAICharacterCommonScript::STAICharacterCommonScript()
{
    // ORIG C1 0x088c8a78：vptr + 逐成员构造后调 clear()。
    // 成员构造由编译器生成；m_status 无用户 ctor，由 clear 内 Clear() 补齐。
    clear();
}

STAICharacterCommonScript::~STAICharacterCommonScript()
{
    // ORIG D1 0x088c903c：vptr + destroy() + 成员逆序析构（编译器生成）。
    destroy();
}

void STAICharacterCommonScript::destroy()
{
    // ORIG 0x088c98bc：仅转发 STAIInfoScript::destroy。
    m_info.destroy();
}

void STAICharacterCommonScript::clear()
{
    // ORIG 0x088c9592（objdump 逐条转写，保持调用次序与默认值）。
    m_minimumInfo.clear();               // MinimumInfo(+4).clear
    m_status.Clear();                    // STRDCharacterStatus(+0xc).Clear
    m_equipment.clear();                 // +0x64
    m_skills[0].clear();                 // +0x70
    m_skills[1].clear();                 // +0x7c
    m_corrections.clear();               // +0x88
    m_items[0].clear();                  // +0x94
    m_items[1].clear();                  // +0xa0
    m_info.clear();                      // +0xac
    m_keyStreamFiles.clear();            // +0x2d0
    m_keyFiles.clear();                  // +0x2dc
    m_keyCooltimes.clear();              // +0x2e8
    m_rarity = 0;                        // +0xd80
    m_appearancePoint = 0;               // +0xd84
    m_deathTowerItems.clear();           // +0xd88
    m_partyMembers.clear();              // +0xd94
    m_farAttackReactionRate = 0;         // +0xda0
    m_pvpWinPointLow = 0;                // +0xda4
    m_pvpWinPointHigh = 0;               // +0xda8
    m_championElements.clear();          // +0xf20（ORIG 先于 StatusRate 清理）
    m_statusRate.Clear();                // +0xdac
    m_customSkills.clear();              // +0xe50
    m_attackDamageRate = 1.0f;           // +0xe5c
    m_addEquipStatusFromLevel = 0;       // +0xe60
    m_armorSubtype = 2;                  // +0xe64
    m_defaultEquipment = 1;              // +0xe68
    m_dollSpecialDungeons.clear();       // +0xe6c
    m_specialStatusRate.Clear();         // +0xe78
    m_attackDamageRateSpecial = 1.0f;    // +0xf1c
    m_independentDrops.clear();          // +0xf2c
    m_onAttackSpeech.clear();            // +0xd68
    m_onKeySpeech.clear();               // +0xd74
    m_limitPassiveObjects.clear();       // +0xf38
    m_overheadGaugeType = 0;             // +0xf44
    m_overheadGaugePosX = 0;             // +0xf48
    m_overheadGaugePosY = 0;             // +0xf4c
    m_avatarDrop.clear();                // +0xf50
    m_getupInvincible = 1;               // +0xf8c = 1
    m_skillDataUps.clear();              // +0xf90
    m_summonStatusRates.clear();         // +0xf9c
    m_leaveBody = true;                  // byte +0xfa8 = 1
    m_addGravityInAerial = false;        // byte +0xff4 = 0
    m_gravityAerialA = 20.0f;            // +0xff8 = 0x41a00000
    m_gravityAerialB = -300.0f;          // +0xffc = 0xc3960000（-300.0）
    // ORIG 0x088c9819..0x088c9898 逐条序：1000 → 1008 → 1004 → POD 组
    // （POD 写序 0,1,7,4,6,5,2,3 非单调——原码语句序照抄）→ 100c → 1010。
    m_noCooltime = 0;                    // +0x1000
    m_npcId = -1;                        // +0x1008
    m_checkAccountItem = true;           // byte +0x1004 = 1
    m_info.m_pod204[0] = 0;
    m_info.m_pod204[1] = 0;
    m_info.m_pod204[7] = 0;
    m_info.m_pod204[4] = 0;
    m_info.m_pod204[6] = 0;
    m_info.m_pod204[5] = 0;
    m_info.m_pod204[2] = 0;
    m_info.m_pod204[3] = 0;
    m_isHellMonster = false;             // byte +0x100c = 0
    m_animationJob = 11;                 // +0x1010 = 0xb
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
bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel);

bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel)
{
    // ORIG 0x08a390e7 段：入口（byLevel spill 之后）即 `call _ZNSsC1Ev` 构造 tok，
    // 紧随 `movb $0x0,-0x21` 置 isType —— 两个局部**声明在函数头**（P0：槽图基线；
    // isType@-0x21 / tok@-0x44 为其后全部局部的分配基准）。放分支内会整体浅移。
    std::string tok;
    bool isType = false;
    // ORIG 0x08a391xx 入口 `movb $0x0,-0x21(%ebp)`（P198① 预清必写）⇒ 函数头另有
    // 一个 bool；[targeting bonus] 两处 `mov %al,-0x21` 即写它（复用同一变量）。
    bool ok = false;
    if (*tag == "[think term]")
    {
        info->m_thinkTerm = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[destination change term]")
    {
        info->m_destinationChangeTerm = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[ai pattern]")
    {
        int bucket = 0;
        while (ScanTypeOrStr(isType, tok, true))
        {
            if (isType)
            {
                if (tok == "[/ai pattern]")
                    break;
                if (tok == "[easy]")         bucket = 0;
                else if (tok == "[medium]")  bucket = 1;
                else if (tok == "[hard]")    bucket = 2;
                else if (tok == "[ultimate]") bucket = 3;
                else if (tok == "[hero]")    bucket = 4;
            }
            else
            {
                // ORIG 0x08a39280：直落块是 !byLevel（makeAIFullPath），byLevel
                // 块在跳转目标处（`movzbl;xor $1;test;je` 布局）⇒ 块序交换。
                if (!byLevel)
                    makeAIFullPath(info->m_dir, tok);
                else
                    tok = info->m_dir + tok;   // ORIG: operator+ 后赋回
                std::string* name = new std::string(tok);   // ORIG operator new(4) + string copy
                info->m_patterns[bucket].push_back(name);
            }
        }
        return true;
    }
    if (*tag == "[no change target]")
    {
        info->m_noChangeTarget = (ScanInt((bool*)0) != 0);
        return true;
    }
    if (*tag == "[targeting bonus]")
    {
        for (int i = 0; i <= 21; ++i)
        {
            info->m_targetingBonus[i] = ScanInt((bool*)0);
            if (i == 20)
            {
                std::string s;
                // ORIG 0x08a3940e：`call ScanStr; mov %al,-0xa5(%ebp)`（-0xa5 = 函数级
                // `ok`）+ `movzbl; xor $1; test; jne <exit>` ⇒ 原码把 ScanStr 结果写回
                // **复用**的函数级 ok（P0 槽往返；直写 `while (ScanStr(&s))` 被 4.4
                // 折成裸 `test %al,%al; je`，少 mov+取反）。
                ok = ScanStr(&s);
                // [C05④ 步骤 2 实测·取反形回退] `while (true) { if (!ok) break; … }` 亦试过：
                // our 473→**478**（gap 2→−3，劣 5 条；A17 探针的 `if(!r)` 取反形在此
                // body 下不成立）⇒ 保留 `while (ok)` 具名形（gap=2，本件最优）。
                while (ok)
                {
                    // ORIG 0x08a39463：实参右值 → push_back(int&&)。
                    info->m_targetingBonusStr[i].push_back(
                        stringToInt(s.c_str(), ACTIVEOBJECT_STATE_STRING, -1));
                    ok = ScanStr(&s);
                }
            }
            else if (i == 21)
            {
                std::string s;
                ok = ScanStr(&s);                    // 同上（ORIG 0x08a394a9）
                while (ok)
                {
                    info->m_targetingBonusStr[i].push_back(
                        stringToInt(s.c_str(), ACTIVESTATUS_ATTACK_TAG, 0x13));
                    ok = ScanStr(&s);
                }
            }
        }
        return true;
    }
    if (*tag == "[keep distance with target]")
    {
        info->m_keepDistance = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[keep range distance with target]" ||
        *tag == "[keep range distance with target 1]" ||
        *tag == "[keep range distance with target 2]" ||
        *tag == "[keep range distance with target 3]" ||
        *tag == "[keep range distance with target 4]" ||
        *tag == "[keep range distance with target 5]")
    {
        // ORIG 0x08a3961c..0x08a396b3：4 条逐句写 m_keepRange0..3（非循环）。
        info->m_keepRange0.push_back(ScanInt((bool*)0));
        info->m_keepRange1.push_back(ScanInt((bool*)0));
        info->m_keepRange2.push_back(ScanInt((bool*)0));
        info->m_keepRange3.push_back(ScanInt((bool*)0));
        return true;
    }
    if (*tag == "[warlike]")
    {
        info->m_intelligence = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[intelligence]")
    {
        info->m_intelligence = ScanInt((bool*)0);
        info->m_field1f0 = ScanInt((bool*)0);
        info->m_field1f4 = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[vision]")
    {
        info->m_vision = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[back vision]")
    {
        info->m_backVision = ScanInt((bool*)0);
        return true;
    }
    return false;
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
bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel);

bool importAICharacterCommonScript(STAICharacterCommonScript* script,
                                   const char* path, bool byLevel)
{
    if (!loadRDARScriptFile(g_aiCharacterScriptBaseDirectory, path))
        return false;

    script->clear();
    getDirectoryPath(script->m_info.m_dir, "%s%s",
                     g_aiCharacterScriptBaseDirectory, path);

    std::string tag;
    std::string tok;
    bool ok = false;

    while (true)
    {
        if (!ScanType(tag, true))
            return true;                       // LAB_088cc01f: ebx=1

        if (tag == "[minimum info]")
        {
            // ORIG 0x088c9dd0..0x088c9eaf：ScanStr→m_name=tok + 14 个逐条
            // ScanInt（第 5 个写 m_field4），无循环合并（P183 物化口径）。
            ScanStr(&tok);
            script->m_minimumInfo.m_name = tok;
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            script->m_minimumInfo.m_field4 = ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanStr(&tok);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
        }
        else if (tag == "[NPC]")
        {
            script->m_npcId = ScanInt((bool*)0);          // +0x1008
        }
        else if (tag == "[additional character status]")
        {
            STRDCharacterStatus& st = script->m_status;
            st.m_hpMax       = ScanInt((bool*)0);         // +0x00
            st.m_mpMax       = ScanInt((bool*)0);         // +0x04
            st.m_physAttack  = (unsigned short)ScanInt((bool*)0);  // +0x08
            st.m_physDefense = (unsigned short)ScanInt((bool*)0);  // +0x0a
            st.m_magAttack   = (unsigned short)ScanInt((bool*)0);  // +0x0c
            st.m_magDefense  = (unsigned short)ScanInt((bool*)0);  // +0x0e
            st.m_inventoryLimit = ScanInt((bool*)0);      // +0x3a
            st.m_hpRegen     = (unsigned short)ScanInt((bool*)0);  // +0x3e
            st.m_mpRegen     = (unsigned short)ScanInt((bool*)0);  // +0x40
            st.m_moveSpeed   = ScanInt((bool*)0);         // +0x42
            st.m_attackSpeed = (unsigned short)ScanInt((bool*)0);  // +0x46
            st.m_castSpeed   = (unsigned short)ScanInt((bool*)0);  // +0x48
            st.m_hitRecovery = (unsigned short)ScanInt((bool*)0);  // +0x4a
            st.m_jumpPower   = (unsigned short)ScanInt((bool*)0);  // +0x4c
            st.m_weight      = ScanInt((bool*)0);         // +0x4e
            st.m_field52     = (unsigned char)ScanInt((bool*)0);   // +0x52
            for (int j = 0; j <= 3; ++j)                  // elemResist @+0x10
                st.m_elemResist[j] = (unsigned short)ScanInt((bool*)0);
            for (int n = 0; ; ++n)                        // activeResist @+0x18
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                st.m_activeResist[n] = (short)v;      // ORIG 0x088ca0bd 无界写，照抄（P171）
            }
        }
        else if (tag == "[equipment]")
        {
            while (true)
            {
                STAICharacterEquipmentData e;
                e.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                e.m_b = ScanInt((bool*)0);
                e.m_c = ScanInt((bool*)0);
                script->m_equipment.push_back(e);
            }
        }
        else if (tag == "[skill]" || tag == "[quick skill]")
        {
            while (true)
            {
                STAICharacterSkillData s;
                s.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                s.m_b = ScanInt((bool*)0);
                if (tag == "[quick skill]")
                    script->m_skills[0].push_back(s);      // +0x70
                else
                    script->m_skills[1].push_back(s);      // +0x7c
            }
        }
        else if (tag == "[item]" || tag == "[quick item]")
        {
            while (true)
            {
                STAICharacterItemData it;
                it.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                it.m_b = ScanInt((bool*)0);
                if (tag == "[quick item]")
                    script->m_items[0].push_back(it);      // +0x94
                else
                    script->m_items[1].push_back(it);      // +0xa0
            }
        }
        else if (tag == "[skill correction]")
        {
            while (true)
            {
                const int first = ScanInt(&ok);
                if (!ok)
                    break;
                STAICharacterSkillCorrection c;
                c.m_skill = first;
                c.m_rate = ScanFloat((bool*)0);
                script->m_corrections.push_back(c);
            }
        }
        else if (tag == "[key stream]")
        {
            bool again = true;
            while (again)
            {
                again = false;
                std::string keyFile;
                bool keyOk = ScanStr(&keyFile);
                if (!keyOk)
                    break;
                if (isKeyFile(keyFile.c_str()))
                {
                    tok = keyFile;
                    keyFile = "";                      // ORIG 0x088ca432 operator=(const char*)
                }
                else
                {
                    keyOk = ScanStr(&tok);
                }
                if (*keyFile.c_str() == '\0')          // ORIG 0x088ca456 c_str+testb（非 empty()）
                    keyFile = getFileName(tok);
                script->m_keyFiles.push_back(keyFile);
                if (keyOk)
                {
                    makeKeyFullPath(script->m_info.m_dir, tok);
                    script->m_keyStreamFiles.push_back(tok);
                    again = true;
                }
            }
        }
        else if (tag == "[key cooltime]")
        {
            while (ScanStr(&tok))
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_keyCooltimes[tok] = v;
            }
        }
        else if (tag == "[rarity]")
        {
            if (!ScanStr(&tok))
                return false;
            if (tok == "common")         script->m_rarity = 0;
            else if (tok == "uncommon")  script->m_rarity = 1;
            else if (tok == "rare")      script->m_rarity = 2;
            else if (tok == "unique")    script->m_rarity = 3;
            else if (tok == "epic")      script->m_rarity = 4;
            else if (tok == "chronicle") script->m_rarity = 5;
            script->m_rarity = getItemRarity(tok.c_str());
            if (script->m_rarity == 6)
                return false;
        }
        else if (tag == "[appearance point]")
        {
            script->m_appearancePoint = ScanInt((bool*)0);
        }
        else if (tag == "[death tower item]")
        {
            script->m_deathTowerItems.clear();
            while (true)
            {
                int first = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG 0x088ca81c/0x088ca837：pair<int&,int> 模板 ctor（第二参为
                // 右值实参）→ push_back(T&&)。
                script->m_deathTowerItems.push_back(
                    std::pair<int, int>(first, ScanInt((bool*)0)));
            }
        }
        else if (tag == "[leave body]")
        {
            script->m_leaveBody = (ScanInt((bool*)0) != 0);
        }
        else if (tag == "[waiting action]")   { ScanStr(&script->m_actionSet.m_waiting); }
        else if (tag == "[move action]")      { ScanStr(&script->m_actionSet.m_move); }
        else if (tag == "[dash action]")      { ScanStr(&script->m_actionSet.m_dash); }
        else if (tag == "[sit action]")       { ScanStr(&script->m_actionSet.m_sit); }
        else if (tag == "[damage action 1]")  { ScanStr(&script->m_actionSet.m_damage[0]); }
        else if (tag == "[damage action 2]")  { ScanStr(&script->m_actionSet.m_damage[1]); }
        else if (tag == "[down action]")      { ScanStr(&script->m_actionSet.m_down); }
        else if (tag == "[overturn action]")  { ScanStr(&script->m_actionSet.m_overturn); }
        else if (tag == "[jump action]")      { ScanStr(&script->m_actionSet.m_jump); }
        else if (tag == "[jumpattack action]"){ ScanStr(&script->m_actionSet.m_jumpattack); }
        else if (tag == "[attack action]" || tag == "[action list]")
        {
            script->m_actionSet.m_attackList.clear();
            std::string line;
            while (ScanStr(&line))
                script->m_actionSet.m_attackList.push_back(line);
        }
        else if (tag == "[etc action]" || tag == "[custom action]")
        {
            script->m_actionSet.m_etc.clear();
            std::string line;
            while (ScanStr(&line))
                script->m_actionSet.m_etc.push_back(line);
        }
        else if (tag == "[last action]")  { ScanStr(&script->m_lastAction); }
        else if (tag == "[proc action]")  { ScanStr(&script->m_procAction); }
        else if (tag == "[add gravity in aerial]")
        {
            script->m_addGravityInAerial = (ScanInt((bool*)0) != 0);
            script->m_gravityAerialA = ScanFloat((bool*)0);
            script->m_gravityAerialB = ScanFloat((bool*)0);
        }
        else if (tag == "[speech on situation]")
        {
            std::string line;
            for (int i = 0; i <= 0x6e; ++i)               // ORIG cmpl $0x6e; setle（P51）
                script->m_speeches[i].clear();           // 24*i + 0x300
            script->m_onAttackSpeech.clear();            // +0xd68
            script->m_onKeySpeech.clear();               // +0xd74
            while (true)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, tok, true))
                    break;                               // 段结束（esi=1）
                if (tok == "[/speech on situation]")     // ORIG 不查 isType 标志
                    break;
                ScanStr(&line);
                const int prob = ScanInt((bool*)0);
                // [C05 F2/P310 一次性旋钮实测·已回退] `unsigned slot` 确使尾段谓词
                // 对齐 ORIG 的 `cmpl $0x6e,-0x44; ja`（内存直比；widthscan 的
                // cmp_l orig=8 our=7 差消失），但 `(std::size_t)slot+1` 三处使用点
                // 因无符号语义各多 1 条 ⇒ our_insns 2108→2111、gap −3→−6（净负）
                // ⇒ 按 P321 S1 恢复直证形态并封 N（本旋钮不可用，勿再试）。
                int slot = -1;                            // ORIG 0x088cadb2 先置 -1
                if (tok == "[on attack]")
                {
                    // ORIG 0x088cad83..0x088caeed：独立块（与 [on key] 不共享代码）。
                    // ORIG 0x088cad8d 只 lea 取址、全函数无 fv 初始化写 ⇒ 原码无初值
                    // （P0/P171：不写即不写，`= 0.0f` 反生 `mov $0,%eax;mov %eax,槽`）。
                    float fv;
                    // P251：返回值无栈槽往返 + 多路相等比较 ⇒ 原码是 switch
                    // （ORIG 0x088cad9b `test %eax,%eax;je` + `cmp $0x1,%eax;je` + `jmp`）。
                    switch (ScanFloatStr(fv, tok))
                    {
                    case 0:
                        slot = (int)fv;
                        break;
                    case 1:
                        for (std::size_t i = 0; i < script->m_keyFiles.size(); ++i)
                        {
                            if (script->m_keyFiles[i] == tok)
                            {
                                slot = (int)i;
                                break;
                            }
                        }
                        break;
                    }
                    if (script->m_onAttackSpeech.size() < (std::size_t)slot + 1)
                        script->m_onAttackSpeech.resize((std::size_t)slot + 1, APCSpeech());
                    script->m_onAttackSpeech[(std::size_t)slot].add(line, prob);
                }
                else if (tok == "[on key]")
                {
                    // 同 [on attack]：无初值 + switch（ORIG 0x088caf1a/0x088caf28）。
                    float fv;
                    switch (ScanFloatStr(fv, tok))
                    {
                    case 0:
                        slot = (int)fv;
                        break;
                    case 1:
                        for (std::size_t i = 0; i < script->m_keyFiles.size(); ++i)
                        {
                            if (script->m_keyFiles[i] == tok)
                            {
                                slot = (int)i;
                                break;
                            }
                        }
                        break;
                    }
                    if (script->m_onKeySpeech.size() < (std::size_t)slot + 1)
                        script->m_onKeySpeech.resize((std::size_t)slot + 1, APCSpeech());
                    script->m_onKeySpeech[(std::size_t)slot].add(line, prob);
                }
                else
                {
                    if (tok == "[on damage]")      slot = 0x65;
                    else if (tok == "[on die]")    slot = 0x66;
                    else if (tok == "[on revenge]") slot = 0x67;
                    else if (tok == "[on etc]")    slot = 0x68;
                    else if (tok == "[on appear]") slot = 0x69;
                    else if (tok == "[on disappear]") slot = 0x6a;
                    else if (tok == "[on go to help]") slot = 0x6b;
                    else if (tok == "[on stand]")  slot = 0x6c;
                    else
                        return false;                // ORIG: esi=0 → 返回失败
                    if ((unsigned)slot <= 0x6e)      // ORIG cmpl $0x6e,-0x44; ja（无符号）
                        script->m_speeches[slot].add(line, prob);
                }
            }
        }
        else if (tag == "[common champion elemental property]")
        {
            script->m_championElements.clear();
            while (ScanStr(&tok))
            {
                for (int e = 0; e < 5; ++e)
                {
                    if (tok == getElementName((ENUM_ELEMENT)e))
                    {
                        script->m_championElements.push_back(e);
                        break;
                    }
                }
            }
        }
        else if (tag == "[party member]")
        {
            script->m_partyMembers.clear();
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                int b = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG 0x088cb399：make_pair<int&,int&> sret + push_back 嵌套实参（P178）。
                script->m_partyMembers.push_back(std::make_pair(a, b));
            }
        }
        else if (tag == "[far attack reaction rate]")
        {
            const float f = ScanFloat(&ok);
            // ORIG flds [_DAT_08e0a1e0]=100.0f; fmulp; fldcw(截断); fistp
            script->m_farAttackReactionRate = (int)(f * 100.0f);
        }
        else if (tag == "[pvp win point]")
        {
            script->m_pvpWinPointLow = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_pvpWinPointHigh = ScanInt(&ok);
            if (!ok)
                return false;
            if (script->m_pvpWinPointHigh < script->m_pvpWinPointLow)
                return false;
        }
        else if (tag == "[character status rate]")
        {
            // ORIG 0x088cb4ed..0x088cb703：head 10 条 + tail 10 条逐条单写
            // （fstps 0xdac.. / 0xe28..），随后 for≤3 写 mid28、while 无界写 mid38。
            STRDCharacterStatusRate& r = script->m_statusRate;
            r.m_head[0] = ScanFloat((bool*)0);
            r.m_head[1] = ScanFloat((bool*)0);
            r.m_head[2] = ScanFloat((bool*)0);
            r.m_head[3] = ScanFloat((bool*)0);
            r.m_head[4] = ScanFloat((bool*)0);
            r.m_head[5] = ScanFloat((bool*)0);
            r.m_head[6] = ScanFloat((bool*)0);
            r.m_head[7] = ScanFloat((bool*)0);
            r.m_head[8] = ScanFloat((bool*)0);
            r.m_head[9] = ScanFloat((bool*)0);
            r.m_tail[0] = ScanFloat((bool*)0);
            r.m_tail[1] = ScanFloat((bool*)0);
            r.m_tail[2] = ScanFloat((bool*)0);
            r.m_tail[3] = ScanFloat((bool*)0);
            r.m_tail[4] = ScanFloat((bool*)0);
            r.m_tail[5] = ScanFloat((bool*)0);
            r.m_tail[6] = ScanFloat((bool*)0);
            r.m_tail[7] = ScanFloat((bool*)0);
            r.m_tail[8] = ScanFloat((bool*)0);
            r.m_tail[9] = ScanFloat((bool*)0);
            for (int t = 0; t <= 3; ++t)
                r.m_mid28[t] = ScanFloat((bool*)0);      // (t+0x370)*4+0x14 → Rate+0x28+4t
            for (int n = 0; ; ++n)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                // ORIG 0x088cb6cc 无界连续写 (n+0x374)*4+0x14 → Rate+0x38+4n；照抄（P171）。
                r.m_mid38[n] = f;
            }
        }
        else if (tag == "[custom skill level]")
        {
            while (true)
            {
                STAICharacterCustomSkillLevel c;
                c.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                c.m_b = ScanInt((bool*)0);
                c.m_c = ScanInt((bool*)0);
                c.m_d = ScanInt((bool*)0);
                script->m_customSkills.push_back(c);
            }
        }
        else if (tag == "[attack damage rate]")
        {
            script->m_attackDamageRate = ScanFloat(&ok);
        }
        else if (tag == "[add equipment status from level]")
        {
            script->m_addEquipStatusFromLevel = ScanInt(&ok);
        }
        else if (tag == "[armor subtype]")
        {
            script->m_armorSubtype = ScanInt(&ok);
        }
        else if (tag == "[default equipment]")
        {
            script->m_defaultEquipment = ScanInt(&ok);
        }
        else if (tag == "[doll special dungeon]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_dollSpecialDungeons.push_back(v);
            }
        }
        else if (tag == "[character status rate special]")
        {
            // ORIG 0x088cb8da..（链位在 doll 之后）：special 独立块，e78.. / ef4..
            // 各 10 条逐条单写（fstps），for≤3 mid28、while 无界 mid38。
            STRDCharacterStatusRate& r = script->m_specialStatusRate;
            r.m_head[0] = ScanFloat((bool*)0);
            r.m_head[1] = ScanFloat((bool*)0);
            r.m_head[2] = ScanFloat((bool*)0);
            r.m_head[3] = ScanFloat((bool*)0);
            r.m_head[4] = ScanFloat((bool*)0);
            r.m_head[5] = ScanFloat((bool*)0);
            r.m_head[6] = ScanFloat((bool*)0);
            r.m_head[7] = ScanFloat((bool*)0);
            r.m_head[8] = ScanFloat((bool*)0);
            r.m_head[9] = ScanFloat((bool*)0);
            r.m_tail[0] = ScanFloat((bool*)0);
            r.m_tail[1] = ScanFloat((bool*)0);
            r.m_tail[2] = ScanFloat((bool*)0);
            r.m_tail[3] = ScanFloat((bool*)0);
            r.m_tail[4] = ScanFloat((bool*)0);
            r.m_tail[5] = ScanFloat((bool*)0);
            r.m_tail[6] = ScanFloat((bool*)0);
            r.m_tail[7] = ScanFloat((bool*)0);
            r.m_tail[8] = ScanFloat((bool*)0);
            r.m_tail[9] = ScanFloat((bool*)0);
            for (int t = 0; t <= 3; ++t)
                r.m_mid28[t] = ScanFloat((bool*)0);      // (t+0x3a4)*4+0x10 → Rate+0x28+4t
            for (int n = 0; ; ++n)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                r.m_mid38[n] = f;                        // ORIG 无界写，照抄（P171）
            }
        }
        else if (tag == "[attack damage rate special]")
        {
            script->m_attackDamageRateSpecial = ScanFloat(&ok);
        }
        else if (tag == "[attack stuck rate]")
        {
            // ORIG fstps 0x5f —— 绝对 +0x5f = STRDCharacterStatus(+0xc)+0x53 = m_field53
            *reinterpret_cast<float*>(&script->m_status.m_field53) = ScanFloat(&ok);
        }
        else if (tag == "[limit passiveobject]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_limitPassiveObjects.push_back(v);
            }
        }
        else if (tag == "[overhead gauge type]")
        {
            script->m_overheadGaugeType = ScanInt((bool*)0);
        }
        else if (tag == "[overhead gauge pos]")
        {
            script->m_overheadGaugePosX = ScanInt((bool*)0);
            script->m_overheadGaugePosY = ScanInt((bool*)0);
        }
        else if (tag == "[getup invincible]")
        {
            script->m_getupInvincible = ScanInt((bool*)0);
        }
        else if (tag == "[drop avatar item]")
        {
            for (int i = 0; i <= 3; ++i)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_rates08[i] = f;
            }
            for (int k = 0; k <= 4; ++k)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_rates18[k] = f;
            }
            script->m_avatarDrop.m_field0 = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_avatarDrop.m_field4 = ScanInt(&ok);
            if (!ok)
                return false;
            while (true)
            {
                stEnamyDropAvatarElement_t el;
                el.m_itemId = ScanInt(&ok);
                if (!ok)
                    break;
                el.m_total = ScanInt(&ok);
                if (!ok)
                    return false;
                el.m_flag = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_total += el.m_total;
                el.m_total = script->m_avatarDrop.m_total;   // 回填累计值
                script->m_avatarDrop.m_elements.push_back(el);
            }
        }
        else if (tag == "[skill data up]")
        {
            importSkillDataUp(script->m_skillDataUps);
        }
        else if (tag == "[summon monster status rate]")
        {
            while (true)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                script->m_summonStatusRates.push_back(f);
            }
        }
        else if (tag == "[no cooltime]")
        {
            script->m_noCooltime = 1;
        }
        else if (tag == "[check accountitem]")
        {
            script->m_checkAccountItem = (ScanInt((bool*)0) == 1);
        }
        else if (tag == "[hell monster]")
        {
            // ORIG 0x088cbfc3/0x088cbfc6：结果先落具名 int 槽（-0x10）再
            // `cmpl $0x0,-0x10(%ebp); setne %dl` ⇒ 原码有具名局部（P0 槽往返律）；
            // 直接 `(ScanInt(...) != 0)` 会被 4.4 折成 `test %eax,%eax`（少 mov+cmp）。
            const int v = ScanInt((bool*)0);
            script->m_isHellMonster = (v != 0);
        }
        else if (tag == "[animation job]")
        {
            const int v = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_animationJob = v;
        }
        else
        {
            if (!importAIInfoScript(&script->m_info, &tag, false))
                return false;
        }

        if (byLevel)
            break;                       // ORIG do{}while(!param_3)
    }
    return true;
}
// ============================================================================
// isExistAiPattern —— ORIG T 0x08a2232f
// 报告 _lt_global_gt_/isExistAiPattern.md 完整转写。
// bucket==-1 遍历 5 个难度桶；否则只查指定桶；任一非空返 1，否则 0。
bool isExistAiPattern(STAIInfoScript* info, int bucket)   // ORIG 调用侧 test %al 直判（P9/P22）
{
    if (info != 0)
    {
        if (bucket != -1)
        {
            if (!info->m_patterns[bucket].empty())
                return 1;
        }
        else
        {
            for (int i = 0; i < 5; ++i)
            {
                if (!info->m_patterns[i].empty())
                    return 1;
            }
        }
    }
    return 0;
}

// ============================================================================
// exportAIPatternPath —— ORIG T 0x08a223b0
// 报告 _lt_global_gt_/exportAIPatternPath.md 完整转写。
// 遍历指定桶内模式名，截掉 m_dir 目录前缀后 fprintf 每行 "`%s`\n"。
// ============================================================================
void exportAIPatternPath(FILE* f, STAIInfoScript* info, int bucket)
{
    std::vector<std::string*>::iterator it;
    std::string out;
    it = info->m_patterns[bucket].begin();
    while (it != info->m_patterns[bucket].end())
    {
        out = (*it)->substr(info->m_dir.length(), (*it)->length());
        fprintf(f, "`%s`\n", out.c_str());
        ++it;
    }
}

// ============================================================================
// getRandomChampionElement —— ORIG T 0x088b9d54（STAICharacterCommonScript 版；
// STMonsterScript 版在 STMonsterScript.cpp）。报告
// _lt_global_gt_/getRandomChampionElement_088b9d54.md 完整转写。
// m_championElements(+0xf20) 非空 → rand()%size 取元素；空 → rand()%4+1。
// ============================================================================
int getRandomChampionElement(const STAICharacterCommonScript* script, int seed)
{
    int result = 4;
    const int size = (int)script->m_championElements.size();
    srand(seed);
    if (size == 0)
    {
        result = rand() % 4 + 1;
    }
    else
    {
        result = script->m_championElements[rand() % size];
    }
    return result;
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

CNPCDynamicInfo::CNPCDynamicInfo(const CNPCScript* script) throw()
    : m_script(script), m_moodState(3), m_field8(0x7fffffff), m_fieldc(0)
{
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
void PowerParameterScript::clear()
{
    m_powerJoinCost = 0;               // +0x00
    m_powerJoinGuildLevel = 0;         // +0x04
    m_powerJoinQuest = 0;              // +0x08
    m_killPointToEndPowerWar = 0;      // +0x0c
    m_expReward = 0;                   // +0x10
    m_ghostTime = 0;                   // +0x14
    m_powerUpRatio = 0;                // +0x1c
    m_refreshProcessInfo = 0;          // +0x30
    m_abuserBanTime = 0;               // +0x34
    m_powerUpGeneratePeriod = 0;       // +0x48
    m_powerUpAssaultCount = 0;         // +0x4c
    m_powerUpAssaultVictoryRatio = 0;  // +0x50
    m_powerWarPointMultiple = 0;       // +0x54
    m_vec20.clear();                   // +0x20
    m_straightVictories = 0;           // +0x58
    m_timeAssaultSuperPowerStateOnJoin = 5;  // +0x5c（ORIG clear 默认 5）
    m_timeAssaultSuperPowerStateAfter = 5;   // +0x60（ORIG clear 默认 5）
    m_vec70.clear();                   // +0x70
    m_basicPowerWarPoint = 0;          // +0x7c
    m_firstGradeBonusPoint = 0;        // +0x80
    m_rankingBonusGrade = 0;           // +0x84
    m_rankingBonusDecreasePoint = 0;   // +0x88
    m_decreasePointPerDay = 0;         // +0x8c
    m_requiredUserPoint4Dungeon = 0;   // +0x90
    m_powerWarDungeonInfo.first = 0;   // +0x94（ORIG pair 子对象）
    m_powerWarDungeonInfo.second = 0; // +0x98
    m_vec38.clear();                    // +0x38
    m_pcRoomBonus = 0;                 // +0x44
    m_minEnterLevel = 0;               // +0x64
    m_minEnterAge = 0;                 // +0x68
    m_powerUpPointMod = 0;             // +0xa8
    m_vecAC.clear();                   // +0xac
    m_powerWarCooldownMinutes = 0;     // +0xb8
    m_vecBC.clear();                   // +0xbc
    m_vecC8.clear();                   // +0xc8
    m_victoriousRewardTime = 0;        // +0xd4
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

CAICharacter::CAICharacter()
    : m_dropItems(), m_independentDrops(), m_avatarDrops(), m_name()
{
}

CAICharacter::~CAICharacter()
{
}

// ---- force 发射 APCSpeech::add（ORIG W 0x088cc3b2）----
namespace {
void forceApcSpeechAddEmission()
{
    APCSpeech sp;
    std::string line;
    sp.add(line, 0);
}
}  // namespace
