// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STSkillScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— STSkillScript（G5 数据/脚本域）
// 逐函数对照 docs/class_func_reports/STSkillScript.md（嵌套类布局）与
// docs/class_func_reports/_lt_global_gt_/ImportSkillScript.md（ImportSkillScript
// ORIG 汇编 0x08a9792f..0x08a9af4e + Ghidra C 全量）。Clear 0x083750b4 /
// ctor 0x08375642 / dtor 0x0837a0fa，嵌套类见 0x08a9b118 / 0x08a9b164 /
// 0x08a9b256 / 0x083660ec。
// 脚本解析函数（loadRDARScriptFile/ScanType/ScanInt/ScanFloat）经
// DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "STSkillScript.h"

#include <map>

#include "DNFLexWrapper.h"
#include "CEnvironment.h"   // class SpinLock
#include "NpcScript.h"      // class STScriptFileList（0x18 布局）
#include "Quest.h"             // enum ENUM_CHARACTERJOB

// ===================== 前置声明（ORIG 自由函数，实现于各权威 TU） =====================

// （RegisterSkillTags / FinalizeImport 为 ORIG ImportSkillScript 内联段，无独立符号，
//   已分别内联到主循环注册分支与尾部修正，禁止以函数形式编造。）

#include "STSkillScript/importCommand.inc"
#include "STSkillScript/processModuleTagSet.inc"

extern char g_szSkillScriptBaseDirectory[0x100];                   // B 0x9500720（InitScriptProducers_4.cpp）
extern std::map<int, std::string> g_staSkillScriptFileList[11];     // B 0x9500820


// ---- ORIG ImportSkillScript 尾部修正段（8a9ac50..8a9af42）内联于 ImportSkillScript，
//      ORIG 无独立 FinalizeImport 符号；对应 back()/size()/empty() 等调用点须落在
//      ImportSkillScript 切片内，故直接内联在函数尾部 finalize 标签处，禁止拆独立函数。
// 汇编证据：docs/class_func_reports/_lt_global_gt_/ImportSkillScript.md 2862-3096 行 +
// Ghidra C（max level 夹紧 / 价格向量 back() 补齐 / 图标索引定位）。

// ===================== 嵌套结构 =====================

#include "STSkillScript/SkillUseItem.inc"
}

#include "STSkillScript/clear.inc"
}

#include "STSkillScript/SkillLevelFeature.inc"
}

#include "STSkillScript/SkillLevelFeature_1.inc"
}

#include "STSkillScript/dtor_SkillLevelFeature.inc"
}

SkillLevelFeature& SkillLevelFeature::operator=(const SkillLevelFeature& other)
{
    m_field0 = other.m_field0;
    m_str8 = other.m_str8;
    m_vecC = other.m_vecC;
    m_vec18 = other.m_vec18;
    return *this;
}

#include "STSkillScript/clear_1.inc"
}

#include "STSkillScript/STPassiveFeatureData.inc"
}

#include "STSkillScript/clear_2.inc"
}

#include "STSkillScript/STKeyCommand.inc"
}

#include "STSkillScript/dtor_STKeyCommand.inc"
}

#include "STSkillScript/clear_3.inc"
}

// ===================== STSkillScript =====================

#include "STSkillScript/STSkillScript.inc"
}

#include "STSkillScript/dtor_STSkillScript.inc"
}

// [b28d-solo-STSkillScript] verdict=NEAR（307/307 insns，助记符序列全等，p167 CLEAR）
// 残差 1 处：+0x110 m_dw110[i] 清零 ORIG `add $0x44,%edx; movl $0,(%eax,%edx,4)`
// vs 4.4 `add $0x40,%edx; movl $0,0x10(%eax,%edx,4)`（成员偏移折入索引 vs disp 折叠）
// ⇒ N4/N102 封顶（P30 定论：4.4 不可造 add+零 disp 形；语义等价）。
// 其余 hunk 差异均为 .o 重定位显示（call 目标/rodata 地址），链接后归 AE。
#include "STSkillScript/Clear.inc"
}


// ===================== ImportSkillScript 辅助（ORIG 静态域） =====================

// ---- ORIG static map<const char*,int>（guard 0x9500948 / 对象 0x9500958）

