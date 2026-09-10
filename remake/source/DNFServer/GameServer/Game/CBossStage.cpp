// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBossStage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CBossStage 实现（批3：从 GameStubs.cpp 迁移；
// FixCBossPlay：加 ctor/dtor，CBossStage : CBossPlay）。
// StageVirtuals：补 4 个新增虚函数 handleMoveMap/handlePickupItem/
// handleDropItem/handleKillMonster（ORIG 0x0814f66a/0x0814fa76/0x0814ff4a/
// 0x08150388），使本 TU 发出 _ZTVN8WongWork10CBossStageE。
//
// ============================================================================
// b28f-SweepCBossStage（本波）判词汇总：四数 228/368/2/7（HEAD 基线，同口径
//   headref 复测确认）→ 228/368/near=3/diff=6（fresh 18:21:04）。逐件：
//   ① SendMapInfoPacket：**真语义 bug 修复**——p167 命中 ORIG 多 1 次
//      `put_short(int)`、我方多 1 次 `put_int(int)`；ORIG 0x0814d083
//      `mov -0x84(%ebp),%eax; call 080d9ea4(put_short)` = `packet.put_short(
//      item.m_itemIndex)`，原写 put_int 令 0x1e2 包 itemIndex 之后全部字段错位
//      2 字节。修后 p167 该对差集归零（该件 AE 分类不变：clean() 把 call 目标
//      归一为 <T>，语义修复不体现在四数上）。
//   ② handleMoveMap：DIFF→NEAR。`if (!moveNextStage()) return 0;` 改 P292 正极性
//      包裹 `if (moveNextStage()) { … }`（ORIG 0x0814f84e `call; test %al; je
//      <失败尾块>` 无 xor）⇒ 消 N122 xor 物化 1 条 + 尾块布局。残差仅 N7
//      （`_checkMemberDie` 间接调用目标 ORIG %edx / 4.4 %eax，3 条）。
//   ③ killMonsterDropItem：语义块 2→0。ORIG 0x0814d648..d68b 是三段序：三个
//      C1（-0x50/-0x58/-0xc8）全发完**之后**才是三个 clear ⇒ 原码「先声明三对象、
//      再统一 clear」；原「声明+clear 交错」形错位。残差=纯槽分区（N82/N126）。
//   ④ dropItem：`int attachType = script->GetAttachType(); switch(attachType)`
//      改 P251 直切 `switch (script->GetAttachType())`（ORIG 0x0814e212
//      `mov [S],%eax; cmp $0x5,%eax` 值无栈槽往返 ⇒ 无具名局部）⇒ 消
//      `cmpl $0x5,slot` + shl 索引形成对。残差=帧 0xd4(ORIG)/0xc4 + 整体槽平移。
//   ⑤ KillMonsterGetExp：三形实测（本形/AE 差 12 块；标量全头部声明→our 229
//      条劣化；float 移入 for 体→AE 差 18 块劣化）⇒ 4.4 按「先用者浅」分区与
//      ORIG 4.1「先用者深」方向相反，声明位置不可翻（P212 反向 + N82附）封顶。
//   ⑥ NormalPickupItem：N7×3 + N82 槽（agency 内层块槽被 4.4 lifetime 复用给尾部
//      PacketGuard，ORIG 4.1 不复用）⇒ 封顶。
//   ⑦ consistMap(gap −7)/RandomPickupItem(N7/N18a 组合绑定)/handlePickupItem
//      (N7 虚槽 + 槽分区)：p167 全 CLEAR（仅 C1/C2、D1/D2 同址别名 P93 豁免）
//      ⇒ 按 §6.2 判「语义完成、byte 层 N 封顶」。
//   ⑧ diff 列表仍含 `_Z41__static_initialization_and_destruction_0ii`：N123
//      每 TU 同名 local 副本撞名伪差，非实现缺口。
//   ⇒ 本 TU 剩余真缺口 = 0（p167 全 CLEAR）。
// ============================================================================
#include "CBossStage.h"
#include "WarField.h"   // MapInfo::Clear / map_item / map_monster
#include "GameRand.h"   // get_rand_int

