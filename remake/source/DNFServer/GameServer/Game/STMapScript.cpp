// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STMapScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— STMapScript 类方法 + MapScript 导入/导出（G5 地图脚本表）
// 逐函数依据 ORIG objdump / Ghidra 伪 C 转写（地址随注）：
//   MapArea::clear 0x08366050 / MapArea ctor 0x0836608e / dtor 0x083660d6 /
//     copy 0x0837e5ca —— ORIG 与本 TU 同段归口
//   支撑类型：STAssignAnimation ctor 0x089e836c/Clear 0x089e832c、
//     STQuestAnimation 0x089e83d2/Clear 0x089e83ae、BackAniScript
//     0x089e8898/clear 0x089e884a、BackGroundEffect 0x089e8936/
//     clear 0x089e88da、BackGroundEffectBlock 0x089e897e/clear 0x089e89c6、
//     STDialog 0x089e8a1c/clear 0x089e8a5e、AssignNpc::clear 0x089e87ee、
//     STAssignItem copy 0x083ef750/dtor 0x083b6a8c（布局见 CMap.h）
//   STMapScript C1 0x0836cc94 / copy C1 0x0837e698 / D2 0x0836d4bc /
//     Clear T 0x0836c688（逐条转写，顺序保持汇编原序）
//   ScanChampionFeatures 0x089e6cf4 / WriteChampionFeatures 0x089e6e94
//     （CHAMPION_FEATURE_LIST/NAME 表自 ORIG rodata 解出，37 项）
//   ImportMapScript T 0x089dcf54：标签分派 + 6 张函数局部静态
//     map<const char*,int,less_TCHAR>（__lookup/__lookup_0..4，键值自
//     ORIG rodata/insert 现场解出，SpinLock+__inited 双检惰性填充）逐段
//     还原；blood/defense 校验状态机、named/boss/apc_boss 静态计数、
//     [background animation]/[ani info] 嵌套表段按 ORIG 形态转写。
//   ExportMapScript T 0x089e3c55：全段落输出转写（fopen "wb" → fprintf/
//     fwrite/fputc → fclose → convertBinaryToText）；容器输出按 ORIG
//     iterator+(*it) / 逐字段 vector::operator[] 形态（P38）。
// ============================================================================

