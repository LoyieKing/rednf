// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptLeafA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>

#include "ScriptLeafA2/getChangeStatusType.inc"
#include "ScriptLeafA2/ConvertWeekdayStringToInt.inc"
#include "ScriptLeafA2/getItemRarity.inc"
#include "ScriptLeafA2/getCharacterJobToEnumValue.inc"

#include "CDataManager.h"        // stNPCCommonData_t / RandomOptionItem_Regenerator /
                                 // eventReward / StageMapList / APSystem 域权威声明
#include "APSystemTypes.h"       // CActionPointEx / _MedalRewardItem 权威布局
#include "StrikerScript.h"       // StrikerScript 及子类型（ContentsType 等）
#include "CItemAmplifier.h"      // upgrade_table_t / upgrade_info_t 权威布局
#include "CItem.h"               // CItem::GetMedalPrice 声明与 m_field14
#include "DNFLexWrapper.h"       // loadRDARScriptFile / ScanType / ScanInt / ScanFloat / ScanStr
#include "CUserProc.h"          // APSystem::_ActionGroupIndex 完整定义
#include "CEnvironment.h"        // less_TCHAR / SpinLock
#include "LogManager.h"          // LogManager::logFormat

// ORIG B 全局（定义 InitScriptProducers_5.cpp；同布局视图访问：
// STScriptFileList +0x00 即 std::map<int,std::string>，sizeof 0x18）。
namespace
{
class StageScriptFileListView
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00
};
}  // namespace
extern StageScriptFileListView g_stageMapScriptFileList;    // B 0x09494e20
extern char g_npcScriptBaseDirectory[0x100];                // B 0x094fe680

#include "ScriptLeafA2/convertTabTypeStringToEnum.inc"



#include "ScriptLeafA2/clear.inc"


#include "ScriptLeafA2/loadNPCCommonData.inc"


// ---- ORIG 全局：g_RegenerationRandomOptionScriptBaseDirectory
//      （B 0x095005c0 size 0x100，CU=RDARScriptRegeneration_ROI.cpp，与本函数同 TU）----
// char[0x100] 零初始化脚本目录缓冲（init 流程填充）。ORIG 全二进制零指令、零数据
// 引用（二进制扫描唯一命中在 .symtab 自身）⇒ 死数据；补定义对齐符号账
// （REAL_GAP_SYMBOL），不新增消费点（importRegenerationRandomOptionScript 的路径
// 参数仍按 ORIG 0x08a7e610 实参字面量 "Etc/RandomOption/..." 形式）。
// 位置说明（P327）：本 TU 首符号保持 convertTabTypeStringToEnum（ORIG 另一 TU 的
// GI 键名），故不把本变量前移；ORIG 的
// `_GLOBAL__I_g_RegenerationRandomOptionScriptBaseDirectory`（0x08a7ea66）与本 TU
// 键名互斥，属结构性 REORDER_ONLY。
char g_RegenerationRandomOptionScriptBaseDirectory[0x100];

#include "ScriptLeafA2/importRegenerationRandomOptionScript.inc"


#include "ScriptLeafA2/importScript.inc"


#include "ScriptLeafA2/compare.inc"
{
    return a.m_uniqueId < b.m_uniqueId;
}

// ---- stagemap 域镜像（布局取自 loadList 报告汇编偏移；类型 ORIG 真实存在，
//      完整建模归 stagemap 域批次）----
namespace stagemap
{

struct StageData
{
    int m_field0;                            // +0x00
    int m_field4;                            // +0x04 (ORIG StageData stride 0x10)
    int m_field8;                            // +0x08
    int m_fieldC;                            // +0x0c
};

struct DistrictData
{
    int m_field0;                            // +0x00
    std::vector<StageData> m_vec4;           // +0x04 (ORIG StageData vector)
    std::string m_appendImage;              // +0x10 (ORIG std::string size 4)
    int m_field14;                           // +0x14
};

struct AreaData
{
    int m_field0;                            // +0x00
    char m_pad04[0x1c];                      // +0x04
    std::vector<DistrictData> m_vec20;       // +0x20
    int m_field2c;                           // +0x2c
};

class StageMapScript
{
public:
    StageMapScript();                        // ORIG W 0x08373488
    ~StageMapScript();                       // ORIG W 0x0837f782
    void clear();                            // ORIG W 0x08373428

