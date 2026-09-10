// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ServiceRestrictManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— RestrictCategory + ServiceRestrictManager
// 权威实现（原 GlobalData.cpp 本地类迁移合并；报告 docs/class_func_reports/
// ServiceRestrictManager/）。
//   initialize 0x816e2b2 / savePolicy 0x816e8ce / loadPolicy 0x816e7d6 /
//   isRestricted(CUser*,Enum,int) 0x816e6b8 / isRestricted(Enum,int) 0x816e75e /
//   ctor 0x816e272。
// 策略类 InterfaceServicePolicy 的 vtable 8 槽（D1/D0 + 6 虚函数）与三个具体
// 策略类的具名符号见 RestrictPolicy.h（ORIG nm/objdump 实证）。
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

// ---- NullPolicy（ORIG 0x0816d5aa 区，全部空/恒真）----
#include "ServiceRestrictManager/NullPolicy.inc"
}

#include "ServiceRestrictManager/dtor_NullPolicy.inc"
}

#include "ServiceRestrictManager/beginPolicy.inc"
}

#include "ServiceRestrictManager/endPolicy.inc"
}                             // ORIG 0816d660 空

#include "ServiceRestrictManager/isApply.inc"
}

#include "ServiceRestrictManager/isApply_1.inc"
}

#include "ServiceRestrictManager/printRestrict.inc"
}

#include "ServiceRestrictManager/getRestrictValue.inc"
}

// ---- AuthMobileRestrictPolicy（ORIG 0x0816d6ce 区，code=1，34 条限制）----
#include "ServiceRestrictManager/AuthMobileRestrictPolicy.inc"
}

#include "ServiceRestrictManager/dtor_AuthMobileRestrictPolicy.inc"
}

#include "ServiceRestrictManager/beginPolicy_1.inc"
}

#include "ServiceRestrictManager/endPolicy_1.inc"
}

#include "ServiceRestrictManager/isApply_2.inc"
}

#include "ServiceRestrictManager/isApply_3.inc"
}

#include "ServiceRestrictManager/printRestrict_1.inc"
}

#include "ServiceRestrictManager/getRestrictValue_1.inc"
}

// ---- CreateCharacterRestrictPolicy（ORIG 0x0816ddd2 区，code=2，3 条限制）----
#include "ServiceRestrictManager/CreateCharacterRestrictPolicy.inc"
}

#include "ServiceRestrictManager/dtor_CreateCharacterRestrictPolicy.inc"
}

#include "ServiceRestrictManager/beginPolicy_2.inc"
}

#include "ServiceRestrictManager/endPolicy_2.inc"
}

#include "ServiceRestrictManager/isApply_4.inc"
}

#include "ServiceRestrictManager/isApply_5.inc"
}

#include "ServiceRestrictManager/printRestrict_2.inc"
}

#include "ServiceRestrictManager/getRestrictValue_2.inc"
}

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----
#include "ServiceRestrictManager/ServiceRestrictManager.inc"
}

// ORIG 0x0816e2b2 旁 D1：仅 call map::~map（不 delete 策略对象，ORIG 原样）。
#include "ServiceRestrictManager/dtor_ServiceRestrictManager.inc"
}

// ORIG 0x0816e2b2：channel_no==1 → 置 +0x19=1；创建 4 个策略对象注册进 map。
#include "ServiceRestrictManager/initialize.inc"
}

// ORIG 0x0816e8ce：+0x19==1 时遍历 map，每条策略经 SigSaveRestrictPolicy 流序列化
// 后入 DB 队列。策略 save() 为虚调用（策略数据未还原，推断）。
#include "ServiceRestrictManager/savePolicy.inc"
}

// ORIG 0x0816e7d6：SigLoadRestrictPolicy 请求（internal_stream(1,-1)）入 DB 队列。
#include "ServiceRestrictManager/loadPolicy.inc"
}

// ORIG 0x0816e6b8：user 空→false；已开 Garena 手机认证→false；map 无该类别→true；
// 否则调策略虚函数（vtable+0x14）。
#include "ServiceRestrictManager/isRestricted.inc"
}

// ORIG 0x0816e75e（WarField 用）：map 无该类别→true；否则策略虚调用。
#include "ServiceRestrictManager/isRestricted_1.inc"
}

// ORIG 0x0816e54a：find 命中则策略 beginPolicy(str)；随后 printPolicy + updatePolicy。
#include "ServiceRestrictManager/beginPolicy_3.inc"
}

// ORIG 0x0816e5de：find 命中则策略 endPolicy()（虚槽 +0xc）。
#include "ServiceRestrictManager/endPolicy_3.inc"
}

// ORIG 0x0816e648：map 无该类别→true；否则策略 isApply()（虚槽 +0x10）。
#include "ServiceRestrictManager/isApply_6.inc"
}

// ORIG 0x0816ea6a：trace 头 + 命中则策略 printRestrict()（虚槽 +0x18），
// 未命中打 End 行。
#include "ServiceRestrictManager/printPolicy.inc"
}

// ORIG 0x0816eb62：find 命中则经 SigSaveRestrictPolicy 流把策略当前值写回 DB。
#include "ServiceRestrictManager/updatePolicy.inc"
}

// ORIG 0x0816e0ac：static 成员（P245/P281：首参 0x8(%ebp)=sret、ret $0x4、全程无 this），
// 返回 std::string（sret）。拼接 Mobile(0..0x21) 与 Character(0..2) 限制串；
// enumToString 的 << 与 ")" 各为独立语句（ORIG 重新 lea oss，非链式复用 %eax）。
#include "ServiceRestrictManager/serviceRestrictStringQuery.inc"
}