// [b28d-lane15 集中判词] 本 TU 清单 12 件的 b28d 复核（fresh check_tu_routed：
//   COMPILE OK / identical=904 ae=1761 near=1 diff=27）。逐件 N 归因（ORIG 地址实查）：
//   · getSkillCategoryName @0x088be4d9 (0xa5) —— N（CStringManager::getStr 调用链 +
//     .rodata 串表引用，N89 族）。
//   · ScanChampionFeatures @0x089e6cf4 (0xd0) —— N（ScanType 解析族 P164/P165 岛）。
//   · getDrawLayerName @0x088be212 (0x131) —— N（串表 + 分支块排布 P47/P191）。
//   · getGrowTypeName @0x088bf2b8 (0xadf) NEAR —— N（大表 + P163 决策树密度差）。
//   · convertBinaryToText @0x088be04c (0xe1) —— N（string 构造/返回 N89）。
//   · STMapScript::Clear @0x0836c688 (0x57f) —— N（N82 帧槽 + N89 vector 析构链）。
//   · procSpecialPassiveObject @0x089e7958 (0x7e7) —— N（P165/P166 三段收敛边界）。
//   · procSpecialPassiveObjectVerifyError @0x089e6f62 (0x9f6) —— N（同上 + P167 差集
//     已归零）。
//   · exportCharacterScript @0x08a102a4 (0x106d) —— N（N82/N89）。
//   · STMapScript::C1ERKS_ @0x0837e698 (0xdc6) —— N（N90 数组成员块搬移分块启发式：
//     ORIG 分 5+4+1+1 组间重载基址，4.4 恒单次 N 连发）。
//   · ExportMapScript @0x089e3c55 (0x309f) —— N（超大导出函数：N82 帧槽 + N89 STL
//     内联；按 §6.2 三件套口径收）。
//   · ImportMapScript @0x089dcf54 (0x6d01) —— N（超大解析函数：N82/N89 + P163 决策树
//     + P164 循环旋转 + P165 三段法；按 §6.2 三件套口径收）。
//   语义均按报告/objdump 逐指令转写并核对；残差全为跨版本结构族，无真语义缺口。
//
// [lane22-STMapScript 2026-09-10 复核] fresh check_tu_routed：
//   COMPILE OK / identical=905 ae=1776 near=1 diff=24 —— 与本轮开工读数相同（未回退）。
//   ①判据①（jcc/jmp 目标下标逐位）：9 件 DIFF/NEAR 全部逐条比对——对齐块内每条跳转的
//     **目标相对偏移**两侧完全一致（0 处真错位）；3 处落点助记符差（ImportMapScript
//     +0x12a1、Clear、C1ERKS_）均为帧/槽平移产物，非目标写错。
//   ②exportCharacterScript 0x08a102a4（989/989；mnemonic 差 61→4）：已修 3 处真形态差——
//     ① `unsigned int i = 0;` + `for (i = 0;)` 双初始化（P191；ORIG 0x8a1030d/0x8a1032a）；
//     ② 该循环 `i < 6` 用 unsigned ⇒ `cmpl $0x5; setbe`（P2，原 setle）；
//     ③ grow 循环复用同一 unsigned i ⇒ 尾测 `cmpl $0x6; setbe`（P2，原 setle）；
//     ④ m_blood 分支按 ORIG 正极性重排（`!= 0` 前置 + je 落 else，ORIG 0x08a110f1）。
//     余 4 条 O113-116（ORIG `imul $0x57; add $0x20; add this; add $4` vs 4.4
//     `lea 0x24(this),%edx` + `lea (%edx,%eax,1)`）＝ add-vs-lea 折痕 ⇒ N（P30/N107；
//     三形源码变体 `m_growStatus[i-1]`/`(&m_initialStatus)[i]`/指针算术 实测均 lea）。
//   ③STMapScript::Clear 0x0836c688（340/339）：A/B 三轮——(a) 去 `register c`（内层占槽
//     但丢 push %ebx）；(b) 各 `for(int i…)` 合并为函数级 r 复用（槽集 8→4）；(c) `int c;`
//     头部声明。ORIG 槽集 {12,16,20,24,28,32}：5 个循环共用 -0x20（17 次）+ j1@-0x18 /
//     j2@-0x14（**4.1 不复用**）；4.4 对互斥块同型变量复用同槽（N82 正身）+ 每轮重载
//     this（ORIG 保 %ecx）⇒ N82 封顶。
//   ④ImportMapScript 0x089dcf54（6233/6173）：p167 差集全为 C1/C2、D1/D2 同址别名
//     （§P93 伪差集）⇒ 语义完整。gap 60 真因 = 31 处「ORIG `movzbl ok; xor $1; test;
//     je body`（4.1 while 旋转 + bool 取反物化）vs 我方 `test; jne`（4.4 正极性收回跳）」
//     + 各段寄存器/槽重载（movzbl-mem 62:45、movzbl-global 31:20、call 29:26；立即数
//     多重集 $1 差 30；je -13 / jne +16）。属 P164/P283 旋转 + P292 物化族 ⇒ N。
//     [A/B 已试·不收敛] 探针 /tmp/lane22/p4.cpp r3（goto 尾标签环，6 形中唯一出 `je`）
//     代入 [dungeon] 段实测：xor1 91→92 仅 +1，4.4 多入口 `jmp L_cond`+`nop`、body/cond
//     物理序仍相反（6173→6175）⇒ 已回滚，判 N 成立。
//   ⑤C1ERKS_ 0x0837e698（988/994）：我方函数尾多 6 条 epilogue（ORIG 以 _Unwind_Resume
//     收尾）＝ N104（EH 尾块相对序）+ N90（ORIG 43 条准备段 vs 我方 7）⇒ N。
//   ⑥STL 实例 15 件（__uninitialized_copy ×12 / _M_insert_aux / __copy_move_backward /
//     new_allocator::construct）：ORIG(4.1 头) 含 EH 回滚段（~T + operator delete +
//     _Unwind_Resume），4.4.7 头判 nothrow 省略 ⇒ N89/N130（libstdc++ 头集差异，非本 TU
//     源码可控）。`_Z41__static_initialization_and_destruction_0ii` = N123 撞名豁免。
//   ⑦procSpecialPassiveObject 0x089e7958（488/505）/ VerifyError 0x089e6f62（613/635）：
//     call 锚对齐后无缺调用，残差为段内 ±10~29 的寄存器/槽分布差；3409 行 int* 视图
//     经 Main「视图 cast 反模式」洞察复核（ORIG 原形为 int 数组视图，保留）⇒ N82/N89。
//   ⑧getGrowTypeName 0x088bf2b8：542=542、mnemonic 全等（NEAR），残差 = pair 两物化槽的
//     立即数配对（ORIG 深槽=first/浅槽=second）⇒ N93/P75（变体实测不可翻）。
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <list>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "CMap.h"
#include "STMapScript.h"    // ENUM_MAP_FOLLOW_TYPE/ENUM_PASSAGETYPE（权威头）
#include "CVillageMonsterMgr.h"   // MapArea / STAttackedMonster（全局作用域权威头）
#include "DNFLexWrapper.h"        // loadRDARScriptFile/ScanType/ScanInt/ScanStr...
#include "StringResource.h"       // CStringManager::getStr（getSkillCategoryName）
#include "CDataManager.h"         // stHellParty_t 视图链头（DataManagerInitDepends 须在其后）
#include "CDataManager.h"         // stHellParty_t 视图链头（DataManagerInitDepends 须在其后）
#include "DataManagerInitDepends.h" // stHellParty_t / stHellPartyMonsterGroupList
#include "HellPartyGroupOrder.h"  // STHellPartyGroupOrder/Rate + pool 单例声明
#include "CreatureScript.h"      // ENUM_ELEMENT（getElementName 形参）
#include "NpcScript.h"          // STScriptFileList（checkValidIndex 形参）
#include "STCharacterScript.h"  // STCharacterScript / STRDCharacterStatus / CharacterEtcStats
#include "CEnvironment.h"       // SpinLock / less_TCHAR（ImportMapScript 6 张函数局部静态查找表）

