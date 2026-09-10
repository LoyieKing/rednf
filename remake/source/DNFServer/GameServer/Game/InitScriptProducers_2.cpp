// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InitScriptProducers_2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// InitScriptProducers_2.cpp —— CDataManager::init（ORIG 0x08358332..0x0835f1d5，
// 报告 docs/class_func_reports/CDataManager/init.md）依赖的脚本装载自由函数
// 生产 TU（第 2 批，InitScripts_2）。
//
// 每个函数均转写自 docs/class_func_reports/_lt_global_gt_/ 对应报告（objdump
// 汇编 + Ghidra C），统一模式：loadRDARScriptFile("",path) → ScanType 分派 →
// ScanInt/ScanStr/ScanFloat 字段回填。调用点（init.md 行号 / 目标成员）：
//   ImportReturnUserRewardScript 0x089180ea  init.md:1044  this+0xb548
//   ImportBingoScript            0x089187b9  init.md:1071  this+0xb568
//   ImportSecurityReward         0x089113ec  init.md:1123  this+0x50d8
//   ImportItemMakingSkill        0x0890f564  init.md:1146  this+0xb458
//   importGrowthPower            0x08911ee3  init.md:1796  this+0x8710
//   ImportWarAreaCounter         0x089026f3  init.md:1956  this+0x87b4
//   importEquipmentPartSetInfo   0x089073d9  init.md:4749  ("Etc/EquipmentPartSet.etc",3)
//   importPairPvPStatScript      0x08977d9c  init.md:4936  this+0x4df8
//
// 附带本批函数的被调依赖（全工程此前无定义，已广播认领）：
//   LoadRewardDataByGrowType 0x08917f44 / deleteTagBracketString 0x08917db3 /
//   getSameStringIndex 0x08990ed5 / characterEquipmentTypeTags D 0x093f4fc0 /
//   makeFairPVPEquipmentPackageKey 0x088945ad / g_equipmentPartSetMap_ B 0x094f7130 /
//   STEquipmentPartSetData C1 0x891b002·D1 0x08907348。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // 脚本类型权威定义（须先于 deps 头）
#include "sync_script.h"              // STEquipmentPartSetPiece 完整类型
#include "STEquipmentScript.h"        // STEquipmentScript/m_paramMap678/ImportEquipmentScript
#include "EquipmentParameterInfo.h"   // EquipmentParameterInfo copy/operator=
#include "DataManagerInitDepends.h"   // 本批声明唯一汇总点
#include "DNFLexWrapper.h"            // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"               // LogManager::logFormat

// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME —— ORIG D 0x093f4ee0 / 0x093f4ee8
// （各 [2] int）。初值取自 ORIG .data 逐字节实读：
//   0x093f4ee0: 20 bf 02 00 20 bf 02 00  c0 27 09 00 c0 27 09 00
//   ⇒ PVP_NORMAL_BATTLE_TIME = {180000, 180000}（0x2bf20，3 分钟毫秒）
//      PVP_DM_BATTLE_TIME     = {600000, 600000}（0x927c0，10 分钟毫秒）
// 非零静态初值同时决定段归属为 .data（D，与 ORIG 一致；零初始化会落 .bss）。
// ORIG 中与本 TU 的 g_equipmentPartSetMap_ / characterEquipmentTypeTags 同源文件且为
// 文件内首个具名对象，决定 `_GLOBAL__I_PVP_NORMAL_BATTLE_TIME` 静态初始化键名
// （c6root gcc4.4 探针验证）；因此定义必须置于本文件首个具名对象位置，
// PvP_Room.cpp / CDataManager.cpp 仅 extern 引用（脚本加载时覆盖 [0]/[1]）。
// ============================================================================
int PVP_NORMAL_BATTLE_TIME[2] = {180000, 180000};
int PVP_DM_BATTLE_TIME[2] = {600000, 600000};

// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
const char* characterEquipmentTypeTags[26] = {
    "[hat avatar]",    "[hair avatar]",   "[face avatar]",    "[coat avatar]",
    "[pants avatar]",  "[shoes avatar]",  "[breast avatar]",  "[waist avatar]",
    "[skin avatar]",   "[aurora avatar]", "[weapon]",         "[title name]",
    "[coat]",          "[shoulder]",      "[pants]",          "[shoes]",
    "[waist]",         "[amulet]",        "[wrist]",          "[ring]",
    "[support]",       "[magic stone]",   "[creature]",
    "[artifact red]",  "[artifact blue]", "[artifact green]",
};

