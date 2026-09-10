// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 APSystemOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// APSystemOnlyOrig.cpp —— Wave5「仅ORIG」APSystem 批（独占 TU；CMake 由 Main
// 统一登记）。
//
// worklist：/tmp/only_orig/APSystem.txt（49 mangled，全部逐符号 DONE）。
// 报告目录：docs/class_func_reports/APSystem__{DB_LoadActionPoint,
//   DB_LoadRewardMedal, DB_UpdateActionPoint, DB_UpdateRewardMedal,
//   CActionPointManager, Inter_LoadRewardMedal}/（各 md 的 ## 汇编 + ## 反编译 C
//   全文已读后转写）。无独立报告者按 objdump 逐指令转写并标注。
//
// 额外实现（Main 授权，唯一属主=本 TU）：
//   * T 0x086b2102 uncompress_zip / T 0x086b201f compress_zip（zlib 包装，
//     objdump 全文转写）
//   * static APSystem::CActionPointManager::_IsGMOn / _gmWeekday 定义
//   * POD 生命周期：_MedalRewardItem / _TodayRewardItem / _SIG_LOAD_ACTION_POINT
// DBDispatcher 本体：唯一定义在 DB_Login.cpp（bool dispatch 版，AltarDB 定稿）；
// 本 TU 仅经 Game/DBDispatcher.h 声明使用。
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "CUser.h"                    // 必须先于 MoreTypes.h：避免其 namespace SIG_GIVE_GROWTH_CREATURE_EVENT 抢占 CUser.h 的全局 struct 前置
#include "APSystemTypes.h"            // _SIG_LOAD_ACTION_POINT/_ActionPoint/zip 声明
#include "DB_AccountCargoSync.h"      // APSystem::DB_UpdateActionPoint 权威声明
#include "APSystemDbDispatchers.h"    // 本批 DB 类权威声明(内含 MoreTypes.h)
#include "CUserProc.h"                // CUserProc::SendActionClear / _ActionGroupIndex
#include "GameEnums.h"                // ENUM_PACKETCLASS
#include "MySQL.h"
#include "DBMgr.h"
#include "InterDispatcher.h"         // APSystem::Inter_LoadRewardMedal 类声明
#include "GlobalData.h"               // s_db_mgr/s_stream_pool/s_msgq_mgr/s_systemTime_
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CEnvironment.h"             // G_CEnvironment / ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "Inven_Item.h"
#include "LogManager.h"               // compress_zip 错误分支
#include "CDataManager.h"             // G_CDataManager/find_item/GetActionPointEtcParameter
#include "DNFFunctionLib.h"         // DNFFLib::get_rand_int（ORIG 0x08108b06）

// ---- 本 TU 局部原型（定义分属 GameUtils.cpp/CItemList 域，签名以 ORIG 为准）----
#include "APSystemOnlyOrig/getDailyInitializeTime.inc"
#include "APSystemOnlyOrig/__GetCurrentDNFDate.inc"

namespace APSystem
{

// ---- 全局压缩缓冲（Select/Update 使用；ORIG 数据段同名 B 数组语义等价）----
char g_szZipAP_CLEAR_STATE[0x800];
char g_szStrAP_CLEAR_STATE[0x800];

// ---- ORIG 域常量（nm：_ZN8APSystemL…E，r 段；值自 ORIG rodata 读取）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int ConstMaxZipAP_CLEAR_STATE = 0x800;    // 0x08b5e0ec (2048)
static const int ConstMaxStrAP_CLEAR_STATE = 0x800;    // 0x08b5e0f0 (2048)
// 指针型分隔符常量（ORIG 值 = 指向 rodata 串；本侧指向等价字符串字面量）：
static const char* ConstDelimterPoint = ",";           // 0x08b5e0fc → ",\0vector::_M_insert_aux" 区
static const char* ConstDelimterAction = "|";          // 0x08b5e0f8 → 0x7c('|')
static const char* ConstDelimterCondition = ",";       // 0x08af2e94 → 0x2c(',')
static const int ConstClientClearAction[5] = {2, 5, 0x29, 0x2b, 0x2c};  // 0x08af2e9c (0x14 字节)

// ORIG 域常量（nm：_ZN8APSystemL19ConstOCC_DATELengthE，r 段；"YYYY-MM-DD" 含 NUL）
static const int ConstOCC_DATELength = 11;

bool CActionPointManager::_IsGMOn;                                // ORIG B 0x093ff07c
CActionPointManager::GmWeekdayCtl CActionPointManager::_gmWeekday;  // ORIG D 0x0937008c

}  // namespace APSystem

