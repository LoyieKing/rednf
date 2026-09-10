// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ARAD/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD 命名空间类实现（批3：从 GameStubs.cpp 迁移，批4：Arad 脚本生命周期）。
// 依据 docs/class_func_reports/ARAD__Arad_DataManager/* + 各子对象报告 + ORIG elf 反汇编。
#include "AradServerStateMessage.h"
#include "Arad_EventPeriodDataManager.h"
#include "Arad_StatisticsEventItemList.h"
#include "AvatarFixedHiddenOptionServer.h"  // #1 注册项（本 TU 新建）
#include "AvatarRechargeServer.h"            // #2 注册项
#include "EmblemCompoundServer.h"            // #3 注册项（FixAradEmblemConvert 新建 TU）
#include "AvatarConvertServer.h"             // #4 注册项（FixAradEmblemConvert 新建 TU）
#include "LevelupSupportEventManger.h"      // #6 注册项
#include "CreateDnfEventSciprt.h"           // #8 注册项
#include "ARAD_FN_DeleteGameScript.h"   // FN_DeleteGameScript 唯一类声明（Arad_DataManager.h 不含）
#include "Arad_Script.h"
#include "CUser.h"          // CerashopAddRestrict::Manager（Init 注册）
#include "PacketGuard.h"    // notifyOpenMessageDialog 组包
#include "LogManager.h"
#include "DNFLexWrapper.h"  // parsing：limited_map 段 Load(g_dnfLex.getInputStreamBuffer())
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ARAD
{

// ============================================================================
// ScriptInterface（虚基接口）
// （ORIG nm 无 ScriptInterface 析构符号：析构隐式平凡，不提供定义——
//  见 Arad_DataManager.h ScriptInterface 注释。）

// ============================================================================
// ScriptManager（ORIG LoadScript 0x81a11dc / DestroyScript 0x81a12f8 /
// RegistScript 0x81a136c / Init 0x81a13f6）
// ============================================================================
// b28d-solo-ARAD verdict=IDENTICAL/AE：P13 end 缓存 + P221 全表达式临时链
//（`cMyTrace(...)("Fail"/"Success")`）——具名 cMyTrace 时 4.4 把聚合对象排到低槽
//（fail -0x30/success -0x20、标量 it -0xc/e -0x10），临时链后槽位与 ORIG 逐一对齐
//（it -0x2c / e -0x30 / fail -0x28 / success -0x18）。残余 diff 全为 .o 未 relink
// 的 call/jmp 与 .rodata 重定位显示（§6.3 假阳性）。
#include "ARAD/LoadScript.inc"
}

#include "ARAD/DestroyScript.inc"
}

#include "ARAD/RegistScript.inc"
}

#include "ARAD/Init.inc"
}

#include "ARAD/Arad_InterfaceGameScript.inc"
}

#include "ARAD/dtor_Arad_InterfaceGameScript.inc"

#include "ARAD/scriptDataName.inc"
}

}  // namespace ARAD

// ============================================================================
// 6 个脚本子对象位于【全局命名空间】（ORIG mangled _ZN13ExceptionItem... 等，
// 无 ARAD:: 前缀）——见 Arad_DataManager.h 说明。
// ============================================================================
// Arad_ScriptLoader（open/parsing 见 ScriptExtras.cpp）
// ============================================================================

// ============================================================================
// ExceptionItem（+0x1c）—— ORIG ctor 0x8188b50 / dtor 0x8188b7a / parsing 0x81a1fce /
// find_index 0x81a210c。解析 "material_exception_item" 节，行读 material_exception_item_index。
// ============================================================================
#include "ARAD/ExceptionItem.inc"
#include "ARAD/dtor_ExceptionItem.inc"
#include "ARAD/parsing.inc"
}

#include "ARAD/find_index.inc"
}

