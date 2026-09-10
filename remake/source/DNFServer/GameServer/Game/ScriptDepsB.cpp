// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScriptDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— ScriptDepsB.cpp（脚本装载依赖 B 组，独占 TU）
//
// 逐报告转写自 docs/class_func_reports/_lt_global_gt_/ 对应 md
// （## 汇编 + ## 反编译 C 全文已核对）：
//   importEtcParameterScriptForServer  0x080966fa  ../RDARScriptEtcParameter.cpp 域
//   importGenuineUpgradeTable          0x08912336  同上
//   importSpPerLevelReferenceTable     0x08910505
//   loadAppendageType                  0x0889253b
//   loadValueType                      0x08892ac4
//   getTitleCollection                 0x08aa94e1
//   importRewards                      0x08aa8b79  （../RDARScriptTitle.cpp 域）
//   importTitleSection                 0x08aa8d65  （同上；三函数级静态）
// ============================================================================

#include "ScriptDepsB.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"           // stTitleBookInfo（m_map18 权威布局）
#include "CEnvironment.h"           // less_TCHAR / SpinLock
#include "CItemUpgrade_Separate.h"  // GenuineDamageUpgradeTable 及元素 POD
#include "CTitleBook.h"             // stTitleSection / stTitleRewardBasis 族
#include "DNFLexWrapper.h"          // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"

// ============================================================================
// Etc/EtcParameter.etc 常量定义（ORIG BSS：g_physicalDamageRateConst_ 等，
// nm 实测 B 0x093f5d10..0x093f5d83；本 TU 为唯一定义点）
// ============================================================================
float g_physicalDamageRateConst_;
float g_physicalDamageAddConst_;
float g_physicalReduceRateConst_;
float g_physicalReduceAddConst_;
float g_physicalRandomBaseConst_;
float g_physicalRandomRateConst_;
float g_magicalDamageRateConst_;
float g_magicalDamageAddConst_;
float g_magicalReduceRateConst_;
float g_magicalReduceAddConst_;
float g_magicalRandomBaseConst_;
float g_magicalRandomRateConst_;
float g_commonElementBaseConst_;
float g_commonElementRateConst_;
float g_commonElementRateConstInPVP_;
float g_commonElementRateConstInLadderPVP_;
float g_hpIncreaseRateConst_;
float g_hpRegenRateConst_;
float g_mpIncreaseRateConst_;
float g_mpRegenRateConst_;
float g_activeStatusDamageRateConst_;
float g_activeStatusDefenseRateConst_;
float g_physicalDefenseAddConst_;
float g_magicalDefenseAddConst_;
float g_hpRegenRateAddRateConst_;
float g_mpRegenRateAddRateConst_;

// ============================================================================
// loadValueType —— ORIG 0x08892ac4（_Z13loadValueTypeRKSs）
// "+"=0、"%"=1、其他=2。
// ============================================================================
#include "ScriptDepsB/loadValueType.inc"
}

// ============================================================================
// loadAppendageType —— ORIG 0x0889253b（_Z17loadAppendageTypeRKSs）
// 纯字符串→枚举映射链；比较顺序与 ORIG 级联完全一致，未匹配返回 -1。
// ============================================================================
#include "ScriptDepsB/loadAppendageType.inc"
}

// ============================================================================
// importEtcParameterScriptForServer —— ORIG 0x080966fa
// 报告 _lt_global_gt_/importEtcParameterScriptForServer.md：
//   loadRDARScriptFile("", "Etc/EtcParameter.etc") 失败→false；
//   do { ScanType(token,false); } while (token != "[damage fomula constant]");
//   随后按序 ScanFloat 写入 26 个 g_*Const_ 全局 → true。
//   （ORIG 另构造一个未使用局部 string local_20，无语义，略。）
// ============================================================================
// [b28d-solo-ScriptDepsB AE: 补 ORIG 两枚 string + int/float/bool 死局部（P232/P171
//  照抄）：ctor 序 -0x18→-0x1c、清零序 -0x14(int 单段)/-0x10(float 两段 eax 中转)/-0x9(bool)，
//  目标串以立即数压参。ORIG 0x080966fa]
#include "ScriptDepsB/importEtcParameterScriptForServer.inc"
}