// ORIG B 0x094f7130（BSS，sizeof(std::map)=0x18）；init 尾段遍历回灌 DB。
std::map<int, STEquipmentPartSetData> g_equipmentPartSetMap_;

// ============================================================================
// STEquipmentPartSetData —— ORIG C1 0x0891b002 / D1 0x08907348
// ============================================================================
// b28c-L20: AE — ORIG C1 0x0891b002 orig=32 our=32 gap=0 ndiff=0。顺序 = 成员构造（+0x04 string、+0x0c vector）
// 先于标量赋值（+0x00=-1、+0x08=0）⇒ 标量必须写在函数体，不能进初始化列表
// （进列表会被 4.4 交错到成员构造之间，ndiff 19）。
#include "InitScriptProducers_2/STEquipmentPartSetData.inc"
}

// b28c-L20: AE — ORIG D1 0x08907348 orig=52 our=52 gap=0 ndiff=0：`m_script = 0` 在 if 之外无条件执行（P141：
// 保留无条件 delete，编译器自带判空守卫，ORIG 双 test/je 形）。
#include "InitScriptProducers_2/dtor_STEquipmentPartSetData.inc"
}

// ============================================================================
// makeFairPVPEquipmentPackageKey —— ORIG T 0x088945ad
// (short)a 落低 16 位、(short)b 落高 16 位后按 int 返回。
// ============================================================================
#include "InitScriptProducers_2/makeFairPVPEquipmentPackageKey.inc"
}

// ============================================================================
// getSameStringIndex —— ORIG T 0x08990ed5
// strcmp 线性查表，命中返回下标，未命中返回 -1。
// ============================================================================
#include "InitScriptProducers_2/getSameStringIndex.inc"
}

// deleteTagBracketString（ORIG T 0x08917db3）定义归 InitScriptProducers_1.cpp
// （InitScripts_1 认领），本 TU 经 DataManagerInitDepends.h 声明引用。

// ============================================================================
// LoadRewardDataByGrowType —— ORIG T 0x08917f44
// 扫描到 endTag 结束；每条：deleteTagBracketString→getCharacterJobToEnumValue
// （-1 → 0x3693），grow=ScanInt 必需；循环 level=ScanInt（失败→回到外层标签循
// 环）、data 两 int（失败→0x36a3/0x36a7）插入 out.m_map0。返回 0=正常结束。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 117）：
//   ①key/data 提到**函数头**构造一次（ORIG 0x08917f5b/0x08917f66，非每轮重构造）；
//   ②P164 纯 goto 循环复位头部条件块（0x08917f71），m_grow 失败与 ScanType 失败
//   共享尾部 `mov $0,%ebx` 块。
#include "InitScriptProducers_2/LoadRewardDataByGrowType.inc"
}

// ============================================================================
// ImportReturnUserRewardScript —— ORIG T 0x089180ea（init.md:1044，
// "Event/ReturnUserReward.evt" → this+0xb548）。返回 0=成功，非 0=错误行。
// [pacakege item] 为 ORIG 原文拼写。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 93）：①单层
//   for(;;) + `goto scan_fail` 尾标签（P164/P223）复位 ORIG 头部条件块
//   （0x08918140）与尾部失败岛（0x08918235 nop; mov $0,%ebx）；②`[reward equip
//   item]` 用正形 == 分支（ORIG 0x089181a5 `test; je`，无 xor 物化）。
#include "InitScriptProducers_2/ImportReturnUserRewardScript.inc"
}

