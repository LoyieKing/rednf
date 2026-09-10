// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AvatarFixedHiddenOptionServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarFixedHiddenOptionServer 实现（独立 TU）。
// 依据 docs/class_func_reports/AvatarFixedHiddenOptionServer/、
// ARAD__SCRIPT__AvatarFixedHiddenOption/（含 parseAbility.md 全量反编译）与 ORIG 反汇编转写。
#include "AvatarFixedHiddenOptionServer.h"

#include <cstdio>
#include <map>

#include "CEnvironment.h"    // less_TCHAR / SpinLock
#include "GameTypes.h"       // cMyTrace
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanFloat

// ---- 布局断言（ORIG elf32-i386 反汇编实证）----
typedef char avatar_hidden_ability_variation_size_assert[
    sizeof(STAvatarHiddenAbilityVariation) == 0x14 ? 1 : -1];
typedef char avatar_fixed_hidden_option_size_assert[
    sizeof(ARAD::SCRIPT::AvatarFixedHiddenOption) == 0x4c ? 1 : -1];
typedef char avatar_fixed_hidden_option_server_size_assert[
    sizeof(AvatarFixedHiddenOptionServer) == 0x98 ? 1 : -1];

#include "AvatarFixedHiddenOptionServer/AvatarFixedHiddenOptionServer.inc"
}

// ============================================================================
// STAvatarHiddenAbilityVariation —— ORIG ctor W 0x088b01e8 / 0x088b021a、dtor W 0x088b0aae
// ============================================================================
#include "AvatarFixedHiddenOptionServer/STAvatarHiddenAbilityVariation.inc"
}

#include "AvatarFixedHiddenOptionServer/STAvatarHiddenAbilityVariation_1.inc"
}


// ============================================================================
// ARAD::SCRIPT::AvatarFixedHiddenOption
// ============================================================================

// ORIG C1 0x088aecac：6 个 vector 顺序构造后调 clearScript()。
#include "AvatarFixedHiddenOptionServer/AvatarFixedHiddenOption.inc"
}

// ORIG D1 0x088aed5e：clearScript 后逆序析构 6 个 vector（成员析构自动完成）。
#include "AvatarFixedHiddenOptionServer/dtor_AvatarFixedHiddenOption.inc"
}

// ORIG 0x088b00e0：clear 每桶并 push_back 一个全 0 默认元素（种子保证 Init 的
// reserve(count-1) 非负，与 ORIG 一致）。
#include "AvatarFixedHiddenOptionServer/clearScript.inc"
}

// ORIG 0x088b0164：return *(int*)(this+0x48)
#include "AvatarFixedHiddenOptionServer/getMaterialItemIndex.inc"
}

// ORIG 0x088b0170：return *(int*)(this+0x48) == itemIndex
#include "AvatarFixedHiddenOptionServer/isMaterialItemIndex.inc"
}

// ORIG 0x088aef52 parseAbility：P333 首次建表状态机（21 项静态映射惰性注册）。
// switch(key) 每个 case 体前半是「!inited ⇒ 注册该项、无 break、fallthrough 到下一
// case」，后半是「inited ⇒ 正常动作 break」；未初始化时 key 取哨兵 0（case 0 与
// case 0x46 共享入口），注册链走完落入 default（此时 inited 仍为 false，其
// `if (inited) return false` 不触发），再由公共尾 leave + inited=true + goto 循环头
// 重查，第二次进入 switch 才按 map 查得的 key 执行正常动作；default 同时承担
// find 未命中（key==-1）的 false 返回。
// [DualArith 2026-09-09] gap 172→0（orig 977 / ours 977；基线 ours 805）。
// p167 修正版（call rel32 + nm 区间 + .plt 反查，唯一认读口径）：MISSING 全为 C2 变体、
// EXTRA 全为 C1 变体（pair/map/SpinLock/const_iterator/STAvatarHiddenAbilityVariation
// ctor），严格 1:1 镜像、call 总数 150/150 ⇒ 按 Main 2026-09-09 裁定② clone 别名对
// 豁免，判 CLEAR、不排期。旧版 p167 / call_multiset_diff.py 读数已弃用（本件无旧版清单）。
#include "AvatarFixedHiddenOptionServer/parseAbility.inc"
}

