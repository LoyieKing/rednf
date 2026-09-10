// ============================================================================
// CCompoundAvatar.h — 头像合成/分离系统脚本域（ORIG df_game_r 实证布局）
//
// 依据 docs/class_func_reports/{WongWork__CCompoundAvatar,tagCompoundAvatarTable}/
// 与 ORIG 反汇编（ctor 0x08331d74 / Clean 0x0891ab56 / vector resize 0x0891e51c
// 的 imul $0x308 元素尺寸实证）：
//   STAncientRareAvatarCube      0xac（C1 W 0x0891aa3a 显式清零；copy/D1 为编译器
//                                生成的逐成员语义，与 ORIG W 等价）
//   tagCompoundAvatar            0x308（C1/copy/Clean/operator= 全 W 符号；除 Clean
//                                外均为逐成员语义 → 不显式声明特member，由编译器生成）
//   tagCompoundAvatarTable       = { vector<tagCompoundAvatar> } 0xc
//                                  （C1 W 0x0833431c / D1 W 0x08334330）
//   WongWork::CCompoundAvatar    0x88：11×table@+0x00..+0x83 + CMTRand*@+0x84
//                                  （C1 T 0x08331d74 / D1 T 0x08331ea2 /
//                                   LoadScript T 0x08333ea6）
// importCompoundAvatarTable     T 0x08903ea7（声明；定义归脚本导入域批次）
// ============================================================================
#ifndef GAME_CCOMPOUND_AVATAR_H_
#define GAME_CCOMPOUND_AVATAR_H_

#include <cstddef>
#include "Inven_Item.h"   // G7 miscB 追加：tagCompoundInfo 按值 Inven_Item 成员（0x3d，pack(1)）
#include "MSG_COMPOUND_AVATAR.h"  // 合成头像请求包权威布局（唯一声明点；原仅前向声明）

class CItem;              // find_item 结果指针（tagCompoundInfo +0x88/+0x8c）
class CUser;
class CMTRand;            // PvPTypes.h（0x9c8 MT19937）

// G7 miscB 追加：SendCmdErrorPacket/PacketGuard 头内声明所需枚举（guard 与权威头一致）。
#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

#include <map>
#include <utility>
#include <vector>

// ProcTest 形参枚举（与 STEquipmentScript.h/Quest.h 同 guard，唯一定义）。
#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

#ifndef DNF_ENUM_EQUIPMENTTYPE_DEFINED
#define DNF_ENUM_EQUIPMENTTYPE_DEFINED
enum ENUM_EQUIPMENTTYPE
{
    ENUM_EQUIPMENTTYPE_0 = 0
};
#endif


// ---- 远古稀有头像立方（ORIG C1 0x0891aa3a：单循环「置零 int + clear 向量」，
//      末尾 clear 首向量；其余生命周期为逐成员语义）----
struct STAncientRareAvatarCube
{
    STAncientRareAvatarCube();

    std::vector<std::pair<int, int> > m_vec;        // +0x00
    // +0x0c（ctor 置 0）；类型由 setCompoundRareAvatarData ORIG 0x08902e78 实证为
    // unsigned：权重加法段 `mov field,%ecx; mov weight,%eax; add %eax,%ecx` 只在
    // 字段 unsigned + RHS int 时出现（P274），且 ORIG push_back 的 pair converting
    // ctor 符号为 `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`（第二参 unsigned int&）。
    unsigned int m_field0c[10];                     // +0x0c
    std::vector<std::pair<int, int> > m_vecs[10];   // +0x34
};  // sizeof == 0xac

static_assert(sizeof(STAncientRareAvatarCube) == 0xac,
              "STAncientRareAvatarCube size drift");

// ---- 单个可合成头像条目（尾垫 +0x304..+0x307 由 resize 元素尺寸 0x308 实证）----
struct tagCompoundAvatar
{
    void Clean();                                    // ORIG W 0x0891ab56