#include <cstring>

#include "CUser.h"                  // CUser/CHackAnalyzer/cUserHistoryLog/ENUM_ERROR
#include "QuickParty.h"         // 先于 CDungeon.h：QUICKPARTY_H 守卫去重
#include "CDungeon.h"               // CDungeon/get_standard_level/RandomList
#include "CMap.h"                   // CMap/_mapMonster/_mapItem 完整类型
#include "CItemGenerator.h"         // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CGenUniqueNo.h"           // WongWork::CGenUniqueNo::genIPGNo
#include "CParty.h"                 // get_user/checkValidUser/getManager/m_battleField.m_bloodState
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"           // find_map/find_item/G_CDataManager
#include "CInventory.h"             // gain_money/delete_item/insertItemIntoInventory
#include "MonsterIndependentDrop.h" // generateIndependentItem/CAICharacter 权威镜像
#include "CMonster.h"               // CMonster::generateIndependentItem
#include "GlobalData.h"             // GlobalData::s_systemTime_
#include "CSystemTime.h"            // getCurTickCount
#include "CSpecialItemRoutingManager.h"  // StartRouting（RoutingPickupItem）
#include "CGameManager.h"               // G_CGameManager
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义

namespace WongWork
{

// Ctor（ORIG 0x0814bd0c）：CBossPlay 基类、置 vtable（编译器自动）、构造
// MapInfo、Clear、get_rand_int(60000) 写 m_assignItemIdx/m_dropItemCnt，
// 函数体内依次 m_stageCur=1、m_dungeon=0、m_difficulty=0。
#include "CBossStage/CBossStage.inc"
}

// Dtor（ORIG 0x0814bdd6）：置 vtable（编译器自动）、m_stageCur=1、
//   m_dungeon=0、m_difficulty=0、Clear、rand×2、析构 MapInfo、基类（编译器自动）。
#include "CBossStage/dtor_CBossStage.inc"
}

// ORIG 0x0814bea2：m_startTick=0、m_endTick=0、MapInfo::Clear、
//   memset(m_useItemCount, 0, 0x10)（CBossPlay::m_useItemCount[4]，ORIG this+0x9c8）。
#include "CBossStage/reset.inc"
}


// ============================================================================
// 新增虚函数（虚表槽 [8]..[11]，紧随 CBossPlay 6 槽之后）
// ============================================================================


// ---- handleMoveMap（ORIG 0x0814f66a）----
// 换图入口：_checkMemberDie 短路 → 逐成员反外挂计数 + MapClear 日志 →
// moveNextStage → randomMapIndex →（GM 强制图/难度）→ find_map → reset →
// consistMap（失败 throw ENUM_ERROR(1)，catch 后广播 0x30 包并返回 0）→
// 记 startTick → SendMapInfoPacket。
// [b28d-62] N82：p167 CLEAR（0 missing/0 extra），273/280 助记符同序；残差为
//   帧/槽号（4.1 生命周期穿插 vs 4.4 分区复用），语义完成。ORIG 0x0814f66a
// [b28d-solo-CBossStage] N封顶：删 manager!=0 判空（ORIG 无，P171 照抄）
//   （ndiff 41→28，gap 0）。残差=N7 虚调用 vptr 寄存器 %eax↔%edx 3 条 +
//   N122 bool 取反 xor 物化 1 条。
#include "CBossStage/handleMoveMap.inc"
}