// ---- 跨 TU 符号（ORIG 地址随注；定义在各自所属批次 TU）----
extern char g_szMapScriptBaseDirectory[0x100];                    // B 0x094fe2a0
extern char g_szCharacterScriptBaseDirectory[0x100];             // B 0x094f7470（InitScriptProducers_4.cpp）
extern STScriptFileList g_stMapScriptFileList;                    // B 0x094fe3a0（ORIG cast 视图同 NpcScript.h）
#include "STMapScript/exportCharacterStatusScript.inc"
#include "STMapScript/exportCharacterGiveSkillScript.inc"
#include "STMapScript/exportCharacterEtcStatsScript.inc"
#include "STMapScript/getScriptFileFullPath.inc"
#include "STMapScript/getDrawLayer.inc"
#include "STMapScript/getElement.inc"
#include "STMapScript/convertBinaryToText.inc"
#include "STMapScript/procSpecialPassiveObject.inc"
extern int* Get_instance_stHellParty_t();                         // T 0x089097c4（CDataManager.cpp 单例）
#include "STMapScript/ScanStrOrInt.inc"
static const char* const g_gridSymbols[16] = {
    "AA", "BB", "CC", "DD", "EE", "FF", "GG", "HH",
    "II", "JJ", "KK", "LL", "MM", "NN", "OO", "PP",
};

// ORIG CHAMPION_FEATURE_NAME @0x093f55a0（D 段全局，可写指针数组；指针非 const → 默认外部链接）
const char* CHAMPION_FEATURE_NAME[37] = {
    "[Immune-Magic]", "[Immune-Physic]", "[Enhance-Physicaldefense]",
    "[Enhance-Magicaldefense]", "[Immune-Fire]", "[Immune-Water]",
    "[Immune-Light]", "[Immune-Dark]", "[Immune-ActiveStatus]",
    "[Enchant-Fire]", "[Enchant-Water]", "[Enchant-Light]",
    "[Enchant-Dark]", "[Speedup-Move]", "[Speedup-Attack]",
    "[Speedup-Hitrecovery]", "[Speedup-Wakeup]", "[Speedup-Hpregen]",
    "[Etc-Neverfly]", "[Etc-Reserrection]", "[onAttack-stun]",
    "[onAttack-bleeding]", "[onAttack-confuse]", "[onAttack-slow]",
    "[onAttack-lightning]", "[onAttack-poison]", "[onAttack-blind]",
    "[high-activestatus]", "[Etc-divide]", "[Etc-invisible]",
    "[Etc-healed]", "[Etc-infected]", "[Etc-counted]",
    "[Etc-berserk]", "[Etc-sleep]", "[Etc-revive]", "[Etc-teleport]",
};

// ORIG _ZL21CHAMPION_FEATURE_LIST @0x08ae5c80（37 项完整特征值）
static const int CHAMPION_FEATURE_LIST[37] = {
    0x001ff00, 0x000ff01, 0x002ff02, 0x002ff03, 0x0030004, 0x0030005,
    0x0030006, 0x0030007, 0x0040008, 0x003fe09, 0x003fe0a, 0x003fe0b,
    0x003fe0c, 0x002000d, 0x003000e, 0x003000f, 0x0020010, 0x0040011,
    0x0010012, 0x0020013, 0x002fd14, 0x002fd15, 0x002fd16, 0x002fd17,
    0x002fd18, 0x002fd19, 0x002fd1a, 0x003001b, 0x004fd1c, 0x004fd1d,
    0x004fd1e, 0x004fd1f, 0x004fd20, 0x004fd21, 0x004fd22, 0x004fd23,
    0x004fd24,
};

// ORIG 函数局部静态（b 段）：convertBinaryToText 缓冲 0x0949b4e0（0x5b400）、
// getDrawLayerName 缓冲 0x0949b4d0（0x10）——声明位于各自函数内。

// ORIG 全局作业名表 0x093f4d40（D，size 0x30 = 12 项；getCharacterJob/
// checkValiedJobName 共用）。逐项字符串地址 0x08e0879f..0x08e0881a 自 ORIG
// rodata 实测；第 12 项 "[all]" 为本 TU 局部表原缺项。
const char* CHARACTER_JOB_TAG_NAMES[12] = {
    "[swordman]",         // 0  0x08e0879f
    "[fighter]",          // 1  0x08e087aa
    "[gunner]",           // 2  0x08e087b4
    "[mage]",             // 3  0x08e087bd
    "[priest]",           // 4  0x08e087c4
    "[at gunner]",        // 5  0x08e087cd
    "[thief]",            // 6  0x08e087d9
    "[at fighter]",       // 7  0x08e087e1
    "[at mage]",          // 8  0x08e087ee
    "[demonic swordman]", // 9  0x08e087f8
    "[creator mage]",     // 10 0x08e0880b
    "[all]"               // 11 0x08e0881a
};

// ===========================================================================
// ScanChampionFeatures / WriteChampionFeatures —— ORIG 0x089e6cf4 / 0x089e6e94
// ===========================================================================