// ============================================================================
// PurchaseLimitItem（+0x38）—— ORIG ctor 0x8188bae / dtor 0x8188bd8 / parsing 0x81a216e /
// find 0x81a2310。解析 "purchase_limit_item" 节，读 Item_Index/purchase_Type/puchase_limit_count。
// ============================================================================
#include "ARAD/PurchaseLimitItem.inc"
#include "ARAD/dtor_PurchaseLimitItem.inc"
#include "ARAD/parsing_1.inc"
}

#include "ARAD/find.inc"
}

// ============================================================================
// AradBonusItem（+0x54）—— ORIG ctor 0x8188c0c / dtor 0x8188c36 / parsing 0x81a238a /
// find 0x81a252c。解析 "bonus_item" 节，读 item_id/bonus_type/bonus_value。
// ============================================================================
#include "ARAD/AradBonusItem.inc"
#include "ARAD/dtor_AradBonusItem.inc"
#include "ARAD/parsing_2.inc"
}

// b28d-solo-ARAD verdict=IDENTICAL/AE：P96/P270 非 const iterator（消 const_iterator
// 转换构造 call）+ P0 具名 key（ORIG 0x81a2535 `mov %eax,-0x1c`）+ P247 具名 Data tmp
// 后结构整体赋值（ORIG 0x81a2590 逐字段读节点到 -0x14/-0x10/-0xc，再 memcpy(out,&tmp,10)
// 带自赋值检查）。槽位 key -0x1c / find -0x20 / end -0x18 / tmp -0x14 全对齐。
#include "ARAD/find_1.inc"
}

// ============================================================================
// Arad_Mileage_Data（+0x70）—— ORIG ctor 0x8188cc0 / dtor 0x8188cea / parsing 0x81a3416 /
// find 0x81a3668 / default_value 0x81a36f0。解析 "default"（percent）与 "item"（ipg_no/percent）。
// ============================================================================
// [b22] ORIG ctor 0x08188cc0 不写 +4（m_defaultPercent）：仅基类 ctor+vptr+vector ctor，
// 16→14 条。默认值清零发生在 parsing()（`m_defaultPercent = 0` 行），ctor 侧删初始化。
#include "ARAD/Arad_Mileage_Data.inc"
#include "ARAD/dtor_Arad_Mileage_Data.inc"
#include "ARAD/parsing_3.inc"
}

#include "ARAD/find_2.inc"
}

#include "ARAD/default_value.inc"
}

// ============================================================================
// Arad_OnePlusCardEvent_ItemList（+0x84）—— ORIG ctor 0x8188d0a / dtor 0x8188d6a /
// parsing 0x81a36fc / find_item x2 / is_limited_map。解析 "item"（web_id/item_index）与
// "limited_map"（index）。
// ============================================================================
// ORIG ctor @0x08188d0a / dtor @0x08188d6a（vector 成员自动构造/析构）。
#include "ARAD/Arad_OnePlusCardEvent_ItemList.inc"
}

#include "ARAD/dtor_Arad_OnePlusCardEvent_ItemList.inc"
}

#include "ARAD/parsing_4.inc"
}

#include "ARAD/find_item.inc"
}

#include "ARAD/find_item_1.inc"
}

#include "ARAD/is_limited_map.inc"
}

// ============================================================================
// Arad_VillageAttackRewardItem（+0xa0）—— ORIG ctor 0x8188dbe / dtor 0x8188e1e /
// parsing 0x81a3b48 / find_reward_item 0x81a3e3c / find_ranking_reward_item 0x81a3ef0。
// 解析 "reward_item"（win_point/item_index/item_count）与 "reward_ranker"
// （item_index/item_count/item_index2/item_count2）。
// ============================================================================
#include "ARAD/Arad_VillageAttackRewardItem.inc"
#include "ARAD/dtor_Arad_VillageAttackRewardItem.inc"

#include "ARAD/parsing_5.inc"
}