// ============================================================================
// zlib 包装（本批唯一属主；objdump 全文转写）
// ============================================================================

extern "C" int compress2(unsigned char* dest, unsigned long* destLen,
                         const unsigned char* source, unsigned long sourceLen,
                         int level);
extern "C" int uncompress(unsigned char* dest, unsigned long* destLen,
                          const unsigned char* source, unsigned long sourceLen);

// ----------------------------------------------------------------------------
// uncompress_zip —— ORIG T 0x086b2102（range [0x86b2102,0x86b216d]）：
//   if (inLen==0) { *outLen=0; return true; }
//   raw = *(uint*)in & 0x3fffffff;
//   ret = uncompress(out,&raw,in+4,inLen);
//   *outLen = raw(无条件写回);  return ret==0;
// ----------------------------------------------------------------------------
#include "APSystemOnlyOrig/uncompress_zip.inc"
}

// ----------------------------------------------------------------------------
//   bound = len + len/5 + 12（ORIG imul $0x66666667+sar 序列 = gcc 除 5 magic；
//            即 zlib compressBound 的 1.2 倍形式，非 1.1 倍）；
//   zerr = compress2(out+4,&bound,in,len,-1)；失败返回 false；
//   成功但 *outLen（入参容量）< bound → 返回 false（ORIG 防御性容量检查）；
//   成功：out 头 dword = len & 0x3fffffff（ORIG and 截断 30 位），outLen = bound+4。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82（已按 P0 修）：ORIG 0x086b201f；补回 out/inLen/bound/dst4/ret
// 五个具名局部（ORIG 有槽往返）后 ndiff 35→14，残差为跨版本槽号分配（N82），
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；66=66；残差仅 <A>/<T> 归一）。
// p167 CLEAR（2/2），语义闭合。
#include "APSystemOnlyOrig/compress_zip.inc"
}

namespace APSystem
{

// ============================================================================
// POD 生命周期（objdump 转写）
// ============================================================================

// _MedalRewardItem —— W 0x08895712 C1/C2 同体：memset(this,0,0xa)（尾 pad 不清）
#include "APSystemOnlyOrig/_MedalRewardItem.inc"
}

// _TodayRewardItem（顶层）—— W 0x08895736 C1/C2 同体：memset(this,0,0xe)；D1/D2 空体
#include "APSystemOnlyOrig/_TodayRewardItem.inc"
}
#include "APSystemOnlyOrig/dtor__TodayRewardItem.inc"
#include "APSystemOnlyOrig/_SIG_LOAD_ACTION_POINT.inc"
}

// ============================================================================
// DB_LoadActionPoint
// ============================================================================

// dispatch —— 报告 dispatch.md 全文转写。ORIG T 0x08123b10。
#include "APSystemOnlyOrig/dispatch.inc"
}

// ParseActionClearState —— 报告全文转写。ORIG T 0x08123f50。this 未使用。
// [b28d-27] DIFF N82：ORIG 0x08123f50；p167 CLEAR（10/10），残差为帧/槽分配，
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；142=142；三独立列指针 col1/col2/col3 + day 字节局部 -0x9 还原后全等）。
// 语义闭合。
#include "APSystemOnlyOrig/ParseActionClearState.inc"
}