// ---- handlePickupItem（ORIG 0x0814fa76）----
// 拾取分发：stageData_.item（MapInfo@+0x9f4 的 m_itemMap@+0xa18）按 itemUID
// 查找；金币（m_item.m_addInfo==0）走 GoldPickupItem；否则 find_item 后按
// 队伍人数/存活数/rarity 分发 Routing/Random/Normal。
// [b28d-62] N89/P96：p167 显示 ORIG 用非 const _Rb_tree_iterator，我方
//   const_iterator（转换 ctor ×2 + const 版 operator 族）+ getParty 缓存差。
//   ORIG 0x0814fa76
// [b28d-solo-CBossStage] N82：去 MapInfo& stage 引用（直读 m_stageData）、
//   map_item item 默认构造 + item=it->second 结构赋值、迭代器 const_iterator
//   （ORIG 有转换构造）。残差=帧/槽分区（ORIG 迭代器槽 -0x64..-0x70）。
// [b28f-SweepCBossTower P248 档①] 返回类型 int→bool（CBossStage.h:54 同步）：
//   ORIG 调用点 0x08143dbb `call; mov %al,-0x9(%ebp)`（无 test/setne ⇒ 单字节
//   返回）；返回类型不入 mangled（_ZN8WongWork10CBossStage16handlePickupItemEP5CUseribRc
//   不变），vtable 槽序/sizeof 零影响；本 TU 四数改前/改后 identical=228
//   ae=368(596) near=2 diff=7 全等（零回归）。
// [b28f-SweepDeps 命中 · P0「ORIG 无槽」形] `bool canRoute` / `bool canRandom`
//   具名局部删除、条件内联进 `if`：ORIG 0x0814fbc0 段为**寄存器直测**
//   （`mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al; je`，**无槽往返**）；
//   我方具名 bool 使 4.4 落槽（`mov %al,-0x1a; cmpb $0x0,-0x1a; je`，每处 +2 条）。
//   实测：immcmp 1/3→**1/1**、test 15/13→**15/15**、our 338→**336**、gap −5→**−3**
//   （ORIG 333）；TU 四数不变（228/369/2/6）⇒ 严格改善保留。
//   残余 = 迭代器/聚合槽分区（ORIG it -0x64..-0x70 / 我 -0x28..-0x34，
//   帧两侧同为 0xec）⇒ N82附 封顶。
#include "CBossStage/handlePickupItem.inc"
}

// ---- handleDropItem（ORIG 0x0814ff4a）----
// 丢弃：dropItem 成功 → 取 +0xa34 序号（-1 回执失败包）→ MapInfo::Add_Item →
// 0x28 广播包 → 0x32 成功回执给丢弃者；失败/越限 → 0x32 失败回执。
// [b28d-62] N82+P52：p167 EXTRA 1×getParty（ORIG 缓存 party 指针跨调用），
//   其余 0/0；280/280。ORIG 0x0814ff4a
// [b28d-solo-CBossStage] N82附封顶：m_party 直读 + 三目布局（ndiff 202→5）。
//   残差 5=ORIG -0x78 seq 具名副本槽（4.1 生命周期穿插 vs 4.4 分区，新增标量
//   即重排类对象槽，三形实测劣化 241）。
#include "CBossStage/handleDropItem.inc"
}

// ---- handleKillMonster（ORIG 0x08150388）----
// 击杀：killMonsterErase → partyType==1（PowerWar）时 0x26 广播即返回 →
// KillMonsterGetExp → KillMonster 日志 → killMonsterDropItem。
// [b28d-62] N82：p167 EXTRA getParty/get_party_type 各 1（ORIG 缓存结果），
//   188/193。ORIG 0x08150388
// [b28d-solo-CBossStage] AE：m_party->m_battleField.m_bloodState 直读（消
//   getParty/get_party_type call）；内联 curExp/roleType（P0）；KillMonsterGetExp
//   返回类型 int→bool（P248 档①，.h 同步，mangled 不变）。ndiff 131→0。
#include "CBossStage/handleKillMonster.inc"
}

// ============================================================================
// 非虚方法（逐函数按 docs/class_func_reports/WongWork__CBossStage/ 报告转写）
// 字段速查（均已具名，偏移仅作对照）：
//   m_dungeon(+0x9f0)  m_stageData(MapInfo@+0x9f4；m_mapIndex+0x9f8、
//     m_assignItemIdx+0xa30、m_dropItemCnt+0xa34、m_mapBasisLevel+0xa3c)
//   m_stageCur(+0xae0)  m_stageMax(+0xae4)  m_difficulty(+0xae8)
//   m_expParamA(+0xaec)  基类 CBossPlay::m_useItemCount[4](+0x9c8)
// ============================================================================