// ============================================================================
// ImportBingoScript —— ORIG T 0x089187b9（init.md:1071，"Event/BingoEvent.evt"
// → this+0xb568）。ORIG 所有路径恒返回 0（含加载失败），忠实保留。
// 段：[board]→m_map18、[quiz value]→m_map0、[reward]→m_map30、[use item]→m_vec48。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82/N89 封顶（原 DIFF 311）ORIG
//   0x089187b9 orig/ours=337/338，P167 CLEAR（41/41）。本波修 p167 差集：
//   ①make_pair 全部去 const 左值（ORIG 3 实例 <int&,vector<int>&>/<int&,int&>/
//   <int&,pair<int,int>&>）；②补 `values.clear()`（ORIG 0x089188ad 显式调用）；
//   ③上限变量化 `int limit = 25`（ORIG -0x20 存 0x19，setl 形）；④P164 goto
//   主循环（0x08918800）；⑤count/ok 失败改尾岛 goto scan_done。残余：槽位/块
//   布局跨版本差。
// [P330 bool→int] 与 DataManagerInitDepends.h 声明同步（ORIG caller 4B store
// 无 movzbl ⇒ int）。ORIG 所有出口 ebx=0 ⇒ 两个 return 0。
// [sweep2-InitScriptProducers_2] 复核：fresh orig/ours=329/328 gap=1 ndiff=17，
//   ORIG-only={je:2,jmp:1} vs OURS-only={jne:2}（循环尾 je-exit/jmp-body 布局）；
//   p167 41/41 仅 C1↔C2 同址别名（P93）⇒ 无真缺口，N 封顶。
#include "InitScriptProducers_2/ImportBingoScript.inc"
}

// ============================================================================
// ImportItemMakingSkill —— ORIG T 0x0890f564（init.md:1146，
// "Etc/ItemMakingSkill.etc" → this+0xb458）。返回 0=成功，非 0=错误行。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 196）：P164 纯
//   goto 主循环复位 ORIG 头部条件块（0x0890f5ab），四段 continue→`goto scan_top`；
//   内层 `while(true)` 各段与 ORIG 同形。
#include "InitScriptProducers_2/ImportItemMakingSkill.inc"
}

// ============================================================================
// importGrowthPower —— ORIG T 0x08911ee3（init.md:1796，
// "Etc/GrowthPowerRewardBuff.etc" → this+0x8710）。返回 bool（真=成功）。
// 段：[breakaway section]→m_vec0、[exp reward]→m_vecC、
//     [package reward]{[group]/[package selection]}→m_map18。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82/N112 封顶（原 DIFF）ORIG
//   0x08911ee3 orig/ours=269/268，P167 CLEAR（47/47）。该波已修真语义 bug：
//   ①[package selection] 内层用 `bool ScanInt(int*)`（ORIG `_Z7ScanIntPi`），
//   原写成 `int ScanInt(bool*)`+ok 槽；②`token != "[/package reward]"` 生成
//   operator!=（ORIG 只有 operator==，改 do+`if(==)break`）；③[exp reward]
//   m_reward 用字节视图（ORIG `mov %al,-0x21`，int 视图产 dword 写）。
// [sweep2-InitScriptProducers_2] verdict=N 封顶（ndiff 37→5）orig/ours=262/261：
//   本波修 [exp reward] 槽 +3 平移——STExpReward.h 加 `__attribute__((packed))`
//   （ORIG 0x08911fe5 `lea -0x21(%ebp),%eax` 非 4 对齐槽 ⇒ 类型对齐 1，P250；
//   探针 /tmp/sw2/p9.cpp：packed 局部落 -13 vs 普通 -16）；sizeof 仍 4、成员偏移/
//   mangled 零变化（CUser::get_growth_power_exp_reward 8/8 identical 复核）。
//   残余 5 行 = ORIG do-while/while 尾「je exit; jmp body」+ 循环头 jmp，4.4 恒
//   「jne body」单跳（跨版本循环尾布局，同 importEquipmentPartSetInfo 族）⇒ N。
#include "InitScriptProducers_2/importGrowthPower.inc"
}