namespace
{

// TAG_* 常量：与 kSkillTags 表 id 一一对应（ORIG ImportSkillScript switch case 实证）
enum
{
    TAG_NAME = 0x129,
    TAG_NAME2 = 0x12c,
    TAG_EXPLAIN = 0x12f,
    TAG_BASIC_EXPLAIN = 0x132,
    TAG_BASIC_PROPERTY = 0x135,
    TAG_MAXIMUM_LEVEL = 0x159,
    TAG_PURCHASE_COST = 0x15c,
    TAG_SPECIAL_PURCHASE_COST = 0x166,
    TAG_REQUIRED_LEVEL = 0x172,
    TAG_REQUIRED_LEVEL_RANGE = 0x175,
    TAG_REQUIRED_RANK = 0x178,
    TAG_REQUIRED_GUILD_LEVEL = 0x17b,
    TAG_REQUIRED_GUILD_LEVEL_RANGE = 0x17e,
    TAG_PRE_REQUIRED_SKILL = 0x181,
    TAG_STEEL_LEARNING_SKILL = 0x18e,
    TAG_TYPE = 0x194,
    TAG_SKILL_CLASS = 0x19b,
    TAG_GROWTYPE_FITNESS = 0x19f,
    TAG_SECOND_GROWTYPE_FITNESS = 0x1a4,
    TAG_GROWTYPE_MAXIMUM_LEVEL = 0x1ac,
    TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL = 0x1b1,
    TAG_SKILL_FITNESS_GROWTYPE = 0x1b9,
    TAG_SKILL_FITNESS_SECOND_GROWTYPE = 0x1c4,
    TAG_CASTING_TIME = 0x1cf,
    TAG_DURABILITY_DECREASE_RATE = 0x1d3,
    TAG_CONSUME_MP = 0x1d6,
    TAG_MAINTAIN_MP = 0x1dc,
    TAG_CONSUME_ITEM = 0x1e0,
    TAG_LIMIT_COUNT = 0x1ee,
    TAG_AUTO_LIMIT_COUNT_REDUCE = 0x1f8,
    TAG_COOL_TIME = 0x1fb,
    TAG_LEAGUE_BAN = 0x1ff,
    TAG_SKILL_UNDER_COOLTIME_EFFECT = 0x202,
    TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH = 0x20d,
    TAG_AUTO_COOLTIME_APPLY = 0x218,
    TAG_WEAPON_EFFECT_TYPE = 0x21b,
    TAG_ICON = 0x224,
    TAG_COMMAND_KEY_EXPLAIN = 0x22a,
    TAG_COMMAND = 0x22d,
    TAG_STATIC_DATA = 0x230,
    TAG_COOLTIME_LEVEL_INFO = 0x23b,
    TAG_LEVEL_INFO = 0x23f,
    TAG_LEVEL_PROPERTY = 0x258,
    TAG_SHAKE_SCREEN = 0x27c,
    TAG_SEAL_ENABLE = 0x280,
    TAG_ACTIVE_SEAL_ENABLE = 0x283,
    TAG_PHYSICAL_MAGICAL_CHANGE = 0x287,
    TAG_SKILL_PRELOADING_IMAGE = 0x28b,
    TAG_PURCHASE_GSP = 0x292,
    TAG_PURCHASE_GOLD = 0x29d,
    TAG_START_COOL_TIME = 0x2a8,
    TAG_SKILL_COMMAND_ADVANTAGE = 0x2ac,
    TAG_IS_LINKED_BLAST = 0x2b1,
    TAG_LINKED_BLAST_ADD_POINT = 0x2b4,
    TAG_LINKED_BLAST_DEC_POINT = 0x2b7,
    TAG_CHILD_SKILL_INDEX = 0x2bb,
    TAG_FEATURE_SKILL_INDEX = 0x2be,
    TAG_FEATURE_SKILL_TYPE = 0x2c1,
    TAG_SPECIAL_LEVEL_UP = 0x2c4,
    TAG_BASIC_EXPLAIN_EX = 0x2e1,
    TAG_EXPLAIN_EX = 0x2e4,
    TAG_EXECUTABLE_STATES = 0x2e7,
    TAG_NUT_SKILL_SCRIPT = 0x2fe,
    TAG_COMMAND_CUSTOMIZING = 0x319,
    TAG_COMBO_SET = 0x31d,
};

// ORIG 全局实证：map<PKc,i> 全部用 less_TCHAR（内容比较）——
// find(token.c_str()) 为动态指针，默认 std::less 指针比较恒 miss。
// 65 张标签条目改为 ImportSkillScript 内函数局部静态 __lookup 的逐条 insert
// （ORIG __lookup 是函数局部 static，编译器 __cxa_guard 包裹，见主函数）。
typedef std::map<const char*, int, less_TCHAR> SkillTagMap;

} // namespace