// ============================================================================
// getTitleCollection —— ORIG 0x08aa94e1（_Z18getTitleCollectionP15stTitleBookInfoi）
// find(tab)；== end → 返回 0；否则返回 &m_map18[tab]。
// ============================================================================
std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab)
{
    // b28c-l23 根因（P0/P183）：ORIG 无 map 引用局部槽——三处成员访问都是
    // `mov 0x8(%ebp),%eax; lea 0x18(%eax),%eX` 就地折叠寻址（@0x08aa94f1/0x08aa9508/
    // 0x08aa953b）；我方具名 `collection` 引用使 4.4 把基址落槽（-0xc）并在每次
    // 实参装载时回读，槽号整体平移（-0x10→-0x14、-0xc→-0x10）。改为逐点直写表达式。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        book->m_map18.find(tab);
    if (it == book->m_map18.end())
        return 0;
    return &book->m_map18[tab];
}

// ============================================================================
// importRewards —— ORIG 0x08aa8b79
// 报告 _lt_global_gt_/importRewards.md。appendage 命中→stRewardChangeStatus；
// 否则 [give emotion]→stRewardGiveEmotion；[send postal]→stRewardPostal；
// 其余令牌无副作用。恒返回 true（eax 流出）。
// 扫描顺序逐条对齐汇编：change=status 先存 appendage 再 Str/ValueType/Float；
// postal 先两个 int(+0x18/+0x1c) 后三个 string(+0xc/+0x10/+0x14)。
// ============================================================================
// [b28d-solo-ScriptDepsB AE-语义 / N(P197④) 形态封顶 | ORIG 0x08aa8b79]
//  修法：else-if 链 + 单一 return 1（typeText 公共尾块析构）；appendage 异型化
//  （unsigned）翻 store RHS-first（P211）；postal 分支照抄 ORIG -0x20 预清死局部。
//  首条件用 while+goto 复现 ORIG 槽比物化（P225 实证）：4.4 对 `if(槽!=K)` 恒
//  直跳（je 1 条），对 **while 条件**才发 `cmpl $K,槽; setne; test; je` 同形；
//  body 末尾 goto 公共尾（等价 break 后跳尾）⇒ 与 ORIG 0x08aa8ba4/0x08aa8c10 逐条
//  一致（探针 /tmp/sdb_probe/p1.o f_while_goto）。
//  D1 虚调目标（旧 N7）已由 CTitleBook.h 指针算术形修复。
#include "ScriptDepsB/importRewards.inc"
}

// ============================================================================
// importTitleSection —— ORIG 0x08aa8d65
// 报告 _lt_global_gt_/importTitleSection.md。三个函数级静态（ORIG 经
// __cxa_guard 初始化）：__lookup 键表 / __init_lock 自旋锁 / __inited 标志。
// 首个令牌触发级联登记（[/section]0x155→[/reward type]0x158→[section name]
// 0x15c→[section icon]0x15f→[frame type]0x166→[lock message]0x169→[name]
// 0x16e→[image path]0x172→[effect path]0x179），置位 __inited 后以当前令牌
// 重新查表派发。查表命中标签则解析对应字段；未命中即奖励行 → importRewards。
// 返回：ScanType 耗尽→true；[/reward type]→true；[/section]→false。
// ============================================================================
// [b28d-solo-ScriptDepsB: switch 驱动级联注册链（P177 static 三件套落循环体内、
//  P163 case 0 撑宽、P192/P315 value_type 直构、P96/P270 const_iterator）；
//  查表段单点共享。ORIG 0x08aa8d65]
#include "ScriptDepsB/importTitleSection.inc"
}

// ============================================================================
// importSpPerLevelReferenceTable —— ORIG 0x08910505
// 报告 _lt_global_gt_/importSpPerLevelReferenceTable.md。
// stSpPerLevelTable 在 ORIG 中为空壳派生（W ctor/dtor 与 vector<unsigned> 一致，
// sizeof 0xc）；此处与 CDataManagerMethods.cpp 保持逐 token 相同的局部定义以
// 产生一致的 mangled 形参 R17stSpPerLevelTable。
// 语义：loadRDARScriptFile("", path) 失败→0x2948；ScanType 循环中命中
// "[sp table]" 段则 reserve(200)、先压入 0 号项，随后每行 ScanInt 两枚——
// 仅第二枚入表（首枚为等级位），任一枚失败回到外层令牌循环。成功返回 0。
// ============================================================================
struct stSpPerLevelTable : public std::vector<unsigned int> {};

