// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RDARScriptEtcParameter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— RDARScriptEtcParameter.cpp 域（自由脚本 producer）
//
// CDataManager::init（0x08358332..0x0835f1d5）调用的若干自由脚本装载函数。
// 本节逐一转写自 docs/class_func_reports/_lt_global_gt_/ 对应报告：
//   importGoldLimitByLevel       0x088fdb69
//   importInvalidName            0x08908338
//   importExperienceItemMapInfo  0x089136a4
//   importBoosterGage            0x08913a7e
//   importImageCommunicationInfo 0x08913dbc
//
// 每个函数返回 int：0=失败（文件加载失败/键缺失/阈值超限），1=成功。
// 数据写入由调用方（CDataManager::init）以 this+offset 传入目标成员地址。
// ============================================================================

#include "CBoosterGage.h"        // BoosterGageData 真实布局 + BoosterPoint
#include "CDataManager.h"        // 目标成员类型：InvalidCharData/ImageCommunicationData
                                 // + m_moneyLimit/m_invalidChar/m_mapA928/m_mapA940/m_imageCommData
#include "PremiumServiceData.h"
#include "CEnvironment.h"       // SpinLock
#include "LogManager.h"
#include "CCompoundAvatar.h"   // ENUM_CHARACTERJOB
#include "NpcScript.h"         // getCharacterJob
#include <set>
#include "AdditionalEffectData.h"
#include "stNoviceCommand.h"
#include "sync_script.h"             // STEquipmentPartSetPiece / STEquipmentScript / EquipmentParameterInfo

// ---- 装备特效套装表（ORIG B 0x094f7148）----
// STEquipmentPartSetData 类视图与 DataManagerInitDepends.h 权威声明一致
// （+0x00 int / +0x04 string / +0x08 STEquipmentScript* / +0x0c vector<piece>），
// 成员函数定义归 InitScriptProducers_2.cpp（importEquipmentPartSetInfo 生产 TU）。
class STEquipmentPartSetData
{
public:
    STEquipmentPartSetData();
    ~STEquipmentPartSetData();

    int m_field0;                                   // +0x00（ctor 置 -1）
    std::string m_name4;                            // +0x04
    STEquipmentScript* m_script;                    // +0x08（dtor delete）
    std::vector<STEquipmentPartSetPiece> m_pieces;  // +0x0c
};

extern std::map<int, STEquipmentPartSetData> g_equipmentEffectSetMap_;
#include "RDARScriptEtcParameter/getSameStringIndex.inc"
extern const char* characterEquipmentTypeTags[26];

// ---- ORIG B 0x094f7148：装备特效套装表（importEquipmentEffectSetInfo 写入）----
std::map<int, STEquipmentPartSetData> g_equipmentEffectSetMap_;

// ---- ORIG B 0x094f7160：附加效果数据表（importAdditionalEffectListScript 写入）----
std::map<int, AdditionalEffectData> g_additionalEffectMap_;

// ---- 跨 TU 依赖声明 ----
#include "RDARScriptEtcParameter/getGrowTypeIndex.inc"

#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include <string>
#include <map>
#include <cstring>

// ============================================================================
// stRangeElement 显式特化（ORIG 0x891f0ac/0x891f0f6 C1 双参构造 +
// 0x8948ff0/0x89491a6 空析构；CDataManager.h 冻结故在本 TU 特化）。
// ORIG importInvalidName push_back(stRangeElement<T>(lo,hi)) 右值形态
// （push_back(EOS2_) @0x891f0d4/0x891f122），依赖本特化提供双参构造。
// ============================================================================
template <>
struct InvalidCharData::stRangeElement<unsigned char>
{
    unsigned char m_min;
    unsigned char m_max;
    stRangeElement(unsigned char a, unsigned char b) : m_min(a), m_max(b) {}
};

template <>
struct InvalidCharData::stRangeElement<unsigned short>
{
    unsigned short m_min;
    unsigned short m_max;
    stRangeElement(unsigned short a, unsigned short b) : m_min(a), m_max(b) {}
};

// ============================================================================
// importGoldLimitByLevel —— ORIG 0x088fdb69
// 报告 docs/class_func_reports/_lt_global_gt_/importGoldLimitByLevel.md
// 调用点（init.md 0x832…+0x2e4b）：CDataManager::init → this+0x6410
//   （CDataManager::m_moneyLimit[201]，int 数组）。
// 目标为 unsigned int*（报告 uint*），访问 out[level]。
// 验收：gap 5→1；P167 差集 11/11 归零；残余 1 行 = ORIG `mov $0,%eax;
// mov %eax,-0x10` 死初始化二段物化，4.4 恒折单条 movl（N18a 族）。
// ===========================================================================
#include "RDARScriptEtcParameter/importGoldLimitByLevel.inc"
}