    char m_pad00[0x14];                      // +0x00（未还原字段区）
    std::vector<AreaData> m_areaVec;         // +0x14
};

}  // namespace stagemap

#include "ScriptLeafA2/importStageMapScript.inc"

#include "ScriptLeafA2/loadList.inc"


// ============================================================================
// APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript
// ORIG T 0x08894996（静态成员）。报告
// APSystem__CActionPointEtcParameter/importActionPointSystemEtcScript.md 全文转写。
// 返回 false=成功/文件失败；错误路径亦为 false（调用方仅判零，语义兼容；
// ORIG 各行级错误码见注释）。
// ============================================================================
namespace APSystem
{

#include "ScriptLeafA2/importActionPointSystemEtcScript.inc"


}  // namespace APSystem

#include "ScriptLeafA2/ImportRewardScript.inc"


// ============================================================================
// importUpgradeTable —— ORIG T 0x08900c26
// 报告 _lt_global_gt_/importUpgradeTable.md 全文转写。
// 返回 1=成功（含 EOF）；0=失败（文件打开失败/关键扫描失败/键重复/异常捕获）。
//
// 【batch-15/16 验收标注（§6.2 口径）】orig 1119 / ours 1166（gap -132 → -47）。
//   ① 调用点差集（P167）**已归零**：direct 138 ↔ 136，missing 14 / extra 12 全部
//      是 C2/D2 ↔ C1/D1 同体构造/析构符号名对称差 + 1 个 _Unwind_Resume，
//      无任何真实调用点缺口；indirect 0/0。
//   ② 段边界切片法（difflib 在 N82 帧号重排下不可信）逐 token 段核对，修掉两处
//      真语义偏差：[cost weight by upgrade level] 的取数通道（ORIG 用
//      ScanInt(bool*) 返回值 + 循环头 ok 复位，非 ScanInt(int*) 出参形）、
//      [amplification const] 循环入口缺 `ok = true` 复位（orig local_cb）；
//      并删除匿名 namespace 的 inline helper（-O0 逐点发 call，见 N-I 延伸）。
//   ③ 余 -47 集中在三处已定位的 4.1 专属形：0x089003fa 的守卫 bool 汇聚岛
//      （实测命名 bool 反而 +12，已回退）、0x08900d5a 的双段 catch
//      （begin_catch/end_catch + 清理后 rethrow 的 EH 展开表形）、
//      0x0890124x 的成功出口 jmp/nop 中继 ⇒ N3/N82/setcc-fold/跨版本 EH
//      命中 ⇒ 判**语义完成、byte 层封顶**。
//   ④ 三处「看起来更像 ORIG 但实测更远」的改写已回退并在原地留数值记录
//      （get_upgrade_info 逐行重发 gain 471、dword 下标视图 326、
//        命名 bool 汇聚岛 1177），避免后续批次重试。
// ============================================================================
// upgrade_info_t 的表行写入使用**真实字段下标直访**（batch-15 N-I 延伸：
// 匿名 namespace 的 inline helper 在 -O0 下同样逐点发 call，不豁免）；偏移→字段
// 映射按 upgrade_info_t 已建分段具名成员（m_prob00[8]/m_prob20[3]/+0x2c..34 三项/
// m_w38[5]/m_w4c[4]/m_w5c[4]，合计恰 0x6c）。

#include "ScriptLeafA2/importUpgradeTable.inc"


#include "ScriptLeafA2/clear_1.inc"


#include "ScriptLeafA2/clear_2.inc"


#include "ScriptLeafA2/GetMedalPrice.inc"

