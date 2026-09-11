// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ServiceRestrictManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ServiceRestrictManager.h"
#include "MsgQueueMgr.h"
#include "RestrictPolicy.h"           // NullPolicy/AuthMobileRestrictPolicy/CreateCharacterRestrictPolicy 声明
#include "MobileRestrict.h"           // MobileRestrict::enumToString（T @0x0816d463）
#include "MobileRestrict.h"           // MobileRestrict::enumToString（T @0x0816d463）
#include "CharacterRestrict.h"        // CharacterRestrict::enumToString（T @0x0816d472）
#include "SecurityRestrict.h"         // SecurityRestrict::enumToString（T @0x0816d481）

#include "CEnvironment.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "TaiwanInternal.h"
#include "TaiwanQueries.h"  // LinkClosureD：Taiwan::SigLoadRestrictPolicy 权威
                            // （MoreTypes.h 0x100 占位已删）

#include <new>
#include <sstream>

// LinkClosureD：局部 Taiwan::GarenaAuthData 镜像已删（与 TaiwanQueries.h 权威
// 定义 ODR 冲突；getMobileAuth 权威实现在 TaiwanQueries.cpp）。

class CStreamGuard;

// ORIG static _ZL19setPolicyApplyValue...（0x0816d490）：策略字符串 '0'/'1' 解析
// 写入 policy vector（下标 1 起）；非法字符记日志置 0；越界即停。与
// SecurityRestrictPolicy.cpp 中副本同源（ORIG 同文件单定义，跨 TU 各一份）。
static void setPolicyApplyValue(RestrictCategory::Enum code, ServicePolicy& policy,
                                std::string const& str)
{
    if (str.empty())
    {
        return;
    }
    unsigned int idx = 1;
    unsigned int size = policy.size();
    std::string::const_iterator it = str.begin();
    while (it != str.end())
    {
        int v = static_cast<signed char>(*it) - 0x30;
        if (v > 1 || v < 0)
        {
            LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                                  "void setPolicyApplyValue(RestrictCategory::Enum, ServicePolicy&, std::string const&)",
                                  0x22,
                                  "[Taiwan, Restrict] Invalid server value. restrict:%d,%d,%d",
                                  code, idx, v);
            v = 0;
        }
        if (idx >= size)
        {
            break;
        }
        policy[idx] = static_cast<short>(v);
        ++it;
        ++idx;
    }
}

// ==================== 三个具体策略（ORIG 全局具名类，具名符号见 RestrictPolicy.h） ====================

#include "ServiceRestrictManager/NullPolicy.inc"


#include "ServiceRestrictManager/dtor_NullPolicy.inc"


#include "ServiceRestrictManager/beginPolicy.inc"


#include "ServiceRestrictManager/endPolicy.inc"
                             // ORIG 0816d660 空

#include "ServiceRestrictManager/isApply.inc"


#include "ServiceRestrictManager/isApply_1.inc"


#include "ServiceRestrictManager/printRestrict.inc"


#include "ServiceRestrictManager/getRestrictValue.inc"


#include "ServiceRestrictManager/AuthMobileRestrictPolicy.inc"


#include "ServiceRestrictManager/dtor_AuthMobileRestrictPolicy.inc"


#include "ServiceRestrictManager/beginPolicy_1.inc"


#include "ServiceRestrictManager/endPolicy_1.inc"


#include "ServiceRestrictManager/isApply_2.inc"


#include "ServiceRestrictManager/isApply_3.inc"


#include "ServiceRestrictManager/printRestrict_1.inc"


#include "ServiceRestrictManager/getRestrictValue_1.inc"


#include "ServiceRestrictManager/CreateCharacterRestrictPolicy.inc"


#include "ServiceRestrictManager/dtor_CreateCharacterRestrictPolicy.inc"


#include "ServiceRestrictManager/beginPolicy_2.inc"


#include "ServiceRestrictManager/endPolicy_2.inc"


#include "ServiceRestrictManager/isApply_4.inc"


#include "ServiceRestrictManager/isApply_5.inc"


#include "ServiceRestrictManager/printRestrict_2.inc"


#include "ServiceRestrictManager/getRestrictValue_2.inc"


// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----

#include "ServiceRestrictManager/ServiceRestrictManager.inc"


#include "ServiceRestrictManager/dtor_ServiceRestrictManager.inc"


#include "ServiceRestrictManager/initialize.inc"


#include "ServiceRestrictManager/savePolicy.inc"


#include "ServiceRestrictManager/loadPolicy.inc"


#include "ServiceRestrictManager/isRestricted.inc"


#include "ServiceRestrictManager/isRestricted_1.inc"


#include "ServiceRestrictManager/beginPolicy_3.inc"


#include "ServiceRestrictManager/endPolicy_3.inc"


#include "ServiceRestrictManager/isApply_6.inc"


#include "ServiceRestrictManager/printPolicy.inc"


#include "ServiceRestrictManager/updatePolicy.inc"


#include "ServiceRestrictManager/serviceRestrictStringQuery.inc"