// ============================================================================
// importInvalidName —— ORIG 0x08908338
// 报告 docs/class_func_reports/_lt_global_gt_/importInvalidName.md
// 调用点（init.md +0x2279）：CDataManager::init → this+0x86c4
//   （CDataManager::m_invalidChar，InvalidCharData）。
// 布局：+0x00 vector<unsigned char> m_charList
//       +0x0c vector<stRangeElement<unsigned char>> m_charRange
//       +0x18 vector<unsigned short> m_shortList
//       +0x24 vector<stRangeElement<unsigned short>> m_shortRange
// 验收：P167 差集 ORIG-only=0；OURS-only 4×~stRangeElement = 4.1 对 user
// 空析构类 temp 消解 dtor 调用、4.4 判 non-trivial 必发 D1+EH cleanup
// （P71/P67 跨编译器结构性差异；ORIG D1/D2 W 0x8948ff0/0x89491a6 须保留，
// 不删用户析构）⇒ 标 N 封顶。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] IDENTICAL/AE：ORIG 0x08908338；本波两处钥匙：
// ①6 处二次守卫 `if (!ok) break;` 改「正极性 if + else break」⇒ 4.4 产 ORIG 的
// `movzbl;test;je`（P194/P168a④ 反例复活；`==0`/`!=true` 两写法仍产 xor）；
// ②6 个 bool 按 ORIG 槽浅→深声明（okA..okF，P309）⇒ 槽 -0x21..-0x26 全部归位。
// 173=173 归一化逐条全等（ndiff 158→0）。删 stRangeElement 用户空析构后 4.4 不发
// 4×D1+EH（P71/P67），D1/D2 仍由 vector 使用点发 W（同 ORIG 0x8948ff0/0x89491a6）。
// p167 CLEAR（22/22），语义闭合。
#include "RDARScriptEtcParameter/importInvalidName.inc"
}

// ============================================================================
// importExperienceItemMapInfo —— ORIG 0x089136a4
// 报告 docs/class_func_reports/_lt_global_gt_/importExperienceItemMapInfo.md

// ============================================================================
// importExperienceItemMapInfo —— ORIG 0x089136a4
// 报告 docs/class_func_reports/_lt_global_gt_/importExperienceItemMapInfo.md
// 调用点（init.md +0x181c / +0x1891）：
//   init → this+0xa928（m_mapA928），bool=0
//   init → this+0xa940（m_mapA940），bool=1
// 阈值：第二个值 > 0x88b8（35000）时 log 报错返回 0。
// OBJDUMP 复核（0x89137b2 true 分支 / 0x8913801 false 分支）：
//   true (param_2==true, -0x4c(%ebp)!=0):
//     make_pair(&local_28 {first=-0x44(level), second=-0x48(val)})  → insert
//   false:
//     make_pair(&local_10 {first=-0x48(val), second=-0x44(level)}) → insert
//   即 second==true → insert(val,key)；second==false → insert(key⋯ 实际以
//   实参压栈序为准；下述实现按 false 存 (key,val)、true 存 (val,key)。
// 注意报告 Ghidra 的 make_pair 两分支实参顺序相反，以 objdump 为准：
//   false 分支把 -0x48(val) 放 first、-0x44(level) 放 second；true 分支相反。
// ============================================================================
// [b28d-solo-RDARScriptEtcParameter] AE：ORIG 0x089136a4；本波按 P309（4.4 头部
// 声明「先声明浅」）把 ok 提到 token 之前 ⇒ token 归位 -0x40、key/val/形参截断槽
// 全部归位（134=134 助记符全等）。残余 4 条 = ok 槽 -0x3c vs ORIG -0x39：4.4 标量
// 槽基 4 对齐而 4.1 从 -0x39 起；实测 4B 数组/结构体尾字节取址恒 `lea -0x3c;add $0x3`
// 不折叠（P235 反例）⇒ N95 族封顶。p167 CLEAR（17/17），语义闭合。
#include "RDARScriptEtcParameter/importExperienceItemMapInfo.inc"
}