// ============================================================================
// ImportWarAreaCounter —— ORIG T 0x089026f3（init.md:1956，this+0x87b4，
// 固定路径 "Etc/WarArea.etc"）。返回 1=成功 / 0=失败。
// 注：ORIG 无下标越界检查（裸指针写入靠相邻区布局兜底）——照抄裸下标（P171）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82 封顶（原 DIFF 297）ORIG
//   0x089026f3 orig/ours=326/328，P167 CLEAR（32/32）。本波修真语义 bug：
//   ①删三处 ORIG 不存在的越界防御（P140/P171 照抄裸下标）；②[counter] 段复用
//   timeCount 作下标（ORIG 0x08902ae2 把 -0x30 存入 m_num 后清零复用），非独立
//   counter 局部；③doorCount/dungeonIdx 为**块内**局部（ORIG -0x20/-0x18）；
//   ④-0x34 为 ORIG 死局部；⑤maxValue 先于 maxIndex 赋值；⑥make_pair<int&,
//   stWarPoint&>（key 去 const，p167 归零）。残余：全帧槽位 +4 平移（N95/N82）
//   + do-while 尾测 setne 物化（4.4 恒物化，ORIG 直跳 jne）。
// [sweep2-InitScriptProducers_2] 复核：fresh orig/ours=313/316 gap=−3 ndiff=443
//   （恒定 +4 槽平移 + 4 处 setne/je/jne 循环尾物化）；p167 32/32 仅 C1↔C2 同址
//   别名（P93）⇒ 无真缺口，维持 N82 封顶。
#include "InitScriptProducers_2/ImportWarAreaCounter.inc"
}

// ============================================================================
// importPairPvPStatScript —— ORIG T 0x08977d9c（init.md:4936，
// this+0x4df8 ← "Etc/FairPvPStat.etc"）。恒返回 0（ORIG 全路径 ebx=0）。
// [grow type index] 以 (job,grow) 为键在 m_map0 插入清零 CharacterStatus，
// 后续段直接写该值字段；[attack speed]/[jump speed] 无 cur 判空（ORIG 缺失，
// 引用恒非空故等价）。[jump speed] 读后丢弃（ORIG 同）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N89/N82 封顶（原 DIFF 524）ORIG
//   0x08977d9c orig/ours=569/496，P167 CLEAR（56/56，§6.2 三件套①）。本波修：
//   insert 结果用 `pair<const_iterator,bool>`（ORIG 转换构造
//   `C1ISt17_Rb_tree_iteratorIS4_EbEEOS_IT_T0_E` + const_iterator ptEv；
//   `const_cast` 取 current）。残余：STL 内联实例差（N89）+ 超大件槽差（N82）。
// [BigEquipScript pass-2 2026-09-09] orig/ours=569/551（gap 73→18）：
//   ①判空下沉：ORIG 17 个数值段各自 `cmpl $0,-0xc(%ebp); jne; mov $0,%ebx;
//     jmp`（[HP MAX] 0x08977fcb … [water resistance] 0x0897860f 逐段实证；
//     [attack speed]/[jump speed] 两段无判空，P171 照抄），原写成链前单一判空；
//   ②ScanType 失败返回当前 ok 值（ORIG 0x0897864d movzbl -0xc9(%ebp),%ebx），
//     非恒 false；③段序按 ORIG（inventory limit/MP regen/move speed/attack/
//     cast/hit/jump power/jump speed/weight/dark/light/fire/water）重排并补回
//     被误覆盖的 [weight] 段；④CDataManager.h CharacterStatus 的 m_field00/
//     m_field04/m_field44 int→unsigned（ORIG [HP MAX] 0x08977fe9 / [MP MAX]
//     0x08978044 / [move speed] 0x089782cb 三段 store 为 RHS-first 中转
//     `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,disp`，而同函数 +0x3c
//     [inventory limit] 0x08978214 / +0x50 [weight] 0x089784b8 为 LHS-first
//     ⇒ P211 判异型；同尺寸同偏移、sizeof 0x54/static_assert 不变；三消费 TU
//     CUser/CDataManagerScripts/InitScriptProducers_2 四数 A/B 实测零回退）。
//   残余 18 = ORIG 19 个 continue 跳板 `nop; jmp loop`（4.1 每个 continue 发
//   3 字节跳板；4.4 jump-threading 合并为共享 jmp，P223 两种 goto 形实测均
//   -1/-2 反劣）⇒ 封 N（跨版本 continue 代码生成差）。
// [sweep2-InitScriptProducers_2] 复核（§6.2 三件套）：fresh orig/ours=548/549
//   gap=−1；p167 calls=56/56，差集仅 C1↔C2 同址别名（P93 结构性噪音，非缺口）；
//   ORIG 18 个集中 continue 跳板（0x08977e02..0x08977e3b 各 `nop; jmp 0x08977e3c`）
//   vs 我方各段直接 jmp 循环头（ndiff 41 全在此）⇒ N 封顶，无真缺口。
#include "InitScriptProducers_2/importPairPvPStatScript.inc"
}