// [b28d-solo-STMapScript] _Z20ScanChampionFeaturesiR15STAssignMonster：AE（离开 DIFF/NEAR）。
// 修法：补 ORIG 具名临时 feature@-0x14（P0 值先物化）并置于 tok 之前，复现槽序
// -0x18(tok)/-0x14(feature)/-0x10(i)/-0xc(k)。p167 残 1：ORIG push_back(int&&)（4.1 视同型
// cast 为 prvalue），4.4 对 (int)feature 多一条 mov ⇒ 保留 lvalue 形；int 拷贝/移动等价（N）。
#include "STMapScript/ScanChampionFeatures.inc"
}

// ORIG T 0x089e6e94（全局符号；本 TU 内唯一调用点，去 static 以发射 T）。
#include "STMapScript/WriteChampionFeatures.inc"
}

// ===========================================================================
// getDrawLayerName / convertBinaryToText —— ORIG 0x088be212 / 0x088be04c
// （STMapScript Export 段使用的 ORIG 静态；逐条转写）
// ===========================================================================

// [b28d-solo-STMapScript] _Z16getDrawLayerName14ENUM_DRAWLAYER：AE（离开 DIFF）。
// 修法：switch→线性 if/else-if 链（P17：ORIG 0x088be212 为 cmpl $K,0x8(%ebp);jne 链，
// 非跳表）。p167 CLEAR 8/8。
const char* getDrawLayerName(ENUM_DRAWLAYER drawLayer)
{
    static char buffer[0x10];   // ORIG _ZZ16getDrawLayerName14ENUM_DRAWLAYERE6buffer 0x0949b4d0
    // ORIG 0x088be212 为线性比较链（cmpl $K,0x8(%ebp); jne next），非 switch
    // 跳表（P17）：逐项 if/else-if 转写。
    if (drawLayer == DRAWLAYER_CLOSE)
        strncpy(buffer, "[close]", 0x10);
    else if (drawLayer == DRAWLAYER_NORMAL)
        strncpy(buffer, "[normal]", 0x10);
    else if (drawLayer == DRAWLAYER_BOTTOM)
        strncpy(buffer, "[bottom]", 0x10);
    else if (drawLayer == DRAWLAYER_CLOSEBACK)
        strncpy(buffer, "[closeback]", 0x10);
    else if (drawLayer == DRAWLAYER_MIDDLEBACK)
        strncpy(buffer, "[middleback]", 0x10);
    else if (drawLayer == DRAWLAYER_DISTANTBACK)
        strncpy(buffer, "[distantback]", 0x10);
    else if (drawLayer == DRAWLAYER_COVER)
        strncpy(buffer, "[cover]", 0x10);
    else
        strncpy(buffer, "[normal]", 0x10);   // ORIG 缺省 → "[normal]"
    return buffer;
}

// [b28d-solo-STMapScript] _Z19convertBinaryToTextPKc：AE（离开 DIFF）。
// 修法：声明序 fp 先于 count（ORIG -0x14/-0x10）、for(;;)+两个 break（复现 ORIG 0x088be085/
// 0x088be092 双 nop 岛）、fwrite 实参序 (buf,count,1,fp)。p167 CLEAR 8/8。
#include "STMapScript/convertBinaryToText_1.inc"
}

// ===========================================================================
// procSpecialPassiveObject —— ORIG T 0x089e7958（[special passive object]
// action 表扫描；"[hellparty]" 分支联动地狱派对单例与组序池）
// ===========================================================================

// [b28d-solo-STMapScript] _Z24procSpecialPassiveObject…：p167 CLEAR 64/64（语义无缺口）；
// verdict DIFF 残差＝N82 槽/寄存器分配（大函数帧伪影，P182）。
// 修法：①make_pair 键用 int 形参 cnt（ORIG 0x089e9db8 make_pair<int&,…>，勿收窄 ushort）；
// ②pool insert 改 std::make_pair(fileIdx,localOrders)（ORIG 走 pair&& move ctor）；
// ③`tok == "[hellparty]"` 正极性包裹（P222，ORIG _ZSteqIcSt…）；④rate 累加改 `+=`。
#include "STMapScript/procSpecialPassiveObject_1.inc"
}
// ===========================================================================
// MapArea 方法（ORIG 与本 TU 同段；布局见 CVillageMonsterMgr.h 注记）
// ===========================================================================

#include "STMapScript/clear.inc"
}

#include "STMapScript/MapArea.inc"
}

#include "STMapScript/MapArea_1.inc"
}

#include "STMapScript/dtor_MapArea.inc"
}

MapArea& MapArea::operator=(const MapArea& other)   // ORIG 0x86d886a
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_w = other.m_w;
    m_h = other.m_h;
    m_list = other.m_list;
    return *this;
}

// ===========================================================================
// 支撑类型方法（Clear/clear 逐条转写）
// ===========================================================================


// ---------------------------------------------------------------------------
// TownPath —— 切片实证 TownPath 以 MapArea 为子对象（基类），本 TU 四个方法
// 全部委托给基类：clear 0x089e8258→call _ZN7MapArea5clearEv 0x08366050、
// C1 0x089e828e→call _ZN7MapAreaC1Ev + vector<int>::C1@0x24 后 call 自身 clear、
// C1ERKS 0x083d9b78→call _ZN7MapAreaC1ERKS_、aSERKS_ 0x089ea970→call
// _ZN7MapAreaaSERKS_；随后 +0x1c/+0x20 两 int 与 vector<int>@0x24。
// ---------------------------------------------------------------------------
#include "STMapScript/TownPath.inc"
}