// ============================================================================
// importBoosterGage —— ORIG 0x08913a7e
// 报告 docs/class_func_reports/_lt_global_gt_/importBoosterGage.md
// 调用点（init.md +0x2412）：CDataManager::init → this+0x8770
//   （CDataManager::m_boosterGage，BoosterGageData）。
// 布局：+0x00 vector<BoosterPoint> m_BoosterPoint
//       +0x0c unsigned char m_cMaxBoosterGage
//       +0x0d unsigned char m_cFieldD
//       +0x0e unsigned char m_cMultiboxCount
// BoosterPoint：+0x00 unsigned long m_dwItemIdx / +0x04 unsigned char m_cPoint
// ============================================================================
// [b28 BM2 判词·AE] 清单 NEAR(2) 为全二进制 call 目标标签伪差；fresh
// /tmp/tu_RDARScriptEtcParameter.o fn_diff_tu 归一 0 差（152=152 逐条）。
// [b28d-27] AE：ORIG 0x08913a7e；fn_diff_tu 归一 0 差（157=157），p167 CLEAR。
// [b28d-solo-RDARScriptEtcParameter] AE（IDENTICAL 级）：fresh .o fn_diff_tu
// 归一 0 差（152=152，raw 157=157），p167 CLEAR（19/19）。
#include "RDARScriptEtcParameter/importBoosterGage.inc"
}

// ============================================================================
// importImageCommunicationInfo —— ORIG 0x08913dbc
// 报告 docs/class_func_reports/_lt_global_gt_/importImageCommunicationInfo.md
// 调用点（init.md +0x0966）：CDataManager::init → this+0xb4c4
//   （CDataManager::m_imageCommData，ImageCommunicationData，写 +0x00 int）。
// 段键："[charge]" 写首字；"[/image communication]" 结束返回 1。
// 验收：gap 12→1；差集 10/10 归零；残余 = ORIG 三态汇聚 ebx+终值 esi 双
// 寄存器物化（4.1 choose-without-store，N3/N76 族）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] AE：ORIG 0x08913dbc；本波把两处 `return 1`
// 改为 `break` + 循环外单一 `return 1`（P283/P63）⇒ 4.4 复现 ORIG 三态共享尾
// （state 0=返回 result / 1=退出返回 1 / 2=继续，result/state 全程 esi/ebx 无槽），
// 112=112 归一化逐条全等（ndiff 45→0）。p167 CLEAR（10/10），语义闭合。
#include "RDARScriptEtcParameter/importImageCommunicationInfo.inc"
}

