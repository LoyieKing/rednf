// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CreateDnfEventSciprt/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CreateDnfEventSciprt 实现（独立新 TU，全局命名空间类）。
// 依据 docs/class_func_reports/CreateDnfEventSciprtServer/{Load,Destroy}.md、
// docs/class_func_reports/CreateDnfEventSciprt/{loadScript,clearScript,
// GetItemInfo}.md 与 ORIG 反汇编逐段转写。
//
// 注册链（ARAD.cpp Init #8，待 ARAD 域批次接入）：
//   RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13)
#include "CreateDnfEventSciprt.h"

#include <cstdio>

#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile /
                              // ScanType / ScanInt / ScanTypeOrInt
#include "GameTypes.h"        // cMyTrace

namespace
{
typedef std::vector<CreateDnfEventSciprt::ItemInfo> ItemInfoVector;
}  // namespace

// ============================================================================
// CreateDnfEventSciprt
// ============================================================================

// ORIG _ZN20CreateDnfEventSciprtC1Ev @0x08ac000c：
// this+4 处 m_itemMap 默认构造（map Rb_tree impl 初始化）。
#include "CreateDnfEventSciprt/CreateDnfEventSciprt.inc"
}

// ORIG _ZN20CreateDnfEventSciprtD1Ev @0x08ac0020：m_itemMap 逆序析构。
#include "CreateDnfEventSciprt/dtor_CreateDnfEventSciprt.inc"
}

// ORIG loadScript @0x08ac0034（0x2f5 字节）。token 循环；
// 各段失败路径 → unloadRDARScriptFile + return 0；
// duplicate [level] → printf 后直接 return -1（不 unload，ORIG 同）。
// b28d-solo-CreateDnfEventSciprt：4.4 对 while 恒 loop-rotation（条件移至 body 末尾），
// ORIG 4.1 为「入口 jmp → continue 中继岛(nop+jmp / nop) → 条件头 → body → 回边 jmp」
// 布局（08ac0087/0089/008c/008d）⇒ 改 for(;;)+显式 goto 岛（探针 /tmp/probe_ls2.cpp
// loop3 逐条同形）；4 次 ScanType 失败各落独立 break 岛（08ac02be/02c1/02c4/02c7）。
// 终判 AE：AE 归一后 210=210 逐条相等（ndiff 184→0，gap 0），p167 32/32 CLEAR。
// 余差钥匙：ORIG 两处字符串判断为 operator== + `test;je`（无 xor）⇒ 4.4 对否定式
// `if (!(token == X))` 恒加 xor 物化（实测 ndiff 4→10），必须正向包裹 + else goto 岛。
#include "CreateDnfEventSciprt/loadScript.inc"
}

// ORIG clearScript @0x08ac032a（0x7b 字节）。逐指令转写【ORIG 原样，含缺陷】：
// 循环体内无迭代器自增，且把 mapped value 首 4 字节当堆指针做显式析构 + delete。
#include "CreateDnfEventSciprt/clearScript.inc"
}

// ORIG GetItemInfo(vector&) @0x08ac04a4（0x13d 字节）：合并所有 day 组的全部元素。
#include "CreateDnfEventSciprt/GetItemInfo.inc"
}

// ORIG GetItemInfo(vector&, int) @0x08ac03a6（0xfe 字节）：仅指定 day 组；
// find 未命中时不动 out（ORIG 同）。
#include "CreateDnfEventSciprt/GetItemInfo_1.inc"
}

// ============================================================================
// CreateDnfEventSciprtServer
// ============================================================================

// ORIG C1 @0x081648ac：先 m_script(this+4) ctor → ARAD::Singleton<T> C1（空）→
// ScriptInterface C1（vptr 0x8b2db78）→ 写自身 vptr 0x8b7cba8。
#include "CreateDnfEventSciprt/CreateDnfEventSciprtServer.inc"
}

// ORIG D1 @0x081648e2：写 vptr 0x8b7cba8 后析构 m_script(this+4)；无基析构调用。
#include "CreateDnfEventSciprt/dtor_CreateDnfEventSciprtServer.inc"
}

// ORIG Load @0x0816451e：PRETTY 串 "virtual bool CreateDnfEventSciprtServer::Load()"
// （.rodata 0x8b7c9a0），行号 0xd0 / 计数 9 / true,false → cMyTrace C1(PKc,iibb)；
// 打点 "- Loading EventCreateDnf script - "；return loadScript()==0（b28c-32：
// m_script 已改为第一个基类，直调基类成员）。
#include "CreateDnfEventSciprt/Load.inc"
}

// ORIG Destroy @0x08164590：mov $0x1,%eax; ret（undefined4 返 1；接口 void 化沿用
// AvatarFixedHiddenOptionServer 先例，见 ScriptInterface vtable 两槽契约）。
#include "CreateDnfEventSciprt/Destroy.inc"
}

// ============================================================================
// ORIG 单例链完整类族：_ZN4ARAD9SingletonI26CreateDnfEventSciprtServerEC1Ev（W
// 0x08164902）、Get（W 0x08164907）、guard var/obj（0x09404630/0x09404638）、
// GetPtr。本 TU 经下列可执行引用完成隐式实例化（inline 模板各 TU COMDAT 合并，
// 与 ARAD.cpp 将来的 #8 RegistScript 调用不冲突）；勿在他处重复显式实例化。
// ============================================================================
namespace
{
ARAD::ScriptInterface* instantiateCreateDnfEventSciprtServerSingleton()
{
    return ARAD::Singleton<CreateDnfEventSciprtServer>::Get();
}
}  // namespace