// Select —— 报告全文转写。ORIG T 0x08123b9e。
// [b28d-27] DIFF N82/N89：ORIG 0x08123b9e；p167 CLEAR（21/21），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap -3→0**（本轮还原：列号具名局部 `col++` 后置自增、5 次 get_ubyte 复用同一 flag 槽 -0x39、col 后死局部 -0x14、apSum 直传 `&sig.m_apSum`、blobLen unsigned、尾部 `if(!Parse)return false; return true;`；245=245）。残差 = N93附（nRows `jne false岛` vs 4.4 `je+mov$0+jmp`）。p167 CLEAR 21/21。
// 实例与帧/槽分配，语义闭合。
#include "APSystemOnlyOrig/Select.inc"
}

// SendResult —— 报告全文转写。ORIG T 0x08124156。
#include "APSystemOnlyOrig/SendResult.inc"
}

// ctor/dtor —— objdump 0x0844dd7a / 0x0812544c 转写（基类链+置 vptr，无数据成员）
#include "APSystemOnlyOrig/DB_LoadActionPoint.inc"
#include "APSystemOnlyOrig/dtor_DB_LoadActionPoint.inc"
#include "APSystemOnlyOrig/MakeActionClearStateString.inc"
}

// Update —— 报告全文转写。ORIG T 0x08124386。
// [b28d-27] DIFF N82/N89：ORIG 0x08124386；p167 CLEAR（22/22），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap 0**（本轮还原 ORIG 0x08124472..d7 指令序：zipLen=0x400 → `cmp $0x7ff; jbe` → memset → compress_zip；旧版把检查放 compress 后、memset 放 zipLen 前 ⇒ 整段错位）。残差 = 实参装载时刻（ORIG 提前装 sig.m_apSum 到 %ebx）+ 寄存器分配（N7/N82）。p167 CLEAR 22/22。
// 实例与帧/槽分配，语义闭合。
#include "APSystemOnlyOrig/Update.inc"
}

// dispatch —— 报告全文转写。ORIG T 0x0812432a。
#include "APSystemOnlyOrig/dispatch_1.inc"
}

#include "APSystemOnlyOrig/DB_UpdateActionPoint.inc"
#include "APSystemOnlyOrig/dtor_DB_UpdateActionPoint.inc"
#include "APSystemOnlyOrig/makeRequest.inc"
}

// dispatch —— 报告全文转写。ORIG T 0x08124882。
#include "APSystemOnlyOrig/dispatch_2.inc"
}

// Select —— 报告全文转写。ORIG T 0x08124910。
// [b28d-27] DIFF N82/N89：ORIG 0x08124910；p167 CLEAR（18/18），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap -4→-1**（本轮还原：medalCount=size() 先于 minAp=99999、strToday/strYesterday 16 字节、get_uint 直传 &sig.m_apSum；165 vs 166）。残差 = N93附（nRows `jle` vs 4.4 `jg+mov$0+jmp`）。p167 CLEAR 18/18。
// 实例与帧/槽分配，语义闭合。
#include "APSystemOnlyOrig/Select_1.inc"
}

// SendResult —— 报告全文转写。ORIG T 0x08124b8e。
#include "APSystemOnlyOrig/SendResult_1.inc"
}

#include "APSystemOnlyOrig/DB_LoadRewardMedal.inc"
#include "APSystemOnlyOrig/dtor_DB_LoadRewardMedal.inc"
#include "APSystemOnlyOrig/makeRequest_1.inc"
}

// Update —— 报告汇编+反编译转写。ORIG T 0x08124cb2。
#include "APSystemOnlyOrig/Update_1.inc"
}

// dispatch —— 报告全文转写。ORIG T 0x08124c78。
#include "APSystemOnlyOrig/dispatch_3.inc"
}

#include "APSystemOnlyOrig/DB_UpdateRewardMedal.inc"
#include "APSystemOnlyOrig/dtor_DB_UpdateRewardMedal.inc"
#include "APSystemOnlyOrig/Set.inc"
}

