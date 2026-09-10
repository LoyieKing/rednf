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
void exportCharacterStatusScript(FILE* fp, STRDCharacterStatus* st);            // T 0x08a0fb38（OnlyOrigBatch14.cpp）
void exportCharacterGiveSkillScript(FILE* fp, std::vector<std::pair<int, int> >* vec);  // T 0x08a0fff6
void exportCharacterEtcStatsScript(FILE* fp, CharacterEtcStats* stats);          // T 0x08a100f3
void getScriptFileFullPath(char* out, unsigned int size,
                           const char* dir, const char* name);    // T 0x088bba4e
const char* getElementName(ENUM_ELEMENT element);                 // 0x088bfee0（CreatureScript.h）
const char* getDrawLayerName(ENUM_DRAWLAYER drawLayer);          // T 0x088be212
int getDrawLayer(const char* s);                                  // 0x088be12d
int getElement(const char* s);                                    // 0x088bfe5b
void convertBinaryToText(const char* path);                       // T 0x088be04c（Export 收尾转换）
bool procSpecialPassiveObject(SPOBJ_ACTION_DATAS& action,         // T 0x089e7958
                              STAssignSpecialPassiveObj& owner, int fileIdx);
const char* toMbcs(const char* src);                              // T 0x08adee98
extern int* Get_instance_stHellParty_t();                         // T 0x089097c4（CDataManager.cpp 单例）
int ScanStrOrInt(bool& isStr, std::string& s, bool& isInt, bool consume);  // DNFLexWrapperHelpers.cpp

// ============================================================================
// 局部静态表（ORIG rodata 数据解出）
// ============================================================================

// ORIG _ZL13g_gridSymbols（[greed] 网格符号，16 项两字符编码）
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
void ScanChampionFeatures(int count, STAssignMonster& monster)
{
    int feature;                       // ORIG -0x14(%ebp)：值先物化再传址（P0）
    std::string tok;
    for (int i = 0; i < count; ++i)
    {
        ScanStr(&tok);
        for (int k = 0; k < 37; ++k)
        {
            if (tok == CHAMPION_FEATURE_NAME[CHAMPION_FEATURE_LIST[k] & 0xff])
            {
                feature = CHAMPION_FEATURE_LIST[k];   // ORIG 0x089e6d4d-0x089e6d6a
                // ORIG 调 push_back(int&&)（4.1 把同类型 cast 视作 prvalue）；
                // 4.4 对 (int)feature 产额外 mov ⇒ 保留 lvalue 形（拷贝/移动对 int 等价）。
                monster.m_vec28.push_back(feature);
                break;
            }
        }
    }
}

// ORIG T 0x089e6e94（全局符号；本 TU 内唯一调用点，去 static 以发射 T）。
void WriteChampionFeatures(FILE* file, STAssignMonster& monster)
{
    fprintf(file, "%d ", static_cast<int>(monster.m_vec28.size()));
    for (size_t i = 0; i < monster.m_vec28.size(); ++i)
        fprintf(file, "`%s` ", CHAMPION_FEATURE_NAME[monster.m_vec28[i] & 0xff]);
    fputc('\n', file);
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
void convertBinaryToText(const char* path)
{
    static char buffer[0x5b400];   // ORIG _ZZ19convertBinaryToTextPKcE6buffer 0x0949b4e0
    FILE* fp = fopen(toMbcs(path), "rb");                 // -0x14(%ebp)（ORIG 声明序 fp 先）
    int count = 0;                                        // -0x10(%ebp)
    for (;;)                                              // ORIG 0x088be04c：循环头在体内
    {
        int c = fgetc(fp);                                // -0xc(%ebp)
        if (c == -1)                                      // 0x088be085 岛
            break;
        if (count > 0x5b3ff)                              // 缓冲上限（0x088be092 岛）
            break;
        if (c == '\n')
        {
            // 首字符或前一字符非 '\r' 时补 '\r'（\n 归一为 \r\n）
            if (count == 0 || buffer[count - 1] != '\r')
                buffer[count++] = '\r';
        }
        buffer[count++] = static_cast<char>(c);
    }
    fclose(fp);
    fp = fopen(toMbcs(path), "wb");                       // 0x08e086af
    fwrite(buffer, count, 1, fp);                         // ORIG 实参序 (buf,count,1,fp)
    fclose(fp);
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
bool procSpecialPassiveObject(SPOBJ_ACTION_DATAS& action,
                              STAssignSpecialPassiveObj& owner, int fileIdx)
{
    std::string tok;                                      // -0xc4(%ebp)
    std::priority_queue<int> groupQueue;                  // less<int> 大顶（-0xd4）
    int curTop = 0;                                       // -0x28
    int lastTop = 0;                                      // -0x24
    int fieldIdx = 0;                                     // -0x20
    int slotIdx = 0;                                      // -0x1c

    for (int j = 0; j <= 5; ++j)                          // -0x18；action 六个 int 字段
    {
        if (j != 0)
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));   // 0x89e80ac
            continue;
        }

        // j == 0：动作类型名
        if (!ScanStr(&tok))                               // 0x88bc7a2
            continue;
        if (tok == "[monster]")                           // 0x08e1ad7b
        {
            action.m_type = 0;
            continue;
        }
        if (tok == "[trap]")                              // 0x08e1c0de
        {
            action.m_type = 1;
            continue;
        }
        if (tok == "[item]")                              // 0x08e1c0e5
        {
            action.m_type = 2;
            continue;
        }
        if (tok == "[quest]")                             // 0x08e1c0ec
        {
            action.m_type = 3;
            continue;
        }
        // P222：ORIG 用正极性 `tok == "[hellparty]"` 包裹（p167 MISSING _ZSteqIcSt…）
        if (tok == "[hellparty]")                         // 0x08e1c0f4
        {

        // ---- [hellparty] 分支（0x89e7ae3..）----
        std::map<unsigned short, STHellPartyGroupOrder> localOrders;   // -0xf0
        for (;;)
        {
            bool ok = false;                              // -0xd5
            int tvIdx = ScanInt(&ok);                     // -0xf4
            if (!ok)
                break;
            groupQueue.push(tvIdx);

            stHellParty_t* hellParty =
                reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
            std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                hellParty->m_groupMap.find(static_cast<unsigned short>(tvIdx));
            if (git == hellParty->m_groupMap.end())
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0xd5c,
                                      "HELLPARTY_LOG : GROUP INDEX NOT FOUND : %d\n",
                                      tvIdx);
                return false;
            }

            // pair 视图 +0x58 = second.m_field54（难度槽位）
            const unsigned char diffSlot = git->second.m_field54;

            bool ok2 = false;
            int rate = ScanInt(&ok2);                     // -0x10 组序号
            if (!ok2)
                return false;
            bool ok3 = false;
            int cnt = ScanInt(&ok3);                      // -0xfc 数量
            if (!ok3)
                return false;
            if (cnt <= 0 || cnt > 9)
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0xd69,
                                      "HELLPARTY_LOG : GROUP ORDER SCRIPT ERROR : %d\n",
                                      cnt);
                return false;
            }

            STHellPartyGroupRate orderRate;               // -0x108（前两 int 赋值）
            orderRate.m_groupIdx = tvIdx;
            orderRate.m_rate = rate;

            std::map<unsigned short, STHellPartyGroupOrder>::iterator mit =
                localOrders.find(static_cast<unsigned short>(cnt));
            if (mit != localOrders.end())                 // 0x89e7d64 前分支
            {
                // P167：ORIG 调 _Rb_tree_iterator::ptEv（operator->）×2
                mit->second.m_rates.push_back(orderRate);
                mit->second.m_rateByDifficulty[diffSlot] +=
                    static_cast<unsigned short>(rate);
            }
                                            // 0x89e7d64 未命中分支
            else
            {
                STHellPartyGroupOrder newOrder;           // ctor memset 0xc + list@+0xc
                newOrder.m_rates.push_back(orderRate);
                newOrder.m_rateByDifficulty[diffSlot] = static_cast<unsigned short>(
                    newOrder.m_rateByDifficulty[diffSlot] + rate);
                // P167：ORIG 调 make_pair<int&, STHellPartyGroupOrder&>
                //（0x089e9db8）；键必须是 int 形参 cnt，勿加 (unsigned short) 收窄。
                localOrders.insert(std::make_pair(cnt, newOrder));
            }
        }

        // 组序表入池（key=fileIdx；ORIG make_pair(0x10(%ebp), localOrders)）
        Get_instance_groupOrder_map_pool()->insert(
            std::make_pair(fileIdx, localOrders));   // P167/P315：ORIG 走 move ctor（pair&&）

        // 大顶队列去重排空：每个唯一非零 top 处理一轮（0x89e7f31..）
        while (!groupQueue.empty())
        {
            curTop = groupQueue.top();
            if (curTop == lastTop || curTop == 0)
            {
                groupQueue.pop();
                continue;
            }
            lastTop = curTop;

            stHellParty_t* hellParty =
                reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
            std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                hellParty->m_groupMap.find(static_cast<unsigned short>(curTop));
            // ORIG 此处未检查 end()（保持原样）

            fieldIdx = 0;                                 // 89e8056/89e805d 等价复位
            slotIdx = 0;
            (&action.m_type)[fieldIdx++] = 5;             // 动作类型 5
            while (git->second.m_monsterIdx[slotIdx] > 0) // pair+0x4 数组逐槽展开
            {
                (&action.m_type)[fieldIdx++] = git->second.m_monsterIdx[slotIdx];
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = -1;
                owner.m_vec10.push_back(action);
                fieldIdx = 1;                             // 保留 action[0]=5
                ++slotIdx;
            }
            fieldIdx = 0;
            slotIdx = 0;
        }
        return false;                                     // 89e8097 ebx=0
        }
        continue;
    }
    return true;                                          // 89e80da ebx=1
}
// ===========================================================================
// MapArea 方法（ORIG 与本 TU 同段；布局见 CVillageMonsterMgr.h 注记）
// ===========================================================================

void MapArea::clear()                       // ORIG 0x08366050
{
    m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
    m_list.clear();
}

MapArea::MapArea()                          // ORIG 0x0836608e
{
    clear();
}

MapArea::MapArea(const MapArea& other)      // ORIG 0x0837e5ca
    : m_x(other.m_x),
      m_y(other.m_y),
      m_w(other.m_w),
      m_h(other.m_h),
      m_list(other.m_list)
{
}