// [b28d-solo-STSkillScript·大件波 BigSkillChar] AE 口径 ndiff 2907→**293**：
//  ① 函数级标量 20 个声明序**反转**（4.4 先声明→更深，原序使 tagId(-0x6c)…
//     finIdx(-0x1c) 与 ORIG 完全倒置，浅层整区平移 0x50）；
//  ② level info `row` 由块内引用改为函数级指针声明（置于 jIdx/chunk 之间），
//     消 4.4 共享 finIdx 死槽并把 chunk/kIdx/srcIdx 拉回 ORIG -0x30/-0x2c/-0x28。
// 语义核对（ORIG↔.o 常量组差集）：imm 集仅剩帧 0x67c/0x69c + xor$1 物化×2；
// 非 ebp 字段访问集仅剩 `lea 0x44(%ebx)`↔`0x40(%ebx)` 一处寄存器折叠差 ⇒
// 65 张 TAG/字段写/循环界**无缺失**。
// [大件语义修复波 BigSkillScript] 归一化 gap 3058/3053(−5)→**3051/3053(+2)**、
// call 锚点段差异 60→5、clean 全序列非 nop 差异 opcode 28（p167 368=368 不变、
// TU 四数 identical=219/ae=491/near=1/diff=4 不变）：
//  ① [special level up] 8a9a6d9..8a9a822：opToken 由循环外改**循环体内**局部
//     （ORIG 每轮 ~string C1 8a9a6d9 / D1 8a9a815）、`while(cont)` 改 do-while、
//     cont 去初始化（ORIG 循环前无 mov $1,%ebx）—— 该段 12 个 call 锚点现与
//     ORIG **逐条全等**（含槽位 -0x610/-0x620/-0x614）；
//  ② [combo set] 8a9ab36..ab80：m_290=1 退出块移到循环外（ORIG 8a9ab76 在循环
//     之后，循环内联形产 je 反极性）；
//  ③ default 8a9ab85：`if(!__inited)break` 改 `if(__inited){...}`（ORIG
//     movzbl;test;je 正极性，非 xor$1;jne）；
//  ④ 尾部 8a9ae90：if/else 两臂各自 store 改**三目**（ORIG 两臂物化到 %eax 后
//     汇合 8a9aebc 单点 store，P47）。
//  ⑤ finalize vec128 8a9ae22..8a9ae7d：`while(!empty() && size<m)` 改
//     `if(!empty()) while(size<m)`（ORIG 回跳目标 8a9ae3b=body，empty 只在入口
//     查一次；Ghidra C 亦为 `if (cVar2 != '\x01') { while … }`）——该段 4 个
//     call 锚点差异全消。
// 剩余构成：deep 区 -0x5f0..-0x67c 块临时槽 +8/+0x24 平移（4.1 跨 case
// 复用同一 -0x650 feat 槽，4.4 两份不重用 ⇒ 帧 +0x20，N82/N86 类封顶）；
//  - N4/N102：成员数组偏移折入索引 vs disp 折叠（m_dw110[i] 等）。
//  - P292/N122：bool 局部循环条件 4.4 恒 test %bl vs ORIG test %ebx（8a9a81a/
//    a9ed）；改 int 反产 setne 物化（+2 条），两难 ⇒ N 封顶。
//  - N121：4.4 jump-threading 尾岛（read_token 前 1 条 jmp+nop，ORIG 无）。
//  - P63/P223：ORIG 主循环尾 6 个共享出口 nop+jmp 中继岛（8a9ac14..8a9ac23），
//    4.4 直跳主循环尾 ⇒ 我方少 2 岛（补岛反而拉大 gap，不补）。
//  - finalize while 族 8a9ad05..8a9ae7f：4.1 body-first 旋转 vs 4.4 条件置顶；
//    ORIG `size<m_14` 物化 setb 8a9ae78 vs 4.4 jae+mov$1/mov$0 岛（P197④ 镜像）。
// （65 个 TAG insert 块槽号实测与 ORIG 逐条一致，不计残差源。）
#include "STSkillScript/ImportSkillScript.inc"
}