// ---- moveNextStage（ORIG 0x0814d1b6）----
// 怪物清空（monsterMap.empty()）且 m_stageCur < m_stageMax（无符号比较）
// 时 m_stageCur+=1 进入下一层。
// [N18a 封顶] 残差仅 0x814d1dd `mov 0xae0(%eax),%edx` 单步直落；4.4 恒折
// `mov %eax; mov %eax,%edx` 两步（N18a：单段 regmove 不可复现），其余 27 条逐条同形。
// [b28d-62] AE：删同型冗余 cast（P41 变体：cast 致 `mov %eax,%edx` 中转）后
//   28=28，p167 CLEAR。ORIG 0x0814d1b6
#include "CBossStage/moveNextStage.inc"
}

// ---- randomMapIndex（ORIG 0x0814f4fc）----
// acc/roll 先置零；栈上 vector 拷贝后遍历；getRandomTowerMapIndexes 在
// find 与 end 处各调用一次（ORIG 两次外呼）。
// [b28d-62] N82：p167 CLEAR，109/112。ORIG 0x0814f4fc
// [b28d-solo-CBossStage] AE：P0 去 receiver 指针局部 + const 视图内联调 const
//   find（消转换构造 call）；stageCur 声明/赋值分离复现槽序。ndiff 58→0。
#include "CBossStage/randomMapIndex.inc"
}

// ---- killMonsterErase（ORIG 0x0814d214）----
// find(uid) → const_iterator 判 end() → mob=it->second、outCount=(uchar)+0x14、
// erase(by key) → true；未找到 false。
// [b28d-62] N82：erase key 改内联 `(int)uidParam`（新临时槽）后 68=68；残差仅
//   uid 与 find-sret 槽互换（P112 槽序不可推）。p167 CLEAR。ORIG 0x0814d214
// [b28d-solo-CBossStage] AE：found 具名 iterator 中转（P50 复活）消 uid/find-sret
//   槽互换；erase key 保持内联 (int)uidParam。ndiff 8→0。
#include "CBossStage/killMonsterErase.inc"
}

// ---- KillMonsterGetExp（ORIG 0x0814d300）----
// 对每名有效队员：get_mob_reward → getMonsterTotalExp → 死亡之塔两层费率 →
// sumGainedEachExp → calcHelpAbuseRatio → gain_exp_sp(+Noti) → CheckQuestMonster。
// [b28d-62] N82：p167 EXTRA getParty/get_party_type 各 1（ORIG 缓存结果）。
//   ORIG 0x0814d300
// [b28d-solo-CBossStage] N82封顶：exp 写回/getMonsterTotalExp + float 截断费率
//   （原 double+0.5 语义错）+ m_party/m_dungeon 直读 + m_expParamAE(P254)。
//   残差 44=4.4 float 浅槽 vs ORIG 深槽（标量分区，N82附）。
#include "CBossStage/KillMonsterGetExp.inc"
}

// ---- SendMapInfoPacket（ORIG 0x0814ce12）----
// [b28d-62] 修: P96 迭代器改非 const（消转换 ctor ×4 + const 版 operator 族，
//   263→243）；余 N82 帧/槽差（sub 0x3dc vs 0x380）+ getParty 缓存。ORIG 0x0814ce12
// [b28d-solo-CBossStage] N104：补 put_short(m_fieldb) + m_addInfo 改 put_int
//   （真 bug：原漏写包字段）+ map_item 栈拷贝 + m_stageData 直读。残差 6 条=
//   EH 尾块相对序（4.4 就地内联 vs 4.1 尾块）。
#include "CBossStage/SendMapInfoPacket.inc"
}


// ---- GoldPickupItem（ORIG 0x0814f0e6）----
// 金币按存活队员平分（拾取者拿余数），逐槽写 0x27 包并 erase 掉落物。
// [b28d-62] N82：p167 EXTRA isMemberAlive/getParty 各 1（ORIG 缓存结果）。
//   ORIG 0x0814f0e6
// [b28d-solo-CBossStage] AE：m_memberAlive 内联(P119) + m_party 直读 +
//   P226 语句表达式消 valid 槽 + baseShare 副本 + member=0 预清(P198①)。
#include "CBossStage/GoldPickupItem.inc"
}