#include "STMapScript/TownPath_1.inc"
}

TownPath& TownPath::operator=(const TownPath& other)   // ORIG 0x089ea970
{
    MapArea::operator=(other);
    m_field1c = other.m_field1c;
    m_field20 = other.m_field20;
    m_conds = other.m_conds;
    return *this;
}

#include "STMapScript/clear_1.inc"
}

// ---------------------------------------------------------------------------
// Assign 系列结构 Clear —— ORIG 逐条转写
// ---------------------------------------------------------------------------

#include "STMapScript/Clear.inc"
}

#include "STMapScript/Clear_1.inc"
}

#include "STMapScript/Clear_2.inc"
}

#include "STMapScript/Clear_3.inc"
}

// ORIG STAssignSpecialPassiveObj::C1Ev W 0x089e87a6：vector 默认构造后调 Clear()。
#include "STMapScript/STAssignSpecialPassiveObj.inc"
}

#include "STMapScript/Clear_4.inc"
}

// ---------------------------------------------------------------------------
// STAICharacterMapArrangeData —— ORIG Clear 0x089e85fe / C1 0x089e8676 /
// copy C1ERKS_ 0x083d31d2 / D1 0x083dc9a0
// ---------------------------------------------------------------------------

#include "STMapScript/Clear_5.inc"
}

#include "STMapScript/STAICharacterMapArrangeData.inc"
}

#include "STMapScript/STAICharacterMapArrangeData_1.inc"
}

#include "STMapScript/dtor_STAICharacterMapArrangeData.inc"
}
#include "STMapScript/clear_2.inc"
}

#include "STMapScript/STAssignItem.inc"
}

#include "STMapScript/dtor_STAssignItem.inc"
}

#include "STMapScript/Clear_6.inc"
}

#include "STMapScript/STAssignAnimation.inc"
}

#include "STMapScript/Clear_7.inc"
}

#include "STMapScript/STQuestAnimation.inc"
}
// ---- STAssignPassiveObject（ORIG C1 0x089e8440 / Clear 0x089e8414，W）----
#include "STMapScript/STAssignPassiveObject.inc"
}

#include "STMapScript/Clear_8.inc"
}

#include "STMapScript/clear_3.inc"
}

#include "STMapScript/BackAniScript.inc"
}

#include "STMapScript/clear_4.inc"
}

#include "STMapScript/BackGroundEffect.inc"
}

#include "STMapScript/clear_5.inc"
}

#include "STMapScript/BackGroundEffectBlock.inc"
}

#include "STMapScript/clear_6.inc"
}

#include "STMapScript/STDialog.inc"
}

// ===========================================================================
// STMapScript ctor / copy / dtor / Clear
// ===========================================================================

#include "STMapScript/STMapScript.inc"
}

// [b28d-solo-STMapScript] _ZN11STMapScriptC1ERKS_：p167 CLEAR 95/95（语义无缺口）；
// verdict DIFF 残差＝N90 数组成员块搬移分块启发式（ORIG 5+4+1+1 组间重载基址，4.4 单次连发）。
// 修法：m_pvpStartArea 进初始化列表（复现 ORIG 0x0837eb82 倒计数 C1ERKS_×5；原为
// C1Ev×5+aSERKS_×5）。
#include "STMapScript/STMapScript_1.inc"
}

#include "STMapScript/dtor_STMapScript.inc"
}

// [b28d-solo-STMapScript] _ZN11STMapScript5ClearEv：p167 CLEAR 41/41（语义无缺口）；
// verdict DIFF 残差＝N82 槽复用（ORIG 两独立 pathgate 外层循环共用 i@-0x20，4.4 第二循环
// 用 -0x18）+ sightScroll 索引 add-vs-lea（P30 族）。
// 修法：①m_str1a0/m_str1b0 的 assign→operator=（ORIG _ZNSsaSEPKc，真调用目标修正）；
// ②pathgate 拆两个独立嵌套循环（ORIG 0x0836c91b/0x0836c963）；③`register int c`（P231）
// 复现 ORIG %ebx 缓存（push ebx;sub $0x34 已与 ORIG 同帧）。
#include "STMapScript/Clear_9.inc"
}

// ORIG ScanChampionFeatures(int, STAICharacterMapArrangeData&) W 0x089e6dc4
//（同表匹配，push 到 +0x1c vector<int>；定义在 OnlyOrigBatch14.cpp）。
#include "STMapScript/ScanChampionFeatures_1.inc"
struct STMapScriptWordView
{
    int m_words[0x50];                                // 覆盖 +0x000..0x13f
};
static_assert(sizeof(STMapScriptWordView) <= 0x140, "word view within header span");