// GM_Reset —— 报告汇编+反编译全文转写。ORIG T 0x08121d12。
// [b28d-27] DIFF N82：ORIG 0x08121d12；p167 CLEAR（1/1），残差为循环体数组寻址
// [b28d-solo-APSystemOnlyOrig] 判词：**N18a 封顶**（唯一残差 = ORIG `mov %edx,%ecx; neg %ecx` vs 4.4 `neg %edx; mov %edx,%ecx` 1 对；p167 CLEAR 1/1；槽位 -0x14/-0x10/-0xc 与帧 0x28 全等）。
// （两步 add vs 4 步 imul 链）+ 帧差，语义闭合。
// [b28d-round1-L24] **DIFF→AE**（55=55）：唯一残差 `mov %edx,%ecx; neg %ecx`
// （ORIG 0x08121d9d/0x08121d9f）靠**提升到 unsigned int 再取负**复现 ——
// `(unsigned char)(-(unsigned int)(signed char)x)`（探针 /tmp/probe_neg.cpp f6
// 逐条同形；f1..f5 恒产 `neg %edx; mov %edx,%ecx`，int 中介亦无效）。
#include "APSystemOnlyOrig/GM_Reset.inc"
}

// ClearActionOne —— 报告全文转写。ORIG T 0x08121f1c。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08121f1c：`GetActionPointEtcParameter(_IsGMOn)`
// （uchar 直传 bool 形参，ORIG 只 movzbl×2）->m_actionPointRefs 直读 +0x18，
// **非 const** find/end（_ZNSt3map…4findERS7_/3endEv）+ iterator→const_iterator
// 转换（C1ERKSt17_Rb_tree_iterator），end 侧表达式整段重写（P48 两次
// G_CDataManager/GetActionPointEtcParameter 调用）；旧版经 getActionPointRefs()
// [b28d-solo-APSystemOnlyOrig] 判词：**N18a 封顶**（唯一残差 = `mov;neg` 序 1 对；p167 CLEAR；非 const map find/end + `&action.m_actionNo` 直接取址 + 循环外缓存 size 还原后 145=145）。
// [b28d-round1-L24] 旧 N18a 判词作废：**DIFF→AE**（145=145）—— 同 GM_Reset，
// 取负写法提升到 `-(unsigned int)(signed char)x` 即复现 ORIG 0x0812207x
// `mov %edx,%ecx; neg %ecx`（探针 /tmp/probe_neg.cpp f6）。
// const getter ⇒ const 版 find/end（p167 MISSING 5）。
#include "APSystemOnlyOrig/ClearActionOne.inc"
}

// ClearAction —— 报告全文转写。ORIG T 0x08121dcc。
// [b28d-27] DIFF N82：ORIG 0x08121dcc；p167 CLEAR（3/3），残差为帧/槽分配，
// [b28d-solo-APSystemOnlyOrig] 判词：**N18b/P265 封顶**（残差 = ①字段 vs enum 形参比较 4.4 恒载寄存器 1 条；②enum 实参 int→unsigned-enum 转换两段中转 ×2；p167 CLEAR 3/3；帧 0x34/槽 -0xd/-0xc/汇聚变量全等）。
// [F2d-C05② 甄别] 比较对象为 enum 形参（非「两具名局部」）⇒ F2 不适用；A/B 实测删
//   `(int)group` cast 两态逐条同形（show_func_diff orig=107 our=111 gap=-4 ndiff=12，
//   OURS-only={mov×3,jmp×1}）⇒ 回滚维持原形，判 N18b。
// 语义闭合。
#include "APSystemOnlyOrig/ClearAction.inc"
}