    int m_idx;                                       // +0x000（Clean 置 0）
    int m_a[10];                                     // +0x004（Clean memset 0x28）
    // [b28d-solo-ScriptDepsA] m_b/m_c/m_d 为累计权重：ORIG setCompoundAvatarData
    // 0x08902d84/0x08902e40 段 push_back 的第二实参 pair converting ctor 符号为
    // `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`（int&/unsigned int&，与 setCompoundRareAvatarData
    // 的 m_field0c 同证据）⇒ 三字段实为 unsigned int。宽度/偏移/sizeof 不变。
    unsigned int m_b[10];                            // +0x02c（Clean 置零）
    unsigned int m_c[10];                            // +0x054（Clean 置零）
    std::vector<std::pair<int, int> > m_optsA[10];   // +0x07c
    std::vector<std::pair<int, int> > m_optsB[10];   // +0x0f4
    unsigned int m_d[10];                            // +0x16c（Clean 置零）
    std::vector<std::pair<int, int> > m_optsC[10];   // +0x194
    STAncientRareAvatarCube m_cube;                  // +0x20c
    std::vector<std::pair<int, int> > m_tailA;       // +0x2b8（Clean clear）
    std::vector<std::pair<int, int> > m_tailB;       // +0x2c4（Clean clear）
    std::vector<std::pair<int, int> > m_tailC;       // +0x2d0（Clean clear）
    int m_flag2dc;                                   // +0x2dc（Clean 置 0）
    int m_e[10];                                     // +0x2e0..+0x307（Clean 置零）
};  // sizeof == 0x308

static_assert(sizeof(tagCompoundAvatar) == 0x308,
              "tagCompoundAvatar size drift");
static_assert(offsetof(tagCompoundAvatar, m_cube) == 0x20c,
              "tagCompoundAvatar cube offset drift");

// ---- 脚本表：一个 vector<tagCompoundAvatar>（ORIG C1/D1 即仅该成员生命周期）----
struct tagCompoundAvatarTable
{
    tagCompoundAvatarTable();                        // ORIG W 0x0833431c
    ~tagCompoundAvatarTable();                       // ORIG W 0x08334330

    std::vector<tagCompoundAvatar> m_avatars;        // +0x00

    // ORIG _ZNK22tagCompoundAvatarTable21getCompoundAvatarInfoEi 0x08903fcc：
    // idx<1 或 size()<=idx → 0；at(idx).m_idx != idx → 0；否则返回元素指针。
    tagCompoundAvatar* getCompoundAvatarInfo(int idx) const;
};  // sizeof == 0xc

static_assert(sizeof(tagCompoundAvatarTable) == 0xc,
              "tagCompoundAvatarTable size drift");

// [M1Free76to90 2026-09-11 · M1S2 申报] ORIG prett 串（0x08c2c540，`objdump -s` 实字节）
// 第三参数拼写作 `stCompoundAvatarTable*`，而 mangled 仍为真名 `...P22tagCompoundAvatarTable`
// ⇒ 原码在 prett 拼写处以 typedef 名书写（typedef 不入 mangling）。补齐后方能逐字节复现
// `_ZZ...__PRETTY_FUNCTION__`（size 0x8a）。
typedef tagCompoundAvatarTable stCompoundAvatarTable;

// ORIG T 0x08903ea7（另有 Core 变体 T 0x089031d2）：声明先行，定义待脚本导入域批次。
int importCompoundAvatarTable(const char* dir, const char* path,
                              tagCompoundAvatarTable* table);

namespace WongWork
{

class CCompoundAvatar
{
public:
    CCompoundAvatar();                               // ORIG T 0x08331d74
    ~CCompoundAvatar();                              // ORIG T 0x08331ea2

    // ORIG T 0x08333ea6：return importCompoundAvatarTable(dir,path,this) == 0。
    bool LoadScript(const char* dir, const char* path);
    // ORIG _ZN8WongWork15CCompoundAvatar8ProcTestE17ENUM_CHARACTERJOB18ENUM_EQUIPMENTTYPEiiRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
    // T 0x08333fda：以表 idx=1 条目按 m_optsA/m_optsB 权重抽样 10000 次统计 itemId 分布。
    void ProcTest(ENUM_CHARACTERJOB job, ENUM_EQUIPMENTTYPE equipType,
                  int addRate, int bonusFlag,
                  std::map<int, int>& result);

    // ---- G7 miscB 纯追加 ----
    // 合成请求上下文（ORIG clear 0x0833424a / C1 0x08334344 / D1 0x08334388；
    // 默认对齐（align 4，非 pack）：Inven_Item(0x3d)@+0x0a/+0x4a/+0x92 后各有
    // 1 字节对齐 pad（0x47/0x87/0xcf），CItem*@+0x88/+0x8c、vector<int>@+0xdc）。
    struct tagCompoundInfo
    {
        tagCompoundInfo();                        // 仅构造三个 Inven_Item 与 vector
        void clear();                             // 字段复位（见 ORIG W 实现）
        ~tagCompoundInfo();                       // 仅析构 vector

