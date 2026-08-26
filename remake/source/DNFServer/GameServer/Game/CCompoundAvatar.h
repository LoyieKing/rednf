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

class CItem;              // find_item 结果指针（tagCompoundInfo +0x88/+0x8c）
class CUser;
class CMTRand;            // PvPTypes.h（0x9c8 MT19937）
struct MSG_COMPOUND_AVATAR;  // 合成头像请求包（源码未建模，仅指针传参）

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
    int m_field0c[10];                              // +0x0c（ctor 置 0）
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
    int m_b[10];                                     // +0x02c（Clean 置零）
    int m_c[10];                                     // +0x054（Clean 置零）
    std::vector<std::pair<int, int> > m_optsA[10];   // +0x07c
    std::vector<std::pair<int, int> > m_optsB[10];   // +0x0f4
    int m_d[10];                                     // +0x16c（Clean 置零）
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

    // ---- 布局（ORIG ctor 汇编实证）----
    tagCompoundAvatarTable m_tables[11];             // +0x00..+0x83
    CMTRand* m_rand;                                 // +0x84（new CMTRand(time)）
};  // sizeof == 0x88

}  // namespace WongWork

#endif  // GAME_CCOMPOUND_AVATAR_H_
