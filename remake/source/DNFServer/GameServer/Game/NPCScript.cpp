// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NPCScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "NPCScript.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "NpcScript.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()（ORIG 0x088c0409）
#include "CDataManager.h"     // G_CDataManager()/getItemList()（giveGiftItem 族）
#include "CItem.h"            // CItem::get_index/getItemGroupName
#include "DNFLexWrapper.h"    // ScanType/ScanInt/ScanStr
#include "DNFFunctionLib.h"   // get_rand_int(int)
// class SpinLock / struct less_TCHAR（[random message balloon] 静态查找表用）。
#include "CEnvironment.h"

// ============================================================================
// CNPCScript / NpcScript / importNpcScript
//   NpcScript ctor/clear：ORIG 0x089fb942 / 0x089fbf22（NpcScript/ 报告逐字段）
//   importNpcScript：     ORIG 0x089fc264（_lt_global_gt_/importNpcScript.md
//                         反编译 C 逐分支转写，§9 主分支级还原）
//   CNPCScript 访问器：    ORIG 0x85809a6 / 0x86d2736 / 0x8580fd0 /
//                         0x8580e6a / 0x85811a4
// ============================================================================

// ---- 外部符号（ORIG 真实 T/B；定义在各自 TU，链接期解析）----
// getNpcRoleByString/getNpcMoodFromString/getDirectoryPath/g_npcScript*/
// STScriptFileList::FindIndexByFullScan 声明见 NpcScript.h。

// [favor rate per mood] 的除数（ORIG _DAT_08e1dbf4 = 100.0f）。
static const float NPC_FAVOR_RATE_DIVISOR = 100.0f;

// ORIG W 0x08582338：仅构造 this+0x04 NpcScript；+0x00 不初始化。
#include "NPCScript/CNPCScript.inc"
}

// ORIG W 0x0858234e：仅析构 this+0x04 NpcScript。
#include "NPCScript/dtor_CNPCScript.inc"
}


// ----------------------------------------------------------------------------
// PresentResponceScript 族 —— ORIG [favor_dialog] 三结构生命周期：
//   Script    C1 0x8a00aba / clear 0x8a00a86（m_a/m_b/m_c=0 + m_str.clear()）
//   Exception C1 0x8a00b22 / clear 0x8a00b02（m_a=0 + m_str.clear()）
//   Event     C1 0x8a00b8a / clear 0x8a00b6a（m_a=0 + m_str.clear()）
// ----------------------------------------------------------------------------
#include "NPCScript/PresentResponceScript.inc"
}

#include "NPCScript/clear.inc"
}

#include "NPCScript/PresentResponceScriptException.inc"
}

#include "NPCScript/clear_1.inc"
}

// InteractiveDialogScript：ORIG C1 0x08a009f2（成员 C1 + clear）/
// clear T 0x08a0098a（+0/+4/+8 int=0、+0xc vector clear、+0x18 byte=0、
// +0x1c int=0、+0x20 byte=0、+0x24 string clear、+0x28 vector clear）。
#include "NPCScript/InteractiveDialogScript.inc"
}

#include "NPCScript/clear_2.inc"
}

#include "NPCScript/PresentResponceScriptEvent.inc"
}

#include "NPCScript/clear_3.inc"
}

// ----------------------------------------------------------------------------
// NpcScript::clear —— ORIG 0x089fbf22 逐字段。
// ----------------------------------------------------------------------------
#include "NPCScript/clear_4.inc"
}

// ----------------------------------------------------------------------------
// NpcScript 构造/析构：ORIG ctor 为各子对象默认构造 + clear()；成员自带构造，
// 此处仅补齐与 ORIG 一致的 clear 调用。析构由成员自动完成 —「推断等价」。
// ----------------------------------------------------------------------------
#include "NPCScript/NpcScript.inc"
}
#include "NPCScript/dtor_NpcScript.inc"
}