// ===========================================================================
// 装备部件套装（ORIG "../RDARScriptEtcParameter.cpp" 域）——本 TU 不再持有。
//
// 唯一定义归 InitScriptProducers_2.cpp（importEquipmentPartSetInfo 生产 TU，
// DataManagerInitDepends.h 权威声明指定）：STEquipmentPartSetData 类视图 +
// C1 0x0891b002 W / D1 0x08907348 T + g_equipmentPartSetMap_（B 0x094f7130）。
// 本文件此前与该 TU 三重重复（wave22 链接 multiple definition），已删除；
// 本 TU 无任何消费点，无需前向声明。
// ===========================================================================
// importPremiumServiceData —— ORIG T 0x08912e47
// （docs/class_func_reports/_lt_global_gt_/importPremiumServiceData.md）
// 真形（ORIG 0x08912e47 汇编+Ghidra C 双段转写，P163/P165/P177/P178 族）：
// 外层字符串匹配 "[premium service]"，内层 while(ScanType) 每轮穿
// static 三件套（__lookup map<char const*,int,less_TCHAR> + __init_lock
// SpinLock + __inited，guard 声明位于重派发标签后）；未建表持锁 id=0 进
// switch，自 case 0（与 0x3059 共 leaf，P163）起逐 case 构造
// value_type(键, nId 具名槽) + insert 后**贯穿**下一 leaf，公共尾
// leave+__inited=1 后 jmp guard 以同 token 重查派发；建表后
// c_str→find→!=end 三目单点写 id，switch(id) 派发：
//   [type]0x3059→+0x00、[icon index]0x305c→+0x10、[icon explain]0x305f→
//   ScanStr(+0x14)、[add equipment list]0x3062→循环 push_back(+0x18)、
//   [main premium image index]0x306e→+0x04/+0x08 双 ScanInt、
//   [additional premium image index]0x3072→+0x0c、
//   [add selectAble equipment list]0x3076→双 ScanInt + insert(make_pair(a,
//     make_pair(false,b))) 嵌套实参形（+0x24 map）、
//   [/premium service]0x3088→out.push_back(data)+data.clear()。
// loadRDARScriptFile 失败记日志（0x3044）返 0，成功返 1。
// 验收（§6.2）：gap 158→1；P167 调用点差集 64/64 全命中；残余 1 insn =
// 4.4 在 token== 匹配后发射 `jmp <内层检查>` 中继 vs 4.1 fall-through
// （跨版本 nop;jmp 岛 vs jump-threading，N 类豁免）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95/N7：ORIG 0x08912e47；
// char[4] framePad 对齐 token 槽（-0xfc，与 ORIG 同址）；残余 413 条 =
// ①data 槽深 4（我方 -0x154 vs ORIG -0x150，槽间距 0x58 vs 0x54：data 与 token
// 之间的编译器临时集合差，sizeof(PremiumServiceData)=0x3c 已 static_assert 锁定）；
// ②循环头 `jmp+nop` 与 XOR 形循环头位置差；③立即数压栈/callee-saved 角色差
// （4.1 vs 4.4，N7）不可翻。485=485 指令数全等。p167 CLEAR（64/64），语义闭合。
#include "RDARScriptEtcParameter/importPremiumServiceData.inc"
}
// ===========================================================================
// importCommonSkillIndexes —— ORIG T 0x08913f48
// （docs/class_func_reports/_lt_global_gt_/importCommonSkillIndexes.md）
// 清空两输出容器后按脚本填充："[common skill]" 后连续 ScanInt 进 vector<int>；
// "[except skill info]" 块内每轮 ScanStr(jobName)→getCharacterJob + 连续
// ScanInt 进 set<int>，插入 map<job,set>；loadRDARScriptFile 失败记日志返 0，
// 成功返 1。
// 验收：差集 35/35 归零；残余为 set/pair 临时链槽位序（N82）+ 块排布镜像。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95（临时对象槽序）：ORIG 0x08913f48。
// 本波修：①token/ok 声明提到两 clear 之前、job 提到 skills 之前（P309 先声明浅）
// ⇒ 局部槽 -0x54/-0x55/-0x5c/-0x60/-0x78/-0x7c/-0x7d/-0x84 全部归位；
// ②okJob 守卫改正极性 if/else-break 形 ⇒ `test;je` 与 ORIG 同助记符。
// 残余 103 条 = 临时对象槽布局：ORIG 4.1 按使用序分配（skills.insert 返回值
// -0x10 → make_pair sret -0x2c → pair 临时 -0x48 → map insert 返回值 -0x50），
// 4.4 按对象大小降序（28B -0x24 → 24B -0x40 → 8B -0x48 → 8B -0x50）；
// 两者对象集合相同、语义全同，无源码旋钮（P178 具名 entry 反致全帧平移）
// ⇒ N95/N82 族封顶。p167 CLEAR（35/35），语义闭合。
#include "RDARScriptEtcParameter/importCommonSkillIndexes.inc"
}