// ---- RoutingPickupItem（ORIG 0x0814e57a）----
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814e57a
// [b28d-solo-CBossStage] AE：删具名 bool failed（P226/P158）；`while (ret != 0)
//   {…return false;}` 复现 ORIG setne;test;je 物化（ndiff 33→0）。
#include "CBossStage/RoutingPickupItem.inc"
}


// ---- NormalPickupItem（ORIG 0x0814ecec）----
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814ecec
// [b28d-solo-CBossStage] 封顶 N7+N82：user==0 正向包裹（P222，ORIG je 814f0d5）、
//   isAvatarItemType 正极+avatar 直落（P168a③b）后 98→37；残差=N7 虚调用
//   %edx↔%eax ×3（12 条）+ agency/PacketGuard 槽 -0x34/-0x40↔-0x40/-0x4c
//   （4.4 聚合优先低槽，N82附）。p167 CLEAR。
#include "CBossStage/NormalPickupItem.inc"
}

// ---- RandomPickupItem（ORIG 0x0814e684）----
// 存活且背包可容纳的成员各掷 1..100，最高者获得物品；广播含四人掷点。
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814e684
// [b28d-solo-CBossStage] 封顶 N7/N122/SAVE_EXPR/N82：bestRoll int→char（ORIG
//   -0x29 `cmp mem,%al; jle` 字节比较）、m_party 直读（消 getParty 局部槽）、
//   member 预清(P198①)、短路 `member==0||!checkValid`(P224)、winner!=0 正向
//   包裹(P222) 后 258→175；残差=N7×3(12 条)+N122 xor 1+4.4 SAVE_EXPR 复用
//   rolls[i]（ORIG 两次独立读）+槽/帧分区。p167 CLEAR。
#include "CBossStage/RandomPickupItem.inc"
}

// ---- dropItem（ORIG 0x0814de1a）----
// 按 invenType 取出物品构建掉落 map_item，再从原容器删除并记流通统计。
// type：0=背包/金币、2=仓库、3=装备栏、7=造物。
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814de1a
// [b28d-solo-CBossStage] N82：p167 CLEAR。if 链改 switch(type)（P208 决策树）。
//   残差=槽/帧分区 + case 0 金币块内联结构（ORIG 金币输出在 case 0 内并 jmp 删除段）。
// [b28d-solo-CBossStage] 封顶 N-switch跳表+N76：case 序改 0,2,3,7（P46 叶块物理
//   序）、case 0 内嵌金币块+公共段只判 tmp.m_addInfo（P222）、attachType 改
//   switch（跳表 0x8b7511c）且 case3 用 `!tmp.m_field0`（P248 档②）、删除段改
//   switch+去三目+deleted bool（P158）、m_amp 按 guid/head 视图序、out+0x00 写
//   m_count 后 169→12；残差=4.4 对 6-case 用二分树 vs ORIG 跳表（跨版本 switch
//   展开，N）+m_amp0Int store 序（this-first vs value-first，P211 旋钮已穷举）。
//   p167 CLEAR。
#include "CBossStage/dropItem.inc"
}