// b28d-solo-ARAD verdict=IDENTICAL/AE：ORIG 0x81a3e3c 不是循环而是
// `std::find(begin, end, out)`——Ghidra C 第四实参即 out（0x10(%ebp)），
// Data::operator== 仅比 winPoint（Arad_DataManager.h:357），故 winPoint 形参未使用
//（ORIG 全程无 0xc(%ebp) 读取，原码遗留，P171 照抄）；命中后 `out = *it`
//（ORIG 0x81a3ed1 三对 dword mov = 结构整体赋值 P247）。
#include "ARAD/find_reward_item.inc"
}

#include "ARAD/find_ranking_reward_item.inc"
}

// ============================================================================
// Arad_DataManager（ORIG ctor 0x8188030；位于 ARAD 命名空间）
// ============================================================================
namespace ARAD
{
#include "ARAD/Arad_DataManager.inc"
}

#include "ARAD/dtor_Arad_DataManager.inc"
}

#include "ARAD/Load.inc"
}

#include "ARAD/Destroy.inc"
}

// b28d-solo-ARAD verdict=IDENTICAL/AE：P118/P139 第一参用 __FUNCTION__（ORIG 局部静态符号
// _ZZN4ARAD16Arad_DataManager12initAradDataEvE12__FUNCTION__）；P221 全部
// cMyTrace 改「构造即调用」全表达式临时链复现 ORIG 递减槽序（-0x100…-0x30）；
// P197③ ep/openResult 用「声明=0 + 独立赋值」两句式（ORIG 0x08188304 -0x20、
// 0x08188691 -0x1c 各一条 movl $0）；单基类指针变量 script 复用 -0x20 槽（P211 档③
// 派生→基类赋值中转）；P225 5 处 open 检查改 `while(openResult != 0){…return}` 复现
// `cmpl $0,槽; setne; test; je`；trace2(0x59) 标志位按 ORIG 为 (true,false)。
// 345=345 指令全等，clean 归一化 0 残差。
#include "ARAD/initAradData.inc"
}

#include "ARAD/destroyAradData.inc"
}

#include "ARAD/__insertGameScript.inc"
}

#include "ARAD/findGameScript.inc"
}

// Singleton<T> 模板定义已在 Arad_DataManager.h 提供。ORIG 对这两个实例只发射
// { Singleton() C1/C2, Get() }（_ZN4ARAD9SingletonINS_16Arad_DataManagerE... /
// ...INS_13ScriptManagerE...），**没有** GetPtr ⇒ 整类显式实例化会连带发出 ORIG
// 不存在的 W _ZN4ARAD9SingletonI...E6GetPtrEv，故降级为成员级显式实例化。
// 唯一定义点在 Game/Arad_Singletons.cpp；此处仅补发本 TU 用到的两个成员。
template Singleton<Arad_DataManager>::Singleton();
template Arad_DataManager* Singleton<Arad_DataManager>::Get();
template Singleton<ScriptManager>::Singleton();
template ScriptManager* Singleton<ScriptManager>::Get();

// b28d-solo-ARAD verdict=IDENTICAL/AE：P248 档①（callee 返回类型旋钮）——返回类型
// int→bool 复现 ORIG 0x0819798d `call CUser::Send; mov %eax,%ebx`（4.4 对 int 返回恒
// `movzbl %al,%ebx`；bool 返回直出全宽 mov，探针 /tmp/probe_arad2.cpp f_bool）。
// 返回类型不入 mangled；调用点（Arad_ServerStateManager/GameWorld/Inter_LoadEtc）均
// 作语句调用忽略返回值，语义等价（0=失败，非0=成功）。
// p167：ORIG 0x08197963 调 put_binary(char*,int)（Pc）⇒ cast 改 (char*)（原
// (const char*) 选 PKc，差集 1/1）；改后 0/0 CLEAR。
#include "ARAD/notifyOpenMessageDialog.inc"
}

}  // namespace ARAD