// ===========================================================================
// importCancelSkillIndices —— ORIG T 0x08914dd1
// （docs/class_func_reports/_lt_global_gt_/importCancelSkillIndices.md）
// 真形（ORIG 0x08914dd1，同 importPremiumServiceData 双相派发族 P163/P177）：
// 外层 for(;;)+ScanType；每轮穿 static 三件套（__lookup map<char const*,int,
// less_TCHAR> + __init_lock SpinLock + __inited，声明位于 redo_lookup 标签后）；
// 未建表 enter 双检 id=0 进 switch（case 0 与 0x3384 共 leaf），自
// "[cancel skill]"0x3384 → "[/cancel skill]"0x3388 → "[character job]"0x338c →
// "[/character job]"0x33aa 逐 case 构造 value_type(键,字面量) + insert 后
// **贯穿**下一 leaf，公共尾 leave+__inited=1 重查派发。建表后仅 0x338c 有
// parse：jobName ScanStr 失败 return 0；key.first=getCharacterJob(c_str)；
// growName ScanStr 失败 return 0；key.second=getGrowTypeIndex；ScanInt 循环
// 进 skills，失败轮 out.insert(value_type(key,skills)) 直构形（无 make_pair
// 转换临时，OnlyOrigBatch14 口径）+ key=(0xb,-1) 重置 + skills.clear()。
// "[cancel skill]"/"[/cancel skill]"/"[/character job]" 命中均无动作（仅登记）。
// loadRDARScriptFile 失败记日志（0x3372）返 0，成功返 1。
// 验收：gap 52→1（413/412）；P167 差集 58/58 归零；残余=槽号平移+ctor/dtor
// 块序（N82/EH-merge 族）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95/N7：ORIG 0x08914dd1；
// char[4] framePad 使槽基精确对齐 ORIG（key -0xb4/-0xb0、skills -0xc8）；
// 残余 338 条 = ①循环头 ORIG 条件置顶直落 vs 我方前置 `jmp+nop`（while 形实测
// 反劣 345 ⇒ 保留 for(;;)）；②立即数压栈与 callee-saved 角色差（4.1 vs 4.4，N7）
// 及 token 槽内偏移。p167 CLEAR（58/58），语义闭合。
#include "RDARScriptEtcParameter/importCancelSkillIndices.inc"
}
// ===========================================================================
// importAdditionalEffectListScript —— ORIG T 0x08907e3a
// （docs/class_func_reports/_lt_global_gt_/importAdditionalEffectListScript.md；
// 偏移经 objdump 复核：+0x00 index/+0x04 m_strings(animation)/+0x10 type/
// +0x14 fixX/+0x18/+0x1c/+0x20/+0x24/+0x28 各延迟项/+0x2c drawOver byte）
// 清空 g_additionalEffectMap_ 后按 "[effect]" 块解析各字段，
// "[/effect]" 时 g_additionalEffectMap_[index] = tmp。
// loadRDARScriptFile 失败记日志返 0，成功返 1。
// 验收：差集 40/40 归零；clean 225=225，残余为 clear/dtor 块序镜像
// （4.4 jump-threading 跨版本伪影，N 类）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] AE：ORIG 0x08907e3a；外层 ScanType 失败改
// break+函数尾 return 1，复现 ORIG 共享出口（P63）；fn_diff_tu 归一 0 差
// （225=225，raw 230=230），p167 CLEAR（40/40）。
#include "RDARScriptEtcParameter/importAdditionalEffectListScript.inc"
}
// ===========================================================================
// importNoviceCommandScript —— ORIG T 0x0890402a
// （docs/class_func_reports/_lt_global_gt_/importNoviceCommandScript.md）
// 先清 0xb 个 vector<vector<int>> 槽；按职业 token 逐轮 push_back：
// gunner→+0x18(10 轮)/fighter→+0x0c(8)/swordman→+0x00(8)/
// at fighter→+0x54(8)/thief→+0x48(6)/mage→+0x24(10)/
// at gunner→+0x3c(10)/priest→+0x30(10)，每轮内层 10 个 ScanInt。
// loadRDARScriptFile 失败返 0x11cc，成功返 1。
// 重建形 = ORIG 物理内联展开（P165 we-smaller 复原：8 个独立职业块、
// 各自 j/k 计数器，勿用 dst/rounds 参数化共享内层循环）。
// ===========================================================================
#include "RDARScriptEtcParameter/importNoviceCommandScript.inc"
}
// ===========================================================================
// importEquipmentEffectSetInfo —— ORIG T 0x0890793f
// （docs/class_func_reports/_lt_global_gt_/importEquipmentEffectSetInfo.md）
// 阶段一：解析 "[effect equipment part set]" 块 → STEquipmentPartSetData
//   （m_field0=ScanInt、m_name4=ScanStr、m_pieces 每件
//   getSameStringIndex(characterEquipmentTypeTags,0x1a)），写入
//   g_equipmentEffectSetMap_[id]；阶段二：遍历生成 STEquipmentScript 并
//   ImportEquipmentScript(script, name, false, moduleType, id)，失败记日志
//   （0x1712）删除后返 0；m_paramMap678 若含 key=-1 档则复制后清空，
//   以套装件数为 key 重存。loadRDARScriptFile 失败记日志（0x16df）返 0。
// 验收：P167 差集 66/66 归零（op-> 13×、op[] 3×、find/end/clear/size/EPI
// 拷贝链逐点对应）；残余 = 循环出口多路 dtor 链块序与槽号
// （N82/EH merge 跨版本族）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95/N82：ORIG 0x0890793f。本波修：
// ①okPiece 具名局部删除、改 `for(;;){ piece; if(!ScanStr(&pieceName)) break; ... }`
// ⇒ 消 4.4 落槽物化（`mov %al,-0x21; cmpb`），与 ORIG 寄存器直测同族；
// ②`pNew` 具名局部删除、内联 `it->second.m_script = new STEquipmentScript;`
// ⇒ 消 -0x20 槽往返（ORIG new 结果留 %ebx）。347/343，ndiff 278→173。
// 残余 = 帧顶槽基差 4（ORIG token -0x34 vs 我方 -0x30；4.4 帧顶 gap 0x24 vs
// 4.1 0x28）+ 帧 0x50c vs 0x4fc：未使用标量/数组局部 4.4 不占槽（int/float
// 取址探针零效），double framePad 反致 +0xc 过度下移 ⇒ N95/N82 族封顶。
// p167 CLEAR（66/66），语义闭合。
#include "RDARScriptEtcParameter/importEquipmentEffectSetInfo.inc"
}