// ---- killMonsterDropItem（ORIG 0x0814d628）----
// 怪物/APC 掉落生成 → 逐件构建 map_item（count=1、dropIndex=b、
// createTick=当前秒）入 MapInfo → 0x26 包广播。
// [b28d-62] N89+N84：const_iterator 族/put_binary(char*) 签名/memcpy + getParty×3
//   缓存差。ORIG 0x0814d628
// [b28d-solo-CBossStage] N82：p167 CLEAR（0/0）。补 newItems.push_back（真 bug：
//   原 newItems 恒空 ⇒ 0x26 包不广播物品）+ put_binary PKc 重载 + 结构赋值替
//   memcpy + P13 end 缓存（两个转换构造）+ G_CDataManager/m_party/m_dungeon 直读。
//   残差=帧/槽分区（527 条超大函数）。
// [b28d-solo-CBossStage] 封顶 N82/N7：AI 分支前置（ORIG 0x0814d699 `jle` AI 直落、
//   mon 跳转，P47）、ref.m_field3e_byte 字节视图（消 and/or 4 条）、ref 写序
//   rateSlot→field34、hasTowerItemDrop 调用点 2 条形（CDungeon.h bool，已 R2 广播）
//   后 327→224；残差=槽/帧分区（ORIG 帧 0x21c vs 0x20c）+ map_monster 构造次数
//   （ORIG 复用 -0xf50 单对象）+N7。p167 CLEAR。
// [b28d-62 本轮] 修 5 处真缺口：m_item 赋值先于 m_itemIndex、m_dropItemCnt 后置++
//   （0x0814d95b 复用 eax）、Add_Item 先于 push_back、countIdx/itemCount 预清 0
//   （P198①）、put_byte(itemCount) 非立即数、isAvatarItemType 两处正极性、
//   period/pEquip 具名局部（0x0814db51/db58）⇒ 527/527 slotnorm 0 块，DIFF→NEAR；
//   残差仅槽号分区（N82/N126）。
#include "CBossStage/killMonsterDropItem.inc"
}

// ---- consistMap（ORIG 0x0814bef4）----
// 按源图布怪：AI 布点 → AP 随机怪（预算扣减）→ 脚本怪列表 → 特殊被动物品。
// 任一 AI 查询失败返回 false。map_monster 具名字段映射：
//   m_field11 半字 = ORIG +0x11/+0x12 两字节写、m_field13/m_field14 同名、
//   m_dropCnt(+0x2c) 收布点/脚本怪掉落数。
// [b28d-62] N89+N82：const/非 const 迭代器族 + map 拷贝 ctor + getParty。
//   ORIG 0x0814bef4
// [b28d-solo-CBossStage] 待办：p167 剩 8 missing（getIdx/getAppearancePoint/
//   get_standard_level 各 -2、G_CDataManager -1、_mapItem C1Ev -1、at -1、
//   getItemGenerator -1）。已修：G_CDataManager 直调 ×5、apStat 默认构造+赋值、
//   (*it).m_type（P167）、mobs/items 去 const。927 条超大函数，槽/帧分区为主。
// [b28d-solo-CBossStage] 封顶 N82：p167 修 getIdx×2/getAppearancePoint×2/
//   get_standard_level×2/G_CDataManager×1/getItemGenerator×1（全部改内联重读，
//   P157/P0）+ find 极性 `!=`（P220）+ upper_bound/begin 缓存（P13）+ at const 版
//   + memcpy→结构赋值（N84）；残差=_mapItem C1Ev 发射点（4.4 list move 赋值 vs
//   ORIG 元素默认构造，N86 族）+map_monster C1/D1 计数+槽/帧分区（0xfbc vs 0xf9c）。
//   927 条超大函数，语义完成（调用点差集仅剩 1 项结构性）。
// [b28d-62 本轮] 修 3 处真缺口：map_monster 提升到三循环外（ORIG 仅一枚
//   -0xf50 对象，P0 槽证据；帧 0x161c→0xfbc 对齐、-29 条）、第二段 AP 循环
//   改底部条件 while（ORIG 0x0814c240 `jmp cond`，P283）、等级夹取拆嵌套
//   if（ORIG 0x0814c704 双判）、refGen1/refGen4 构造提前到 objectDropItems/
//   item_list 之前 ⇒ 929/920（gap +9，其中 6 条为 list 拷贝构造的 4.4 move
//   折叠，N 类）⇒ 本件无我方缺失的语义段；p167 CLEAR。
#include "CBossStage/consistMap.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814f426（docs/class_func_reports/WongWork__CBossStage/
// RandomConsistMap.md 全文，GlobalBossAndAltar 2026-08-24）：
// randomMapIndex 失败 → false；GM 且 m_gmBossMapIndex(+0x796d0) 非空时强制该图并清零；
// find_map 空/consistMap 失败 → false；成功记 m_startTick(+0x9e8) 并返回 true。
// ----------------------------------------------------------------------------
#include "CBossStage/RandomConsistMap.inc"
}

}  // namespace WongWork