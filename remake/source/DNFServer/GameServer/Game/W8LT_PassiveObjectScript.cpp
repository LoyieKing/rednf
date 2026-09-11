// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8LT_PassiveObjectScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "W8LT_PassiveObjectScript.h"
#include "STMapScript.h"      // ENUM_DRAWLAYER/MAP_FOLLOW_TYPE/PASSAGETYPE + 名字函数声明
#include "GameEnums.h"       // ENUM_CATEGORY
std::string getObjectCategoryString(ENUM_CATEGORY cat);   // T 0x08a3ca81（ScriptDepsA.cpp）
#include "STPassiveObjectAdditionalParticleInfo.h"
#include "RDARScriptStringManager.h"  // getTagByName（[name] 段）
#include "SyncScriptDeps.h"           // toTString
#include <stdio.h>

// ---- TU-local 视图：ORIG export 0x08a2b44b 以「对象首地址 + int 索引 (k+4)」
//      访问 +0x10..+0x18 三 int（P30 两步寻址实证：`add $0x4,%edx;
//      mov (%eax,%edx,4),%eax`）。仅复现该寻址形，不改变布局/不读写前四项。
struct STPassiveObjectAdditionalParticleInfoIntView
{
    int m_ints[7];   // +0x00..+0x18（[4]/[5]/[6] 即 m_i10/m_i14/m_i18）
};

// ---- 跨 TU 依赖声明 ----
extern char g_szPassiveObjectScriptBaseDirectory[0x100];   // InitScriptProducers_4.cpp（B 0x094f6ac0）
#include "W8LT_PassiveObjectScript/getScriptFileFullPath.inc"
                           const char* dir, const char* name);   // T 0x088bba4e（STMonsterScript.cpp）
#include "W8LT_PassiveObjectScript/convertBinaryToText.inc"
extern RDARScriptStringManager g_scriptStringManager_;            // RDARScriptStringManager.cpp
#include "W8LT_PassiveObjectScript/WarningDistance.inc"


#include "W8LT_PassiveObjectScript/WarningDistance_1.inc"


#include "W8LT_PassiveObjectScript/PassiveObjGeneratorObject.inc"


#include "W8LT_PassiveObjectScript/PassiveObjGeneratorPattern.inc"


#include "W8LT_PassiveObjectScript/PassiveObjGeneratorSequence.inc"


#include "W8LT_PassiveObjectScript/SpeedCtrlPassiveObjInfo.inc"


#include "W8LT_PassiveObjectScript/PassiveIObjSniperInfo.inc"


#include "W8LT_PassiveObjectScript/clear.inc"


#include "W8LT_PassiveObjectScript/STPassiveObjectDestroy.inc"


#include "W8LT_PassiveObjectScript/dtor_STPassiveObjectDestroy.inc"


#include "W8LT_PassiveObjectScript/clear_1.inc"



#include "W8LT_PassiveObjectScript/STPassiveObjectAddEffect.inc"


#include "W8LT_PassiveObjectScript/dtor_STPassiveObjectAddEffect.inc"


#include "W8LT_PassiveObjectScript/clear_2.inc"

#include "W8LT_PassiveObjectScript/RandomItemFairy.inc"


#include "W8LT_PassiveObjectScript/clear_3.inc"


#include "W8LT_PassiveObjectScript/STPassiveObjectScript.inc"


#include "W8LT_PassiveObjectScript/clear_4.inc"

#include "W8LT_PassiveObjectScript/exportPassiveObjectScript.inc"