// SetTodayAction —— 报告汇编+反编译全文转写。ORIG T 0x0812161c。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x0812161c（Ghidra C + objdump 逐段）：
// ①time_t 单局部 -0x4c（旧版 now/tt 双局部）；②`it != table.end()`（ne，非 eq）
// 且 end 走具名临时 -0x4c 槽；③`list` 为具名指针（ptEv 结果）；④candidates 为
// **空 vector + 独立建表循环 `push_back(&(*list)[pass])`（prvalue ⇒ OS3_ 右值重载）**
// ——旧版预尺寸 C1EjRKS3_RKS4_ 缺 C1Ev/push_back(&&)；
// ⑤过滤循环用 `candidates[i]=0` 标记 + goto next（每字段重调 operator[]，共 11 次）；
// ⑥group 判定链顺序 0x1d/0x1e/0x1f/**0x2d**/0x20（P46 case 序）；
// [b28d-solo-APSystemOnlyOrig] 判词：**gap 10→2**（本轮还原：标准 `for(;row<300 && it!=end; it++)` 循环旋转布局（4.1 init 后 jmp 底部条件）、P223 fA/fH goto 空标签岛补回 3 处 nop+jmp 岛、lo/hi 表达式直读；370 vs 368）。残差 = N18b（weekday 比较 2 条 mov 中转）+ N107（memcpy dest `add $0x15` vs `lea`）。p167 CLEAR 35/35。
// ⑦循环尾 `it++`（ppEi 后置，P28 增量侧）。
#include "APSystemOnlyOrig/SetTodayAction.inc"
}

// GetTodayRewardAndSendMail —— 报告全文转写。ORIG T 0x08122104。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08122104：size() 循环外缓存（-0x1c），
// 逐字段 `todays[i].xxx` **每次重调** const operator[]（P157 禁引用缓存；旧版
// `const _TodayRewardItem& entry=todays[i]` 只 1 次调用 ⇒ p167 MISSING 3）；
// `entry` 是**循环外声明的具名指针**（-0x18 初值 0，命中即 break），循环后
// 虚调用槽核对（Main 2026-09-09 要求）：ORIG 0x08122263 `call *%edx` 槽 = vptr+0x8
// （`add $0x8,%eax`）；我方 0x203b 同槽 ⇒ make_item 目标一致（仅 %eax vs %edx = N7）。
// [b28d-solo-APSystemOnlyOrig] 判词：**N82 + 档②封顶**（残差 = 槽基浅 4 + N93附 两处 `je` vs 4.4 `jne+mov$0+jmp` + N7 虚调目标寄存器 + P75 实参装载序 + P254 字段中转；m_isRewardItem uchar `cmp $1` vs ORIG bool `xor $1`（档②候选：探针 /tmp/ap_probe6.cpp gS3 显示需 bool 字段 + `if(!f)` 才出 xor，共享头改动未落）；p167 CLEAR 18/18）。
// `if(entry==0)return false`（死检查照抄 P171），`m_flag00=1` 在该检查**之后**。
#include "APSystemOnlyOrig/GetTodayRewardAndSendMail.inc"
}

// ============================================================================
// Inter_LoadRewardMedal::SendMail —— 报告全文转写。ORIG T 0x08124ea0。
// 声明位于 InterDispatcher.h（类内 public）。
// ============================================================================
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08124ea0：size() 缓存 -0x1c、hit -0x18
// （初值 0）、i -0x14；两个条件各逐字段重调 const operator[]（P157；旧版
// 虚调用槽核对（Main 2026-09-09 要求）：ORIG 0x08125015 `call *%edx` 槽 = vptr+0x8
// （`add $0x8,%eax`）；我方 0x22df 同槽 ⇒ make_item 目标一致（仅 %eax vs %edx = N7）。
// [b28d-solo-APSystemOnlyOrig] 判词：**N82 封顶**（残差 = 槽基浅 4 + N93附 两处 return 块序 + N7 虚调目标寄存器 + P254 字段 eax→ebx 中转 + P75 实参装载序（`movl $0x8,S` 提前）；171 vs 175）。p167 CLEAR 20/20。
// `const _MedalRewardItem& m = medals[i]` 只 1 次 ⇒ p167 MISSING 5）。
#include "APSystemOnlyOrig/SendMail.inc"
}

}  // namespace APSystem