// [b28d-solo-STMapScript] _Z15ImportMapScriptP11STMapScriptPKc：p167 残 1/851——
// vector<pair<int,int>>::push_back 选 const& 而 ORIG 选 &&（同 ScanChampionFeatures 的
// 重载选择族，int 对拷贝/移动等价 ⇒ N）。
// 已闭合：SPOBJ_ACTION_DATAS::D1Ev ×2 经 P317 删用户空析构（Main 代落 SPOBJ_ACTION_DATAS.h/
// .cpp）后消失（ORIG 0x089e0a66 循环内 C1Ev 后无 D1 call）。
// 超大函数 verdict DIFF 按 §6.2 口径收：残差＝N82/N89 + P163 决策树密度。
// 修法：m_vec180 嵌套转换临时形、[hellparty] make_pair 键 int 化等（与 proc 同族）。
#include "STMapScript/ImportMapScript.inc"
}

// ===========================================================================
// ExportMapScript —— ORIG T 0x089e3c55（全段落输出转写；ORIG 以 fopen("wb") +
// fprintf 生成中间文本，收尾 convertBinaryToText 转最终脚本）
// ===========================================================================

// [b28d-solo-STMapScript] _Z15ExportMapScriptP11STMapScriptPKc：p167 CLEAR 378/378
//（§6.2 三件套②归零）；超大函数 verdict DIFF 按 §6.2 口径收：残差＝N82 帧/槽号 + N89
// libstdc++ 全内联（ORIG 大量 out-of-line 模板实例），无真语义缺口。
#include "STMapScript/ExportMapScript.inc"
}

// ===========================================================================
// 脚本名表导出自由函数 —— ORIG 0x088be0xx-0x088c0xxx 段（脚本字符串/枚举
// 名表导出域；getDrawLayerName/convertBinaryToText 同域已在本 TU 定义）。
// 逐函数依据 docs/class_func_reports/_lt_global_gt_/*.md 转写。
// ===========================================================================

// ---- 全局枚举（ORIG mangled 名要求类型名与全局作用域一致；成员名不可考——
//      ORIG 无 DWARF，按对应名字符串推断命名）----

// ORIG 0x088be343：follow 类型 → 名字。命中 strncpy 到函数局部静态缓冲
// （_ZZ..E6buffer，b 段 0x30）并返回；未命中返回 0。
const char* getFollowTypeName(ENUM_MAP_FOLLOW_TYPE followType)
{
    static char buffer[0x30];
    if (followType == MAP_FOLLOW_TYPE_PATH_GATE)
        strncpy(buffer, "[path gate]", 0x30);
    else if (followType == MAP_FOLLOW_TYPE_PARENT)
        strncpy(buffer, "[parent]", 0x30);
    else if (followType == MAP_FOLLOW_TYPE_FROM_PARENT)
        strncpy(buffer, "[from parent]", 0x30);
    else if (followType == MAP_FOLLOW_TYPE_KEEP_POSITION)
        strncpy(buffer, "[keep position]", 0x30);
    else
        return 0;
    return buffer;
}

// ORIG 0x088be3e7：passage 类型 → 名字（同 getFollowTypeName 缓冲模式）。
const char* getPassageTypeName(ENUM_PASSAGETYPE passageType)
{
    static char buffer[0x30];
    if (passageType == PASSAGETYPE_PASS_ALL)
        strncpy(buffer, "[pass all]", 0x30);
    else if (passageType == PASSAGETYPE_PASS_ONLY_FLOAT_TYPE)
        strncpy(buffer, "[pass only float type]", 0x30);
    else if (passageType == PASSAGETYPE_DO_NOT_PASS)
        strncpy(buffer, "[do not pass]", 0x30);
    else if (passageType == PASSAGETYPE_PASS_ONLY_RED_TEAM)
        strncpy(buffer, "[pass only red team]", 0x30);
    else if (passageType == PASSAGETYPE_PASS_ONLY_BLUE_TEAM)
        strncpy(buffer, "[pass only blue team]", 0x30);
    else if (passageType == PASSAGETYPE_PASS_ONLY_CHARACTER)
        strncpy(buffer, "[pass only character]", 0x30);
    else
        return 0;
    return buffer;
}

// ORIG 0x088be60e：职业枚举 → 职业标签名。strncpy 0x10 到函数局部静态缓冲
// （_ZZ..E6buffer 0x10）。表为 ORIG 全局 CHARACTER_JOB_TAG_NAMES(0x93f4d40)；
// 无边界检查照 ORIG。
const char* getCharacterJobName(ENUM_CHARACTERJOB job)
{
    static char buffer[0x10];
    strncpy(buffer, CHARACTER_JOB_TAG_NAMES[job], 0x10);
    return buffer;
}

