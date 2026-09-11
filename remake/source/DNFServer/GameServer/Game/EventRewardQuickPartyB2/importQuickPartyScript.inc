// ORIG T 0x08a6e282。双遍结构（ORIG 函数级静态 __lookup/__init_lock/__inited，
// SpinLock 双检建表）：首令牌触发 7 个 (段名,id) 登记后回到派发块重新查表，
// 当前令牌被正常处理（与 importTitleBookEtcScript 同构）。主循环仅以
// [/quick party]（返回 true）或主扫描失败（返回 false）退出；节内子扫描失败
// 一律回到主循环继续取令牌；未知令牌（id==-1）跳过。
//
// 各节写入目标（ORIG 偏移 ↔ QuickPartyData 具名字段，CQuickPartyRewardManager.h）：
//   [level section]        → +0x00 m_vec00（vector<int>）
//   [unable to quickparty] → +0x0c m_dungeonIdxList（vector<int>）
//   [increase gold card]   → +0x18 BasicReward.m_levelDatas（LevelData{率%,档}）
//   [hell ratio]           → +0x30/0x34 m_basicReward.m_field18/m_field1c、
//                            +0x24 BasicReward.m_hellDatas（LevelDataHell）
//   [reward]               → +0x38 m_randomRewards map insert（键=buff 类型）
//   [random reward prob]   → +0x50 m_probs[type][i]（i<7；type≤10 才写）
//   [require party num]    → m_minCounts[dungeonType][diff]
//                            （QuickPartyData +0x1a0 区，行 stride 0x10；
//                            ORIG 寻址 (type*4+diff+0x68)*4 完全一致）
// b28d-solo-EventRewardQuickPartyB2 判词：DIFF（§6.2 三件套达标，byte 层 N 封顶）。
// 已修真缺口：①7 段注册由「循环头统一插入」改 ORIG 的 case 内 fall-through 链
//   （含 case 0 哨兵、goto re_lookup 双检、switch 后 leave+__inited+goto）——
//   消除 pair 值构造/make_pair/临时 ~pair 共 10 类 EXTRA，改 map::value_type
//   引用构造（pair<const char* const,int>::pair<char const(&)[N],int&&>）；
//   ②补 token/value 的 Ss::clear、LevelData entry 提为函数级复用槽、lv 复用
//   -0xf8、ok=ScanStr 两跳槽往返、ScanInt(&lv) 复用；③[reward] 的
//   !=+continue 改 == 包裹（消 operator!= EXTRA / operator== MISSING）；
//   ④while(ScanType) 改 for+if(!ScanType)break（P164 抗循环旋转）。
// 验收：p167 调用点 97=97 CLEAR（0 missing / 0 extra）；栈槽集 42/42 逐项全等；
// gap=2（ORIG 740 / ours 742）。归一化残差 19 行仅剩：LevelData::C1 后 4.4
// EH 边界 jmp+nop（P223/N121 族）、循环内 if(!ok)break 的 je+jmp vs 4.1 jne
// （N122 族）、while 条件块物理位置差（difflib 对齐）。语义与 ORIG 逐块一致。
// 依赖：LevelDataRandomReward::m_field0 int→RandomBuffType（.h 改动，R2 已广播）。
// [P6S6 2026-09-11 追加钥匙] 主循环 `while (true) { if (!ScanType(token,true))
//   break; … }` 改为 `for (; ScanType(token,true); ) { … }`（等价重构，取消 break
//   出口）⇒ 4.4 少发 1 条跳转：ours 742 → **741**（ORIG 740，gap −2→**−1**），
//   四数不变。另一形 `while(true)+goto main_scan_failed` 实测回退到 742 ⇒ 不采用。
//   剩余 3 块：①`xor $0x1,%eax; test; jne` vs 我方 `jmp`（首测块物理位置差）；
//   ②内层 `if (!ok) break;` 的 `jne` vs `je+jmp`（N122 族）；③缺进循环前的
//   前置 `jmp`（4.1 的 while-rotate 形，4.4 恒直落）⇒ 跨版本封顶。
bool importQuickPartyScript(const char* path, QuickPartyData* data)