MapArea::~MapArea()                         // ORIG 0x083660d6（仅析构 +0x10 向量）
{
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
TownPath::TownPath()                        // ORIG 0x089e828e
{
    clear();
}

TownPath::TownPath(const TownPath& other)   // ORIG 0x083d9b78
    : MapArea(other),
      m_field1c(other.m_field1c),
      m_field20(other.m_field20),
      m_conds(other.m_conds)
{
}

TownPath& TownPath::operator=(const TownPath& other)   // ORIG 0x089ea970
{
    MapArea::operator=(other);
    m_field1c = other.m_field1c;
    m_field20 = other.m_field20;
    m_conds = other.m_conds;
    return *this;
}

void TownPath::clear()                      // ORIG 0x089e8258
{
    MapArea::clear();
    m_field1c = 0;                       // +0x1c movl $0
    m_field20 = 0;                       // +0x20 movl $0
    m_conds.clear();                     // vector<int>@0x24 clear
}

// ---------------------------------------------------------------------------
// Assign 系列结构 Clear —— ORIG 逐条转写
// ---------------------------------------------------------------------------

void STAssignBloodSystem::Clear()           // ORIG 0x089e8454
{
    m_id = -1;                              // +0x00 movl $-1
    m_field04 = 0;
    m_phase = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_timeA = 0;
    m_timeB = 0;
    m_startCnt = 0;
    m_cntInc = 0;
    m_totalCnt = 0;                         // +0x24
}

void STAssignBloodPhaseTime::Clear()        // ORIG 0x089e84d0
{
    m_smallRoundOrder = 0;                  // +0x00
    m_phaseIdx = -1;                        // +0x04（-1 = 随机）
    m_interval = 0;                         // +0x08
    m_time = 1.0f;                          // +0x0c mov $0x3f800000
    m_field10 = 0;
    m_spawnCnt = 0;
    m_difficulty = 0;                       // +0x18 movb $0
}

void STAssignDefenseSystem::Clear()         // ORIG 0x089e852c（与 Blood 同构）
{
    m_id = -1;
    m_field04 = 0;
    m_phase = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_timeA = 0;
    m_timeB = 0;
    m_startCnt = 0;
    m_cntInc = 0;
    m_totalCnt = 0;
}

void STAssignDefensePhaseTime::Clear()      // ORIG 0x089e85a8
{
    m_smallRoundOrder = 0;
    m_phaseIdx = -1;
    m_interval = 0;
    m_time = 1.0f;
    m_field10 = 0;
    m_spawnCnt = 0;                         // 无 +0x18 字节写（布局止于 +0x14）
}

// ORIG STAssignSpecialPassiveObj::C1Ev W 0x089e87a6：vector 默认构造后调 Clear()。
STAssignSpecialPassiveObj::STAssignSpecialPassiveObj()
{
    Clear();
}

void STAssignSpecialPassiveObj::Clear()     // ORIG 0x089e875a
{
    m_0 = -1;                               // +0x00 movl $-1
    m_c = 0;                                // +0x0c movl $0
    m_8 = m_c;                              // +0x08 ← +0x0c
    m_4 = m_8;                              // +0x04 ← +0x08
    m_vec10.clear();                        // vector@0x10
    m_1c = -1;                              // +0x1c movl $-1
}

// ---------------------------------------------------------------------------
// STAICharacterMapArrangeData —— ORIG Clear 0x089e85fe / C1 0x089e8676 /
// copy C1ERKS_ 0x083d31d2 / D1 0x083dc9a0
// ---------------------------------------------------------------------------

void STAICharacterMapArrangeData::Clear()
{
    m_type = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
    m_camp = 0;
    m_roleType = 0;
    m_posX = 0;                             // +0x18 movb $0
    m_posY = 0;                             // +0x19 movb $0
    m_championFeatures.clear();             // vector<int>@0x1c
    m_npcIdx = -1;                          // +0x28 movl $-1
    m_collisionGroup.clear();               // vector<uchar>@0x2c
}

STAICharacterMapArrangeData::STAICharacterMapArrangeData()
{
    Clear();
}

STAICharacterMapArrangeData::STAICharacterMapArrangeData(
    const STAICharacterMapArrangeData& other)
    : m_type(other.m_type),
      m_field04(other.m_field04),
      m_field08(other.m_field08),
      m_field0c(other.m_field0c),
      m_camp(other.m_camp),
      m_roleType(other.m_roleType),
      m_posX(other.m_posX),
      m_posY(other.m_posY),
      m_championFeatures(other.m_championFeatures),   // copy 于 +0x28 前
      m_npcIdx(other.m_npcIdx),
      m_collisionGroup(other.m_collisionGroup)        // 尾部向量拷贝
{
}

STAICharacterMapArrangeData::~STAICharacterMapArrangeData()
{
    // ORIG 仅按逆序析构两个向量（编译器生成），无额外语句
}
void AssignNpc::clear()                     // ORIG 0x089e87ee
{
    m_npcIdx = 0;                           // +0x00 movl $0
    m_field04 = -1;                         // +0x04 movl $-1
    m_visible = false;                      // +0x08 movb $0
    m_dir = 0;                              // +0x0c
    m_x = 0;                                // +0x10
    m_y = 0;                                // +0x14
    m_z = 0;                                // +0x18
}

STAssignItem::STAssignItem(const STAssignItem& other)   // ORIG 0x083ef750
    : m_field00(other.m_field00), m_pair04(other.m_pair04),
      m_field0c(other.m_field0c),
      m_field10(other.m_field10), m_field14(other.m_field14),
      m_field18(other.m_field18), m_field1c(other.m_field1c),
      m_field20(other.m_field20), m_generateInfos(other.m_generateInfos)
{
}

STAssignItem::~STAssignItem()               // ORIG 0x083b6a8c（仅析构 +0x24 向量）
{
}

void STAssignAnimation::Clear()             // ORIG 0x089e832c
{
    m_name = "";                            // ORIG Ss::operator=("")
    m_layer = 1;                            // +0x04 movl $1
    m_field08 = 0;                          // +0x08
    m_field0c = 0;                          // +0x0c
    m_field10 = 0;                          // +0x10
}

STAssignAnimation::STAssignAnimation()      // ORIG 0x089e836c
{
    Clear();
}

void STQuestAnimation::Clear()              // ORIG 0x089e83ae
{
    STAssignAnimation::Clear();
    m_field14 = 0;                          // +0x14
    m_flag18 = true;                        // +0x18 movb $1
}

STQuestAnimation::STQuestAnimation()        // ORIG 0x089e83d2
{
    Clear();
}
// ---- STAssignPassiveObject（ORIG C1 0x089e8440 / Clear 0x089e8414，W）----
STAssignPassiveObject::STAssignPassiveObject()
{
    Clear();
}

void STAssignPassiveObject::Clear()
{
    // 定义置于 ctor 之后：4.4 对"被调函数定义未前置"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x089e8440 帧形一致（探针 /tmp/near7/p7-p8）。
    m_0 = -1;       // +0x00 movl $0xffffffff
    m_4 = 0;        // +0x04
    m_8 = 0;        // +0x08
    m_c = 0;        // +0x0c
}

void BackAniScript::clear()                 // ORIG 0x089e884a
{
    m_filename = "";                        // ORIG Ss::operator=("")
    m_layer = 1;                            // +0x04
    m_orderUpper = false;                   // +0x08
    m_loopYAxis = false;                    // +0x09
    m_hasScreenPos = false;                 // +0x0a
    m_screenX = 0;                          // +0x0c
    m_screenY = 0;                          // +0x10
}

BackAniScript::BackAniScript()              // ORIG 0x089e8898
{
    clear();
}

void BackGroundEffect::clear()              // ORIG 0x089e88da
{
    m_type = 2;                             // +0x00 movl $2
    m_layer = 7;                            // +0x04 movl $7
    m_field08 = -1;                         // +0x08
    m_field0c = -1;                         // +0x0c
    m_field10 = -1;                         // +0x10
    m_field14 = -1;                         // +0x14
    m_field18 = -1;                         // +0x18
    m_animName.clear();                     // +0x1c
}

BackGroundEffect::BackGroundEffect()        // ORIG 0x089e8936
{
    clear();
}

void BackGroundEffectBlock::clear()         // ORIG 0x089e89c6
{
    m_type = 2;                             // +0x00
    m_mode = 2;                             // +0x04
    m_val = 0;                              // +0x08
    m_animName.clear();                     // +0x0c
}

BackGroundEffectBlock::BackGroundEffectBlock()  // ORIG 0x089e897e
{
    clear();
}

void STDialog::clear()                      // ORIG 0x089e8a5e
{
    m_name.clear();
    for (int i = 0; i < 3; ++i)             // ORIG movb $0,0x4(%edx,%eax,1) 循环
        m_flag[i] = false;
}

STDialog::STDialog()                        // ORIG 0x089e8a1c
{
    clear();
}

// ===========================================================================
// STMapScript ctor / copy / dtor / Clear
// ===========================================================================

STMapScript::STMapScript()                  // ORIG C1 0x0836cc94
{
    // ORIG C1：仅隐式成员构造（非平凡类 C1 按声明序），无标量写；
    // 标量清零全部由 body 的 Clear() 完成（P0 槽律同源）。
    Clear();
}

// [b28d-solo-STMapScript] _ZN11STMapScriptC1ERKS_：p167 CLEAR 95/95（语义无缺口）；
// verdict DIFF 残差＝N90 数组成员块搬移分块启发式（ORIG 5+4+1+1 组间重载基址，4.4 单次连发）。
// 修法：m_pvpStartArea 进初始化列表（复现 ORIG 0x0837eb82 倒计数 C1ERKS_×5；原为
// C1Ev×5+aSERKS_×5）。
STMapScript::STMapScript(const STMapScript& other)      // ORIG C1 0x0837e698
    : m_index(other.m_index),                           // *param_1 = *param_2
      m_list4(other.m_list4),                           // list<int> copy
      m_c(other.m_c),                                   // param_1[3]
      m_recoveryObjects(other.m_recoveryObjects),       // vector<pair>@0x10
      m_listAnimation(other.m_listAnimation),           // list@0x1c
      m_listQuestAnimation(other.m_listQuestAnimation), // list@0x24
      m_listPassiveObject(other.m_listPassiveObject),   // list@0x2c
      m_vecMonster(other.m_vecMonster),                  // vector@0x34
      m_vecConditionMonster(other.m_vecConditionMonster), // vector@0x40
      m_championRate(other.m_championRate),             // param_1[0x13]
      m_listItem(other.m_listItem),                     // list@0x50
      m_listSpecialPassive(other.m_listSpecialPassive), // list@0x58
      m_vecNpc(other.m_vecNpc),                         // vector@0x60
      m_element(other.m_element),                       // param_1[0x1b]
      m_vecTile(other.m_vecTile),                       // vector<string>@0x70
      m_vecExtendedTile(other.m_vecExtendedTile),       // vector<string>@0x7c
            m_vecBackAni(other.m_vecBackAni),                 // vector@0xa0
      m_vecBackGroundEffect(other.m_vecBackGroundEffect), // vector@0xac
      m_vecBlockEffect(other.m_vecBlockEffect),         // vector@0xb8
      m_vecDialog(other.m_vecDialog),                   // vector@0xc4
      m_startPathDir(other.m_startPathDir),             // param_1[0x34]
            m_tileOptionUpper(other.m_tileOptionUpper),       // byte @0xd8
      m_vecTownMovableArea(other.m_vecTownMovableArea), // TownPath@0xdc
      m_vecVirtualMovableArea(other.m_vecVirtualMovableArea), // MapArea@0xe8
      m_vecGuildAgitEntrance(other.m_vecGuildAgitEntrance),   // TownPath@0xf4
                  m_dungeonStartArea(other.m_dungeonStartArea),     // MapArea@0x140
      m_summonStartArea(other.m_summonStartArea),       // MapArea@0x15c
      m_178(other.m_178),                               // param_1[0x5e]
      m_17c(other.m_17c),                               // param_1[0x5f]
      m_vec180(other.m_vec180),                         // vector<pair>@0x180
      m_openingBgm(other.m_openingBgm),                 // string@99(0x18c)
      m_openingBgmParam(other.m_openingBgmParam),       // param_1[100](0x190)
      m_vecSound(other.m_vecSound),                     // vector<string>@0x194
      m_str1a0(other.m_str1a0),                         // string@0x68(0x1a0)
      m_str1a4(other.m_str1a4),                         // string@0x69(0x1a4)
      m_str1a8(other.m_str1a8),                         // string@0x6a(0x1a8)
      m_sortBottom(other.m_sortBottom),                 // byte @0x6b(0x1ac)
      m_str1b0(other.m_str1b0),                         // string@0x6c(0x1b0)
      m_playerCountMin(other.m_playerCountMin),         // param_1[0x6d]/[0x6e] 成对
      m_playerCountMax(other.m_playerCountMax),
      m_pvpStartArea(other.m_pvpStartArea),             // MapArea[5]@0x1bc（ORIG C1ERKS_×5 倒计数）
            m_pvpByPartySlot(other.m_pvpByPartySlot),         // byte @0x92(0x248)
      m_keepStateAxisX(other.m_keepStateAxisX),         // u16 @0x249 成对
      m_keepStateAxisY(other.m_keepStateAxisY),
      m_keepStateAxisZ(other.m_keepStateAxisZ),         // byte @0x24b
            m_vecMonsterSpawnPos(other.m_vecMonsterSpawnPos), // vector@0x94(0x250)
      m_vecAICharacter(other.m_vecAICharacter),         // vector@0x97(0x25c)
      m_apcRandomPoint(other.m_apcRandomPoint),         // param_1[0x9a]
      m_mapLoadingImagePath(other.m_mapLoadingImagePath), // string@0x9b(0x26c)
      m_270(other.m_270),                               // param_1[0x9c]/[0x9d] 成对
      m_274(other.m_274),
      m_vecEventMonsterPos(other.m_vecEventMonsterPos), // vector@0x9e(0x278)
      m_maxMonsterDropException(other.m_maxMonsterDropException), // param_1[0xa1]
      m_vecBloodSystem(other.m_vecBloodSystem),         // vector@0xa2(0x288)
      m_vecBloodPhaseTime(other.m_vecBloodPhaseTime),   // vector@0xa5(0x294)
      m_vecDefenseSystem(other.m_vecDefenseSystem),     // vector@0xa8(0x2a0)
      m_vecDefensePhaseTime(other.m_vecDefensePhaseTime), // vector@0xab(0x2ac)
      m_tournamentMonsters(other.m_tournamentMonsters), // map@0xae(0x2b8)
      m_tournamentStartPos1(other.m_tournamentStartPos1), // vector@0xb4(0x2d0)
      m_tournamentStartPos2(other.m_tournamentStartPos2), // vector@0xb7(0x2dc)
      m_vecStaticStartPos(other.m_vecStaticStartPos),   // vector@0xba(0x2e8)
      m_vecStaticStartPos2(other.m_vecStaticStartPos2), // vector@0xbd(0x2f4)
      m_limitPlayerMoveAreas(other.m_limitPlayerMoveAreas), // map@0xc0(0x300)
      m_vecAttackedMonster(other.m_vecAttackedMonster), // vector@0xc6(0x318)
      m_maxMonsterCountException(other.m_maxMonsterCountException), // param_1[0xc9]
      m_328(other.m_328),                               // param_1[0xca]
      m_32c(other.m_32c),                               // param_1[0xcb]
      m_timeLine(other.m_timeLine),                     // TimeLine@0xcc(0x330)
      m_str34c(other.m_str34c),                         // string@0xd3(0x34c)
      m_moveLayeredType(other.m_moveLayeredType),       // param_1[0xd4]/[0xd5] 成对
      m_zposUnder(other.m_zposUnder),
      m_allDeadCasePassable(other.m_allDeadCasePassable), // byte @0xd6(0x358)
      m_screenEdgeLeft(other.m_screenEdgeLeft),         // param_1[0xd7](0x35c)
      m_screenEdgeRight(other.m_screenEdgeRight),       // param_1[0xd8](0x360)
      m_screenEdgeTop(other.m_screenEdgeTop),           // param_1[0xd9](0x364)
      m_screenEdgeBottom(other.m_screenEdgeBottom),     // param_1[0xda](0x368)
      m_blockUseActiveSkill(other.m_blockUseActiveSkill), // byte @0xdb(0x36c)
      m_blockUseStackableItem(other.m_blockUseStackableItem), // byte 0x36d
      m_fixChampion(other.m_fixChampion),               // byte 0x36e
      m_pad36f(0),
      m_str370(other.m_str370),                         // string@0xdc(0x370)
      m_visibleMinimap(other.m_visibleMinimap),         // param_1[0xdd](0x374)
      m_visibleTownMinimap(other.m_visibleTownMinimap)  // param_1[0xde](0x378)
{
    // 标量区补拷贝（copy 伪 C 中间段）：
    for (int i = 0; i < 6; ++i)                         // param_1[0x22..0x27]=0x88..0x9f
        m_sightScroll[i] = other.m_sightScroll[i];
    for (int i = 0; i < 4; ++i)                         // param_1[0x35]+i = 0xd4..0xd7
        m_blockPath[i] = other.m_blockPath[i];
    for (int i = 0; i < 8; ++i)                         // param_1[0x40..0x4f]=0x100..0x13f
    {
        m_pathgatePos[i] = other.m_pathgatePos[i];
        m_pathgateRange[i] = other.m_pathgateRange[i];
    }
}

STMapScript::~STMapScript()                 // ORIG D2 0x0836d4bc（逆序析构由编译器生成）
{
}

// [b28d-solo-STMapScript] _ZN11STMapScript5ClearEv：p167 CLEAR 41/41（语义无缺口）；
// verdict DIFF 残差＝N82 槽复用（ORIG 两独立 pathgate 外层循环共用 i@-0x20，4.4 第二循环
// 用 -0x18）+ sightScroll 索引 add-vs-lea（P30 族）。
// 修法：①m_str1a0/m_str1b0 的 assign→operator=（ORIG _ZNSsaSEPKc，真调用目标修正）；
// ②pathgate 拆两个独立嵌套循环（ORIG 0x0836c91b/0x0836c963）；③`register int c`（P231）
// 复现 ORIG %ebx 缓存（push ebx;sub $0x34 已与 ORIG 同帧）。
void STMapScript::Clear()                   // ORIG T 0x0836c688（顺序保持汇编原序）
{
    m_maxMonsterDropException = 0;          // param_1[0xa1]=0
    m_vecBloodSystem.clear();               // +0xa2
    m_vecBloodPhaseTime.clear();            // +0xa5
    m_vecDefenseSystem.clear();             // +0xa8
    m_vecDefensePhaseTime.clear();          // +0xab
    m_summonStartArea.clear();              // MapArea::clear(+0x57)
    m_178 = 0;                              // param_1[0x5e]=0
    m_timeLine.reset();                     // advancealtar::TimeLine::reset(+0xcc)
    m_17c = 0;                              // param_1[0x5f]=0
    m_vec180.clear();                       // vector<pair>::clear(+0x60)
    m_list4.clear();                        // list<int>::clear(+1)
    m_c = 0;                                // param_1[3]=0
    m_recoveryObjects.clear();              // vector<pair>::clear(+4)
    m_listAnimation.clear();                // list<STAssignAnimation>::clear(+7)
    m_listQuestAnimation.clear();           // list<STQuestAnimation>::clear(+9)
    m_listPassiveObject.clear();            // list<STAssignPassiveObject>::clear(+0xb)
    m_vecMonster.clear();                   // vector<STAssignMonster>::clear(+0xd)
    m_vecConditionMonster.clear();          // vector<STAssignMonster>::clear(+0x10)
    m_championRate = 0;                     // param_1[0x13]=0
    m_listItem.clear();                     // list<STAssignItem>::clear(+0x14)
    m_listSpecialPassive.clear();           // list<STAssignSpecialPassiveObj>::clear(+0x16)
    m_element = 4;                          // param_1[0x1b]=4
    m_vecTile.clear();                      // vector<string>::clear(+0x1c)
    m_vecExtendedTile.clear();              // vector<string>::clear(+0x1f)
    // ORIG 0x0836c7f7：2×3 嵌套循环（外 -0x20≤1 双 init；内层 c 槽驻留 + body
    // 缓存 %ebx 为 4.1 寻址寄存器分配，4.4 不缓存 → 残余 N，助记符已全同）。
    // ORIG 全程复用同一循环变量槽 -0x20（17 次使用，跨 5 个循环）⇒ 原码声明一个
    // 函数级变量供各循环共用（4.4 对每个循环新声明会给独立槽，槽图不符）。
    int r = 0;                               // [b28d-solo-STMapScript] register 提示实测反劣
    int c;                                   //（帧 0x34→0x20 + 多 push %esi）⇒ 回滚，N82/N126 封顶
    for (r = 0; r < 2; ++r)
        for (c = 0; c < 3; ++c)               // ORIG 内层占槽 -0x1c
            m_sightScroll[r * 3 + c] = 100; //  0x20+r*3+c word 基 0x22..0x27
    m_vecBackAni.clear();                   // vector<BackAniScript>::clear(+0x28)
    m_vecBackGroundEffect.clear();          // vector<BackGroundEffect>::clear(+0x2b)
    m_vecBlockEffect.clear();               // vector<BackGroundEffectBlock>::clear(+0x2e)
    m_vecDialog.clear();                    // vector<STDialog>::clear(+0x31)
    m_startPathDir = 4;                     // param_1[0x34]=4
    for (r = 0; r < 4; ++r)                 // byte(0xd4+i)=0（复用 -0x20）
        m_blockPath[r] = 0;
    m_tileOptionUpper = 0;                  // byte(param_1+0x36)=0
    m_vecTownMovableArea.clear();           // vector<TownPath>::clear(+0x37)
    m_vecVirtualMovableArea.clear();        // vector<MapArea>::clear(+0x3a)
    m_vecGuildAgitEntrance.clear();         // vector<TownPath>::clear(+0x3d)
    // ORIG 0x0836c91b / 0x0836c963：**两个独立嵌套循环**（各自 i@-0x20 复用，
    // j 分别 -0x18/-0x14），非单一外层包两内层。
    for (r = 0; r < 4; ++r)                 // 复用 -0x20
    {
        for (int j = 0; j < 2; ++j)
            m_pathgatePos[r * 2 + j] = 0;   // 0x40+i*2+j word 基
    }
    for (r = 0; r < 4; ++r)                 // 复用 -0x20
    {
        for (int j = 0; j < 2; ++j)
            m_pathgateRange[r * 2 + j] = 0; // 0x48+i*2+j word 基
    }
    m_dungeonStartArea.clear();             // MapArea::clear(+0x50)
    m_openingBgm.clear();                   // Ss::clear(+99=0x18c)
    m_openingBgmParam = 0;                  // param_1[100]=0(0x190)
    m_sortBottom = 0;                       // byte(param_1+0x6b)=0
    m_vecSound.clear();                     // vector<string>::clear(+0x65=0x194)
    // p167：ORIG 调 `_ZNSsaSEPKc`（string::operator=），非 assign（P167 调用目标）
    m_str1a0 = "";                          // Ss::operator=(+0x68,"")（UNK_08c2f620=""）
    m_str1b0 = "PVP놀이게임";                // Ss::operator=(+0x6c,&UNK_08c2f621)
    m_playerCountMin = 2;                   // param_1[0x6d]=2
    m_playerCountMax = 8;                   // param_1[0x6e]=8
    for (r = 0; r < 5; ++r)                 // 5×MapArea::clear(+0x6f+i*7 words)（复用 -0x20）
        m_pvpStartArea[r].clear();
    m_24c = 0;                              // param_1[0x93]=0
    m_str1b0.clear();                       // Ss::clear(+0x6c)（汇编原序保留的重复清空）
    m_vecMonsterSpawnPos.clear();           // vector<STRivalMapData>::clear(+0x94)
    m_vecAICharacter.clear();               // vector<STAICharacterMapArrangeData>::clear(+0x97)
    m_apcRandomPoint = 0;                   // param_1[0x9a]=0
    m_mapLoadingImagePath.clear();          // Ss::clear(+0x9b)
    m_274 = 0;                              // param_1[0x9d]=0
    m_270 = m_274;                          // param_1[0x9c]=param_1[0x9d]
    m_vecEventMonsterPos.clear();           // vector<STRivalMapData>::clear(+0x9e)
    m_vecNpc.clear();                       // vector<AssignNpc>::clear(+0x18)
    m_index = 0;                            // *param_1=0
    m_vecAttackedMonster.clear();           // vector<STAttackedMonster>::clear(+0xc6)
    m_limitPlayerMoveAreas.clear();         // map<int,MapArea>::clear(+0xc0)
    m_maxMonsterCountException = 0;         // param_1[0xc9]=0
    m_328 = 0;                              // param_1[0xca]=0
    m_32c = 0;                              // param_1[0xcb]=0
    m_tournamentMonsters.clear();           // map<char,vector<TournamentMonster>>::clear(+0xae)
    m_moveLayeredType = 4;                  // param_1[0xd4]=4
    m_zposUnder = -1;                       // param_1[0xd5]=-1
    m_allDeadCasePassable = 0;              // byte(param_1+0xd6)=0
    m_screenEdgeLeft = -1;                  // param_1[0xd7]=-1
    m_screenEdgeRight = -1;                 // param_1[0xd8]=-1
    m_screenEdgeTop = -1;                   // param_1[0xd9]=-1
    m_screenEdgeBottom = -1;                // param_1[0xda]=-1
    m_blockUseActiveSkill = 0;              // byte(param_1+0xdb)=0
    m_blockUseStackableItem = 0;            // byte(0x36d)=0
    m_fixChampion = 0;                      // byte(0x36e)=0
    // ORIG Clear 0x0836c688：movb $0,0x248（m_pvpByPartySlot）+
    // 循环 movb $0,0x249+i（m_keepStateAxisX/Y/Z 三 byte）——布局修正补漏。
    m_pvpByPartySlot = 0;
    for (r = 0; r < 3; ++r)
        (&m_keepStateAxisX)[r] = 0;
    m_str370.clear();                       // Ss::clear(+0xdc)
    m_visibleMinimap = 1;                   // param_1[0xdd]=1
    m_visibleTownMinimap = 1;               // param_1[0xde]=1
}

// ORIG ScanChampionFeatures(int, STAICharacterMapArrangeData&) W 0x089e6dc4
//（同表匹配，push 到 +0x1c vector<int>；定义在 OnlyOrigBatch14.cpp）。
void ScanChampionFeatures(int count, STAICharacterMapArrangeData& data);

// ===========================================================================
// ImportMapScript —— ORIG T 0x089dcf54（标签分派逐分支转写；ORIG 的
// static map<const char*,int> 查找表还原为等价 if/else 链）
// ===========================================================================

// STMapScript 词索引视图（P82/P95 具名视图）：ORIG [pathgate pos]/
// [pathgate recognize range] 以 `words[i*2+j+0x40/0x48]` 形态做
// `add $imm,%ecx; mov %eax,(%base,%ecx,4)` 三步寻址（报告 0x1bae-0x1bd4、
// 0x1c35-0x1c58），数组基址即对象首（m_index@+0x000）。
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
bool ImportMapScript(STMapScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_szMapScriptBaseDirectory, path))
        return false;
    script->Clear();
    getDirectoryPath(script->m_str1a0, "%s%s", g_szMapScriptBaseDirectory, path);

    // 声明序 = ORIG prologue 构造/初始化序（槽位锚点，报告 0x0071..0x0126）：
    std::string tok;                                // -0x42c 主循环 token
    std::string sub;                                // -0x430 分支内子 token
    std::string tok3;                               // -0x434（[NPC] ScanStrOrInt 目标）
    int okInt = 0;                                  // -0x438（ScanInt 返回值暂存）
    bool ok = false;                                // -0x439（共享 ok flag）
    // ORIG 函数头构造、跨轮复用的解析对象（清理路径仅 dtor 其中 3 个非平凡者）。
    STAssignAnimation aniObj;                       // -0x450
    STAssignPassiveObject passiveObj;               // -0x460
    STAssignMonster monObj;                         // -0x76c
    STAssignSpecialPassiveObj spoObj;               // -0x67c
    AssignNpc npcObj;                               // -0x47c
    int normalMonsterCount = 0;                     // -0x13c（[monster] 计数）
    STAssignBloodSystem bloodSys;                   // -0x6a4
    STAssignBloodPhaseTime bloodPt;                 // -0x498
    STAssignDefenseSystem defSys;                   // -0x6cc
    STAssignDefensePhaseTime defPt;                 // -0x4b0
    bool pvpPracticeRead = false;                   // -0x135
    // ORIG 0x89dd081..0x89dd0aa：prologue 末尾至循环顶之间有 13 对
    // `jmp short <loop head>` + `nop` 填充（39 字节，不可达，ORIG 编译产物
    // 形态；nop 被 AE 口径剥离，13 条 jmp 参与计数）。以 .byte 序列原样补齐，
    // rel8 递减 3 与 ORIG 一致。
    __asm__ __volatile__(
        ".byte 0xeb,0x28,0x90, 0xeb,0x25,0x90, 0xeb,0x22,0x90\n\t"
        ".byte 0xeb,0x1f,0x90, 0xeb,0x1c,0x90, 0xeb,0x19,0x90\n\t"
        ".byte 0xeb,0x16,0x90, 0xeb,0x13,0x90, 0xeb,0x10,0x90\n\t"
        ".byte 0xeb,0x0d,0x90, 0xeb,0x0a,0x90, 0xeb,0x07,0x90\n\t"
        ".byte 0xeb,0x04,0x90");
    for (;;)
    {
        if (!ScanType(tok, true))
            break;

        if (tok == "[map name]")                            // 0x089dd0c1
        {
            ScanStr(&script->m_str1b0);
            continue;
        }
        if (tok == "[player number]")                       // 0x089dd10e
        {
            script->m_playerCountMin = ScanInt(static_cast<bool*>(0));
            script->m_playerCountMax = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[show dust]")                           // 0x089dd157
        {
            ScanStr(&sub);
            script->m_str1a4.append(sub);                   // Ss::append(+0x69)
            ScanStr(&sub);
            script->m_str1a8.append(sub);                   // Ss::append(+0x6a)
            continue;
        }
        if (tok == "[sort bottom]")                         // 0x089dd1c8
        {
            script->m_sortBottom = ScanInt(static_cast<bool*>(0)) == 1;
            continue;
        }
        if (tok == "[pvp start area]")                      // 0x089dd202
        {
            for (int i = 0; i < 3; ++i)
            {
                script->m_pvpStartArea[i].m_x = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_y = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_w = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_h = ScanInt(static_cast<bool*>(0));
            }
            continue;
        }
        if (tok == "[pvp start area by party slot]")        // 0x089dd309
        {
            int slotVal = ScanInt(static_cast<bool*>(0));   // -0x130（ORIG 先落槽再判）
            script->m_pvpByPartySlot = slotVal != 0;
            continue;
        }
        if (tok == "[map move keep state axis pos]")        // 0x089dd34d
        {
            int axisX = ScanInt(static_cast<bool*>(0));     // -0x12c
            int axisY = ScanInt(static_cast<bool*>(0));     // -0x128
            int axisZ = ScanInt(static_cast<bool*>(0));     // -0x124
            script->m_keepStateAxisX = axisX != 0;
            script->m_keepStateAxisY = axisY != 0;
            script->m_keepStateAxisZ = axisZ != 0;
            continue;
        }
        if (tok == "[pvp practice start area]")             // 0x089dd3db
        {
            for (int i = 3; i < 5; ++i)
            {
                script->m_pvpStartArea[i].m_x = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_y = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_w = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_h = ScanInt(static_cast<bool*>(0));
            }
            pvpPracticeRead = true;                         // cStack_139=1
            continue;
        }
        if (tok == "[shadow type]")                         // 0x089dd4e9
        {
            ScanStr(&sub);
            if (sub == "[mirror]")
                script->m_24c = 1;                          // +0x24c
            else if (sub == "[more darker]")
                script->m_24c = 2;
            continue;
        }
        if (tok == "[dungeon]")                             // 0x089dd56d
        {
            script->m_list4.clear();
            // [lane22 A/B 已回滚] ORIG 0x89dd57f 为 4.1 while 旋转 + bool 取反物化
            // （`movzbl ok; xor $1; test; je body`）。goto 尾标签环（探针 p4.cpp r3，
            // 唯一能出 `je` 的形）在真实上下文实测**不收敛**：4.4 多入口 `jmp L_cond`
            // + `nop` 且 body/cond 物理序仍相反（3673→3675，xor1 91→92）⇒ 回滚。
            while (okInt = ScanInt(&ok), ok == true)
                script->m_list4.push_back(okInt);
            continue;
        }
        if (tok == "[type]")                                // 0x089dd5d7
        {
            ScanStr(&sub);
            if (sub == "[normal]")
                script->m_c = 0;
            else if (sub == "[boss]")
                script->m_c = 1;
            else if (sub == "[dummy]")
                script->m_c = 2;
            continue;
        }
        if (tok == "[background pos]")                      // 0x089dd682
        {
            script->m_32c = ScanInt(static_cast<bool*>(0)); // param_1[0xcb]
            continue;
        }
        if (tok == "[greed]")                               // 0x089dd6b6
        {
            script->m_recoveryObjects.clear();
            ScanStr(&sub);
            // 声明序 = ORIG 初始化序（-0x11c/-0x118/-0x4b4/-0x114/-0x110）
            int len = static_cast<int>(sub.size());         // -0x11c
            int badCount = 0;                               // -0x118（未识别符号数）
            int pending = -1;                               // -0x4b4
            int pos = 0;                                    // -0x114
            int unusedGrid = 0;                             // -0x110（ORIG dead 槽，P31）
            while (pos < len - 1)
            {
                std::string two;                            // -0x4b8（每轮 ctor/dtor）
                two.append(sub.c_str() + pos, 2);           // Ss::append(PKcj)
                if (*two.c_str() == '\r' || *two.c_str() == '\n' ||
                    *two.c_str() == ' ')
                {
                    pos += 1;                               // 换行/空白：跳一格
                }
                else
                {
                    int sym = 0;                            // -0x4bc
                    for (; sym < 16; ++sym)
                    {
                        if (two.c_str()[0] == g_gridSymbols[sym][0] &&
                            two.c_str()[1] == g_gridSymbols[sym][1])
                            break;
                    }
                    if (sym == 16)
                    {
                        pos += 1;                           // 未识别符号：跳一格
                        ++badCount;
                    }
                    else
                    {
                        if (pending == -1)
                            pending = sym;
                        else
                        {
                            std::pair<int, int> objPair(pending, sym);  // 直接构造（P81）
                            script->m_recoveryObjects.push_back(objPair);
                            pending = -1;
                        }
                        pos += 2;
                    }
                }
            }
            if (badCount != 0)
                return false;                               // ORIG：有未识别格 → 失败
            (void)unusedGrid;
            continue;
        }
        if (tok == "[tile]")                                // 0x089dd90x 区
        {
            script->m_vecTile.clear();
            while (ScanStr(&sub))                           // 复用子 token（ORIG local_434）
                script->m_vecTile.push_back(sub);
            continue;
        }
        if (tok == "[extended tile]")
        {
            script->m_vecExtendedTile.clear();
            while (ScanStr(&sub))
                script->m_vecExtendedTile.push_back(sub);
            continue;
        }
        if (tok == "[elemental property]")                  // 0x089ddaef 区
        {
            ScanStr(&sub);
            script->m_element = getElement(sub.c_str());    // param_1[0x1b]
            continue;
        }
        if (tok == "[far sight scroll]")
        {
            script->m_sightScroll[0] = ScanInt(static_cast<bool*>(0));  // param_1[0x22]
            continue;
        }
        if (tok == "[middle sight scroll]")
        {
            script->m_sightScroll[1] = ScanInt(static_cast<bool*>(0));  // param_1[0x23]
            continue;
        }
        if (tok == "[near sight scroll]")
        {
            script->m_sightScroll[2] = ScanInt(static_cast<bool*>(0));  // param_1[0x24]
            continue;
        }
        if (tok == "[background animation]")                // 0x089ddaf0 区（__lookup 表段）
        {
            // ORIG：局部对象循环复用（BackAniScript ctor / string ctor 在 do 之前，
            // dtor 在段尾）；子键分派走函数局部静态表 __lookup / __lookup_0。
            script->m_vecBackAni.clear();                   // vector(+0xa0)::clear
            BackAniScript ani;                              // BackAniScript::BackAniScript
            std::string line;                               // local_4d8
            bool finished = false;                          // local_10d
            do
            {
                if (!ScanType(line, true))
                    return false;
                int id;
                while (true)                                // 构建轮完成后重入 find（0x1608）
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                for (;;)
                {
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(line.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x1ca:                                 // [ani info]
                {
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ani info]", 0x1ca));
                        /* fall through */
                        goto ani_next210;
                    }
                    ani.clear();                            // BackAniScript::clear
                    std::string subLine;                    // local_4e0
                    bool aniDone = false;                   // local_105
                    do
                    {
                        if (!ScanType(subLine, true))
                            return false;
                        int subId;
                        while (true)
                        {
                            static std::map<const char*, int, less_TCHAR> __lookup_0;
                            static SpinLock __init_lock_0;
                            static bool __inited_0 = false;
                        for (;;)
                        {
                            if (!__inited_0)
                            {
                                __init_lock_0.enter();
                                if (__inited_0)
                                {
                                    __init_lock_0.leave();
                                    continue;
                                }
                                subId = 0;
                                break;
                            }
                            std::map<const char*, int, less_TCHAR>::const_iterator it =
                                __lookup_0.find(subLine.c_str());
                            if (it != __lookup_0.end())
                                subId = it->second;
                            break;
                        }
                        switch (subId)
                        {
                        case 0:
                        case 0x1d7:                         // [filename]
                        {
                            if (!__inited_0)
                            {
                                __lookup_0.insert(std::map<const char*, int, less_TCHAR>::value_type("[filename]", 0x1d7));
                                goto sub_next;
                            }
                            if (!ScanStr(&sub))
                                return false;
                            ani.m_filename = sub;           // Ss::operator=(RKSs)
                            goto sub_tail;
                            break;
                        }
                        case 0x1dd:                         // [layer]
                        {
                            if (!__inited_0)
                            {
                                __lookup_0.insert(std::map<const char*, int, less_TCHAR>::value_type("[layer]", 0x1dd));
                                goto sub_next;
                            }
                            if (!ScanStr(&sub))
                                return false;
                            ani.m_layer = getDrawLayer(sub.c_str());  // local_4d0
                            goto sub_tail;
                            break;
                        }
                        case 0x1e3:                         // [order]
                        {
                            if (!__inited_0)
                            {
                                __lookup_0.insert(std::map<const char*, int, less_TCHAR>::value_type("[order]", 0x1e3));
                                goto sub_next;
                            }
                            if (!ScanStr(&sub))
                                return false;
                            if (sub == "[below]")
                                ani.m_orderUpper = false;   // local_4cc=0
                            else if (sub == "[upper]")
                                ani.m_orderUpper = true;    // local_4cc=1
                            else
                                return false;
                            goto sub_tail;
                            break;
                        }
                        case 0x1ee:                         // [loop y axis]
                        {
                            if (!__inited_0)
                            {
                                __lookup_0.insert(std::map<const char*, int, less_TCHAR>::value_type("[loop y axis]", 0x1ee));
                                goto sub_next;
                            }
                            int flag = -1;                  // local_4e8
                            if (ScanInt(&flag) != 1)
                                return false;
                            ani.m_loopYAxis = (flag != 0);  // uStack_4cb
                            goto sub_tail;
                            break;
                        }
                        case 0x1f8:                         // [screen pos]
                        {
                            if (!__inited_0)
                            {
                                __lookup_0.insert(std::map<const char*, int, less_TCHAR>::value_type("[screen pos]", 0x1f8));
                                goto sub_next;
                            }
                            int v = -1;                     // local_4ec
                            if (ScanInt(&v) != 1)
                                return false;
                            ani.m_screenX = v;              // uStack_4c8
                            if (ScanInt(&v) != 1)
                                return false;
                            ani.m_screenY = v;              // uStack_4c4
                            ani.m_hasScreenPos = true;      // uStack_4ca
                            goto sub_tail;
                            break;
                        }
                        case 0x205:                         // [/ani info]
                        {
                            if (!__inited_0)
                            {
                                __lookup_0.insert(std::map<const char*, int, less_TCHAR>::value_type("[/ani info]", 0x205));
                                goto sub_next;
                            }
                            script->m_vecBackAni.push_back(ani);  // vector(+0xa0)
                            aniDone = true;
                            goto sub_tail;
                            break;
                        }
                        default:
                            break;
                        }
                    sub_next:
                            if (!__inited_0)
                        {
                                    __init_lock_0.leave();
                                __inited_0 = true;
                            continue;                       // 重入本轮 find（0x1157）
                        }
                    sub_tail:
                        break;
                        }
                    } while (!aniDone);
                    break;
                }
                ani_next210:
                case 0x210:                                 // [/background animation]
                {
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/background animation]", 0x210));
                        break;
                    }
                    finished = true;
                    break;
                }
                default:
                    break;
                }
                    if (!__inited)
                {
                            __init_lock.leave();
                        __inited = true;
                    continue;                               // 重入本轮 find（0xbed）
                }
                        break;
                }
            } while (!finished);
            continue;
        }
        if (tok == "[town movable area]")                   // 0x089dbc5 区
        {
            script->m_vecTownMovableArea.clear();
            okInt = ScanInt(&ok);                           // 0x16bf：循环前一次消费
            while (ok == true)
            {
                TownPath tp;                                // -0x704 TownPath::TownPath
                tp.m_x = okInt;
                tp.m_y = ScanInt(static_cast<bool*>(0));
                tp.m_w = ScanInt(static_cast<bool*>(0));
                tp.m_h = ScanInt(static_cast<bool*>(0));
                tp.m_field1c = ScanInt(static_cast<bool*>(0));
                tp.m_field20 = ScanInt(static_cast<bool*>(0));
                {                                           // 可选 [quest condition]
                    bool isType;                            // -0x4ed（ORIG 无初始化存储）
                    std::string s;                          // -0x4ec
                    okInt = ScanTypeOrInt(isType, s, ok, true);
                    if (isType != false)
                    {
                        if (s == "[quest condition]")
                        {
                            while (okInt = ScanInt(&ok), ok == true)
                                tp.m_conds.push_back(okInt);
                            okInt = ScanInt(&ok);           // ORIG while 后额外消费一 token
                        }
                        else
                            ok = false;                     // 0x1814：非 quest → 终止循环
                    }
                }
                script->m_vecTownMovableArea.push_back(tp);
            }
            continue;
        }
        if (tok == "[virtual movable area]")
        {
            script->m_vecVirtualMovableArea.clear();
            okInt = ScanInt(&ok);                           // 0x18ee
            while (ok == true)
            {
                MapArea area;                               // -0x50c MapArea::MapArea
                area.m_x = okInt;
                area.m_y = ScanInt(static_cast<bool*>(0));
                area.m_w = ScanInt(static_cast<bool*>(0));
                area.m_h = ScanInt(static_cast<bool*>(0));
                {                                           // 可选 [quest condition]
                    bool isType;                            // -0x511（ORIG 无初始化存储）
                    std::string s;                          // -0x510
                    okInt = ScanTypeOrInt(isType, s, ok, true);
                    if (isType != false)
                    {
                        if (s == "[quest condition]")
                        {
                            while (okInt = ScanInt(&ok), ok == true)
                                area.m_list.push_back(okInt);
                            okInt = ScanInt(&ok);
                        }
                        else
                            ok = false;                     // 0x19ed：非 quest → 终止循环
                    }
                }
                script->m_vecVirtualMovableArea.push_back(area);
            }
            continue;
        }
        if (tok == "[guild agit entrance info]")
        {
            script->m_vecGuildAgitEntrance.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x1a95：赋值在循环头
            {
                TownPath tp;                                // -0x704 复用
                tp.m_x = okInt;
                tp.m_y = ScanInt(static_cast<bool*>(0));
                tp.m_w = ScanInt(static_cast<bool*>(0));
                tp.m_h = ScanInt(static_cast<bool*>(0));
                tp.m_field1c = ScanInt(static_cast<bool*>(0));
                tp.m_field20 = ScanInt(static_cast<bool*>(0));
                script->m_vecGuildAgitEntrance.push_back(tp);
            }
            continue;
        }
        if (tok == "[pathgate pos]")
        {
            for (int i = 0; i < 4; ++i)                 // -0xfc/-0xf8 双层 setle 循环
                for (int j = 0; j < 2; ++j)
                    reinterpret_cast<STMapScriptWordView*>(script)->m_words[i * 2 + j + 0x40] =
                        ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[pathgate recognize range]")
        {
            for (int i = 0; i < 4; ++i)                 // -0xf4/-0xf0
                for (int j = 0; j < 2; ++j)
                    reinterpret_cast<STMapScriptWordView*>(script)->m_words[i * 2 + j + 0x48] =
                        ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[dungeon start area]")
        {
            script->m_dungeonStartArea.m_x = ScanInt(static_cast<bool*>(0));
            script->m_dungeonStartArea.m_y = ScanInt(static_cast<bool*>(0));
            script->m_dungeonStartArea.m_w = ScanInt(static_cast<bool*>(0));
            script->m_dungeonStartArea.m_h = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[opening bgm]")
        {
            ScanStr(&sub);
            script->m_openingBgm = sub;                     // Ss::assign(RKSs)
            script->m_openingBgmParam = ScanInt(static_cast<bool*>(0));  // +0x190
            continue;
        }
        if (tok == "[sound]")
        {
            script->m_vecSound.clear();
            while (ScanStr(&sub))
                script->m_vecSound.push_back(sub);
            continue;
        }
        if (tok == "[animation]")
        {
            script->m_listAnimation.clear();
            while (ScanStr(&aniObj.m_name))                 // 函数头复用对象（无逐轮 clear）
            {
                ScanStr(&sub);
                aniObj.m_layer = getDrawLayer(sub.c_str());
                aniObj.m_field08 = ScanInt(static_cast<bool*>(0));
                aniObj.m_field0c = ScanInt(static_cast<bool*>(0));
                aniObj.m_field10 = ScanInt(static_cast<bool*>(0));
                script->m_listAnimation.push_back(aniObj);
            }
            continue;
        }
        if (tok == "[passive object]")
        {
            script->m_listPassiveObject.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x1ea8 循环头赋值
            {
                passiveObj.m_0 = okInt;                     // 函数头复用对象
                passiveObj.m_4 = ScanInt(static_cast<bool*>(0));
                passiveObj.m_8 = ScanInt(static_cast<bool*>(0));
                passiveObj.m_c = ScanInt(static_cast<bool*>(0));
                script->m_listPassiveObject.push_back(passiveObj);
            }
            continue;
        }
        if (tok == "[monster]" || tok == "[monster condition]")  // code_r0x089deeb7
        {
            std::vector<STAssignMonster>* target = &script->m_vecMonster;
            if (tok == "[monster]")
                script->m_vecMonster.clear();               // vector(+0x34)
            else
            {
                script->m_vecConditionMonster.clear();      // vector(+0x40)
                target = &script->m_vecConditionMonster;
            }
            while (okInt = ScanInt(&ok), ok == true)        // 0x1fd4 循环头
            {
                monObj.Clear();                             // 函数头复用对象
                monObj.m_0 = okInt;
                // [STMapScript sweep] ORIG 0x089defc1 段 `cmp $0x1770,%eax; jbe` ⇒ 计数为
                // 无符号（P196/T2 直证）；int 生 jle 语义分叉（负数计数值域）。
                static unsigned int named_count = 0;        // 函数级静态校验计数
                if ((okInt > 49999 && okInt < 0xc739) ||
                    (okInt > 55999 && okInt < 0xea61))
                {
                    named_count = named_count + 1;
                    if (named_count > 6000)
                        return false;
                }
                monObj.m_4 = ScanInt(static_cast<bool*>(0)) != 0;
                monObj.m_8 = ScanInt(static_cast<bool*>(0));
                monObj.m_c = ScanInt(static_cast<bool*>(0));
                monObj.m_10 = ScanInt(static_cast<bool*>(0));
                monObj.m_14 = ScanInt(static_cast<bool*>(0));
                monObj.m_pair18.m_pair.m_lo = ScanInt(static_cast<bool*>(0));   // +0x18
                monObj.m_pair18.m_pair.m_hi = ScanInt(static_cast<bool*>(0));   // +0x1c
                ScanStr(&sub);
                if (sub == "[fixed]")                       // uStack_750
                    monObj.m_20 = 0;
                else if (sub == "[half]")
                    monObj.m_20 = 1;
                else if (sub == "[ratio]")
                    monObj.m_20 = 2;
                ScanStr(&sub);
                if (sub == "[NPC]")                         // NPC 携带 idx→+0x58
                {
                    monObj.m_58 = ScanInt(static_cast<bool*>(0));
                    ScanStr(&sub);
                }
                if (sub == "[normal]")                      // uStack_74c
                    monObj.m_24 = 0;
                else if (sub == "[champion]")
                {
                    monObj.m_24 = 1;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0xe8
                    // [STMapScript sweep] ORIG 0x089dfxxx 段为 `test; jle <clear 块>`（Scan 直落）
                    // ⇒ 源码按 if(>0){Scan} else {clear} 书写才复现块序（P47/P222）。
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, monObj);
                    else
                        monObj.m_vec28.clear();
                }
                else if (sub == "[super champion]")
                {
                    monObj.m_24 = 2;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0xe4
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, monObj);
                    else
                        monObj.m_vec28.clear();
                }
                else if (sub == "[boss]")
                {
                    // [STMapScript sweep] ORIG 0x089df24e `cmp $0x5dc,%eax; jbe` ⇒ unsigned。
                    static unsigned int boss_count = 0;
                    boss_count = boss_count + 1;
                    if (boss_count > 0x5dc)
                        return false;
                    monObj.m_24 = 3;
                }
                monObj.m_34 = 0;                            // local_73c=0（push 前复位）
                int total = static_cast<int>(script->m_vecMonster.size());  // esi 先算
                total = total + static_cast<int>(script->m_vecConditionMonster.size());
                (void)total;                                // ORIG 死存储（校验和，未用）
                target->push_back(monObj);
                if (tok == "[monster]")
                    ++normalMonsterCount;                   // iStack_140++
            }
            continue;
        }
        if (tok == "[blood monster]" || tok == "[ultimate monster]")  // code_r0x089df2ff
        {
            int lineIdx = 0;                                // iStack_e4
            script->m_vecBloodSystem.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x23d9 循环头
            {
                bloodSys.Clear();                           // 函数头复用对象
                bloodSys.m_id = okInt;
                bloodSys.m_field04 = ScanInt(static_cast<bool*>(0));
                bloodSys.m_phase = ScanInt(static_cast<bool*>(0));
                bloodSys.m_field0c = ScanInt(static_cast<bool*>(0));
                bloodSys.m_field10 = ScanInt(static_cast<bool*>(0));
                bloodSys.m_timeA = ScanInt(static_cast<bool*>(0));
                bloodSys.m_timeB = ScanInt(static_cast<bool*>(0));
                bloodSys.m_startCnt = ScanInt(static_cast<bool*>(0));
                bloodSys.m_cntInc = ScanInt(static_cast<bool*>(0));
                bloodSys.m_totalCnt = ScanInt(static_cast<bool*>(0));
                if (bloodSys.m_timeA + -500 <= bloodSys.m_totalCnt * bloodSys.m_timeB)
                {
                    printf("\n(%s) [blood monster](%d) GROUP : MATCHING TIME ERROR!!\n",
                           path, lineIdx);
                    return false;
                }
                script->m_vecBloodSystem.push_back(bloodSys);
                lineIdx = lineIdx + 1;
            }
            continue;
        }
        if (tok == "[blood phase time]" || tok == "[ultimate phase time]")  // code_r0x089df4b2
        {
            script->m_vecBloodPhaseTime.clear();
            int e0 = 0;                                     // iStack_e0 行序
            int dc = 0;                                     // iStack_dc 已见 spawn 组数
            int d8 = 0;                                     // iStack_d8 上上 spawnCnt
            int d4 = 0;                                     // iStack_d4 上 spawnCnt（mismatch 基线）
            int d0 = 0;                                     // iStack_d0
            int cc = 0;                                     // iStack_cc 同 round 连击数
            int c8 = 0;                                     // iStack_c8 上 spawnCnt
            int c4 = 0;                                     // iStack_c4 上 round（首值 0）
            int c0 = -1;                                    // iStack_c0 上 order
            int bc = 0;                                     // iStack_bc 上 interval
            bool unusedB6 = false;                          // -0xb2（ORIG dead 槽，P31）
            (void)unusedB6;
            while (okInt = ScanInt(&ok), ok == true)        // 0x25ed 循环头
            {
                bloodPt.Clear();                            // 函数头复用对象
                bloodPt.m_smallRoundOrder = okInt;
                bloodPt.m_phaseIdx = ScanInt(static_cast<bool*>(0));
                bloodPt.m_interval = ScanInt(static_cast<bool*>(0));
                bloodPt.m_time = static_cast<float>(ScanFloat(static_cast<bool*>(0)));
                bloodPt.m_field10 = ScanInt(static_cast<bool*>(0));
                bloodPt.m_spawnCnt = ScanInt(static_cast<bool*>(0));
                if (tok == "[ultimate phase time]")         // ultimate 才读难度名
                {
                    bool diffOk = false;                    // -0xb1（char）
                    std::string diffTok;                    // -0x518（每轮 ctor/dtor）
                    diffOk = ScanStr(&diffTok);
                    if (!diffOk)
                        return false;                       // 0x2630 fail 出口
                    if (diffTok == "A")
                        bloodPt.m_difficulty = 1;
                    else if (diffTok == "B")
                        bloodPt.m_difficulty = 2;
                    else if (diffTok == "C")
                        bloodPt.m_difficulty = 3;
                    else if (diffTok == "D")
                        bloodPt.m_difficulty = 4;
                    else if (diffTok == "E")
                        bloodPt.m_difficulty = 5;
                    else
                        return false;                       // 0x26f1 未知难度 → 失败
                }
                if (bloodPt.m_phaseIdx >=
                    static_cast<int>(script->m_vecBloodSystem.size()))
                {
                    printf("\n(%s) [blood phase time](%d) MONSTER LISTINDEX ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (bloodPt.m_smallRoundOrder != c4 && bloodPt.m_field10 != 0)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : SPAWN ORDER IS NOT ZERO. ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (bloodPt.m_spawnCnt > 10)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : TIMER CNT OVER!!\n",
                           path, e0);
                        return false;
                }
                if (tok == "[blood phase time]" && bloodPt.m_smallRoundOrder == c4 &&
                    bloodPt.m_field10 == c0)
                {
                    if (d4 == 0 || bloodPt.m_spawnCnt == 0 || d4 < bloodPt.m_spawnCnt)
                    {
                        printf("\n(%s) [blood phase time](%d) SMALLROUND : ORDER - ORDER_CNT MISMATCHING!!\n",
                               path, e0);
                        return false;
                }
                    if (bloodPt.m_interval < bc && bloodPt.m_field10 == 0)
                    {
                        printf("\n(%s) [blood phase time](%d) SMALLROUND INTERVAL TIME ORDER ERROR:!!\n",
                               path, e0);
                        return false;
                }
                }
                // [STMapScript sweep] ORIG 0x089df8c1 段为 `jl ERR; sub c4-X; test; jle OK`
                // （无 js/je）；`(c4!=X && -1<c4-X)` 与 `c4-X>0` 整数等价（P214/P222 块序）。
                if (bloodPt.m_smallRoundOrder < c4 ||
                    c4 - bloodPt.m_smallRoundOrder > 0)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : ROUND ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (tok == "[blood phase time]" && bloodPt.m_smallRoundOrder == c4 &&
                    (bloodPt.m_field10 < c0 || c0 - bloodPt.m_field10 > 0))
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : ROUND ORDER ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (e0 == 0 && (bloodPt.m_field10 != 0 || bloodPt.m_smallRoundOrder != 0))
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : FIRST SPAWN CNT || ORDER IS NOT ZERO. ERROR || ROUND IS NOT ZERO. ERROR!!\n",
                           path, 0);
                        return false;
                }
                if (d8 != 0 && c8 < bloodPt.m_spawnCnt)
                {
                    if (dc != d8)
                    {
                        printf("\n(%s) [blood phase time](%d) SMALLROUND : SPAWN CNT ERROR!!\n",
                               path, e0);
                        return false;
                }
                    dc = 0;
                    d8 = 0;
                }
                // [STMapScript sweep] ORIG 0x089df9xx 段为 `test; jle <spawnCnt<1 块>`（else 直落，
                // P272 德摩根块序交换）；`< 1` 形在 4.4 反产 `jg <else>`。
                if (bloodPt.m_spawnCnt >= 1)
                {
                    if (bloodPt.m_spawnCnt != 1)
                        d8 = bloodPt.m_spawnCnt;
                    dc = dc + 1;
                }
                else
                {
                    if (dc != 0)
                    {
                        if (dc != d8)
                        {
                            printf("\n(%s) [blood phase time](%d) SMALLROUND : SPAWN CNT ERROR!!\n",
                                   path, e0);
                        return false;
                }
                        dc = 0;
                        d8 = 0;
                }
                }
                if (bloodPt.m_smallRoundOrder == c4)
                {
                    d0 = bloodPt.m_smallRoundOrder;
                    cc = cc + 1;
                }
                else
                {
                    cc = 0;
                }
                (void)d0; (void)cc; (void)dc;
                c8 = bloodPt.m_spawnCnt;
                c4 = bloodPt.m_smallRoundOrder;
                c0 = bloodPt.m_field10;
                d4 = bloodPt.m_spawnCnt;
                bc = bloodPt.m_interval;
                e0 = e0 + 1;
                script->m_vecBloodPhaseTime.push_back(bloodPt);
                }
            {   // ORIG 收尾校验：最后两条 round 相同且最后一条 spawnCnt!=0 → 失败
                // [STMapScript sweep] P183：ORIG 0x089df541/0x089df55x 每处重发
                // `mov 0x8(%ebp),%edx; add $0x294,%edx`（三次独立 operator[]）；
                // 引用局部会缓存 &vec 到 -0xc8(%ebp) 并省掉两次 add。
                int last = script->m_vecBloodPhaseTime[
                    static_cast<unsigned int>(e0 - 1)].m_smallRoundOrder;
                int prev = script->m_vecBloodPhaseTime[
                    static_cast<unsigned int>(e0 - 2)].m_smallRoundOrder;
                if (last == prev &&
                    script->m_vecBloodPhaseTime[
                        static_cast<unsigned int>(e0 - 1)].m_spawnCnt != 0)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : LAST SMALL ROUND ERROR!!\n",
                           path, e0);
                        return false;
                }
            }
            continue;
        }
        if (tok == "[time line]")                           // call 0x0889a930
        {
            // ORIG 只压 1 个入参（&m_timeLine）：按调用点原样，this 与引用同值。
            reinterpret_cast<advancealtar::StageTimeLineParameter*>(
                &script->m_timeLine)->importPhase(script->m_timeLine);
            continue;
        }
        if (tok == "[defense monster]")
        {
            int lineIdx = 0;                                // iStack_b4
            script->m_vecDefenseSystem.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x2cf7 循环头
            {
                defSys.Clear();                             // 函数头复用对象
                defSys.m_id = okInt;
                defSys.m_field04 = ScanInt(static_cast<bool*>(0));
                defSys.m_phase = ScanInt(static_cast<bool*>(0));
                defSys.m_field0c = ScanInt(static_cast<bool*>(0));
                defSys.m_field10 = ScanInt(static_cast<bool*>(0));
                defSys.m_timeA = ScanInt(static_cast<bool*>(0));
                defSys.m_timeB = ScanInt(static_cast<bool*>(0));
                defSys.m_startCnt = ScanInt(static_cast<bool*>(0));
                defSys.m_cntInc = ScanInt(static_cast<bool*>(0));
                defSys.m_totalCnt = ScanInt(static_cast<bool*>(0));
                if (defSys.m_timeA + -500 <= defSys.m_totalCnt * defSys.m_timeB)
                {
                    printf("\n(%s) [Defense monster](%d) GROUP : MATCHING TIME ERROR!!\n",
                           path, lineIdx);
                    return false;
                }
                script->m_vecDefenseSystem.push_back(defSys);
                lineIdx = lineIdx + 1;
            }
            continue;
        }
        if (tok == "[defense phase time]")
        {
            script->m_vecDefensePhaseTime.clear();
            int b0 = 0;                                     // iStack_b0 行序
            int cAc = 0;                                    // iStack_ac（dead，P31）
            int cA8 = 0;                                    // iStack_a8（dead）
            int a4 = 0;                                     // iStack_a4 上 spawnCnt
            int a0 = 0;                                     // iStack_a0
            int c98 = 0;                                    // iStack_98 上 spawnCnt
            int c9c = 0;                                    // iStack_9c 同 round 连击数
            int c94 = 0;                                    // iStack_94 上 round
            int c90 = -1;                                   // iStack_90 上 order
            int c8c = 0;                                    // iStack_8c 上 interval
            bool unused85 = false;                          // iStack_85（dead bool，P31）
            (void)cAc; (void)cA8; (void)unused85;
            while (okInt = ScanInt(&ok), ok == true)        // 0x2ee1 循环头
            {
                defPt.Clear();                              // 函数头复用对象
                defPt.m_smallRoundOrder = okInt;
                defPt.m_phaseIdx = ScanInt(static_cast<bool*>(0));
                defPt.m_interval = ScanInt(static_cast<bool*>(0));
                defPt.m_time = static_cast<float>(ScanFloat(static_cast<bool*>(0)));
                defPt.m_field10 = ScanInt(static_cast<bool*>(0));
                defPt.m_spawnCnt = ScanInt(static_cast<bool*>(0));
                if (defPt.m_phaseIdx >=
                    static_cast<int>(script->m_vecDefenseSystem.size()))
                {
                    printf("\n(%s) [Defense phase time](%d) MONSTER LISTINDEX ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (defPt.m_smallRoundOrder != c94 && defPt.m_field10 != 0)
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : SPAWN ORDER IS NOT ZERO. ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (defPt.m_spawnCnt > 10)
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : TIMER CNT OVER!!\n",
                           path, b0);
                    return false;
                }
                // ORIG 此处以大写 D 的 "[Defense phase time]" 与 token 比较（实际 token 为
                // 小写，判定恒 false）——按 ORIG 字面照录。
                if (tok == "[Defense phase time]" && defPt.m_smallRoundOrder == c94 &&
                    defPt.m_field10 == c90)
                {
                    if (a4 == 0 || defPt.m_spawnCnt == 0 || a4 < defPt.m_spawnCnt)
                    {
                        printf("\n(%s) [Defense phase time](%d) SMALLROUND : ORDER - ORDER_CNT MISMATCHING!!\n",
                               path, b0);
                        return false;
                    }
                    if (defPt.m_interval < c8c && defPt.m_field10 == 0)
                    {
                        printf("\n(%s) [Defense phase time](%d) SMALLROUND INTERVAL TIME ORDER ERROR:!!\n",
                               path, b0);
                        return false;
                    }
                }
                // [STMapScript sweep] 同 blood 段：ORIG 0x089e005d 为 `jl; sub; test; jle`。
                if (defPt.m_smallRoundOrder < c94 ||
                    c94 - defPt.m_smallRoundOrder > 0)
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : ROUND ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (tok == "[Defense phase time]" && defPt.m_smallRoundOrder == c94 &&
                    (defPt.m_field10 < c90 || c90 - defPt.m_field10 > 0))
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : ROUND ORDER ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (b0 == 0 && (defPt.m_field10 != 0 || defPt.m_smallRoundOrder != 0))
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : FIRST SPAWN CNT || ORDER IS NOT ZERO. ERROR || ROUND IS NOT ZERO. ERROR!!\n",
                           path, 0);
                    return false;
                }
                if (defPt.m_smallRoundOrder == c94)
                {
                    a0 = defPt.m_smallRoundOrder;
                    c98 = c98 + 1;
                }
                else
                {
                    c98 = 0;
                }
                (void)a0; (void)c98;
                c94 = defPt.m_smallRoundOrder;
                c90 = defPt.m_field10;
                c8c = defPt.m_interval;
                a4 = defPt.m_spawnCnt;
                b0 = b0 + 1;
                script->m_vecDefensePhaseTime.push_back(defPt);
            }
            continue;
        }
        if (tok == "[summon start area]")
        {
            script->m_summonStartArea.m_x = ScanInt(static_cast<bool*>(0));
            script->m_summonStartArea.m_y = ScanInt(static_cast<bool*>(0));
            script->m_summonStartArea.m_w = ScanInt(static_cast<bool*>(0));
            script->m_summonStartArea.m_h = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[center index]")                         // 0x089e042c
        {
            okInt = ScanInt(&ok);
            if (!ok)
                break;                                      // 0x3358 → 主循环出口（收尾）
            script->m_178 = okInt;                          // +0x178
            continue;
        }
        if (tok == "[/center index]")                        // 0x089e0457
            continue;
        if (tok == "[camera edge exception]")                // 0x089e0463
        {
            okInt = ScanInt(&ok);
            if (!ok)
                break;                                      // 0x33ca → 主循环出口
            script->m_17c = okInt;                          // +0x17c
            continue;
        }
        if (tok == "[/camera edge exception]")               // 0x089e0492
            continue;
        if (tok == "[camera edge recovery object]")         // 0x089e036c（0x341f 位序）
        {
            int x = -1;                                     // -0x51c
            int y = 0;                                      // -0x520
            while (true)                                    // 0x3434 两级 && 短路口
            {
                okInt = ScanInt(&ok);
                if (ok != true)
                    break;
                x = okInt;
                okInt = ScanInt(&ok);
                if (ok != true)
                    break;
                y = okInt;
                // P178/P135：ORIG push_back(pair&&)（MISSING EOS1_）；嵌套转换临时形
                script->m_vec180.push_back(std::pair<int, int>(std::make_pair(x, y)));
            }
            continue;
        }
        if (tok == "[heroes mode map index]")               // 0x089e0435
        {
            script->m_maxMonsterDropException = ScanInt(&ok);   // +0x284 直存
            if (!ok)
                break;                                      // 0x34fb → 主循环出口
            continue;
        }
        if (tok == "[monster team]")                        // 0x089e049f 区
        {
            for (int i = 0; i < normalMonsterCount; ++i)
                script->m_vecMonster[i].m_4c = ScanInt(static_cast<bool*>(0));
            ScanStr(&sub);
            continue;
        }
        if (tok == "[monster specific AI]")                 // 0x089e050f 区
        {
            for (int i = 0; i < normalMonsterCount; ++i)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[normal]")
                    script->m_vecMonster[i].m_34 = 0;
                else if (sub == "[zone defence]")
                {
                    script->m_vecMonster[i].m_34 = 1;
                    script->m_vecMonster[i].m_pair38.m_arr[0] = ScanInt(static_cast<bool*>(0));
                    script->m_vecMonster[i].m_pair38.m_arr[1] = ScanInt(static_cast<bool*>(0));
                }
                else if (sub == "[roaming]")
                {
                    script->m_vecMonster[i].m_34 = 2;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0x78
                    for (int k = 0; k < cnt; ++k)
                    {
                        std::pair<int, int> way;                // -0x528 默认 ctor
                        way.first = ScanInt(static_cast<bool*>(0));
                        way.second = ScanInt(static_cast<bool*>(0));
                        script->m_vecMonster[i].m_vec40.push_back(way);
                    }
                }
            }
            ScanStr(&sub);
            continue;
        }
        if (tok == "[monster collision group]")             // 0x089e0660 区
        {
            for (std::vector<STAssignMonster>::iterator it = script->m_vecMonster.begin();
                 it != script->m_vecMonster.end(); it++)
            {
                it->m_vec5c.clear();
                int cnt = ScanInt(&ok);
                for (int k = 0; k < cnt; ++k)
                {
                    unsigned char v = static_cast<unsigned char>(ScanInt(&ok));  // local_531
                    it->m_vec5c.push_back(v);
                }
            }
            ScanType(tok, true);                            // 收尾 consume 标记
            continue;
        }
        if (tok == "[apc collision group]")                 // 0x089e07f0 区
        {
            for (std::vector<STAICharacterMapArrangeData>::iterator it =
                     script->m_vecAICharacter.begin();
                 it != script->m_vecAICharacter.end(); it++)
            {
                it->m_collisionGroup.clear();
                int cnt = ScanInt(&ok);
                for (int k = 0; k < cnt; ++k)
                {
                    unsigned char v = static_cast<unsigned char>(ScanInt(&ok));  // local_539
                    it->m_collisionGroup.push_back(v);
                }
            }
            ScanType(tok, true);
            continue;
        }
        if (tok == "[champion]")                            // 0x089e097b
        {
            script->m_championRate = ScanInt(static_cast<bool*>(0));  // +0x4c
            continue;
        }
        if (tok == "[special passive object]")              // 0x089e099c 区
        {
            std::list<STAssignItem> assignItems(script->m_listItem);  // local_544 copy
            script->m_listSpecialPassive.clear();
            int group = 0;                                  // iStack_64
            while (okInt = ScanInt(&ok), ok == true)        // 0x3a7d 循环头
            {
                spoObj.Clear();                             // 函数头复用对象
                spoObj.m_0 = okInt;
                spoObj.m_4 = ScanInt(static_cast<bool*>(0));
                spoObj.m_8 = ScanInt(static_cast<bool*>(0));
                spoObj.m_c = ScanInt(static_cast<bool*>(0));
                int actionCount = ScanInt(static_cast<bool*>(0));   // local_60
                bool hasItem = false;                       // local_59
                for (int k = 0; k < actionCount; ++k)       // local_58
                {
                    bool actionError = false;               // local_51（ORIG 恒 false）
                    SPOBJ_ACTION_DATAS action;              // ctor memset 0xff
                    procSpecialPassiveObject(action, spoObj,
                                             g_stMapScriptFileList.FindIndexByFullScan(path));
                    if (action.getActionType() == 2)
                        hasItem = true;
                    if (action.getActionType() != 5)
                        spoObj.m_vec10.push_back(action);
                    if (actionError)
                        return false;
                }
                if (hasItem)
                {
                    spoObj.m_1c = group;                    // local_664（组序）
                    group = group + 1;
                }
                script->m_listSpecialPassive.push_back(spoObj);
            }
            continue;
        }
        if (tok == "[NPC]")                                 // 0x089e0ba1
        {
            script->m_vecNpc.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x3c6c 循环头
            {
                bool wasStr;                                // -0x559（ScanStrOrInt 首参，无独立初始化）
                wasStr = false;                             // 0x3c96：movb $0,-0x559
                npcObj.m_npcIdx = okInt;                    // 函数头复用对象（npcObj+0）
                okInt = ScanStrOrInt(wasStr, tok3, ok, true);
                if (ok != true)
                    break;
                if (wasStr != true)                         // 第二 token 为 int
                {
                    npcObj.m_field04 = okInt;               // -0x478（npcObj+4）
                    ScanStr(&tok3);
                }
                if (tok3 == "[visible on dungeon clear]")
                {
                    npcObj.m_visible = true;                // uStack_478（ORIG 不复零）
                    ScanStr(&tok3);
                }
                if (tok3 == "[left]")
                    npcObj.m_dir = 0;                       // uStack_474=0
                else if (tok3 == "[right]")
                    npcObj.m_dir = 1;
                npcObj.m_x = ScanInt(static_cast<bool*>(0));
                npcObj.m_y = ScanInt(static_cast<bool*>(0));
                npcObj.m_z = ScanInt(static_cast<bool*>(0));
                script->m_vecNpc.push_back(npcObj);
            }
            continue;
        }
        if (tok == "[monster spawn pos]")                   // 0x089e0d43
        {
            script->m_vecMonsterSpawnPos.clear();
            int count = ScanInt(static_cast<bool*>(0));     // -0x4c
            STRivalMapData proto;                           // -0x568 ctor（循环外一次）
            script->m_vecMonsterSpawnPos.reserve(count * 3);  // 0x3e29 imul 形态
            for (int i = 0; i < count; ++i)
            {
                proto.m_field00 = ScanInt(static_cast<bool*>(0));
                proto.m_field04 = ScanInt(static_cast<bool*>(0));
                proto.m_field08 = ScanInt(static_cast<bool*>(0));
                script->m_vecMonsterSpawnPos.push_back(proto);
            }
            continue;
        }
        if (tok == "[ai character]")                        // 0x089e0e00 区
        {
            int type = -1;                                  // -0x44
            while (type = ScanInt(&ok), ok == true)         // 0x3edd 循环头
            {
                STAICharacterMapArrangeData ai;             // -0x704 ctor（含 vector 成员）
                ai.m_type = type;
                ai.m_field04 = ScanInt(static_cast<bool*>(0));
                ai.m_field08 = ScanInt(static_cast<bool*>(0));
                ai.m_field0c = ScanInt(static_cast<bool*>(0));
                {                                           // 阵营：character/monster/neutral
                    ScanTypeOrStr(ok, tok, true);            // local_430 被覆盖（ORIG 原样）
                    if (tok == "[character]")
                        ai.m_camp = 0;
                    else if (tok == "[monster]")
                        ai.m_camp = 100;
                    else if (tok == "[neutral]")
                        ai.m_camp = 200;
                }
                char hasTok = ScanStr(&sub);                // -0x3d（char 直存 al）
                if (sub == "[NPC]")                         // 0x3ff8 仅比对 token，不查 hasTok
                {
                    ai.m_npcIdx = ScanInt(static_cast<bool*>(0));
                    hasTok = ScanStr(&sub);
                }
                if (hasTok != 1)
                {
                    sub = "[normal]";                       // Ss::operator=(PKc)
                }
                if (sub == "[normal]")
                    ai.m_roleType = 5;
                else if (sub == "[champion]")               // uStack_6f4
                {
                    ai.m_roleType = 6;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0x3c
                    // [STMapScript sweep] 同 monster 段：ORIG 为 `test; jle <clear>`（Scan 直落）。
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, ai);
                    else
                        ai.m_championFeatures.clear();
                }
                else if (sub == "[super champion]")
                {
                    ai.m_roleType = 7;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0x38
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, ai);
                    else
                        ai.m_championFeatures.clear();
                }
                else if (sub == "[boss]")
                {
                    // [STMapScript sweep] ORIG 0x089e10d3 `cmp $0x5dc,%eax; jbe` ⇒ unsigned。
                    static unsigned int apc_boss_count = 0;
                    // 两句式：ORIG 是 `count = count + 1;` 后重新读槽比较（0x089e10c0 段）；
                    // `++count >= K` 前缀形在 4.4 生 seta 物化（P225 族），偏离 jbe 直跳。
                    apc_boss_count = apc_boss_count + 1;
                    if (apc_boss_count > 0x5dc)
                        return false;
                    ai.m_roleType = 8;
                }
                    else
                    return false;                           // 未知 role → 失败
                ai.m_posX = ScanInt(&ok);                   // -0x6f0（char 直存 al）
                if (!ok)
                    return false;
                ai.m_posY = ScanInt(&ok);                   // -0x6eb
                if (!ok)
                    return false;
                script->m_vecAICharacter.push_back(ai);
            }
            continue;
        }
        if (tok == "[apc random point]")                    // 0x089e11c8
        {
            script->m_apcRandomPoint = ScanInt(static_cast<bool*>(0));  // +0x268
            continue;
        }
        if (tok == "[event monster position]")              // 0x089e11ec
        {
            STRivalMapData proto;                           // -0x574 ctor 循环外一次
            while (true)
            {
                proto.m_field00 = ScanInt(&ok);
                if (!ok)
                    return false;                           // 0x42d0 首 token 失败也 false
                proto.m_field04 = ScanInt(&ok);
                if (!ok)
                    return false;
                proto.m_field08 = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_vecEventMonsterPos.push_back(proto);
            }
            continue;
        }
        if (tok == "[map loading image path]")              // 0x089e12cc
        {
            ScanStr(&script->m_mapLoadingImagePath);        // param_1+0x9b
            continue;
        }
        if (tok == "[quest animation]")
        {
            script->m_listQuestAnimation.clear();
            STQuestAnimation qani;                          // 段内一次 ctor，跨轮复用
            while (ScanStr(&qani.m_name))
            {
                ScanStr(&sub);
                qani.m_layer = getDrawLayer(sub.c_str());
                qani.m_field08 = ScanInt(static_cast<bool*>(0));
                qani.m_field0c = ScanInt(static_cast<bool*>(0));
                qani.m_field10 = ScanInt(static_cast<bool*>(0));
                qani.m_field14 = ScanInt(static_cast<bool*>(0));
                qani.m_flag18 = ScanInt(static_cast<bool*>(0)) != 0;
                script->m_listQuestAnimation.push_back(qani);
            }
            continue;
        }
        if (tok == "[attacked monster info]")
        {
            script->m_vecAttackedMonster.clear();
            int acc = 0;                                    // -0x34 累加权重
            STAttackedMonster mon;                          // -0x5ac（POD，无 ctor）
            while (mon.m_id = ScanInt(&ok), ok == true)     // 0x44da 循环头直存字段
            {
                mon.m_field04 = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_field08 = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_field0c = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_attackWeight = ScanInt(&ok);          // iStack_5a0（先存原始值）
                if (!ok)
                    return false;
                mon.m_attackWeight = acc + mon.m_attackWeight;   // local_38 += w
                acc = mon.m_attackWeight;
                mon.m_field14 = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_field18 = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_vecAttackedMonster.push_back(mon);
            }
            continue;
        }
        if (tok == "[max monster drop exception]")          // 0x089e17xx 区
        {
            script->m_maxMonsterCountException = ScanInt(&ok);  // param_1[0xc9]
            if (!ok)
                return false;                               // 0x4679 fail→false
            continue;
        }
        if (tok == "[max monster count exception]")
        {
            script->m_328 = ScanInt(&ok);                   // +0x328
            if (!ok)
                return false;                               // 0x46cc fail→false
            continue;
        }
        if (tok == "[background effect]")                   // 0x089e1640 区
        {
            script->m_vecBackGroundEffect.clear();
            BackGroundEffect effect;                        // -0x704 段外一次 ctor，跨轮复用
            while (true)
            {
                effect.clear();
                if (!ScanStr(&sub))
                    break;
                if (sub == "[swing]")                       // type=0 分支
                {
                    if (!ScanStr(&sub))
                        break;                              // 0x476a
                    effect.m_layer = getDrawLayer(sub.c_str());
                    effect.m_type = 0;                      // -0x704（layer 后写）
                    effect.m_field08 = ScanInt(static_cast<bool*>(0));
                    effect.m_field0c = ScanInt(static_cast<bool*>(0));
                    effect.m_field10 = ScanInt(static_cast<bool*>(0));
                    effect.m_field14 = ScanInt(static_cast<bool*>(0));
                    effect.m_field18 = ScanInt(static_cast<bool*>(0));
                    {                                       // 可选动画名 → +0x1c string
                        std::string anim;                   // -0x5b4（每轮 ctor/dtor）
                        anim.clear();                       // Ss::clear
                        ScanStr(&anim);
                        if (anim == "")
                            effect.m_animName = "";         // Ss::operator=(PKc)
                        else
                            effect.m_animName = anim;       // Ss::operator=(RKSs)
                    }
                }
                else if (sub == "[dust]")                   // type=1 分支
                {
                    effect.m_type = 1;
                    if (!ScanStr(&sub))
                        break;                              // 0x48d1
                    effect.m_layer = getDrawLayer(sub.c_str());
                    effect.m_field0c = ScanInt(static_cast<bool*>(0));
                    effect.m_field10 = ScanInt(static_cast<bool*>(0));
                    effect.m_field14 = ScanInt(static_cast<bool*>(0));
                }
                script->m_vecBackGroundEffect.push_back(effect);
            }
            continue;
        }
        if (tok == "[block effect]")                        // 0x089e18fc 区
        {
            script->m_vecBlockEffect.clear();
            BackGroundEffectBlock block;                    // -0x5c0 段外一次 ctor
            while (true)
            {
                block.clear();
                if (!ScanStr(&sub))
                    break;
                if (sub == "[swing]")
                    block.m_type = 0;
                else if (sub == "[dust]")
                    block.m_type = 1;                       // ORIG 字面判定
                if (!ScanStr(&sub))
                    break;
                if (sub == "[object]")                      // uStack_5c0=0
                {
                    block.m_mode = 0;
                    block.m_val = ScanInt(static_cast<bool*>(0));
                }
                else if (sub == "[animation]")              // uStack_5c0=1
                {
                    block.m_mode = 1;
                    ScanStr(&sub);
                    block.m_animName = sub;                 // Ss::operator=(RKSs)
                }
                script->m_vecBlockEffect.push_back(block);
            }
            continue;
        }
        if (tok == "[absolute start path]")                 // 0x089e19xx 区
        {
            script->m_startPathDir = 4;
            for (;;)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[left]")        script->m_startPathDir = 0;
                else if (sub == "[right]")  script->m_startPathDir = 1;
                else if (sub == "[up]")     script->m_startPathDir = 2;
                else if (sub == "[down]")   script->m_startPathDir = 3;
            }
            continue;
        }
        if (tok == "[block path]")
        {
            for (int i = 0; i < 4; ++i)
            {
                if (!ScanStr(&sub))
                    break;
                script->m_blockPath[i] =
                    static_cast<char>(sub == "[close]");
            }
            continue;
        }
        if (tok == "[tile option]")
        {
            for (;;)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[upper]")
                    script->m_tileOptionUpper = 1;          // byte(param_1+0x36)
            }
            continue;
        }
        if (tok == "[dialog]")                              // 0x089e1c73 区
        {
            STDialog dlg;                                   // -0x5c8 ctor
            char scanOk;                                    // -0x2b（char 物化）
            scanOk = ScanStr(&dlg.m_name);
            if (scanOk != 1)
                return false;                               // 0x4d2b → dlg dtor false
            {
                std::string when;                           // -0x5cc（每入构造）
                scanOk = ScanStr(&when);
                if (scanOk != 1)
                    return false;                           // 0x4d62
                if (when == "on start")
                {
                    dlg.m_flag[0] = 1;                      // -0x5c4（dlg+4）
                    script->m_vecDialog.push_back(dlg);     // vector(+0xc4)
                }
                else if (when == "on complete")
                {
                    dlg.m_flag[1] = 1;                      // -0x5c3（dlg+5）
                    script->m_vecDialog.push_back(dlg);
                }
                else
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                          0x84f,
                                          "Error on scanning tower dialog. There is no match string %s",
                                          when.c_str());   // 未知串仅记日志，继续
                }
            }
            continue;
        }
        if (tok == "[map dialog]")                          // 0x089e1c61
        {
            ScanStr(&script->m_str34c);                     // 0x4ec1：直存 +0x34c
            continue;
        }
        if (tok == "[tournament monster]")                  // 0x089e1e53 区
        {
            std::vector<TournamentMonster> mons;            // -0x5d8（先 ctor）
            char teamCount = 0;                             // -0x5d9
            teamCount = ScanInt(&ok);                       // mov %al 截断直存
            if (teamCount < 1 || teamCount > 2)             // signed char 双判定
                return false;
            if (!ok)
                return false;                               // 0x4f4d → mons dtor false
            while (true)
            {
                TournamentMonster mon;                      // -0x5e4 每轮 ctor
                mon.m_mobId = ScanInt(&ok);
                if (!ok)
                    break;
                mon.m_rate = ScanInt(&ok);
                if (!ok)
                    return false;
                mons.push_back(mon);
            }
            if (script->m_tournamentMonsters.insert(
                    std::make_pair(teamCount, mons)).second != true)   // make_pair<char&,vector&>
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0x878,
                                      "[tournament monster] map index(%d) is duplicated",
                                      static_cast<int>(teamCount));
                return false;
            }
            continue;
        }
        if (tok == "[tournament start area]")               // 0x089e1f00 区
        {
            TournamentStartPos pos;                         // -0x5f0（先 ctor）
            char t = 0;                                     // -0x2a
            t = ScanInt(&ok);                               // mov %al 直存
            if (t < 1 || t > 2)
                return false;
            if (!ok)
                return false;
            if (t == 1)
            {
                pos.m_x = ScanInt(&ok);
                if (!ok)
                    return false;
                pos.m_y = ScanInt(&ok);
                if (!ok)
                    return false;
                char slot = 0;                              // -0x29
                slot = ScanInt(&ok);
                if (slot < 1 || slot > 2)
                    return false;
                pos.m_slot = slot;
                if (slot == 1)
                    script->m_tournamentStartPos1.push_back(pos);   // vector@0x2d0
                else if (slot == 2)
                    script->m_tournamentStartPos2.push_back(pos);   // vector@0x2dc
            }
            continue;
        }
        if (tok == "[move layered map]")                    // 0x089e2216 区（__lookup_1 表段）
        {
            static std::map<const char*, int, less_TCHAR> __lookup_1;
            static SpinLock __init_lock_1;
            static bool __inited_1 = false;
            std::string line;                               // local_5f8（循环外构造一次）
            bool loop = true;                               // local_5f9
            do
            {
                loop = ScanType(line, true);
                if (!loop)
                    return false;
                int id;
                for (;;)
                {
                    if (!__inited_1)
                    {
                        __init_lock_1.enter();
                        if (__inited_1)
                        {
                            __init_lock_1.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup_1.find(line.c_str());
                    if (it != __lookup_1.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x8b8:                                 // [zpos under]
                {
                    if (__inited_1)
                    {
                        script->m_moveLayeredType = 1;      // movl $1,0x350
                        script->m_zposUnder = ScanInt(&loop);  // -0x354 ← &-0x5f5
                        if (loop != true)
                            return false;
                        goto layered_next;
                    }
                    __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[zpos under]", 0x8b8));
                    /* fall through */
                }
                case 0x8be:                                 // [all dead case passable]
                {
                    if (__inited_1)
                    {
                        script->m_allDeadCasePassable = 1;  // movb $1,0x358
                        if (loop != true)
                            return false;
                        goto layered_next;
                    }
                    __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[all dead case passable]", 0x8be));
                    /* fall through */
                }
                case 0x8c3:                                 // [/move layered map]
                {
                    if (__inited_1)
                        loop = false;
                    else
                        __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[/move layered map]", 0x8c3));
                    break;
                }
                default:
                    break;
                }
                if (!__inited_1)
                {
                    __init_lock_1.leave();
                    __inited_1 = true;
                }
            layered_next:;
            } while (loop);
            continue;
        }
        if (tok == "[static player start pos]")             // 0x089e26a0 区（__lookup_2 表段）
        {
            // ORIG：string / StaticStartPos 每轮 do 体内构造（dtor 同轮尾）；
            // ScanInt 全部走 bool* 版（局部 ok 复用 ScanType 的 flag 槽）。
            static std::map<const char*, int, less_TCHAR> __lookup_2;
            static SpinLock __init_lock_2;
            static bool __inited_2 = false;
            int cont = 0;
            do
            {
                std::string line;                           // local_604（每轮 ctor/dtor）
                StaticStartPos pos;                         // local_614（每轮 ctor）
                bool ok2 = ScanType(line, true);            // local_615
                if (!ok2)
                    return false;
                int id;
                while (true)
                {
                for (;;)
                {
                    if (!__inited_2)
                    {
                        __init_lock_2.enter();
                        if (__inited_2)
                        {
                            __init_lock_2.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup_2.find(line.c_str());
                    if (it != __lookup_2.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x8dc:                                 // [player 1]
                {
                    if (__inited_2)
                    {
                        pos.m_playerIdx = 0;                // movl $0,-0x604
                        goto static_after;
                    }
                    __lookup_2.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 1]", 0x8dc));
                    /* fall through */
                }
                case 0x8df:                                 // [player 2]
                {
                    if (__inited_2)
                    {
                        pos.m_playerIdx = 1;
                        goto static_after;
                    }
                    __lookup_2.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 2]", 0x8df));
                    /* fall through */
                }
                case 0x8e2:                                 // [player 3]
                {
                    if (__inited_2)
                    {
                        pos.m_playerIdx = 2;
                        goto static_after;
                    }
                    __lookup_2.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 3]", 0x8e2));
                    /* fall through */
                }
                case 0x8e5:                                 // [player 4]
                {
                    if (__inited_2)
                    {
                        pos.m_playerIdx = 3;
                        goto static_after;
                    }
                    __lookup_2.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 4]", 0x8e5));
                    /* fall through */
                }
                case 0x8e8:                                 // [/static player start pos]
                {
                    if (__inited_2)
                        ok2 = false;                        // movb $0,-0x611
                    else
                        __lookup_2.insert(std::map<const char*, int, less_TCHAR>::value_type("[/static player start pos]", 0x8e8));
                    break;
                }
                default:
                    break;
                }
                if (!__inited_2)
                {
                    __init_lock_2.leave();
                    __inited_2 = true;
                    continue;                               // 重入本轮 find（0x26dd）
                }
            static_after:
                break;
                }
                if (!ok2)
                    break;                                  // iVar6 = 1 → 退出 do
                pos.m_x = ScanInt(&ok2);
                if (!ok2)
                    return false;
                pos.m_y = ScanInt(&ok2);
                if (!ok2)
                    return false;
                pos.m_z = ScanInt(&ok2);
                if (!ok2)
                    return false;
                script->m_vecStaticStartPos.push_back(pos);  // vector(+0x2e8)
                cont = 2;
            } while (cont != 1);
            continue;
        }
        if (tok == "[limit player move area]")              // 0x089e2c8c 区（__lookup_3 表段）
        {
            // ORIG：每轮 string + MapArea（ctor 后额外 clear()）；未知键 idx 保持 0
            // 且仍读 4 int（id=-1 同路），find==end 才 insert。
            static std::map<const char*, int, less_TCHAR> __lookup_3;
            static SpinLock __init_lock_3;
            static bool __inited_3 = false;
            int cont = 0;
            do
            {
                std::string line;                           // local_620（每轮 ctor/dtor）
                int playerIdx = 0;                          // local_624
                MapArea area;                               // local_640：ctor + clear
                area.clear();
                bool ok2 = ScanType(line, true);            // local_641
                if (!ok2)
                    return false;
                int id;
                while (true)
                {
                for (;;)
                {
                    if (!__inited_3)
                    {
                        __init_lock_3.enter();
                        if (__inited_3)
                        {
                            __init_lock_3.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup_3.find(line.c_str());
                    if (it != __lookup_3.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x911:                                 // [player 1]
                {
                    if (__inited_3)
                    {
                        playerIdx = 0;                      // movl $0,-0x620
                        goto limit_after;
                    }
                    __lookup_3.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 1]", 0x911));
                    /* fall through */
                }
                case 0x914:                                 // [player 2]
                {
                    if (__inited_3)
                    {
                        playerIdx = 1;
                        goto limit_after;
                    }
                    __lookup_3.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 2]", 0x914));
                    /* fall through */
                }
                case 0x917:                                 // [player 3]
                {
                    if (__inited_3)
                    {
                        playerIdx = 2;
                        goto limit_after;
                    }
                    __lookup_3.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 3]", 0x917));
                    /* fall through */
                }
                case 0x91a:                                 // [player 4]
                {
                    if (__inited_3)
                    {
                        playerIdx = 3;
                        goto limit_after;
                    }
                    __lookup_3.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 4]", 0x91a));
                    /* fall through */
                }
                case 0x91d:                                 // [/limit player move area]
                {
                    if (__inited_3)
                        ok2 = false;
                    else
                        __lookup_3.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit player move area]", 0x91d));
                    break;
                }
                default:
                    break;
                }
                if (!__inited_3)
                {
                    __init_lock_3.leave();
                    __inited_3 = true;
                    continue;                               // 重入本轮 find（0x2ce1）
                }
            limit_after:
                break;
                }
                if (!ok2)
                    break;                                  // iVar6 = 1 → 退出 do
                area.m_x = ScanInt(&ok2);
                if (!ok2)
                    return false;
                area.m_y = ScanInt(&ok2);
                if (!ok2)
                    return false;
                area.m_w = ScanInt(&ok2);
                if (!ok2)
                    return false;
                area.m_h = ScanInt(&ok2);
                if (!ok2)
                    return false;
                std::map<int, MapArea>::iterator it =
                    script->m_limitPlayerMoveAreas.find(playerIdx);
                if (it == script->m_limitPlayerMoveAreas.end())
                    script->m_limitPlayerMoveAreas.insert(
                        std::make_pair(playerIdx, area));   // make_pair + insert
                cont = 2;
            } while (cont != 1);
            continue;
        }
        if (tok == "[customized screen edge]")              // 0x089e3404 区（__lookup_4 表段）
        {
            // ORIG：结束键以 eq 前置判定；ScanType 返回 false（自然耗尽）→ 整函数失败。
            static std::map<const char*, int, less_TCHAR> __lookup_4;
            static SpinLock __init_lock_4;
            static bool __inited_4 = false;
            std::string line;                               // local_64c（循环外一次）
            bool done = false;
            while (ScanType(line, true))
            {
                if (line == "[/customized screen edge]")
                {
                    done = true;
                    break;
                }
                int id;
                while (true)
                {
                for (;;)
                {
                    if (!__inited_4)
                    {
                        __init_lock_4.enter();
                        if (__inited_4)
                        {
                            __init_lock_4.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup_4.find(line.c_str());
                    if (it != __lookup_4.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x946:                                 // [left]
                {
                    if (__inited_4)
                    {
                        int leftV;                          // -0x650（ORIG 无初始化）
                        if (ScanInt(&leftV) != 1)
                            return false;
                        script->m_screenEdgeLeft = leftV;   // +0x35c
                        goto edge_after;
                    }
                    __lookup_4.insert(std::map<const char*, int, less_TCHAR>::value_type("[left]", 0x946));
                    /* fall through */
                }
                case 0x94f:                                 // [right]
                {
                    if (__inited_4)
                    {
                        int rightV;                         // -0x654
                        if (ScanInt(&rightV) != 1)
                            return false;
                        script->m_screenEdgeRight = rightV; // +0x360
                        goto edge_after;
                    }
                    __lookup_4.insert(std::map<const char*, int, less_TCHAR>::value_type("[right]", 0x94f));
                    /* fall through */
                }
                case 0x958:                                 // [top]
                {
                    if (__inited_4)
                    {
                        int topV;                           // -0x658
                        if (ScanInt(&topV) != 1)
                            return false;
                        script->m_screenEdgeTop = topV;     // +0x364
                        goto edge_after;
                    }
                    __lookup_4.insert(std::map<const char*, int, less_TCHAR>::value_type("[top]", 0x958));
                    /* fall through */
                }
                case 0x961:                                 // [bottom]
                {
                    if (__inited_4)
                    {
                        int bottomV;                        // -0x65c
                        if (ScanInt(&bottomV) != 1)
                            return false;
                        script->m_screenEdgeBottom = bottomV;  // +0x368
                        goto edge_after;
                    }
                    __lookup_4.insert(std::map<const char*, int, less_TCHAR>::value_type("[bottom]", 0x961));
                    break;
                }
                default:
                    break;
                }
                if (!__inited_4)
                {
                    __init_lock_4.leave();
                    __inited_4 = true;
                    continue;                               // 重入本轮 find（0x644b）
                }
            edge_after:
                break;
                }
            }
            if (!done)
                return false;
            continue;
        }
        if (tok == "[block use active skill]")              // else 链最外层
        {
            script->m_blockUseActiveSkill = 1;              // byte @0xdb
            continue;
        }
        if (tok == "[block use stackable item]")
        {
            script->m_blockUseStackableItem = 1;            // byte 0x36d
            continue;
        }
        if (tok == "[fix champion]")
        {
            script->m_fixChampion = 1;                      // byte 0x36e
            continue;
        }
        if (tok == "[basic action]")
        {
            if (ScanStr(&script->m_str370) != 1)            // 0x6a72：直存 +0x370
                return false;
            continue;
        }
        if (tok == "[visible minimap]")
        {
            if (ScanInt(&okInt) != 1)
                return false;
            script->m_visibleMinimap = okInt;               // +0x374
            continue;
        }
        if (tok == "[visible town minimap]")
        {
            if (ScanInt(&okInt) != 1)
                return false;
            script->m_visibleTownMinimap = okInt;           // +0x378
            continue;
        }
        // 未识别 token：继续扫描（ORIG 无 default 分支动作）
    }

    // ---- 收尾（code_r0x089e3a93）----
    if (!pvpPracticeRead)
    {
        // words: [0x84..0x87]=[0x76..0x79]、[0x8b..0x8e]=[0x7d..0x80]
        // 即 MapArea[3].xywh = MapArea[1].xywh、MapArea[4].xywh = MapArea[2].xywh
        script->m_pvpStartArea[3].m_x = script->m_pvpStartArea[1].m_x;
        script->m_pvpStartArea[3].m_y = script->m_pvpStartArea[1].m_y;
        script->m_pvpStartArea[3].m_w = script->m_pvpStartArea[1].m_w;
        script->m_pvpStartArea[3].m_h = script->m_pvpStartArea[1].m_h;
        script->m_pvpStartArea[4].m_x = script->m_pvpStartArea[2].m_x;
        script->m_pvpStartArea[4].m_y = script->m_pvpStartArea[2].m_y;
        script->m_pvpStartArea[4].m_w = script->m_pvpStartArea[2].m_w;
        script->m_pvpStartArea[4].m_h = script->m_pvpStartArea[2].m_h;
    }
    script->m_index = g_stMapScriptFileList.FindIndexByFullScan(path);  // *param_1
    return true;
}

// ===========================================================================
// ExportMapScript —— ORIG T 0x089e3c55（全段落输出转写；ORIG 以 fopen("wb") +
// fprintf 生成中间文本，收尾 convertBinaryToText 转最终脚本）
// ===========================================================================

// [b28d-solo-STMapScript] _Z15ExportMapScriptP11STMapScriptPKc：p167 CLEAR 378/378
//（§6.2 三件套②归零）；超大函数 verdict DIFF 按 §6.2 口径收：残差＝N82 帧/槽号 + N89
// libstdc++ 全内联（ORIG 大量 out-of-line 模板实例），无真语义缺口。
bool ExportMapScript(STMapScript* script, const char* path)
{
    static char szSaveFileName[0x100];                 // 函数级静态 szSaveFileName
    getScriptFileFullPath(szSaveFileName, 0x100,
                          g_szMapScriptBaseDirectory, path);
    // ORIG 槽序实证（-0x94 file / -0x8c tileIdx / -0x85..-0x88 bool4）：
    // 声明位置在 file 初始化之前（实测最优形；仅声明、不初始化，不发指令）。
    bool chL, chR, chT, chB;                           // -0x88..-0x85（screen edge）
    unsigned int tileIdx;                              // -0x8c：tile/exttile 共用循环变量
    FILE* file = fopen(szSaveFileName, "wb");
    if (!file)
        return false;

    STMapScript defaults;                       // auStack_48c：缺省值对照

    if (script->m_str1b0.length() != 0)         // +0x1b0 非空才输出 [map name]
        fprintf(file, "[map name] `%s`\n", script->m_str1b0.c_str());
    fprintf(file, "[player number] %d %d\n",
            script->m_playerCountMin, script->m_playerCountMax);
    fwrite("[pvp start area]\n", 1, 17, file);
    for (int i = 0; i < 3; ++i)                 // words 0x6f..0x72 步进 7
        fprintf(file, "%d %d %d %d\n",
                script->m_pvpStartArea[i].m_x,
                script->m_pvpStartArea[i].m_y,
                script->m_pvpStartArea[i].m_w,
                script->m_pvpStartArea[i].m_h);
    {                                           // practice 区（3/4）任一非零才输出
        bool any = false;
        for (int i = 3; i < 5; ++i)
            if (script->m_pvpStartArea[i].m_x || script->m_pvpStartArea[i].m_y ||
                script->m_pvpStartArea[i].m_w || script->m_pvpStartArea[i].m_h)
                any = true;
        if (any)
        {
            fwrite("[pvp practice start area]\n", 1, 26, file);
            for (int i = 3; i < 5; ++i)
                fprintf(file, "%d %d %d %d\n",
                        script->m_pvpStartArea[i].m_x,
                        script->m_pvpStartArea[i].m_y,
                        script->m_pvpStartArea[i].m_w,
                        script->m_pvpStartArea[i].m_h);
        }
    }
    // ORIG 0x89e3fc0 展开序：cmp $1 je; cmp $2 je; test(=case0); jmp end
    // ⇒ switch、case 书写序 1,2,0（case0 体空）。
    switch (script->m_24c)
    {
    case 1:
        fwrite("[shadow type] `[mirror]`\n", 1, 25, file);
        break;
    case 2:
        fwrite("[shadow type] `[more darker]`\n", 1, 30, file);
        break;
    case 0:
        break;
    }
    if (!script->m_list4.empty())
    {
        fwrite("[dungeon]\n", 1, 10, file);
        for (std::list<int>::iterator it = script->m_list4.begin();
             it != script->m_list4.end(); it++)
            fprintf(file, "%d\n", *it);
        fwrite("[/dungeon]\n", 1, 11, file);
    }
    if (script->m_c == 0)
        fwrite("[type] `[normal]`\n", 1, 18, file);
    else if (script->m_c == 1)
        fwrite("[type] `[boss]`\n", 1, 16, file);
    else if (script->m_c == 2)
        fwrite("[type] `[dummy]`\n", 1, 17, file);
    if (!script->m_recoveryObjects.empty())     // +0x10 [greed]
    {
        std::string grid;
        for (unsigned int i = 0; i < script->m_recoveryObjects.size(); ++i)
        {
            grid += g_gridSymbols[script->m_recoveryObjects[i].first];
            grid += " ";
            grid += g_gridSymbols[script->m_recoveryObjects[i].second];
            if (script->m_recoveryObjects.size() - 1 != i)
                grid += "\n ";
        }
        fprintf(file, "[greed]\n`%s`\n", grid.c_str());
    }
    if (!script->m_vecTile.empty())             // +0x70 [tile]
    {
        fwrite("[tile]\n", 1, 7, file);
        for (tileIdx = 0; tileIdx < script->m_vecTile.size(); ++tileIdx)
            fprintf(file, "`%s`\n", script->m_vecTile[tileIdx].c_str());
        fwrite("[/tile]\n", 1, 8, file);
    }
    if (!script->m_vecExtendedTile.empty())     // +0x7c [extended tile]
    {
        fwrite("[extended tile]\n", 1, 16, file);
        for (tileIdx = 0; tileIdx < script->m_vecExtendedTile.size(); ++tileIdx)
            fprintf(file, "`%s`\n", script->m_vecExtendedTile[tileIdx].c_str());
        fwrite("[/extended tile]\n", 1, 17, file);
    }
    if (script->m_element != 4)                 // +0x6c != Clear 值
        fprintf(file, "[elemental property] `%s`\n",
                getElementName(static_cast<ENUM_ELEMENT>(script->m_element)));
    if (script->m_sightScroll[0] != defaults.m_sightScroll[0])
        fprintf(file, "[far sight scroll] %d\n", script->m_sightScroll[0]);
    if (script->m_sightScroll[1] != defaults.m_sightScroll[1])
        fprintf(file, "[middle sight scroll] %d\n", script->m_sightScroll[1]);
    if (script->m_sightScroll[2] != defaults.m_sightScroll[2])
        fprintf(file, "[near sight scroll] %d\n", script->m_sightScroll[2]);
    if (!script->m_vecBackAni.empty())          // +0xa0 [background animation]
    {
        fwrite("[background animation]\n", 1, 23, file);
        for (unsigned int i = 0; i < script->m_vecBackAni.size(); ++i)
        {
            const char* ord = script->m_vecBackAni[i].m_orderUpper
                                  ? "[upper]" : "[below]";
            fprintf(file, "`%s` `%s` `%s`\n",
                    script->m_vecBackAni[i].m_filename.c_str(),
                    getDrawLayerName(
                        static_cast<ENUM_DRAWLAYER>(script->m_vecBackAni[i].m_layer)),
                    ord);
        }
        fwrite("[/background animation]\n", 1, 24, file);
    }
    if (!script->m_vecTownMovableArea.empty())  // +0xdc [town movable area]
    {
        fwrite("[town movable area]\n", 1, 20, file);
        for (unsigned int i = 0; i < script->m_vecTownMovableArea.size(); ++i)
            fprintf(file, "%d %d %d %d %d %d\n",
                    script->m_vecTownMovableArea[i].m_x,
                    script->m_vecTownMovableArea[i].m_y,
                    script->m_vecTownMovableArea[i].m_w,
                    script->m_vecTownMovableArea[i].m_h,
                    script->m_vecTownMovableArea[i].m_field1c,
                    script->m_vecTownMovableArea[i].m_field20);
        fwrite("[/town movable area]\n", 1, 21, file);
    }
    if (script->m_vecVirtualMovableArea.empty())    // +0xe8：空 → [pathgate pos]
    {
        fwrite("[pathgate pos]\n", 1, 15, file);
        for (int i = 0; i < 4; ++i)                 // words 0x20+i*8 / 0x104+i*8
            fprintf(file, "%d %d\n",
                    script->m_pathgatePos[i * 2],
                    script->m_pathgatePos[i * 2 + 1]);
    }
    else                                        // 非空 → [virtual movable area]
    {
        fwrite("[virtual movable area]\n", 1, 23, file);
        for (unsigned int i = 0; i < script->m_vecVirtualMovableArea.size(); ++i)
            fprintf(file, "%d %d %d %d\n",
                    script->m_vecVirtualMovableArea[i].m_x,
                    script->m_vecVirtualMovableArea[i].m_y,
                    script->m_vecVirtualMovableArea[i].m_w,
                    script->m_vecVirtualMovableArea[i].m_h);
        fwrite("[/virtual movable area]\n", 1, 24, file);
    }
    if (!script->m_vecGuildAgitEntrance.empty())    // +0xf4
    {
        fwrite("[guild agit entrance info]\n", 1, 27, file);
        for (unsigned int i = 0; i < script->m_vecGuildAgitEntrance.size(); ++i)
            fprintf(file, "%d %d %d %d %d %d\n",
                    script->m_vecGuildAgitEntrance[i].m_x,
                    script->m_vecGuildAgitEntrance[i].m_y,
                    script->m_vecGuildAgitEntrance[i].m_w,
                    script->m_vecGuildAgitEntrance[i].m_h,
                    script->m_vecGuildAgitEntrance[i].m_field1c,
                    script->m_vecGuildAgitEntrance[i].m_field20);
        fwrite("[/guild agit entrance info]\n", 1, 28, file);
    }
    {                                           // MapArea@0x140 四字段任一非零
        if (script->m_dungeonStartArea.m_x || script->m_dungeonStartArea.m_y ||
            script->m_dungeonStartArea.m_w || script->m_dungeonStartArea.m_h)
            fprintf(file, "[dungeon start area] %d %d %d %d\n",
                    script->m_dungeonStartArea.m_x,
                    script->m_dungeonStartArea.m_y,
                    script->m_dungeonStartArea.m_w,
                    script->m_dungeonStartArea.m_h);
        if (script->m_summonStartArea.m_x || script->m_summonStartArea.m_y ||
            script->m_summonStartArea.m_w || script->m_summonStartArea.m_h)
            fprintf(file, "[summon start area] %d %d %d %d\n",
                    script->m_summonStartArea.m_x,
                    script->m_summonStartArea.m_y,
                    script->m_summonStartArea.m_w,
                    script->m_summonStartArea.m_h);
    }
    if (!script->m_vecSound.empty())            // +0x194 [sound]
    {
        fwrite("[sound]\n", 1, 8, file);
        for (unsigned int i = 0; i < script->m_vecSound.size(); ++i)
            fprintf(file, "`%s`\n", script->m_vecSound[i].c_str());
        fwrite("[/sound]\n", 1, 9, file);
    }
    if (!script->m_listAnimation.empty())       // +0x1c [animation]
    {
        fwrite("[animation]\n", 1, 12, file);
        std::string layerName;
        for (std::list<STAssignAnimation>::iterator it =
                 script->m_listAnimation.begin();
             it != script->m_listAnimation.end(); it++)
        {
            layerName = getDrawLayerName(
                static_cast<ENUM_DRAWLAYER>((*it).m_layer));
            fprintf(file, "`%s` `%s` %d %d %d\n",
                    (*it).m_name.c_str(), layerName.c_str(),
                    (*it).m_field08, (*it).m_field0c, (*it).m_field10);
        }
        fwrite("[/animation]\n", 1, 13, file);
    }
    if (!script->m_listQuestAnimation.empty())  // +0x24 [quest animation]
    {
        fwrite("[quest animation]\n", 1, 18, file);
        std::string layerName;
        for (std::list<STQuestAnimation>::iterator it =
                 script->m_listQuestAnimation.begin();
             it != script->m_listQuestAnimation.end(); it++)
        {
            layerName = getDrawLayerName(
                static_cast<ENUM_DRAWLAYER>((*it).m_layer));
            fprintf(file, "`%s` `%s` %d %d %d %d\n",
                    (*it).m_name.c_str(), layerName.c_str(),
                    (*it).m_field08, (*it).m_field0c, (*it).m_field10,
                    (*it).m_field14);
        }
        fwrite("[/quest animation]\n", 1, 19, file);
    }
    if (!script->m_listPassiveObject.empty())   // +0x2c [passive object]
    {
        fwrite("[passive object]\n", 1, 17, file);
        for (std::list<STAssignPassiveObject>::iterator it =
                 script->m_listPassiveObject.begin();
             it != script->m_listPassiveObject.end(); it++)
            fprintf(file, "%d %d %d %d\n",
                    (*it).m_0, (*it).m_4, (*it).m_8, (*it).m_c);
        fwrite("[/passive object]\n", 1, 18, file);
    }
    if (!script->m_vecMonster.empty())          // +0x34 [monster]
    {
        fwrite("[monster]\n", 1, 10, file);
        bool isChampion = false;                // cStack_5d
        for (std::vector<STAssignMonster>::iterator it =
                 script->m_vecMonster.begin();
             it != script->m_vecMonster.end(); it++)
        {
            isChampion = false;
            std::string typeTok;                // local_fc（每轮 ctor/dtor）
            std::string roleTok;                // local_100
            if ((*it).m_20 == 0)      typeTok = "`[fixed]`";
            else if ((*it).m_20 == 1) typeTok = "`[half]`";
            else if ((*it).m_20 == 2) typeTok = "`[ratio]`";
            if ((*it).m_24 == 0)      roleTok = "`[normal]`";
            else if ((*it).m_24 == 1) { roleTok = "`[champion]`"; isChampion = true; }
            else if ((*it).m_24 == 3) roleTok = "`[boss]`";
            fprintf(file,
                    "%d %d %d %d %d %d %d %d %s %s%c",
                    (*it).m_0, static_cast<int>((*it).m_4 != 0),
                    (*it).m_8, (*it).m_c, (*it).m_10, (*it).m_14,
                    (*it).m_pair18.m_pair.m_lo,
                    (*it).m_pair18.m_pair.m_hi,
                    typeTok.c_str(), roleTok.c_str(),
                    isChampion ? ' ' : '\n');
            if (isChampion)
                WriteChampionFeatures(file, *it);
        }
        fwrite("[/monster]\n", 1, 11, file);
    }
    if (script->m_vecMonster.size() != 0)       // [monster specific AI]（ORIG 089e565b..）
    {
        fwrite("[monster specific AI]\n", 1, 0x16, file);
        for (unsigned int i = 0; i < script->m_vecMonster.size(); ++i)
        {
            if (script->m_vecMonster[i].m_34 == 0)
                fwrite("`[normal]`\n", 1, 0xb, file);
            else if (script->m_vecMonster[i].m_34 == 1)
                fprintf(file, "%s %d %d\n", "`[zone defence]`",
                        script->m_vecMonster[i].m_pair38.m_arr[0],
                        script->m_vecMonster[i].m_pair38.m_arr[1]);
            else if (script->m_vecMonster[i].m_34 == 2)
            {
                int wayCount = static_cast<int>(
                    script->m_vecMonster[i].m_vec40.size());
                fwrite("`[roaming]`", 1, 0xb, file);
                fprintf(file, " %d", wayCount);
                for (int k = 0; k < wayCount; ++k)
                    fprintf(file, " %d", script->m_vecMonster[i].m_vec40[k].first);
                for (int k = 0; k < wayCount; ++k)
                    fprintf(file, " %d", script->m_vecMonster[i].m_vec40[k].second);
                fputc(10, file);
            }
        }
        fwrite("[/monster specific AI]\n", 1, 0x17, file);
    }
    if (script->m_vecMonster.size() != 0)       // m_4c 任一非默认(100) → team 表
    {
        bool anyTeam = false;
        for (unsigned int i = 0; i < script->m_vecMonster.size(); ++i)
            if (script->m_vecMonster[i].m_4c != 100)
                anyTeam = true;
        if (anyTeam)
        {
            fwrite("[monster team]\n", 1, 15, file);
            for (unsigned int i = 0; i < script->m_vecMonster.size(); ++i)
                fprintf(file, "%d\n", script->m_vecMonster[i].m_4c);
            fwrite("[/monster team]\n", 1, 16, file);
        }
    }
    if (script->m_championRate != defaults.m_championRate)      // +0x4c
        fprintf(file, "[champion] %d\n", script->m_championRate);
    if (!script->m_listItem.empty())            // +0x50 [item]
    {
        fwrite("[item]\n", 1, 7, file);
        for (std::list<STAssignItem>::iterator it =
                 script->m_listItem.begin();
             it != script->m_listItem.end(); it++)
        {
            fprintf(file, "%d %d %d %d %d %d %d %d %d %d",
                    (*it).m_field00, (*it).m_pair04.a[0], (*it).m_pair04.a[1],
                    (*it).m_field0c, (*it).m_field10, (*it).m_field14,
                    (*it).m_field18, (*it).m_field1c, (*it).m_field20,
                    static_cast<int>((*it).m_generateInfos.size()));
            if ((*it).m_generateInfos.size() != 0)  // ORIG：仅非空才 fputc(10)
                fputc(10, file);
            for (unsigned int k = 0; k < (*it).m_generateInfos.size(); ++k)
                fprintf(file, "%d %d\n",
                        (*it).m_generateInfos[k].m_field00,
                        (*it).m_generateInfos[k].m_field04);
            fputc(10, file);
        }
        fwrite("[/item]\n", 1, 8, file);
    }
    if (!script->m_listSpecialPassive.empty())  // +0x58 [special passive object]
    {
        fwrite("[special passive object]\n", 1, 25, file);
        for (std::list<STAssignSpecialPassiveObj>::iterator it =
                 script->m_listSpecialPassive.begin();
             it != script->m_listSpecialPassive.end(); it++)
        {
            STAssignSpecialPassiveObj* p = &*it;            // 单次 de，后续全用指针
            if (p->m_vec10.size() == 0)
                fprintf(file, "%d %d %d %d %d\n",
                        p->m_0, p->m_4, p->m_8, p->m_c,
                        static_cast<int>(p->m_vec10.size()));
            else
            {
                fprintf(file, "%d %d %d %d\n",
                        p->m_0, p->m_4, p->m_8, p->m_c);
                fprintf(file, "%d\n", static_cast<int>(p->m_vec10.size()));
            }
            for (unsigned int k = 0; k < p->m_vec10.size(); ++k)
            {
                int* act = reinterpret_cast<int*>(&p->m_vec10[k]);
                for (int j = 0; j < 6; ++j)
                {
                    if (j == 0)
                    {
                        if (*act == 0)
                            fwrite("`[monster]` ", 1, 0xc, file);
                        else if (*act == 1)
                            fwrite("`[trap]` ", 1, 9, file);
                        else if (*act == 2)
                            fwrite("`[item]` ", 1, 9, file);
                        else if (*act == 3)
                            fwrite("`[quest]` ", 1, 10, file);
                    }
                    else
                        fprintf(file, "%d ", act[j]);
                }
                fputc(10, file);
            }
        }
        fwrite("[/special passive object]\n", 1, 26, file);
    }
    if (!script->m_vecNpc.empty())              // +0x60 [NPC]
    {
        fwrite("[NPC]\n", 1, 6, file);
        for (std::vector<AssignNpc>::iterator it = script->m_vecNpc.begin();
             it != script->m_vecNpc.end(); it++)
        {
            std::string dirTok;                             // local_110（每轮 ctor/dtor）
            if ((*it).m_dir == 0)
                dirTok = "`[left]`";
            else
                dirTok = "`[right]`";
            fprintf(file, "%d %s %d %d %d\n",
                    (*it).m_npcIdx, dirTok.c_str(),
                    (*it).m_x, (*it).m_y, (*it).m_z);
        }
        fwrite("[/NPC]\n", 1, 7, file);
    }
    if (script->m_vecMonsterSpawnPos.size() != 0) // +0x250 [monster spawn pos]
    {                                           // 块序=ORIG 源序（0x89e624x spawn 先、
        fwrite("[monster spawn pos]\n", 1, 20, file);   //  0x89e637a ai 后；loop var 槽
        fprintf(file, "%d\n", static_cast<int>(script->m_vecMonsterSpawnPos.size()));
        for (unsigned int i = 0; i < script->m_vecMonsterSpawnPos.size(); ++i)
            fprintf(file, "%d %d %d\n",
                    script->m_vecMonsterSpawnPos[i].m_field00,
                    script->m_vecMonsterSpawnPos[i].m_field04,
                    script->m_vecMonsterSpawnPos[i].m_field08);
        fwrite("[/monster spawn pos]\n", 1, 21, file);
    }
    else if (script->m_vecAICharacter.size() != 0)  // +0x25c [ai character]
    {
        fwrite("[ai character]\n", 1, 15, file);
        for (unsigned int i = 0; i < script->m_vecAICharacter.size(); ++i)
        {
            fprintf(file, "%d %d %d %d ",
                    script->m_vecAICharacter[i].m_type,
                    script->m_vecAICharacter[i].m_field04,
                    script->m_vecAICharacter[i].m_field08,
                    script->m_vecAICharacter[i].m_field0c);
            if (script->m_vecAICharacter[i].m_camp == 0)
                fwrite("`[character]`", 1, 13, file);
            else if (script->m_vecAICharacter[i].m_camp == 100)
                fwrite("`[monster]`", 1, 11, file);
            else if (script->m_vecAICharacter[i].m_camp == 200)
                fwrite("`[neutral]`", 1, 11, file);
            fputc(' ', file);
            if (script->m_vecAICharacter[i].m_roleType == 5)
                fwrite("`[normal]`", 1, 10, file);
            else if (script->m_vecAICharacter[i].m_roleType == 6)
                fwrite("`[champion]`", 1, 12, file);
            else if (script->m_vecAICharacter[i].m_roleType == 7)
                fwrite("`[super champion]`", 1, 18, file);
            else if (script->m_vecAICharacter[i].m_roleType == 8)
                fwrite("`[boss]`", 1, 8, file);
            else
                fwrite("`[normal]`", 1, 10, file);
            fputc(' ', file);
            fprintf(file, "%d %d",
                    static_cast<int>(script->m_vecAICharacter[i].m_posX),
                    static_cast<int>(script->m_vecAICharacter[i].m_posY));
            fputc(10, file);
        }
        fwrite("[/ai character]\n", 1, 16, file);
    }
    if (!script->m_mapLoadingImagePath.empty()) // +0x26c（ORIG：fprintf 无 \n，089e67b4）
        fprintf(file, "[map loading image path] %s",
                script->m_mapLoadingImagePath.c_str());
    // ORIG 0x89e67d5 `call empty; xor $1; test; je`：非空分支为 fall-through
    // （[event monster position] 就地），空分支 je 至远端（[move layered map]）。
    // 源形状必须是 `if (!empty()) {monster} else {layered}` 才能复现块序。
    if (!script->m_vecEventMonsterPos.empty())  // +0x278 非空 → [event monster position]
    {
        fwrite("[event monster position]\n", 1, 25, file);
        for (unsigned int i = 0; i < script->m_vecEventMonsterPos.size(); ++i)
        {
            STRivalMapData* p = &script->m_vecEventMonsterPos[i];  // 单次 ix
            fprintf(file, "%d %d %d\n", p->m_field00, p->m_field04, p->m_field08);
        }
        fwrite("[/event monster position]\n", 1, 26, file);
    }
    else                                        // 空 → [move layered map]
    {
        if (script->m_moveLayeredType != 4)
        {
            fwrite("[move layered map]\n", 1, 19, file);
            if (script->m_allDeadCasePassable)
                fwrite("\t[all dead case passable]\n", 1, 26, file);
            if (script->m_moveLayeredType == 1)
                fprintf(file, "\t[zpos under] %d\n", script->m_zposUnder);
            fwrite("[/move layered map]\n", 1, 20, file);
        }
    }
    if (!script->m_vecStaticStartPos.empty())   // +0x2e8
    {
        fwrite("[static player start pos]\n", 1, 26, file);
        for (int i = 0; i < 4; ++i)
        {
            if (static_cast<unsigned int>(i) < script->m_vecStaticStartPos.size())
                fprintf(file, "\t[player %d] %d %d %d\n",
                        script->m_vecStaticStartPos[i].m_playerIdx + 1,
                        script->m_vecStaticStartPos[i].m_x,
                        script->m_vecStaticStartPos[i].m_y,
                        script->m_vecStaticStartPos[i].m_z);
        }
        fwrite("[/static player start pos]\n", 1, 27, file);
    }
    {                                           // screen edge（对照 defaults）
        chL = script->m_screenEdgeLeft != defaults.m_screenEdgeLeft;
        chR = script->m_screenEdgeRight != defaults.m_screenEdgeRight;
        chT = script->m_screenEdgeTop != defaults.m_screenEdgeTop;
        chB = script->m_screenEdgeBottom != defaults.m_screenEdgeBottom;
        if (chL || chR || chT || chB)
        {
            fwrite("[customized screen edge]\n", 1, 25, file);
            if (chL) fprintf(file, "[left] %d\n", script->m_screenEdgeLeft);
            if (chR) fprintf(file, "[right] %d\n", script->m_screenEdgeRight);
            if (chT) fprintf(file, "[top] %d\n", script->m_screenEdgeTop);
            if (chB) fprintf(file, "[bottom] %d\n", script->m_screenEdgeBottom);
            fwrite("[/customized screen edge]\n", 1, 26, file);
        }
    }
    if (script->m_blockUseActiveSkill)
        fwrite("[block use active skill]\n", 1, 25, file);
    if (script->m_blockUseStackableItem)
        fwrite("[block use stackable item]\n", 1, 27, file);

    fclose(file);                               // func_0x0807dea0
    convertBinaryToText(szSaveFileName);
    return true;
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
void OutputErrorMessage(const char* msg)
{
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0xa26, msg);
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
bool checkValidIndex(int index, STScriptFileList& list)   // [b28d-Main·P292] int→bool（ORIG 调用点 `call; xor $0x1,%eax`）
{
    if (list.GetFileName(index) == 0)
        return 0;
    return 1;
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
bool procSpecialPassiveObjectVerifyError(SPOBJ_ACTION_DATAS& action,
                                         const char* path,
                                         STScriptFileList& monsterList,
                                         STScriptFileList& trapList,
                                         std::list<STAssignItem>& assignItems,
                                         STAssignSpecialPassiveObj& owner,
                                         int fileIdx)
{
    bool hasError = false;                     // -0x35
    std::string tok;                           // -0xd4
    std::vector<int> heapVec;                  // -0xd0
    // P167/P234：ORIG 选 priority_queue(less&, vector&&)（MISSING C1ERKS4_OS2_），
    // heapVec 此后不再使用 ⇒ 用 static_cast<T&&> 选中 move 重载（零 out-of-line call）。
    std::priority_queue<int> groupQueue(std::less<int>(),
                                        static_cast<std::vector<int>&&>(heapVec));  // -0xe4
    int fieldIdx = 0;                          // -0x2c
    int slotIdx = 0;                           // -0x28
    int curTop = 0;                            // -0x34
    int lastTop = 0;                           // -0x30

    for (int j = 0; j <= 5; ++j)               // -0x24
    {
        if (j == 0)
        {
            if (!ScanStr(&tok))
                continue;
            if (tok == "[monster]")            // 0x8e1ad7b
            {
                action.m_type = 0;
                continue;
            }
            if (tok == "[trap]")               // 0x8e1c0de
            {
                action.m_type = 1;
                continue;
            }
            if (tok == "[item]")               // 0x8e1c0e5
            {
                action.m_type = 2;
                continue;
            }
            if (tok == "[quest]")              // 0x8e1c0ec
            {
                action.m_type = 3;
                continue;
            }
            // P222：ORIG 用正极性 `tok == "[hellparty]"`（ORIG 5 处 eq 之一）
            if (tok == "[hellparty]")          // 0x8e1c0f4
            {
            // ---- [hellparty] 分支（同 procSpecialPassiveObject，错误记 hasError）----
            std::map<unsigned short, STHellPartyGroupOrder> localOrders;  // -0x100
            for (;;)
            {
                bool ok = false;               // -0xe5
                int tvIdx = ScanInt(&ok);      // -0x104
                if (!ok)
                    break;
                groupQueue.push(tvIdx);

                stHellParty_t* hellParty =
                    reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
                std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                    hellParty->m_groupMap.find(static_cast<unsigned short>(tvIdx));

                int rate = ScanInt(&ok);       // -0x1c（复用 ok）
                if (!ok)
                {
                    hasError = true;
                    break;
                }
                int cnt = ScanInt(&ok);        // -0x10c
                if (!ok)
                {
                    hasError = true;
                    break;
                }
                if (cnt <= 0 || cnt > 9)       // 0x89e71da
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xca8,
                                          "HELLPARTY_LOG : GROUP ORDER SCRIPT ERROR : %d\n",
                                          cnt);
                    hasError = true;
                }

                STHellPartyGroupRate orderRate;   // -0x118/-0x114
                orderRate.m_groupIdx = tvIdx;
                orderRate.m_rate = rate;

                const unsigned char diffSlot = git->second.m_field54;

                std::map<unsigned short, STHellPartyGroupOrder>::iterator mit =
                    localOrders.find(static_cast<unsigned short>(cnt));
                if (mit != localOrders.end())
                {
                    mit->second.m_rates.push_back(orderRate);
                    mit->second.m_rateByDifficulty[diffSlot] +=
                        static_cast<unsigned short>(rate);
                }
                else
                {
                    STHellPartyGroupOrder newOrder;
                    newOrder.m_rates.push_back(orderRate);
                    newOrder.m_rateByDifficulty[diffSlot] =
                        static_cast<unsigned short>(
                            newOrder.m_rateByDifficulty[diffSlot] + rate);
                    localOrders.insert(std::make_pair(cnt, newOrder));   // P167 同上
                }
            }
            Get_instance_groupOrder_map_pool()->insert(
                std::make_pair(fileIdx, localOrders));   // P167：ORIG make_pair<int&,map&>

            while (!groupQueue.empty())
            {
                curTop = groupQueue.top();
                if (curTop == lastTop || curTop == 0)
                {
                    groupQueue.pop();
                    continue;
                }
                lastTop = curTop;

                stHellParty_t* hellParty =
                    reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
                std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git2 =
                    hellParty->m_groupMap.find(static_cast<unsigned short>(curTop));

                fieldIdx = 0;
                (&action.m_type)[fieldIdx++] = 5;
                while (git2->second.m_monsterIdx[slotIdx] > 0)
                {
                    (&action.m_type)[fieldIdx++] =
                        git2->second.m_monsterIdx[slotIdx];
                    (&action.m_type)[fieldIdx++] = 1;
                    (&action.m_type)[fieldIdx++] = 1;
                    (&action.m_type)[fieldIdx++] = 1;
                    (&action.m_type)[fieldIdx++] = -1;
                    owner.m_vec10.push_back(action);
                    fieldIdx = 1;
                    ++slotIdx;
                }
                fieldIdx = 0;
                slotIdx = 0;
            }
            return hasError;                   // 89e762c：hellparty 直接收尾
            }
            LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                  __FUNCTION__, 0xced,
                                  "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 - \xbe\xf8\xb4\xc2 \xc4\xab\xc5\xd7\xb0\xed\xb8\xae - %s\n",
                                  path);
            hasError = true;
            continue;
        }
        else if (j == 1)
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));
            switch (action.m_type)
            {
            case 0:
                if (!checkValidIndex((&action.m_type)[j], monsterList))   // 0x10(%ebp)
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xcfc,
                                          "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xb8\xf3\xbd\xba\xc5\xcd` - %s\n",
                                          path);
                    hasError = true;
                }
                break;
            case 1:
                if (!checkValidIndex((&action.m_type)[j], trapList))      // 0x14(%ebp)
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xd04,
                                          "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xc6\xae\xb7\xa6` - %s\n",
                                          path);
                    hasError = true;
                }
                break;
            case 2:
                if ((&action.m_type)[j] == -1)
                    break;
                if (assignItems.size() >
                    static_cast<unsigned int>((&action.m_type)[j]))
                {
                    int target = (&action.m_type)[j];
                    std::list<STAssignItem>::iterator it = assignItems.begin();
                    for (int k = 0; k < target; ++k)
                        it++;                         // P167：ORIG ppEi（后置）
                    if (it->m_field00 != 0xffff0001)
                    {
                        it->m_field00 = 0xffff0001;
                    }
                    else
                    {
                        hasError = true;
                        LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                              __FUNCTION__, 0xd1d,
                                              "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xbe\xc6\xc0\xcc\xc5\xdb` - \xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xb8\xae\xbd\xba\xc6\xae - %s\n",
                                              path);
                    }
                }
                else
                {
                    hasError = true;
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xd23,
                                          "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xbe\xc6\xc0\xcc\xc5\xdb` - \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xb8\xae\xbd\xba\xc6\xae \xbb\xe7\xbf\xeb\xc7\xcf\xb7\xc1 \xc7\xd4- %s\n",
                                          path);
                }
                break;
            }
        }
        else
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));
        }
    }
    return hasError;                           // 89e78f2
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
int exportCharacterScript(STCharacterScript* script, const char* path)
{
    static char saveFileName[0x100];
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szCharacterScriptBaseDirectory, path);
    FILE* file = fopen(saveFileName, "wb");
    if (!file)
        return 0;

    fprintf(file, "[job] `%s`\n", getCharacterJobName(ENUM_CHARACTERJOB(script->m_job)));
    unsigned int i = 0;                     // P191：ORIG 声明 + for-init 双初始化（0x8a1030d/0x8a1032a）
    fwrite("[growtype name]", 1, 0xf, file);
    for (i = 0; i < 6; ++i)                 // P2：ORIG cmpl $0x5 + setbe ⇒ unsigned 计数
        fprintf(file, " `%s`", script->m_growtypeName[i].c_str());
    fputc(10, file);
    fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    fputc(10, file);
    fprintf(file, "[jump speed] %d\n", script->m_jumpSpeed);
    fputc(10, file);
    fwrite("[initial value]\n", 1, 0x10, file);
    exportCharacterStatusScript(file, script->m_status);
    exportCharacterGiveSkillScript(file, &script->m_skillList[0]);
    exportCharacterEtcStatsScript(file, &script->m_etcStats[0]);
    for (i = 1; i < 7; ++i)                 // ORIG 复用同一 unsigned i（槽 -0x1c，0x8a1046a 尾测 setbe）
    {
        fprintf(file, "\n[\xbe\xd5\xd0\xd1\xbc\xbc%d]\n", i);  // ORIG "\n[觉醒技%d]\n"
        // ORIG 0x8a1038b：`imul $0x57; add $0x20; add this; add $4` ⇒ 基址取
        // m_initialStatus(+0x24) + i*0x57（与 m_growStatus[i-1] 同址，P30 两步折叠形）。
        exportCharacterStatusScript(file, &script->m_status[i]);
        exportCharacterGiveSkillScript(file, &script->m_skillList[i]);
        exportCharacterEtcStatsScript(file, &script->m_etcStats[i]);
    }
    fputc(10, file);

    if (script->m_motionWaiting != "")
        fprintf(file, "[waiting motion] `%s`\n", script->m_motionWaiting.c_str());
    if (script->m_motionMove != "")
        fprintf(file, "[move motion] `%s`\n", script->m_motionMove.c_str());
    if (script->m_motionSit != "")
        fprintf(file, "[sit motion] `%s`\n", script->m_motionSit.c_str());
    if (script->m_motionDamage[0] != "")        // S248 STCharacterScript.h 数组化同步
        fprintf(file, "[damage motion 1] `%s`\n", script->m_motionDamage[0].c_str());
    if (script->m_motionDamage[1] != "")
        fprintf(file, "[damage motion 2] `%s`\n", script->m_motionDamage[1].c_str());
    if (script->m_motionDown != "")
        fprintf(file, "[down motion] `%s`\n", script->m_motionDown.c_str());
    if (script->m_motionOverturn != "")
        fprintf(file, "[overturn motion] `%s`\n", script->m_motionOverturn.c_str());
    if (script->m_motionJump != "")
        fprintf(file, "[jump motion] `%s`\n", script->m_motionJump.c_str());
    if (script->m_motionJumpattack != "")
        fprintf(file, "[jumpattack motion] `%s`\n", script->m_motionJumpattack.c_str());
    if (script->m_motionRest != "")
        fprintf(file, "[rest motion] `%s`\n", script->m_motionRest.c_str());
    if (script->m_throwMotion1[0] != "")
        fprintf(file, "[throw motion 1-1] `%s`\n", script->m_throwMotion1[0].c_str());
    if (script->m_throwMotion2[0] != "")
        fprintf(file, "[throw motion 1-2] `%s`\n", script->m_throwMotion2[0].c_str());
    if (script->m_throwMotion1[1] != "")
        fprintf(file, "[throw motion 2-1] `%s`\n", script->m_throwMotion1[1].c_str());
    if (script->m_throwMotion2[1] != "")
        fprintf(file, "[throw motion 2-2] `%s`\n", script->m_throwMotion2[1].c_str());
    if (script->m_throwMotion1[2] != "")
        fprintf(file, "[throw motion 3-1] `%s`\n", script->m_throwMotion1[2].c_str());
    if (script->m_throwMotion2[2] != "")
        fprintf(file, "[throw motion 3-2] `%s`\n", script->m_throwMotion2[2].c_str());
    if (script->m_throwMotion1[3] != "")
        fprintf(file, "[throw motion 4-1] `%s`\n", script->m_throwMotion1[3].c_str());
    if (script->m_throwMotion2[3] != "")
        fprintf(file, "[throw motion 4-2] `%s`\n", script->m_throwMotion2[3].c_str());
    if (script->m_motionDash != "")
        fprintf(file, "[dash motion] `%s`\n", script->m_motionDash.c_str());
    if (script->m_motionDashattack != "")
        fprintf(file, "[dashattack motion] `%s`\n", script->m_motionDashattack.c_str());
    if (script->m_motionGetitem != "")
        fprintf(file, "[getitem motion] `%s`\n", script->m_motionGetitem.c_str());
    if (script->m_motionBuff != "")
        fprintf(file, "[buff motion] `%s`\n", script->m_motionBuff.c_str());

    if (!script->m_attackMotion.empty())
    {
        fwrite("[attack motion]\n", 1, 0x10, file);
        for (std::vector<std::string>::iterator it = script->m_attackMotion.begin();
             it != script->m_attackMotion.end(); it++)   // P167/P28：ORIG deEv + ppEi
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack motion]\n", 1, 0x11, file);
    }
    if (!script->m_etcMotion.empty())
    {
        fwrite("[etc motion]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it = script->m_etcMotion.begin();
             it != script->m_etcMotion.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc motion]\n", 1, 0xe, file);
    }
    fputc(10, file);

    if (script->m_jumpattackInfo != "")
        fprintf(file, "[jumpattack info] `%s`\n", script->m_jumpattackInfo.c_str());
    if (script->m_dashattackInfo != "")
        fprintf(file, "[dashattack info] `%s`\n", script->m_dashattackInfo.c_str());
    if (!script->m_attackInfo.empty())
    {
        fwrite("[attack info]\n", 1, 0xe, file);
        for (std::vector<std::string>::iterator it = script->m_attackInfo.begin();
             it != script->m_attackInfo.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack info]\n", 1, 0xf, file);
    }
    if (!script->m_etcAttackInfo.empty())
    {
        fwrite("[etc attack info]\n", 1, 0x12, file);
        for (std::vector<std::string>::iterator it = script->m_etcAttackInfo.begin();
             it != script->m_etcAttackInfo.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc attack info]\n", 1, 0x13, file);
    }
    fputc(10, file);

    for (std::vector<WeaponInfo>::size_type w = 0; w < script->m_weaponWav.size(); ++w)
    {
        fwrite("[weapon wav]\n", 1, 0xd, file);
        std::vector<std::string>& wavs = script->m_weaponWav[w].m_wavs;
        // P167：ORIG 用 size()+operator[]（MISSING ixEj/size），非 begin/end 迭代器
        for (std::vector<std::string>::size_type i = 0; i < wavs.size(); ++i)
            fprintf(file, "`%s`\n", wavs[i].c_str());
        fwrite("[/weapon wav]\n", 1, 0xe, file);
    }
    fputc(10, file);

    fwrite("[weapon hit info]\n", 1, 0x12, file);
    for (std::vector<WeaponInfo>::size_type w = 0; w < script->m_weaponWav.size(); ++w)
    {
        const WeaponInfo& info = script->m_weaponWav[w];
        if (info.m_hitType == 0)
            fwrite("`[blow]` ", 1, 9, file);
        else if (info.m_hitType == 1)
            fwrite("`[cut]` ", 1, 8, file);
        else if (info.m_hitType == 2)
            fwrite("`[burn]` ", 1, 9, file);
        else if (info.m_hitType == 3)
            fwrite("`[etc]` ", 1, 8, file);
        // ORIG 0x8a110f1：`test %al,%al; je <no blood 块>` ⇒ 正极性 != 0 前置
        if (info.m_blood != 0)
            fwrite("`[blood]` ", 1, 0xa, file);
        else
            fwrite("`[no blood]` ", 1, 0xd, file);
        fprintf(file, "%d %.2f\n", info.m_field14, info.m_rate18);
    }
    fwrite("[/weapon hit info]\n", 1, 0x13, file);

    fwrite("[weapon skill info]\n", 1, 0x14, file);
    for (std::vector<WeaponInfo>::size_type w = 0; w < script->m_weaponWav.size(); ++w)
    {
        const WeaponInfo& info = script->m_weaponWav[w];
        fprintf(file, "%.2f %.2f %.2f %.2f\n",
                info.m_skillRates[0], info.m_skillRates[1],
                info.m_skillRates[2], info.m_skillRates[3]);
    }
    fwrite("[/weapon skill info]\n", 1, 0x15, file);

    fclose(file);
    convertBinaryToText(saveFileName);
    return 1;
}