// stFavorableRelationShip_t::clear —— ORIG 0x089fb56c T（忠实转写）：
//   标量区清零（word@0 / dword@4 / word@8,0xa,0xc）
//   → moodChanges(+0x10)/keyItems(+0x1c) 两个 vector clear
//   → 四张礼物表 ×5 个 map（stride 0x18）clear
//   → favorRate×3 置 1.0f（0x3f800000，+0x4d8/4dc/4e0）
//   → m_rewardItems(+0x208).clear()。
// [b28d-solo-NPCScript] 同族 `_ZN25stFavorableRelationShip_tD1Ev`（清单旧记 NEAR 2）
// 现与 ORIG **AE**：ORIG 存在用户析构符号（非 trivial，P39/P317 反向）⇒ 类内
// 用户空析构保留不动，隐式体逐条对齐（无源码可改）。
#include "NPCScript/clear_5.inc"
}

namespace {

// 解析失败统一出口：ORIG 在各 scan 失败处 return 0。
const int IMPORT_OK    = 1;
const int IMPORT_FAIL  = 0;

}  // namespace

// ----------------------------------------------------------------------------
// importNpcScript —— ORIG 0x089fc264 主分支逐关键字转写。
// 深水区（长尾 keyword 的次要字段、静态 lookup 表优化）以语义等价形式实现，
// 已在对应分支标注「推断」。
// ----------------------------------------------------------------------------
// [b28e-NPCScript 语义修复批] ORIG 0x089fc264：超大函数（>500，§6.2 三件套）。
//   p167 调用点差集 CLEAR（ORIG 351 = OUR 351，仅 C1/C2、D1/D2 同址别名差，P93）。
//   按 call 锚点分段 + 槽号归一化多重集比对，本批修 6 处：
//   ① [web page] 0x089fc74a：ORIG ScanStr 后直接 push_back（无返回值检查）⇒
//      删我方多余的 `if(!ScanStr(&sub)) break;`（P171 照抄）。
//   ② [click mark] 0x089fc94f：ORIG info 在循环外、m_param 仅在 m_type==0 时由
//      ScanInt(&ok) 更新（type!=0 保留旧值）⇒ 改条件写 + info 移出循环。
//   ③ [favor_duration_reward_buff_dialog] 0x089fcf57：ScanInt 实参 NULL→&ok。
//   ④ [change] 0x089fd2f4：ORIG 循环头 `int value = 0` ⇒ 提前声明。
//   ⑤ 函数头 0x089fc2e3：ORIG 函数级 int 槽（-0x1a4，被 [skill]/[speech]/[dialog]/
//      [int data] 复用）⇒ int v 提升函数级（P320 int 版）。
//   ⑥ [reward item] 0x089fdc81/0x089fdcf0/0x089fddeb：三层 `!ScanType || tok==end`
//      拆两条独立 if（P225 推广），消 4.4 setcc 物化。
//   残余 = 4.1/4.4 结构性差异（N 封顶）：ORIG 9 条 continue 中继岛
//   [round5 复核] p167 复算 ORIG 351 calls = OUR 351 calls，MISSING/EXTRA 全部成对
//   为 C1↔C2 / D1↔D2 **同址别名**（`-24 _ZNSsD2Ev` ↔ `+24 _ZNSsD1Ev`、
//   `-11 _ZNSsC2Ev` ↔ `+11 _ZNSsC1Ev`、stGiftItemInfo 的 C2Im… ↔ C1Im… 等，P93）
//   ⇒ **无真语义缺口**；逐助记符比对得 109 个形态差异块（我方 2532 / ORIG 2593），
//   主体为下述 N 族，不属可译缺口。
//   （0x089fc2f4..0x089fc30e 九对 jmp/nop 同落 +0xab，4.4 就地 jmp）、bool 局部
//   物化（ORIG mov %al,-S; movzbl; xor$1 vs 4.4 直接 test，N122）、状态机路径标志
//   （ORIG %esi/%ebx vs 4.4 栈 bool）、`fav` 引用缓存致成员偏移基数差 0x170
//   （P183，语义等价）、帧 0x67c vs 0x66c（N82）。
#include "NPCScript/importNpcScript.inc"
}

// ORIG W 0x08582374/0x085823a4：isKeyItem 的具名 functor（nm 三符号
//   isSameItem C1/C2/cl，W 弱符号；ctor 以 pair<unsigned long,unsigned int>
//   转发构造 m_pair，operator() 非 const 且语义为
//   item.first==m_pair.first && m_pair.second>=item.second）。
struct isSameItem
{
    std::pair<unsigned long, unsigned int> m_pair;

    isSameItem(unsigned long m, unsigned int j)
        : m_pair(m, j) {}