        union
        {
            int m_field00;                        // +0x00（clear 置 0xb；ProcCompound 取表索引）
            // [b28d-solo-WongWorkMiscB_A] P254 同址 enum 视角：ORIG 0x0833394f
            // `mov -0x100(%ebp),%eax; mov %eax,%edx` 两段装载（int 字段 4.4 恒折
            // 单段）⇒ 该字段原码为枚举类型。同宽零布局影响。
            ENUM_CHARACTERJOB m_field00Job;
        };
        int m_field04;                            // +0x04
        unsigned short m_field08;                 // +0x08
        Inven_Item m_materialA;                   // +0x0a 合成材料①
        unsigned short m_field48;                 // +0x48
        Inven_Item m_materialB;                   // +0x4a 合成材料②
        CItem* m_scriptA;                         // +0x88 材料①脚本
        CItem* m_scriptB;                         // +0x8c 材料②脚本
        unsigned short m_field90;                 // +0x90
        Inven_Item m_avatar;                      // +0x92 待合成头像槽物品
        int m_fieldD0;                            // +0xd0
        unsigned short m_slot;                    // +0xd4（clear 置 100）
        bool m_fieldD6;                           // +0xd6（clear 置 0）
        bool m_errorFlag;                         // +0xd7（脚本检索失败/不匹配置 1）
        unsigned short m_resultSlot;              // +0xd8（clear 置 0xffff）
        std::vector<int> m_results;               // +0xdc 结果 byte/short/int 三元组序列
        unsigned char m_type;                     // +0xe8（msg->+0x22 合成类型；ORIG
                                                  //   _MakeCompoundInfo +0x6d9 mov %dl,0xe8 实证；
                                                  //   分派 m_tailA/B/C/m_cube.m_vec 匹配）
    };                                            // sizeof == 0xec（默认对齐）

    // ORIG T 0x083338c2 _ZN8WongWork15CCompoundAvatar12ProcCompoundEP5CUserPK19MSG_COMPOUND_AVATAR
    // （定义 WongWorkMiscB_A.cpp）：Make→Check→Core，ENUM_ERROR 抛出转 _SendError。
    void ProcCompound(CUser* user, const MSG_COMPOUND_AVATAR* msg);
    // ORIG T 0x08333a76：SendCmdErrorPacket(user, 0x66, (unsigned char)err)。
    void _SendError(CUser* user, ENUM_ERROR err);
    // ORIG T 0x08333a9c：组 0x66 结果包（见 WongWorkMiscB_A.cpp）。
    // [b28d-solo-WongWorkMiscB_A] 返回 int：ORIG 尾 `mov $0x1,%ebx; … call D1;
    // mov %ebx,%eax` ⇒ 返回 1（P159 判据；返回类型不入 mangled，零 ABI 影响）。
    int _SendResult(CUser* user, const tagCompoundInfo& info);

    // [M1Free76to90 2026-09-11 · M1S2 申报] ORIG prett 串直证（0x08c2c540 size 0x8a）：
    // `ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*,
    //  WongWork::CCompoundAvatar::stCompoundAvatarInfo&, stCompoundAvatarTable*)`
    // ⇒ 类内 typedef（GCC prett 展开按声明拼写的 typedef 名、嵌套类型全限定）+ 返回类型
    // ENUM_ERROR。返回类型与 typedef 均不入 mangled ⇒ 符号/sizeof/偏移零影响。
    typedef tagCompoundInfo stCompoundAvatarInfo;

    // ---- ProcCompound 直接依赖（声明先行；定义归后续批次）----
    tagCompoundInfo _MakeCompoundInfo(const CUser* user,
                                      const MSG_COMPOUND_AVATAR* msg) const;  // T 0x08331efe
    int _CheckCondition(const CUser* user, const tagCompoundInfo& info) const;  // T 0x0833260c
    // ORIG T 0x08333df6 _ZNK8WongWork15CCompoundAvatar14_generateIPGNoEPK5CUserlPc
    // （定义 CompoundDeathLCB.cpp）：snprintf(buf,0x14,"C%02d%02d%02d%02d%010d",
    //   G_CEnvironment()+0x1b0,(year+1900)%2000,mon+1,mday,characNo)。
    char* _generateIPGNo(const CUser* user, long curSec, char* buf) const;
    ENUM_ERROR _ProcCompoundCore(CUser* user, stCompoundAvatarInfo& info,
                                 stCompoundAvatarTable* table);              // T 0x08332ee8
    void _SendDisjointAvatarStatistic(tagCompoundInfo& info);                 // T 0x08333ecc

    // ---- 布局（ORIG ctor 汇编实证）----
    tagCompoundAvatarTable m_tables[11];             // +0x00..+0x83
    CMTRand* m_rand;                                 // +0x84（new CMTRand(time)）
};  // sizeof == 0x88

}  // namespace WongWork

#endif  // GAME_CCOMPOUND_AVATAR_H_