// ORIG 0x088be4d9：职业/技能类目 → 字符串资源 id（CStringManager::getStr）。
// job>0xa 或 category>4 → 0；category==4 → getStr(0x522a)；否则按 job 基址
// + category 查询（job 8..10 基址为 0，照 ORIG switch 无 default 的 local_10）。
// [b28d-solo-STMapScript] _Z20getSkillCategoryName17ENUM_CHARACTERJOBi：p167 CLEAR 2/2；
// verdict DIFF 残差＝跨版本共享 return 块合并（ORIG 把两处 return 0 合并为中间块：
// 0x088be4ef `jg` 落点=内层块 + `jbe body`；4.4 恒尾置块并多发一条 mov）⇒ N（块排布）。
// 修法：switch((int)job)（P305：窄枚举 switch(job) 的 case 1..7 被 -fstrict-enums 静默删除，
// 原 36 条缺跳表）+ 外层正向 if 包裹（P222）复现 8 路跳表与 jg/jbe 极性。
const char* getSkillCategoryName(ENUM_CHARACTERJOB job, int category)
{
    // ORIG：job>0xa 直接返回 0（jg）；category<=4（jbe 无符号）才进主体。
    // 【P305】ENUM_CHARACTERJOB 为窄枚举（全仓仅 _0=0），switch(job) 的
    // case 1..7 会被 -fstrict-enums 静默删除 ⇒ 必须 switch((int)job) 才发
    // ORIG 的 8 路跳表（0x088be51c `jmp *table(,%eax,4)`）。
    // ORIG 0x088be4e5 `jg`(job>0xa) 与 0x088be4ed `jbe`(cat<=4 → body) 共落
    // 0x088be4ef 单一 zero 块（`mov $0,%eax; jmp epilogue`）⇒ 合并短路守卫（P133）。
    if (job > 0xa || (unsigned int)category > 4u)
        return 0;
    if (category == 4)
        return CStringManager::getStr(0x522a);
    int base = 0;
    switch ((int)job)
    {
    case 0:  base = 0x5226; break;
    case 1:  base = 0x522b; break;
    case 2:  base = 0x522f; break;
    case 3:  base = 0x5233; break;
    case 4:  base = 0x5237; break;
    case 5:  base = 0x522f; break;
    case 6:  base = 0x3310; break;
    case 7:  base = 0x522b; break;
    default: break;
    }
    return CStringManager::getStr(base + category);
}

// ORIG 0x088c0144：字符串向量线性查找，命中返回下标，未命中 0xffffffff。
unsigned int getIndexOfStringFromStringVector(const char* s,
                                              std::vector<std::string>& vec)
{
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        if (vec[i] == s)
            return i;
    }
    return 0xffffffff;
}

// ORIG 0x088c019d：下标 → 字符串（越界返回 0）。
const char* getStringOfIndexFromStringVector(int index,
                                             std::vector<std::string>& vec)
{
    if ((unsigned int)index < vec.size())
        return vec[index].c_str();
    return 0;
}

// ORIG 0x088c03d6：RDARScript 错误日志输出。
#include "STMapScript/OutputErrorMessage.inc"
}

// ORIG _Z15getGrowTypeName17ENUM_CHARACTERJOBi @ 0x088bf2b8（报告
// _lt_global_gt_/getGrowTypeName.md 汇编+Ghidra C 双段转写）。ORIG 每次调用
// 现场构造局部 std::map<std::pair<ENUM_CHARACTERJOB,int>, const char*>（32 条
// operator[] 写入）后 find 派发，未命中返回 "none"（0x088bfd58）。逐条恢复。
// 【残差豁免·NEAR 封顶】orig_insns=542 ours=542，帧 0x240 与全部栈槽（map@-0x22c、
// 64 个 pair 物化槽 -0x20c..-0x1c、iterator/pair 临时）逐一对位，助记符序列全等。
// 唯一文本差 = 每对物化槽立即数配对互换（ORIG 4.1 U1 先物化：`movl $1,-0x20c;
// movl $0,-0x208`；4.4 U2 先：`movl $0,-0x20c; movl $1,-0x208`，ORIG 0x088bf2d1
// vs ours .o 同偏移）——跨版本实参物化槽序，无合法源码写法翻转（P75 同类）。
// [b28d-solo-STMapScript] _Z15getGrowTypeName17ENUM_CHARACTERJOBi：NEAR 封顶（542=542）。
// p167 CLEAR 73/73（pair converting ctor C1IS0_iEEOT_OT0_ 与 map ctor/operator[] 全对齐）。
// 残差＝每对 pair 物化槽的立即数配对序（ORIG 先 first 后 second：`movl $1,-0x20c;
// movl $0,-0x208`；4.4 反之）⇒ N（P75 跨版本实参物化序，帧 0x240/槽位集全等）。
const char* getGrowTypeName(ENUM_CHARACTERJOB job, int grow)
{
    typedef std::pair<ENUM_CHARACTERJOB, int> GrowKey;
    typedef std::map<GrowKey, const char*> GrowTypeMap;
    GrowTypeMap table;                        // ORIG map ctor 0x088c4548
    table[GrowKey((ENUM_CHARACTERJOB)1, 0)] = "weaponmaster";
    table[GrowKey((ENUM_CHARACTERJOB)2, 0)] = "soulbringer";
    table[GrowKey((ENUM_CHARACTERJOB)3, 0)] = "berserker";
    table[GrowKey((ENUM_CHARACTERJOB)4, 0)] = "asura";
    table[GrowKey((ENUM_CHARACTERJOB)1, 1)] = "nenmaster";
    table[GrowKey((ENUM_CHARACTERJOB)2, 1)] = "striker";
    table[GrowKey((ENUM_CHARACTERJOB)3, 1)] = "streetfighter";
    table[GrowKey((ENUM_CHARACTERJOB)4, 1)] = "grappler";
    table[GrowKey((ENUM_CHARACTERJOB)1, 2)] = "ranger";
    table[GrowKey((ENUM_CHARACTERJOB)2, 2)] = "launcher";
    table[GrowKey((ENUM_CHARACTERJOB)3, 2)] = "mechanic";
    table[GrowKey((ENUM_CHARACTERJOB)4, 2)] = "spitfire";
    table[GrowKey((ENUM_CHARACTERJOB)1, 3)] = "elementalmaster";
    table[GrowKey((ENUM_CHARACTERJOB)2, 3)] = "summoner";
    table[GrowKey((ENUM_CHARACTERJOB)3, 3)] = "battlemage";
    table[GrowKey((ENUM_CHARACTERJOB)4, 3)] = "witch";
    table[GrowKey((ENUM_CHARACTERJOB)1, 4)] = "crusader";
    table[GrowKey((ENUM_CHARACTERJOB)2, 4)] = "infighter";
    table[GrowKey((ENUM_CHARACTERJOB)3, 4)] = "exorcist";
    table[GrowKey((ENUM_CHARACTERJOB)4, 4)] = "avenger";
    table[GrowKey((ENUM_CHARACTERJOB)1, 5)] = "ranger";
    table[GrowKey((ENUM_CHARACTERJOB)2, 5)] = "launcher";
    table[GrowKey((ENUM_CHARACTERJOB)3, 5)] = "mechanic";
    table[GrowKey((ENUM_CHARACTERJOB)4, 5)] = "spitfire";
    table[GrowKey((ENUM_CHARACTERJOB)1, 6)] = "rogue";
    table[GrowKey((ENUM_CHARACTERJOB)2, 6)] = "necromancer";
    table[GrowKey((ENUM_CHARACTERJOB)1, 7)] = "nenmaster";
    table[GrowKey((ENUM_CHARACTERJOB)2, 7)] = "striker";
    table[GrowKey((ENUM_CHARACTERJOB)3, 7)] = "streetfighter";
    table[GrowKey((ENUM_CHARACTERJOB)4, 7)] = "grappler";
    table[GrowKey((ENUM_CHARACTERJOB)1, 8)] = "elementalbomber";
    table[GrowKey((ENUM_CHARACTERJOB)2, 8)] = "glacialmaster";
    GrowTypeMap::iterator it = table.find(GrowKey(job, grow));
    if (it != table.end())
        return it->second;
    return "none";
}