    bool operator()(const std::pair<unsigned long, unsigned int>& item)
    {
        if (m_pair.first == item.first && m_pair.second >= item.second)
        {
            return true;
        }
        return false;
    }
};

// ORIG 0x85809a6:
//   ret = importNpcScript((NpcScript*)(this+4), path);
//   if (ret == 1) *(int*)this = *(int*)(this+4);
//   return ret == 1;
#include "NPCScript/load.inc"
}

// ORIG 0x86d2736: m_roleMap.find(role) != end()（CNPCScript+0x50 = NpcScript+0x4c）。
#include "NPCScript/isExistRole.inc"
}

// ORIG 0x8580fd0: 遍历奖励等级表（CNPCScript+0x658 = NpcScript+0x654）。
#include "NPCScript/isRewardLevel.inc"
}

// 具名局部 comp 先构造（ORIG ctor 调用居函数首），比较用 eq + je 分支形态。
#include "NPCScript/isKeyItem.inc"
}

// ORIG 0x85811a4: +0x670 字符串 == "item" -> 1；=="quest" -> 2；否则 0。
#include "NPCScript/getPeriodRewardType.inc"
}

// ============================================================================
// CNPCScript 礼物/心情/奖励 6 函数 —— ORIG 0x85809e4 / 0x8580d30 / 0x8580f28 /
// 0x8581076 / 0x85811f8（T）；getFavorRatePerMood 为头内联（W 0x085819c6）。
// ============================================================================

// ORIG 0x85809e4：按 mood 槽依次查四张礼物表（m_maps[mood]），命中后比较
//   it->second.m_b（ushort）与 count 决定采纳 m_c 还是写 result=FAIL；
//   give!=0 时提前返回。m_c 为有符号 short（setg 16 位比较 / std::max<short>）。
// [b28d-solo-NPCScript] ORIG 0x085809e4：**NEAR**（助记符序列全等）。已对齐
// it/give/key1/find-sret 槽（-0x30/-0x2a/-0x28/-0x3c，P309 全头部声明 +
// 声明序 gn2/key2/gn1/key/give/it）与 give!=0→max 极性（ORIG +0x304
// `test %ax,%ax; je <m_c 块>`）。残差 = 块内 end() 临时槽与 gn/key2 的分配
// 方向（ORIG -0x24/-0x1c/-0x14/-0x20/-0x18/-0x10 vs 4.4 -0x18/-0x14/-0x10/
// -0x24/-0x20/-0x1c）⇒ **N93/N82附 封顶**（编译器临时槽不可由源码控制）。
// p167 CLEAR 28=28。
#include "NPCScript/giveGiftItem.inc"
}

// ORIG 0x8580d30：按 m_rewardItems.m_slots[index][mood].m_byDir[change] 权重表
//   随机抽取（get_rand_int(weight)）；权重 0 条目（m_a==0 && m_b==0）直接 break。
// [b28d-solo-NPCScript] ORIG 0x08580d30 已逐条对齐（帧 0x2c、push edi/esi/ebx、
// 槽 -0x28/-0x24/-0x20/-0x1c/-0x10 全等；p167 CLEAR 7=7）。唯一残差 =
// create_item 第 3 实参 `mov 0x4(%eax),%eax; mov %eax,%esi`（4.1 两段 regmove）
// vs 4.4 单段直落 callee-saved ⇒ **N18a 封顶**（P248 三档已试：callee 返回
// void / 字段 int 与形参 int 同型 / 无转换旋钮；反向亦不可造）。
#include "NPCScript/getGiftRewardItem.inc"
}

// ORIG 0x8580f28：m_moodChanges（NpcScript+0x180 处 pair<MOOD,uint> vector）
//   轮转：空表时 mood=1/count=0 返回 0；否则 ++count 后越界回绕，
//   输出 at(count) 的 first/second。
#include "NPCScript/getNextMood.inc"
}

// ORIG 0x8581076：遍历 m_levelRewards，匹配 level 后拷贝五字段（string 用
//   operator=）；循环条件每轮重求 end()。
#include "NPCScript/getLevelRewardInfo.inc"
}

// ORIG 0x85811f8：m_buffRewards 权重随机（get_rand_int(1000)），条件
//   it->m_a < level；命中后拷贝 m_b/m_a/m_c 并 break。
#include "NPCScript/getBuffRewardInfo.inc"
}