// ORIG 0x088aedce loadScript。控制流忠实转写：外层 ScanType 失败即返回 0；
// "[upper]" / "[rare]" 分支内先 ScanType 再 parseAbility，parse 未命中回落外层；
// "[mystic circle]" 读一个整数到材料索引。打开脚本失败返回 -1。
#include "AvatarFixedHiddenOptionServer/loadScript.inc"
}

// ============================================================================
// AvatarFixedHiddenOptionServer —— ORIG C1 0x0817e02c / D1 0x0817e0e6 /
// Load 0x0817e154 / Destroy 0x0817e1e2 / Init 0x0817e3e8
// ============================================================================

#include "AvatarFixedHiddenOptionServer/dtor_AvatarFixedHiddenOptionServer.inc"
}

// ORIG 0x0817e154：cMyTrace(__PRETTY_FUNCTION__,0x1a,9,true,false) +
// "\t- Loading Avatar Hidden Option - "；loadScript==0 且 Init()!=0 才算成功。
#include "AvatarFixedHiddenOptionServer/Load.inc"
}

// ORIG 0x0817e1e2：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
#include "AvatarFixedHiddenOptionServer/Destroy.inc"
}

// ORIG 0x0817e3e8：6 个桶各自把下标 1..count-1 的 m_nRate（元素 +0x10）累加成前缀和
// push 入 m_cumulativeRate[i]，之前 reserve(count-1)。恒返回 1。
// [b28d-56] 返回类型 int→bool：ORIG Load 0x0817e1bd 对 Init 的调用点形为
// `call; xor $1,%eax; test %al,%al; je`（bool callee 特征，P292/P248 档①）；
// int callee 恒 `test %eax; sete; test`。mangled 不含返回类型，符号/ABI 不变。
#include "AvatarFixedHiddenOptionServer/Init.inc"
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetSkillIndex(rarity)       ORIG T 0x0817e1ec（GetSkillIndex.md 转写）
//   GetSkillIndex(rarity, idx)  ORIG T 0x0817e23e（GetSkillIndex_0817e23e.md）
//   GetRandomInput              ORIG T 0x0817e334（GetRandomInput.md）
//   IsVaildSkillIndex           ORIG T 0x0817e392（IsVaildSkillIndex.md）
//   CheckCondition              ORIG T 0x0817e4c6（CheckCondition.md 全文转写）
//   ChangeHiddenOption          ORIG T 0x0817e8d6（ChangeHiddenOption.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item
#include "LogManager.h"
#include "GlobalData.h"
#include "MoreTypes.h"          // arad::SigAvatarHiddenOptionChange / SigAradAvatarHiddenOptionSave
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "Arad_InternalDispatcher.h"   // DISPATCHER::make_internal_stream_jpn

#include "AvatarFixedHiddenOptionServer/GetRandomInput.inc"
}

#include "AvatarFixedHiddenOptionServer/IsVaildSkillIndex.inc"
}

#include "AvatarFixedHiddenOptionServer/GetSkillIndex.inc"
}

#include "AvatarFixedHiddenOptionServer/GetSkillIndex_1.inc"
}

#include "AvatarFixedHiddenOptionServer/CheckCondition.inc"
}

#include "AvatarFixedHiddenOptionServer/ChangeHiddenOption.inc"
}
// ============================================================================
// UnitTest / Test1 —— ORIG UnitTest T 0x0817ecd8、Test1 t 0x0817ec67
// （docs/class_func_reports/AvatarFixedHiddenOptionServer/UnitTest.md、
//  docs/class_func_reports/_lt_global_gt_/Test1.md 全文转写）
// ============================================================================

// ORIG 0x0817ec67（文件内 static，_ZL 局部符号）：调 GetSkillIndex(rarity=3, input)
// 并把结果与期望值 printf 比对。
static void Test1(AvatarFixedHiddenOptionServer* server, int input, int expect)
{
    int output = server->GetSkillIndex((ENUM_RARITY)3, input);
    if (expect != output) {
        printf("input==%d, output!=%d/%d\n", input, output, expect);
    } else {
        printf("input==%d, output==%d/%d\n", input, output, expect);
    }
}

// ORIG 0x0817ecd8：非空 server 时依次跑 6 组 (input, expect) 用例。
#include "AvatarFixedHiddenOptionServer/UnitTest.inc"
}