// ============================================================================
// OnlyOrigBatch11 追加：checkValidIndex —— ORIG T 0x089e6f33
//（与 WriteChampionFeatures 同段；STScriptFileList::GetFileName 命中返回 1）
// ============================================================================
#include "STMapScript/checkValidIndex.inc"
}

// ===========================================================================
// OnlyOrigBatch12 追加：procSpecialPassiveObjectVerifyError —— ORIG T
// 0x089e6f62。动作验证状态机：j==0 读动作类型名（[monster]/[trap]/[item]/
// [quest]/[hellparty]，未知记 CRITICAL_ERROR）；j==1 读索引并核对
// （monster/trap 查 checkValidIndex，item 查 assignItems 未用标记 0xffff0001）；
// j>=2 直接 ScanInt。[hellparty] 分支与 procSpecialPassiveObject 同构，
// 处理完立即返回 hasError（ORIG 控制流）。
// ===========================================================================
// [b28d-solo-STMapScript] _Z35procSpecialPassiveObjectVerifyError…：p167 CLEAR 74/74
//（语义无缺口）；verdict DIFF 残差＝N82 槽/寄存器分配（大函数帧伪影，P182）。
// 修法：①pool insert 改 std::make_pair；②`tok == "[hellparty]"` 正极性包裹（P222）；
// ③priority_queue 第二实参 static_cast<vector<int>&&>（ORIG C1ERKS4_OS2_ move 重载）；
// ④assignItems 步进改后置 it++（ORIG _List_iterator ppEi）；⑤rate 累加改 `+=`。
#include "STMapScript/procSpecialPassiveObjectVerifyError.inc"
}
// ===========================================================================
// exportCharacterScript —— ORIG T 0x08a102a4
// （docs/class_func_reports/_lt_global_gt_/exportCharacterScript.md）
// 导出 STCharacterScript 为中间文本（fopen "wb" + fprintf/fwrite），收尾
// convertBinaryToText 转最终脚本；fopen 失败返 0，成功返 1。
// 偏移经 objdump 复核：motions 段 +0xad4..+0xb30 与头成员一一对应，
// throw 段顺序 11/21/31/41 再 12/22/32/42（+0xb04/+0xb08/+0xb0c/+0xb10/
// +0xb14/+0xb18/+0xb1c/+0xb20）。
// ===========================================================================
// [b28d-solo-STMapScript] _Z21exportCharacterScriptP17STCharacterScriptPKc：
// p167 CLEAR 162/162（语义无缺口）；verdict DIFF 残差＝N82/N89（fprintf/fwrite 巨型序列
// 的槽与 STL 内联差）。
// 修法：①5 个 vector<string> 循环改非 const iterator + `(*it).c_str()` + 后置 `it++`
//（P96/P167/P28：ORIG 调 iterator deEv/ppEi，非 ptEv/ppEv）；②m_wavs 改 size()+operator[]
// 下标形（ORIG 调 ixEj/size，非 begin/end）。
#include "STMapScript/exportCharacterScript.inc"
}