// [b28d-solo-ScriptDepsB AE: int 局部 + 隐式转换物化右值 push_back(EOj) 对齐
//  ORIG 0x08910505；等级位与数值位均为 int 具名局部（-0x10/-0xc），
//  push_back 传 int 触发 int→unsigned 转换临时（-0x18/-0x14）⇒ 右值重载 EOj]
#include "ScriptDepsB/importSpPerLevelReferenceTable.inc"
}

// ============================================================================
// importGenuineUpgradeTable —— ORIG 0x08912336（../RDARScriptEtcParameter.cpp）
// 报告 _lt_global_gt_/importGenuineUpgradeTable.md。节分派：
//   [table]                    m_infos 清空后逐行 {int,int,int,level/10000f,float}
//                              以自增键插入 +0x158；
//   [separate upgrade max]     ScanInt(+0x004)，失败 logFormat(0x2f9e)；
//   [level]                    ScanInt(+0x000)，失败 logFormat(0x2fa5)；
//   [item weights by grade]    {key,int,int} 插入 +0x140；
//   [item weights by rarity]   6×float → +0x008 区（失败 logFormat(0x2fbe) 续扫）；
//   [stat weights by rarity]   6×float → +0x020 区（logFormat(0x2fc9)）；
//   [weapon weights]           11×6 float → +0x038 区（logFormat(0x2fd5)）；
//   [separate upgrade effect]  行循环 {r,g,b}→RGBA 打包入 +0x04，再
//                              {+0xc,+8,+0} 三 int，自增键插入 +0x170。
// loadRDARScriptFile 失败→false；其余路径均 true。
// ============================================================================
// [b28d-solo-ScriptDepsB NEAR(仅 RGBA 寄存器角色) | ORIG 0x08912336]
//  修法：补 ORIG 两枚死 string（-0xb0/-0xb4，P171 照抄）⇒ ctor/dtor 符号族归位；
//  两处 `make_pair(key++, X)` 后置 ++（右值第一参 ⇒ make_pair<int,T&>，P315）；
//  循环上界 `<= 5`/`<= 10` 复现 4.1 `setle;test;jne`（P51）。
//  ① 7 处 `continue` 改 `goto sectionHead`（标签在循环体首）：4.4 对 continue 恒发
//  共享 latch 块（`jmp <尾部>`，与 ORIG 直跳循环头 0x089123a2 形不符），goto 头标签
//  才逐条复现 ORIG 的 10 处 `jmp 0x089123a2`（含入口 3 个 `jmp;nop` 中继岛，
//  0x08912399/0x0891239c/0x0891239f）——探针 /tmp/sdb_probe/p5.o fGoto。
//  ② [separate upgrade effect] 尾 `continue`→`break`（真 bug：ORIG 0x0891285d
//  `je 0x089123a1` 后处理完即 0x089129c4 `jmp 退出块`，不再扫后续令牌）。
//  ③ RGBA 打包用 `(unsigned char)` 逐字节 cast（movzbl），`& 0xffu` 会退化成 and。
//  收口读数：raw 448=448、norm 444=444、p167 55/55 CLEAR；仅余 RGBA 首两操作数
//  寄存器角色（4.1 累加器留 %eax vs 4.4 留 %edx，P321-T5 寄存器角色 ⇒ N）。
//  [E06 2026-09-10 复测] 归一 diff 已收敛到单点：RGBA 打包段
//    ORIG `movzbl %dl,%edx; shl $0x8,%edx; or %edx,%eax`（累加器 %eax）
//    4.4  `movzbl %dl,%edx; shl $0x8,%edx; or %eax,%edx`（累加器 %edx）。
//  探针穷举 15 形（/tmp/e06_rgba/probe.cpp v1..v6 + probe2.cpp v7..v15：
//  原形/具名 unsigned/交换前两项/去 unsigned 提升/右结合/`&0xff` 掩码形/
//  三具名 int/uchar 局部/unsigned long 提升）**全部产 `or %eax,%edx`**；
//  `-fdump-tree-gimple` 证实 GIMPLE 树序未交换（`D = r | (g<<8)`），差异在
//  expand 阶段的可交换操作数 dst 选择 ⇒ 判 N86b（同一算术多 codegen 定论）+
//  P321-T5（寄存器角色不改类型/源码），**无源码旋钮**（探针路径见上）。
#include "ScriptDepsB/importGenuineUpgradeTable.inc"
}