// ============================================================================
// ImportSecurityReward —— ORIG T 0x089113ec（init.md:1123，this+0x50d8）。
// 三段各 4 个 int；返回 bool（真=成功）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N1 封顶（原 DIFF 102）ORIG
//   0x089113ec orig/ours=125/120，P167 CLEAR（22/22）。本波修真语义 bug：
//   ①ORIG 用 `bool ScanInt(int*)`（`_Z7ScanIntPi`）**语句调用**写字段（每段 4 次
//   展开，非 for 循环），原写成 `int ScanInt(bool*)`+ok 槽+循环；②P164 goto 循环。
//   残余：5 处地址 `add $8; add $4` 两步形（4.1 常量乘法展开），4.4 恒折单条
//   `add $0xc`（探针 /tmp/probe2d.cpp g2d/g3/g4/g5/g6 全折）⇒ N1 跨版本常量算术。
// [sweep2-InitScriptProducers_2] 复核确认 N1：orig/ours=123/118 gap=5 ndiff=15，
//   ORIG-only 全为 5 对 `add $X; add $4`（0x089114b2/b5、0x089114ea/ed、0x08911509/0c、
//   0x08911548/4b、0x08911567/6a —— 即奇数索引 [1]/[3] 的两步地址）。探针
//   /tmp/sw2/p7.cpp 两形（`&a[3]` 与 `&a[2]+1`）在 4.4.7 与 4.4.6 后端下均折单条
//   `add $0xc` ⇒ 源形不可择，地址值等价，N1 封顶。
#include "InitScriptProducers_2/ImportSecurityReward.inc"
}

// ============================================================================
// importEquipmentPartSetInfo —— ORIG T 0x089073d9（init.md:4749，
// "Etc/EquipmentPartSet.etc", ENUM_MODULE_TYPE 3）。返回 bool（真=成功）。
// 解析 [equipment part set] → g_equipmentPartSetMap_；随后逐集合
// new STEquipmentScript 并 ImportEquipmentScript(name,false,module,key)；
// 成功后把该脚本 m_paramMap678[-1]（存在时）迁移为键=pieces.size() 的条目。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N89 封顶（原 DIFF 336）ORIG
//   0x089073d9 orig/ours=374/349，p167 仅剩 `_ZNKSt17_Rb_tree_iterator<
//   pair<const int,STEquipmentPartSetData>>::ptEv` ×7（4.4 内联头内 operator->，
//   4.1 out-of-line；N89 类不可控）。本波修：①operator== 正形（去 operator!=）；
//   ②`data.m_field0 = ScanInt((bool*)0)` 去独立 key 局部；③do-while 复用
//   ScanStr 返回值（去重复调用）；④`paramMap[-1]` 替代 `def->second`（补
//   map::operator[] 调用点）；⑤P164 goto 主循环。
// [sweep2-InitScriptProducers_2] verdict=N 封顶（ndiff 467→4，gap 25→0）：
//   orig/ours=372/372。本波修：①删 `STEquipmentScript* script` 具名局部——
//   ORIG 0x08907619 new 结果经 ebx 跨 ctor 保活后直存 it->second.m_script
//   （P0/P183），后续 10 处 `call operator->; mov 0xc(%eax),%eax` 逐点重读；
//   ②删 `paramMap` 引用局部，`it->second.m_script->m_paramMap678` 全展开；
//   ③find/end 改单表达式（ORIG 0x08907741 先 end、0x08907775 后 find，P293）；
//   ④do-while 的 ok 改 `register bool`（ORIG ebx 承载、无栈槽，P231）。
//   残余 4 行 = ORIG `test %ebx,%ebx; je exit; jmp body` vs 4.4 `testb %bl; jne body`
//   （bool 寄存器宽度 + do-while 尾布局，跨版本，N 封顶）。
#include "InitScriptProducers_2/importEquipmentPartSetInfo.inc"
}
