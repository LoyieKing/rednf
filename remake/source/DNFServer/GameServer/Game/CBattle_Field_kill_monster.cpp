#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBattle_Field_kill_monster/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CGlobalEffectManager.h"   // isAffectedEffect / getEffectValue
#include "CSystemTime.h"            // CSystemTime::getCurTickCount
#include "GlobalData.h"             // s_globalEffectManager_ / s_systemTime_
#include "LogManager.h"             // LogManager::logFormat
#include "CDataManager.h"           // getDropRate / find_monster（桥清零）
#include "CKillMonsterInfo.h"       // 击杀/奖励链六谓词（桥清零）
#include "CHellPartyItemStatistic.h" // 地狱派对统计单例/发送（桥清零）
// [N5 2026-09-11 · P139 符号轴] ORIG 局部静态
// `_ZZN13CBattle_Field12kill_monsterER11PacketGuardR11map_monsterRjRK17STKillMonsterDataE19__PRETTY_FUNCTION__`
// @0x08c23400（size 0x5e）内容 = "bool CBattle_Field::kill_monster(PacketGuard&,
// map_monster&, uint&, const STKillMonsterData&)"。旧实现用硬编码 s_bfkm_pretty 供
// LogManager/cMyTrace，运行期文本虽一致但**该符号不发射**（nm 该 TU 0 个 prett 符号）；
// 改用 __PRETTY_FUNCTION__ 后由编译器生成同文串。
// 定义处第 3 形参拼写必须是 `uint&`（GCC 取**定义处**拼写并保留 typedef 名——M3S4
// 「Arad_INTER_kAradJumpingCharacter::sendHelperBox(CUser*, byte, byte)」先例）；声明侧
// 保持 unsigned int& 不变，底层类型相同 ⇒ mangled（_ZN13CBattle_Field12kill_monsterE…RjRK…）
// 逐字符不变。typedef 必须置于**全局作用域**（namespace 内会打印限定名）。
typedef unsigned int uint;
#define BFKM_ROUND(v) ((int)((v) + 0.5f))


// ================= kill_monster 主函数 =================

#include "CBattle_Field_kill_monster/kill_monster.inc"

