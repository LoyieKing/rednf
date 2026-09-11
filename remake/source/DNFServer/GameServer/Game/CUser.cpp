// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUser/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng（FatigueUp/set_server_fatigue_count）
#include "GameRand.h"
// ============================================================================
// df_game_r 还原 —— CUser（G1-3 批次，在线用户会话聚合根）
// 布局见 CUser.h；本文件按 ORIG 反汇编（docs/class_func_reports/CUser.md）
// 逐函数转录，identical 口径验证：source/toolchain/check_tu_game_orig.sh。
// ============================================================================
// b28d-S402_CUser 单文件泳道（2026-09-09）：清单 125 件；TU 读数
//   identical=704 / ae=1503（合计 2207，起点 2195 = +12）/ near=4（起点 6）/
//   diff=119（起点 123）/ missing_in_game=530；结果 TSV /tmp/b28d_solo_CUser_result.tsv。
//   本轮 AE 转正 8 件：AddMoneyPlus、AddMoneyMinus、AddDBLogItem、RemoveCharac、
//   AuctionPreCheck、IsRestrictedGoods、update_old_pvp_point、is_equip_aura_avatar。
//   转 NEAR：SetGameMasterMode（P52 链式 receiver，待 CUser.h 返回 void→bool）。
//   p167 批量扫描 19 件差集，已清 6 件（is_equip_aura_avatar/update_old_pvp_point/
//   ResetTrainingQuest/GetLimitItemUsage/CheckCoolTimeItem/HasCurCharacRentalItem）
//   + 2 件 make_pair 左值引用 cast（_deleteSpecificItemInven/_Cargo，IRmRiE）。
//   P0：本文件 0 处 register-asm 绑定（历史注释提及不计）。
//   待 Main：① CUser.h:3143 SetGameMasterMode 返回 void→bool（ORIG 尾恒 mov $1,%eax）；
//   ② CUser.cpp 加 -march=i486 路由（x87 fucompp/fnstsw/sahf 族，P230，需双后端读数）。
// b28d-solo-CUser 泳道判词总表（2026-09-09 S178_CUser3 收口；逐件明细
// /tmp/b28d_solo_CUser_result.tsv，清单 /tmp/b28d_tu/CUser.tsv **136 件**；
// 口径 check_tu_routed.sh + /tmp/s178_nd.py 逐件 ndiff）：
//   IDENTICAL/AE 11 件：getBlueMarble / is_equip_aura_avatar / get_area /
//     gainGuildSkillExp / update_old_pvp_point / SendCmdPacket / recover_stamina /
//     SetManagePoint / givePvPSkillTree / GetBlackListCount / SendClientCrashDownLogToLogDB。
//     （S163 修复：SetGMUpgradeMode、setLastLotteryTime——P211 同型律，CUser.h:908 enum
//      Main 代落、CMCAPManager.h:81 ulong 本泳道落，二者已按 Main 要求移出清单。
//      S178 本轮修复：① SetManagePoint——register unsigned int lvmax→int（与
//      m_manageLevelMax(int) 同型）⇒ 复现 ORIG `mov %eax,-0x10; mov -0x10,%ebx` 槽往返；
//      ② givePvPSkillTree——P214①/P59 正条件 if 包裹（`==0 continue`→`!=0 {body}`）⇒
//      复现 ORIG `call;test %al,%al;je` 无 xor 段，285=285；③ GetBlackListCount——
//       P211/P274 字段侧异型化：SigTypes_C.h:391 m_blackCount int→unsigned int + 删冗余
//      (unsigned) cast ⇒ RHS-first+%edx 中转；④ SendClientCrashDownLogToLogDB——P159 尾形
//      判返回类型 void→int + `return 1`（CUser.h:3228 Main 代落）。四件均 fresh 复编 AE。）
//   NEAR 5 件：AddDBLogItem（N82/P112 begin-sret 槽 + N98 首次使用序）/ AddMoneyPlus、AddMoneyMinus
//     （N81/N7，+= load 中转；BM1 unsigned 化净负已回退）/ ResetItemByScript（P75 实参装载中转）/
//     gainExpAsUpperMember（N82/P112 槽基差 8）。
//   DIFF 120 件归因分组（每件函数头均有 b28d-lane5/b28c-L5/b28d-solo-CUser 判词）：
//     N82/P112 槽号分配族（含 begin/sret 槽阶梯）：约 90 件（含 Send/AddItem/AddDailyItem/
//       processDelDailyItem/giveLotteryCountItem/GetEtcInfo/_SaveCharacData 等）；
//     N7/N18a 跨版本寄存器分配族 25 件：C1/D1/SendPacket/RecoverFatigue/CheckMoveTown/
//       WorkPerFiveMin/ResetDailyQuest/add_inventory_item/givePvPSkillTree/
//       updateCharacViewVisibleData/GetPcRoomRentInfo/GetDimensionRentInfo/GetInventory/
//       GetInventoryInfo/getTagCharacInventory/GetTicket/getCharacInfoDetail/GetSkill/
//       get_skill_command_buf×2/DoGetCeraPointItem/delete_ontime_reward_list 等；
//     N18a/P200 拓宽中转：set_grow_type/ResetTrainingQuest/SetPvpIndex/DisConnSig/
//       master_new_skill/UpdateData/FatigueUp；
//     P168a/P248 setcc 物化方向：deleteSpecificItem/update_pvp_point/CacheCharacterMemory/
//       CheckCoolTimeItem/SendEventJarOutputItemStatistics/UpdateCurCharacVisibleToCharacViewMemory；
//     P11 符号扩展：master_new_skill/doLinkCharacDisconnect/make_charac_stat/SendOpenflag/
//       _UpdateUserInfo4Guild/SendLoginToPCRoomServer；
//     N85 x87 lowering：_processSpeicalContinuousTimeItem/reqHumanCertify4ClearMap；
//     N97/P230 cmov：gainCurCharacFatigue/make_basic_info；N96/P168a④：AuctionPreCheck；
//     cwtl fold：GetUID（4.4 把 `(unsigned short)(short)` 折成单 movzwl，4.1 留 cwtl）；
//     uchar 直测物化：exceptedStateMoneyLog；N104 EH 尾块序：add_inventory_item。
//   本轮实测无效记录（已写回函数头）：AddDBLogItem info 声明前置、gainExpAsUpperMember
//     全头部声明（P212/P309）、is_fighting register 缓存 —— 均 N98 首次使用序/N7 封顶。
//   S178 收口轮增量（均已 fresh 复编）：
//     改善：NextCharacMoneyLog 30→9（P0 槽往返反推：删 current/old 具名 time_t 局部，
//       localtime_r 直传 &now / &m_money[2] 字段地址；残余 N30/N4 常量分解差）；
//       IsGuildSkillLearn 44→36（P171 删 gdb 判空 + P183 去 skills 指针缓存 + count int 化
//       + P212 全头部声明槽序 gdb→count→i；残余 N95/N115 帧 0x28 非 16k+4 量化）；
//       HasCurCharacRentalItem（P198 指针侧两句式复现落槽判空；残余 N82/N7 push ebx）。
//     负结论：AuctionPreCheck 纯 register bool 实测反劣 12→19（已回退，N96+N18a 封顶）；
//       exceptedStateMoneyLog uchar 字段显式 `!=0` 无效（仍 setne，字段不可 bool 化）。
//     待 Main 代落（CUser.h 冻结头，三项，均 mangled 不含返回类型）：①
//       SendClientCrashDownLogToLogDB void→int（ORIG 0x0864870e 尾 `mov %ebx,%eax` 返 1）；
//       ② IsGuildSkillLearn bool→unsigned char（ORIG 尾 `movzbl (%eax),%eax` 直返无归一，P9）；
//       ③ UserMercenaryInfoMgr::CheckValidAppend int→bool（ORIG 0x081b4300 调用点
//       `call;xor $0x1,%eax;test %al,%al;jne`，P168a/P292）。
//   p167 调用点差集复核（巨件，本轮修复）：make_basic_info 407/404（缺口 10→3；已修
//     strlen×3、teamName getter×3、GetEnterCount、getCurCharacR 缓存×2）；CheckQuestMonster
//     124/122（已修 put_binary 重载 char*→const char* ×4，P167 经典重载选错）。
//   余缺口待后续（语义待查，非形态差）：make_basic_info put_byte×2+put_int×1；
//     CheckQuestMonster getCurCharacInvenR(const)×2/GetAvatarItemMgrR(const)×2/
//     MonsterKillItem::C1/GetInvenRef const×1/set_add_info×2/getItemList×1；
//     DisConnSig G_GameWorld×3+getStatisticsToObserveCrash×3+CStreamGuard dtor×2；
//     _SaveCharacData CStreamGuard dtor×2/ptEv/setBit_Byte；UpdateLogout CStreamGuard
//     dtor×2/ptEv/GetCharacStartPlayTick/get_server_fatigue_day_data（EXTRA get_acc_id×2）；
//     master_new_skills getCurCharacSkillR×5/SkillSlot SFP×6 等；getCharacInfoDetail
//     vector<_Charac_info>::operator[] vs memcpy。
//   P167 CLEAR 巨件：UpdateData 271/271、FatigueUp 100/100、update_pvp_point 102/102、
//     set_grow_type 115/115、getCharacInfoDetail 除 1 处 operator[]/memcpy 外 72/72。
// ---------------------------------------------------------------------------
// b28d-solo-CUser 第二轮（S318_CUser，2026-09-09）：以 p167 调用点差集为探针的修复
//   （逐件明细 /tmp/b28d_solo_CUser_result.tsv；TU 总 identical+ae 2181→2195）：
//   FIXED-AE：Add_RedeemInfo（P171 删防御 + P59/P214① 正向包裹 + P235 具名 m_addInfo2
//     + P0 内联 find_item；p167 EXTRA get_add_info 归零）。
//   p167 CLEAR 件（语义缺口关闭，指令残差另记）：RemoveCharac（FindIndexByCharacNo +
//     found + try/catch）、get_skill_command_buf×2（去 v 缓存逐字段 it->second）、
//     getTagCharacInfo（去 src 缓存逐字段 operator[]）、SetMercenaryInfoPacket（去 merc
//     缓存）、UpdateEffectItemInfo/UpdateCoolTimeItemInfo（迭代器 + CONTINUOUS_ITEM_INFO
//     局部 + memcpy 重写）、GetRecord（结构赋值替 memcpy + 去 rec 缓存 3 次调用）。
//   其他修复：RecoverFatigue（m_premium 直调，p167 -7 GetPremiumInfoW）、
//     deleteSpecificItem（_checkTimeoutItem 值拷贝/getMailIdx/while 单调用点）、
//     set_server_fatigue_count（map::value_type 直构）、IsRestrictedGoods/
//     SendRestrictedGoods/AddDailyItem（非 const iterator）、checkRentedItem/
//     getCurrentCharacRentalInfo/HasCurCharacRentalItem/GetSpecialLevelCharacCnt/
//     setInitQuestPoint（迭代器 cv 旋钮）、make_pair 枚举 cast ×5、memset→char[N]={0} ×5
//     （P214③/P312）、UpdateCharacView（it.operator->()×5 + guard.operator*）、
//     ChangeCharacSlot（begin()+b+1 两次 operator+）、ReCalcChattingEmoticon（guard 链 +
//     结构赋值）、_postCheckForceChangeGrowType（getQuestList + const 容器 + goto 共享
//     命中块）、reqHumanCertify4ClearMap（getPoint 3 次直调）、NextCharacMoneyLog
//     （P285 else{return;} 消尾岛）、delete_ontime_reward_list（P59/P222 块序）。
//   负结论（实测反劣已回退）：ResetItemByScript int 化/引用形、IsRestrictedGoods
//     begin/end 缓存形、exceptedStateMoneyLog 裸 if —— 维持 N93/N98/N96 封顶。
//   冻结头需求（已报 Main 代落）：① cUserHistoryLog 删用户空析构（ORIG nm 仅 W C1/C2，
//     无 D1/D2 ⇒ P317）；② CharacMoneyLog 匿名 union 增 unsigned 视图（AddMoneyPlus/
//     Minus 的 += 段 RHS-first，P235/P274）。
//   仍 NEAR/DIFF 124 件：沿用上表 N 族归因（N82/P112 槽号、N7/N18a 寄存器/折叠、
//     N85 x87、N97 cmov、P168a/P248 setcc、N18a/P200 拓宽、N104 EH 块序等）。
// ---------------------------------------------------------------------------
// b28d-PoolCUser 第三轮（2026-09-09，池 /tmp/b28d_tu/CUser.tsv 123 件、ndiff≤30 优先）：
//   TU 三数 基线 703/4/119（ae1504/合计2207）→ 现在 703/4/115（ae1508/合计2211）。
//   池前 16 件现状：getBlueMarble/get_area/gainGuildSkillExp/SendCmdPacket/
//     recover_stamina 五件**已 AE**（池快照过期）；GetUID 封 N（cwtl fold，6 形探针
//     既有）；ResetItemByScript/NextCharacMoneyLog/SetGameMasterMode/
//     gainExpAsUpperMember 封 N（N82附 槽分区/N115 outgoing/P75 装载序；本轮复测
//     gEUM double framePad 反劣 ndiff 14→16 已回退；SGMM 帧差 = 4.4 聚合/标量分区）。
//   翻 AE 4 件：① _ZN5CUser18DoGetCeraPointItemEjbR10ENUM_ERRORm——P171 删 s_pIPGHelper
//     判空 + P17 双 call 物理展开 + **P282 槽序纠错**（尾 call vptr+0x2c = _ZTV word13
//     = slot11 IPGQuery(CUser*)，旧实现误绑 IPGOutputConfirm 重载，指令数 43≠52 真缺口）；
//   ② _ZN5CUser12GetUserStateEv——P157/P48 删 state 具名缓存逐使用点重读 + P222 否定
//     早返 assault 岛前置（58=58）；③ _ZN5CUser18SendUpdateItemListE…——P45 switch 改
//     if-else-if 线性链（ORIG `cmpl $K,0x10(%ebp)` 槽直比序 1/7/3）+ ORIG `call SendPacket;
//     mov $1,%ebx` = 返回值丢弃形（改语句调用 + return true）；④ _ZN5CUser11SetPvpIndexEs
//     ——P227 全表达式临时链 CSwitchLog(...)(fmt, get_acc_name(), getCurCharacName(),
//     (int)idx)（ORIG varargs 先于 ctor 求值 + edi/esi/ebx 跨 ctor 保活）。
//   ndiff 改善（计数未翻正）：delete_ontime_reward_list 16→6（P223 尾标签把 DEL 块物理
//     后置复现 ORIG 块序；残 6 = 两处 outgoing spill 装载序 P75）；_checkTimeoutItem
//     镜像声明 int→bool 跨 TU 同步（定义端 Main 已 P292 bool 化，ORIG 调用点 test%al
//     直判）：deleteSpecificItem 308→192、_deleteSpecificItemCargo 47→34、
//     _deleteSpecificItemInven 44→26。
//   P326（N93 翻案）按 Main 指令复测：GetSpecialLevelCharacCnt 三形并列 ①const& 15/
//     ②裸容器隐式 23/③P326 右值临时 51 ⇒ 该件非双迭代器槽互换形，P326 不适用（函数头
//     已记录）；SendPacket switch 形 4.4 产跳表合并体（68→22 指令）反劣 54→58，维持
//     if 链 + 跨版本 switch lowering 封顶。
//   待 Main：① CDungeon::m_89e（CDungeon.h:826）uchar→bool——exceptedStateMoneyLog
//     消费点 ORIG `movzbl F; test %al; je`（P292 bool），但 producer CDungeon.cpp:459
//     `m_89e = script.m_28d` 多值赋值有截断风险 + 他 TU（CBattle_Field_KillChain
//     signed char 读）消费，跨 TU 类型旋钮按 S2 归 Main；② CUser.cpp 加 -march=i486
//     路由复测（N85 x87 族：_processSpeicalContinuousTimeItem ndiff5 fucompp/fnstsw
//     /sahf 段、reqHumanCertify4ClearMap 同族），沿用上轮申报。
//   收口补记（Main 双指示已落地）：① m_89e **驳回**——producer 0x0834b39b movzx
//     裸字节搬运 + 全二进制 15 处消费零 `test BYTE PTR`，字段保 uchar；本件残差
//     重分类为跨版本 uchar 测试物化 lowering（4.1 三段直分支 vs 4.4 恒插 setne），
//     判词与地址已写入函数头（N7/N18b 族封顶）；② -march=i486 手工三组复测：
//     (a) 现路由 703/1508/4/115；(b) +i486+mtune=generic → 703/1509/**4/114**
//     （合计 2212 净 +1，_processSpeicalContinuousTimeItem 归零 AE，reqHuman
//     97→92 未归零，零回退）；(c) 仅 i486 → 674/142/2/1512 崩，-mtune=generic
//     必须同带。b 组达「净升+无回退+一件归零」但未满足双归零判据，读数已并列
//     两处函数头，落表与否待 Main。
// ============================================================================

// ---------------------------------------------------------------------------
// b28d-Lane3CUser 第四轮（2026-09-10）：判据① 全 TU jcc/jmp 目标下标逐位比对
//   （106 件有目标差；对齐归一后无可确证"目标落点语义错位"，残差全由指令数差/
//   块平移解释）+ 前 20 件修复。TU 四数 759/1593/4/105 → 759/1595/4/103（ae +2、
//   diff −2、identical/near 持平）。
//   归零 2 件：SaveInventory（P227 全表达式临时链 + P259 去 stream 具名槽；218=218）、
//     _processCoolTimeItem（删冗余 (int) cast 得 `cmp mem,%eax`；82=82）。
//   P259 批量（Acquire→CStreamGuard 内联实参）13 处，改善：UpdateData 1055→911、
//     UpdateEffectItemInfo/UpdateCoolTimeItemInfo 68→16×2、ReCalcChattingEmoticon
//     82→50、make_basic_info 3630→3618、DisConnSig 787→779、UpdateLogout 269→267；
//     _SaveCharacData 为反例（ORIG 有 `mov %eax,-0x20` 具名槽）已回退。
//   DIFF→NEAR：reqHumanCertify（删 len 具名局部，P0；ndiff 78→10，残余 put_binary
//     len/ptr 寄存器角色 = N7/N18a）。
//   其他：UpdateCoolTime/EffectItemInfo count 清零前置 + begin() 提前（P212/P50 复活），
//     残余仅 `seta vs setbe`（跨版本 setcc 方向，N 封顶）；SetGameMasterMode characNo
//     具名槽→`(void)` 表达式（帧 0x44→0x34 与 ORIG 齐，残余 N82 槽复用）；
//     delete_ontime_reward_list 顺序 if 结构实测反劣 5→27 已回退。
//   类型（Main 批准；mangled 不含非模板返回类型）：isHackUser unsigned char→bool
//     （CUser.h:3219/CUser.cpp:4113；ORIG 0x081be54c 无 setcc）；setTagCharacInfo
//     int→bool（CUser.h:3755，定义 CUserOnlyOrig.cpp:7270 由 Lane6 代落；ORIG
//     0x084c360b `xor $1` 形）。消费 TU（CTradeSpace/DisPatcherCore/Dispatchers/
//     InterDispatcher/CUserOnlyOrig）复编全部 rc=0。
//   判据① 脚本 /tmp/j2/jcc2.py；逐件 diff 读数 /tmp/j2/（batch4/5、cooltime、si3 等）。
// ============================================================================

// ---------------------------------------------------------------------------
// b28d-J3CUser 第五轮（2026-09-11 01:00–01:45，H3 之后）：§6.5 日志盲区 + P139 +
//   类型轴 + codegen 旋钮。TU 四数 808/1695/4/91（missing 588）→ **808/1697/4/89**
//   （diff −2、ae +2、missing 零增，零劣化）。
//   A. P139（手写签名字符串 → `__PRETTY_FUNCTION__`）×8：AntibotSend×2 /
//      InsertOnTimeEventReward / prepareDisconnect / send_ontime_reward_start_notify×2 /
//      CheckQuestMonster×2 ⇒ 本 TU 的 PF 符号 21→26；96 件逐件核
//      `_ZZ<fn>E19__PRETTY_FUNCTION__` 存在性：**ORIG 有我方缺 = 0**（全对齐）。
//   B. §6.5 日志文本契约（AE 盲区，以 ORIG `.rodata` 原字节为据）×3：
//      WorkPerFiveMin / WorkPerTenMin 格式串（补 `CUser::` 前缀、删发明 `..`）；
//      SetTradePunishType printf 前导空格（ORIG `" CUser::SetTradePunishType(%d) \n"`）。
//      ours-only 串扫描（逐字节 vs ORIG 全库串集）已清零；余 1 项为
//      `DB_LogBuyItem::makeRequest` 的 P110 typedef 名差异（ORIG `memberIdentificationNumber_t`），
//      落地需 include InterSelectPcroomDailyReward.h（TU 级 include 风险）⇒ **不落地，记 N**。
//   C. 类型轴（`.h` 改动均已 R2 广播）：
//      · CheckQuestMonster bool→**void**（P174 串 `void CUser::CheckQuestMonster(...)`）；
//      · LogoutCachedCharacter void→unsigned char + `return 1`（ORIG 0x0864cd85 `mov $0x1,%eax`）
//        ⇒ **DIFF→AE** 156/156；
//      · ChangeCharacSlot int→void（ORIG 全函数无 `mov $0,%eax`，P159）⇒ **DIFF→AE** 172/172；
//      · CheckLoadRentalInfoFromExchange unsigned char→bool（J2 请求；ORIG 0x084c45cd
//        `call; test %al,%al; je` 无 setne，P292 T3）。
//   D. 语义真修（BUG-FIX）：
//      · ResetCurCharac：补 2 处 ORIG 实写清零 `m_gmUserFlag`(+0x711d8) / `m_progLogout`(+0x711e4)
//        （ORIG 0x08658793 / 0x086587c8 `movb $0x0`），并修 3 处书写序（0x8d004→0x8d002、
//        m_field4c→m_field48）⇒ 原缺 2 条清零 = 真语义缺口；代价 238→242（ORIG 240，+2）。
//      · giveLotteryCountItem：**字段错位真修** —— 旧形写 it+0/+4，ORIG 0x08691672/
//        0x08691696 写 it+**0x02/0x07**（Inven_Item::m_addInfo/m_addInfo2）⇒ 事件币/邮件
//        分支取值全错；同修 424→413。
//      · AntibotSend：vtable 槽**多一层解引用**（`*(VerifyFn*)` → `(VerifyFn)`；ORIG
//        0x086480e2 / 0x0864826a 各一次 load）⇒ 运行期跳飞的真 bug；341→340 = ORIG。
//   E. codegen 旋钮（ORIG 直证 + 探针）：
//      · ReCalcChattingEmoticon 136→**102**（ORIG 101）：等宽 align-4 局部视图替 pack(1)
//        类型块拷贝 ⇒ 复现 ORIG 纯 `rep movsl` 8 条（H3 的 aligned-typedef 别名因仍以
//        align-1 为基而失败；本形为 P311/P246 视图族正解）。
//      · UpdateLogout 504→499（删 `(uchar)(f()?1:0)` 三目岛）、GetCurExpertJobLevel 95→93
//        （嵌套 if + `return level-1`）、FatigueUp 626→621（拆 `&&` 消 setcc 岛）、
//        get_skill_command_buf(const) 137→133（end() 外提 + 显式 break）、STGuildDBInfoOnly
//        91→90（复用 memset 返回值寻址）。
//   F. 负结论（实测已回退并留痕）：
//      · SendPacket 的 `switch` 化实测 **66→24** 条（真实路由产跳表；lane1 探针读数不可
//        迁移到本 TU）⇒ 恢复 if 链，维持 P45/P46 封顶；
//      · ReturnToSelectCharacList 的 `register unsigned char` 320 形反劣 312→313 ⇒ 改内联
//        实参（→311）；
//      · CDataMgr 的 N83（4.4 不内联常量 memset + placement-new 必发 `_ZnwjPv`）、
//        UpdateData / make_basic_info / set_grow_type 的 P332/N126 帧常数族：本轮复测无新旋钮。
//   证据与逐件读数：/tmp/cuser_lane1..5.md、/tmp/cuser_p167.txt、/tmp/j3_final_diff.txt。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// b28d-K4CUser 第六轮（2026-09-11 01:35–02:05，5 子代理并行 + 父泳道 fresh 复编核账）：
//   TU 四数 808/1699/4/87 → **808/1707/5/78**（ae +8、diff −9、near 4→5 系一件 DIFF→NEAR
//   的改善方向、identical 持平、missing 588 零增、**零劣化**）。
//   A. 落地（逐件 ORIG 直证；父泳道应用补丁后逐轮 check_tu_routed fresh 核账）：
//      · SetGameMasterMode（P322①）：`(void)getCurCharacNo()` → `pkt.m_field0a =`（ORIG
//        0x086796dc 槽址 = pkt 槽基 -0x16 + 0x0a）⇒ 真语义修复（GM 注册包缺 characNo），DIFF→AE；
//      · SendNotiPacket（P261/P324）：if 链→`switch((int)cmd)`、case 0x25 先于 case 2
//        （ORIG 0x0867ba89 寄存器比较链 + 叶块物理序）。**P305 修正**：`ENUM_NOTIPACKET`
//        是窄枚举（全仓仅 `_0 = 0`）⇒ 裸 `switch(cmd)` 被 -fstrict-enums **静默删 case**
//        （实测本函数 212→50 条 = 真损坏），必须 `switch((int)cmd)` + int 常量 case；
//      · gainCurCharacFatigue（P222/P214①）：`if (remain >= 0)` 分支极性（ORIG 0x0864f871
//        `js` 且正块 fall-through）⇒ DIFF→NEAR；
//      · getCharacInfoDetail（P171 照抄）：补 ORIG 重复赋值 `m_field114 ← m_field1225`
//        （0x0864c58b/0x0864c5b2 两段逐条同形 + Ghidra 连续两条 param_3+0x114）；
//      · FatigueUp（语义轴）：hack 判定乘数 -0x2a(42)→-0x32(50)（ORIG 0x08655dd0-0x08655e02
//        尾 `add %eax,%eax` ⇒ 判定式 = `fat % 50 == 0`；旧形 fat=50k 漏报、fat=42 误报）；
//      · AddCharac（P133 嵌套 if/else 使失败块后置）、AddItem（P183/P329 去具名 invenW/
//        expiration/usable）、ChangeGrowType_GM（P0 具名 checkLevel）、CacheCharacterMemory
//        （P0/P183 链式 GetCharacExpandDataR）、GetDimensionRentInfo / GetPcRoomRentInfo
//        （P183 + P168a `!` 取反早跳）、checkRentedItem（P219 `end() != find_if(...)`）、
//        UpdateCoolTimeItemInfo / UpdateEffectItemInfo（P223 裸早返双跳 `if (size()>0x32)
//        return;`）、send_equip（then/else 体互换为语义等价的 `!=` 链，块序对齐）、
//        reqHumanCertify4ClearMap（P0 删具名 point + max 实参序）、resetDailyData（P0 去 coin）、
//        SendAradEventItemLog（P329 成员写入序 m_id/m_characNo 先于 itemId/itemCount）；
//      · _postCheckForceChangeGrowType（P13）：缓存 `qmap.end()`（ORIG 0x08668853 循环外
//        求值一次）⇒ fn_diff 差异行 111→49；
//      · master_new_skills（**P-L11d**，Main 2026-09-11 转发判据）：secondGrow/firstGrow
//        两互斥分支各自声明的同名标量/指针（chars/got/i/skillId/level/group/slotNo）
//        提升为**函数级共用** ⇒ 促 4.4 复用同一批槽：fn_diff `ours` 495→**502**（ORIG 506，
//        gap +11→**+4**，严格改善）、TU 四数零劣化、p167 87=87 无新缺口。非平凡的
//        `std::vector<pair>` / `std::pair` 未提升（P331 禁提前声明非 POD）。
//      · CUser.h 三处返回类型→bool（P174 串定；K7CUserOnlyOrig 请求、本泳道代落 + R2 广播）：
//        `SelectCharac(int,int)` / `SaveInventoryOnly()` / `setTagCharacInven(TAG_CHARAC_INVEN_DATA*)`。
//   B. p167 结论：92 件全扫（固定 .o 副本）**无真游戏域 callee 缺口**；全部差集为
//      C1↔C2 / D1↔D2 / CStreamGuard `de↔pt` 同址别名（P93 伪差）。注：本波早期批次曾现
//      `OUR calls=0` 大清单，系 .o 被编译流程删除重建期的假读数（已作废并向各子代理广播更正）。
//      唯一非别名残余：`getCharacInfoDetail` ORIG=72 / OUR=71 —— 少 1 次
//      `_ZNKSt6vectorI12_Charac_infoSaIS0_EEixEj`（ORIG 该符号 64 次 / 我方 63 次，1 条级，
//      属 P157 逐字段重读族的单点差异，未定位到具体字段；其余 71 次两侧全等）。
//   C. .h 报告项（未落，交属主）：
//      ① `CStackableItem::StatChangeInfo::m_flagLo`(+0x2e0 首字节) → bool（ORIG 0x0865ec1d
//         `movzbl/xor $0x1/test %al,%al/je`，P158/P248 档②）；
//      ② `TAG_CHARAC_DATA` +0x21 目标字段 char→uchar（ORIG 0x086905c1 单段 `movzbl` 直存）；
//      ③ `SIG_LOAD_ETC` +0xc9ae/+0xc9b3/+0xc9b7 字段与 getter 同型化（ORIG 0x08677af2 同型直存）；
//      ④ `CUserCharacInfo.h:308` `CCargo::MakeItemList(PacketGuard*)` int→bool（ORIG 0x0865dc10
//         → 0x0865dc1f `xor $0x1,%eax`）+ 定义侧 CCargo.cpp 需同刻同步（跨 TU，未落）。
//      ⑤ `CUser::GetPuUser()`（CUser.h:2858）unsigned char→bool **判 N 不改**（K8GlobalData 请求，
//         Main 本泳道复核）：ORIG 本体 0x08692b74 `movzbl 0x796d8(%eax),%eax; pop; ret`（无归一化，
//         生产端像 uchar）vs 消费点 0x08179e56 `call; xor $0x1,%eax; test %al,%al; je`（消费端像 bool）
//         ⇒ P321 T3 「两端矛盾 ⇒ 记 N」；且探针（/tmp/pu_probe.cpp，c6root 4.4.7）证明 bool 化后
//         4.4 在 `return <uchar 字段>` 处插 `testb %al,%al; setne %al` ⇒ 本体 +2 条、本 TU
//         identical→DIFF；字段 `m_field796d8` 亦不可 bool 化（InterDispatcher.cpp:18163 包读可 >1、
//         CUserOnlyOrig.cpp:7163 store 形依赖 uchar=bool 异型、W8_CU_G.cpp:278/282 按 !=0 消费）。
//   逐件 TSV：/tmp/CuA..CuE/patch.diff + result.tsv、/tmp/cuser_scan.tsv、/tmp/p167_summary.txt。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// b28d-L9InterDispatcherCUser 第七轮（2026-09-11 02:05–03:10，6 子代理并行 + 父泳道串行复编核账）
//   TU 四数 808/1707/5/78（missing 588）→ **808/1712/5/73**（ae +5、diff −5、near 持平、零劣化）。
//   A. 落地（逐件 ORIG 直证；每件 fresh 复编核账）：
//      · send_itemspace：外部依赖（L5 落 `CCargo::MakeItemList` int→bool，调用点 ORIG 0x0865dc1f
//        `xor $0x1,%eax`）⇒ **DIFF→AE**；
//      · SendOpenflag（F#2）：`put_byte((int)(char)GetOpencoin())` 复现 ORIG 0x08656a3c
//        `call GetOpencoin; movsbl %al,%eax`（P9/P11）⇒ **DIFF→AE**；
//      · gainCurCharacFatigue（F#4）：`prefatigue` 声明序前置（P-L11d/P212；ORIG 0x0864f7fe
//        `movl $0x0,-0x1c(%ebp)` 槽向）⇒ **DIFF→AE**（原 NEAR）；
//      · GetCurCharacOption（F#6）：裸偏移→具名成员 `getCurCharacR()->m_characterOption`
//        （P83/P183；ORIG 0x0868f068 `mov %eax,%edx; add $0x12b7,%edx`）⇒ **DIFF→AE**；
//      · SendNotiPacket（E#4）：case 2 内 `int p = param;` 补回 ORIG 0x0867bc90 具名槽（P0）⇒ **DIFF→AE**；
//      · WorkPerFiveMin（F#3a/#3b）：两段 packet 的 m_expType store 提到 m_notify 读之前
//        （P211/P324 语句序）⇒ DIFF→**NEAR**；
//      · SendPacket（E#1）：if/else 链→`switch((int)target)` 翻正（P258/P305 窄枚举；
//        ORIG 0x0867b902 决策树）⇒ 差异点 53→**6**（68/68 与 ORIG 齐）—— **旧负结论「switch 恒产跳表
//        66→24」作废**（该读数与 4.4.6 真实路由不符）；
//      · reqHumanCertify4ClearMap（E#2）：**语义修复**（符号性轴⑤）—— `0x7ffffffe < key` 按 unsigned
//        比较（ORIG 0x0868044e `cmp $0x7ffffffe,%eax` + 0x08680453 `jbe`；signed 形仅 x==0x7fffffff
//        清零 ⇒ 高位/负位模式行为相反）；差异点 102→39；
//      · get_skill_command_buf（E#3）：`int idx = 0;` 移到 it/itEnd 声明之后（P212 物化点；
//        ORIG 0x086545e8 在 begin/end 之后）⇒ 差异点 123→110；
//      · SendLoginToPCRoomServer（D#1）：删具名 `int len` 内联 `pkt.GetLength()`（P0/P183；
//        ORIG 0x086816a1 call 后无槽）⇒ 121→119；
//      · UpdateData（D#2）：删 4 个具名 int，直传 `&sig->m_remain{Sp,Sfp}Tree{1,2}`（P0/P183/P329；
//        ORIG 0x08653621/2f/39/43 四条 `add $0x344/0x340/0x1a4/0x8`）⇒ 1274→1268；
//      · ReturnToSelectCharacList（D#3）：去具名 receiver proxy，链式内联（P183；
//        ORIG 0x08687079→0x08687097 无槽写）⇒ 310→308。
//      · ResetCurCharac / send_equip / deleteDailyStackableItem（L#1–#3）：删具名 characNo 两处内联
//        （ORIG 0x0865851c `mov %eax,%ebx` 无槽）/ `if (slot != 0x16) return;` → 包裹形（ORIG
//        0x0865def2 单条 `jmp <+0x1e1>`+nop）/ 末位 `if (m_char3c5 <= 0) continue;` → 包裹形
//        （ORIG 0x0867e874 `jle <+0x141>` 直跳循环尾）⇒ **三件 DIFF→AE**（TU 808/1715/5/70）。
//   B. 回滚（负结论已写入函数体注释）：
//      · removeRentalItemInfoFromInvenBuf（F#5）极性改造：差异点 54→100、140→141 ⇒ 回滚（残差主体是
//        -0x28 槽块序，非极性）；
//      · DisConnSig（F#1a/#1b）`(int)(short)GetSlotIDX()`：零变化 494→494 ⇒ 保留但已标注负结论。
//      · 组 J 五块（add_guild_pvp_result 视图 / RecoverFatigue 取址解引用→值成员 / UpdateCurCharac
//        expansion[1]→视图 / FatigueUp `*(int*)((char*)this+0x8e064)+=v`→视图）**整体落地后 TU 劣化**
//        （ae 1712→1711、missing 588→587、四件 verdict 未翻正）⇒ **五块全部回滚**（反向补丁
//        `/tmp/L9/J/PATCHES_rev.md`，回滚后 808/1712/5/73 复原）。疑点：函数内 pack(1) 视图类型
//        可能改变符号发射 ⇒ 下一轮须拆块单测（优先 J3/J4），并改为 TU 级视图或既有具名成员形。
//        【已定位并修复·同一轮】子代理拆块单测 + nm 直证：**只有 J1 劣化** —— v1 把实例类型换成
//        局部 struct ⇒ 不再发射 `_ZN36Packet_Notice_Guild_War_Point_ChangeC1/C2Ev` 两个 weak ctor
//        （ORIG 存在 C1）⇒ 符号集偏离（与历史 NoticeHolder 教训同族）。修正版 `/tmp/L9/J/PATCHES2.md`
//        改为**保留真实类型实例 + pack(1) 视图 cast 成员写**，五块（J1'+J2+J2b+J3+J4）全落地后
//        **808/1712/5/73 missing 588 零劣化、符号集与基线 0 差异**，且逐件 raw 下降：
//        add_guild_pvp_result 125→123、RecoverFatigue 336→334、UpdateCurCharac 92→91、FatigueUp 621→619。
//   C. 报告工具撞名：`_Z41__static_initialization_and_destruction_0ii` 本 TU 读数 47 vs 16 为工具按名
//      错配（ORIG 该名 493 实例，首个 0x0807ee76 属 PacketTypes）；我方 16 条纯 ioinit 形正确 ⇒ 跳过。
//   D. 本波 CUser.h 与 .h 类改动：零（唯一涉及的 `CUser.h:1598` CheckBuyableProduct int→bool 由 L6 落）。
//   E. §6.5 串盲区真修（AE 不可见，按 ORIG .rodata 原字节直证）：
//      · `SendLoginToPCRoomServer` 的 printf 格式串多尾 `\n`：ORIG 0x0868167d `mov [esp],0x8ceff8c`
//        ⇒ VA 0x8ceff8c = 文件偏移 0xca7f8c（本串起始），.rodata 原字节以 `m_id:%d\x00` 结束
//        （**无尾 \n**）⇒ 去 `\n`。同波同 TU 的 `Inter_CutOffCallUserInfo::dispatch_sig` 同型修复在
//        InterDispatcher 侧（见该文件头部第七轮 E 节）。
//      · 方法学：新增 `/tmp/L9/str_refs.py <our.o> <ORIG> <mangled>` 逐函数比对「引用串集合」；
//        rodata ours-only 扫描（本 TU 现已清零）只能抓「ORIG 无此串」。
//   证据与逐件读数：/tmp/L9/{D,E,F,J,K,L}/RESULT.md、/tmp/{CUser,InterDispatcher}_sizes.tsv。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// b28d-N8CUser 第八轮（2026-09-11 04:00–05:00，单代理独占本 TU）：AE 转正 ×2 + §6.5 串盲区
//   真修 ×1 + P139 符号补齐 ×2 + 四轴审计封账。TU 四数 **808/1715/5/70 → 808/1717/5/68**
//   （ae +2、diff −2、identical/near 持平、missing 588 零增，**零劣化**）。
//   口径：compare_tu_game_orig.py（四数）+ 自建 reloc 归一逐件 diff（/tmp/n8cu/fn_diff2.py，
//   与 /tmp/n1_v.py 同口径）。**禁用** `game_func_report --no-compile` 的 verdict —— 其 objdump
//   不带 -r，.o 未重定位 call 显示成本函数+偏移 ⇒ 恒判假 DIFF（本波全员广播）。
//   A. AE 转正（正条件包裹旋钮 P214①；判据 = ORIG 的 fall-through 属于哪个分支）：
//      · `SendPacket`：case 2/3 由 `if (p == 0) Send(packet); else p->f();` 交换为
//        `if (p != 0) p->f(); else Send(packet);` —— ORIG 0x0867b9xx 决策树内
//        `cmpl $0x0,-0x10(%ebp); je <this 分支>` ⇒ fall-through 是 p!=0 分支；旧形 4.4 产
//        `jne` + fall-through = this 分支（条件方向与块序双反、diff 16 行）⇒ 现 0 行、68=68 AE。
//      · `send_ontime_reward_start_notify`：**半包裹**（只把第一个 if 改成
//        `if (ev != 0) { IsEventing 检查 } else { trace; return 0; }`；全包裹形实测 26 行
//        反劣于原 25 行，已弃）—— ORIG 0x0868c194 `cmp DWORD PTR [ebp-0x24],0x0; je 0x0868c1c5`
//        （else 块 = trace 紧跟 IsEventing 的 return-0 岛）⇒ 现 0 行、151=151 AE。
//   B. §6.5 串盲区真修（AE 不可见，运行期日志文本）：`DB_LogBuyItem::makeRequest` 的
//      `__PRETTY_FUNCTION__` 第一参类型 —— ORIG prett 实字节 @0x08b639e0/133B =
//      `static void DB_LogBuyItem::makeRequest(memberIdentificationNumber_t, unsigned int,
//      unsigned int, unsigned int, ENUM_DBLOG_ITEM_TYPE)`，我方旧形 `unsigned int,...`
//      ⇒ 本 TU 顶部补 `typedef unsigned int memberIdentificationNumber_t;`
//      （与 InterSelectPcroomDailyReward.h:18 同型；重复 typedef 合法）+ 定义侧首参改名。
//      探针实证（c6root 4.4.7，/tmp/n8cu/probe）：声明写 unsigned int、定义写 typedef 名
//      ⇒ prett 取**定义处拼写**。改后 `strings /tmp/tu_CUser.o` 该串与 ORIG 逐字节一致，
//      TU 四数零劣化（ours-only 串扫描清零）。
//   C. P139 符号补齐（零语义风险）：`master_new_skills`（2 处 logFormat 实参）与
//      `_processEffectItem`（cMyTrace 实参）的手写签名字符串 → `__PRETTY_FUNCTION__`
//      ⇒ 本 TU PF 符号 26→28，28/28 内容与 ORIG 逐字节一致（nm 取址读 .rodata 全量比对：
//      content mismatch = 0、发明 PF = 0）。父泳道点名的第 3 处 `AddSecuRewardItem` 为**假缺口**：
//      ORIG 本体 0x08688328 仅 20B（push/mov/sub $0xa4/mov %al,-0xa4/leave/ret，**零 prett 引用**），
//      ORIG 侧该 PF 符号属他 TU 副本 ⇒ 不落地。
//   D. 四轴审计（全 TU 复扫；结论 = 已清零 / 无新真缺口）：
//      · p167（96 件）：MISSING/EXTRA 全为 C1/C2、D1/D2 同址别名（P93 伪差）+ N130（C++11 右值
//        `pair<int,int>::pair<unsigned long,int>(pair<...>&&)`）+ N89 伴生模板件 ⇒ 无真游戏域 callee 缺口。
//      · 串轴：our-only 串 1 项（即 B 项，已修）；strall.py 升级版复扫仅余 base64 码表/typeinfo
//        碎片（结构性假阳性）。编码轴：本 TU 非 ASCII 字面量 **0 处**。
//      · 大位移轴（disp ≥ 0x20000 = CUser 成员域）：唯一候选 `NextCharacMoneyLog` 我方 0x79678
//        vs ORIG 0x79670 经逐条直证为**常量分解差**（我方 0x79678+lea 0x4 ≡ ORIG 0x79670+lea 0xc
//        = member+0x7967c）⇒ 非字段错位；CheckQuestMonster 0x11d/0x15a 等全为 `lea [ebp-…]` 栈帧位移。
//   E. 负结论（本轮实测/直证后封 N，未落地）：
//      · `setInitQuestPoint` 仅差 1 条 `jmp`（ORIG 0x868aeff `jmp 0x868af02` 跳过 else 位 nop）：
//        4.4 前端折叠「跳转到紧邻下一条」⇒ if 包裹/continue/goto 三形均不可复现 ⇒ N。
//      · `CacheCharacterMemory`：ORIG `setne+test+je <函数尾 return-0 共享岛>` vs 我方 4.4
//        `sete+…+本地岛`（setcc 方向 P168a/P248 + 共享尾岛 P325 双轴同现）⇒ N。
//      · `GetSkill` 的 add+mov vs lea 折叠、`_deleteSpecificItemAccountCargo` 的 `setne+test` 物化、
//        `STGuildDBInfoOnly::C1` 的 %edx/%eax 角色、`WorkPerFiveMin` 的 %edi/%esi 角色、
//        `gainExpAsUpperMember` 的 x87 槽 ⇒ 均 N7/N18a/N82/N115 封顶（逐件 diff /tmp/n8cu/diffs*.txt）。
//   证据与逐件读数：/tmp/n8cu/{base_compare,scan_strs_out,disp_all,p167_all,diffs1,diffs2,diffs3}.txt、
//     /tmp/n8cu/rank.py（逐件 reloc 归一行数排序）、/tmp/n8cu/fn_diff2.py。
// ---------------------------------------------------------------------------

#include <string.h>
#include <cstdarg>
#include "TimeLog_.h"      // CUser::LogHistory（ORIG 0x0867c8d8 时间头）
#include "cHistoryTrace.h" // CUser::LogHistory 日志输出
#include "CSwitchLog.h"   // CUser::SetPvpIndex（ORIG 0x08669ee8 pvp@log）
#include "HistoryLog.h"  // CUser::master_new_skill WriteLearnSkill（ORIG 0x0866b4d7）
#include "CUser.h"
#include "W6_HackPunishDeps.h"   // onUpdateHackCount：CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver / WongWork::HackTypeInfo_t

#include "CUser.h"
#include "MoneyLogPacket.h"   // CUser::SendMoenyLog（ORIG 0x0868f472 载荷 0x1e）
#include "Packet_Notice_Guild_War_Point_Change.h"   // 权威类（原局部镜像已删）
#include "SigTypes.h"        // SIG_LOAD_ETC/SIG_LOAD_DUNGEON_CLEAR/SIG_REQ_BLACK_COUNT/SIG_ATTENDANCE_EVENT 完整布局
#include "CDungeonClear.h"   // WongWork::CDungeonClear::toString
#include "CCharacList.h"          // CCharacList::m_characs（calc_lev_stat 角色表）
#include "CharacManageScript.h"   // CharacManageScript 完整类型（管理等级链）
#include "GameResultType.h"  // GameResultType（GetRecord 序列化 0x50/元素）
#include "CTitleBook.h"   // item_lock::CItemLock / stItemLockRef 权威定义
#include "CGameManager.h"
#include "CInventory.h"
#include "CPrivateStoreMgr.h"   // b22：private_store 权威头（局部影子类退役，
                                //        deleteSpecificItem 转真 RemovePrivateStore）
#include "DB_LogBuyItem.h"   // DB_LogBuyItem 权威声明（P104 统一：makeRequest 定义本文件）
#include "APSystemTypes.h"
#include "GameBasicsScripts.h"
#include "CParty.h"
#include "SkillSlot.h"
#include "CSkill.h"       // master_new_skills 成长展开 get_group（ORIG 866b9c0）
#include "WarRoom.h"
#include "WarRoom.h"
#include "PvP_Room.h"   // add_pvp_result/update_pvp_point 房间查询（get_user_seat 等）
#include "SyncScriptDeps.h"   // toTString（RewardItem2DeleteInvalidItem 字符串表翻译）
#include "PacketBuf.h"
#include "GameTypes.h"
#include "CCapchaDataManager.h"  // WongWork::CCapchaDataManager（GlobalData::s_capchaDataManager）
#include "CDungeon.h"          // CDungeon::m_89e（exceptedStateMoneyLog 搬运副本标记）
#include "DBInsertGiveBoxEventReward.h"  // GiveMeBoxReward 转发（ORIG 0x08693758）
#include "Packet_Monitor_Increase_Guild_Exp.h"  // WorkPerFiveMin 公会上报包（0x40e,0x1c）
#include "CMonitorServerProxy.h"  // SendNoticeCharLive（WorkPerTenMin）
#include "CGuildServerProxy.h"    // SendPacket（WorkPerFiveMin）
#include "InterfacePacketBuf.h"
#include "cMyTraceNoop.h"  // SendCmdErrorPacket PAY_TYPE 命中 trace（ORIG 0x867c0ad/0x867c0c8）
#include "CExpertJob.h"              // charac_expand::CDataMgr slot0 CCharacExpertJob（StubS3A ctor）
#include "CExpertJobList.h"          // STExpertJobEtcScript（SetCharacLevel 专家职业经验）
#include "nexon_packet.h"           // stLoginPacket（SendLogin/LogoutToPCRoomServer）
#include "PCRoomServerProxy.h"     // CPCRoomServerProxy::SendTcpPacket（PC 房登录/登出通知）
#include "W9_EventSmall.h"           // slot1 CCharacBloodDungeon
#include "CPvPLiveEventData.h"       // slot3 CPvPLiveEventData
#include "Redeem_Item__CRedeemItem.h"// slot6 Redeem_Item::CRedeemItem
#include "OnlinePreliminary.h"       // slot4/5 online_preliminary 成员
#include "CLeagueMatch.h"            // CLeagueMatch::GetPlayCountPerOneTeam（make_basic_info 校队块）
#include "CFairPvPScore.h"           // slot7 fair_pvp::CFairPvPScore
#include "CMissionList_Charac.h"     // slot8 CMissionList_Charac
#include "CUserCharacInfo.h"         // CExpandEquipslot/CTitleBook 完整类型
#include "CExpandEquipslot.h"        // CExpandEquipslot 权威虚表（getData @ vtable+8；GetDataFn 桥真实化）
#include "DataManagerInitDepends.h"  // TOD_UserState
#include "CQuestShop.h"              // CQuestShop
#include "CKillMonsterInfo.h"        // CKillMonsterInfo
#include "CAchievement.h"            // CAchievement
#include "CItemDictionary.h"         // CItemDictionary
#include "CBoosterGage.h"            // CBoosterGage
#include "CBurningFatigueEvent.h"     // FatigueUp 燃烧疲劳 getRefillFatigue（ORIG 0x810a166）
#include "CCharacterDayEvent.h"       // FatigueUp 角色日事件 GetEventJob（ORIG 0x8697886）
#include "AvatarRechargeServer.h"     // FatigueUp CalcDurability（ORIG 0x819049a）
#include "BestClearTime.h"           // BestClearTime
#include "CCharacInformNotice.h"     // CCharacInformNotice
#include "CItemMakingSkill.h"        // CItemMakingSkill（若存在）
#include "GameWorld.h"
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "RDARScriptStringManager.h"   // g_scriptStringManager_.findString
#include "CMyFileLog.h"   // InsertOnTimeEventReward OnTimeEvent 日志
#include "CItemAmplifier.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "Arad_DataManager.h"    // ARAD::Singleton<Arad_DataManager>（ReturnToSelectCharacList）
#include "Arad_EventPeriodDataManager.h"  // ARAD::Arad_EventPeriodDataManager::delUserRewardData
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn (ENUM_INTERNALPACKET_JPN)
#include "ARAD_Database.h"           // ARAD::DATABASE::sourceCargoSize（仓库快照长度）
#include "TaiwanCash.h"             // WongWork::IPG::CIPGHelper 虚接口（DoGetCeraPointItem）
#include "CExpertJob.h"          // expert_job::CExpertJobMgr::OnLeaveUser
#include "CHandlePremium.h"      // WongWork::CHandlePremium（handleReturn2CharacView）
#include "W8LT_DBTask.h"        // DB_UpdateSchoolPoint（RecoverFatigue 学校点落库）
#include "DB_InterDeps.h"       // DB_UpdateCharacPlayTime（ReturnToSelectCharacList）
#include "COnTimeEvent.h"       // COnTimeEvent::delete_reward_user（delete_ontime_reward_list）
#include "MoreTypes.h"         // arad::SigAradEventItemLog（SendAradEventItemLog）
#include "CPowerManager.h"    // s_power_manager 完整类型（make_basic_info 排名查询）
#include "AuctionProxy.h"    // CAuctionServerProxy/CCeraAuctionServerProxy 完整类型（IsRunning）
#include "CDeathTowerRanking.h"
#include "Packet_Register_GM_MID.h"
#include "Packet_Monitor_Max_Level_BroadCast.h"  // SetCharacLevel 46 级监控广播包（ORIG W ctor 0x08694560）
#include "Packet_Randombox_statistic.h"  // CUser::SendRandomBoxStatistic（ORIG 0x08676826）
#include "Packet_Event_UpgradeJar_Output_Statistic_GTS.h"  // SendEventJarOutputItemStatistics
#include "CConditionEventManager.h"
#include "CCoinEventPerDay.h"     // CCoinEventPerDay（GetCoinNoPerDay 权威实现）

#include "CAssaultMgr.h"
#include "CMissionList_Charac.h" // CMissionList_Charac 任务列表查询（charac_expand 槽位 8）
#include "RefPvpGrade.h"        // RefPvpGrade::GetPvpCurrRankPoint/GetPvpNextRankPoint
#include "QuestClear.h"         // WongWork::CQuestClear::getClearedQuest（UserQuest +0x04）
#include "CUserGlobalInfoHandle.h"  // CUserGlobalInfoHandleInstance/reset_uniqueid_flag
#include "CBoosterGage.h"       // charac_expand::CData（CDataMgr 20 槽 ResetDaily 循环）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（isBoundaryExpValue/GetLevel）
#include "Redeem_Item__CRedeemItem.h"  // Redeem_Item::CRedeemItem（isAddableFilter/AddRedeemList）
#include <algorithm>
#include <boost/foreach.hpp>   // giveLotteryCountItem：ORIG 用 BOOST_FOREACH 遍历 rewards
#include <iostream>
#include "CQuestShop.h"          // set_grow_type 清 QP
#include "Quest.h"               // Quest/MonsterKillItem（CheckQuestMonster）
#include "CUserProc.h"           // APSystem::_ActionGroupIndex / CUserProc
#include "HeroMissionEvent.h"
#include "ServiceRestrictManager.h"     // ARAD::Singleton<HeroMissionEvent>
#include "EpollHandler.h"        // G_EpollHandler
#include "MsgQueueMgr.h"         // s_msgq_mgr->put
#include "CStreamGuard.h"                  // CStreamGuard（SendAradEventItemLog）
#include "StreamPool.h"                    // GlobalData::s_stream_pool->Acquire
#include "Arad_StatisticsEventItemList.h"  // Arad_DataManager Singleton/needStatistics
// SIG_* 布局经 CDoubleConnCheckServerProxy.h → SigTypes.h 可见。
// ORIG T 0x08655327（自由函数）
#include "CUser/SetUserManagerToolIP.inc"
#include "CDoubleConnCheckServerProxy.h"
// DisConnSig / prepareDisconnect 统计与退出落库依赖
#include "CSystemTime.h"                    // GlobalData::s_systemTime_::getCurSec
#include "Packet_User_Ting_TimeCheck_Statistic_Add.h"  // DisConnSig Ting 时段统计包
#include "Packet_Client_Spec_Statistic.h"   // DisConnSig HWSpec 统计包
#include "DBUpdateExitTime.h"               // prepareDisconnect 退出时间落库
#include "TaiwanQueries.h"                  // prepareDisconnect GarenaAuthData::getTimeStamp
#include "CEnvironment.h"                   // prepareDisconnect getProcessSequence
#include "Dispatchers.h"                    // prepareDisconnect Dispatcher_SecurityCardAuthCancel
class DB_ForceSaveSimpleSSO { public: static void makeRequest(unsigned int, int); };  // ORIG T 0x084281ae（定义 W7LinkDB.cpp）

// stBuySkillInfo 完整定义（与 CEventCharacterHandler.cpp 一致；CUser.h 仅前向声明）
struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};


// g_ServerString_（ORIG 0x93fa660 B C 名；全局定义在 ServerCommon/ServerXml.cpp）
namespace np_server_xml
{
class CServerXml
{
public:
    std::string GetServerString(int nIndex, bool* pFlag) const;
};
}

extern np_server_xml::CServerXml g_ServerString_;


// ORIG W 0x082a70ab（自由函数）
class EpollHandler;
EpollHandler* G_EpollHandler();


// b22 类建模（Model22）：原「private_store 域最小前向声明」的规避理由
// （SkillSlot.h↔SigTypes.h 包含序冲突）经双序探针实测**已不存在**（两序
// 均 COMPILE OK，冲突注记过时）——改为 include 权威头 CPrivateStoreMgr.h
// （本文件头区），局部影子类删除（§4.4 唯一声明点）。

// CBattle_Field（本地最小视图；get_dungeon_index 定义于 CBattle_Field.cpp，
// ORIG 0x0822d0d8，符号 _ZN13CBattle_Field17get_dungeon_indexEv。完整类头
// CBattle_Field.h 与 WarField.h 的 map_item/map_monster 重复类型冲突，属
// FixMapMonsterReal/FixWarFieldTypes 域，此处仅声明所需方法）
#ifndef DNF_GAME_CBATTLE_FIELD_H_
class CBattle_Field
{
public:
    int get_dungeon_index();
    char m_pad[0x128];            // +0x00..+0x127
    short m_villageMonsterId;     // +0x128（exceptedStateMoneyLog 读取，完整类 CBattle_Field.h:421）
};
#endif  // DNF_GAME_CBATTLE_FIELD_H_（batch-16：CParty.h 折叠后 BF 完整头可达，占位类需让位）


// ---- 外部符号声明（对应 TU 翻译后移除） ----
extern unsigned char _NS_PI_2ND_GetDefaultRandomHashKey();

extern char* GetPacketName(ENUM_PACKETCLASS packetClass, unsigned short packetId);   // ORIG T 0x082a2112

extern bool IsLightServer();   // ORIG 0x0822ad44 W（bool 实证：giveup_panalty 0x086786cb test %al 无 setne）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08668823 | _Z29_postCheckForceChangeGrowTypeP5CUserii
extern void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow);  // 0x08668823 T
extern void _postCheckForceChangeGrowType4AheadQuest(CUser* user, Quest* quest);  // 0x086686ed T（定义 StubS3A_deps.cpp）


namespace ARAD
{
namespace DISPATCHER
{
#include "CUser/make_cmd_packetheader_jpn.inc"
}
}

namespace item_lock
{
}
// CItemLock 权威声明在 CTitleBook.h（item_lock::CItemLock）

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0832a778 | _ZN13charac_expand8CDataMgrC1Ev
// 2026-09-11 CuA（P5 父泳道点名「EXTRA 3×placement new/memset/delete」复核）：
//   p167fix 的 EXTRA `_ZnwjPv`/`memset`/`_ZdlPvS_` ×3 与 MISSING `_ZdlPv` 均**非多余结构** ——
//   ORIG 0x0832a781..0x0832a7b8 把 `operator new(size,ptr)` **内联**并把手写常量 `memset`
//   内联成 `rep stos`；4.4 两者都走 out-of-line call。我方源码已是最小单表达式
//   （下行三处 `new (memset(::operator new(N),0,N)) T()`，无额外指针具名/无显式析构）
//   ⇒ 属 N78b 跨版本内联差，无源码可改。
#include "CUser/CDataMgr.inc"
{
    // slot0 +0x00：ORIG 0x0832a781..0x0832a7b8 `movl $0x30,(%esp); call operator new;
    //   mov %eax,%esi`（**无具名指针槽**，mem 保活 %esi）→ `rep stos`（0xb dword +
    //   尾 word，4.1 对常量 memset 的内联形，N78b 不可翻案）→ `mov %ebx,(%esp);
    //   call CCharacExpertJob::CCharacExpertJob()` ⇒ 指针不再落具名槽：以
    //   `new (memset(::operator new(N), 0, N)) T()` 单表达式表达（P0/P183）。
    m_pad[0] = new (memset(::operator new(0x30), 0, 0x30)) expert_job::CCharacExpertJob();

    // slot1 +0x04：ORIG 0x0832a7c6 `mov %eax,-0x20(%ebp)`（此件 ORIG 有槽）→ 同上
    m_pad[1] = new (memset(::operator new(0x2c), 0, 0x2c)) CCharacBloodDungeon();

    // slot2..slot19：ORIG 纯 new + 默认 ctor（无 memset）
    m_pad[2] = new item_lock::CItemLock();            // new(0x58)
    m_pad[3] = new CPvPLiveEventData();                  // new(0xc)
    m_pad[4] =
        new online_preliminary::COnlinePreliminaryMember();                                          // new(0x14)
    m_pad[5] =
        new online_preliminary::COnlinePreliminaryMemberMatchList();                                 // new(0x20)
    m_pad[6] = new Redeem_Item::CRedeemItem();    // new(0x10)
    m_pad[7] = new fair_pvp::CFairPvPScore();      // new(0x5c)

    // slot8 +0x20：operator new(0x158) → memset 全块 → CMissionList_Charac
    m_pad[8] = new (memset(::operator new(0x158), 0, 0x158)) CMissionList_Charac();

    m_pad[9] = new CExpandEquipslot();                    // new(0x89c)
    m_pad[10] = new CItemMakingSkill();                    // new(0x20)
    m_pad[11] = new CQuestShop();                                // new(0x48)
    m_pad[12] = new CKillMonsterInfo();                    // new(0x50)
    m_pad[13] = new TOD_UserState();                          // new(0x20)
    m_pad[14] = new CTitleBook();                                // new(0x6b58)
    m_pad[15] = new CAchievement();                            // new(0x28)
    m_pad[16] = new CItemDictionary();  // new(0x1390)；CItemDictionary 现真继承 charac_expand::CData
    m_pad[17] = new CBoosterGage();                            // new(0xc)
    m_pad[18] = new BestClearTime();                          // new(0x24)
    m_pad[19] = new CCharacInformNotice();              // new(0x18)
}
// ---- G1-3 临时外部桩（对应 TU 翻译后移除） ----
// ORIG T 0x0808ca12：mov $0x7,%eax 直返 7
unsigned char _NS_PI_2ND_GetDefaultRandomHashKey() { return 7; }
// ORIG W 0x08695058：movl $0,0x10(%eax) —— 清 m_field10(+0x10)。
#include "CUser/resetExposedCount.inc"
// 0x0864809c：antibot 校验/加密发送主链路（依据 AntibotSend.md 反编译；
// s_antibotChecker_ vtable +0x34 校验 / +0x28 加密，ORIG 栈缓冲此处以 vector 等价）。
extern void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user); // ORIG T 0x082751ee
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864809c | _ZN5CUser11AntibotSendER9PacketBuf
// [b28d-GrpD 判词·结构化 N] ORIG 0x0864809c：残差为 ① checker/全局访问的
//   `mov 0x0,%R; mov (%R),%R`（未解析重定位 + 指针解引用形）② VLA 段槽号整体位移
//   （-0x30.. vs -0x34..）③ 未解析 call 标注。语义关键点逐条对齐：VLA 步进
//   `-(len+0x212)&~0xf`、虚槽 +0x34(verify)/+0x28(encrypt) 直调、encReq 单次短路物化、
//   encrypt 非 0=失败→trace(0x807)、`(!encReq&&isEncryptRequired())||encFailed` 链、
//   send_packet<0 → trace(0x834)+packetId!=199 DisConnSig ⇒ 无语义缺口（第三方 checker
//   槽建模已在下 §4.5 例外块终判）。
#include "CUser/AntibotSend.inc"
                                              // "bool CUser::AntibotSend(PacketBuf&)"（P174）
{
    // ORIG T 0x0864809c 全文转写（报告 AntibotSend.md Ghidra C+asm 双段读完）：
    // ①verify（vtable+0x34）先行，成功后才进加密缓冲段；②加密缓冲=**VLA +
    // guard.clear + memset 500+len**（0x864822b/245：orig memset@plt=1、无
    // vector——alloca 步进 `-(len+0x212)&0xfffffff0`）；③encrypt 返回 **非0=失败**
    // → trace(0x807)+encFailed，**返回0=成功** → put_binary/Send 后 return
    // （0x86483b0 早退，非我旧版 ==0 续跑）；④encrypt 调用实参求值序 get_len→
    // get_packet→getAntibotKey（右→左）；⑤trace1 的 key/len 在 C1 **之前**求值；
    // ⑥isEncryptRequired 缓存（encReq 真则二段不再调，0x864842b else 路）。
    unsigned short packetId =
        *(unsigned short*)(packet.get_packet(0) + 1);
    bool encFailed = false;   // ORIG 0x086480c5 movb $0,-0x2e 预清（在 encReq 赋值前）
    // ORIG 0x086480cf..0x08648117：`isEncryptRequired() && verify(...)!=0` **单一
    //   短路物化**到 -0x2d（`mov $1/$0 + mov %al,-0x2d`），verify 仅在需求为真时调用。
    typedef int (*VerifyFn)(void*, int, unsigned short);   // vtable +0x34
    bool encReq = packet.isEncryptRequired() &&
        ((VerifyFn)((*(void***)GlobalData::s_antibotChecker_)[0x34 / 4]))(
            GlobalData::s_antibotChecker_, getAntibotKey(), packetId) != 0;
    if (encReq)
    {
        // §4.5 例外：s_antibotChecker_（GlobalData.h `void*`）承自外部
        // 【b22 终判·第三方】CreateAntiServerObj(ORIG 0x082a6539 W) 反汇编实证
        // = strlen/strncat/memcpy 拼路径 → dlopen → dlsym → call *%eax：checker
        // 对象来自运行时加载的外部 .so（反外挂 SDK），其 C++ 类/构造/vtable 均
        // 不在 df_game_r 编译单元内 ⇒ 本仓建模该类无从取证（未知槽的签名不可
        // 声明、§4.1 禁造），typedef 槽指针直调即 ORIG 行为的忠实表达——建模
        // 收益评估否决，此案终结。全族七槽用法同源：VerifyFn+0x34/EncryptFn+
        // 0x28/Fn3+0x08/Fn1+0x24/ReportFn+0x38/AddClient+0x04/P2PAddClient+
        // 0x20（本文件 :7746 与 PacketDispatcher/InterDispatcher 各点同判）。
        const int bufLen = packet.get_len() + 500;   // local_6c
        char buf[bufLen];                            // ORIG VLA（alloca 步进）
        int outLen = bufLen;                         // local_24
        PacketGuard guard;                           // local_68（块作用域，
                                                       // dtor 于早退前 0x86483b5）
        ((InterfacePacketBuf*)&guard)->clear();
        memset(buf, 0, bufLen);
        ((InterfacePacketBuf*)&guard)->put_header(0, 0x231);
        typedef int (*EncryptFn)(void*, int, char*, int, char*, int*);  // +0x28
        // 实参右→左求值：get_len→get_packet→getAntibotKey（P24/実参序）
        int r = ((EncryptFn)((*(void***)GlobalData::s_antibotChecker_)[0x28 / 4]))(
            GlobalData::s_antibotChecker_, getAntibotKey(),
            packet.get_packet(0), packet.get_len(), buf, &outLen);
        if (r != 0)
        {
            // ORIG 0x08648323..0x0864838a：全表达式临时链，get_len/getAntibotKey
            //   在 cMyTrace C1 **之前**求值并跨构造保活（P87/P227；具名局部会槽化）。
            cMyTrace(__PRETTY_FUNCTION__, 0x807, 0)(
                "[RecvBufferForEncrypt] result : %d, PacketID : %d, "
                "Key : %d, Length : %d", r, (int)packetId,
                getAntibotKey(), packet.get_len());
            encFailed = true;
        }
        else
        {
            ((InterfacePacketBuf*)&guard)->put_binary(buf, bufLen);
            ((InterfacePacketBuf*)&guard)->finalize(false);
            return Send(guard);   // 0x8648313 起 return = Send 返回值透传
        }
    }
    PacketBuf sendBuf;
    char* data = packet.get_packet(0);
    int len = packet.get_len();
    // ORIG 0x08648379..0x08648441：`(!encReq && isEncryptRequired()) || encFailed`
    //   短路链（`movzbl encReq; xor $1; test; je 查 encFailed` + `mov $1/$0`）。
    bool doEnc = (!encReq && packet.isEncryptRequired()) || encFailed;
    if (doEnc)
    {
        sendBuf.copy(packet);
        sendBuf.encrypt_packet();
        data = sendBuf.get_packet(0);
        len = sendBuf.get_len();
        sendBuf.encFinalize();
    }
    _NS_PI_2ND_SendHash_SC(data, (unsigned short)len, this);
    int sret = m_network.send_packet(data, len);
    if (sret < 0)
    {
        // ORIG 0x086484ec..0x08648539：getCurCharacName 在 C1 之前求值（%ebx 保活）
        cMyTrace(__PRETTY_FUNCTION__, 0x834, 0)(
            "User %s - send_packet() fail length(ID:%d,LEN:%d),",
            getCurCharacName(), (int)packetId, len);
        if (packetId != 199)
            DisConnSig((DISCONN_SIG)6, false, 0);
        return 0;
    }
    return 1;
}

#include "CUser/_Quest_Authen_Data.inc"
// ORIG reset 0x0814aa16 逐字段清：movw +0/+2、movl +8、movb +0xc、movl +0x10/+0x14
// （不动 +0x04/+0x18 起字段）；旧 memset 0x48 语义与形态均不符。
#include "CUser/reset.inc"
{
    m_characNo = 0;
    m_characClass = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
}
// [b28d-Main·检查点18] 删除 stub `mission_list_is_clear`：ORIG 无该符号、全仓无调用点。

#include "CUser/reset_1.inc"
{
    m_field0 = 0;
    m_field1 = 0;
    // ORIG 0x86454e8-0x86454f5 单装载双 store = DImode 清零形；ll 成员直写复现（w2 probe 实证）。
    m_hackTradeGold = 0;
    m_hackTradeCnt = 0;
}

// ORIG C1 0x086940c6：仅 movb $0,(%eax)（清 m_pad[0] 首字节）。
#include "CUser/STGuildAgitDBInfo.inc"
{
    m_pad[0] = 0;
}
#include "CUser/resetInformNoticeFlag.inc"
{
    // ORIG memset(this+0x8ebb1, 0, 1)：只清当前通知 flag 首字节（m_informNoticeFlags[0]），
    // 不动其后的 15B（已保存的通知 flag 板）。
    memset(m_informNoticeFlags, 0, 1);
}

#include "CUser/disableSaveInformNoticeFlag.inc"
{
    m_saveInformNoticeFlag = 0;   // ORIG 0x868fa6c 写 +0x8ebc1
}

#include "CUser/enableSaveInformNoticeFlag.inc"
{
    m_saveInformNoticeFlag = 1;   // ORIG enableSave 写 +0x8ebc1
}

namespace WongWork
{
// WongWork::CDeathTowerRanking 权威声明见 CDeathTowerRanking.h（唯一声明点，
// 定义在 CDeathTower.cpp；本地同名类已删除，§7）。
}

namespace APSystem
{
// _SIG_LOAD_ACTION_POINT 权威定义见 CDataManager.h（经其传递包含），
// 本 TU 不再本地镜像；DB_UpdateActionPoint::makeRequest 仅指针使用。
class DB_UpdateActionPoint
{
public:
    // 与 ORIG/DB_AccountCargoSync.h 一致：int + const _SIG_LOAD_ACTION_POINT& + bool，
    // 符号 _ZN8APSystem20DB_UpdateActionPoint11makeRequestEiRKNS_22_SIG_LOAD_ACTION_POINTEb。
    static void makeRequest(int uid, const _SIG_LOAD_ACTION_POINT& data,
                            bool save);
};
}   // APSystem（完整声明见 CUserProc.h）

// ORIG T 0x0812887e _ZN13DB_LogBuyItem11makeRequestEjjjj20ENUM_DBLOG_ITEM_TYPE：
// characNo==0 早退；StreamPool Acquire("localchina/localchina_DBThread.cpp",0x59b)
// → CStreamGuard(stream,true) → 每条 << 前 call operator*（**guard）
// → <<0x251/<<-1/<<accId/<<characNo/<<itemIdx/<<type/<<value
// → NumberToString(accId,0) 独立求值 → cMyTrace(__PRETTY_FUNCTION__,0x5af,0)
// → trace(fmt,accStr,characNo,itemIdx,type,value) → s_msgq_mgr->put(LOGDB_Q,guard)。
// [P110] ORIG prett 实字节（0x8b639e0，133B）第一参类型名为 `memberIdentificationNumber_t`
//   ⇒ 定义侧参数必须写该 typedef 名（GCC 取**定义处拼写**展开 prett；探针实证：
//   声明写 unsigned int、定义写 typedef 名 ⇒ 展开取 typedef 名）。typedef 与
//   InterSelectPcroomDailyReward.h:18 同型（重复 typedef 合法），本 TU 顶部补齐。
typedef unsigned int memberIdentificationNumber_t;
#include "CUser/makeRequest.inc"
                                unsigned int itemIdx, unsigned int type,
                                ENUM_DBLOG_ITEM_TYPE value)
{
    if (characNo == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                            "localchina/localchina_DBThread.cpp", 0x59b),
                        true);
    **guard << 0x251;
    **guard << -1;
    **guard << accId;
    **guard << characNo;
    **guard << itemIdx;
    **guard << type;
    **guard << (int)value;
    cMyTrace(__PRETTY_FUNCTION__, 0x5af, 0)(
        "DB_LogBuyItem::makeRequest m_id=%s nCharacNo=%d nItemID=%u nCount=%u nType=%d",
        NumberToString(accId, 0), characNo, itemIdx, type, value);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

class DB_SecurityCardUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt);
};

class DB_PassPadUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt,
                            char* webAddress);
};

namespace WongWork
{
// ORIG W 0x086959a8：movl $0x0,0x48(%eax) —— 清 +0x48。
#include "CUser/resetCancelCnt.inc"
// ORIG W 0x0822ef4c：mov 0x48(%eax),%eax —— 直返 +0x48。
#include "CUser/getCancelCnt.inc"
}


// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b4300 | _ZN20UserMercenaryInfoMgr9AddCharacER21CHARAC_LOAD_MERCENARY
#include "CUser/AddCharac.inc"
{
    // ORIG T 0x081b4300：未命中（CheckValidAppend==1）才添加；先置
    // m_avatarBonus=0xa，再按 8 个 avatar 槽物品取装备 grade 的最小值；
    // 非头像装备/非 CEquipItem 直接清 0。最后 push_back 返回 1。
    // b28d-solo-CUser：CheckValidAppend 已由 Main 代落 bool（CUser.h:793）⇒ P168a/P292
    //   裸形 `!fn()` 复现 ORIG 0x081b4300 `call; xor $0x1,%eax; test %al,%al; jne`。
    if (!CheckValidAppend(info))
        return 0;
    info.m_avatarBonus = 0xa;
    for (int i = 0; i < 8; ++i)
    {
        // 2026-09-10 gA（A 组）：ORIG 0x081b4356 find_item 返回值无槽往返（结果直接留
        //   %eax：`test %eax,%eax; jne 081b4366(call __dynamic_cast); mov $0x0,%eax;
        //   jmp 081b4386`）⇒ 删具名 item 与三目，改裸 dynamic_cast（GCC 自身发射该
        //   null 短路形）；我方旧形多 -0x10 槽并致 i/equip 槽号下移 4（93 vs 85）。
        CEquipItem* equip = dynamic_cast<CEquipItem*>(
            G_CDataManager()->find_item(info.m_avatarItems[i]));
        // ORIG 0x081b4389 `cmpl $0x0,-0xc(%ebp); je 081b43e1` 与 0x081b43a3
        //   `test %al,%al; je 081b43d8` 是两个独立失败块（各含 movb $0+jmp 081b43fd）
        //   ⇒ 拆两个独立 if（P133；合并 `||` 会多出 xor$1/test bool 物化）。
        // 2026-09-11 CuA A组：ORIG 两处失败块落在循环体**之后**（43d8 内层、43e1 外层，
        //   跳转 `je` 向后）⇒ 原码是 `if (equip != 0) { if (IsAvatarItem()) {…} else {失败} }
        //   else {失败}`（嵌套 if/else，内层 else 先于外层 else 发射）；我方旧 `if (equip==0)
        //   {失败}` 形令失败块前置（`jne` 跳过），布局整体镜像。
        if (equip != 0)
        {
            if (equip->IsAvatarItem())
            {
                // [b23S3C] P48/P0：ORIG get_grade 双臂各重取一次（+0xb5/+0xc9，无 grade 槽）
                // 2026-09-10 gA：ORIG 0x081b43a8..0x081b43bc 先把 m_avatarBonus 读入 %ebx
                //   （`movzbl 0x32(%eax),%eax; movsbl %al,%ebx`），再 call get_grade，
                //   `cmp %eax,%ebx; setg` ⇒ 左操作数先物化（P66）⇒ 比较式反转，勿写
                //   `get_grade() < m_avatarBonus`（那是 setl + 提前 call 的反向形）。
                if (static_cast<int>(static_cast<signed char>(info.m_avatarBonus)) > equip->get_grade())
                    info.m_avatarBonus = static_cast<unsigned char>(equip->get_grade());
            }
            else
            {
                info.m_avatarBonus = 0;
                break;
            }
        }
        else
        {
            info.m_avatarBonus = 0;
            break;
        }
    }
    m_characs.push_back(info);
    return 1;
}
// ORIG T 0x081b441a：按 characNo 线性查找并 erase（ORIG 经 FindIndexByCharacNo，
// 内联等价）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b441a | _ZN20UserMercenaryInfoMgr12RemoveCharacEj
#include "CUser/RemoveCharac.inc"
{
    // b28d-solo-CUser p167 修复：ORIG 0x081b441a 实形 =
    //   idx = FindIndexByCharacNo(characNo)；found=(idx!=-1 && idx<size())；
    //   if(found){ try{ erase(begin()+idx); }catch(...){} }
    //   （我方原线性循环缺 FindIndexByCharacNo + __cxa_begin/end_catch）。
    // b28d-solo-CUser 重写（2026-09-09，P226 条件向）：ORIG 无 found 具名局部——条件
    //   全程寄存器物化（`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`），旧
    //   `bool found` 被 4.4 落槽（movb $1,-0x9 / cmpb $0,-0x9）；且 ORIG 第二比较为
    //   **有符号** `jle`（size-idx）⇒ idx/size 两侧取 int。
    int idx = (int)FindIndexByCharacNo(characNo);
    if (({ idx != -1 && idx < (int)m_characs.size(); }))
    {
        try
        {
            m_characs.erase(m_characs.begin() + idx);
        }
        catch (...)
        {
        }
    }
}
using expert_job::CCharacExpertJob;

// ORIG 0x0832ac48 T：遍历 0x14 个 CData* 槽，非空则经 vtable+0x14 槽
//（CData 虚表 D0/deleting 析构）删除对象。
#include "CUser/dtor_CDataMgr.inc"
{
    for (int i = 0; i < 0x14; ++i)
    {
        // ORIG：无判空，直接经虚表 +0x14（D0）删除（null 时 ORIG 崩溃语义照抄）。
        delete m_pad[i];
    }
}
#include "CUser/CharacAdvanceAltarManager.inc"
#include "CUser/dtor_CharacAdvanceAltarManager.inc"
// ORIG C1 0x0808bf68：写 vptr（编译器自动）+ movb $0x27,0x4 —— +0x04 key 种子。
#include "CUser/PIReceiverManager.inc"
#include "CUser/dtor_PIReceiverManager.inc"
#include "CUser/PISenderManager.inc"
#include "CUser/dtor_PISenderManager.inc"
// ORIG T 0x0833c28a：休息(3)↔战斗(1) 两态切换；无宠物早退。
#include "CUser/TurnStomach.inc"
{
    if (m_equippedCreature != 0)
    {
        if (fighting)
        {
            if (m_equippedCreature->GetState() == 3)
                m_equippedCreature->SetState(m_user, (CREATURE_STATE)1);
        }
        else
        {
            if (m_equippedCreature->GetState() == 1)
                m_equippedCreature->SetState(m_user, (CREATURE_STATE)3);
        }
    }
}
// ORIG T 0x0832ac94：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 GetDataR 同形（ORIG 两函数字节一致）；enum 底层无符号 → jbe。
#include "CUser/GetData.inc"
{
    if (!((unsigned int)type <= 0x13))
        return 0;
    return (CExpandEquipslot*)m_pad[(int)type];
}
// ORIG T 0x0832acb2：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 update/ResetDaily 一致的 CData*[0x14] 槽解释；enum 底层无符号 → jbe。
#include "CUser/GetDataR.inc"
{
    if (!((unsigned int)type <= 0x13))
        return 0;
    return (CExpandEquipslot*)m_pad[(int)type];
}
// ORIG T 0x0832ad78：对 20 个 CData* 槽（+0x00..+0x4c）依次调用非虚 CData::reset。
#include "CUser/reset_2.inc"
{
    for (int i = 0; i <= 0x13; ++i)
    {
        m_pad[i]->reset();
    }
}

// ORIG 0x0832ade8：对 20 个成员（+0x00..+0x4c，各为 charac_expand::CData*）
// 依次调用虚表槽 0（ResetDailyMidnight）。
// ORIG 0x0832adaa：同上，调用虚表槽 3（ResetDaily）。
#include "CUser/ResetDailyMidnight.inc"
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDailyMidnight();
    }
}

#include "CUser/ResetDaily.inc"
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDaily();
    }
}

// ORIG T 0x0832acd0：对 20 个 CData* 槽（m_pad）isSave→saveData 全链；任一失败返回 0。
#include "CUser/update.inc"
{
    // [b28c-11] AE（32/32）：`isSave() != 0` + bool 字段/返回（CBoosterGage.h
    // m_bIsSave/isSave bool 化）复现 ORIG `call isSave; je` 直判（char 形 4.4
    // 恒物化 setne %al; test %al，P73）。
    char result = 1;
    for (int i = 0; i < 0x14; ++i)
    {
        if (m_pad[i]->isSave() != 0)
        {
            if (m_pad[i]->saveData(pUser) != 1)
                result = 0;
        }
    }
    return result;
}

#include "CUser/update_1.inc"
{
    // [b28c-11] AE（25/25）：同上（bool isSave 直判，无 setne 物化）。
    char result = 1;
    if (m_pad[(int)type]->isSave() != 0)
    {
        if (m_pad[(int)type]->saveData(pUser) != 1)
            result = 0;
    }
    return result;
}
// ORIG C1 0x08697068：清 byte+0、int+4/+8/+0xc、short+0x10/+0x12。
#include "CUser/GrowthWeaponEvent.inc"
{
    m_eventFlag = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field12 = 0;
}
// ORIG C1 0x08695ad0：基类 C1（同 this）后置 +0x38/+0x39/+0x3a=1。
#include "CUser/Secu_HackLogCheck.inc"
{
    m_flag223 = 1;
    m_flag224 = 1;
    m_flag225 = 1;
}
// ORIG C1 0x08695ab0：清 +4/+5/+6（m_active223/224/225）。
// 定义挪至 Secu_HackLogCheck C1 之后（技巧 31 规则2：callee 同 TU 前置定义
// 会收紧 caller 帧；挪后 caller 0x8695ad3 ORIG sub $0x18 宽松帧恢复）。
#include "CUser/Secu_MonsterCheck.inc"
{
    m_active223 = 0;
    m_active224 = 0;
    m_active225 = 0;
}
// ORIG C1 0x08694f68：构造 3 个 user_gold_info 子对象（+0x04/+0x40/+0x7c）。
#include "CUser/Secu_GoldControl.inc"
{
    // ORIG C1 0x08694f68：m_total/m_cur/m_backup 三个 user_gold_info 成员
    // 默认构造（编译器自动插入，各调 user_gold_info::C1，与 ORIG 同形）。
}
// ORIG C1 0x0848683c：IData C1 + PlayInfo C1（基类）+ Character::Reset()（0x08486818
// = PlayInfo::Reset() + movl $0,0x3c）。ORIG 不初始化 Character 自身字段。
#include "CUser/Character.inc"
{
    Reset();
}
// ORIG W 0x080cbc6c：movl $0,0x0 + movb $0,0x4/0x5/0x6 —— 清 eventId/m_pad4/isWin/needSave。
// ORIG C1 0x08697128：movl $0,+0 / movb $0,+4/+5/+6（m_eventId/m_pad4/m_isWin/m_needSave）。
#include "CUser/BingoData.inc"
{
    m_eventId = 0;
    m_pad4 = 0;
    m_isWin = false;
    m_needSave = false;
}
#include "CUser/clear.inc"
{
    m_eventId = 0;
    m_pad4 = 0;
    m_isWin = false;
    m_needSave = false;
}
#include "CUser/GrowthCreatureEvent.inc"
{
    // ORIG 0x086970a4（W）：vector<eventCharacInfo> 默认构造后调用 clear()。
    m_vec.clear();
}
#include "CUser/CharacLevelUpGiftAccountOnce.inc"
{
    // ORIG 0x086970e6（W）：multimap<int,int> 默认构造后调用 clear()。
    m_map.clear();
}
// ORIG C1 0x084866d4：仅 call PlayInfo::Reset()。
#include "CUser/PlayInfo.inc"
{
    Reset();
}
// ORIG W 0x08486818：PlayInfo::Reset() 后清 +0x3c。
#include "CUser/Reset.inc"
{
    PlayInfo::Reset();
    m_f3c = 0;
}
// 定义挪至 Character::Reset 之后（技巧 31 规则2：恢复 caller 0x848681b
// ORIG sub $0x18 宽松帧；PlayInfo 自身 ctor 0x84866d4 亦在定义之前，不受影响）。
#include "CUser/Reset_1.inc"
{
    m_f00 = 0;
    m_f04 = 0;
    m_f08 = 0;
    m_f0c = 0;
    m_f10 = 0;
    m_f14 = 0;
    m_f18 = 0;
    m_f1c = 0;
    m_f20 = 0;
    m_f24 = 0;
    m_f28 = 0;
    m_f2c = 0;
    m_f30 = 0;
    m_f34 = 0;
    m_f38 = 0;
}

// ORIG W 0x084867ee：PlayInfo::operator+=(other) 后 m_f3c 自增。
void Character::operator+=(const Character& other)
{
    PlayInfo::operator+=(other);
    // ORIG 0x8486806-0x8486812：t=m_f3c; m_f3c=t+1 四段形；尾 0x8486815 leave;ret
    // 无 return-reload → 返回类型 void（Main 已落 CUser.h:1874，ORIG 0x8484dcd 调用点不消费）。
    m_f3c = m_f3c + 1;
}

// ============================================================================
// CUser 实现
// ============================================================================

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864e410 | _ZN5CUserC1Ev
#include "CUser/CUser.inc"
    : m_field704a8(0), m_field79670(0), m_field796d8(0), m_chState((ch_state)0)
{
    m_field8ec24 = 0;
    m_field8ec2c = 0;
    m_premium.m_mailBox = 0;
    m_field796d9 = 0;
    m_field8cfd0 = 300;
    m_field8cfd4 = 0;
    m_field8cfd8 = 0;
    setAntibotKey(-1);
    setClientVersion(0);
    m_mcap.m_serialHitCount = 0;
    m_mcap.m_monsterDieHackCnt = 0;
    m_postChecksumData.m_field54 = 0;
    m_field796f8 = 0;
    reset();
    m_historyLog.SetUser(this);
    m_field705d8 = 0;
    m_field8e094 = 0;
    ResetUsedCoinInDungeon();
    m_piSender.m_field10 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_field8eb18 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_accountCargo.Clear();
    m_field8eb1c = 0;
    ClearItemSellCount();
    m_field8eb44 = 0;
    m_field8eb48 = 0;
    m_field8eb99 = 0;
    resetInformNoticeFlag();
    disableSaveInformNoticeFlag();
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_rentalInfo.clear();
    m_field8cfcc = (UserSpace::T)0;
    // 2026-09-10（FL2CUser 接收者/字段轴，ORIG 直证）：ORIG ctor 第 390 条为
    //   `movb $0x0,0x7962c(%eax)`（块内 **+0x08** = m_tradePunishOverThirtyDays），
    //   全函数**从不**写 0x79654（块内 +0x30 = m_firstLogin）⇒ 旧形「firstLogin=0」
    //   是发明初始化（多写一条），且真正被 ORIG 清的是 tradePunishOverThirtyDays。
    m_postChecksumData.m_tradePunishOverThirtyDays = 0;   // 块内 +0x08（0x7962c）
    m_advanceAltarIdx = 0xffff;
    m_field8eba8 = 0;
    m_field8ebac = 0;
    m_field8ebb0 = 0;
    m_field8ec29 = 0;
    m_rentalFlag3 = 0;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864ef9c | _ZN5CUserD1Ev
// [CUserWorker 2026-09-11 复核·残差精确定位（双侧 objdump 逐条）] 差异全在
//   0x0864f4e9..0x0864f536 段的 **EH cleanup 链 lowering**：
//   · ORIG：LP1(0x0864f4e9 `mov %edx,%ebx; mov %eax,%esi`) 后 **fall-through 直入 LP2**
//     (0x0864f501 同类序)，正常路径 0x0864f51b 把相邻两成员析构
//     （`CDungeonGainedGold::~` @+0x8cf64 → `vector<unsigned int>::~` @+0x8cef4）**连续置于
//     同块**，全段只需 1 条 `jmp`；
//   · 4.4：同一对析构被拆成两个 cleanup region（每块 `call + jmp`）⇒ 我方 +3 条（679 vs 676）。
//   ⇒ 纯跨版本 EH lowering（4.1 fall-through 串接 vs 4.4 分段 + 显式 jmp），与本函数手写体
//     （仅 `m_powerUp = 0`，两侧同形）无关；成员声明序/类型已核（两侧同偏移同序）⇒ 无源码旋钮。判 N。
#include "CUser/dtor_CUser.inc"
{
    // 2026-09-10 gA（A 组）：**实现体不在本 .inc**（本文件 37 字节，仅函数头 +
    //   m_powerUp=0 开头），主体在聚合器 CUser.cpp ⇒ 本组写域外，未动。
    //   判定依据：ORIG 0x0864ef9c（676 vs 678，差 2 条为帧/槽分配残差）。
    m_powerUp = 0;
}

#include "CUser/reset_3.inc"
{
    ResetCurCharac();
    m_gmBossDifficulty = 0;
    m_gmBossMapIndex = 0;
    m_field8cf00 = 0;
    m_field8ead8 = 0;
    m_premium.InitPremium();
    m_characList.clear();
    m_characList.reserve(8);
    m_guildAgitDB.m_pad[2] = 0;
    m_characterView.reset();
    m_hackAnalyzer.resetHackInfo();
    for (int i = 0x65; i < 0xfa1; ++i)
        getHackAnalyzer()->resetServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)i);
    m_accHacking.resetInfo();
    if (m_premium.m_mailBox != 0)
    {
        WongWork::CMailBoxHelper::FreeMailBox((WongWork::CMailBox*)m_premium.m_mailBox);
        m_premium.m_mailBox = 0;
    }
    m_list711ec.clear();
    InitFinishPoint();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    m_guildAgitDB.m_pad[1] = 0;
    m_codeHackCheck.reset();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
    setUserEventCharacterFlag(0);
    m_vec8cef4.clear();
    m_field8d0f0 = 0;
    m_field8d0f1 = 0;
    m_field8d0f2 = 0;
    m_field8e074 = 0;
        m_character.PlayInfo::Reset();
    m_field8d0f8 = 0;
    m_field8d0f4 = 0;
    m_379.reset();
    setAntibotKey(-1);
    setClientVersion(0);
    m_dungeonGainedGold.reset();
    m_postChecksumData.m_field44 = 0;
    m_rating = 0;
    m_field8cfc0 = 0xffffffff;
    m_postChecksumData.m_totalFatigueUsed = 0;
    m_levelupEvents.clear();
    m_characExpand.reset();
    m_premium.m_field98c = 0;
    m_limitItemUsage.clear();
    m_quest.m_field761c = 0;
    m_accId = 0;
    // ORIG reset 0x864a5c0+164..195 语句序：59 → accountCargo.Clear → 5c(0x7965c)
    // → 5e → 60 → dup(0x7966c) → str0 → str1 → 58(0x7965c int) → 56。
    m_postChecksumData.m_field59 = 0;
    m_accountCargo.Clear();
    m_postChecksumData.m_field60 = 0;
    m_postChecksumData.m_field62 = 0;
    m_postChecksumData.m_field64 = 0;
    m_dupMsgSlotIdx = 0;
    m_str79664[0].clear();
    m_str79664[1].clear();
    m_postChecksumData.m_field5c = 0;
    m_postChecksumData.m_field5a = 0;
    ResetUsedCoinInDungeon();
    DBUpdateDBLogItem();
    ResetDBLogItem();
    m_actionPoint.Reset();
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
    reset_server_fatigue_data();
    reset_ingame_event_history();
    reset_ingame_event_history_update();
    m_mercenaryMgr.m_characs.clear();
    m_field6ef68 = 0;
    m_field6ef69 = 0xff;
    resetMoneyLog();
    m_field4 = 0;
    m_field5 = 0;
    m_receivedRequestType = 0xff;
    m_field796d8 = 0;
    m_quest.reset();
    m_field8ebd8 = 0;
    m_field8ebd4 = m_field8ebd8;
    m_field8ebd0 = m_field8ebd4;
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_bingo.clear();
    m_levelUpGift.m_map.clear();
    m_growthCreature.m_vec.clear();
    GetGameOptionRef()->reset_load_hotkey_flag();
    m_rentalFlag3 = 0;
    m_field8ec31 = 0;
    m_field8ec32 = 0;
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865851c | _ZN5CUser14ResetCurCharacEv
#include "CUser/ResetCurCharac.inc"
{
    getPVPResultRefW()->Clear();
    getBattleRecordRefW()->clear();
    m_list8cf80.clear();
    getDungeonClearRefW()->clear();
    m_gmBossDifficulty = 0;
    m_gmBossMapIndex = 0;
    if (getCurCharacR() != 0)
    {
        ((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrW()->Reset();
        ((SkillSlot*)getCurCharacSkillR())->clear_all_skills_both();
        ((CCargo*)getCurCharacCargoR())->reset();
        getCurCharacQuestR()->reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->Reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SetUser(this);
        if (isDisguiseCharac() != 0)
            setDisguiseCharac(false, 0, 0);
    }
    reset_guild_temp_exp();
    reset_guild_exp();
    reset_level_before_dungeon();
    for (int i = 1; i < 5; ++i)
    {
        // ORIG（CUser::ResetCurCharac 反汇编实证）：characNo = getCurCharacNo()，
        // 然后 getRankTable(i)->unregistBestRecord(characNo)（成员签名，见 CDeathTowerRanking.h）。
        // [L9-GrpL 2026-09-11 P0/P183 去具名局部] ORIG 0x0865851c 该循环段（报告反汇编
        //   0x086585xx）为 `call getCurCharacNo; mov %eax,%ebx`（**无 characNo 槽**）→
        //   `mov -0xc(%ebp),%eax`（循环变量 i 读自 -0xc）→ `mov %ebx,0x4(%esp)`；
        //   我方具名 `unsigned int characNo` 多占 -0xc 槽、把循环变量 i 顶到 -0x10，
        //   且每处多 store+load 一对（`mov %eax,-0xc(%ebp)` / `mov -0xc(%ebp),%edx`）；
        //   段末 `addl $0x1,-0xc` vs 我方 -0x10 亦由此。⇒ 内联实参（ORIG 无该局部）。
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistBestRecord(getCurCharacNo());
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistRanking(getCurCharacNo());
    }
    resetCurCharacSchoolPoint();
    if (getCurCharacR() != 0)
    {
        // b28d-GrpD P183/P329（ORIG 0x0865851c 直证）：ORIG 该段为
        //   `call G_GameWorld`（%eax 保活）→ `call get_charac_job`（%eax）→
        //   `lea 0x58(%R),%R; mov 0x4(%R,%R,4),%R; sub $0x1,%R; mov %R,0x4(%R,%R,4)`
        //   —— **无 pWorld / job / pCnt 具名槽**；我方旧形 3 个具名量（+ 指针缓存）
        //   多占 -0xc/-0x10/-0x14 并产 shl/add 链（帧 0x38 vs ORIG 0x24）。
        //   +4+(job+0x58)*4 = +0x164+job*4 = GameWorld::m_channelUserCount[job]。
        G_GameWorld()->m_channelUserCount[get_charac_job()]--;
    }
    setCurCharac(0);
    setTagCharac(0);
    m_field8d004 = 0xffff;
    m_tradeSpaceIdx = 0xffff;
    m_pvpRoomIdx = 0xffff;
    m_field8d008 = 0xffff;
    m_field8d00c = 0xffff;
    m_field8cfcc = (UserSpace::T)0;
    m_gmUserFlag = 0;   // ORIG 0x08658793 movb $0x0,0x711d8（+0x711d8 直证）
    m_field6ef68 = 0;
    m_field6ef69 = 0xff;
    m_guildAgitDB.m_pad[2] = 0;
    m_playingDungeonIdx = 0xffffffff;
    m_progLogout = 0;   // ORIG 0x086587c8 movb $0x0,0x711e4（+0x711e4 直证）
    m_postChecksumData.m_field4c = 0;
    m_postChecksumData.m_field48 = 0;
    if (m_premium.m_mailBox != 0)
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    resetGuildCreateFlag();
    resetTempGuildNameAddr();
    reset_pvp_masterid_walkingout_me();
    m_mcap.resetExposedCount();
    m_field8d1dc = 0;
    m_field8d1e0 = 0;
    m_quest.m_field761c = 0;
    m_pcRoomAuth = 0;   // CUser+0x8cef1（原 m_quest.m_field761d 别名）
    memset(&m_reward, 0, 4);
    setForceDropFlag(false);
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_field8ec29 = 0;
}

#include "CUser/reset_server_fatigue_data.inc"
{
    m_serverFatigueDbType = 0;
    m_serverFatigueValue = 0;
    m_map8eb50.clear();
}

// ORIG 0x0818d3fe：在 map<uchar,short>(+0x8eb50) 中按 type 查疲劳值，未命中返回 0。
#include "CUser/get_server_fatigue_count.inc"
{
    // ORIG 0x868d454 call _Rb_tree_iterator::ne + je 尾岛（技巧 30：命中 fall 在前）。
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(type);
    if (it != m_map8eb50.end())
        return it->second;
    return 0;
}

// ORIG 0x0818d4c4：若当前疲劳 DB 类型(+0x8eb4d) == type 返回 (+0x8eb4c) 否则 0。
#include "CUser/get_fatigue_count_db.inc"
{
    // ORIG 0x868d4ca-0x868d4d4：双零扩展 + `cmp 0xc(%ebp),%eax` arg 槽直折、**无 cur 槽**
    // （sub 无、pop ebp 直返）。b28d-solo-CUser AE：内联条件（具名 cur 落 -0x4 致帧 0x10）。
    if ((unsigned char)m_serverFatigueDbType == type)
        return (unsigned char)m_serverFatigueValue;
    return 0;
}

#include "CUser/reset_ingame_event_history.inc"
{
    m_map8eb68.clear();
}

#include "CUser/reset_ingame_event_history_update.inc"
{
    m_map8eb80.clear();
}

#include "CUser/reset_pvp_masterid_walkingout_me.inc"
{
    m_set8cfe4.clear();
}

#include "CUser/set_guildwar_point_per_pvpplay.inc"
{
    m_guildWarPoint = point;
}

#include "CUser/get_guildwar_point_per_pvpplay.inc"
{
    return m_guildWarPoint;
}

#include "CUser/getHades.inc"
{
    return &m_hades;
}

#include "CUser/GetPVPRoom.inc"
{
    // ORIG 0x86551a2 js 尾岛 + 0x86551d1 jmp merge：正向 return（probe12 B 实证复现 js;body;jmp;mov$0;merge）
    if (m_pvpRoomIdx >= 0)
        return G_CGameManager()->GetPvp(m_pvpRoomIdx, this, 0);
    return 0;
}

// ORIG T 0x0868860a：转发 party->SendSecretShopItemList(this)（声明 CUser.h:3432）
#include "CUser/SendSecretShopItemList.inc"
{
    CParty* party = (CParty*)GetParty();
    if (party != 0)
        party->SendSecretShopItemList(this);
}

#include "CUser/GetPICSMap.inc"
{
    return &m_map8eabc;
}

#include "CUser/CheckMoney.inc"
{
    // ORIG 0x866af2f/0x866af3c：accName→%esi、level→%ebx 跨 call 缓存（push esi/ebx
    // 帧 0x20）；limit@-0x10、curMoney@-0xc；尾 `cmp -0x10,%eax; jg F; mov $1; jmp;
    // F: mov $0` 直分支岛。4.4 goto-RET0 形在 label 前恒插 1 字节 nop 填充（探针
    // nl11_probe1/2 实证），if/else return 形无 nop（仅 cmp 操作数序 4.4 恒折反向）。
    register const char* accName = get_acc_name();   // b28d-solo-CUser：裸 register（非绑定，P0 合规）跨 call 保活 %esi
    register int level = get_charac_level();   // b28d-solo-CUser：裸 register 跨 call 保活 %ebx（P231/P248）
    int limit = G_CDataManager()->GetMoneyLimitPerLevel(level, accName);
    int curMoney = getCurCharacMoney();
    if (curMoney + money <= limit)
        return true;
    else
        return false;
}

#include "CUser/getDungeonIdxAfterClear.inc"
{
    CParty* party = (CParty*)GetParty();
    if (!party)
        return -1;
    // CParty+0xb24 = BF 区内嵌视图（CParty.h m_battleField@+0xb24 建模区）。
    return party->m_battleField.get_dungeon_index(
        );
}

#include "CUser/resetMoneyLog.inc"
{
    for (int i = 0; i <= 6; ++i)
        m_moneyLogs[i].reset();
    m_moneyLogIdx = 0;
}

#include "CUser/ResetDBLogItem.inc"
{
    m_dbLogItems.clear();
}

#include "CUser/DBUpdateDBLogItem.inc"
{
    // b28c-L5：ORIG 0x0868950x 增量段调
    //   `_ZN9__gnu_cxx17__normal_iteratorIPKN5CUser14_DBLogItemInfoESt6vectorIS2_SaIS2_EEEppEi`
    //   （**后置** ++，带独立 sret 临时槽 -0x1c：`lea -0x1c; movl $0,0x8(%esp);
    //   lea -0x24,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call ppEi; sub $4,%esp`）；
    //   4.4 对 `++it` 走 ppEv（返回引用）⇒ 少 sret 槽 + 少 `sub $0x4,%esp`（ndiff 10）。
    //   改后置形后该 sret 槽本身即把迭代器阶梯顶到 ORIG 位置 ⇒ 原「技巧 31 dead
    //   slotPad」变为多余（会让 it 深一格到 -0x28、全帧平移），删除。
    for (std::vector<_DBLogItemInfo>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        DB_LogBuyItem::makeRequest(get_acc_id(), getCurCharacNo(), it->m_itemIdx,
                                   it->m_type, it->m_value);
    }
}

#include "CUser/GetParty.inc"
{
    // ORIG 0x8655160 js 尾岛（0x8655182 mov $0）：正向 early-return（probe12 B 形）
    if (m_field8d004 >= 0)
        return G_CGameManager()->GetParty(m_field8d004);
    return 0;
}

// CUser::GetUserPosInParty @ 0x08688638 —— 返回 own 在队伍中的槽位，不在返回 -1。
#include "CUser/GetUserPosInParty.inc"
{
    CParty* party = (CParty*)GetParty();
    // ORIG 0x8688650 je 尾岛（0x8688666 mov $-1）：正向 early-return（同 GetPVPRoom 族 probe12 B 形）
    if (party != 0)
        return party->GetUserPosition(this);
    return -1;
}

#include "CUser/GetWarRoom.inc"
{
    // ORIG 0x86551f2 js 尾岛（0x8655214 mov $0）：正向 early-return（probe12 B 形）
    if (m_field8d008 >= 0)
        return G_CGameManager()->GetWarRoom(m_field8d008);
    return 0;
}

// ORIG T 0x0864df12：遍历 m_characList（this+0x796e8），首字段 m_characNo
// 与 characNo 相等则返回下标，否则返回 -1。
#include "CUser/get_charac_idx.inc"
{
    // ORIG 0x864df12：prologue 连构 begin→-0x10、end→-0x14（尾循环 cond 双 lea
    // 无重构，技巧 25 具名缓存消重）；声明序 end 后占远槽。
    int idx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itend = m_characList.end();
    for (; it != itend; ++it)
    {
        if ((unsigned int)it->m_characNo == characNo)
            return idx;
        ++idx;
    }
    return -1;
}
#include "CUser/get_charac_no.inc"
{
    // ORIG 0x08677a?? prologue cmp $-1 的 fall-through 块 = param==-1 路径
    //（技巧 17：等号形让 -1 块前置，!= 形被 4.4 后置）。
    if (param == -1)
    {
        if (getCurCharacR() == 0)
            return 0;
        return *((unsigned int*)getCurCharacR());
    }
    if (param < 0 || (int)m_characList.size() <= param)
        return 0;
    return *((unsigned int*)&m_characList[param]);
}

#include "CUser/CheckItemLock.inc"
{
    if (getCurCharacR() == 0)
        return 0;
    // ORIG 0x86469??：call getCurCharacInvenR 结果留 %eax（this 最后入栈），实参经 %edx
    // 中转——无 pInven 具名槽（直调链式形）。
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(a, b);
    if (item == 0)
        return 1;
    unsigned char lock =
        item->m_amp.GetLock();   // Inven_Item::m_amp @+0x11
    if (lock == 0)
        return 0;
    // ORIG 0x86469d4：test;je → mov $0 尾岛 / fall mov $1（正向 if；`!()` 反产 xor 物化）。
    if (((item_lock::CItemLock*)GetCharacExpandDataR(
              (ENUM_CHARAC_EXPAND_TYPE)2))
            ->CheckItemLock(lock))
        return 1;
    return 0;
}

#include "CUser/getCurCharacTotalFatigue.inc"
{
    // ORIG 0x865776c：total dead-init `movw $0,-0xa`（技巧 8）+ call 物化 `mov %ax,-0xa`
    // (0x865777d)；rate 无槽：test %ax,%ax;setne;test;je 物化（技巧 22，0x8657791-99）。
    unsigned short total = 0;
    total = getCurCharacFatigue();
    // b28d-solo-CUser AE: 条件内联免 bool 槽（ORIG 0x8657791-99 setne;test;je 无槽，
    // 具名 nz 使 4.4 落 -0x9 并推 total 槽 -0xa→-0xc；probe_bool a3/a5）。
    if (m_premium.GetAdvantageFatigueRate() != 0)
    {
        // ORIG 槽反推（声明序即分配序，技巧 21：先声明占近槽）：maxPremium@-0xe、
        // premiumFatigue@-0xc（min 双 lea 0x86577c8-0x86577d2 实参序 (premium,maxPremium)
        // 对应 std::min(a,b) 签名 (T,T) 取 &a,&b → min 第1参=premium@-0xc? lea -0xe 入
        // 0x4(%esp)（第2参）、lea -0xc 入 (%esp)（第1参）⇒ min(premiumFatigue,maxPremium)
        // 保留；added@-0x10、maxTotal@-0x12（后声明远槽——此处三者 ORIG 强制 -0x12<-0xe<-0xc
        // 交叠，maxTotal 声明最先占最远 16bit 槽位序，见下）。
        // 槽序=声明序（4.4 先声明近槽）：premium@-0xc/maxPremium@-0xe/added@-0x10/
        // maxTotal@-0x12（ORIG 0x86577a6-0x86577c4 装载序与赋值分开写）。
        unsigned short premiumFatigue;
        unsigned short maxPremium;
        unsigned short added;
        unsigned short maxTotal;
        maxTotal = getCurCharacTotalMaxFatigue();
        maxPremium = getCurCharacMaxPremiumFatigue();
        premiumFatigue = getCurCharacPremiumFatigue();
        // ORIG 0x86577da-0x86577e1：`_ZSt3minIt` = min<unsigned short>（t=ushort），
        // movzwl (%eax) 解引用 + `add -0xa,%ax`、截断写 added@-0x10。
        added = std::min(premiumFatigue, maxPremium) + total;
        // ORIG 0x86577f2-0x86577fa：total = min(added, maxTotal)（双 lea 形）。
        total = std::min(added, maxTotal);
    }
    return total;  // ORIG 0x86577fe 单点 movzwl -0xa(%ebp)
}

#include "CUser/getCurCharacTotalMaxFatigue.inc"
{
    // ORIG 0x86??：`movw $0,-0xa` dead init + call 结果 `mov %ax,-0xa` 双写（技巧 8）；
    // rate 无槽（register asm("ax")，call 结果直落 test;setne;test;je 物化）。
    unsigned short total = 0;
    total = getCurCharacMaxFatigue();
    // ORIG 0x8??：`test %ax,%ax; setne %al; test %al,%al; je`（技巧 22：register bool
    // 结果 !=0 物化）。b28d-solo-CUser AE：条件内联免 bool 槽（probe_bool a3/a5）。
    if (m_premium.GetAdvantageFatigueRate() != 0)
        total = (unsigned short)(total + getCurCharacMaxPremiumFatigue());
    return total;
}

// ============================================================================
// CUser::make_basic_info —— ORIG _ZN5CUser15make_basic_infoEPcc @0x0865a44e
// 报告：docs/class_func_reports/CUser/make_basic_info.md（## 汇编 + ## 反编译 C
// 全文核对）。按 type 分派：0=ENUM_USERINFO_MINIMUM、1=ENUM_USERINFO_ADDITION、
// 2=账号角色列表（m_characList 循环）、3=ENUM_USERINFO_OTHER_CHARACTER；
// 其余值直接返回 1。三个需当前角色的分支在 getCurCharacR()==0 时
// LogManager::logFormat 报错并返回 0；各分支 lock → 填包 → unlock。
// ============================================================================

// ---- checkSpecialChar / checkSpecialCharName（ORIG 自由函数，分支 2 使用）----
// ORIG _Z16checkSpecialCharPc T 0x0829efae / _Z20checkSpecialCharNamePc T 0x0829f01f。
// 依赖的 CDataManager 名字过滤四方法均已建模；后续若其它 TU 需要应迁往公共 TU，
// 届时删除此处定义避免重复符号。
#include "CUser/checkSpecialChar.inc"
{
    if (G_CDataManager()->getCharacNameFilterPass())
    {
        G_CDataManager()->setCharacNameFilterPass(false);
        return false;
    }
    return G_CDataManager()->hasSpecialCharacter(name)
           || G_CDataManager()->hasPreventString(name);
}

#include "CUser/checkSpecialCharName.inc"
{
    if (G_CDataManager()->getCharacNameFilterPass())
    {
        G_CDataManager()->setCharacNameFilterPass(false);
        return false;
    }
    return G_CDataManager()->hasPreventStringName(name);
}

namespace
{

#pragma pack(push, 1)

// PvpResultType 只读视图：权威最小定义（CUserCharacInfo.h）中 +0x5c 尚在 m_pad5c
// 内；ORIG 分支 0/3 以 int 读该槽（全量布局见 PvpResultType.h：今日胜场【推断】）。
struct PvpResultBasicView
{
    int m_totalWinCount;            // +0x00
    int m_field04;                  // +0x04
    int m_field08;                  // +0x08
    int m_field0c;                  // +0x0c
    int m_field10;                  // +0x10
    int m_pvpGrade;                 // +0x14（与 get_pvp_grade 同源）
    int m_pad18[(0x5c - 0x18) / 4];
    int m_field5c;                  // +0x5c（分支 0/3 写出槽）
};
typedef char pvp_view_size_ok[(sizeof(PvpResultBasicView) == 0x60) ? 1 : -1];

// getCurCharacR() 当前角色数据扩展字节视图（权威声明中这些偏移均在 pad 区；
// +0x39 起 0x14 字节与 CommonTime 创建时间区对齐【推断】）。
struct CurCharacByteView
{
    char m_pad00[0x39];
    char m_field39;                 // +0x39（分支 3）
    char m_field3a;                 // +0x3a（分支 3）
    char m_field3b;                 // +0x3b（分支 3）
    char m_pad3c[0x75 - 0x3c];
    unsigned char m_field75;        // +0x75（分支 0，无符号写出）
};

// m_characList 元素扩展视图：权威 struct _Charac_info（CUser.h，pack(1)，总长
// 0x14d1）中段为 pad；此处仅命名 ORIG 分支 2 访问的偏移（视图止于 +0x14ca）。
struct CharacListElemView
{
    int m_characNo;                 // +0x000
    char m_name[0x1d];              // +0x004
    char m_pad21[0x26 - 0x21];
    char m_job;                     // +0x026
    short m_level;                  // +0x027
    char m_field29;                 // +0x029
    char m_pad2a[0x39 - 0x2a];
    CommonTime m_createTime;        // +0x039（UpdateCharacView → long 写记录 +0x8c）
    char m_pad4d[0x83 - 0x4d];
    char m_forbidPlayFlag;          // +0x083（禁玩标记；到期清零并投递 SIG_FORBID_USER_TO_PLAY）
    int m_forbidPlayLeftSec;        // +0x084（剩余秒）
    char m_pad88[0xda - 0x88];
    char m_inventory[0xe40 - 0xda]; // +0x0da 每角色 CInventory 子对象（MakeEquipList 接收者）
    char m_fielde40;                // +0xe40（bit1 → 记录 flags bit4）
    char m_pade41[0xe5d - 0xe41];
    int m_fielde5d;                 // +0xe5d（UpdateCharacView 直拷记录 +0x90）
    char m_pade61[0xe66 - 0xe61];
    unsigned char m_reenterable;    // +0xe66（now < expireSec → 1，随 viewState 写出）
    char m_pade67[0x1240 - 0xe67];
    unsigned char m_field1240;      // +0x1240
    char m_pad1241[0x1246 - 0x1241];
    char m_field1246;               // +0x1246
    char m_pad1247[0x12ad - 0x1247];
    int m_expireSec;                // +0x12ad（与 CSystemTime::getCurSec() 比较）
    char m_pad12b1[0x14c6 - 0x12b1];
    int m_viewState;                // +0x14c6（==0xff 时写 8）
};
typedef char elem_view_size_ok[(sizeof(CharacListElemView) == 0x14ca) ? 1 : -1];

// GetGuildDBInfo()（STGuildDBInfoOnly，CUser.h 中 +0x00..0x94 为 pad）访问视图：
// +0x00 公会名字符串、+0x1b 随名写出的字节、+0x44 成员数、成员表自 +0x45 起
// 5 字节/项（int 等级 + char 职位）。
struct GuildMemberEntryView
{
    int m_grade;                    // 条目 +0x00（>0xc7 才写出）
    char m_position;                // 条目 +0x04（有符号写出）
};

struct GuildDbInfoView
{
    char m_guildName[0x1b];         // +0x00
    unsigned char m_field1b;        // +0x1b
    char m_pad1c[0x44 - 0x1c];
    unsigned char m_memberCount;    // +0x44
    GuildMemberEntryView m_members[1]; // 实际数量由 m_memberCount 给出
};
typedef char guild_view_size_ok[(sizeof(GuildDbInfoView) == 0x4a) ? 1 : -1];

// SIG_FORBID_USER_TO_PLAY 前 9 字节写入视图（SigTypes.h 现为 0x100 pad 占位；
// ORIG：memset 9 字节后 +0x00=characNo、+0x04=0、+0x05..0x08=0）。
struct SigForbidUserToPlayBody
{
    unsigned int m_characNo;        // +0x00
    unsigned char m_field04;        // +0x04
    unsigned int m_field05;         // +0x05..0x08
};

#pragma pack(pop)

}  // namespace

// b28d-lane5 N封顶: N97/P230 cmov; P11 符号扩展; P11 反向符号扩展; N18a/P200 拓宽中轉; P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865a44e | _ZN5CUser15make_basic_infoEPcc
#include "CUser/make_basic_info.inc"
{
    // 2026-09-10（G3CUser-B 负结论·余量登记）：栈槽集对照（报告 ORIG 汇编 vs 我方
    //   重建汇编）：ORIG 50 槽 / 我方 59 槽、整体平移 +0x28，平移后**仍多余 9 个
    //   具名槽**（-0x13c/-0x12c/-0x120/-0x11c/-0xb1/-0x65/-0x49/-0x3c/-0x31 等）。
    //   即本件确有可削的具名局部，但函数 2149 insn、字段写入 500+ 站点，定位需
    //   逐块比对；本轮未落（避免无直证改动）。建议后续单件专批：按上述槽地址回查
    //   我方汇编 store 点 → 对应源码具名量，按 P0/P183 内联。
    // b28d-solo-CUser p167 复核（ORIG 407 / OUR 404，本轮 10→3 缺口）：
    //   已修 —— creatureName/guild->m_guildName/GetGuildName 长度具名复用（消 strlen×3）、
    //   teamName getter 三次独立调用（P183）、blood->GetEnterCount 具名复用（消 EXTRA×1）、
    //   两处 getCurCharacR 指针缓存改逐点重调（P183，消 MISSING×2）。
    //   余 3 缺口待后续：MISSING put_byte×2 + put_int×1（ORIG 有我们无的字段写入段，
    //   疑某条件分支 3 个 put；需逐块比对 0x0865ae63/0x0865b00d 两区）。
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)buf;
    PacketGuard& pg = *(PacketGuard*)buf;

    if (type == 1)
    {
        // ---- 分支 1：ENUM_USERINFO_ADDITION（ORIG +0xbb9）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x31c3,
                                  "Error ENUM_USERINFO_ADDITION, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        // 公会技能/成员双组块 ×2（保留索引-回填计数模式）
        const GuildDbInfoView* guild =
            (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[6 + (i + 0x20) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[6 + (i + 0x20) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[6 + 1 + (i + 0x20) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[0xe + (i + 0xe8) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[0xe + (i + 0xe8) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[0xe + 1 + (i + 0xe8) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        pkt.put_byte(6);
        for (int i = 0; i < 6; ++i)
            pkt.put_byte((int)(char)getDemensionInoutValue((char)i));
        if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            pkt.put_byte(0);
        else
            pkt.put_byte((int)GetCurRevengeDungeonCount());
        pkt.put_byte((int)(char)getUltimateInoutValue(2));
        const TOD_UserState* tod = (const TOD_UserState*)GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xd);
        pkt.put_byte(tod->getEnterCount());
        int limit = (int)(char)G_CDataManager()->get_limit_inout_count(
            ENUM_BLOOD_TYPE_1);
        const CCharacBloodDungeon* blood =
            (const CCharacBloodDungeon*)GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)1);
        // b28d-solo-CUser p167：ORIG 0x865b623 单次 GetEnterCount 落 -0x48 槽后
        // 两处复用（P0 槽往返）⇒ 具名局部。
        const int bloodEnterCount = blood->GetEnterCount();
        if (limit - bloodEnterCount < 0)
            pkt.put_byte(0);
        else
            pkt.put_byte(limit - bloodEnterCount);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    else if (type < 2)
    {
        if (type == 0)
        {
            // ---- 分支 0：ENUM_USERINFO_MINIMUM（ORIG +0x52）----
            if (getCurCharacR() == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__,
                    0x308c,
                    "Error ENUM_USERINFO_MINIMUM, getCurCharacR() == 0");
                return 0;
            }
            lock();
            pkt.put_short((int)(get_unique_id() & 0xffff));
            // ORIG 0x0865a..：put_int(strlen)/put_str/put_str 的 strlen 各自重调 getCurCharacName()。
            pkt.put_int((int)strlen(getCurCharacName()));
            pkt.put_str(getCurCharacName(), (int)strlen(getCurCharacName()));
            pkt.put_byte(get_charac_job());
            pkt.put_byte((int)getCurCharacGrowType());
            if (G_GameWorld()->IsCharacterLevelRevisionChannel())
                pkt.put_byte(0x55);
            else
                pkt.put_byte(get_charac_level());
            pkt.put_byte(get_pvp_grade());
            const PvpResultBasicView* pvp =
                (const PvpResultBasicView*)getPVPResultR();
            pkt.put_byte(pvp->m_field5c);
            pkt.put_byte(GetUserState());
            ((CInventory*)getCurCharacInvenR())
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)0,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
                pkt.put_int((int)GetSchoolNo());
            if (G_GameWorld()->GetChannelType() == 0xe)
                pkt.put_int(GetCurCharacOnlinePreliminaryTeamId());
            else
                pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());

            // ORIG 0x0865b..：每处 creature 访问重取 getCurCharacInvenR()->GetCreatureMgrR()
            // （P36/P44），creatureName 仍缓存（ORIG 亦缓存 local_9c）。
            pkt.put_int(
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                    ->GetCreatureItemId());
            const char* creatureName =
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                    ->GetCreatureName();
            // b28d-solo-CUser p167：ORIG 0x865a7b5 单次 strlen 落 -0x94 槽后复用
            //（put_int(len)+put_str(name,len)）⇒ 具名 len 局部（P0 槽往返证据）。
            const int creatureNameLen = (int)strlen(creatureName);
            pkt.put_int(creatureNameLen);
            pkt.put_str(creatureName, creatureNameLen);
            pkt.put_byte(
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                        ->IsDieCreature()
                    ? 0
                    : 1);

            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_int(getCurrCharacBlackCount());

            const GuildDbInfoView* guild =
                (const GuildDbInfoView*)GetGuildDBInfo();
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
            {
                // ORIG：CLeagueMatch::GetPlayCountPerOneTeam() 读全局
                // nPlayerCountPerOneTeam_（+0x00 字节）→ put_byte(& 0xff)。
                pkt.put_byte((int)CLeagueMatch::GetPlayCountPerOneTeam());
                pkt.put_int((int)m_str8e080.length());
                pkt.put_str(m_str8e080.c_str(), (int)m_str8e080.length());
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                // ORIG：online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam()
                // （内部取 G_CDataManager()->GetPvPTeamCount()）→ put_byte(& 0xff)。
                pkt.put_byte(
                    (int)(online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam()
                          & 0xff));
                // b28d-solo-CUser p167：ORIG 0x865a99a/0x865a9c1/0x865a9d6 三次
                // 独立调用 getter（P183 禁缓存）：#1→strlen→put_int；
                // #2→strlen→#3→put_str。
                pkt.put_int(
                    (int)strlen(GetCurCharacOnlinePreliminaryTeamName()));
                pkt.put_str(
                    GetCurCharacOnlinePreliminaryTeamName(),
                    (int)strlen(GetCurCharacOnlinePreliminaryTeamName()));
            }
            else
            {
                pkt.put_byte((int)guild->m_field1b);
                // b28d-solo-CUser p167：ORIG 0x865aa1b 单次 strlen 复用。
                const int guildNameLen = (int)strlen(guild->m_guildName);
                pkt.put_int(guildNameLen);
                pkt.put_str(guild->m_guildName, guildNameLen);
            }

            pkt.put_int(GetCurCharacChaosPoint());
            pkt.put_byte(0xe < GetAge() ? 1 : 0);
            pkt.put_byte((int)(char)getPowerSide());
            pkt.put_byte((int)(char)isJoinPowerWar());
            pkt.put_byte(GetCurCharacExpertJobType());
            pkt.put_int(GetCurCharacExpertJobExp());
            pkt.put_byte((int)getDisguiseKind());
            pkt.put_short((int)getDisguiseIndex());
            pkt.put_byte((int)(char)get_charac_visible_values());

            if (G_GameWorld()->IsForbiddenChat(getCurCharacName()))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenChat(getCurCharacName());
                if (left == 0)
                    G_GameWorld()->EnableChat(getCurCharacName());
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3154, 0)
                        ("%s FCT1: %ld\n", getCurCharacName(), left);
                }
                left = left / 60000u;
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3156, 0)
                        ("%s FCT2: %ld\n", getCurCharacName(), left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }
            if (G_GameWorld()->IsForbiddenMove(getCurCharacName()))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenMove(getCurCharacName());
                if (left == 0)
                    G_GameWorld()->EnableMove(getCurCharacName());
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3163, 0)
                        ("%s FMT1: %ld\n", getCurCharacName(), left);
                }
                left = left / 60000u;
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3165, 0)
                        ("%s FMT2: %ld\n", getCurCharacName(), left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }

            pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
            // b28d-solo-CUser p167：ORIG 0x865ae5d-0x865ae7c 每使用点重调
            // getCurCharacR()（P183 禁指针缓存）⇒ 内联视图 cast。
            pkt.put_byte((int)((const CurCharacByteView*)getCurCharacR())
                             ->m_field75);
            pkt.put_short((int)getCurCharacUsedFatigue());
            pkt.put_byte((int)GetServerGroup());
            pkt.put_short((int)(short)GetUserPowerWarPoint());
            pkt.put_byte(GlobalData::s_power_manager->GetRankingByCharacNo(
                (unsigned int)getCurCharacNo()));
            pkt.put_short((int)(short)GetCurCharacEmotion());
            pkt.put_byte((int)(char)GetCurCharacSkillTreeIndex());
            // b28d-CUser p167 真缺口：ORIG 0x0865af90/0x0865afa5 为 if/else **两个**
            //   put_byte 调用点（`test %al; je <put_byte(0)>`），三目只发一个。
            if (IsReturnUser() != 0)
                pkt.put_byte(1);
            else
                pkt.put_byte(0);
            pkt.put_byte((int)getLinkCharacSlotIndex());
            pkt.put_byte((int)getCharacLinkType());
            unlock();
        }
    }
    else if (type == 2)
    {
        // ---- 分支 2：账号角色列表（ORIG +0x19e3；无当前角色守卫）----
        lock();
        pkt.put_byte((int)getCharacSlotLimit());
        pkt.put_byte((int)getSlotEffectCount());
        pkt.put_byte(GetManageLevel());
        pkt.put_int(GetManagePoint());
        int written = 0;
        int countIdx = pkt.get_index();
        pkt.put_short(written);
        const int nowSec = GlobalData::s_systemTime_.getCurSec();
        for (int i = 0; i < (int)m_characList.size(); ++i)
        {
            if (G_GameWorld()->GetChannelType() == 0xe
                && (unsigned int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_characNo != getOnlinePreliminaryCharacNo())
                continue;
            pkt.put_short(written);
            ++written;
            pkt.put_int((int)strlen(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name));
            pkt.put_str(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name, (int)strlen(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name));
            pkt.put_byte((checkSpecialChar(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name) != false
                          || checkSpecialCharName(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name) != false) ? 1 : 0);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_job);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field29);
            pkt.put_byte((int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_level);
            pkt.put_byte(0);
            if (!(nowSec < reinterpret_cast<CharacListElemView&>(m_characList[i]).m_expireSec))
                m_characList[i].m_fieldE66 = 0;   // +0xe66 ≡ ElemView::m_reenterable（Main 视图 cast 反模式修法：具名直访问）
            else
                m_characList[i].m_fieldE66 = 1;
            if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_viewState == 0xff)
                pkt.put_byte(8);
            else
                pkt.put_byte(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_reenterable != 0 ? 4 : 0);
            ((CInventory*)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_inventory)
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)2,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());
            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_fielde40);
            pkt.put_byte((int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field1240);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field1246);
            if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayFlag == '\0')
            {
                pkt.put_int(0);
            }
            else if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec < 1)
            {
                pkt.put_int(0);
                m_characList[i].m_pad83 = 0;      // +0x83 ≡ ElemView::m_forbidPlayFlag
                m_characList[i].m_field84 = 0;    // +0x84 ≡ ElemView::m_forbidPlayLeftSec
                // P259：ORIG 无 Stream* 具名槽（Acquire 结果 %eax 直传 C1）。
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("user.cpp", 0x344c), true);
                // ORIG 0x0865c4f9/0x0865c524/0x0865c53e：`*guard << v`（operator*
                // 链产生 deEv）+ `guard->GetInBuffer`（ptEv 链），P103/P132。
                (*guard)->operator<<(0x93);
                (*guard)->operator<<((int)GetUID());
                // ORIG 0x0865b40b：GetInBuffer<SIG_FORBID_USER_TO_PLAY>()（真 SIG 类型
                // 保证 mangled `GetInBufferI23SIG_FORBID_USER_TO_PLAYEEPT_v` 匹配）；
                // 前 9 字节 payload 借 SigForbidUserToPlayBody 视图写。
                SIG_FORBID_USER_TO_PLAY* sig =
                    guard.operator->()->GetInBuffer<SIG_FORBID_USER_TO_PLAY>();
                SigForbidUserToPlayBody* body =
                    (SigForbidUserToPlayBody*)sig;
                memset(body, 0, 9);
                body->m_characNo = (unsigned int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_characNo;
                body->m_field04 = 0;
                body->m_field05 = 0;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            else
            {
                int days = reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec / 0x15180;
                // b28d-CUser p167 真缺口：ORIG 为 if/else 两个 put_int 调用点
                //   （Ghidra G101/G102；三目只发一个）。
                if (days < 1)
                    pkt.put_int(1);
                else
                    pkt.put_int(days);
            }
        }
        pkt.put_short(countIdx, written);
        unlock();
    }
    else if (type == 3)
    {
        // ---- 分支 3：ENUM_USERINFO_OTHER_CHARACTER（ORIG +0x127f）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__,
                0x328c,
                "Error ENUM_USERINFO_OTHER_CHARACTER, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)3, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        const GuildDbInfoView* guild = (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[6 + (i + 0x20) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[6 + (i + 0x20) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[6 + 1 + (i + 0x20) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[0xe + (i + 0xe8) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[0xe + (i + 0xe8) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[0xe + 1 + (i + 0xe8) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        // ORIG 0x0865c..：每字段重调 getPVPResultR()（P44），共 4 次。
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_totalWinCount);
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_field04);
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_pvpGrade);
        pkt.put_byte(((const PvpResultBasicView*)getPVPResultR())->m_field5c);
        // b28d-solo-CUser p167：ORIG 0x865bc41/0x865bc65/0x865bc89 每字段重调
        // getCurCharacR()（P183 禁指针缓存）。
        pkt.put_byte((int)(char)((const CurCharacByteView*)getCurCharacR())
                         ->m_field39);
        pkt.put_byte((int)(char)((const CurCharacByteView*)getCurCharacR())
                         ->m_field3a);
        pkt.put_byte((int)(char)((const CurCharacByteView*)getCurCharacR())
                         ->m_field3b);
        pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
        pkt.put_short((int)(short)GetUserPowerWarPoint());
        pkt.put_int(GetGuildPowerWarPoint());
        // b28d-solo-CUser p167：ORIG 0x865bd0b/0x865bd36 GetGuildName 两次 + 单次
        // strlen 落 -0x38 槽 ⇒ 长度具名、getter 不缓存（P183）。
        const int guildNameLen = (int)strlen(GetGuildName());
        pkt.put_int(guildNameLen);
        pkt.put_str(GetGuildName(), guildNameLen);
        pkt.put_byte((int)GetGuildLevel());
        pkt.put_byte((int)(char)getPowerSide());
        // b28d-CUser p167 真缺口：ORIG 0x0865bdb4/0x0865bdc9 为 if/else **两个**
        //   put_byte 调用点（`test %al; je <put_byte(0)>`），三目只发一个。
        if (IsExistGuildAgit() != 0)
            pkt.put_byte(1);
        else
            pkt.put_byte(0);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    return 1;
}
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865dd14 | _ZN5CUser10send_equipEi
#include "CUser/send_equip.inc"
{
    // ORIG 0x0865dd14：state(+0x8cfc4) ∈ {5,8,10,0xc} 且 slot==0x16 → 发给自己；
    // 否则（slot<0xc 或 0x16）→ 广播 make_basic_info(type 0) 给全房间。
    if (slot >= 0xc && slot != 0x16)
        return;
    // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x0865dd2c/0x0865dd3e/0x0865dd50/
    //   0x0865dd62 四处**各自** `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax;
    //   cmp $..,%eax; je 0x0865de35` —— m_chState 每次经 this 重读（无 state 槽、无
    //   寄存器缓存），4 个条件各自直跳同一落点（inState **无槽**）⇒ 我方
    //   `int state` / `bool inState` 两个具名量 ORIG 均无，删并内联为 4 项 `||`。
    // b28d-CuE 2026-09-11（P47/P194 then-块物理序直证）：ORIG 0x0865dd2c/3e/50/62 四处
    //   各自 `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax; cmp $..,%eax; je 0x0865de35`
    //   —— 四个 **不等链失败** 才跳入后置的 then 块（0x0865de35 起才是 slot==0x16 检查
    //   + type-1 包体），fall-through 落 **0x0865dd74 起的广播体（else）**；我方旧
    //   `== || == || == || ==` 形式被 4.4 折成 3×je + 1×jne 且把 then 块放 fall-through
    //   （0x0865dc.. jne +0x116，then 体在 0x0865dc2a 之前）⇒ 多 2 条且块序相反。
    //   改写为语义等价的 `!= && != && != && !=` 使每个失败条件各自 `je` 跳后置块。
    if (m_chState != 0xc && m_chState != 5 && m_chState != 8 && m_chState != 10)
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        G_GameWorld()->send_all(packet, this);
    }
    else
    {
        // [L9-GrpL 2026-09-11 尾块跳转合并] ORIG 0x0865def2 全函数**只有一条** `jmp epilogue`
        //   （`jmp 0865def5 <+0x1e1>` 后跟 `nop`，0x0865def5 即 `add $0x30,%esp`）；
        //   我方 `if (slot != 0x16) return;` 额外产一条 `jmp +0x1e4`（raw 128 vs ORIG 125，
        //   尾部 `jmp;nop;jmp;nop` 双岛）⇒ 去掉裸 return，改正向 if 包裹：
        //   4.4 对 `if (cond) {…}` 恒 `jne <跳过体>`，跳过目标即 else 体出口（不再生成
        //   return 的跳转指令）。
        if (slot == 0x16)
        {
            PacketGuard packet;
            ((InterfacePacketBuf*)&packet)->put_header(0, 2);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(1);
            make_basic_info((char*)&packet, (char)0);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x086893ac | _ZN5CUser12AddDBLogItemEjj20ENUM_DBLOG_ITEM_TYPE
#include "CUser/AddDBLogItem.inc"
                         ENUM_DBLOG_ITEM_TYPE value)
{
    // ORIG 0x08689360：循环步进用后置 it++（ppEi 符号），非 ++it。
    // 槽序实证（2026-09-05 CUserC8 修正前批方向）：ORIG info=-0x14（近，5 处
    // lea -0x14）/it=-0x20（远）——4.4 后声明占近槽（技巧 21 正用）：info 声明
    // 置于 for-init 之后。
    // 残差 N82/P112 族：4.1 it=-0x14/info=-0x20 vs 4.4 it=-0x20/info=-0x1c，
    // begin() sret 槽分配跨版本恒差；dead 前置/后置只整体平移不换相对序
    // （nl11_probe6 实证），源形不可复现。
    // b28d-solo-CUser 复测：info 声明前置到 for 之前（P212/P309 头部声明族）实测
    // 槽位逐字不变（it -0x20 / info -0x1c）⇒ 4.4.6 按**首次使用序**分配（N98），
    // 声明序非开关；维持 N82/P112 封顶。
    // b28d-solo-CUser 二试（P309 全头部声明 + 声明序按 ORIG 槽浅→深）：
    std::vector<_DBLogItemInfo>::iterator it = m_dbLogItems.begin();
    _DBLogItemInfo info;
    for (; it != m_dbLogItems.end(); it++)
    {
        if (it->m_itemIdx == itemIdx)
        {
            it->m_type += type;
            return;
        }
    }
    memset(&info, 0, 0xc);
    info.m_itemIdx = itemIdx;
    info.m_type = type;
    info.m_value = value;
    m_dbLogItems.push_back(info);
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681224 | _ZN5CUser13addAvatarItemEmccPKc20eAvatarItemAddReason
#include "CUser/addAvatarItem.inc"
                         const char* ipgAgency, eAvatarItemAddReason reason)
{
    // b28d-GrpD P183/P329 按 ORIG 0x08681224 直证：ORIG 无 G_CDataManager() 具名槽
    //   （+0x18 `call G_CDataManager` 后 %eax 直接作 this 传给 find_item），也**无**
    //   getCurCharacInvenW 具名槽（+0x9f 的 %eax 直接作 this 传 AddAvatarItem）；
    //   我方旧形多 -0x1c/-0xc 两槽（帧 0x64 vs ORIG 0x5c，并多 push esi/edi 需求）。
    // 2026-09-11（H3CUser）：ORIG 0x08681224 有**两个**具名量槽——`-0x28`
    //   （find_item 结果，`cmpl $0,-0x28` 判空）与 `-0x24`（`mov -0x28,%eax;
    //   mov %eax,-0x24` 拷贝，后续 getAvatarTypeSelect/getUsablePeriod 的 this 均
    //   从 -0x24 装载）。我方旧形单槽 -0x24 且判空直用 ⇒ 少 2 条 mov、
    //   后续槽整体上移 4（-0x20/-0x1c vs ORIG -0x20/-0x1c 恰好同，因 ORIG 多占
    //   一槽而起始更深）。补第二个具名局部复现。
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
        return -1;
    CEquipItem* pEquip = (CEquipItem*)pItem;
    // 判据⑤（重载选择）：ORIG 0x08681224 调 `_ZNKSt6vectorI20stAvatarTypeSelect_t
    //   SaIS0_EEixEj`（const operator[]）⇒ 视图必须为 const 指针（P96 cv 旋钮）。
    const std::vector<stAvatarTypeSelect_t>* pSelect =
        (const std::vector<stAvatarTypeSelect_t>*)pEquip->getAvatarTypeSelect();
    int usablePeriod = pEquip->getUsablePeriod();
    if (param3 < (int)pSelect->size())
        usablePeriod = (int)(*pSelect)[param3].m_field0;
    return ((CInventory*)getCurCharacInvenW())->AddAvatarItem(
        itemIdx, usablePeriod, 0, param2, param3, ipgAgency, reason, 0, 0);
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c65a | _ZN5CUser18SendUpdateItemListENS_11eSendTargetE14ENUM_ITEMSPACEi
#include "CUser/SendUpdateItemList.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    // ORIG 0x0867c65a：空间→INVEN_TYPE 映射（1→2、7→3、3→0、其余→1）；
    // 每分支独立重调 getCurCharacInvenR（访问器结果禁缓存，战术 P183/P48）；
    // MakeItemPacket != 1 → 返回 0（不发包），否则 finalize + SendPacket 返回 1。
    // b28d-PoolCUser 重写（2026-09-09，P17/P45）：ORIG 判定为 `cmpl $K,0x10(%ebp)`
    //   形参槽直比 + jne 下一检查的**线性比较链**（1→7→3→else），非 switch 决策树
    //   （4.4 switch 恒产值载入+二分序 3/7/1）⇒ 改 if-else-if 链、书写序按 ORIG。
    //   尾段 ORIG `call SendPacket; mov $0x1,%ebx` —— SendPacket 返回值**被丢弃**
    //   （非 mov %eax,%ebx）⇒ 调用作语句 + `return true`。
    if (space == 1)
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)2, slot, packet) != 1)
            return 0;
    }
    else if (space == 7)
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)3, slot, packet) != 1)
            return 0;
    }
    else if (space == 3)
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)0, slot, packet) != 1)
            return 0;
    }
    else
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet) != 1)
            return 0;
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return true;
}

// 0x0868bc7c
// ORIG：无角色 → -1；无专家职业 → 0；否则按 GetExpertJobScript 的
// isBoundaryExpValue/GetLevel（_ZN17STExpertJobScript*）计算等级，
// 边界值时用 ExpertJobEtcScript 内 map<uchar,short>（等级→所需角色等级）修正：
// 查不到或角色等级不足 → 等级 -1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868bc7c | _ZN5CUser20GetCurExpertJobLevelEi
#include "CUser/GetCurExpertJobLevel.inc"
{
    if (getCurCharacR() == 0)
        return -1;
    if (GetCurCharacExpertJobType() == 0)
        return 0;
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
    // b28d-solo-CUser P171：ORIG 0x0868bc7c 无 script 判空（我方多 `cmpl $0,-0x1c; jne;
    //   mov $0,%eax; jmp` 段）⇒ 删防御照抄。
    char isBoundary = script->isBoundaryExpValue(exp);
    int level = script->GetLevel((unsigned int)exp);
    if (isBoundary != 0)
    {
        // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x0868bd14 `call G_CDataManager;
        //   mov %eax,(%esp); call GetExpertJobEtcScript; mov %eax,-0xc(%ebp)` →
        //   0x0868bd2a 起直接把 `-0xc(%ebp)` 当 this 传 `map::find`
        //   (0x0849fd46)，全程**无** `cmpl $0x0,-0xc(%ebp); je` 判空；Ghidra 反编译 C
        //   同一位置亦无判空 ⇒ 删我方 `if (etc != 0)` 发明防御。
        // 推断：etc 对象首部为 std::map<uchar,short>（等级 → 所需角色等级）
        std::map<unsigned char, short>* m =
            (std::map<unsigned char, short>*)G_CDataManager()->GetExpertJobEtcScript();
        std::map<unsigned char, short>::iterator it =
            m->find((unsigned char)level);
        if (it != m->end())
        {
            if (it->second <= get_charac_level())
                return level;
        }
        return level - 1;
    }
    return level;
}

#include "CUser/is_clear_stealingSkillMission.inc"
{
    // ORIG：GetCharacExpandData(8) 结果暂存 -0xc(%ebp) 后调
    // CMissionList_Charac::isClearStealingSkillMission
    //（mission_list_is_clear 为 stub 返回 false，禁用）
    void* data = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return reinterpret_cast<CMissionList_Charac*>(data)
        ->isClearStealingSkillMission();
}

// ORIG T 0x08679736：公会技能公告。文案 = ServerString(0x3ea) + "%d "(count) +
// skillName + ServerString(0x3eb)；总长 ≥0x78 丢弃；公会公告板写入
// (boardNo=0, charNo, job=-1, growType=-1, guildKey)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679a36 | _ZN5CUser21makeGuildSkillMessageEPKci
#include "CUser/makeGuildSkillMessage.inc"
{
    if (skillName == 0)
        return;
    // b28d-solo-CUser 试形（P28/P212 声明序）：ORIG 0x08679a36 的 board[120] 清零
    //   在 +0x16（GetServerString 之前），countStr[256] 清零在 +0x52（之后，且为非 4
    //   对齐慢路 -0x19f）⇒ 声明序 board → body → countStr。
    char board[120] = {0};
    std::string body = g_ServerString_.GetServerString(0x3ea, 0);
    char countStr[255] = {0};   // ORIG 0x08679a88 rep stos 0xff 字节（-0x19f 起）
    sprintf(countStr, "%d ", count);
    body += countStr;
    // ORIG 0x08679b3f..0x08679ba3：先构造 name(skillName)（local_28），再取尾段串
    // （local_2c = ServerString(0x3eb)），ORIG 0x14e this=-0x24/arg=-0x28 ⇒ `name += tail`，
    // 0x160 this=-0x20/arg=-0x24 ⇒ `body += name`（两次 operator+=(string,string)）。
    std::string name(skillName);
    std::string tail = g_ServerString_.GetServerString(0x3eb, 0);
    name += tail;
    body += name;
    if (body.length() >= 0x78)
        return;
    memcpy(board, body.c_str(), body.length());
    // ORIG 0x08679c00..0x08679c5d：无 proxy 判空，实参右→左先求 guildkey(%esi)、
    //   charNo(%ebx) 再求 receiver（GetServerGroup→GetServerProxy）⇒ 单条链式表达式。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                              -1, -1, (unsigned int)get_charac_guildkey());
}

// ============================================================================
// CUser::makeGuildItemUpgradMessage @ 0x8679cee（返回 1 已广播 / 0 未达条件）
//   公会强化公告。isAmplify && to>10 用 "+to itemName"，否则 from>10 用
//   "+{from+1} itemName"。消息 = GetServerString(0x3e8) + 数字串 +
//   GetServerString(0x3ec 强化 / 0x3ed 降级) + 角色名；长度>0x77 不发，
//   否则经 CGuildServerProxy::SendGuildBoardWrite 广播。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679cee | _ZN5CUser26makeGuildItemUpgradMessageEPKciib
#include "CUser/makeGuildItemUpgradMessage.inc"
                                       int to, bool isAmplify)
{
    // ORIG T 0x08679cee 全录（汇编+Ghidra C 双段读完）：返回类型 void
    // （epilogue 0x867a236 无 %eax 写，0/1 仅内部 %ebx goto 判定——Main 已落
    // CUser.h 声明 void）。结构：入口 board[120] rep stos 清零 →
    // **双分支整段复制**（4.1 无跨分支复用，gap196 主因）：
    //   分支1 isAmplify&&to>10：name/pre(0x3e8)/suf(0x3ec) 三 string 构造 →
    //     msg[0xff] memset → sprintf("+%d %s",to) → pre+=msg → pre+=suf →
    //     name+=pre → len>0x77 seta 物化 → !tooLong: memcpy(board,c_str,len)
    //     （len 先算存寄存器再 c_str，Ghidra sVar2/pvVar3 序）→ sent=1；
    //   分支2 !isAmplify&&from>10：同构 key=0x3ed、"+%d %s" from+1（lea 0x1）；
    //   两分支块末 dtor suf/pre/name 反序链 + EH resume pad →
    //   共享 SEND 尾（0x867a1c7）：guildkey、characNo、group 三取后
    //   GetServerProxy、SendGuildBoardWrite(board,0,characNo,-1,-1,guildkey)。
    // 【N 封顶（Main 批准 2026-09-05）】语义/调用序/清零形全对齐；残差：
    //   slot 漂移 12B（4.4 memcpy 临时落槽 vs 4.1 寄存器承载，N76/N82 族）、
    //   sent 二次归一化 `test;jne;mov$0;jmp;mov$1` 岛（N7 channel-predicate 形）、
    //   EH pad 布局。
    // 【2026-09-05 21:0x 事故重建】本函数体曾被 StringResource 批次误写覆盖，
    // 从 HEAD(batch-10) 恢复后按会话记录重放。
    char board[0x78] = {0};
    if (isAmplify && to > 10)
    {
        std::string name(getCurCharacName());
        std::string pre = g_ServerString_.GetServerString(0x3e8, 0);
        std::string suf = g_ServerString_.GetServerString(0x3ec, 0);
        char msg[0xff] = {0};   // ORIG 0x8679dd0 非对齐 peel+rep stos 展开
        sprintf(msg, "+%d %s", to, itemName);
        pre += msg;
        pre += suf;
        name += pre;
        // ORIG 0x8679e93 `cmp $0x77,%eax; seta %al; test; je 续` ⇒ 超长早返
        //   （0x8679e9d mov $0,%ebx 仅为析构后跳过 SEND 的编译器标志）。
        if (name.length() > 0x77)
            return;
        memcpy(board, name.c_str(), (unsigned int)name.length());
    }
    // ORIG 0x8679f70 `movzbl isAmplify; xor $1; test; je 尾` ⇒ `!isAmplify` 守卫（P292）；
    //   0x8679f82 `cmpl $0xa,0x10(%ebp); jle 尾` ⇒ from<=10 直接返回。
    else if (!isAmplify)
    {
        if (from > 10)
        {
            std::string name(getCurCharacName());
            std::string pre = g_ServerString_.GetServerString(0x3e8, 0);
            std::string suf = g_ServerString_.GetServerString(0x3ed, 0);
            char msg[0xff] = {0};   // ORIG 0x867a02c 同形第二份
            sprintf(msg, "+%d %s", from + 1, itemName);   // ORIG 0x867a08a lea 0x1(%eax)
            pre += msg;
            pre += suf;
            name += pre;
            if (name.length() > 0x77)
                return;
            memcpy(board, name.c_str(), (unsigned int)name.length());
        }
        else
            return;
    }
    else
        return;
    // ORIG 0x867a1c7..0x867a230：无 proxy 判空，实参右→左先 guildkey(%esi)、
    //   charNo(%ebx) 再 receiver ⇒ 单条链式表达式。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(), -1, -1,
                              (unsigned int)get_charac_guildkey());
}

// ORIG 0x086787fc：改成长类型主分支（set_grow_type.md 汇编+反编译 C 全文核对）。
// 校验 first<6/second<3；second==0 重做任务列表(0x1f→0x20)；reason==1||second==0
// 清 QP；写历史日志；setCurCharGrowthType → enableSaveCharacView → second==0 时
// adjust_charac_stat + state==3 回 ENUM_USERINFO_ADDITION 包；reason∈{2,0} 且
// second==0 时双树清空重建+CSkillChanger+send_skill_info；get_give_skill 取技能表
// growtype_skill 双树（buf!=null 编码回填双树计数块）；成功后 _UpdateUserInfo4Guild
// + Guild/Monitor SendCharLevelGrowType + processGrowTypeEvent。
// name 实为 InterfacePacketBuf*（CUserOnlyOrig.cpp:4952/4977 传 &ack 需双树编码回填；null=直接应用）。
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x086787fc | _ZN5CUser13set_grow_typeEhhPc21eChangeGrowTypeReason
#include "CUser/set_grow_type.inc"
                          char* name, eChangeGrowTypeReason reason)
{
    if (firstGrow >= 6 || secondGrow >= 3)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x713b, "User %s - grow type < 1 || grow type >= GROW_TYPE_NUMBER",
            getCurCharacName());
        return;
    }

    if (secondGrow == 0)
    {
        // ORIG：主职业变更 → 使命列表 kind 0x1f 移除、0x20 重建。
        CMissionList_Charac* ml =
            (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ml->Remove_Mission(0x1f);
        ml->MakeMissionList_JustKind(*this, 0x20);
    }
    if (reason == (eChangeGrowTypeReason)1 || secondGrow == 0)
    {
        CQuestShop* questShop =
            (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
        if (questShop != 0)
        {
            questShop->clearQP(this);
            questShop->sendCharacQp(this, 1);
        }
    }

    // ORIG:if/else 两分支各自调用 getCurCharacGrowType/getCurCharacVill/get_charac_job。
    if (secondGrow == 0)
    {
        // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x086788fc 段
        //   `call getCurCharacGrowType; movsbl %al,%edi`（oldGrow **无槽**、%edi 保活），
        //   紧随 `movzbl -0x10c(%ebp),%esi`（firstGrow 参数槽）与 getCurCharacVill
        //   的 `movsbl %al,%ebx` ⇒ 原码把 getCurCharacGrowType() 直接写在实参位，
        //   我方 `char oldGrow` 具名量 ORIG 无（且其 1 字节槽另占帧）。
        m_historyLog.ChangeGrowType(
            get_charac_job(), (int)(signed char)getCurCharacVill(),
            (int)firstGrow, (int)(signed char)getCurCharacGrowType(), 1, reason);
    }
    else
    {
        // 2026-09-10（G3CUser-B，P0）：同上一分支 —— ORIG 0x0867897e
        //   `call getCurCharacGrowType; movsbl %al,%edi`（无槽），0x08678998
        //   `movsbl %al,%ebx`（vill 无槽）⇒ 内联。
        m_historyLog.ChangeGrowType(
            get_charac_job(), (int)(signed char)getCurCharacVill(),
            (int)secondGrow, (int)(signed char)getCurCharacGrowType(), 2, reason);
    }
    InterfacePacketBuf* buf = (InterfacePacketBuf*)name;
    setCurCharGrowthType((char)firstGrow, (char)secondGrow);
    m_characterView.enableSaveCharacView();
    if (secondGrow == 0)
    {
        // ORIG：主职业切换重算属性；角色选择视口（state==3）回 ADDITION 包。
        adjust_charac_stat();
        if (m_chState == 3)
        {
            PacketGuard pg;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&pg;
            pkt->clear();
            pkt->put_header(0, 2);
            pkt->put_byte(1);
            pkt->put_short(1);
            make_basic_info((char*)pkt, (char)1);
            pkt->finalize(true);
            Send(pg);
        }
    }

    std::vector<std::pair<int, int> > giveVec;
    // 配对（D06 定义侧 2026-09-10 已 bool 化；A17 spec + ORIG 0x08678c6c 消费形
    //   `call get_give_skill; mov %al,-0x3d(%ebp)` = 1 字节直存无归一）⇒ 消费端局部
    //   必须同为 bool（Main 净账必配对律），否则 4.4 恒插 test/setne 归一。
    bool giveOk = false;
    // ORIG:每 get_give_skill 站点独立调用 G_CDataManager() 并解引 +0x14。
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        giveOk = 1;
    }
    else if (reason == (eChangeGrowTypeReason)2 ||
             reason == (eChangeGrowTypeReason)0)
    {
        if (secondGrow == 0)
        {
            // ORIG：双树清空重建（addSkillOnCreateCharacter 树 A +0x46 / 树 B +0x1de）、
            // CSkillChanger 初始化 + send_skill_info 后按 get_give_skill(...,type 1)。
            // ORIG:clear/set_parent/addSkill 各自调用 getCurCharacSkillW()。
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot, get_charac_job());
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot2, get_charac_job());
            {
                WongWork::CSkillChanger changer;
                changer.SkillInitialize(this, 0, false);
            }
            send_skill_info();
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, 0, giveVec, 1);
        }
        else
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, secondGrow, giveVec, 2);
        }
    }
    else if (reason == (eChangeGrowTypeReason)1)
    {
        // ORIG:secondGrow==0 与否则两站点各自调用。
        if (secondGrow == 0)
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, 0, giveVec, 1);
        }
        else
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, secondGrow, giveVec, 0);
        }
    }
    if (giveOk == 1)
    {
        // ORIG:pair<int,int> 默认构造站点(local_5c)。
        std::pair<int, int> pairLocal5c;
        if (buf == 0)
        {
            // ORIG：直接应用 —— 每项双树 growtype_skill。
            // 2026-09-11（H3CUser，负结论 N）：ORIG 0x08678ed7/0x08678fea/0x086790eb
            //   三处循环回边为 `setg`（有符号）⇒ 试 `int i` + `(int)giveVec.size()`；
            //   实测 insns 667→**668 反劣**（3 处只翻 1 处、另 2 处连带）⇒ 回滚，
            //   维持 size_t 形并按 N82/N18b 封顶。
            //   同件残余②（char 中转）：`CUserCharacInfo.h:825 getCurCharacGrowType()`
            //   → `char` 的提案**已移交 H7PartyGlobal 持写权**（Main 2026-09-11 裁决），
            //   本泳道不改该头；若 H7 落地后本件读数变化，由 Main 通知复编复核。
            for (size_t i = 0; i < giveVec.size(); ++i)
            {
                // ORIG:每字段读独立 operator[](second→first 序),ix ×4/迭代。
                int second0 = giveVec[i].second;
                int first0 = giveVec[i].first;
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                    get_charac_job(), first0, second0,
                    (ENUM_SKILL_TREE_KIND)0);
                int second1 = giveVec[i].second;
                int first1 = giveVec[i].first;
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                    get_charac_job(), first1, second1,
                    (ENUM_SKILL_TREE_KIND)1);
            }
        }
        else
        {
            // ORIG:树 0 / 树 1 两个显式循环(非 tree 变量循环)。
            {
                int countIdx = buf->get_index();
                int cnt = 0;
                buf->put_byte(0);
                for (size_t i = 0; i < giveVec.size(); ++i)
                {
                    const std::pair<int, int>& pr = giveVec[i];
                    int r = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(), pr.first, pr.second,
                        (ENUM_SKILL_TREE_KIND)0);
                    if (r > 0)
                    {
                        buf->put_byte(r);
                        buf->put_byte(pr.first);
                        buf->put_byte(pr.second);
                        ++cnt;
                    }
                }
                buf->put_byte(countIdx, cnt);
            }
            {
                int countIdx = buf->get_index();
                int cnt = 0;
                buf->put_byte(0);
                for (size_t i = 0; i < giveVec.size(); ++i)
                {
                    const std::pair<int, int>& pr = giveVec[i];
                    int r = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(), pr.first, pr.second,
                        (ENUM_SKILL_TREE_KIND)1);
                    if (r > 0)
                    {
                        buf->put_byte(r);
                        buf->put_byte(pr.first);
                        buf->put_byte(pr.second);
                        ++cnt;
                    }
                }
                buf->put_byte(countIdx, cnt);
            }
        }
        _UpdateUserInfo4Guild();
        // 2026-09-10（G3CUser-B）：ORIG 0x086790f4..0x0867917b（guild 段）——
        //   guildGrow 是唯一具名槽：0x0867910f `mov %eax,-0x118(%ebp)`（movsbl 后
        //   **4 字节** int 存 ⇒ 原码 `int guildGrow = getCurCharacGrowType();`）；
        //   level/characNo/accId **全无槽**（0x08679120 `movswl %ax,%edi`、
        //   0x08679136 `mov %eax,%esi`、0x08679143 `mov %eax,%ebx`，跨调用 callee-saved
        //   保活）；0x0867915d `call GetServerProxy` 结果**直接**作 this
        //   （0x08679178 `mov %eax,(%esp)`）——**无** `test %eax,%eax; je` 判空。
        //   ⇒ 删 guildLevel/guildCharacNo/guildAccId/guild 四具名量与 `if (guild != 0)`
        //   发明防御，实参内联，receiver 链式（ORIG 求值序：实参 → GetServerGroup →
        //   GetServerProxy → call）。
        int guildGrow = getCurCharacGrowType();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendCharLevelGrowType(get_acc_id(), get_charac_no(-1),
                                    (short)get_charac_level(), (short)guildGrow);
        // 2026-09-10（G3CUser-B）：monitor 段同构 —— monGrow 具名
        //   0x0867918e `movsbl %al,%eax; mov %eax,-0x114(%ebp)`（4 字节 int 槽）；
        //   0x0867919f `movswl %ax,%edi` / 0x086791b5 `mov %eax,%esi` /
        //   0x086791c2 `mov %eax,%ebx` 无槽；0x086791dc `call GetServerProxy` 结果
        //   0x086791f7 直传，无判空。
        int monGrow = getCurCharacGrowType();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendCharLevelGrowType(get_acc_id(), get_charac_no(-1),
                                    (short)get_charac_level(), (short)monGrow);
        processGrowTypeEvent(firstGrow, secondGrow, reason);
    }
    else
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x720c,
            "User %s - CUser::set_grow_type first_grow_type %d, second_grow_type %d",
            getCurCharacName(), (int)firstGrow, (int)secondGrow);
    }
}

// ---- 基础访问器（G1-3 桩替换） ----
#include "CUser/get_acc_id.inc"
{
    return m_accId;
}

#include "CUser/get_acc_name.inc"
{
    return m_accName;
}

#include "CUser/get_state.inc"
{
    return m_chState;
}

#include "CUser/set_state.inc"
{
    // ORIG 0x867edb2-0x867ee27（probe s13 实证）：`if(cur==0) goto END` 产
    // `test;sete;test;jne` 物化岛；bFight 绑 asm("al")（call 结果原位 no-op）
    // 末用点 4.4 出 `movzbl %al,%ebx` 单条跨 call 持值（技巧 22b 新形）；
    // invenR/GetCreatureMgrW 全链式零槽（技巧 37）；尾 `call;jmp; nop; epilogue`。
    m_chState = state;
    if (m_chState == 5)
        ClearItemSellCount();
    if (getCurCharacR() == 0)
        goto ST_END;
    {
        // b28d-solo-CUser：ORIG `call is_fighting; movzbl %al,%ebx` 无 bool 槽（跨
        // getCurCharacInvenR/GetCreatureMgrW 保活 %ebx）。具名 bFight 落 -0x9 ⇒ 内联实参。
        ((CInventory*)getCurCharacInvenR())
            ->GetCreatureMgrW()
            ->TurnStomach(is_fighting());
    }
ST_END:;
}

#include "CUser/GetUID.inc"
{
    // ORIG 0x80c8c9c movzwl 704a6,%eax；0x80c8ca3 cwtl；0x80c8ca4 movzwl %ax,%edx。
    // b28d-solo-CUser N封顶: 4.4 折叠 `(unsigned short)(short)` 中间 cwtl（探针 6 形：
    //   具名 rawHi→落槽、cast 链/语句表达式均无 cwtl），4.1 保留三段 = N18a 折叠族。
    // [CUserLane 2026-09-11 T1 实测无效] 形 `(unsigned short)((int)(short)f)` 仍无 cwtl
    //   （ndiff 1 不变）⇒ 复现第 7 形失败，维持 N18a 折叠族封顶。
    return (unsigned short)(short)m_field704a6 | ((int)(short)m_slotIDX << 16);
}

#include "CUser/get_unique_id.inc"
{
    return (unsigned short)m_field704a8;
}

#include "CUser/getDungeonGainedGold.inc"
{
    return &m_dungeonGainedGold;
}

#include "CUser/getHackAnalyzer.inc"
{
    return &m_hackAnalyzer;
}

#include "CUser/getGmQuestFlag.inc"
{
    return m_gameOption.m_gmQuestFlag;
}

#include "CUser/GetAccountLastPlayTime.inc"
{
    return m_premium.m_lastPlayTime;
}

#include "CUser/GetServerGroup.inc"
{
    return (ENUM_SERVER_GROUP)m_postChecksumData.m_serverGroup;
}

#include "CUser/GetCharacExpandData.inc"
{
    int frame_pad; (void)frame_pad;   // ORIG 0x80dd587 sub $0x18：调用点 2 outgoing 槽 + 16B 对齐（4.1 帧舍入），dead 局部无指令
    return m_characExpand.GetData(type);
}

#include "CUser/GetCharacExpandDataR.inc"
{
    int frame_pad; (void)frame_pad;   // 值版重载无 ORIG 对应符号；与引用版（0x828b5de sub $0x18）保持一致
    return m_characExpand.GetDataR(type);
}


// ---- quest point / piece helpers（ORIG：CUser 0x868ad88 / 0x868afd0 / 0x868adc2 / 0x868af66）----

#include "CUser/clearInitQuestPoint.inc"
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->setInitQp(0);
    }
    return;
}

#include "CUser/clearQuestPiece.inc"
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->setPiece(0);
    }
    return;
}

#include "CUser/incInitQuestPoint.inc"
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->incInitQp(val);
    }
    return;
}

#include "CUser/incQuestPiece.inc"
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    // ORIG 0x868af8c：双 early-return 形（probe k2 实证）——`je nop; body; je leave;
    // call; jmp leave; nop; leave`。getPiece 结果 spill -0xc dead（技巧 8）+
    // shop=-0x10（技巧 21 先声明远槽）。
    if (shop == 0)
        return;
    int deadPiece = shop->getPiece();
    (void)deadPiece;
    shop->addPiece(val);
    if (!send)
    {
        shop->sendCharacQuestPiece(this);
        return;
    }
}

#include "CUser/gainWinPoint.inc"
{
    // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c。
    if (winPoint != 0)
    {
        // ORIG 0x864fd2c：call 结果留 %eax 直读直存（`mov 0x58(%eax),%edx; add; mov %edx,0x58(%eax)`
        // 无具名槽——链式形；具名 wpW/wpR 反产 spill 槽撑帧 0x28）。
        getPVPResultW()->m_winPoint += winPoint;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, winPoint, reason);
    }
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868adfc | _ZN5CUser17setInitQuestPointEv
// 2026-09-11 CuA（剩余 1 条 `jmp` 的终判）：ORIG 0x868aeff `jmp 868af02` + 0x868af01
//   `nop` 是 4.1 为**空 else/空语句**发射的 nop 岛（if 体尾必须 jmp 跳过）；探针
//   /tmp/P5/p_nop p.cpp f1(`else ;`)/f2 在 4.4 下产物**逐条相同且无 nop/无 jmp**
//   ⇒ 该条 jmp 不可造（N）。本方其余指令已逐条对齐。
#include "CUser/setInitQuestPoint.inc"
{
    // ORIG 0x868adfc T：清空 init QP 与 piece，再按 NPC 任务表逐条累加已清任务奖励点。
    clearInitQuestPoint();
    clearQuestPiece();

    // b28d-solo-CUser p167：ORIG 0x0868adfc 用 `multimap::begin()/end() const`
    //   （无 iterator→const_iterator 转换）；我方非 const 指针致 +2 转换构造 ⇒
    //   容器指针 const 化（P96 旋钮）。
    const std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE);

    // 2026-09-10 gA（A 组）：ORIG 0x0868ae31 无判空（`mov %eax,-0x10(%ebp)` 后直接
    //   lea -0x14/call multimap::end）⇒ 删我方发明的 `if (npcList==0) return;`（判据库 1）。
    // ORIG 0x0868ae34 在循环外只调一次 end()（结果落 -0x14 槽，循环体内 0x0868ae63..
    //   0x0868af1f 无 end() call，`jmp cond` + 尾部 `call operator!=`）⇒ 具名缓存 end（P13）。
    const std::multimap<int, int>::const_iterator itEnd = npcList->end();
    for (std::multimap<int, int>::const_iterator it = npcList->begin();
         it != itEnd; ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);
        // [CUserLane 2026-09-11 T2] P223 尾标签岛：ORIG 0x868ae89 `je 0x868af01`（nop 岛）
        //   + body 尾 0x868aeff `jmp 0x868af02` ⇒ quest==0 与体尾在**不同**落点上，
        //   用两条独立 goto + 双尾标签复现（旧 if 包裹形被 4.4 jump-thread）。
        if (quest == 0)
            goto QUEST_NULL;
        // ORIG 0x0868ae8b..0x0868aeb5：先 `call operator->; mov 0x4(%eax),%ebx`
        //   取 it->second，再 `call getCurCharacQuestR; add $0x4,%eax` 直内联 receiver
        //   （无具名 cleared 指针槽）⇒ 删具名 cleared（P183 地址重物化铁律）。
        if (((WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4))
                ->isClearedQuest((unsigned int)it->second))
        {
            incInitQuestPoint(quest->m_field138);
            incQuestPiece(quest->m_field13a, quest->m_index, true);
        }
        goto LOOP_NEXT;
QUEST_NULL:;
LOOP_NEXT:;
    }
}

#include "CUser/GetPremiumInfo.inc"
{
    return (WongWork::CUserPremium*)&m_premium;
}

// ORIG W 0x0864f8e0：mov 0x854(%eax),%eax 直读 +0x854（outPremiumCount）。
#include "CUser/GetAdvantageCoin.inc"
{
    return m_outPremiumCount;   // +0x854（ORIG mov 0x854(%eax) 直读；m_pad 字段化后此越界读改具名成员）
}

// ORIG 0x08694a54：movzbl 0x85c(%eax),%eax 直读 +0x85c（overEquipFlag byte）。
#include "CUser/isAffectedUnlimitFatigue.inc"
{
    return m_overEquipFlag;
}

#include "CUser/getCurCharacQuestR.inc"
{
    return (UserQuest*)&m_quest;
}

// ORIG W 0x0814aa5e：enableSaveQuest() 后返回 m_quest（+0x858d4）。
#include "CUser/getCurCharacQuestW.inc"
{
    enableSaveQuest();
    return (UserQuest*)&m_quest;
}

#include "CUser/DecreaseGuildPowerWarPoint.inc"
{
    m_guildDB.m_powerWarPoint -= point;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867ed68 | _ZN5CUser11is_fightingEv
#include "CUser/is_fighting.inc"
{
    register int bit;   // 无初始化 POD 免槽（P177/P231）
    // ORIG 0x867ed68：无 state/bit 槽（P0 全寄存器直通）——ja 直落后置 zero 块、
    // 两 mask 岛（mov$1/mov$0）后置共享；(bit&0xdf)==0 与 fall 同值返 0 = 照抄（P171）。
    // [G5] 去 goto 标签 + 改正向 `<=10` 包裹形：4.4 对 goto 目标标签恒插 1B nop 填充
    //   （CheckMoney 探针 nl11_probe1/2 实证；if/else return 形无 nop），且 goto 到
    //   zero 会多出一道蹦床块 ⇒ 原 3×nop + 1×jmp（+4 条）全消；ja/je 仍共享尾 return
    //   false（jcc 方向与块序与 ORIG 一致）。
    // 残差（N 封顶）：ORIG `mov 0x8cfc4(%eax),%eax` 单读跨 ja 复用为 shl 计数
    //   （4.1 register 缓存免中转），4.4 恒二读；`register unsigned int st=m_chState`
    //   两形均反劣（st 落 %ebx：+mov %eax,%ebx/+mov %ebx,%eax）⇒ N7/N18a；掩码段
    //   ebx→eax→edx 双中转同源（N18b 族）。
    if ((unsigned)m_chState <= 10)
    {
        bit = 1 << (int)m_chState;
        if ((bit & 0x720) != 0)
            return true;
        if ((bit & 0xdf) == 0)
            return false;
    }
    return false;
}

#include "CUser/set_before_area.inc"
{
    m_field8cfb8 = area;
}

#include "CUser/set_area.inc"
{
    int frame_pad; (void)frame_pad;   // ORIG 0x8647867 sub $0x18：2 outgoing 槽 + 16B 帧舍入（4.1 特征），dead 局部无指令
    set_before_area(m_field8cfbc);
    m_field8cfbc = area;
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086813be | _ZN5CUser8get_areaEb
// [b28d-GrpD 判词·无实差异] ORIG 0x086813be 逐条对照：除 `.o` 未解析重定位
//   （我方 `call 7c19 <本函数+0x13>` vs ORIG `call 80da3a7 <_Z11G_GameWorldv>`）
//   与 je 目标地址文本外，助记符+操作数序**逐条一致**（norm 后两侧序列相等）
//   ⇒ 非可改项；Main 做 link 后重定位解析即应转 AE/IDENTICAL。
#include "CUser/get_area.inc"
{
    if (param) {
        if (G_GameWorld()->IsSchoolPvPChannel() &&
            getCurCharacVill() == 2 &&
            m_field8cfc0 == 4) {
            return m_field8cfc0;
        }
    }
    if (param && getCurCharacVill() == 8) {
        if (m_field8cfc0 == 0 || m_field8cfc0 == 1 ||
            m_field8cfc0 == 2 || m_field8cfc0 == 3) {
            return m_field8cfc0;
        }
    }
    return m_field8cfbc;
}

#include "CUser/get_pvp_WinningRate_relateMission.inc"
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return mission->getWinningRate();
}

#include "CUser/get_aura_avatar_option_value.inc"
{
    // ORIG：判空 = 4.1 bool 物化 `test eax;setne al;test;je`（技巧 22 形，指针
    // 结果免槽）；界检查单条无符号 `cmp $2; jg`（(unsigned)idx>2 吸收负数）；
    // 取值**二次** getCurCharacR call（结果直用 `add $0x4a0; mov 0x1(%eax,%edx,4)`
    // scaled 单级，scaled 分解残差记 N）。
    // b28 重构（P0/N96 去绑定）：界检查 `>=0 && <=2` 正向包裹 ⇒ 4.4 产
    // `cmpl $0,mem; js BAD; cmpl $2,mem; jg BAD` 双跳共享尾 -1 块（P222 族）。
    if (getCurCharacR() == 0)
        return -1;
    if (idx >= 0 && idx <= 2)
        return getCurCharacR()->m_auraValues[idx];
    return -1;
}

#include "CUser/SetTradeSpace.inc"
{
    lock();
    m_tradeSpaceIdx = (short)idx;
    unlock();
}

#include "CUser/CheckFatigue.inc"
{
    if (m_accId == 0)
        return true;
    return getCurCharacTotalFatigue() < getCurCharacTotalMaxFatigue();
}

// ORIG T 0x0864f7fe sz0xeb：扣除 1 点疲劳（先消耗高级会员疲劳额度）。
//   min(premiumFatigue, maxPremiumFatigue) → 不足时回落到普通疲劳。
// b28d-lane5 N封顶: N97/P230 cmov; N82/P112 槽号分配 | ORIG 0x0864f7fe | _ZN5CUser20gainCurCharacFatigueEi
#include "CUser/gainCurCharacFatigue.inc"
{
    // b28d-G3C 2026-09-10（P211 类型同型化 + P183/P59 形态）：
    // ① ORIG 0x0864f847/0x0864f84e `lea -0x1c(%ebp),%eax / lea -0x18(%ebp),%eax`
    //    ——两个具名槽的**地址直接**喂 std::min<int>(int const&, int const&) ⇒
    //    原码 maxPrefatigue/prefatigue 为 **int**。我方 unsigned 形须为 const int&
    //    造两个临时（多 -0x28/-0x24 槽 + 2 条 `mov %eax,-0x2c/-0x28` 拷贝）。
    // ② ORIG 0x0864f873 `sub %eax,-0x14(%ebp)`（mem 目标）⇒ 原码 `premiumVal -= value;`
    //    （我方原写 `premiumVal - (unsigned int)value` 产 16 位 `sub %ax,%cx`）。
    // ③ ORIG 0x0864f8ba `mov -0x10(%ebp),%eax; mov -0xc(%ebp),%edx;
    //    lea (%edx,%eax,1),%eax; test %eax,%eax; jns` ⇒ normal 侧**无 sum 具名槽**、
    //    if/else 两分支（我方原三目形产 `sar $0x1f; not; and` 位运算夹取 + 独有槽）。
    // 2026-09-11 L9GrpF（P-L11d/P212 声明序）：ORIG 0x0864f7fe 帧内
    //   `movl $0x0,-0x1c(%ebp)`（maxPrefatigue@-0x1c、prefatigue@-0x18），我方对调。
    //   4.4 -O0 槽向 = 先声明→浅槽 ⇒ 把 prefatigue 声明提前（初始化语句仍留在原处）。
    //   探针 /tmp/L9/F/pf3.cpp f0(现形)/f1(本形)：f1 与 ORIG 逐条同形（movl $0,-0x1c /
    //   mov %eax,-0x1c / mov %eax,-0x18 / lea -0x1c→4(%esp) / lea -0x18→(%esp)）。
    int prefatigue;
    int maxPrefatigue = 0;
    if (m_premium.GetAdvantageFatigueRate() != 0)
        maxPrefatigue = getCurCharacMaxPremiumFatigue() & 0xffff;

    prefatigue = getCurCharacPremiumFatigue() & 0xffff;
    // ORIG 0x864f854 调用 std::min<int>(int const&, int const&)（外部函数调用，
    // 非三元/内联）——写显式 std::min<int> 以产出同符号调用点。
    int premiumVal = std::min<int>(prefatigue, maxPrefatigue);
    int remain = (int)premiumVal - value;
    // b28d-GrpD：ORIG 0x0864f871 `js 0x864f890` ⇒ 原码条件为 `remain >= 0`（正块
    //   fall-through、负块物理后置）。探针 /tmp/CuD/probe/p5.cpp f1/f2 实证：
    //   `>= 0` 产 js+正块在前，`< 0` 产 jns+负块在前 ⇒ 按 ORIG jcc 方向与块序改写。
    if (remain >= 0)
    {
        premiumVal -= value;
        setCurCharacPremiumFatigue((unsigned short)premiumVal);
    }
    else
    {
        if ((int)premiumVal > 0)
            setCurCharacPremiumFatigue(0);
        int normal = getCurCharacFatigue() & 0xffff;
        if (normal + remain < 0)
            normal = 0;
        else
            normal = normal + remain;
        setCurCharacFatigue(normal);
    }
}

#include "CUser/has_within_Mission.inc"
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    // ORIG 0x8662739 call 后直 leave;ret（无归一），调用点 0x865d433 族 `test %ax,%ax`（16 位判）⇒ short 直返
    return mission->getIndex_byKind(0x1b);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086552e6 | _ZN5CUser13getBlueMarbleEv
#include "CUser/getBlueMarble.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（21 vs 22 唯一差）：ORIG 0x0865531c `js` 目标的
    //   失败块为 `mov $0x0,%eax`（无 nop），我方在同址多一条**布局 nop**
    //   （`jmp <epi>` 后、`mov $0x0,%eax` 前）——gcc `-falign-jumps` 产物，源码层
    //   不可控（recover_stamina 呈镜像：ORIG 有 nop 而我方无）⇒ §6.1 对齐豁免。
    // ORIG 0x8??：无 idx 槽——`movzwl mem,%eax; test %ax,%ax; js`（ushort 零装载
    // + js 符号判、body fall-through）+ 二次装载 `movswl %ax,%ebx`。关键：short→int
    // 的拓宽必须**作为普通实参直传**（隐式转换），4.4 才会在 %ax 上直接出 2 操作数
    // movswl 并入 callee-saved %ebx（跨 G_CGameManaging 调用存活）；若写成
    // `register int idx asm("ebx")` 中间变量则 4.4 退化成 cwtl+mov（probe s3 实证）。
    // 尾 `call;jmp merge` + `mov $0` 尾岛（goto 钉块序，技巧 23）。
    if (m_field8d00c < 0)
        goto RET0;
    return G_CGameManager()->getBlueMarble(m_field8d00c);
RET0:
    return 0;
}

#include "CUser/checkLogOutCorrectly.inc"
{
    m_guildAgitDB.m_pad2[0] = 1;   // +0x8d0df（ORIG 单指令 movb 写；组件视图折叠）
}

#include "CUser/find_pvp_masterid_walkingout_me.inc"
{
    if (m_set8cfe4.empty())
        return false;
    // ORIG 0x865cf32 `test;je → mov $1 尾岛`：eq + 命中返常量 1（技巧 17 fall 块在前 +
    // 技巧 30 尾岛）；`return *it` 反产 pt/deref 块。
    std::set<unsigned int>::iterator it = m_set8cfe4.find(id);
    if (it == m_set8cfe4.end())
        return false;
    return true;
}

#include "CUser/insert_pvp_masterid_walkingout_me.inc"
{
    m_set8cfe4.insert(id);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865cfac | _ZN5CUser20update_old_pvp_pointEv
#include "CUser/update_old_pvp_point.inc"
{
    // ORIG 0x8??：writable `%mov eax,%ebx` 单条跨 call 存活；P0/N96 去绑定改
    // 链式单语句（LHS receiver 跨 RHS call 保活，P52），残差记 N18a。
    // b28d-solo-CUser p167 修正（调用目标）：ORIG 0x0865cfac 调
    //   getPVPResultRefW/getPVPResultRefR（非 W/R 变体）；两者实现等价但符号不同。
    getPVPResultRefW()->m_oldExpPoint = getPVPResultRefR()->m_expPoint;
}

#include "CUser/SetSaveRentalInfoToExchange.inc"
{
    m_rentalFlag1 = flag;
}

#include "CUser/DeleteRentalItemInfo.inc"
{
    m_rentalInfo[idx].clear();
}


#include "CUser/isAffectedPremium.inc"
{
    return m_premium.CheckPremium((int)type);
}

#include "CUser/IsPermissionPrivateStore.inc"
{
    return isAffectedPremium((ENUM_PREMIUM_TYPE)8);
}

// CMaxFatigueFactorEvent：ORIG 仅 GetMaxFatigueFactor（W 0x0812ce54）被消费，
// 权威类未建模；本文件以最小类声明 + reinterpret_cast 视图消费（GetRepeatEvent(2)
// 返回的 CEventBase* 实际指向该事件对象）。
class CMaxFatigueFactorEvent
{
public:
    int GetMaxFatigueFactor();   // ORIG W _ZN22CMaxFatigueFactorEvent19GetMaxFatigueFactorEv
};

// 0x08657ada
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08657ada | _ZN5CUser14RecoverFatigueEi
// 2026-09-11 CuA 复核（**勿按「形参互换」改**）：ORIG 0x8657b35..0x8657b5a 的
//   makeRequest(get_acc_id, getCurCharacSchoolPoint, GetUID) 段，ORIG 把 arg3(GetUID) 放
//   %ebx、arg2 放 %esi；我方 %ebx/%esi 恰好对调 —— 但**两侧求值序同为右→左**
//   （arg3 先算），esi/ebx 内容同步互换、写入 0x8/0x4(%esp) 的槽位不变 ⇒
//   语义等价，仅 4.1↔4.4 寄存器选择差（N7）。改源码反而会真错位。
#include "CUser/RecoverFatigue.inc"
{
    // ORIG 0x08657ada：角色为空直接返回（跳过全部）；否则先写回
    // premium 疲劳（ushort 截断）与当前疲劳
    if (getCurCharacR() == 0)
        return;
    setCurCharacPremiumFatigue((unsigned short)value);
    setCurCharacFatigue(value);

    // ORIG 0x08657ada：学校点非零 → DB 上报并清零
    if (getCurCharacSchoolPoint() != 0)
    {
        DB_UpdateSchoolPoint::makeRequest(get_acc_id(),
                                          getCurCharacSchoolPoint(), GetUID());
        resetCurCharacSchoolPoint();
    }

    // ORIG：事件 2（CMaxFatigueFactorEvent）活跃 → m_maxFatigue = factor*DEFAULT/100，
    // 否则 DEFAULT_MAX_FATIGUE。
    CEventBase* ev2 = GlobalData::s_event_manager->GetRepeatEvent(2);
    _Charac_info* cur = getCurCharacW();
    if (cur != 0)
    {
        if (ev2 != 0)
        {
            if (ev2->IsEventing((CUser*)0))
            {
                cur->m_field35 =
                    (unsigned short)(
                        reinterpret_cast<CMaxFatigueFactorEvent*>(ev2)
                            ->GetMaxFatigueFactor()
                        * DEFAULT_MAX_FATIGUE / 100);
            }
            else
            {
                cur->m_field35 = (unsigned short)DEFAULT_MAX_FATIGUE;
            }
        }
        if (IsHavePremiumAdvantage())
        {
            if (ev2 != 0 && ev2->IsEventing((CUser*)0))
            {
                // base = (short)(factor*DEFAULT/100) - (short)DEFAULT
                short base = (short)(
                        reinterpret_cast<CMaxFatigueFactorEvent*>(ev2)
                            ->GetMaxFatigueFactor()
                        * DEFAULT_MAX_FATIGUE / 100)
                           - (short)DEFAULT_MAX_FATIGUE;
                setCurCharacMaxPremiumFatigue(
                    (unsigned short)(
                        // b28d-solo-CUser p167：ORIG 0x08657ada 直接
                        // `call CUserPremium::GetAdvantageFatigueRate(this+0x7121c)`（7 次），
                        // 无 GetPremiumInfoW() 中间调用（我方 EXTRA ×7）⇒ 改具名成员访问。
                        (unsigned short)m_premium.GetAdvantageFatigueRate()
                            * DEFAULT_MAX_FATIGUE / 100
                      + (unsigned short)base
                            * (unsigned short)m_premium.GetAdvantageFatigueRate()
                            / 100));
            }
            else
            {
                setCurCharacMaxPremiumFatigue(
                    (unsigned short)(
                        (unsigned short)getCurCharacMaxFatigue()
                            * (unsigned short)m_premium.GetAdvantageFatigueRate()
                            / 100));
            }
        }
    }

    // ORIG：事件 0x57 活跃（ORIG 无空检查）→ m_maxFatigue += DataManager 0x51c；
    // rate!=0 且 PC 房 → premium = rate*DEFAULT/100 + 0x51c，否则 rate*DEFAULT/100。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing((CUser*)0))
    {
        _Charac_info* curW = getCurCharacW();
        curW->m_field35 = (unsigned short)(
            // dm+0x51c = SPS(+0x68)+0x4b4 = m_weekendBouns4b4.m_bonusFatiguePercent
            // [L9-C组J PATCH J2] ORIG `movzwl 0x51c(%eax),%eax` 单条 disp；取址+解引用在 4.4
            //   产 `add $0x51c,%eax; movzwl (%eax),%eax`（多 1 条）⇒ 直接值成员访问
            //   （字段本为 unsigned short，ServerParameterScript.h:302，读值不变）。
            curW->m_field35
            + G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                  .m_bonusFatiguePercent);
        if (m_premium.GetAdvantageFatigueRate() != 0
            && GetPCRoomNo() != 0)
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)m_premium.GetAdvantageFatigueRate()
                        * DEFAULT_MAX_FATIGUE / 100
                  + G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                        .m_bonusFatiguePercent));
        }
        else
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)m_premium.GetAdvantageFatigueRate()
                        * DEFAULT_MAX_FATIGUE / 100));
        }
    }
    else
    {
        if (IsHavePremiumAdvantage())
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)getCurCharacMaxFatigue()
                        * (unsigned short)m_premium.GetAdvantageFatigueRate()
                        / 100));
        }
    }
    resetCurCharacUsedFatigue();
}

#include "CUser/LogHistory.inc"
{
    // ORIG 0x0867c8d8：日志前缀（Hangame 服 'X' 否则 'D'）+ 固定头（账号/角色/
    // 时间/成长/职业/等级/IP/端口/频道）+ 变参 fmt 追加，经 cHistoryTrace 输出。
    // 布局对齐（ORIG 0x867c8fe 帧 0x4bc）：typeFlag@-0x19、history@-0x1a、
    // timeLog@-0x22、accIdBuf[24]@-0x3a、ap@-0x40、buf[1024]@-0x440，
    // sprintf 实参右→左求值，temp 槽自 -0x470 起。
    char typeFlag;
    if (isHangameUser())
        typeFlag = 'X';
    else
        typeFlag = 'D';
    cHistoryTrace history;
    TimeLog_ timeLog;
    timeLog.setTime();
    char accIdBuf[24];
    memset(accIdBuf, 0, 4);
    strcpy(accIdBuf, NumberToString(get_acc_id(), 7));
    va_list ap;
    char buf[1024];
    sprintf(buf,
        "\"%s\",%s,%c,%02d%02d%02d,\"%s\",%d,%d,%d,%d,%s,%s,%d,%d, ",
        m_accName, NumberToString(get_acc_id(), 0), typeFlag,
        (int)(char)(unsigned char)timeLog.m_hour,
        (int)(char)(unsigned char)timeLog.m_min,
        (int)(char)(unsigned char)timeLog.m_sec,
        getCurCharacR() ? getCurCharacName() : "",
        getCurCharacNo(), get_charac_level(), get_charac_job(),
        (int)getCurCharFirstGrowType(),
        getWebAddress(),
        m_network.GetPeerIP2(reinterpret_cast<char*>(this) + 0x6eef0, 0x10),
        (int)m_network.get_port(),
        G_CEnvironment()->m_serverEnvir.m_channelNo);
    va_start(ap, fmt);
    vsprintf(buf + strlen(buf), fmt, ap);
    va_end(ap);
    history(buf);
}

#include "CUser/isBlackUser.inc"
{
    if (m_vec8cef4.empty())
        return false;
    for (std::vector<unsigned int>::iterator it = m_vec8cef4.begin();
         it != m_vec8cef4.end(); ++it) {
        if (*it == accId)
            return true;
    }
    return false;
}

#include "CUser/IsHavePremiumAdvantage.inc"
{
    return m_premium.GetAdvPremiumCount() != 0;
}

// b28d-solo-CUser AE: ORIG 0x0867?? `call; test %eax,%eax; setne %al; test %al,%al; je`
//   = 条件内联形（探针 /tmp/probe_bool.cpp a3/a5 逐条同形）；具名 bool 局部 4.4 恒
//   落槽（a2/a4：mov %al,-0x9; cmpb $0,-0x9）⇒ 删局部内联即归位。| _ZN5CUser13IsEquipAvatarEv
#include "CUser/IsEquipAvatar.inc"
{
    if (getCurCharacR() != 0)
        return ((const CInventory*)getCurCharacInvenR())->IsEquipAvatar();
    return false;
}

#include "CUser/getWebAddress.inc"
{
    return m_gameOption.m_webAddress;
}

#include "CUser/getSecurityCard.inc"
{
    return &m_securityCard;
}

#include "CUser/GetGameOptionRef.inc"
{
    return &m_gameOption;
}

#include "CUser/getPad.inc"
{
    return &m_cpad;
}

#include "CUser/getBeforeAreaTime.inc"
{
    return m_field79670;
}

#include "CUser/setBeforeAreaTime.inc"
{
    m_field79670 = t;
}

#include "CUser/getLastPacketID.inc"
{
    return m_lastPacketID;
}

#include "CUser/setLastPacketID.inc"
{
    m_lastPacketID = id;
}

#include "CUser/getPlayingDungeonIndex.inc"
{
    return m_playingDungeonIdx;
}

#include "CUser/setPlayingDungeonIndex.inc"
{
    m_playingDungeonIdx = index;
}

#include "CUser/getMoveSpace.inc"
{
    return (UserSpace::T)m_field8cfcc;
}

#include "CUser/setMoveSpace.inc"
{
    m_field8cfcc = space;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08678526 | _ZN5CUser13CheckMoveTownEi
// 2026-09-11 CuA 探针终判（/tmp/P5/p_cmp probe_cmp{,2}.cpp）：4.4 对 **(int 参数 + `(unsigned)` cast)**
//   恒产 `mov 0xc(%ebp),%eax; cmp $0xa,%eax`；折成 ORIG 的 `cmpl $0xa,0xc(%ebp)` 只在
//   **形参自身为 unsigned**（`_Z1bj` 探针）或 e1..e5 五种等价写法全不改 —— 而本件 mangled
//   是 `…13CheckMoveTownEi`（int）⇒ 不可造。判 N，勿再试。
#include "CUser/CheckMoveTown.inc"
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    // ORIG 0x8678548 `cmpl $0xa,0xc(%ebp); ja`：4.1 对 arg 槽直折无符号比较；
    // 4.4 恒先载 %eax（具名 uv 槽双 spill 更差，probe），保留 cast 形 ndiff=1。
    if (static_cast<unsigned int>(vill) <= 10)   // ORIG: ja 无符号比较
    {
        // N6 消解：直写表达式 `(1 << vill) & 0x580` 被 4.4 前端折成 `sar+and$1`
        // （probe p17.f2）；改具名 bit 局部保 shl 链。裸 `register` 提示（非
        // asm("reg") 绑定，P0/N96 合规）令 4.4 免槽回写，复现 ORIG
        // `mov $1; shl %cl; mov %ebx,%eax; and $0x580` 全寄存器形（同 is_fighting 先例）。
        register int bit;
        bit = 1 << vill;
        if (bit & 0x580)
            return 0;
    }
    	Village* village = G_GameWorld()->GetVillage(vill);
	if (!village)
		return 8;
	if (get_charac_level() < village->m_field2c)
		return 8;
	if (village->m_questIdx != 0)
	{
		if (!getCurCharacQuestR()->isClearQuest(village->m_questIdx))
			return 7;
	}
	return 0;
}

#include "CUser/isUseCraneStart.inc"
{
    return m_field8eb98;
}

#include "CUser/setUseCraneStart.inc"
{
    m_field8eb98 = flag;
}

#include "CUser/getForceDropFlag.inc"
{
    return m_gameOption.m_forceDropFlag;
}

#include "CUser/setForceDropFlag.inc"
{
    m_gameOption.m_forceDropFlag = flag;
}

#include "CUser/isDungeonClearHackCheck.inc"
{
    if (m_mcap.m_field54c == 3)
        return 1;
    return 0;
}

#include "CUser/setDungeonClearHackCheck.inc"
{
    if (flag) {
        // b28d-solo-CUser AE: ORIG 0x8649bab-0x8649bb4 `movzbl 0x8e93c; mov %eax,%edx;
        //   or $0x1,%edx`（无 cur 槽，sub $0x4）；具名 char cur 落 -0x2 多两段 ⇒ 内联。
        m_mcap.m_field54c = (m_mcap.m_field54c | 1);
    } else {
        m_mcap.m_field54c = (m_mcap.m_field54c | 2);   // ORIG 0x8649bc5-0x8649bce or $0x2,%edx 同形
    }
}

#include "CUser/isAllowLocationGeoState.inc"
{
    return m_rentalFlag3;
}

#include "CUser/setAllowLocationGeoState.inc"
{
    m_rentalFlag3 = flag;
}

#include "CUser/isComboSkillSet.inc"
{
    return m_field8ec29;
}

#include "CUser/setComboSkillSet.inc"
{
    m_field8ec29 = flag;
}

#include "CUser/lock4DataLoad.inc"
{
    // ORIG 0x864a9ac：test %al,%al; je WRITE; mov $0; jmp merge; WRITE: movb $1,0x8d0f0; mov $1
    //（bool 返回 if() 直判；isLocked4DataLoad 定义置于调用点之后以松帧，CDMSB 技巧）
    if (isLocked4DataLoad())
        return 0;
    m_field8d0f0 = 1;
    return 1;
}

#include "CUser/unlock4DataLoad.inc"
{
    // ORIG 0x864a9dd：xor $0x1,%eax; test; je WRITE（!bool 取反形）; WRITE: movb $0; mov $1
    if (!isLocked4DataLoad())
        return 0;
    m_field8d0f0 = 0;
    return 1;
}

#include "CUser/lockCreateCharac.inc"
{
    if (isLockedCreateCharac())   // ORIG 0x864aa88 test %al;je 同 lock4DataLoad
        return 0;
    m_field8d0f2 = 1;
    return 1;
}

#include "CUser/unlockCreateCharac.inc"
{
    if (!isLockedCreateCharac())  // ORIG 0x864aab6 xor$1;test;je 族
        return 0;
    m_field8d0f2 = 0;
    return 1;
}

#include "CUser/lockDeleteCharac.inc"
{
    if (isLockedDeleteCharac())   // ORIG 0x864aa1a test %al;je 同族
        return 0;
    m_field8d0f1 = 1;
    return 1;
}

#include "CUser/unlockDeleteCharac.inc"
{
    if (!isLockedDeleteCharac())  // ORIG 0x864aa48 xor$1;test;je 同族
        return 0;
    m_field8d0f1 = 0;
    return 1;
}

#include "CUser/isLocked4DataLoad.inc"
{
    return m_field8d0f0;   // ORIG 0x864aa0a movzbl 直返（bool 字段兼容）
}

#include "CUser/isLockedCreateCharac.inc"
{
    return m_field8d0f2;   // ORIG 0x864aae6 movzbl 0x8d0f2 直返
}

#include "CUser/isLockedDeleteCharac.inc"
{
    return m_field8d0f1;   // ORIG 0x864aa78 movzbl 0x8d0f1 直返
}

#include "CUser/get_growth_power_exp_reward.inc"
{
    // ORIG 把 CUser+0x8d24d 的 dword 直接写入 sret（无 STExpReward ctor，9 insns）。
    // 真值成员 return 直写 = 4.4 唯一复现形（probe g5/g6 实证；cast/引用/临时均 memcpy）。
    return m_reward;
}

#include "CUser/setAntibotKey.inc"
{
    m_antibotKey = key;
}

#include "CUser/setClientVersion.inc"
{
    m_clientVersion = version;
}

#include "CUser/getClientVersion.inc"
{
    return m_clientVersion;
}

#include "CUser/ResetUsedCoinInDungeon.inc"
{
    m_field8eae0 = 0;
}

#include "CUser/ClearItemSellCount.inc"
{
    m_itemSellCount = 0;
    memset(&m_sellCountData, 0, 0x1c);
}

// ---- 发包族（PacketGuard 局部 + InterfacePacketBuf） ----
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086485ba | _ZN5CUser4SendER11PacketGuard
#include "CUser/Send.inc"
                                        // 调用点 0x082e6c2b `xor$1;test%al` bool callee 直判
{
    // 2026-09-10 gA（A 组）P0/P87/P183 三轴：
    //  · ORIG 0x086485da `test %al,%al; je 086486ab`（**无 xor$1**，成功块 fall-through、
    //    失败块在 0x086486ab）⇒ 源码是正向 if/else，非 `if (!f())`（P168a③）。
    //  · 失败块 ORIG 0x086486be `movzwl (%eax),%eax; movzwl %ax,%ebx`（packetId 无槽、
    //    跨 getCurCharacName 保活 %ebx）、name 直留 %eax ⇒ 删具名 packetId/name。
    //  · 成功块 ORIG 0x0864861e packetId 保活 %ebx、0x08648634 `movzbl %al,%eax`（area
    //    直传）、0x08648658 `cmp $0x6,%al; setne %al; test %al,%al; je`（b 直判）
    //    ⇒ 删具名 area/b，全部实参内联。
    PacketBuf* pBuf = ((InterfacePacketBuf*)&packet)->get();
    if (pBuf->is_finallized())
    {
        // ORIG 0x086485e2/0x086485f5：两个死 PacketBuf::at() 直调（结果丢弃，P184 保留）
        pBuf->at(0);
        pBuf->at(1);
        GetPacketName((ENUM_PACKETCLASS)pBuf->at(0),
                      *((unsigned short*)pBuf->get_packet(1)));
        if (pBuf->at(1) != 6)
        {
            // ORIG 块内 at(1)/at(0) 各重调一次，非复用上面的值
            m_pad8cf0c[0] = pBuf->at(1);
            m_pad8cf0c[1] = pBuf->at(0);
        }
        return AntibotSend(*pBuf);
    }
    LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x8a0,
                          "User %s - Packet.is_finallized packet_id(%d) return false",
                          getCurCharacName(),
                          (int)*((unsigned short*)pBuf->get_packet(1)));
    return 0;
}

// [L9-GrpE 2026-09-11 探针翻正·P305] ORIG 0x0867b902..0x0867b927 的
//   `mov 0xc(%ebp),%eax; cmp $1,%eax; je <1体>; cmp $1,%eax; jg <2/3>; test %eax,%eax;
//    je <0体>; jmp <默认>; cmp $2,%eax; je <2体>; cmp $3,%eax; je <3体>; jmp <默认>`
//   是 4.4 O0 对 `switch((int)x)`（case 0/1/2/3）的**决策树** lowering（非跳表）；
//   case 体物理序 = 书写序 0,1,2,3。
//   同口径探针 /tmp/L9/E/p_sp2.cpp（4.4.6 + -march=i486 -mtune=generic）：
//   FormB2=`switch((int)target)` 与 FormB3=`switch(int)` 均逐条复现上述链；
//   FormB 裸 `switch(target)`（窄枚举 eSendTarget 只定义 {0,1}）被 -fstrict-enums
//   把 case 2/3 静默删除 ⇒ **必须 `switch((int)target)`**（同 SendNotiPacket P305 修法）。
//   旧负结论「switch 恒产跳表 66→24」不再成立（该读数与本次探针不符，已作废）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867b8fe | _ZN5CUser10SendPacketENS_11eSendTargetER11PacketGuard
#include "CUser/SendPacket.inc"
{
    switch ((int)target)
    {
    case 0:
        G_GameWorld()->send_all(packet, this);
        break;
    case 1:
        Send(packet);
        break;
    case 2:
    {
        CParty* pParty = (CParty*)GetParty();
        // 2026-09-11 N8CUser P214①/P222 正条件：ORIG 0x0867b9xx 决策树 case2 段
        //   `cmpl $0x0,-0x10(%ebp); je <this 分支>` ⇒ fall-through 是 pParty!=0 分支
        //   （receiver=槽 %ebp-0x10），我方旧形 `if (pParty==0) Send(...) else ...`
        //   产 `jne` + fall-through=this 分支 ⇒ 条件与块序双反。交换两分支体。
        if (pParty != 0)
            pParty->send_to_party(packet);
        else
            Send(packet);
    }
    break;
    case 3:
    {
        WarRoom* pRoom = (WarRoom*)GetWarRoom();
        // 同上（ORIG case3 段同形：je 跳 this 分支、pRoom!=0 块 fall-through）。
        if (pRoom != 0)
            pRoom->SendToRoom(packet);
        else
            Send(packet);
    }
    break;
    }
    return 1;
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c1d4 | _ZN5CUser13SendCmdPacketENS_11eSendTargetE14ENUM_CMDPACKETi
#include "CUser/SendCmdPacket.inc"
{
    // ORIG 0x867c1d4 块序（技巧 23 goto 钉）：true 路径 put_byte×2 后 **jmp 到位于
    // false(log) 块之后的共享 finalize+SendPacket 段（0x867c279）**；false 块
    // cmd→%ebx 缓存、logFormat、%ebx=0 汇合 0x867c2c4 dtor 尾。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    if (static_cast<unsigned int>(cmd) != 0x41)
        // P0/N96 去绑定；ORIG `mov 0x10,%eax; cmp $0x41,%eax` reg cmp——
        // P209：4.4 对 unsigned 比较恒先载寄存器（signed enum 直折 mem cmp 反形）。
        goto BAD;
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_byte(param);
    goto DO_FIN;
BAD:
    LogManager::logFormat(1, "user.cpp",
                          __PRETTY_FUNCTION__,
                          0x7741, "User %s - CmdID %d invalid", getCurCharacName(),
                          (int)cmd);
    return 0;
DO_FIN:
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return SendPacket(target, packet);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bea0 | _ZN5CUser15SendCmdOkPacketE14ENUM_CMDPACKET
#include "CUser/SendCmdOkPacket.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

#include "CUser/SendCmdErrorPacket.inc"
{
    PacketGuard packet;
    return SendCmdErrorPacket(cmd, error, packet);
}

// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步（ORIG 尾 call Send 直返）| ORIG 0x0867c022 | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKETh8PAY_TYPE
#include "CUser/SendCmdErrorPacket_1.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->put_byte((int)pay);   // ORIG 写 1 字节（put_int 会破坏包格式）
    ((InterfacePacketBuf*)&packet)->finalize(true);
    // ORIG 0x867c0a0-0x867c0c8：finalize 后 cMyTraceNoop()("CmdID %d, nErrorCode %d", cmd, error)
    // （format @0x8cefc8e；禁追踪 noop 空对象，临时 @-0x9）。1st=CmdID(cmd) 2nd=nErrorCode(error)。
    cMyTraceNoop()("CmdID %d, nErrorCode %d", (int)cmd, (int)error);
    return Send(packet);
}

// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步 | ORIG 0x0867bfae | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKEThR11PacketGuard
#include "CUser/SendCmdErrorPacket_2.inc"
{
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c110 | _ZN5CUser23SendCmdErrorPacket2ByteE14ENUM_CMDPACKETt
#include "CUser/SendCmdErrorPacket2Byte.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x08692824 | _ZN5CUser21SendCmdErrorPacketJpnE14ENUM_CMDPACKETt
#include "CUser/SendCmdErrorPacketJpn.inc"
{
    PacketGuard packet;
    ARAD::DISPATCHER::make_cmd_packetheader_jpn(packet, cmd, false);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

// ORIG T 0x08680a96：拍卖操作前置门（AuctionPreCheck.md）。
// 控制流：
//   1) pay==1（cera 拍卖）且 CCeraAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   2) pay==0（普通拍卖）且 CAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   3) get_state()<3 或 getCurCharacR()==0 → 返回 0x8151；
//   4) CSecu_ProtectionField::Check(this, 0x35) !=0 → 报对应错误，返回 0；
//   5) 全部通过 → 返回 -1。
// b28d-lane5 N封顶: N96+P168a④ 清 `register bool fail asm("al")` 后 bool 暂存落栈槽
// （movb $1,-0xd / cmpb $0,-0xd），ORIG 为寄存器 `mov $1,%eax`；无绑定即不可达（P0 硬约束）。
// | ORIG 0x08680a96 | _ZN5CUser15AuctionPreCheckE14ENUM_CMDPACKET8PAY_TYPE
#include "CUser/AuctionPreCheck.inc"
{
    // b28d-solo-CUser 重写（2026-09-09，P226/P214①）：ORIG 无 fail 具名局部——两段门
    //   均为「pay 值比较 && !IsRunning()」**直接 if 短路**（4.1 物化 `mov $1,%eax;
    //   jmp; mov $0,%eax; test %al,%al; je` 到寄存器，0 槽往返）；旧 `bool fail`+goto
    //   SET01/SET02 结构被 4.4 落槽（movb $1,-0xd / cmpb $0,-0xd）。裸 if 形即复现。
    if (pay == (PAY_TYPE)1 && !GlobalData::s_cera_auction_proxy->IsRunning())
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (pay == (PAY_TYPE)0 && !GlobalData::s_auction_proxy->IsRunning())
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (get_state() <= 2 || getCurCharacR() == 0)
    {
        return 0x8151;
    }

    int err =
        GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x35);
    if (err != 0)
    {
        SendCmdErrorPacket(cmd, (unsigned char)err, pay);
        return 0;
    }

    return -1;
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bd84 | _ZN5CUser22SendCmdPeerErrorPacketE14ENUM_CMDPACKET22ENUM_PEER_REQUEST_TYPEh
#include "CUser/SendCmdPeerErrorPacket.inc"
                                   unsigned char error)  // b28d-lane5：ORIG 0x0867be6f 保存 4 参版返回值
{
    PacketGuard packet;
    return SendCmdPeerErrorPacket(cmd, type, error, packet);
}

#include "CUser/SendCmdPeerErrorPacket_1.inc"
                                   unsigned char error, PacketGuard& packet)  // b28d-lane5
{
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error & 0xff);
    ((InterfacePacketBuf*)&packet)->put_byte((int)type);
    if ((int)type == 4)
        ((InterfacePacketBuf*)&packet)->put_int(0);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

#include "CUser/SendZeroExpPacket.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x107);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

#include "CUser/SendMaxEquipLevel.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x1b6);
    ((InterfacePacketBuf*)&packet)->put_short(GetCurCharacMaxEquipLevel());
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

#include "CUser/SendFatigue.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x24);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacTotalFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacTotalMaxFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacUsedFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacFatigueGrownUpBuff() & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// ============================================================================
// G1-4 批次：索引 / 状态访问器（ORIG 逐符号转录）
// ============================================================================

#include "CUser/CheckInTrade.inc"
{
    return (~(unsigned short)m_tradeSpaceIdx & 0x8000) != 0;
}

#include "CUser/isEnableAvatarSocketAction.inc"
{
    // ORIG _ZN5CUser26isEnableAvatarSocketActionEv @ 0x086810a8
    // 检查链：安全保护(0x11) → 私摊忙 → 幽灵角色 → 交易中 → 袭击区 → PVP 频道。
    // 安全保护命中返回其错误码，其余任一命中返回 0x13（禁止），全部通过返回 0（允许）。
    int ret = GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x11);
    if (ret != 0)
        return ret;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
        return 0x13;
    if (IsCurCharacGhost())
        return 0x13;
    if (CheckInTrade())
        return 0x13;
    if (GetAssaultPlace() != 0)
        return 0x13;
    if (G_GameWorld()->IsPVPChannel())
        return 0x13;
    return 0;
}

#include "CUser/CheckInParty.inc"
{
    return (~(unsigned short)m_field8d004 & 0x8000) != 0;
}

#include "CUser/checkInBlueMarble.inc"
{
    return (~(unsigned short)m_field8d00c & 0x8000) != 0;
}

#include "CUser/CheckInWarRoom.inc"
{
    return (~(unsigned short)m_field8d008 & 0x8000) != 0;
}

#include "CUser/checkInDeathTower.inc"
{
    return (~(unsigned short)m_field8d00e & 0x8000) != 0;
}

#include "CUser/checkInBossTower.inc"
{
    return (~(unsigned short)m_bossTowerIdx & 0x8000) != 0;
}

#include "CUser/checkInAdvanceAltar.inc"
{
    return (~(unsigned short)m_advanceAltarIdx & 0x8000) != 0;
}

#include "CUser/getAdvanceAltarIndex.inc"
{
    return (short)(unsigned short)m_advanceAltarIdx;
}

#include "CUser/getBlueMarbleIndex.inc"
{
    return (short)m_field8d00c;
}

#include "CUser/setBlueMarbleIndex.inc"
{
    m_field8d00c = index;
}
#include "CUser/getDeathTowerIndex.inc"
{
    return (short)(unsigned short)m_field8d00e;
}

#include "CUser/setDeathTowerIndex.inc"
{
    m_field8d00e = (unsigned short)idx;
}

#include "CUser/getDeathTower.inc"
{
    // ORIG 0x086552a4：idx<0 => NULL；否则 G_CGameManager()->getDeathTower(idx)
    // ORIG 直读 m_field8d00e（movzwl+test %ax+js 正路径顺序，无局部栈槽）。
    if ((short)(unsigned short)m_field8d00e >= 0)
        return G_CGameManager()->getDeathTower(
            (int)(short)(unsigned short)m_field8d00e);
    return 0;
}

// ---- Redeem_Item::CRedeemItem（GetCharacExpandData(6) 返回，未建模为
// CExpandEquipslot* 槽位；权威声明见 Redeem_Item__CRedeemItem.h，实现于
// Redeem_Item__CRedeemItem.cpp，ORIG 0x085f76f0 / 0x085f71a8）----

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086472c0 | _ZN5CUser14Add_RedeemInfoERK10Inven_Itemib
#include "CUser/Add_RedeemInfo.inc"
{
    // ORIG 0x086472c0：GetCharacExpandData(6)->isAddableFilter(item)，可加则记录历史并 AddRedeemList
    Redeem_Item::CRedeemItem* cdata =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    // b28d-solo-CUser P171：ORIG 0x086472c0 call 后 `mov %eax,-0xc(%ebp)` 直接落槽、
    //   **无判空分支**（我方原形多 `cmpl $0,-0x10; jne; mov $0,%eax; jmp` 段）⇒ 删防御照抄。
    // b28d-solo-CUser p167 修复：ORIG 0x086472c0 调用点 `call; test %al,%al; je`（无 xor、
    //   无 setne）⇒ 正条件包裹（P59/P214①）；EXTRA `Inven_Item::get_add_info()` ×1 ⇒
    //   ORIG 直接读 +0x7 字段（`mov 0x7(%eax),%edx`），改具名成员 m_addInfo2（P235）。
    if (cdata->isAddableFilter(item))
    {
        // b28d-solo-CUser P171/P0：ORIG 0x086472c0 无 pItem 判空、且 find_item 结果
        //   无槽往返（我方原具名 pItem 落 -0xc 多 2 条）⇒ 内联表达式直用。
        if (G_CDataManager()->find_item(item.m_addInfo)->is_stackable())
            m_historyLog.RedeemItemAdd(item.m_addInfo, item.m_addInfo2);
        else
            m_historyLog.RedeemItemAdd(item.m_addInfo, 1);

        return cdata->AddRedeemList(item, count, isRedeem);
    }
    return 0;
}

#include "CUser/getBossTowerIndex.inc"
{
    return (short)(unsigned short)m_bossTowerIdx;
}

#include "CUser/setBossTowerIndex.inc"
{
    m_bossTowerIdx = index;
}

#include "CUser/GetPartyIndex.inc"
{
    return (short)(unsigned short)m_field8d004;
}

#include "CUser/CheckInPvp.inc"
{
    return (~(unsigned short)m_pvpRoomIdx & 0x8000) != 0;
}

#include "CUser/GetPvpIndex.inc"
{
    return (short)(unsigned short)m_pvpRoomIdx;
}

#include "CUser/GetWarRoomIndex.inc"
{
    return (short)(unsigned short)m_field8d008;
}

#include "CUser/GetTradeSpace.inc"
{
    return (short)(unsigned short)m_tradeSpaceIdx;
}

#include "CUser/SetPartyIndex.inc"
{
    lock();
    m_field8d004 = index;
    unlock();
}

#include "CUser/SetWarRoomIndex.inc"
{
    m_field8d008 = index;
}

#include "CUser/SetSlotIDX.inc"
{
    m_slotIDX = idx;
}

#include "CUser/GetIncreID.inc"
{
    // 字段 short（Main 回滚 :2248，混合号性 N 类）——ORIG 0x82a6842 `movzwl` 需
    // (unsigned short) 中间 cast 形（short 直读反产 movswl/符号形）。
    return (short)(unsigned short)m_field704a6;
}

#include "CUser/SetIncreID.inc"
{
    m_field704a6 = id;
}

#include "CUser/set_unique_id.inc"
{
    m_field704a8 = id;
}

#include "CUser/set_position.inc"
{
    m_posX = x;
    m_posY = y;
    m_direction = z;
}

#include "CUser/get_posX.inc"
{
    return m_posX;
}

#include "CUser/get_posY.inc"
{
    return m_posY;
}

#include "CUser/get_direction.inc"
{
    return m_direction;
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x082f0e70 | _ZN5CUser12GetUserStateEv
#include "CUser/GetUserState.inc"
{
    // b28d-PoolCUser 重写（2026-09-09，P157/P168 逐使用点重载）：ORIG 0x082f0e70
    //   每个条件均 `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax; cmp $K,%eax` 全量
    //   重读（Ghidra/asm 同址 8 次 = P48 源码两次独立读），**无 state 槽**（P0）
    //   ⇒ 删具名缓存局部。|| 短路对（5/10、6/7）je 落前置共享岛 + 第二条件
    //   jne 跳过 = 4.4 `a==x || a==y` 自然形。
    // b28d-PoolCUser 二修：ORIG `cmp $3; jg +0x24` 正向落主链、假路（<=3）跳**前置**
    //   assault 岛 + `jmp` 共享尾（P222 判据——早返假路前置岛形）⇒ 改否定早返写法。
    if (m_chState <= 3)
        return CUserCharacInfo::GetUserStateByAssault();
    if (m_chState == 5 || m_chState == 10)
        return 1;
    if (m_chState == 8)
        return 3;
    if (m_chState == 6 || m_chState == 7)
        return 2;
    if (m_chState == 12)
        return 1;
    if (m_chState == 11)
        return 7;
    if (m_chState == 12)
        return 1;
    return 0;
}

#include "CUser/GetAge.inc"
{
    return m_age;
}

#include "CUser/IsOverBirthDay.inc"
{
    // ORIG 0x0867ec66：出生月/日是否已过（用于 PowerWar 最低年龄判定）。
    // ORIG 0x867ec66 形：cur@-0x18 → localtime_r **返回值**落槽 pLocal@-0x14
    // （0x867ec8d mov %eax,-0x14），month/day 各自 reload pLocal（0x867ec90/9c，
    // T-CU14 同款）；birth 字段无具名槽——两处成员直读+展宽直发 cmp。
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    tm* pLocal = localtime_r(&cur, &local);
    int curMonth = pLocal->tm_mon + 1;
    int curDay = pLocal->tm_mday;
    if (m_birthMonth < curMonth)
        return true;
    if (m_birthMonth == curMonth &&
        m_birthDay < curDay)
        return true;
    return false;
}

#include "CUser/IsProgLogout.inc"
{
    return m_progLogout;
}

#include "CUser/GetSeedFromDate.inc"
{
    return m_seedFromDate;
}

#include "CUser/SetGMUpgradeMode.inc"
{
    // ORIG 0x822fea7-0x822fead：mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x711dc(%eax)
    // （this-先 + 无帧 pop ebp）。b28d-solo-CUser AE: P211 同型律——ORIG LHS-first ⇒ 字段须与
    //   形参 enum 同型；CUser.h:908 已由 Main 代落 `unsigned int`→`ENUM_GM_ITEM_UPGRADE`
    //   （探针 /tmp/probe_store.cpp：uint 字段+enum 形参 ⇒ RHS-first；enum 字段 ⇒ LHS-first）。
    //   sizeof/偏移(0x711dc)/mangled 不变。
    m_gmUpgradeMode = mode;
}

#include "CUser/isGMUser.inc"
{
    return m_gmUserFlag;
}

#include "CUser/IsGameMasterMode.inc"
{
    return m_gmUserFlag;
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086796dc | _ZN5CUser17SetGameMasterModeEb
#include "CUser/SetGameMasterMode.inc"
{
    m_gmUserFlag = flag;
    Packet_Register_GM_MID pkt;
    if (flag)
    {
        // b28d-solo-CUser 重写（2026-09-09，P52 链式 receiver+R2L 实参）：ORIG
        //   0x086796dc 把 `lea -0x16(%ebp),%ebx`（pkt 地址）跨 GetServerProxy 调用
        //   保活，group/proxy 均不落槽（链式直传）；characNo 为**死槽**（P171 照抄）。
        //   另：ORIG 尾恒 `mov $0x1,%eax` ⇒ 真实返回 bool（CUser.h 冻结，待 Main 代落）。
        // ORIG 0x086796dc 有 `call getCurCharacNo` 但结果**不落槽**（P0 表达式直用）；
        // 具名 characNo 反致帧 +0x10（sub 0x34→0x44、槽 -0x1c→-0x2c）。
        // b28d-solo-CUser 三试（报告双段复核 + Ghidra local_10）：ORIG 0x08679712
        //   `mov %eax,-0xc(%ebp)` 的槽址 = pkt 槽基 -0x16 + 0x0a = -0xc ⇒ 按 **P322①**
        //   （参数对象成员写入判据）该 store 是 **pkt.m_field0a = getCurCharacNo()**，
        //   不是具名 int 死局部（旧 "P171 死写" 判词作废，其两次复测读数属补救具名
        //   标量反劣，与本形无关）。语义：GM 注册包必须带当前 characNo，我方旧形
        //   (void) 丢弃 ⇒ 包内容错误（真语义 bug）。
        //   4.4 形验证（探针 /tmp/CuB/probe/p1.cpp 的 f，c6root 4.4.7 -m32 -O0）：
        //   `PKT pkt; pkt.f0a = getNo();` ⇒ `call getNo; mov %eax,-12(%ebp)` 直落成员。
        pkt.m_field0a = getCurCharacNo();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendTcpPacket((char*)&pkt, 0xe);
    }
    return true;   // ORIG 尾恒 mov $0x1,%eax（两路径同返 1）
}
#include "CUser/GetEventCreateDnfReward.inc"
{
    return m_eventCreateDnfReward;
}

#include "CUser/SetETC.inc"
{
    // ORIG W 0x080f9968 _ZN5CUser6SetETCEsi：
    // key 截断 short → m_mapEtc.operator[](key) → *ret = value → return value。
    // （m_mapEtc 已在 CUser.h +0x8eaa4 建模，旧 TODO 注释已过时。）
    m_mapEtc[key] = value;
    return value;
}

// ============================================================================
// G1-4 批次：CodeHackCheckStorage / 返回用户 / 预登录数据
// ============================================================================

#include "CUser/setTournamentAccount.inc"
{
    m_postChecksumData.m_field31 = flag;
}

#include "CUser/isTournamentAccount.inc"
{
    return m_postChecksumData.m_field31;
}

#include "CUser/setJoinedSchoolPointEvent.inc"
{
    m_postChecksumData.m_field44 = flag;
}

#include "CUser/setOnlinePreliminaryServerGroup.inc"
{
    m_postChecksumData.m_onlineServerGroup = group;
}

#include "CUser/setOnlinePreliminaryCharacNo.inc"
{
    m_postChecksumData.m_onlineCharacNo = characNo;
}

#include "CUser/setOnlinePreliminaryPlayType.inc"
{
    m_postChecksumData.m_onlinePlayType = playType;
}

#include "CUser/getOnlinePreliminaryPlayType.inc"
{
    return m_postChecksumData.m_onlinePlayType;
}

#include "CUser/getOnlinePreliminaryCharacNo.inc"
{
    return m_postChecksumData.m_onlineCharacNo;
}

#include "CUser/SetPCRoomNo.inc"
{
    m_postChecksumData.m_field54 = no;
}

#include "CUser/SetFirstLogin.inc"
{
    m_postChecksumData.m_firstLogin = 1;
}

#include "CUser/SetReturnUser.inc"
{
    m_postChecksumData.m_field59 = 1;
}

#include "CUser/ResetReturnUser.inc"
{
    m_postChecksumData.m_field59 = 0;
}

#include "CUser/IsReturnUser.inc"
{
    return m_postChecksumData.m_field59;
}

#include "CUser/SetReturnUserFirstUserLogin.inc"
{
    m_postChecksumData.m_field5a = flag;
}

#include "CUser/IsReturnUserFirstLogin.inc"
{
    return m_postChecksumData.m_field5a;
}

#include "CUser/GetReturnUserExpireTime.inc"
{
    return m_postChecksumData.m_field5c;
}

#include "CUser/SetReturnUserExpireTime.inc"
{
    m_postChecksumData.m_field5c = time;
}

#include "CUser/EnableSaveMemberBonusFatigue.inc"
{
    m_postChecksumData.m_field60 = 1;
}

#include "CUser/incAccountMemberBonusFatigue.inc"
{
    m_postChecksumData.m_field62 = (short)(m_postChecksumData.m_field62 + value);
}

#include "CUser/resetAccountMemberBonusFatigue.inc"
{
    m_postChecksumData.m_field62 = 0;
}

#include "CUser/getAccountMemberBonusFatigue.inc"
{
    return (short)(unsigned short)m_postChecksumData.m_field62;
}

#include "CUser/resetAccountUsedFatigue.inc"
{
    m_postChecksumData.m_totalFatigueUsed = 0;
}

#include "CUser/incChattingMessageCount.inc"
{
    m_postChecksumData.m_field64 += count;
}

#include "CUser/setChattingMessageCount.inc"
{
    m_postChecksumData.m_field64 = count;
}

#include "CUser/getChattingMessageCount.inc"
{
    return m_postChecksumData.m_field64;
}

#include "CUser/SetGameMasterCharacter.inc"
{
    m_postChecksumData.m_gameMasterCharac = pCharac;
}

#include "CUser/getStdDropRate.inc"
{
    return m_postChecksumData.m_field28;
}

#include "CUser/getAcccountHackInfo.inc"
{
    return &m_accHacking;
}

#include "CUser/setHackUserTimer.inc"
{
    m_379.m_field0 = 1;
}

#include "CUser/setHackUserFlag.inc"
{
    m_379.m_field1 = 1;
}

// P292/T3：ORIG W 0x0822fd3c（0xf）；消费点 0x081be54c 无 setcc ⇒ 返回 bool。
#include "CUser/isHackUser.inc"
{
    return m_379.m_field1;
}

#include "CUser/isHackUserTimer.inc"
{
    return m_379.m_field0;
}

// ============================================================================
// G1-4 批次：突破奖励 / 账号货物 / 商城 / 日常
// ============================================================================

#include "CUser/STBreakAwayRewards.inc"
{
    clear();
}

#include "CUser/clear_1.inc"
{
    m_rewardItemId = 0;
    m_rewardAItemCnt = 0;
    m_luckPoint = 0;
    m_fieldc = 0;
    m_itemOrder = 0;
}

void STBreakAwayRewards::operator=(const STBreakAwayRewards& other)
{
    m_rewardItemId = other.m_rewardItemId;
    m_rewardAItemCnt = other.m_rewardAItemCnt;
    m_luckPoint = other.m_luckPoint;
    m_fieldc = other.m_fieldc;
    m_itemOrder = other.m_itemOrder;
}

#include "CUser/getBreakAwayRewardItemId.inc"
{
    return m_breakAway.m_rewardItemId;
}

#include "CUser/setBreakAwayRewardItemId.inc"
{
    m_breakAway.m_rewardItemId = id;
}

#include "CUser/getBreakAwayRewardAItemCnt.inc"
{
    return m_breakAway.m_rewardAItemCnt;
}

#include "CUser/setBreakAwayRewardAItemCnt.inc"
{
    m_breakAway.m_rewardAItemCnt = cnt;
}

#include "CUser/getUserBreakAwayRewardLuckPoint.inc"
{
    return m_breakAway.m_luckPoint;
}

#include "CUser/setUserBreakAwayRewardLuckPoint.inc"
{
    m_breakAway.m_luckPoint = point;
}

#include "CUser/getBreakAwayRewardItemOrder.inc"
{
    return m_breakAway.m_itemOrder;
}

#include "CUser/getBreakAwayRewards.inc"
{
    return &m_breakAway;
}

#include "CUser/setBreakAwayRewards.inc"
{
    int frame_pad; (void)frame_pad;   // ORIG 0x831d52b sub $0x18：operator= 调用点 2 outgoing 槽帧舍入，dead 局部无指令
    m_breakAway = rewards;
}

#include "CUser/getBreakAwayQuestTime.inc"
{
    return m_gameOption.m_breakAwayQuestTime;
}

#include "CUser/setBreakAwayQuestTime.inc"
{
    m_gameOption.m_breakAwayQuestTime = time;   // ORIG 0x822fc7c this-先 uint 直写（字段已 uint 化）
}

#include "CUser/getBreakAwayDungeonClearCnt.inc"
{
    return m_gameOption.m_breakAwayDungeonClearCnt;
}

#include "CUser/setBreakAwayDungeonClearCnt.inc"
{
    m_gameOption.m_breakAwayDungeonClearCnt = cnt;
}

#include "CUser/subBreakAwayDungeonClearCnt.inc"
{
    m_gameOption.m_breakAwayDungeonClearCnt -= 1;
    setBreakAwayDungeonClear(true);
}

#include "CUser/setBreakAwayDungeonClear.inc"
{
    m_gameOption.m_breakAwayDungeonClear = flag;
}

#include "CUser/isBreakAwayRewardData.inc"
{
    return m_breakAwayRewardData;   // CUser+0x703c0（CGameOption 尾部 8B 收敛后）
}

#include "CUser/setGmQuestFlag.inc"
{
    m_gameOption.m_gmQuestFlag = flag;   // ORIG 0x822fca1 bool=bool 直落（字段已 bool 化，(char) cast 产 arg-先序）
}

#include "CUser/setWebAddress.inc"
{
    strncpy(m_gameOption.m_webAddress, addr, 0x10);
}


#include "CUser/SetCera.inc"
{
    m_cera = cera;
}

#include "CUser/UseCera.inc"
{
    if (m_cera >= cera)
        m_cera -= cera;
    else
        m_cera = 0;
    return m_cera;
}

#include "CUser/setRegDate.inc"
{
    m_regDate = date;
}

#include "CUser/setRegTime.inc"
{
    m_regTime = (int)time;
}

#include "CUser/getRegTime.inc"
{
    return m_regTime;
}

#include "CUser/resetUsedCera.inc"
{
    m_guildAgitDB.m_usedCera = 0;
}

#include "CUser/addUsedCera.inc"
{
    m_guildAgitDB.m_usedCera += cera;
}

#include "CUser/getUsedCera.inc"
{
    return m_guildAgitDB.m_usedCera;
}

#include "CUser/SetVillageAttackedReward.inc"
{
    m_quest.m_field761c = flag;
}

#include "CUser/setPcRoomAuth.inc"
{
    m_pcRoomAuth = flag;   // CUser+0x8cef1（= m_quest+0x761d；UserQuest 收敛后）
}

#include "CUser/fillUserInfo4Antibot.inc"
{
    m_field8cf58 = -1;
    strncpy(m_pad8cf18, getCurCharacName(), 0x14);
}

#include "CUser/getUserInfo4Antibot.inc"
{
    return m_pad8cf18;
}

#include "CUser/SetAccountLastPlayTime.inc"
{
    m_premium.m_lastPlayTime = (int)time;
}

#include "CUser/SetLoginTime.inc"
{
    m_premium.m_loginTime = (int)time;
}

#include "CUser/GetLoginTime.inc"
{
    return m_premium.m_loginTime;
}

#include "CUser/SetLogInOutState.inc"
{
    m_premium.m_field98c = state;
}

#include "CUser/GetLogInOutState.inc"
{
    return m_premium.m_field98c;
}

#include "CUser/get_public_ip_address.inc"
{
    return m_publicIp;
}

#include "CUser/set_public_ip_address.inc"
{
    m_publicIp = ip;
}

#include "CUser/set_local_ip_address.inc"
{
    m_localIp = ip;
}

#include "CUser/setHangameUser.inc"
{
    m_hangameUser = flag;
}

#include "CUser/isHangameUser.inc"
{
    return m_hangameUser;
}

#include "CUser/IsPowerUp.inc"
{
    return m_powerUp;
}

#include "CUser/SetPowerUp.inc"
{
    m_powerUp = flag;   // ORIG 直写 mov %dl,0x8d253（P32 bool 字段直赋）
}

#include "CUser/SetRating.inc"
{
    m_rating = rating;
}

#include "CUser/SetSchoolNo.inc"
{
    m_schoolNo = no;
}

#include "CUser/GetSchoolNo.inc"
{
    return m_schoolNo;
}

#include "CUser/SetSchoolName.inc"
{
    m_str8e080 = name;
}

#include "CUser/GetTutorialSkipable.inc"
{
    return m_tutorialSkipable;
}

#include "CUser/SetTutorialSkipable.inc"
{
    m_tutorialSkipable = flag;
}

#include "CUser/getHumanCertifyTimerKey.inc"
{
    return m_humanCertifyTimerKey;
}

#include "CUser/setHumanCertifyErrorCnt.inc"
{
    m_humanCertifyErrorCnt = cnt;
}

#include "CUser/isSendMailCertified.inc"
{
    return m_field8e3ed;
}

#include "CUser/setRestingUserRestrict.inc"
{
    m_mcap.m_field544 = flag;
}

#include "CUser/setPunishTradeAlert.inc"
{
    m_mcap.m_field546 = alert;
}

#include "CUser/getPunishTradeAlert.inc"
{
    return (short)(unsigned short)m_mcap.m_field546;
}

#include "CUser/resetDungeonClearHackCheck.inc"
{
    m_mcap.m_field54c = 0;
}

#include "CUser/IsRecvEvent.inc"
{
    return m_mcap.m_recvEventWaiting;
}

#include "CUser/GetAutoClientRate.inc"
{
    return m_mcap.m_field1c;
}

#include "CUser/isARSUserKick.inc"
{
    return m_mcap.m_field561;
}

#include "CUser/setSecuReward.inc"
{
    m_hades.m_secuReward = flag;
}

#include "CUser/NeedSecuReward.inc"
{
    return m_hades.m_secuReward;
}

#include "CUser/setSecuType.inc"
{
    m_hades.m_secuType = type;
}

#include "CUser/getSecuType.inc"
{
    return m_hades.m_secuType;
}

#include "CUser/getRewardType.inc"
{
    return m_hades.m_rewardType;
}

#include "CUser/setRewardType.inc"
{
    m_hades.m_rewardType = type;
}

#include "CUser/setSecuGrade.inc"
{
    m_hades.m_secuGrade = grade;
}

#include "CUser/setFlagPresentAvengerTitle.inc"
{
    m_ceraUserInfo.m_field62 = flag;
}

#include "CUser/incCharacSlotLimit.inc"
{
    m_ceraUserInfo.m_field61 += count;
}

// ORIG 0x08230860 (W)：movzbl +0x8d245（m_ceraUserInfo.m_field61）
#include "CUser/getCharacSlotLimit.inc"
{
    return m_ceraUserInfo.m_field61;
}

// ORIG 0x08230870 (W)：movzbl +0x8d244（m_ceraUserInfo.m_field60）
#include "CUser/getSlotEffectCount.inc"
{
    return static_cast<unsigned char>(m_ceraUserInfo.m_field60);
}

// ORIG 0x08230880 (W)：写 +0x8d244（m_ceraUserInfo.m_field60）
#include "CUser/setSlotEffectCount.inc"
{
    m_ceraUserInfo.m_field60 = count;   // ORIG 0x865c… 0x8230893 mov %dl 直写无归一 ⇒ 字段 uchar（Main 已回改），同型 this-先直落
}

// ORIG 0x08676826 (T)：组 Packet_Randombox_statistic（header 0x17b6/0xc，boxKind@+0xa、
// actionKind@+0xb）经 CStatisticServerProxy::SendPacket(pkt, 0xc) 发统计服务器（组 0）。
#include "CUser/SendRandomBoxStatistic.inc"
{
    // ORIG 0x8676826：无 proxy 判空/无具名槽（call 结果 %eax 直传 (%esp)，&pkt 先
    // 载 %ebx 跨 call，技巧 37），SendPacket 无条件调（非虚 null-this 安全）。
    Packet_Randombox_statistic pkt;
    pkt.m_boxKind = static_cast<char>(boxKind);
    pkt.m_actionKind = static_cast<char>(actionKind);
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0xc);
}

// ORIG 0x0868f6bc (T)：find_item(itemIdx)→verifyStackableItemProcess
#include "CUser/verifyStackableItemActionPlace.inc"
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    return verifyStackableItemProcess(type, item);
}

// ORIG 0x0868f60e (T)：is_stackable→verifyStackableAction→verifyUsablePlace→verifyUsableServerType
#include "CUser/verifyStackableItemProcess.inc"
{
    if (item == 0)
        return 0;
    if (!item->is_stackable())
        return 0;
    // ORIG 0x0868f60e：三处调用每次重读 0x10(%ebp) arg 槽（`mov 0x10(%ebp),%eax;
    // mov 0xc/0x8(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp)`）——具名 cast
    // 中转变量会膨胀 -0x10/-0xc 槽（技巧 36/37），cast 内联。
    if (!((CStackableItem*)item)->verifyStackableAction(type))
        return 0;
    if (!((CStackableItem*)item)->verifyUsablePlace(this))
        return 0;
    if (!((CStackableItem*)item)->verifyUsableServerType())
        return 0;
    return 1;
}

#include "CUser/set_ontime_last_recv_idx.inc"
{
    m_ceraUserInfo.m_ontimeLastRecvIdx = idx;
}

#include "CUser/get_ontime_last_recv_idx.inc"
{
    return m_ceraUserInfo.m_ontimeLastRecvIdx;
}

#include "CUser/set_update_ontime_last_recv_idx.inc"
{
    m_ceraUserInfo.m_updateOntime = 1;
}

#include "CUser/setCheckPickUpRandomOptionItem.inc"
{
    m_field8eb44 = flag;
}

#include "CUser/getCheckPickUpRandomOptionItem.inc"
{
    return m_field8eb44;
}

#include "CUser/set_cera_item_order_no.inc"
{
    m_levelUpGift.m_field18 = no;
}

#include "CUser/get_cera_item_order_no.inc"
{
    return m_levelUpGift.m_field18;
}

#include "CUser/setPcRoomPlayStartTime.inc"
{
    m_pcRoomPlayStartTime = time;
}

#include "CUser/IsDeleteEventDungeonInvitation.inc"
{
    return m_deleteEventDungeonInvitation;
}

#include "CUser/IsUsedCoinInDungeon.inc"
{
    return m_field8eae0;
}

#include "CUser/ResetCoinCount.inc"
{
    m_field8eb1c = 0;
}

#include "CUser/GetUsedCoinCount.inc"
{
    return m_field8eb1c;
}

// ORIG W 0x082604ec _ZNK5CUser21getCurCharacQuestRefR Ev（const）
// 返回 m_quest（+0x858d4）引用指针；与 getCurCharacQuestR 同址、独立符号。
#include "CUser/getCurCharacQuestRefR.inc"
{
    return const_cast<UserQuest*>(&m_quest);
}

// ORIG W 0x0830edaa _ZN5CUser36getAddIndependentDropRateFromPremiumE17ENUM_PREMIUM_TYPEj
// 转发 m_premium（+0x7121c）独立掉落率查询（参数直传，ORIG 无交换）。
#include "CUser/getAddIndependentDropRateFromPremium.inc"
{
    return m_premium.GetAddIndependentDropRate(type, level);
}

// ORIG T 0x08689576 _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE
// 事件地下城清剿奖励发放：逐 (itemId,count)；find_item 失败直接结束；
// 背包无该物品 → make_item 构造后 insertItemIntoInventory（reason 13，log=1，
// stackable=false）；已有 → ItemAdd 日志 + 数量累加 update_item；
// 成功落位后 SendUpdateItemList(1, 0, slot)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689576 | _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/EventDungeonClearReward.inc"
{
    const int count = items.size();
    for (int i = 0; i < count; ++i)
    {
        int itemId = items[i].first;
        int itemCnt = items[i].second;
        CItem* data = G_CDataManager()->find_item(itemId);
        if (data == NULL)
            break;
        Inven_Item item;
        int slot = getCurCharacInvenR()->GetInvenData(itemId, item);
        if (slot < 0)
        {
            data->make_item(item);
            // b28d-G3C 2026-09-10（语句序 = ORIG 0x0868 序）：ORIG 先写 +0x7 再写
            //   +0x2 —— `mov -0x14(%ebp),%eax; mov %eax,-0x56(%ebp)`（m_addInfo2
            //   == itemCnt，对象基址 -0x5d ⇒ +0x7）在后一条 `mov -0x18(%ebp),%eax;
            //   mov %eax,-0x5b(%ebp)`（m_addInfo == itemId ⇒ +0x2）**之前**；
            //   我方原序（addInfo 先）与之相反。
            item.m_addInfo2 = itemCnt;
            item.m_addInfo = itemId;
            slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, false);
        }
        else
        {
            // b28d-G3C 2026-09-10（P183 去具名）：ORIG 0x0868 段 `call get_add_info`
            //   结果经 %eax **直传**实参位（`mov %eax,0xc(%esp)`，无 store/load），
            //   我方原 `int addInfo` 具名槽多 2 条（-0xc 槽往返）且独占 4 字节。
            m_historyLog.ItemAdd(INVEN_TYPE_EQUIP, itemCnt, item.get_add_info(),
                                 item, eItemAddReason_0xd);
            item.m_addInfo2 += itemCnt;
            getCurCharacInvenW()->update_item(INVEN_TYPE_EQUIP, slot, item);
        }
        if (slot >= 0)
        {
            SendUpdateItemList(eSendTarget_1, ENUM_ITEMSPACE_0, slot);
        }
    }
}

#include "CUser/set_trick_area.inc"
{
    m_field8cfc0 = area;
}

#include "CUser/get_before_area.inc"
{
    return m_field8cfb8;
}

// ============================================================================
// G1-4 批次：锁 / 账号货物 / 其余访问器
// ============================================================================

#include "CUser/GetAccountCargo.inc"
{
    return &m_accountCargo;
}

#include "CUser/IsExistAccountCargo.inc"
{
    return m_accountCargo.GetCapacity() != 0;
}

#include "CUser/DeleteCargo.inc"
{
    m_accountCargo.Clear();
}

#include "CUser/lock.inc"
{
    m_mutex1.lock();
}

#include "CUser/unlock.inc"
{
    m_mutex1.unlock();
}

#include "CUser/getBackupSeed.inc"
{
    return m_backupSeed;
}

#include "CUser/getPowerSide.inc"
{
    return m_guildDB.m_field95;
}

#include "CUser/getClientHWSpec.inc"
{
    return (char*)&m_clientSpec;
}

#include "CUser/SetGuildDBInfo.inc"
{
    memcpy(&m_guildDB, &info, 0xbd);
}

#include "CUser/ChangeGuildName.inc"
{
    memset(&m_guildDB, 0, 0x17);
    memcpy(&m_guildDB, name, 0x16);
}

#include "CUser/SetGuildAgitFlag.inc"
{
    if (flag)
        m_guildDB.m_agitExist = 1;
    else
        m_guildDB.m_agitExist = 0;
}

#include "CUser/SetGuildAgitDBInfo.inc"
{
    memcpy(&m_guildAgitDB, &info, 1);
}

#include "CUser/SetGuildCargoCapacity.inc"
{
    m_field8e094 = capacity;
}

#include "CUser/SetTodayGuildMember.inc"
{
    m_guildAgitDB.m_todayGuildMember = flag;
}

#include "CUser/IsTodayGuildMember.inc"
{
    return m_guildAgitDB.m_todayGuildMember;
}

#include "CUser/SetLastLoginCharacNo.inc"
{
    m_field8d0f4 = characNo;
}

#include "CUser/SetLastLoginChannelNo.inc"
{
    m_field8d0f8 = channelNo;
}

#include "CUser/SetValidLastLoginCharac.inc"
{
    m_field8d0fc = flag;
}

#include "CUser/SetReportMannerlessUserCount.inc"
{
    m_field8d110 = count;
}

#include "CUser/GetChecksumAnswerCount.inc"
{
    return m_field8cfd8;
}

#include "CUser/IncChecksumAnswerCount.inc"
{
    m_field8cfd8 += 1;
}

#include "CUser/GetCheckConnInterval.inc"
{
    return m_field8cfd0;
}

#include "CUser/SetResumeChecksumTime.inc"
{
    m_field8cfd4 = time;
}

#include "CUser/incre_check_count.inc"
{
    m_field8cfc8++;
}

#include "CUser/get_connect_count.inc"
{
    return (short)(unsigned short)m_field8cfc8;
}

#include "CUser/setTotalHackUserTradeGold.inc"
{
    // ORIG 0x84ec940：mov this,%ecx; mov -0x8,%eax; mov -0x4,%edx; mov %eax,0x8d1ac(%ecx);
    // mov %edx,0x8d1b0(%ecx)。ll 成员直写= 4.4 同形（probe w2 实证）。
    m_379.m_hackTradeGold = gold;
}

#include "CUser/setTotalHackUserTradeCnt.inc"
{
    m_379.m_hackTradeCnt = cnt;
}

#include "CUser/SetRevivalTime.inc"
{
    m_field8d1dc = (int)time;
}

#include "CUser/GetRevivalTime.inc"
{
    return m_field8d1dc;
}

#include "CUser/SetUseAPCPotionTime.inc"
{
    m_field8d1e0 = (int)time;
}

#include "CUser/GetUseAPCPotionTime.inc"
{
    return m_field8d1e0;
}

#include "CUser/SetDailyBadge.inc"
{
    for (int i = 0; i <= 2; ++i)
        m_dailyBadge[i] = badge[i];
}

#include "CUser/SetUserDetailInfo.inc"
{
    strncpy(&m_accName[0x16], a, 0x80);
    strncpy(&m_accName[0x96], b, 0x80);
    strncpy(&m_accName[0x116], c, 0x10);
}

// ============================================================================
// G1-4 批次：角色列表 / 视图
// ============================================================================

#include "CUser/get_charac_name.inc"
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0;
    return m_characList[idx].m_name;
}

#include "CUser/GetCharacJob.inc"
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0xb;
    // ORIG 0x822ff5c：单条 movzbl 0x26(%eax),%eax。4.4 对 char 字段两种写法均多一级
    // 扩展（值 cast=双 movzbl；地址重解释=add+双 movzbl），需字段 uchar 化（Main 评估中，
    // Dispatchers 0x6306 movsbl 依赖 char 语义）——暂取值 cast（ndiff 更小）。
    return (unsigned char)m_characList[idx].m_field26;
}

#include "CUser/getCharacLevelByCharacNo.inc"
{
    // ORIG 0x0868e5xx：begin→-0x10、end→-0x14 prologue 双缓存（技巧 25）；
    // return 无显式 cast（ORIG 尾单条 movzwl 直返，(unsigned short) 强转反产两段）。
    int idx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itend = m_characList.end();
    for (; it != itend; ++idx, ++it)   // ORIG 0x84ecc4f ++idx 先于 ppEv
    {
        if (it->m_characNo == characNo)
            // 函数返回类型 ushort（Main 落头）后裸 return：4.4 隐式 HImode 截断
            // 单条 movzwl 0x27(%eax)（ORIG 0x84ecc49 同形）；显式 cast 反产
            // movzwl %ax 二段展宽（probe 三试）——本行为定稿。
            return it->m_level;
    }
    return -1;
}

#include "CUser/get_charac_name_by_charac_no.inc"
{
    // ORIG 0x0863bece：循环下标 int，size 有符号比较（setg）。
    for (int i = 0; i < static_cast<int>(m_characList.size()); ++i)
    {
        if (m_characList[i].m_characNo == characNo)
            return m_characList[i].m_name;
    }
    return 0;
}

#include "CUser/setCharacName.inc"
{
    // ORIG W 0x84ecc7c：prologue begin→-0x10、end→-0x14 双缓存；latch ++idx 先于
    // ++it（技巧 25 具名缓存消 end 重构）。
    int idx = 0;
    std::vector<_Charac_info>::iterator it = m_characList.begin();
    std::vector<_Charac_info>::iterator itend = m_characList.end();
    for (; it != itend; ++idx, ++it)
    {
        if (it->m_characNo == characNo)
        {
            strncpy(it->m_name, name, 0x1d);
            m_characterView.enableSaveCharacView();
            return true;
        }
    }
    return false;
}

#include "CUser/SetCharacViewState.inc"
{
    m_characterView.enableSaveCharacView();
    CUserCharacInfo::setCharacViewState(state);
}

#include "CUser/SetCharacViewState_1.inc"
{
    if (characNo < 0 || characNo >= (int)m_characList.size())
        return;
    m_characterView.enableSaveCharacView();
    m_characList[characNo].m_viewState = state;
}

#include "CUser/GetCharacViewState.inc"
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return -1;
    return m_characList[idx].m_viewState;
}

#include "CUser/SetCharacViewStateByCharacNo.inc"
{
    // ORIG 0x0868??：prologue begin→-0xc、end→-0x10 双缓存（技巧 25/T-CU3）。
    std::vector<_Charac_info>::iterator it = m_characList.begin();
    std::vector<_Charac_info>::iterator itend = m_characList.end();
    for (; it != itend; ++it)
    {
        if (it->m_characNo == characNo)
        {
            m_characterView.enableSaveCharacView();
            it->m_viewState = state;
            return true;
        }
    }
    return false;
}

// ============================================================================
// 批次 A 访问器（ORIG 反汇编逐偏移补全：0x8101048 / 0x82301ae / 0x827565a /
// 0x85a932a / 0x868a99a / 0x868fa46 / 0x86912b0 / 0x8691cd0 / 0x8692af6 等）
// ============================================================================

#include "CUser/sendEventInfo.inc"
{
}

#include "CUser/questAutoClear.inc"
{
}

#include "CUser/_onCompoundItem.inc"
{
}

#include "CUser/_updateNPCRelationship.inc"
{
}

#include "CUser/processNPCGiftOnLevelUp.inc"
{
}

#include "CUser/GetUserName.inc"
{
    return m_accName + 0x16;
}

#include "CUser/GetUserEMail.inc"
{
    return m_accName + 0x116;
}

#include "CUser/GetSsnString.inc"
{
    return m_accName + 0x96;
}

#include "CUser/getSex.inc"
{
    const char* ssn = GetSsnString();
    if (strlen(ssn) < 0xd)
        return -1;
    if (ssn[6] == '2' || ssn[6] == '4')
        return 0;
    return 1;
}

#include "CUser/GetCSHashSet.inc"
{
    return &m_set8eb00;
}

#include "CUser/GetGuildName.inc"
{
    return (char*)&m_guildDB;
}

#include "CUser/GetGuildLevel.inc"
{
    return m_guildDB.m_fundLevel;   // +0x8d03b（成员视图折叠单指令 movzbl）
}

#include "CUser/GetHackCheck.inc"
{
    return &m_hackLogCheck;
}

#include "CUser/getBingoData.inc"
{
    return &m_bingo;
}

#include "CUser/GetGuildDBInfo.inc"
{
    return &m_guildDB;
}

#include "CUser/getGarenaAuthData.inc"
{
    return reinterpret_cast<char*>(&m_garenaAuthData);  // +0x6ef92 Taiwan::GarenaAuthData 成员视图（签名保持 char*）
}

#include "CUser/GetPISenderManager.inc"
{
    return &m_piSender;
}

#include "CUser/GetPIReceiverManager.inc"
{
    return &m_piReceiver;
}

#include "CUser/getGrowthWeaponEventdata.inc"
{
    return &m_growthWeapon;
}

#include "CUser/get_ingame_event_history.inc"
{
    return &m_map8eb68;
}

#include "CUser/get_ingame_event_history_update.inc"
{
    return &m_map8eb80;
}

#include "CUser/getGrowthCreatureEventdata.inc"
{
    return &m_growthCreature;
}

#include "CUser/get_server_fatigue_day_data.inc"
{
    return &m_map8eb50;
}

#include "CUser/getCharacLevelUpGift_AccountOnce.inc"
{
    return &m_levelUpGift;
}

#include "CUser/GetMailBox.inc"
{
    return *(void**)((char*)&m_premium.m_mailBox);
}

#include "CUser/GetMileage.inc"
{
    return m_mileage;
}

#include "CUser/GetPCRoomNo.inc"
{
    return m_postChecksumData.m_field54;
}

#include "CUser/GetCeraPoint.inc"
{
    return m_ceraPoint;
}

#include "CUser/getAntibotKey.inc"
{
    return m_antibotKey;
}

#include "CUser/GetManageLevel.inc"
{
    return m_field8ebd0;
}

#include "CUser/GetManagePoint.inc"
{
    return m_field8ebd4;
}

#include "CUser/GetUserMaxLevel.inc"
{
    return m_field8ebd8;
}

#include "CUser/onDungeonClear.inc"
{
}

#include "CUser/GetDebugCommand.inc"
{
    return m_debugCommand;
}

#include "CUser/GetTradePunishType.inc"
{
    return m_postChecksumData.m_tradePunishType;
}

#include "CUser/getLastLotteryTime.inc"
{
    return m_mcap.m_lastLotteryTime;
}

#include "CUser/GetNonClientRandInt.inc"
{
    return m_piSender.m_nonClientRandInt;
}

#include "CUser/GetNonClientFlag.inc"
{
    return m_piSender.m_nonClientFlag;
}

#include "CUser/GetLastLoginCharacNo.inc"
{
    return m_field8d0f4;
}

#include "CUser/GetLastLoginChannelNo.inc"
{
    return m_field8d0f8;
}

#include "CUser/IsValidLastLoginCharac.inc"
{
    return m_field8d0fc;
}

#include "CUser/getRecipeProbability.inc"
{
    return m_field8ec2c;
}

#include "CUser/get_local_ip_address.inc"
{
    return *(int*)((char*)&m_localIp);
}

#include "CUser/GetGuildCargoCapacity.inc"
{
    return m_field8e094;
}

#include "CUser/GetGuildPowerWarPoint.inc"
{
    return m_guildDB.m_powerWarPoint;
}

#include "CUser/GetResumeChecksumTime.inc"
{
    return m_field8cfd4;
}

#include "CUser/GetGameMasterCharacter.inc"
{
    return *(CGameMasterCharacter**)((char*)&m_postChecksumData.m_gameMasterCharac);
}

#include "CUser/getBreakAwayAccureCera.inc"
{
    return m_gameOption.m_breakAwayAccureCera;
}

#include "CUser/getBreakAwayRewardOrder.inc"
{
    return m_gameOption.m_breakAwayRewardOrder;   // +0x654
}

#include "CUser/get_multiboxLotteryItemFailCnt.inc"
{
    return m_multiboxLotteryFailCnt;   // CUser+0x703bc
}

#include "CUser/GetCurCharacUsedFatigueQuantity.inc"
{
    return m_field8eba8;
}

#include "CUser/isHumanCertified.inc"
{
    return m_humanCertified;
}

#include "CUser/isLoadingHackGold.inc"
{
    return m_mcap.m_loadingHackGold;
}

#include "CUser/isSaveInformNoticeFlag.inc"
{
    return m_saveInformNoticeFlag;   // ORIG movzbl 0x8ebc1 直返（m_informNoticeFlags 起 +0x10）
}

#include "CUser/CheckMaxLuckyLevel.inc"
{
    return 1;
}

// ---- 批次 A 第 2 组（ORIG 0x82300e8 / 0x82301d8 / 0x8230246 / 0x8692a08 /
// 0x8692b74 / 0x86971ce / 0x86975b6 / 0x8568e48 等） ----

#include "CUser/OnRecvEvent.inc"
{
    m_mcap.m_recvEventWaiting = true;   // +0x18
}

#include "CUser/GetBuyingGold.inc"
{
    return m_field8ead8;   // ORIG movzbl 直返（无 test/setne）
}

#include "CUser/SetBuyingGold.inc"
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl,0x8ead8 完全一致）。
    m_field8ead8 = flag;
}

#include "CUser/GetFirstLogin.inc"
{
    return m_postChecksumData.m_firstLogin;   // ORIG movzbl 直返（成员 bool）
}

#include "CUser/SetProgLogout.inc"
{
    m_progLogout = 1;   // 字段已 bool 化（ORIG IsProgLogout@0x82a6796 movzbl 直返）；&-cast 反而多余
}

#include "CUser/isRestingUser.inc"
{
    return m_mcap.m_field544;   // ORIG movzbl 直返（无 test/setne）
}

#include "CUser/InitReceivedRequestType.inc"
{
    m_receivedRequestType = 0xff;
}

#include "CUser/isBreakAwayDungeonClear.inc"
{
    return m_gameOption.m_breakAwayDungeonClear;   // ORIG movzbl 直返（无 test/setne）
}

#include "CUser/isUsedBreakAwayLuckPoint.inc"
{
    return m_usedBreakAwayLuckPoint;   // ORIG movzbl 0x703c1 直返
}

#include "CUser/isJoinedSchoolPointEvent.inc"
{
    return (unsigned char)m_postChecksumData.m_field44;
}

#include "CUser/DisableSaveMemberBonusFatigue.inc"
{
    m_postChecksumData.m_field60 = false;
}

#include "CUser/IsEnableSaveMemberBonusFatigue.inc"
{
    return (unsigned char)m_postChecksumData.m_field60;
}

#include "CUser/SetProperLevelDungeonUser.inc"
{
    m_field8eb99 = true;
}

#include "CUser/ClearProperLevelDungeonUser.inc"
{
    m_field8eb99 = false;
}

#include "CUser/IsProperLevelDungeonUser.inc"
{
    return m_field8eb99;
}

#include "CUser/isCharacLinkMessageFlag.inc"
{
    return m_breakAway.m_field14;
}

// b28d-solo-CUser：CUser.h:2718 经 Main 代落 unsigned char→bool（P292：ORIG 调用点
//   0x08652c9a `call; test %al,%al; je` 无 setcc）。ORIG 定义体 0x086973f8 直返
//   `movzbl 0x703d9; pop; ret`（P9 直返字节）⇒ 体写 (bool) 转换试直返形。
#include "CUser/isLinkCharacDisconnectFlag.inc"
{
    return (bool)m_breakAway.m_field15;
}

#include "CUser/getDisconnectLinkCharacSlotIdx.inc"
{
    return (unsigned char)m_breakAway.m_field16;
}

// b28d-CUser P292：ORIG 0x08697598 体 movzbl 直返（无 setne）⇒ bool。
#include "CUser/is_update_ontime_last_recv_idx.inc"
{
    return m_ceraUserInfo.m_updateOntime;   // §4.3 具名(UserInfo+0x68)
}

#include "CUser/reset_update_ontime_last_recv_idx.inc"
{
    m_ceraUserInfo.m_updateOntime = false;                 // §4.3 具名(UserInfo+0x68)
}

#include "CUser/CheckLoadRentalInfoFromExchange.inc"
{
    return m_rentalFlag2;   // ORIG movzbl 0x6ef91 直返
}

#include "CUser/GetPuUser.inc"
{
    return m_field796d8;   // +0x796d8（成员直返折叠单指令 movzbl）
}

#include "CUser/SetCeraPoint.inc"
{
    m_ceraPoint = point;
}

#include "CUser/SetDebugCommand.inc"
{
    m_debugCommand = cmd;
}

#include "CUser/decre_check_count.inc"
{
    *(short*)((char*)&m_field8cfc8) = 0;
}

#include "CUser/setLastLotteryTime.inc"
{
    // ORIG 0x82307cb-0x82307d1：this-先 uint 直写、无帧。b28d-solo-CUser AE：P211 同型律——
    //   ORIG LHS-first ⇒ 字段与形参 `unsigned long t` 同型；CMCAPManager.h:81 已改
    //   int→unsigned long（ia32 4B，偏移 0x548/sizeof 不变），探针 /tmp/probe_store.cpp 双形实证。
    m_mcap.m_lastLotteryTime = t;
}

#include "CUser/SetNonClientRandInt.inc"
{
    m_piSender.m_nonClientRandInt = v;
}

#include "CUser/recipeForceProbability.inc"
{
    *(int*)((char*)&m_field8ec2c) = v;
}

#include "CUser/setBreakAwayAccureCera.inc"
{
    m_gameOption.m_breakAwayAccureCera = v;
}

#include "CUser/setCharacAntibotSerialNum.inc"
{
    *(int*)((char*)&m_field8ec24) = v;
}

#include "CUser/set_multiboxLotteryItemFailCnt.inc"
{
    m_multiboxLotteryFailCnt = v;   // CUser+0x703bc
}

#include "CUser/resetPlayExpAdd.inc"
{
    m_gameOption.m_playExpAdd = 0;  // 字段 unsigned 化后直写复原 ORIG `movl $0x0,0x703a4(%eax)` 单条
}

#include "CUser/resetCleanpadFailCnt.inc"
{
    *(int*)((char*)&m_humanCertifyErrorCnt) = 0;
}

#include "CUser/isCleanPadVerifyLimit.inc"
{
    return *(int*)((char*)&m_humanCertifyErrorCnt) > 2;
}

#include "CUser/resetTotalPcRoomPlayTime.inc"
{
    m_totalPcRoomPlayTime = 0;
}

#include "CUser/ResetCurCharacUsedFatigueQuantity.inc"
{
    m_field8eba8 = 0;   // ORIG 0x86975d4 movl $0x0,0x8eba8(%eax) 单条位移直写（&-cast 反产 add+mov 双条）
}

// ORIG _ZN5CUser24set_server_fatigue_countEs T 0x0868d32a：按当日
// get_today_time_day 在 m_map8eb50 累加，无表项则插入。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868d32a | _ZN5CUser24set_server_fatigue_countEs
// 2026-09-10（FL2CUser 类型轴）：ORIG 0x0868d32a 段 p167 实形 = value_type 构造
//   `_ZNSt4pairIKhsEC2IRhRKsEE`（第二实参 **const short&**）；我方 `C1IRhRsEE`
//   （非 const）⇒ 原码形参为顶层 const（顶层 cv 不进 mangled，声明侧不变）。
#include "CUser/set_server_fatigue_count.inc"
{
    unsigned char todayDay =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->get_today_time_day();
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(todayDay);
    if (it != m_map8eb50.end())
    {
        // b28d-G3C 2026-09-10（P167 调用点差集）：ORIG 0x0868d3a? 段只调
        //   `_Rb_tree_iterator::ptEv` **一次**（`call ptEv; movzwl 0x2(%eax),%edx;
        //   mov %edx,%ecx; movzwl -0x2c(%ebp),%edx; lea (%ecx,%edx,1),%edx;
        //   mov %dx,0x2(%eax)` → 同一 %eax 读改写）。我方原 `it->second =
        //   static_cast<short>(it->second + v);` 两次写 `it->second` ⇒ 两次 ptEv
        //   调用（EXTRA ×1）。改复合赋值（单次 ->）。
        it->second += v;
    }
    else
    {
        // b28d-solo-CUser p167：ORIG 0x0868d32a 直接构造 value_type
        //   `pair<unsigned char const, short>::pair<unsigned char&, short const&>`
        //   （无 make_pair 调用）⇒ 改 value_type 直构（P123/P61）。
        m_map8eb50.insert(std::map<unsigned char, short>::value_type(todayDay, v));
    }
}

// ORIG _ZN5CUser31IncCurCharacUsedFatigueQuantityEj W 0x08110e1e：m_field8eba8 += n。
#include "CUser/IncCurCharacUsedFatigueQuantity.inc"
{
    m_field8eba8 += n;
}

// ORIG _ZN5CUser35IncCurCharacUsedGiftFatigueQuantityEj W 0x08110e3a：m_field8ebac += n。
#include "CUser/IncCurCharacUsedGiftFatigueQuantity.inc"
{
    m_field8ebac += n;
    // ORIG 0x08110e57：累加后 SetChangedGiftFatigueQuantity(true)
    SetChangedGiftFatigueQuantity(true);
}

// ORIG _ZN5CUser27ProcProperLevelDungeonClearEv T 0x0868f5fa（0x13 字节）：单行转发。
#include "CUser/ProcProperLevelDungeonClear.inc"
{
    GiveMeBoxReward();
}

// ORIG _ZN5CUser15GiveMeBoxRewardEv T 0x08693758：单行转发 DB 请求。
#include "CUser/GiveMeBoxReward.inc"
{
    DBInsertGiveBoxEventReward::makeRequest(this);
}

// ORIG _ZN5CUser18SetTradePunishTypeEi T 0x08689890：printf 日志后
// m_postChecksumData.m_tradePunishType |= type。
#include "CUser/SetTradePunishType.inc"
{
    printf(" CUser::SetTradePunishType(%d) \n", type);
    m_postChecksumData.m_tradePunishType =
        m_postChecksumData.m_tradePunishType | type;
}

// ORIG _ZN5CUser10SetMileageEi T 0x086509c6：LogHistory 记录新旧值后写 m_mileage。
#include "CUser/SetMileage.inc"
{
    LogHistory("Mileage %s,%d,%d,%d", "Set", mileage, m_mileage, mileage);
    m_mileage = mileage;
}

// ORIG _ZN5CUser12AddMoneyPlusEi T 0x0868f266：当日滚窗 m_money[0]+=value，m_money[2]=now。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868f266 | _ZN5CUser12AddMoneyPlusEi
#include "CUser/AddMoneyPlus.inc"
{
    // 残差 N81 附注族（4.4 恒形）：尾句 `+=` 的 load 4.4 恒 eax 中转+mov 转发+
    // add mem 直加（ORIG 4.1 mem→ecx 直载+value→eax+add eax,ecx）。穷举 +=/自加
    // 表达式/操作数交换/引用局部/寄存器复刻（nl11_probe4/5/7）均不可翻。
    if (exceptedStateMoneyLog())
    {
        return;
    }
    const int now = GlobalData::s_systemTime_.getCurSec();
    NextCharacMoneyLog(now);
    m_moneyLogs[m_moneyLogIdx].m_money[2] = now;
    m_moneyLogs[m_moneyLogIdx].m_moneyU[0] += value;
}

// ORIG _ZN5CUser13AddMoneyMinusEi T 0x0868f310：当日滚窗 m_money[1]+=value，m_money[2]=now。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868f310 | _ZN5CUser13AddMoneyMinusEi
#include "CUser/AddMoneyMinus.inc"
{
    // 残差 N81 附注族，同 AddMoneyPlus（load 中转寄存器 4.4 恒 eax）。
    if (exceptedStateMoneyLog())
    {
        return;
    }
    const int now = GlobalData::s_systemTime_.getCurSec();
    NextCharacMoneyLog(now);
    m_moneyLogs[m_moneyLogIdx].m_money[2] = now;
    m_moneyLogs[m_moneyLogIdx].m_moneyU[1] += value;
}

// ORIG _ZN5CUser18NextCharacMoneyLogEi T 0x0868f1c0：当前槽含时间且与
// 传入时刻不同日 → 槽推进（钳 6）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f1c0 | _ZN5CUser18NextCharacMoneyLogEi
#include "CUser/NextCharacMoneyLog.inc"
{
    if (m_moneyLogs[m_moneyLogIdx].m_money[2] != 0)
    {
        struct tm currentTm;
        struct tm oldTm;
        // b28d-solo-CUser 试形（P0 槽往返反推）：ORIG 0x868f1xx 两个 localtime_r 实参为
        //   `lea 0xc(%ebp)`（**形参 now 的地址**）与 `lea 0xc(%eax)`（**成员 m_money[2]
        //   的地址**）⇒ 原码无 current/old 具名 time_t 局部（我方原形各多一槽 + 装载序差）。
        localtime_r((const time_t*)&now, &currentTm);
        // b28d-solo-CUser 复测：`m_money + 2` 指针算术形与 `&m_money[2]` 取址形
        // 产物逐字相同（4.4 恒 add $0x79678 + lea 0x4；ORIG add $0x79670 + lea 0xc）
        // ⇒ 常量分解差维持 N30/N4 封顶。
        localtime_r(
            (const time_t*)&m_moneyLogs[m_moneyLogIdx].m_money[2], &oldTm);
        // ORIG 0x0868f227/-0x28 与 0x0868f22a/-0x54：currentTm@-0x34/oldTm@-0x60 加 0xc
        // ⇒ 读的是 **tm_mday**（月内日），非 tm_yday（+0x1c 会落 -0x18/-0x44）。
        // 原 tm_yday 属真语义 bug：跨月同日（如 1/5 对 2/5）yday 不同而 mday 相同，
        // 会导致本不该轮换的每日金钱日志被轮换。
        if (currentTm.tm_mday != oldTm.tm_mday)
        {
            m_moneyLogIdx = m_moneyLogIdx + 1;
            if (m_moneyLogIdx > 6)
            {
                m_moneyLogIdx = 6;
            }
            else
            {
                return;   // b28d-solo-CUser 试形（P285）：ORIG 0x868f2xx 赋值后保留
                          // `jmp <leave>` 体尾岛；4.4 对纯正向 if 恒 jump-thread。
            }
        }
    }
}

// ORIG _ZN5CUser21exceptedStateMoneyLogEv T 0x0868f3ba：组队村庄怪标记/搬运副本/
// 状态 8/村庄 7 任一命中 → 跳过货币滚窗。
#include "CUser/exceptedStateMoneyLog.inc"
{
    // ORIG T 0x868f3ba 全录（报告缺失，objdump 71 insn 转写）：GetParty() 每使用点
    // 重调共 4 次（0x868f3c6/3da/3fd/417，无具名槽，T-CU15）；条件全走 4.1 bool
    // 物化 `test;setne %al;test %al,%al;je`（技巧 22 register bool al）；m_89e
    // uchar `movzbl 0x89e;test %al` 免物化直判；get_state `cmp $0x8,%eax; sete`、
    // getCurCharacVill `cmp $0x7,%al` 8 位形（callee uchar 返回）。bf 读保留
    // CParty+0xb24 视图（§4.3 具名，0xc4c 单级系 4.1 复合折叠，4.4 两段为 N）。
    // b28d-solo-CUser AE: ORIG 0x868f3ba 全段 `setne %al; test %al,%al; je` / `movzbl
    // 0x89e; test %al,%al` 均无槽（sub $0x18）；具名 gp/bfNZ/m89e/st8/vil7 各落 -0x9..-0xd
    // 使帧 +0x10 ⇒ 全部内联条件（probe_bool a3/a5）。GetParty() 每使用点重调（T-CU15）。
    if (GetParty() != 0)
    {
        if ((*(&((CParty*)GetParty())->m_battleField)).m_villageMonsterId != 0)
            return true;
        if (((CParty*)GetParty())->m_battleField.m_pDungeon != 0)
        {
            // b28d-PoolCUser 定性（2026-09-09，Main 裁决：字段**保持 uchar**，驳回
            //   bool 化提案）。决定性 producer 直证：ORIG
            //   _ZN8CDungeon11set_dungeonER15STDungeonScript @0x0834b39b..0x0834b3a5
            //   = `movzx edx,BYTE [eax+0x28d]` → `mov BYTE [eax+0x89e],dl` 裸逐字节
            //   搬运（若 bool，4.1 必产 test+setne 再存）；全二进制 +0x89e 消费普查
            //   15 处 = 14× `movzx BYTE[..+0x89e]` + 1× 上述 store，零处 `test BYTE
            //   PTR` ⇒ ORIG 本调用点 `test %al,%al; je`（0x0868f429 后）只是 movzx
            //   装载后的寄存器直测（uchar 同形），**不构成 bool 证据**（P321 T3 修正）。
            //   残差真实成因 = **跨版本 uchar 测试物化 lowering 差**：4.1 直分支
            //   `movzbl;test;je` 三段，4.4 恒插 `setne %al` 四段（探针 probe_uc2/
            //   uc3/uc4 三形实测不可复现；return 1/!=0/扁平 goto 均无效）——按
            //   N7/N18b 装载-物化族封顶。
            // b28d-solo-CUser 试形：uchar 字段显式 `!= 0`（P292 反向探测；ORIG
            //   `movzbl 0x89e; test %al,%al; je` 无 setne，字段不可 bool 化——
            //   CParty.cpp 多处 `!= 1` 消费依赖 >1 值域）。
            // b28d-solo-CUser 试形（P292 档②裸 if）：ORIG 0x868f429 `movzbl 0x89e; test %al,%al; je`
            // 无 setne ⇒ 源形是裸 if(字段)（非 `!= 0` 显式比较）。
            if (((CParty*)GetParty())->m_battleField.m_pDungeon->m_89e)
                return true;
        }
    }
    if (get_state() == 8)
        return true;
    if (getCurCharacVill() == 7)
        return true;
    return false;
}


#include "CUser/UseWinPoint.inc"
{
    useWinPoint(winPoint, reason);
    if (winPoint != 0)
    {
        m_historyLog.WPSub(getPVPResultR()->m_winPoint, winPoint, reason);
    }
    return getPVPResultR()->m_winPoint;
}

// ORIG _ZN5CUser17AddSecuRewardItemEh T 0x08688328（0x14 字节）：仅参数存
// 未使用局部（ORIG 即空操作）。
#include "CUser/AddSecuRewardItem.inc"
{
    // ORIG 0x868832b sub $0xa4 + 0x8688334 mov %al,-0xa4(%ebp)（4.1 把 arg→local 复制折叠为单条）。
    // 4.4 双槽（probe17）；big[0x9c] 垫 0xa4 帧。local 初始化多余——删，仅留 arg  spill。
    char big[0x9c]; (void)big;
    (void)rewardType;
    // P139 prett 缺口（P5 父泳道点名）：ORIG .rodata 有
    //   `_ZZN5CUser17AddSecuRewardItemEhE19__PRETTY_FUNCTION__` @0x8cf2660 size=0x2d
    //   （串 "void CUser::AddSecuRewardItem(unsigned char)"，逐字节同）；
    //   全库 ORIG 反汇编 0 处引用它（DEAD 串），但 4.1 为该函数发射了它 ⇒ 我方
    //   也须发射以对齐符号形态。sizeof 是编译期量，**零指令/零行为影响**（/tmp/P5/p_prett
    //   探针：加了 sizeof 后函数仍是 0x14 字节逐条同，且 emit 该 prett 符号）。
    (void)sizeof(__PRETTY_FUNCTION__);
}

// ORIG _ZN5CUser24GenerateCleanPadVeriDataERtRhS0_ T 0x08649ac4：生成三段
// CleanPad 验证种子（ushort randInt(0xffff)+1 / uchar randInt(6) /
// ushort randInt(0xffff)+1）存 m_mcap 并回填出参。
#include "CUser/GenerateCleanPadVeriData.inc"
                                     unsigned short& outC)
{
    m_mcap.m_cleanPadVeriA =
        static_cast<unsigned short>(get_rand_int(0xffff) + 1);
    m_mcap.m_cleanPadVeriB = static_cast<unsigned char>(get_rand_int(6));
    m_mcap.m_cleanPadVeriC =
        static_cast<unsigned short>(get_rand_int(0xffff) + 1);
    outA = m_mcap.m_cleanPadVeriA;
    outB = m_mcap.m_cleanPadVeriB;
    outC = m_mcap.m_cleanPadVeriC;
}

// ORIG _ZN5CUser15reqHumanCertifyEv T 0x0867f26e（0x252 字节）：未认证时发起
// CleanPad 人工认证——记日志/暴露计数、从验证队列随机取一条填 0x7d 包
// （byte 5 + int + binary + 认证种子三段 + 错误计数 + 0），发送后记 tick。
// [b28d-GrpD 判词·等价 N] ORIG 0x0867f26e：唯一实差异是 put_binary 实参照搬对中
//   的寄存器选择（ORIG `mov 0xa(%eax),%eax; mov -0xc(%ebp),%edx; add $0xe,%edx` vs
//   我方 `mov 0xa(%eax),%edx; mov -0xc(%ebp),%eax; add $0xe,%eax`）—— 同址同值等价；
//   其余为未解析重定位。字段偏移（msg+0x2/+0xa、m_capchaCertify 0xa 字节）与
//   GenerateCleanPadVeriData 三出参序全一致 ⇒ 无语义缺口。
#include "CUser/reqHumanCertify.inc"
{
    if (!isHumanCertified())
    {
        return false;
    }
    m_historyLog.RequestCleanPad(m_mcap.getPoint(), 0);
    m_mcap.incExposedCount();
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return false;
    }
    MSG_UDP_CAPCHA_CERTIFY* msg = GlobalData::s_capchaDataManager.peekData();
    memcpy(m_capchaCertify, msg->m_text, 0xa);

    PacketGuard pg;
    pg.put_header(0, 0x7d);
    pg.put_byte(5);
    pg.put_int(msg->m_size);
    // ORIG 0x867f366-0x867f376：len→%eax、ptr→%edx —— 无具名槽（P0：msg->m_size
    // 表达式直传；历史 register asm 钉法已按 P0/N96 清绑，改内联实参）。
    pg.put_binary((const char*)msg->m_image, msg->m_size);
    pg.put_byte(m_mcap.getPoint() > 0x251b ? 1 : 0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    pg.put_short(veriA);
    pg.put_byte(veriB);
    pg.put_short(veriC);
    pg.put_byte((unsigned char)m_humanCertifyErrorCnt);
    pg.put_byte(0);
    pg.finalize(true);
    Send(pg);
    m_mcap.m_capchaReqTick = GlobalData::s_systemTime_.getCurTickCount();
    return true;
}

// ORIG _ZN5CUser22sendNotiPcRoomPlayTimeEv T 0x0868f83c：包 0x1a8（网吧时长通知）。
#include "CUser/sendNotiPcRoomPlayTime.inc"
{
    PacketGuard pg;
    pg.put_header(0, 0x1a8);
    pg.put_int(getTotalPcRoomPlayTime());
    pg.finalize(true);
    Send(pg);
}

// ORIG _ZN5CUser12SendCashDataEb T 0x0865082a：包 0x35（商城货币数据；
// flag=1 时全 0，否则 Cera/Mileage/CeraPoint）。
#include "CUser/SendCashData.inc"
{
    PacketGuard pg;
    pg.put_header(0, 0x35);
    pg.put_byte(1);
    if (flag)
    {
        pg.put_int(0);
        pg.put_int(0);
        pg.put_int(0);
    }
    else
    {
        pg.put_int(GetCera());
        pg.put_int(GetMileage());
        pg.put_int(GetCeraPoint());
    }
    pg.finalize(true);
    Send(pg);
}

// ORIG _ZN5CUser15recover_staminaEv T 0x0865784e：满体力外每分钟恢复
// （+9，钳 100）并回发包 0x4。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865784e | _ZN5CUser15recover_staminaEv
#include "CUser/recover_stamina.inc"
{
    // ORIG 0x8657859-0x8657874：两次独立装载 `cmp $0x2,%eax; jle` / `cmp $0x5,%eax; je`，
    // 均落 0x865797d `mov $0,%eax` dead 岛后 fall epi（P0/N96：dead eax 置零与
    // 尾 dead 读为 4.1 寄存器遗留，4.4 无源码等价发射，去绑定后记 N 封顶）。
    if (m_chState <= 2 || m_chState == 5)
        goto RET0;
    {
        if ((bool)(getCurCharacR()->m_stamina <= 0x63))
        {
            addCurcharacStamina(9);
            // ORIG 0x86578b9-0x86578c0：cmp $0x64,%al; seta 物化。
            if ((bool)(getCurCharacR()->m_stamina > 0x64))
            {
                setCurCharacStamina(0x64);
            }
            PacketGuard pg;
            pg.put_header(0, 0x4);
            pg.put_byte(getCurCharacR()->m_stamina);
            pg.finalize(true);
            Send(pg);
        }
        // ORIG 0x8657969-0x8657978：pg dtor merge 后 dead 读（技巧 16 原地双零扩展）；
        // 两分支共落。
        // ORIG 0x8657969 尾：call getCurCharacR + `movzbl 0x75(%eax),%eax;
        // movzbl %al,%eax` = 成功路返回 stamina 字段（第二 movzbl 为 4.1 int
        // 返回截断产物，4.4 单段零扩展，残差 1 条记跨版本 N）。
        return getCurCharacR()->m_stamina;
    }
    goto RET0;   // 不可达守卫镜像（保持 ORIG 块序：成功 fall 在前、mov$0 岛在后）
RET0:;
    return 0;
}

// ORIG _ZN5CUser13WorkPerTenMinEv T 0x08653196：无角色时 trace；有激活角色
// 则向监视代理上报角色在线（characNo）。
#include "CUser/WorkPerTenMin.inc"
{
    if (m_chState <= 2)
    {
        return;
    }
    if (getCurCharacR() == 0)
    {
        // ORIG 0x86531f4-：NumberToString 两 call 先执行、结果暂 %ebx，cMyTrace
        // 临时构造成其后（0x8653215 mov %eax,%ebx 证据）→ 临时链式（技巧 3）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d71, 5)(
            "CUser::WorkPerTenMin() m_id(%s) character - m_selected is null",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_charac_memberkey() != 0)
    {
        GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendNoticeCharLive(getCurCharacNo());
    }
}

// ORIG _ZN5CUser14WorkPerFiveMinEv T 0x08652f0c：五/十分钟公会上报族
// （一般经验与今日经验两段同构）；无角色时 trace 返回。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08652f0c | _ZN5CUser14WorkPerFiveMinEv
#include "CUser/WorkPerFiveMin.inc"
{
    // 2026-09-10（G3CUser-B 复验）：槽集对照 ORIG 13 槽 / 我方 13 槽、**零平移零多余**
    //   （insn 176:176）⇒ 具名局部已与 ORIG 一一对应，残余 ndiff 纯属跨版本寄存器/
    //   指令选择形（P227 链式 trace 已落地），判 N 封顶，不动。
    if (m_chState <= 2)
    {
        return;
    }
    if (getCurCharacR() == 0)
    {
        // ORIG 0x08652f0c 段（P227 全表达式临时链）：NumberToString 结果先升格 %ebx
        //   跨 cMyTrace ctor 保活（`mov %eax,%ebx` 在 ctor 实参装载前排），
        //   两语句具名形会把 ctor 提到求值之前 ⇒ 必须临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d24, 5)(
            "CUser::WorkPerFiveMin() m_id(%s) character - m_selected is null",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_guild_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_exp();
        // 2026-09-11 L9GrpF（P211/P324 store 序）：ORIG `movb $0x0,-0x2d` 在
        //   `movzbl 0x796d9(%eax),%eax` **之前** ⇒ 常量 store 语句在字段读之前。
        packet.m_expType = 0;
        packet.m_notify = m_field796d9;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_exp();
    }
    if (get_guild_today_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_today_exp();
        // 同 #3a（ORIG 第二段 `movb $0x2,-0x49` 亦在字段读之前）。
        packet.m_expType = 2;
        packet.m_notify = m_field796d9;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_today_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_today_exp();
    }
}

#include "CUser/get_server_fatigue_day_size.inc"
{
    return (int)m_map8eb50.size();
}

#include "CUser/IncreaseUsedCoinCount.inc"
{
    *(int*)((char*)&m_field8eb1c) = *(int*)((char*)&m_field8eb1c) + 1;
}

#include "CUser/setLoadHackGold.inc"
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    m_mcap.m_loadingHackGold = flag;
}

#include "CUser/SetNonClientFlag.inc"
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    m_piSender.m_nonClientFlag = flag;
}

#include "CUser/setHumanCertified.inc"
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    *(bool*)((char*)&m_humanCertified) = flag;
}

#include "CUser/incTradeCount.inc"
{
    m_tradeCount = m_tradeCount + 1;
}

#include "CUser/GetRating.inc"
{
    return m_rating;
}

#include "CUser/backupSeed.inc"
{
    m_backupSeed = m_checkSumSeed;  // ORIG 0x859506f: *(this+0x796e4) = *(this+0x796e0)
}

#include "CUser/ResetCharacExpandData.inc"
{
    m_characExpand.reset();
}

#include "CUser/IsExistGuildAgit.inc"
{
    return m_guildDB.m_agitExist != 0;   // +0x8d0be（成员视图折叠单指令 movzbl）
}

#include "CUser/GetDailyBadge.inc"
{
    return m_dailyBadge[type];
}

// ORIG @0x08568e0c（W）：out[i]=m_dailyBadge[i]，i=0..2
// （ORIG 寻址 (i+0x47040)*2+4 即 m_dailyBadge 逐字读）。
#include "CUser/GetDailyBadge_1.inc"
{
    for (int i = 0; i <= 2; ++i)
        out[i] = m_dailyBadge[i];
}

// ORIG @0x08681c30。On Time Event 奖励发放：
// 双日志 ./log/%s/OnTimeEvent → itemNo==0 短路返 0 → 包头(1,0x120)+byte1+
// get_index(记录未用)+byte1 → find_item(itemNo)（无表项返 0）→
// create_item(NONE,item,count)→有效性(m_addInfo||m_addInfo2)→插背包
// （reason 0xd；失败系统邮件 msg_132/133 + item.reset，成功 SendUpdateItem）
// → put_int/put_short → finalize 发送，返 1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681c30 | _ZN5CUser23InsertOnTimeEventRewardEjj
#include "CUser/InsertOnTimeEventReward.inc"
{
    // b28d-solo-CUser p167：ORIG 0x08681c30 内联 rep stos（我方 EXTRA memset ×2）。
    char logDir[256] = {0};
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x08681cbb `call get_charac_no;
    //   mov %eax,%esi`（characNo **无槽**、%esi 保活）与 0x08681ccd..0x08681cdd
    //   `call get_acc_id; call NumberToString; mov %eax,%ebx`（accStr **无槽**、%ebx
    //   保活）——O0 下具名标量必占栈槽，ORIG 无槽 ⇒ 原码把两调用直接写在实参位
    //   （cdecl 右→左：`%u` 的 get_charac_no 先算、`%s` 的 NumberToString 后算，
    //   与 ORIG 0x08681cbb→0x08681cdd 次序一致）。删我方 characNo/accStr 两具名量
    //   （其 store/load 另致帧 0x2dc→0x2ec、+0x10）。
    {
        CMyFileLog log(
            __PRETTY_FUNCTION__,
            0x869f);
        log(logDir, "On Time Event:InsertOnTimeEventReward(%s, %u)",
            NumberToString(get_acc_id(), 0), get_charac_no(-1));
    }

    if (itemNo == 0)
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x120);
    packet.put_byte(1);
    int pktIndex = packet.get_index();   // ORIG 记录后未再使用
    (void)pktIndex;
    packet.put_byte(1);

    // 2026-09-10（G3CUser-B，语句位置轴）：ORIG 0x08681d98 的 find_item 检查位于
    //   `put_byte(1)`（0x08681d76）与 Inven_Item 默认构造（0x08681daa）**之间**，
    //   而 put_header/put_byte/get_index/put_byte 四步在 0x08681d3a..0x08681d76 先行；
    //   我方原先把本检查放在 PacketGuard 构造之前 ⇒ 整段顺序与 ORIG 逆置。位置照抄。
    //   ORIG 0x08681da0 `cmpl $0x0,-0x20(%ebp); je 0x0868201c`（返回 0，不发包）。
    // ORIG 0x08681da0 `mov %eax,-0x20(%ebp); cmpl $0x0,-0x20(%ebp); je`：具名指针槽，
    // 非 `sete` 布尔物化（我方 `sete %al; test %al,%al` 多 2 条）。
    CItem* pItem = G_CDataManager()->find_item((int)itemNo);
    if (pItem == 0)
        return 0;

    {
        Inven_Item item;                 // 默认构造（ORIG ctor 0x080cb854）
        item.m_addInfo = (int)itemNo;    // +0x02 预填 item index（ORIG -0x88 写）
        // b28d-solo-CUser p167：ORIG 0x08681c30 直接读 `dm+0xc` 成员
        //   （`CItemList::create_item(*(CItemList**)(dm+0xc), ...)`，无 getItemList
        //   调用；我方 EXTRA ×1）⇒ 改具名成员访问。
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)0, item, (int)itemCount);

        // ORIG 0x08681dd4..0x08681e10 全程无 bool 槽（`test %al,%al; je` 直判）
        // ⇒ 删具名 give（我方多 `mov %al,-0x25(%ebp); cmpb $0x0,-0x25(%ebp)`）。
        if ((item.m_addInfo != 0) || (item.get_add_info() != 0))
        {
            int slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)0xd, true, true);
            if (slot < 0)
            {
                char title[20];          // strncpy(src, 0x14)，不补零
                // ORIG 0x08681f??：findString 结果**不经具名槽、无判空**，直接作
                // strncpy 第二实参（我方多 `mov %eax,-0x20; cmpl $0x0,-0x20; je` 段）。
                strncpy(title,
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_132", (bool*)0),
                        0x14);

                // ORIG 无 msg 零初始化（P171：ORIG 不写即删；strncpy n=0xff 自补零）
                // —— 我方 `= {0}` 整段 4.4 慢路清零（and $0x1/and $0x2/rep stos/尾）为
                // 多余 21 条。sMsg 同样去具名与判空。
                char msg[256];
                strncpy(msg,
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_133", (bool*)0),
                        0xff);

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0, (unsigned int)getCurCharacNo(),
                    msg, (int)strlen(msg), 0, GetServerGroup(), false, false);

                item.reset();
            }
            else
            {
                SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
            }

            packet.put_int((int)itemNo);
            packet.put_short((short)itemCount);
        }
    }

    packet.finalize(true);
    Send(packet);
    return 1;
}

#include "CUser/get_charac_count.inc"
{
    return (int)m_characList.size();
}

#include "CUser/isTradePunishOverThirtyDays.inc"
{
    return m_postChecksumData.m_tradePunishOverThirtyDays;   // +0x7962c（ORIG movzbl 直返；原误读 m_firstLogin）
}


#include "CUser/sendBlueMarbleEnterCount.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1b4);
    packet.put_byte(getBlueMarbleEnterCount());
    packet.finalize(true);
    Send(packet);
}

#include "CUser/ResetDailyCharacExpandDataMidnight.inc"
{
    m_characExpand.ResetDailyMidnight();
}

#include "CUser/ResetDailyCharacExpandData.inc"
{
    m_characExpand.ResetDaily();
}

#include "CUser/resetNPCRelationShipDailyData.inc"

#include "CUser/sendNPCRelationShipFavor.inc"

#include "CUser/SendProperDungeonClearCount.inc"
{
    // ORIG 0x8681833-0x868183b：`cmp $2; setle; test; jne skip` = state<=2 早退
    // （body 在 state>2 执行，此前极性写反已修正）；0x868188a `mov %ax,-0xa;
    // movswl -0xa,%eax` = short 具名槽（pg 基址随之平移 -0x18）。
    if (get_state() <= 2)
        return;
    if (!getCurCharacR())
        return;
    PacketGuard packet;
    packet.put_header(0, 0x10b);
    short cnt = GetProperDungeonClearCount();
    packet.put_short(cnt);
    packet.finalize(true);
    Send(packet);
}

#include "CUser/acceptable_within_mission.inc"
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    // ORIG 0x8??：grade 走 %ebx（`mov %eax,%ebx` 技巧 2），`mov %ebx,0x4(%esp)` 直传；
    // mission=-0x10 / idx=-0xc。b28d-solo-CUser：裸 register 复现 ebx 保活（P231）。
    register int grade = get_pvp_grade();
    int idx = G_CDataManager()->get_WithinMissionIndex(grade);
    if (idx == 0)
        return false;
    return mission->MakeMissionList_JustKind(*this, 0x1b);
}

#include "CUser/isDuplicationMessage.inc"
{
    // ORIG 0x08688826：比较实参 = `lea 0x79664/0x79668(%eax),%edx` + `mov %eax,
    // 0x4(%esp)` ⇒ **m_str[i] == msg**（str 在左占 this 槽，原 msg==str 反序）；
    // ORIG -0x9 槽 = dup 结果槽；索引无槽——`movzbl 0x7966c; movsbl %al,%eax`
    // 原地（(char) 有符号 cast 内联，具名 idx 局部多占槽致 dup 漂到 -0xa）。
    bool dup = (m_str79664[0] == msg) || (m_str79664[1] == msg);
    m_str79664[(char)m_dupMsgSlotIdx] = msg;   // 写址 this+0x79664+idx*4（Ghidra (idx+0x1e598)*4 换算）
    m_dupMsgSlotIdx = m_dupMsgSlotIdx ^ 1;   // ORIG 0x8688?? `mov 0x8; movzbl 0x7966c` 重读成员
    return dup;
}

#include "CUser/DimensionInoutUpdate.inc"
{
    // ORIG i 槽 -0xc（probe：4.4 首声明 4B 局部占 -0x10；前置 dead int 占远槽
    // -0x10 后 i 平移至 -0xc，技巧 31）。
    int deadpad;
    (void)deadpad;
    for (int i = 0; i <= 5; i++)
    {
        if (flag1)
        {
            // ORIG：value 无槽——call 结果原地 `movsbl %al,%ecx`（右实参先评估）；
            // i 次之 `mov -0xc,%eax; movsbl %al,%edx`。
            setDemensionInoutValue((char)i,
                                   (char)G_CDataManager()->get_dimensionInout(i));
        }
    }
    if (flag2)
    {
        setUltimateInoutValue(2,
                              (char)G_CDataManager()->get_limit_inout_count(
                                  (ENUM_BLOOD_TYPE)2));
    }
}

#include "CUser/send_pvp_record.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x30);
    // ORIG：getPVPResultRefR 每处 put 独立重调 7 次——访问器结果禁缓存
    // （战术 P183/P48）。
    packet.put_int(getPVPResultRefR()->m_totalWinCount);
    packet.put_int(getPVPResultRefR()->m_totalLoseCount);
    packet.put_int(getPVPResultRefR()->m_expPoint);
    packet.put_int(GlobalData::s_ref_pvp_grade
                       ->GetPvpCurrRankPoint(getPVPResultRefR()->m_pvpGrade));
    packet.put_int(GlobalData::s_ref_pvp_grade
                       ->GetPvpNextRankPoint(getPVPResultRefR()->m_pvpGrade));
    packet.put_byte(getPVPResultRefR()->m_pvpGrade);
    // ORIG 0x: `mov 0x5c(%eax),%eax` 32 位直读 m_todayWinCount(int@+0x5c) 后
    // 交 put_byte(int)——用字节指针 `*(unsigned char*)&` 会被 4.4 折成
    // add+movzbl 载低字节（多 3 条），改传 int 直读对齐 ORIG。
    packet.put_byte(getPVPResultRefR()->m_todayWinCount);
    packet.finalize(true);
    Send(packet);
}

#include "CUser/send_clear_quest_list.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x163);
    packet.put_int(0x7530);
    // ORIG 0x08??：quest/cleared 均无槽——`call getCurCharacQuestR; add $0x4,%eax;
    // mov %eax,(%esp); call getClearedQuest` 全链式（技巧 37/36：具名指针中转 =
    // 槽膨胀主因，pg 基址 -0x1c 应回 -0x14）。
    packet.put_binary(
        (const char*)reinterpret_cast<WongWork::CQuestClear*>(
            (char*)getCurCharacQuestR() + 4)->getClearedQuest(),
        0x7530);
    packet.finalize(true);
    Send(packet);
}

#include "CUser/processReturnUserQuestAutoClear.inc"
{
    if (IsReturnUser() && IsReturnUserFirstLogin())
    {
        questAutoClear(get_charac_level() - 1);
    }
}

#include "CUser/update_pvp_rank.inc"
{
    getPVPResultRefW()->AddNewResult(result);
    return true;
}

#include "CUser/saveTaxMoneyForUpperMember.inc"
{
    // b28d-solo-CUser AE: ORIG 0x864f8fb-0x864f902 `sete %al; test %al,%al; jne`
    //   （P292 非 bool 返回的 ==0 物化）；具名 mk0 落槽（mov %al,-0x9; cmpb $0,-0x9）
    //   ⇒ 删局部内联条件（probe_bool a3）。2/3 守卫直折 test %al/je + cmpl/jle。
    if (get_charac_memberkey() == 0)
        return;
    // ORIG 0x864f90f-0x864f917 正向嵌套（技巧 19）：`test %al,%al; je` /
    // `cmpl $0x0,0xc; jle` 均直落 leave，无 xor/中转 jmp。
    if (is_connect_upper_member())
    {
        if (money > 0)
            add_member_pay_tex_money_to_upper(money);
    }
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fa08 | _ZN5CUser20gainExpAsUpperMemberEi
#include "CUser/gainExpAsUpperMember.inc"
{
    // b28d-solo-CUser 复测：level/rate/result 全头部声明（P212/P309，深→浅序）实测
    // 槽位逐字不变（level -0x12 / rate -0x10 / result -0xc）⇒ 4.4.6 按首次使用序
    // （N98），ORIG 的 level@-0x1a 系 4.1 槽基差；维持 N82/P112 封顶。
    if (get_charac_memberkey() == 0)
        return 0;
    // ORIG 0x0864fa08：connect 块 fall-through（技巧59），未连接尾块 set(0)+ret0；
    // 率常量 = 0.01f（@0x08cf3958 = 0x3c23d70a，此前 0.1f 为语义错）；
    // result<=0 亦调 set_charac_member_bonus_exp(0)（ORIG 3 call 点）。
    if (is_connect_upper_member())
    {
        // b28d-lane5：ORIG 0x0864fa2x `movzbl %al,%eax; mov %ax,-0x1a; filds -0x1a`
        // ⇒ 原码局部为 **short**（非 uchar，P0 槽往返 + 宽度证据）；率计算
        // `fstps -0x10` 具名 float 局部（P252）后再 `fildl exp; fmuls -0x10`。
        short level = get_uppermember_exp_level();
        float rate = level * 0.01f;
        int result = (int)(exp * rate);
        if (result > 0)
            set_charac_member_bonus_exp(result);
        else
            set_charac_member_bonus_exp(0);
        return result;
    }
    set_charac_member_bonus_exp(0);
    return 0;
}

#include "CUser/gainPowerWarRewardExp.inc"
{
    return 0;
}

#include "CUser/add_guild_point_item.inc"
{
    int threshold = G_CDataManager()->m_guildParameter.m_field4;   // ORIG 0x865ccd9 槽 -0xc
    // ORIG 0x865cce8 cmp -0xc(%ebp),%eax; setl;test;je skip：call 结果寄存器直参与比较
    // （不物化 rand 局部，probe21 f 实证复现；rand 入槽反产 spill+reload 三行）
    if (get_rand_int(0x2710) < threshold)
        add_inventory_item(0xc84);
    return;   // ORIG 0x865cd05 body 尾 jmp merge + 0x865cd07 nop 岛（技巧 7 尾置 return）
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c844 | _ZN5CUser19SendMoneyFullReasonE22ENUM_MONEY_FULL_REASONmm
#include "CUser/SendMoneyFullReason.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x33);
    packet.put_byte(reason);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    return Send(packet);
}

#include "CUser/AddDungeonClear.inc"
{
    // ORIG 0x086??：getClearedDungeonDiff 结果 movsbl %al,%eax 原地单段（无 char
    // 槽物化）+ 比较 setl %al;test;je 尾物化岛（技巧 22 register bool al，直折
    // jge 反形）；残差 movswl %ax,%ebx（4.1 arg 截断 sext 直落 ebx）N18a。
    // 二修：getter 结果直用免槽（技巧 37，ORIG call 后 %eax 即 this 实参）、
    // 正向 `if(less)`（`!less` 反产 xor$1 取反）。
    // b28d-solo-CUser AE: ORIG `cmp; setl %al; test %al,%al; je` 无槽；具名 less 落 -0x9
    // 使帧 +0x10 ⇒ 内联条件（probe_bool a6/a7）。
    if (getDungeonClearRefR()->getClearedDungeonDiff(dungeonIdx) < clearCount)
    {
        getDungeonClearRefW()->addClearedDungeon(dungeonIdx, clearCount);
        return true;
    }
    return false;
}
#include "CUser/SendConditionEventInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xea);
    CConditionEventManager* pMgr = G_CGameManager()->GetConditionEventManager();
    if (pMgr)
    {
        int eventIdx = pMgr->GetCurEventIndex();
        // ORIG 0x868195d/0x868196b：`mov %al,-0xa/-0x9`（char 槽直存）+ movsbl 读
        // ——局部为 char（signed byte），put_byte 传参符号扩展。
        char step = (char)GetCurConditionEventStep();
        char rewardStep = (char)GetCurConditionEventRewardStep();
        packet.put_short(eventIdx);
        packet.put_byte(step);
        packet.put_byte(rewardStep);
        packet.finalize(true);
        Send(packet);
    }
}

#include "CUser/ResetCurCharacUsedGiftFatigueQuantity.inc"
{
    if (m_field8ebac != 0)
    {
        m_field8ebac = 0;
        SetChangedGiftFatigueQuantity(true);
    }
}

#include "CUser/SetChangedGiftFatigueQuantity.inc"
{
    m_field8ebb0 = flag;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08657570 | _ZN5CUser15ResetDailyQuestEv
#include "CUser/ResetDailyQuest.inc"
{
    std::list<int> questList;
    // ORIG 0x865753x：G_CDataManager()->m_questList 结果不留槽（技巧 37），
    // 直接 `mov %eax,(%esp)` 传 GetDailyQuestList——具名 pQuestList 反产 -0x10 spill
    // 使整帧平移 4（ORIG it=-0x18/end=-0x1c 证据）。
    G_CDataManager()->m_questList->GetDailyQuestList(questList);
    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        // ORIG 0x08657574：getCurCharacQuestRefR 链式调用 isClearQuest（无中间变量、不判空）
        if (getCurCharacQuestRefR()->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
}

// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x08657662 | _ZN5CUser18ResetTrainingQuestEv
#include "CUser/ResetTrainingQuest.inc"
{
    std::vector<int> questList;
    // b28d-lane5 复测（P0/P200）：ORIG 0x08657662 段——`call get_charac_level;
    //   movswl %ax,%ebx`（level **无槽**、拓宽直传 callee-saved）与
    //   `call G_CDataManager; mov 0x18(%eax),%eax; mov %eax,(%esp)`（m_questList
    //   **无具名指针槽**）⇒ 删 level/pQuestList 两个具名局部，表达式就位内联。
    G_CDataManager()->m_questList->GetTrainingQuestList(questList,
                                                       get_charac_level());
    for (std::vector<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        // b28d-solo-CUser p167 修正：ORIG 0x08657662 调 getCurCharacQuestRefR。
        // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x086576cb
        //   `call getCurCharacQuestRefR` 的返回值 %eax **直传** isClearQuest 的 this
        //   （0x086576d0 `mov -0xc(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp);
        //   call isClearQuest`），全程**无** `pQuest` 具名槽、**无** `test %eax,%eax /
        //   je` 判空，也无 `&&` 的 setcc 物化段 ⇒ 删除我方具名 pQuest 与判空。
        if (getCurCharacQuestRefR()->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
}

// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x085df9b2 | _ZN5CUser11SetPvpIndexEs
#include "CUser/SetPvpIndex.inc"
{
    // ORIG 0x08669ee8：CSwitchLog("SetPvpIndex",0x2339,0,0) 记
    // "pvp@log %s,%s,room(%d)"（acc_name/charac_name/idx）后加锁写 m_pvpRoomIdx。
    // b28d-PoolCUser 重写（2026-09-09，P227/N80）：ORIG 0x085df9b2 段内 varargs
    //   （get_acc_name→esi / getCurCharacName→ebx / idx movswl→edi）**先于 ctor**
    //   求值且跨 ctor callee-saved 保活 ⇒ 原码是全表达式临时链形（非两句具名）。
    // [CUserLane 2026-09-11] P139：ORIG 有 r 段符号 _ZZN5CUser11SetPvpIndexEsE12__FUNCTION__
    //   （0x08cc2628/0x08cfd425，size 0xc）；手写串只生匿名 .rodata ⇒ 改 __FUNCTION__ 补符号。
    CSwitchLog(__FUNCTION__, 0x2339, 0, 0)(
        "pvp@log %s,%s,room(%d)", get_acc_name(), getCurCharacName(), (int)idx);
    lock();
    m_pvpRoomIdx = idx;
    unlock();
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0865cadc | _ZN5CUser18add_inventory_itemEj
#include "CUser/add_inventory_item.inc"
{
    // 2026-09-10（G3CUser-B 复验）：槽集对照 ORIG/我方 各 21 槽、零平移零多余
    //   （134:135 insn）⇒ 具名局部与 ORIG 一一对应，残余属跨版本 codegen（N 族），判 N。
    	CItem* pItem = G_CDataManager()->find_item(itemId);
	if (!pItem)
		return;
	Inven_Item invenItem;
	pItem->make_item(invenItem);
	invenItem.m_addInfo2 = 1;  // count
	invenItem.m_addInfo = itemId;  // item id
	// ORIG 0x0865cad2/0x0865cae6：slot 先以 -1 初始化（movl $0xffffffff,-0xc），
	// 调用回写后 `== -1` bool 物化比较（cmpl/sete/test/jne）。
	int slot = -1;
	slot = getCurCharacInvenW()->insertItemIntoInventory(
	    invenItem, (eItemAddReason)0x18, true, true);
	// b28d-solo-CUser：ORIG 0x0865cae6 `cmpl; sete %al; test %al,%al; jne`（bool 物化，
	//   P295 while 形复现；`(bool)(x==-1)` 直折 je 无 sete）。
	while (slot == -1)
		return;
	// ORIG 0x0865cb19：MakeItemPacket 成功则 finalize+Send（0xe/byte 0/short 1）
	PacketGuard packet;
	((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
	((InterfacePacketBuf*)&packet)->put_byte(0);
	((InterfacePacketBuf*)&packet)->put_short(1);
	// ORIG 0x0865cb??：`call; xor $1,%eax; test %al,%al; jne 尾` = if(!f()) goto
	// 尾出口反形（4.4 正向 if 生 test/je 无 xor）。
	if (!getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet))
		goto done;
	((InterfacePacketBuf*)&packet)->finalize(true);
	Send(packet);
done:;
}

#include "CUser/send_MissionList.inc"
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    mission->Send_MissionList(*this);
}

#include "CUser/send_aura_avatar_option.inc"
{
    if (getCurCharacR() == 0) return;
    PacketGuard packet;
    packet.put_header(0, 0x17a);
    packet.put_byte(3);
    for (int idx = 0; idx < 3; ++idx) {
        packet.put_byte((char)idx);
        int now = GlobalData::s_systemTime_.getCurSec();
        // [b23S3C] P48/P0：ORIG 双臂各重取 getCurCharacR（无 cur/expire 槽），
        // if/else 分支岛（P56/P198），put_int 两分支各一次
        if (now < getCurCharacR()->m_auraValues[idx])
            packet.put_int(getCurCharacR()->m_auraValues[idx] - now);
        else
            packet.put_int(0);
    }
    packet.finalize(true);
    Send(packet);
}

// ===================== GameStubs 迁移（ORIG 强符号 T；最小正确语义） =====================
// 全部签名经 nm -C /home/wangyilei/下载/df_game_r 核实。

// 0x086489f4（DisConnSig.md 汇编+反编译 C 全文核对）
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x086489f4 | _ZN5CUser10DisConnSigE11DISCONN_SIGbi
#include "CUser/DisConnSig.inc"
{
    // b28d-CUser p167 真缺口：ORIG 0x08648a1b 调 `Guard<Mutex>::C2EPS0_`（RAII），
    //   全函数无 Mutex::lock/unlock call；原手动 lock+3×unlock 多 4 个调用点。
    Guard<Mutex> lockGuard(&m_mutex2);   // ORIG this+0x8cfa0 实证

    // ORIG 0x08648a45..0x08648a63：持锁后立即经 Arad_DataManager::findGameScript
    // ("Arad_EventPeriodDataManager") 取周期事件管理并 delUserRewardData(this)，
    // 不受后文 socket/state 短路影响（无条件先执行）。
    {
        // 2026-09-10（FL2CUser 类型轴）：ORIG 0x08648a45 段 p167 无具名 string 局部
        //   （差集只有「我们多 1×`SsC2ERKSs` 拷贝构造 + 2×`SsD2Ev`」）；形参
        //   `_ZN4ARAD16Arad_DataManager14findGameScriptESs` 为**按值 std::string**
        //   ⇒ 原码直传字面量（临时经 elide 直达形参），无 name 局部。
        // 2026-09-10 gA（A 组）P183：ORIG 0x08648a45..0x08648a63 全链式 ——
        //   `call ARAD::Singleton<Arad_DataManager>::Get(); lea -0x6c(%ebp),%edx;
        //   mov %edx,0x4(%esp); mov %eax,(%esp); call findGameScript`，返回值再直接
        //   `mov 0x8(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call
        //   delUserRewardData` —— **无 adm/epdm 具名指针槽**（我方旧形多 -0x48/-0x44
        //   两槽 + 帧 0xd4c vs ORIG 0xd1c）⇒ 删两具名量、改单条链式表达式。
        ((ARAD::Arad_EventPeriodDataManager*)
             ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                 "Arad_EventPeriodDataManager"))
            ->delUserRewardData(this);
    }

    int sock = m_network.m_sock;   // ORIG this+0xe0
    int chState = m_chState;       // ORIG this+0x8cfc4
    if (sock < 0 || chState == 0)
    {
        // ORIG +0x137 trace（0x911）：无 socket / 未登录态仅留痕后返回。
        cMyTrace(__PRETTY_FUNCTION__, 0x911, 5)
            ("this %x,incre_id= %d, idx= %d, m_socket<0,m_ChState==NONE , "
              "DisConnSig %s, From %d",
              // [L9GrpF PATCH#1a 负结论] (int)(short)GetSlotIDX() 实测**零变化**
              //   （fn_diff 差异点 494→494、指令数 594 持平；4.4.6 已把返回值折叠）
              this, (int)GetIncreID(), (int)(short)GetSlotIDX(),
              // b28d-CUser p167：ORIG 0x08648ad9 直读 0x704ac（非 get_acc_id）。
              NumberToString(m_accId, 0), (int)sig);
        return;
    }

    if (IsProgLogout())
    {
        // ORIG +0x21c trace（0x917）：已在登出中。
        cMyTrace(__PRETTY_FUNCTION__, 0x917, 5)
            ("[Prog log out...]this %x,incre_id= %d, idx= %d, DisConnSig %s, From %d",
              // [L9GrpF PATCH#1b 负结论] 同 #1a：实测零变化（494→494）。
              this, (int)GetIncreID(), (int)(short)GetSlotIDX(),
              NumberToString(m_accId, 0), (int)sig);   // ORIG 0x08648d89 直读
        return;
    }
    SetProgLogout();

    // ORIG +0x240：GetPeerIP2 后按有无 IP 打两条 trace（0x939 无 IP / 0x937 有 IP）；
    // send/recv 计数 = +0x8cf0d/+0x8cf0c 双字节、+0x8cf08（m_lastPacketID）。
    char ip[16];
    bool haveIp = m_network.GetPeerIP2(ip, 0x10) != 0;
    {
        if (!haveIp)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x939, 0)
                ("from (%d) m_id[%s] uid[%d], send(%d:%d), recv(%d), "
                  "ip(GetPeerIP:NONE), dungeon(%d)",
                  (int)sig, NumberToString(m_accId, 0), (int)m_field704a8,
                  (int)m_pad8cf0c[1],
                  (int)m_pad8cf0c[0], m_lastPacketID,
                  getPlayingDungeonIndex());
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x937, 0)
                ("from (%d) m_id[%s] uid[%d], send(%d:%d), recv(%d), ip(%s), dungeon(%d)",
                  (int)sig, NumberToString(m_accId, 0), (int)m_field704a8,
                  (int)m_pad8cf0c[1],
                  (int)m_pad8cf0c[0], m_lastPacketID, ip,
                  getPlayingDungeonIndex());
        }
    }

    // ORIG +0x2c0：sig==5 时按 accId/ip 有无重映射断线原因（0x30/0x31/0x32）。
    int reason = (int)sig;
    if (reason == 5)
    {
        // b28d-CUser p167：ORIG 0x08648e17/0x08648e3e/0x08648e65 三处均直读 0x704ac。
        if (m_accId == 0 && ip[0] != '\0')
        {
            reason = 0x30;
            flag = true;
        }
        else if (m_accId == 0 && ip[0] == '\0')
        {
            reason = 0x32;
            flag = true;
        }
        else
        {
            reason = 0x31;
            flag = true;
        }
    }
    SendClientCrashDownLogToLogDB(reason);   // ORIG T 0x0864870e

    // ORIG +0x4a5：SetCurTime 后读小时（CommonTime+0x03，布局见 CommonTime.cpp
    // 注释：+0x00 year/+0x01 month/+0x02 day/+0x03 hour）∈(0x12,0x17) 且登录时长
    // (0,0x2a31) → 组 Ting 时段统计包发统计服务器（组 0，0x12 字节）。
    CommonTime nowTime;
    nowTime.SetCurTime();
    char hour = *((char*)&nowTime + 3);   // +0x03 m_hour（CommonTime.cpp 权威布局）
    if (0x12 < hour && hour < 0x17)
    {
        Packet_User_Ting_TimeCheck_Statistic_Add ting;
        int playSec = (int)(GlobalData::s_systemTime_.getCurSec() - GetLoginTime());
        // ORIG 0x08648efb `mov 0x704ac(%eax),%eax; mov %eax,-0x9c(%ebp)`：
        //   直读成员入死局部（P171 照抄），非 get_acc_id 调用。
        unsigned int accId = m_accId;
        (void)accId;
        if (0 < playSec && playSec < 0x2a31)
        {
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
            if (proxy != 0)
                proxy->SendPacket((char*)&ting, 0x12);
        }
    }

    if (sock == -1)
    {
        // ORIG +0x567：socket 已断 → 仅留痕。
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x971,
            "[CUser::DisConnSig] Already Disconnected! Socket(-1) ACCID[%s]",
            NumberToString(m_accId, 0));   // ORIG 0x08648f5e 直读
    }
    else
    {
        // ORIG +0x5b5：reason∉{6,0x30,0x31,0x32} → 先发包 199(int reason)。
        if (reason != 6 && reason != 0x30 && reason != 0x31 && reason != 0x32)
        {
            PacketGuard pkt;
            ((InterfacePacketBuf*)&pkt)->put_header(0, 199);
            ((InterfacePacketBuf*)&pkt)->put_int(reason);
            ((InterfacePacketBuf*)&pkt)->finalize(true);
            if (Send(pkt) != 1)
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x988,
                    "[CUser::DisConnSig] Fail to disconnect reason send: reason[%d]",
                    reason);
            }
        }
        // ORIG +0x640：ResetEpoll 失败 → errno 留痕（0x98e）。
        int err = G_EpollHandler()->ResetEpoll(sock);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x98e,
                "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                NumberToString(get_acc_id(), 0), sock, err, strerror(err));
        }
        // ORIG +0x730：组流(<<1<<uid)（0x991）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x991), true);
        // b28d-CUser p167：ORIG 0x08649181/0x086491ac 显式调 CStreamGuard::operator*
        //   （deEv）再 operator<<（P132 链保留；原 `guard <<` 无 operator* call）。
        (*guard)->operator<<((int)1);
        (*guard)->operator<<((int)GetUID());
        // ORIG +0x7c9：+0x8d0df != 1（= m_guildAgitDB.m_pad2[0]，语义未建模【推断】）
        // 时补崩溃统计 + HWSpec 上报。
        if (m_guildAgitDB.m_pad2[0] != (char)1)
        {
            if (!flag)
            {
                // ORIG +0x7f0：crash 观测计数（key=lastPacketID / dungeonIdx / party）。
                // b28d-CUser p167 真缺口：ORIG 0x086491ed/0x08649213/0x0864922c/
                //   0x0864925b **四次** G_GameWorld()+getStatisticsToObserveCrash()
                //   （P157/P183 逐使用点重调，不缓存 crash 指针）。
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incUserLastCrashPacketID(m_lastPacketID);
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incUserLastCrashDungeonID(getPlayingDungeonIndex());
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incAbnormalDownUserCount();
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incUserLastCrashPlayStatus(CheckInParty() ? 1 : 0);
            }
            // ORIG +0x883：Client_Spec 统计包（header 0x9ce/0xc06）：+0xa type=1、
            // +0xb count=hwSpec[1]、+0xc errCode=param、+0x10 errLine=最后登录频道、
            // +0x12 起 memcpy hwSpec[+4]（0xc/条 × count）；SendPacket 长
            // count*0xc+0x12（布局对照 shared/packet/include 权威头）。
            char* hwSpec = getClientHWSpec();
            unsigned char hwCount = (unsigned char)hwSpec[1];
            Packet_Client_Spec_Statistic specPkt;
            char* specBase = (char*)&specPkt;
            specPkt.m_type = 1;                                       // +0x0a
            specPkt.m_count = hwCount;                                // +0x0b
            specPkt.m_errorCode = param;                              // +0x0c（movl）
            specPkt.m_errorLine = (short)GetLastLoginChannelNo();     // +0x10
            memcpy((char*)specPkt.m_specs, hwSpec + 4, hwCount * 0xc);
            int specLen = (int)(unsigned short)(hwCount * 0xc + 0x12);
            CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)GetServerGroup());
            if (proxy != 0)
                proxy->SendPacket(specBase, specLen);
        }
        // ORIG +0x95a：队列 1 投递（在 if(+0x8d0df) 块之后，无条件执行）。
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
}

// 0x08665400：PvP 技能树发放主循环（givePvPSkillTree.md）：
// secondGrow∈[0..CurSecondGrow]、grade∈[a..pvp_grade]；脚本 miss 跳过；
// mode 1/2 只发树 0/树 1，mode 3 双树；b=true 且已有等级≥目标跳过。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08665400 | _ZN5CUser16givePvPSkillTreeEibi
#include "CUser/givePvPSkillTree.inc"
{
    // ORIG T 0x8665400 全录（报告双段读完）：mode 判断为 **param_3==1/2/3 三分支
    // 复制**（SkillR/W、FindSkill、growtype_skill 各 4 次调用，勿 hoist 指针/合并
    // 条件——旧 `c==1||c==3` 共享形 call 计数 17 vs ORIG 46）；内层 **iterator
    // 扫描**（begin/end/++/pt，每字段读独立 pt 调用共 16）；b 真才 FindSkill
    //（短路 `b && ...`），条件 `FindSkill < second`（FindSkill 先、second 后读）；
    // growtype 实参求值序 second→first→job→SkillW（Ghidra uVar1/uVar2 序）。
    for (int secondGrow = 0;
         secondGrow <= (int)(signed char)getCurCharSecondGrowType(); ++secondGrow)
    {
        for (int grade = a; grade <= get_pvp_grade(); ++grade)
        {
            std::vector<std::pair<int, int> >* tree = 0;
            // b28d-solo-CUser 试形：正条件 if 包裹（P214①/P59）——ORIG `call; test
            //   %al,%al; je <循环尾>`（无 xor 段）⇒ 原码是 `if (fn(...)) { body }`，
            //   非 `==0 continue`（后者 4.4 产 xor$1;test;jne 反向形）。
            if (G_CDataManager()->m_pvpSkillTree.getPvPSkillTree(
                    get_charac_job(), (int)(signed char)getCurCharFirstGrowType(),
                    secondGrow, grade, b, &tree) != 0)
            {
            for (std::vector<std::pair<int, int> >::iterator it = tree->begin();
                 it != tree->end(); ++it)
            {
                if (c == 1)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)0) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)0);
                    }
                }
                else if (c == 2)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)1) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                else if (c == 3)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)0) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)0);
                    }
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)1) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
            }
            }
        }
    }
}

// 0x0867a95c

// 0x086804ce
// ORIG：断线回调。state==7（PvP 房间）时通知 PvP_Room::Disconnect；
// 在袭击区域（GetAssaultPlace != 0）时通知 CAssaultMgr::OnLeaveAssaultPlace(user, false)。
#include "CUser/OnDisconnect.inc"
{
    if (get_state() == 7)
    {
        // ORIG 0x08??：pvpIdx 走 %ebx（`movswl %ax,%ebx` 单条跨 G_CGameManagerv
        // 存活）。short→int 拓宽必须作为 GetPvp 首实参**直传**（隐式转换，同
        // getBlueMarble probe s3）；`register int asm("ebx")` 中间变量反产 cwtl+mov。
        // pRoom 槽落 -0xc（无 short spill，技巧 21）。末段 ORIG 对
        // GetInstanceAssaultMgr 结果**无判空**、%eax 直用（技巧 37）。
        PvP_Room* pRoom =
            G_CGameManager()->GetPvp(GetPvpIndex(), this, 0);
        if (pRoom != 0)
            pRoom->Disconnect(this);
    }
    if (GetAssaultPlace() != 0)
        pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(this, false);
}

// 0x0867a95c（返回 1 成功 / 0 角色为空；DisPatcher_DebugCommand 以 !=1 判失败）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867a95c | _ZN5CUser14SetCharacLevelEi
#include "CUser/SetCharacLevel.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（458 vs 461）：ORIG 0x0867a9cc..0x0867a9d6 条件为
    //   `cmp; setl %al; test %al,%al; je <else>`（4.1 布尔物化），4.4 对同一比较恒产
    //   `cmp; jge <else>` 直跳（P197④ 镜像族，setcc/直跳不可互换）；另我方帧 0x90 vs
    //   ORIG 0x9c 属槽分配差 ⇒ 判 N，保留现三目源形。
    // 0x0867a95c 主分支（SetCharacLevel.md）：等级钳制[1,200]；按方向取
    // GetSpAtLevelUp 计算 SP 增量加到树 0/1 并 clamp；exp=get_level_exp；
    // SFP：<50 清 idx2/3，旧级<0x33 则=level-0x31 否则各+差值；逐级加/减
    // 成长属性、写日志、premium 金币奖励；专家/公会/监控/副本通知收尾。
    if (getCurCharacR() == 0)
        return 0;
    // ORIG 0x0867a986..0x0867a9ba：level = max(1, min(200, level))——两个
    // std::min/std::max<int> 各发 out-of-line call（常量实参在栈上建临时槽）。
    level = std::max(1, std::min(200, level));

    // P183：ORIG 逐处重调 get_charac_level()（+0x6b/+0xb8/+0x35e/+0x39e/+0x3c6/
    //   +0x3f3/+0x466/+0x48e/+0x4ce/+0x615/+0x6a6 共 11 次），不落 oldLevel 缓存。
    int spGain = (get_charac_level() < level)
                     ? G_CDataManager()->GetSpAtLevelUp(level)
                     : G_CDataManager()->GetSpAtLevelUp(level + 1);
    int spDiff = (level - get_charac_level()) * spGain;

    setCurCharacExp(G_CDataManager()->get_level_exp(level));

    // ORIG 0x0867aa54..0x0867ab36：树 0 / 树 1 两段显式展开（非循环），
    //   各以 std::max(0, std::min(0x3fffffff, v)) 钳制（min/max 各发 call）。
    {
        int v = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
                    (ENUM_SKILL_TREE_KIND)0) + spDiff;
        v = std::max(0, std::min(0x3fffffff, v));
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
            v, (ENUM_SKILL_TREE_KIND)0);
    }
    {
        int v = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
                    (ENUM_SKILL_TREE_KIND)1) + spDiff;
        v = std::max(0, std::min(0x3fffffff, v));
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
            v, (ENUM_SKILL_TREE_KIND)1);
    }

    if (level < 0x32)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)3);
    }
    else
    {
        int sfp2, sfp3;
        if (get_charac_level() < 0x33)
        {
            sfp2 = sfp3 = level - 0x31;
        }
        else
        {
            sfp2 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)2) + (level - get_charac_level());
            sfp3 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)3) + (level - get_charac_level());
        }
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp2, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp3, (ENUM_SKILL_TREE_KIND)3);
    }

    // 成长类型属性表：G_CDataManager()->m_characList（ORIG set_grow_type 0x08678c36
    // `mov 0x14(%eax),%edi` 指针装载 + imul 0x7dc 实证；CCharacList::m_characs 表）
    CCharacList* growTable = G_CDataManager()->m_characList;
    const _Additioal_info* growStat = (const _Additioal_info*)growTable->m_characs[get_charac_job()]
        .get_growtype_stat(getCurCharFirstGrowType(), getCurCharSecondGrowType());

    while (get_charac_level() != level)
    {
        if (get_charac_level() < level)
        {
            // ORIG 0x867acca：先加成长属性再升级（升/降分支对称）
            ((_Additioal_info*)getCurCharacAddInfoRefW())->add(*growStat);
            incCurCharacLevel();
            m_historyLog.LevelUp(get_charac_level(), (eLevelUpReason)1);
            // ORIG 0x0867ad32/+0x0867ad5f：GetGoldBonus 调两次（判零一次、传参一次）
            if (m_premium.GetGoldBonus(get_charac_level()) != 0)
            {
                getCurCharacInvenW()->gain_money(
                    m_premium.GetGoldBonus(get_charac_level()),
                    (eMoneyAddReason)0xd, true, 0);
                SendUpdateItemList((eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
            }
        }
        else
        {
            ((_Additioal_info*)getCurCharacAddInfoRefW())->subtract(*growStat);
            decCurCharacLevel();
            m_historyLog.LevelDown(get_charac_level(), (eLevelDownReason)0);
        }
        m_premium.RecalcAdditionalInfo(this);
    }
    setCurCharacLevel(level);

    // ORIG 0x867ae4c：专家职业——类型非 0 且等级命中限定等级 → 增加专家职业经验
    STExpertJobEtcScript* ejScript =
        (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    if (GetCurCharacExpertJobType() != 0 && ejScript->isExpertJobLimitLevel(level))
    {
        expert_job::CExpertJob::IncreaseExpertJobExp(this, GetCurCharacExpertJobExp());
    }

    makeGuildLevelUpMessage(level);   // ORIG T 0x8679754

    if (level == 0x46 &&
        GlobalData::s_pGMAccounts_->isGM(get_acc_id()) != 1)
    {
        // ORIG 0x0867aef9：真实 Packet_Monitor_Max_Level_BroadCast 对象（ctor
        //   W 0x08694560 = PacketHeader(0x177e,0x28) + memset(name@+0xa,0,0x1e)）
        Packet_Monitor_Max_Level_BroadCast pkt;
        strncpy(pkt.m_name, getCurCharacName(), 0x1e);
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket((char*)&pkt, 0x28);
    }

    // ORIG 0x867af66：等级==副本 0x2aff 最低要求等级时，按副本开放状态广播进出场
    if (get_charac_level() == G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff))
    {
        // ORIG 0x0867af66..0x0867aff7：isDungeonOpen 假/真各有独立调用点
        //   （无 bool 局部），且两个分支各自重取 G_GameWorld()。
        if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) == 0)
            G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff, false);
        else
            G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff, true);
    }

    if (get_charac_level() > 0x3b)
        DimensionInoutUpdate(true, true);

    m_characterView.enableSaveCharacView();
    return 1;
}

// 0x0866afa2：习得单技能主分支（master_new_skill.md）：
// skillIdx∈[0xc8,0xd0] 走公会技能（需会长/副会长）并经 guild proxy 上报；
// 否则普通 SkillSlot::master_new_skill。回包 header(1,0x1f)：失败字节0+错误码
// 映射{-1:0x15,-2:0x16,-3:0x12,-4:0x01,-5:0x5b,-6:0x5c}；成功 byte1+treeIdx+
// 剩余SP/SFP(short)+新槽位+skillIdx+等级。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0866afa2 | _ZN5CUser16master_new_skillER14stBuySkillInfob
// [b28d-GrpD 判词·结构化 N] ORIG 0x0866afa2：残差为 `skillIdx`/`isGuildSkill` 等具名量
//   槽化（ORIG 多次 `movzbl 0x1(%eax),%eax; cmp $0xc7/$0xd0,%al` 重读字段，我方
//   `cmpb $0xc7,-0xS`）+ 帧 0xac vs 0x8c；跳表常量岛（0x5c/0x5b/0x1/0x12/0x16）、
//   get_remain_sfp kind2/kind3 站点、全局 `mov 0x700(,%eax,4)` 索引全部同址
//   ⇒ 无语义缺口；可按 P183 逐点删具名收敛。
#include "CUser/master_new_skill.inc"
{
    (void)flag;
    int treeIdx = GetCurCharacSkillTreeIndex();
    unsigned char skillIdx = (unsigned char)info.m_skillIdx;
    int ret;
    int learnCnt = 0;
    int needGold = 0;
    short needGSP = 0;
    bool isGuildSkill = (skillIdx >= 0xc8 && skillIdx <= 0xd0);
    // 2026-09-11 R5 负结论（P183「删具名缓存⇒逐使用点重读」实测**净劣化**，已回滚）：
    //   按「ORIG 0x0866afa2 段 9× `movzbl 0x1(%eax),%eax`（重读 info.m_skillIdx）」
    //   删 skillIdx/isGuildSkill 两具名 ⇒ 实测 `movzbl 0x1(%eax),%eax` 我方 1→**10**
    //   （ORIG 9），指令数 395→409（ORIG 399 ⇒ gap +4→**−10**），ndiff 仅 61→60
    //   ⇒ 净劣化（多出的 14 条为每使用点重载 `mov 0xc(%ebp),%eax` 取 info 地址，
    //   ORIG 侧 info 指针常驻寄存器 = N7/N18a 寄存器分配族，源码不可造）⇒ 维持具名缓存。

    if (isGuildSkill)
    {
        // 2026-09-11 CuA（P59/P222 守卫包裹形 + 去取反物化）：ORIG 0x866afd2..0x866affe
        //   `call IsGuildMaster; test %al,%al; jne +0x1e` / `call IsSubGuildMaster; test
        //   %al,%al; je +0x19` / `mov $0x1,%eax; jmp` / `mov $0x0,%eax; test %al,%al;
        //   je <尾部 return>` —— **无 `xor $0x1` 物化**（正向 `A||B` 到 al 再 test）。
        //   原写 `!A() && !B()`（德摩根反写）令 4.4 恒产 `xor $0x1,%eax`（每侧一条）+
        //   jne/je 极性镜像。探针 /tmp/P5/p_guard g.cpp：v1(`!A&&!B`)/v2(`!(A||B)`) 均产
        //   xor；v3(`A||B`)/w1(void 版 + else return) 与 ORIG 逐条同形。
        if (IsGuildMaster() || IsSubGuildMaster())
        {
        // ⚠ 内层缩进未调整（最小化 diff）：本 `if` 的正文即原 guild 块全部语句。
        // ORIG 0x0866b..：经 GetGuildDBInfo() 访问器取 guildDB(+0x42=m_guildSkillPoint)。
        unsigned short guildLevel =
            ((STGuildDBInfoOnly*)GetGuildDBInfo())->m_guildSkillPoint;
        ret = ((SkillSlot*)getCurCharacSkillR())->master_new_guild_skill(
            (int)skillIdx, get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            *reinterpret_cast<STGuildDBInfoOnly*>(GetGuildDBInfo()), (int)getCurCharacMoney(),
            needGold, learnCnt, needGSP, (int)guildLevel);
        if (ret >= 0)
        {
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            if (proxy != 0)
                proxy->SendBuyGuildSkill((unsigned int)get_charac_no(-1),
                                         (unsigned int)get_charac_guildkey(),
                                         (int)skillIdx, learnCnt, needGSP,
                                         (unsigned int)needGold);
        }
        }
        else
        {
            return;
        }
    }
    else
    {
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (info.m_field0 == 1)
                return;
        }
        else if (info.m_field0 != 1)
        {
            return;
        }
        ret = ((SkillSlot*)getCurCharacSkillW())->master_new_skill(
            (int)skillIdx, get_charac_level(), get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            (int)info.m_count);
    }

    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(1, 0x1f);
    if (ret < 0)
    {
        pkt->put_byte(0);
        // ORIG 0x0866b0xx 跳表(0x8cef5b0)：每 ret 值一独立 put_byte 常量站点（6 case），
        // 非数组索引。展开以匹配 put_byte 调用多重集。
        switch (ret)
        {
            case -6: pkt->put_byte(0x5c); break;
            case -5: pkt->put_byte(0x5b); break;
            case -4: pkt->put_byte(0x01); break;
            case -3: pkt->put_byte(0x12); break;
            case -2: pkt->put_byte(0x16); break;
            case -1: pkt->put_byte(0x15); break;
            default: break;
        }
    }
    else
    {
        pkt->put_byte(1);
        // ORIG 0x0866b3b2..：put_byte/sp/sfpKind 判定各重调 GetCurCharacSkillTreeIndex()
        //（P44），sfp 按 (tree==-1||tree==0) 分 kind2/kind3 两独立 get_remain_sfp 站点。
        pkt->put_byte(GetCurCharacSkillTreeIndex());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex()));
        int treeForSfp = GetCurCharacSkillTreeIndex();
        if (treeForSfp == -1 || GetCurCharacSkillTreeIndex() == 0)
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)2));
        else
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)3));
        if (!isGuildSkill)
        {
            pkt->put_byte(ret);
            pkt->put_byte(skillIdx);
            int level = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
            pkt->put_byte(level);
        }
        else
        {
            pkt->put_byte(0xff);
            pkt->put_byte(skillIdx);
            pkt->put_byte(learnCnt);
        }
        // ORIG 0x0866b4b4：if/else 之后两分支共用一次 getCurCharacSkillR()+get_skill_level()
        // 再 WriteLearnSkill（原实现误置于 !isGuildSkill 内，公会技能分支漏记日志）。
        int learnLevel = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
        HistoryLog::WriteLearnSkill((FILE*)m_field796f8, (int)skillIdx, learnLevel);
    }
    pkt->finalize(true);
    Send(packet);
}

// 0x0867b048

// ORIG：GM 改成长类型。无角色/参数越界(0..5, 0..1) → 0 并 cerr 报错；
// 否则 debugCheckGrowTypeSkill 校验 → set_grow_type(reason=2) →
// _postCheckForceChangeGrowType 后处理，返回 1。
// 返回 int：ORIG 0x0867b048 尾 `mov eax,0x1; ret`、早退 `mov eax,0x0`
// （Model22 桥清零实证；声明 CUser.h:3330 由 Main 代落）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b048 | _ZN5CUser17ChangeGrowType_GMEii
#include "CUser/ChangeGrowType_GM.inc"
{
    if (getCurCharacR() == 0)
        return 0;
    if (a < 0 || a > 5 || b < 0 || b > 1)
    {
        std::cerr << "Invalid grow type , first :" << a << "second :" << b << std::endl;
        return 0;
    }
    // 2026-09-10 gA（A 组）P0/P183/P200：ORIG 0x0867b0e7 `mov %eax,%ebx`
    //   （get_charac_level() 结果无槽、跨 GetPremiumInfo/getOverSkillLevel 调用保活 %ebx）、
    //   0x0867b101 `movzwl %ax,%eax; lea (%ebx,%eax,1),%eax; mov %eax,-0xc(%ebp)`
    //   （只有 level+over 的结果落 -0xc 槽）、0x0867b115 `mov %eax,%ebx`
    //   （get_charac_job() 无槽）、receiver getCurCharacSkillW 最后求值
    //   ⇒ 删 level/overLevel/slotW/job 四个具名量（我方旧形 3 槽 + 帧 0x38 vs 0x34）。
    // ORIG 0x0867b5f4：GetPremiumInfo() 转发后 getOverSkillLevel，不直读 m_premium
    // 2026-09-11 CuA A组 P0：ORIG 0x0867b107 `mov %eax,-0xc(%ebp)`（level+over 结果**落槽**，
    //   装载段 0x0867b134 `mov -0xc(%ebp),%edx` 回读）⇒ 原码有具名局部；且 ORIG 求值序为
    //   「先 sum 落槽 → 再 get_charac_job（跨 receiver call 保活 %ebx）→ receiver」，
    //   与单一表达式右→左求值（我方旧形：job 先、sum 留 %esi）相反。
    int checkLevel = get_charac_level() + (int)(GetPremiumInfo()->getOverSkillLevel() & 0xffff);
    ((SkillSlot*)getCurCharacSkillW())->debugCheckGrowTypeSkill(
        checkLevel, get_charac_job(), a, b);
    set_grow_type((unsigned char)a, (unsigned char)b, 0, (eChangeGrowTypeReason)2);
    _postCheckForceChangeGrowType(this, a, b);
    return 1;
}

// 0x0867b6d4
// ORIG：添加物品。角色空/交易中/物品不存在/count<0 → -1；
// 构造 Inven_Item（make_item），stackable 则 set_add_info(count)；
// *space 默认 0（生物物品 → 7）；slot∈(0,0x20) 且为装备 → SetUpgrade(slot)；
// insertItemIntoInventory(reason, true, true)，失败 -1；
// 生物物品再 InsertCreatureItem。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b6d4 | _ZN5CUser7AddItemEii14eItemAddReasonR14ENUM_ITEMSPACEi
#include "CUser/AddItem.inc"
                   ENUM_ITEMSPACE& space, int slot)
{
    if (getCurCharacR() == 0)
        return -1;
    if (CheckInTrade() != 0)
        return -1;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;
    if (count < 0)
        return -1;
    Inven_Item inven;
    // 2026-09-11（H3CUser，语义轴）：ORIG 0x0867b6d4 在 `call Inven_Item::C1Ev`
    //   之后、`make_item` 虚调用**之前**写 `mov 0xc(%ebp),%eax; mov %eax,-0x5b(%ebp)`
    //   = inven+0x02 的 m_addInfo（unsigned int item idx，Inven_Item.h:269）。
    //   我方旧形缺此写入（make_item 前 inven 的 item idx 仍为 0）⇒ 补。
    inven.m_addInfo = (unsigned int)itemIdx;
    item->make_item(inven);
    if (item->is_stackable() != 0)
        inven.set_add_info(count);
    space = (ENUM_ITEMSPACE)0;
    if (inven.IsCreatureItemType() != 0)
        space = (ENUM_ITEMSPACE)7;
    if (slot > 0 && slot < 0x20 && inven.m_field1 == (unsigned char)1)
        inven.SetUpgrade((unsigned char)slot);
    // 2026-09-11 CuA A组 P183/P0：ORIG 0x0867b7e9 处 `call getCurCharacInvenW` 结果
    //   **无具名槽**（返回值直接 `mov %eax,(%esp)` 作 this，我方旧具名 invenW 占
    //   -0x18 槽 ⇒ 帧 0xb4 vs ORIG 0xac、全帧槽号 +4）。
    int ret = getCurCharacInvenW()->insertItemIntoInventory(inven, reason, true, true);
    if (ret < 0)
        return -1;
    if (inven.IsCreatureItemType() != 0)
    {
        // 2026-09-11 CuA A组 P329/P0：ORIG 0x0867b8a3/0x0867b8b0 的 getExpirationDate
        //   （→%esi）/getUsablePeriod（→%ebx）结果**不落具名槽**（跨 GetCreatureMgrW call
        //   由 callee-saved 保活）⇒ 删两个具名局部、内联回实参位（右→左求值序一致）。
        getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
            &inven, ret, (int)reason, item->getUsablePeriod(),
            item->getExpirationDate());
    }
    return ret;
}

// 0x0864cb66
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864cb66 | _ZN5CUser21LogoutCachedCharacterEh
#include "CUser/LogoutCachedCharacter.inc"
{
    // 0x0864cb66 主分支（LogoutCachedCharacter.md）：货币日志 → 缓存角色内存 →
    // Guild/Monitor 代理 Logout → param==0 双开检测 UDP Logout → PC 房通知 →
    // 清 ontime 奖励 → HeroMission 存档。
    SendMoenyLog();
    unsigned char cached = (unsigned char)CacheCharacterMemory();

    // ORIG 0x0864cb66：Guild 段与 Monitor 段各自完整重新取参（P48 no-CSE），
    // 无 GetServerProxy 判空——直接解引用调用 SendLogout（P140/P171 照抄）。
    // b28d-GrpD P183/P329：ORIG 的第一轮实参只落 4 个自动临时槽（param -0x50、
    //   memberKey -0x4c、guildKey -0x48、characNo -0x44），name/channel/accId 及
    //   proxy **不落槽**（+0x06c→%edi、+0x079→%esi、+0x083→%ebx、+0x0a2 GetServerProxy
    //   的 %eax 直接作 this）；我方旧「7 个具名量」写法把 7 个值全落 -0x34..-0x1c
    //   再逐个回读 ⇒ 多 7×2 条 mov、帧 0x78 vs ORIG 0x7c。改成单条链式调用表达式。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendLogout(get_acc_id(), G_CEnvironment()->get_channel_no(),
                     (char*)getCurCharacName(), (unsigned int)get_charac_no(-1),
                     (unsigned int)get_charac_guildkey(),
                     (unsigned int)get_charac_memberkey(), param);

    // 第二轮同形（ORIG +0x0d7 起）：ORIG 自动临时槽 -0x40(cached)/-0x3c(param)/
    //   -0x38/-0x34/-0x30，同样无我方具名量。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendLogout(get_acc_id(), G_CEnvironment()->get_channel_no(),
                     (char*)getCurCharacName(), (unsigned int)get_charac_no(-1),
                     (unsigned int)get_charac_guildkey(),
                     (unsigned int)get_charac_memberkey(), (int)param, cached);

    if (param == 0)
    {
        // ORIG +0x19a 段：`call G_CEnvironment; mov 0x378(%eax),%esi`（第 3 实参直接
        //   落 %esi）→ `call G_CEnvironment; call get_channel_no`（%ebx）→ get_acc_id
        //   （%eax）；**无 env / dcGroup / ch 具名槽**（P183/P329，我方旧形多 3 个槽）。
        GlobalData::s_double_check_proxy->SendLogout(
            get_acc_id(), G_CEnvironment()->get_channel_no(),
            G_CEnvironment()->m_serverEnvir.m_serverGroup);
    }

    if (getCurCharacR() != 0)
        SendLogoutToPCRoomServer();       // ORIG T 0x0868170c

    delete_ontime_reward_list();          // ORIG T 0x0868c3bc
    // ORIG T 0x08166d38：ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true)
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true);
    // ORIG 0x0864cd85 `mov $0x1,%eax`（call 之后、epilogue 之前）⇒ 原码显式 `return 1;`；
    // 我方 void 形把 saveEventData 的 %al 当返回值直落 %eax（差 1 条 + 返回值不同）。
    return 1;
}

// 0x08689a22：表情配置重算（ReCalcChattingEmoticon.md）。读 GameOption 表情配置 →
// CDataManager 重算 → 写回；变化则组 SIG_SAVE_CHATTING_EMOTICON（0x214：+0 accId、
// +4.. 配置 0x210）投递队列 2。SIG 当前 pad 布局按偏移写入（推断）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689a22 | _ZN5CUser22ReCalcChattingEmoticonEv
#include "CUser/ReCalcChattingEmoticon.inc"
{
    CGameOption* opt = GetGameOptionRef();
    CChattingEmoticonConfig cfg = opt->getEmoticonInfo();
    G_CDataManager()->ReCalcEmoticonConf(cfg, *this);
    // 2026-09-10（G3CUser-B，P59 分支布局反写）：ORIG 0x08689a87 `test %al,%al;
    //   je 0x08689b85`（==0 直接跳到尾声 -0x163，落点体即整段发报体）⇒ 源形是
    //   **正向** `if (SetEmoticonInfo(cfg)) { ... }`；我方 `if (!x) return;` 多出
    //   `xor $0x1,%eax` 一条且跳转目标反置。
    if (opt->SetEmoticonInfo(cfg))
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xa076), true);
        // b28d-solo-CUser p167：ORIG 0x08689a22 用 `guard.operator*()->operator<<` ×2 +
        //   `guard.operator->()->GetInBuffer`（MISSING operator* ×2/operator-> ×1），
        //   且 cfg 用结构整体赋值（我方 memcpy EXTRA ×1，P83/P132）。
        guard.operator*()->operator<<((int)0x2d7);
        guard.operator*()->operator<<((int)GetUID());
        char* sig = (char*)guard.operator->()->GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
        *(unsigned int*)(sig + 0) = get_acc_id();
        // 2026-09-11（Lane2，等宽 align-4 视图 = N78 慢路解药）：ORIG 0x08689b2b 对
        //   +4 的 0x210 字节块走**纯** `lea 0x4(%eax),%ebx; mov $0x84,%eax; mov %ebx,%edi;
        //   mov %edx,%esi; mov %eax,%ecx; rep movsl`（8 条，无对齐剥离前导/尾部）。
        //   我方 `*(CChattingEmoticonConfig*)(sig+4)` 因 CGameOption.h 的
        //   `#pragma pack(push,1)`（CGameOption.h:129/144/166）使该类型 align=1
        //   ⇒ 4.4 发「align 探测 + 剥离 1/2 字节 + rep movsl + 尾 2/1 字节」39 条慢路
        //   （历史 H3「aligned typedef 别名」失败正因别名仍以 align-1 类型为基）。
        //   等宽 align-4 局部视图后 4.4 逐条复现 ORIG 8 条（探针
        //   /tmp/lane2/probe_align3.cpp g2）。视图为纯局部类型：零符号发射、零语义变更。
        struct EmoticonCfgA4View
        {
            unsigned int m_dw[0x210 / 4];   // 0x210 字节，align 4
        };
        *(EmoticonCfgA4View*)(sig + 4) = *(const EmoticonCfgA4View*)&cfg;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
}

// 0x08689010：表驱动重置列表（ResetItemByScript.md）：元素 +0 itemspace、
// +4 到期秒；到期则压 (space,0)。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08689010 | _ZN5CUser17ResetItemByScriptERSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/ResetItemByScript.inc"
{
    // 残差 P75/N80 族（跨版本实参装载中转寄存器差）：两处 operator[](i) 的
    // 实参 i 装载 ORIG 4.1 经 %edx 中转入 0x4(%esp)，4.4 恒 %eax。求值序/槽位/
    // 调用全同，仅中转寄存器名差，源形不可复现。
    // p167 差集（b28）：ORIG 走 `operator[](uint) const`×2 + make_pair<int const&,int>
    // ⇒ 原码以 const 指针访问（容器 cv 限定决定实例化，P96）。
    // b28d-solo-CUser 三形复测（2026-09-09）：显式 `items->operator[](i)` /
    // `unsigned int i` / const 引用 `items[i]` 三形均不翻 ORIG 的「this 先装
    // -0x14→%eax、i 后装 -0xc→%edx」实参序（4.4 恒值先）⇒ 维持 P75/N80 封顶。
    const std::vector<stResetItemInfo::ResetItem>* items =
        G_CDataManager()->GetResetItemInfo();
    int now = GlobalData::s_systemTime_.getCurSec();
    for (size_t i = 0; i < items->size(); ++i)
    {
        if ((*items)[i].m_resetSec <= now)
            list.push_back(std::make_pair((*items)[i].m_space, 0));
    }
}

// 0x08689b90：聊天表情配置包(0x19b)。24 行：index、m_nCount(行内 +0x14)、
// 名字（行首 0x14 字节）。行大小 0x16（CChattingEmoticonConfigRow, pack(1)）。
#include "CUser/SendChattingEmoticon.inc"
{
    // ORIG 0x8689b90：GetGameOptionRef 结果**落槽 -0x10** 再作 this 调
    // getEmoticonInfo（sret 就地构 cfg@-0x22c）；行地址**每使用点重算**
    // （`imul $0x16` 段，无 row 缓存变量——strlen/put_str 各自 lea+imul）。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x19b);
    ((InterfacePacketBuf*)&packet)->put_short(0x18);
    CGameOption* go = GetGameOptionRef();
    CChattingEmoticonConfig cfg = go->getEmoticonInfo();
    for (int i = 0; i < 0x18; ++i)
    {
        ((InterfacePacketBuf*)&packet)->put_short(i);
        ((InterfacePacketBuf*)&packet)->put_short(cfg.m_Row[i].m_nCount);
        ((InterfacePacketBuf*)&packet)->put_int((int)strlen(cfg.m_Row[i].m_data));
        ((InterfacePacketBuf*)&packet)->put_str(cfg.m_Row[i].m_data,
                                                (int)strlen(cfg.m_Row[i].m_data));
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x08653270：登出前数据保存主链路（UpdateData.md）。守卫 state>2 且频道!=7；
// CheckNFix；会员税上报并清零；_SaveCharacData/UpdateCharacView/HeroMission；
// ---- UpdateData uses DB request classes declared in W8LT_DBTask.h ----
// The declarations are real dispatcher types; makeRequest definitions remain
// in W8LT_DBTask.cpp and this TU only invokes them.
// §6.2 闭合：P167 调用点差集 CLEAR (0/0, 271=271)。修复：hack 计数循环
// 守卫+取值双读 getServerHackCnt（3×getHackAnalyzer/2×getServerHackCnt）；
// SIG_OBJECT_BRING_UP m_serverGroup 改内联 m_serverEnvir.m_serverGroup（ORIG
// 直读 +0x378 &0xff，无 get_server_group() 调用）。verdict orig1246/our1293，
// 残余 gap 属 N82/-O0 帧差异，逐块 vs Ghidra C 全对齐。

// §6.2 巨件核销：P167 271/271 [0缺0多]；gap 1246→1293（our+47）；残差归因 N82
// （4.1 无 lifetime 槽复用 ⇒ 全帧槽号错位，ndiff 941 与语义完成度反向）+
// CStreamGuard 链式写法的槽物化差；EXTRA=0 已排除多发语义（任务书关注点），
// 语义完整仅剩 codegen 差，N 封顶（详见 docs/asm_translation_patterns.md 负例表）。
// 各 isSaveXxx 脏块经 StreamPool+CStreamGuard 组包投递队列 2 后 disable。
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x08653270 | _ZN5CUser10UpdateDataEv
#include "CUser/UpdateData.inc"
{
    if (m_chState <= 2)
        return;
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    // ORIG +0x9f：无当前角色 → trace（0x1d99）后直接返回，不发任何保存。
    if (getCurCharacR() == 0)
    {
        // b28d-lane5 复测（P227）：ORIG 0x086532d0 段 getter 的 call 在 cMyTrace ctor
        //   **之前**、结果以 %ebx 跨 ctor 保活（`mov %eax,%ebx` … `mov %ebx,0x8(%esp)`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d99, 5)
            ("CUser::UpdateData() character %s - m_selected is null",
             getCurCharacName());
        return;
    }
    // ORIG +0x121：数据加载未完成 → trace（0x1da1）后直接返回。
    if (isLocked4DataLoad())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1da1, 5)
            ("Can not save character until loading done(m_id: %s, charac_no: %d",
             NumberToString(get_acc_id(), 0), getCurCharacNo());
        return;
    }

    if (getCurCharacInvenR()->checkToFix())
        getCurCharacInvenW()->CheckNFix();

    if (getCurCharacR() != 0 && get_charac_memberkey() != 0)
    {
        int payTaxMoney = get_member_pay_tex_money_to_upper();
        int payTaxFatigue = get_member_pay_tex_fatigue_to_upper();
        CMonitorServerProxy* monProxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (monProxy != 0)
            monProxy->SendMemberPayTax(is_connect_upper_member() != 0,
                                       (unsigned int)getCurCharacNo(),
                                       payTaxMoney, payTaxFatigue);
        if (payTaxMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payTaxFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    _SaveCharacData();                    // ORIG T 0x08650aa2
    UpdateCharacView();
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, false);  // ORIG T 0x08166d38

    if (isSaveInven() || isSaveCargo())
    {
        // ORIG +0x27b：state(+0x8cfc4)==5（角色选择视口阶段）仅存背包。
        if (m_chState == 5)
            SaveInventoryOnly();
        else
            SaveInventory();
        disableSaveInven();
        disableSaveCargo();
    }

    WongWork::CMailBoxHelper::ReqDBSendStoredMail(this);

    if (isSaveSkill())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1ded), true);
        guard.operator*()->operator<<((int)0x24);              // SIG_LOAD_SKILL
        guard.operator*()->operator<<((int)GetUID());
        SIG_LOAD_SKILL* sig = guard.operator->()->GetInBuffer<SIG_LOAD_SKILL>();
        memset(sig, 0, sizeof(SIG_LOAD_SKILL));           // 0x414
        // ORIG +0x394：+0x00 槽写 getCurCharacNo（movl 4B 实证）。
        sig->m_characNo = (unsigned int)getCurCharacNo();
        // ORIG 0x0865360e `movzbl 0xa850(%eax)`（unsigned char 宽实证）
        sig->m_field411 = G_CDataManager()->m_pvpSkillTree.m_field50;
        // b28d-GrpD（P0/P183/P329）：ORIG 0x08653621/0x0865362f/0x08653639/0x08653643
        //   `mov -0x60(%ebp),%eax; add $0x344/$0x340/$0x1a4/$0x8,%eax` —— 4 个实参就是
        //   包内字段地址（Ghidra C：`get_remain_sp(this,(int*)(sig+8),…,(int*)(sig+0x344))`）；
        //   字段均 `int`（SigTypes_C.h:1005/1008/1010/1011，偏移 8/0x1a4/0x340/0x344 全等），
        //   形参 `int*`（SkillSlot.h:229）⇒ 删 4 个具名 int（我方原多 4 槽 + 4×3 条拷回）。
        ((SkillSlot*)getCurCharacSkillR())->get_remain_sp(
            &sig->m_remainSpTree1, &sig->m_remainSpTree2,
            &sig->m_remainSfpTree1, &sig->m_remainSfpTree2);
        _Mastered_skill* buf0 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)0);
        _Mastered_skill* buf1 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)1);
        if (buf0 != 0 && buf1 != 0)
        {
            // ORIG 拷贝 0x198 字节（= 0xcb 槽 × 2B，越界 2 字节读忠实保留）。
            memcpy(sig->m_skillSlotTree1, buf0, 0x198);
            memcpy(sig->m_skillSlotTree2, buf1, 0x198);
        }
        // ORIG +0x49a：技能命令区（+0x348，200 字节）拷贝后清零自身区（+0x703db）。
        STSkillCommandData::STCommand* cmdBuf = get_skill_command_buf();
        if (cmdBuf != 0)
            memcpy(sig->m_commands, cmdBuf, 200);
        memset(m_skillCommands, 0, sizeof(m_skillCommands));   // 200 = 0xc8
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        updateComboSkill();                 // ORIG T 0x08692396
        disableSaveSkill();
    }

    if (isSavePvP())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e2c), true);
        guard.operator*()->operator<<((int)0x25);              // SIG_SAVE_PVP
        guard.operator*()->operator<<((int)GetUID());
        SIG_SAVE_PVP* sig = guard.operator->()->GetInBuffer<SIG_SAVE_PVP>();
        memset(sig, 0, sizeof(SIG_SAVE_PVP));      // 0x88
        sig->m_characNo = getCurCharacNo();
        // ORIG：PvpResultType::Set((PvpResultType*)sig, getPVPResultRefR()) 整体拷入前段。
        ((PvpResultType*)sig)->Set(*getPVPResultRefR());
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSavePvP();
    }

    if (isSaveDungeonClear())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e4e), true);
        guard.operator*()->operator<<((int)0x27);              // SIG_SAVE_DUNGEON_CLEAR
        guard.operator*()->operator<<((int)GetUID());
        char* sig = (char*)guard.operator->()->GetInBuffer<SIG_SAVE_DUNGEON_CLEAR>();
        memset(sig, 0, 0x180c);
        *(unsigned int*)(sig + 0) = (unsigned int)getCurCharacNo();
        *(unsigned int*)(sig + 4) = get_acc_id();
        // ORIG +0x77b：CDungeonClear::toString(sig+0xc, 0x1800) 序列化；
        // 返回负值（shr 31）走 "Quest buffer overflow" 报错 trace。
        if (getDungeonClearRefR()->toString(sig + 0xc, 0x1800) < 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1e5e, 5)
                ("Quest buffer overflow (m_id: %s, charac_no: %d",
                  NumberToString(get_acc_id(), 0), getCurCharacNo());
        }
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveDungeonClear();
    }

    if (isSaveBattleRecord())
        disableSaveBattleRecord();     // ORIG +0x870：战斗记录脏标记仅清除

    if (isSaveQuest())
    {
        CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1e8c), true);
        guard.operator*()->operator<<((int)0x29);              // SIG_UPDATE_QUEST
        guard.operator*()->operator<<((int)GetUID());
        SIG_UPDATE_QUEST* sig = guard.operator->()->GetInBuffer<SIG_UPDATE_QUEST>();
        memset(sig, 0, sizeof(SIG_UPDATE_QUEST));
        // questR+0x04 即 UserQuest::m_cleared（CQuestClear；+0x7538=m_quest[i]、
        // +0x7588=m_trigger[i]、+0x75f0/0x75f4 在 m_authenData 内）。
        // ORIG 每处重调 getCurCharacQuestR()（P44，含循环内），共 6 站点。
        if (getCurCharacQuestR()->m_cleared.checkSaveFlag())
        {
            memcpy(sig->m_clearedQuest,
                   getCurCharacQuestR()->m_cleared.getClearedQuest(), 30000);
            sig->m_clearedFlag = 1;
        }
        sig->m_characNo = (unsigned int)getCurCharacNo();
        for (int i = 0; i < 0x14; ++i)
        {
            sig->m_quest[i] =
                (unsigned int)getCurCharacQuestR()->m_quest[i];
            sig->m_trigger[i] =
                (unsigned int)getCurCharacQuestR()->m_trigger[i];
        }
        sig->m_urgentQuest =
            (unsigned int)getCurCharacQuestR()->m_authenData.m_urgentQuest;
        memcpy(sig->m_authen, getCurCharacQuestR()->m_authenData.m_authen, 0x28);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        // ORIG +0xa70：put 后清 QuestClear 保存标记再 disable。
        getCurCharacQuestW()->m_cleared.resetSaveFlags();
        disableSaveQuest();
    }

    // ORIG +0xaae：宠物（creature）项脏块经背包 CreatureMgr 落盘（读引用+R 视图取 W mgr，
    // 同 CUser.cpp 830 行先例 cast 掉 const）。
    if (((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->IsSaveCreatureItem())
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SaveCreatureItem();

    // ORIG 判定与取值各重调一次 GetCurCharacEventItemCount()（P44）。
    if (GetCurCharacEventItemCount() != 0)
    {
        // ORIG +0xafd：DB_UpdateItemCountValue::makeRequest(get_acc_id(), count)（SIG 0x240）。
        DB_UpdateItemCountValue::makeRequest(get_acc_id(),
                                             (unsigned int)GetCurCharacEventItemCount());
        ResetCurCharacEventItemCount();
    }

    if (isSaveInformNoticeFlag())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f2b), true);
        guard.operator*()->operator<<((int)0x2f5);             // InformNoticeFlagData
        guard.operator*()->operator<<((int)-1);
        char* sig = (char*)guard.operator->()->GetInBuffer<InformNoticeFlagData>();
        memset(sig, 0, 0x14);
        *(unsigned int*)(sig + 0) = get_acc_id();
        // ORIG +0xc05：i∈[0,0x10) 逐位 getInformNoticeFlag(i) 填 +4..+0x13。
        for (int i = 0; i < 0x10; ++i)
            *(sig + 4 + i) = (char)getInformNoticeFlag(i);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveInformNoticeFlag();
    }

    if (GetCurCharacUsedFatigueQuantity() != 0)
    {
        // ORIG +0xc90：DB_UpdateUserFatigueQuantity::makeRequest(accId, qty)（SIG 0x287）。
        DB_UpdateUserFatigueQuantity::makeRequest(
            get_acc_id(),
            (unsigned int)GetCurCharacUsedFatigueQuantity());
        ResetCurCharacUsedFatigueQuantity();
    }
    // ORIG +0xcd8：赠礼疲劳脏标记 → 三参 DB 请求（SIG 0x288，accId/characNo/giftQty
    // 依报告反编译 C 实参序）后复位标记。
    if (GetChangedGiftFatigueQuantity())
    {
        DB_UpdateUserGiftFatigueQuantity::makeRequest(
            get_acc_id(), getCurCharacNo(),
            (unsigned int)GetCurCharacUsedGiftFatigueQuantity());
        SetChangedGiftFatigueQuantity(false);
    }

    // ORIG +0xd24：事件 0x19b（OBJECT_BRING_UP）进行中且攒有材料数 → 投递 SIG 并复位。
    EventClassify::CEventScriptMng* esm =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    if (esm->is_eventing(0x19b) && getCurUseBringUpMaterialCount() != 0)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f73), true);
        guard.operator*()->operator<<((int)0x2ca);             // SIG_OBJECT_BRING_UP
        guard.operator*()->operator<<((int)-1);
        SIG_OBJECT_BRING_UP* sig = guard.operator->()->GetInBuffer<SIG_OBJECT_BRING_UP>();
        sig->m_accId = get_acc_id();
        sig->m_characNo = getCurCharacNo();
        // ORIG：+0x08 = *(G_CEnvironment()+0x378) & 0xff = m_serverEnvir.m_serverGroup
        // 低字节。P167：ORIG 为内联字段直读（无 get_server_group() 调用），
        // 故用 m_serverEnvir.m_serverGroup 内联访问，勿经 get_server_group() 访问器。
        sig->m_serverGroup = (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        sig->m_usedMaterialCnt = getCurUseBringUpMaterialCount();
        sig->m_rewardFlag = 0;           // ORIG：+0x0d 恒 0
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        resetCurUseBringUpMaterialCount();
    }

    // ORIG 判定与取值各重调一次 GetCurCharacEventAddItemCount()（P44）。
    if (GetCurCharacEventAddItemCount() != 0)
    {
        // ORIG +0xedc：DB_UpdateAddItemCountValue::makeRequest(accId, count)（SIG 0x280）。
        DB_UpdateAddItemCountValue::makeRequest(get_acc_id(),
                                                (unsigned int)GetCurCharacEventAddItemCount());
        ResetCurCharacEventAddItemCount();
    }

    // ORIG +0xee8：hack 类型区间 [0x65,0xfa0] 服务器计数落库并复位。
    for (int hackType = 0x65; hackType <= 0xfa0; ++hackType)
    {
        // ORIG 0x086541dd：守卫读一次 getServerHackCnt（iVar25 != 0），进入后
        // 再读第二次作为 ReqDBSaveHackInfo 实参（3 次 getHackAnalyzer / 2 次
        // getServerHackCnt，P167 缺口），勿缓存单次计数。
        if (getHackAnalyzer()->getServerHackCnt(
                (WongWork::ENUM_HACKTYPE)hackType) != 0)
        {
            WongWork::CHackAnalyzer::ReqDBSaveHackInfo(
                get_acc_id(), (unsigned int)get_charac_no(-1),
                (unsigned short)hackType, GetETC((short)hackType),
                getHackAnalyzer()->getServerHackCnt(
                    (WongWork::ENUM_HACKTYPE)hackType));
            getHackAnalyzer()->resetServerHackCnt(
                (WongWork::ENUM_HACKTYPE)hackType);
        }
    }
    // ORIG +0xfd6：MC 点>499 → 曝光计数与点落库（type 0x329）后清曝光。
    if (m_mcap.getPoint() > 499)
    {
        WongWork::CHackAnalyzer::ReqDBSaveHackInfo(
            get_acc_id(), (unsigned int)get_charac_no(-1), 0x329,
            (int)m_mcap.getPoint(), (int)m_mcap.getExposedCount());
        m_mcap.resetExposedCount();
    }

    // ORIG +0x1046：学校点（脏累加）落库后复位。
    if (getCurCharacSchoolPoint() != 0)
    {
        DB_UpdateSchoolPoint::makeRequest(get_acc_id(),
                                          getCurCharacSchoolPoint(),
                                          (int)GetUID());
        resetCurCharacSchoolPoint();
    }

    // ORIG +0x109a：血之最高纪录/维度进出/断线运气/断线副本计数等脏块逐一落库。
    if (isBloodBestRecordUpdate())
        UpdateBloodRecord();
    if (get_charac_level() > 0x3b &&
        (IsSaveDemensionInoutValue() || IsSaveBloodInoutValue()))
        UpdateDimensionInout();
    if (isUsedBreakAwayLuckPoint())
        UpdateBreakAwayLuckPoint();
    if (isBreakAwayDungeonClear())
        UpdateBreakAwayDungeonClearCnt();
    // ORIG +0x115b：角色链接奖励 exp/gold 任一带值 → 提交（state=2）。
    if (getCharacLinkBonusExp() != 0 || getCharacLinkBonusGold() != 0)
        UpdateCharacLinkBonus(getCharacLinkBonusExp(), getCharacLinkBonusGold(),
                              (unsigned int)getLinkCharacNo(getLinkCharacSlotIndex()),
                              (ENUM_CHARAC_LINK_BONUS_STATE)2);

    // ORIG +0x11c3：NPC 关系 / 扩展数据链 / 条件事件 / 适正副本数。
    _updateNPCRelationship();
    m_characExpand.update(this);
    _saveConditionEventInfo(getCurCharacNo(), GetCurConditionEventStep(),
                            GetCurConditionEventRewardStep());
    _saveProperDungeonClearCount(getCurCharacNo(),
                                 GetProperDungeonClearCount());
    if (isSaveCharacOption())
        _updateSaveOption();

    // ORIG +0x1275：账号仓库有改动 → DB_SaveAccountCargo（SIG 0x1f1；随后 SetStable）。
    if (IsExistAccountCargo())
    {
        CAccountCargo* cargo = GetAccountCargo();
        if (cargo->IsAlter())
        {
            DB_SaveAccountCargo::makeRequest((int)GetUID(), get_acc_id(),
                                             cargo);
        }
    }
    // ORIG +0x12c7：金币保护存库 + 杂项周期落库。
    GetGoldControl()->SavetoDB(true, true, false);
    SaveMercenaryData();
    updatePcRoomPlayTime();
    updateBlueMarbleEnterCount();
    updateBingoData();
}

// 0x086554aa：登出主链路（UpdateLogout.md）。SIG_UPDATE_LOGOUT(0x2c, 0x8b 字节)：
// accId/characNo/gcNo/fatigue；state>1 记 playTime/IP/机器码/会员奖励疲劳并复位；
// 两条 trace（0x22e0/0x22e2）；写登录时间戳；历史日志 Logout；DB 杂项（复仇/
// ontime/疲劳出勤）；!param 发包(1,3)；清商城内购限制。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086554aa | _ZN5CUser12UpdateLogoutEb
#include "CUser/UpdateLogout.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（ORIG 492 vs 我方 513）：ORIG 0x086554aa 与 p167
    //   已对齐的调用序一致（CStreamGuard::operator*/-＞、sig 具名槽、DB 段、两处
    //   cMyTrace），差集为**全帧槽号整体平移**（param -0xac→-0xbc、guard -0x50→-0x44、
    //   sig -0x28→-0x3c；帧 0xcc→0xdc）——属 N82/P112 槽分配族（4.1 与 4.4 局部
    //   分配方向/复用不同），逐块无字段或调用缺口可下刀 ⇒ N 封顶。
    // 2026-09-11 R5 补（disp2 命中终判，门②/③d）：disp2 报本件 ORIG-only `+0x47`。核指令
    //   上下文：ORIG 0x08655735 `lea 0x47(%eax),%edx`（eax = -0x28(%ebp) 槽值）→
    //   `movl $0x41,0x8(%esp); lea -0x9d(%ebp),%eax; …; call <memcpy>` ⇒ 是**局部指针槽的
    //   地址算术**（0x47 是目标缓冲偏移），**非结构成员读**；我方同点 `add $0x47,%eax`
    //   （imm 轴同值命中）⇒ 门② 同值互补 + 门③d ⇒ 等价，非 bug，勿再改。
    UpdateCharacView();

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2260), true);
    // b28d-CUser p167 真缺口：ORIG 0x086555e0/0x086555f1 显式调 CStreamGuard::operator*
    //   （deEv）×2、0x086555?? operator->（ptEv）×1 再 GetInBuffer（P132/P103）；
    //   直接 `guard <<` / `guard.GetInBuffer` 少 3 个调用点。
    (*guard)->operator<<((int)0x2c);
    (*guard)->operator<<((int)GetUID());
    SIG_UPDATE_LOGOUT* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_LOGOUT>();
    memset(sig, 0, sizeof(SIG_UPDATE_LOGOUT));
    // b28d-CUser p167：ORIG 0x8655582 `mov 0x704ac(%eax),%eax` 直读成员
    // （非 get_acc_id 调用；该访问器在 ORIG 本函数仅出现 2 次，见下方 DB 段）。
    sig->m_accId = m_accId;
    // ORIG 0x865557c：IntegratedPvP 频道用 get_gc_no_hardcode(GetServerGroup())。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        sig->m_gcNo =
            (unsigned int)G_CEnvironment()->get_gc_no_hardcode(GetServerGroup());
    else
        sig->m_gcNo = (unsigned int)G_CEnvironment()->get_gc_no();
    if (getCurCharacR() != 0)
    {
        sig->m_characNo = (unsigned int)getCurCharacNo();
        sig->m_fatigue = GetFatigue();
    }
    ((XNuclear::CHades*)getHades())->Send_Logout();   // ORIG 0x8655605

    long now = (long)GlobalData::s_systemTime_.getCurSec();
    sig->m_stateFlag = 0;
    if (get_state() > 1)
    {
        sig->m_stateFlag = 1;
        sig->m_playTime =
            (int)(now - m_premium.m_loginTime);   // 推断：登录时刻
        sig->m_field17 = (unsigned int)m_tradeCount;   // CUser+0x8d10c（ORIG movl）
        sig->m_playExpAdd = m_gameOption.m_playExpAdd;   // ORIG this+0x703a4 = m_gameOption+0x640
        sig->m_totalFatigueUsed =
            (unsigned short)m_postChecksumData.m_totalFatigueUsed;   // ORIG this+0x79642 实证
        // b28d-CUser p167 真缺口：ORIG 0x865568e `movzbl 0x8d0df(%eax),%edx;
        //   mov %dl,0x21(%eax)` 直读 this+0x8d0df（= m_guildAgitDB.m_pad2[0]，
        //   SigTypes_E.h:521 注释同证）；原写 GetPuUser() 读的是 +0x796d8（错字段，
        //   且多发一次 call）。
        sig->m_puUser = (unsigned char)m_guildAgitDB.m_pad2[0];
        SetUserManagerToolIP(this, reinterpret_cast<SIG_UPDATE_LOGOUT*>(sig));  // ORIG 0x865569e 实证传结构基址

        // b28d-CUser p167：ORIG 0x86556ad 起为非对齐 peel + rep stos 内联清零
        // （无 memset call）⇒ 用 `= {0}` 常量清零形（P214③/P253）。
        char mac[0x41] = {0};
        memcpy(mac, m_network.get_mac_addr(), 0x41);
        strncpy(sig->m_mac, mac, 0x41);
        sig->m_saveMemberBonusFlag = IsEnableSaveMemberBonusFatigue();
        DisableSaveMemberBonusFatigue();
        sig->m_memberBonusFatigue =
            (unsigned short)getAccountMemberBonusFatigue();
        resetAccountMemberBonusFatigue();
    }
    {
        // ORIG 0x8655791/0x8655803：0x71ba4 更新前后各打一条 trace。
        // ORIG 0x08655791-0x086557cc：mid/playTime **无栈槽**——`movzwl %ax,%ebx`（mid 直入
        // callee-saved）、`mov -0x28(%ebp),%eax; mov 0x13(%eax),%esi`（sig->m_playTime 直读）。
        // 我方两具名 int 各占一槽 ×2 块 = 4 槽，正是帧 0xdc vs 0xcc（+0x10）的唯一来源。
        // 实参求值序（右→左）由 GCC 自行处理，与 ORIG 一致。
        cMyTrace tr1(__PRETTY_FUNCTION__, 0x22e0, 0);
        tr1("trace mid:%d playTime:%d login_time:%d now_time:%d",
            get_unique_id(), sig->m_playTime, m_premium.m_loginTime, (int)now);
    }
    m_premium.m_loginTime = now;
    {
        // 同上（ORIG 0x08655803-0x08655843）。
        cMyTrace tr2(__PRETTY_FUNCTION__, 0x22e2, 0);
        tr2("trace mid:%d playTime:%d login_time:%d now_time:%d",
            get_unique_id(), sig->m_playTime, m_premium.m_loginTime, (int)now);
    }

    // b28d-CUser p167：ORIG 0x8655869 `mov 0x704ac(%eax),%eax; test %eax,%eax`
    // 直读成员（非 get_acc_id 调用）。
    // ORIG 0x08655869..0x086558a5：三段短路全部直判（`test %eax,%eax; jne` /
    // `test %al,%al; jne` / `cmp $0x7,%eax; jne` → `mov $1/$0` 合流），**无 bool 槽**；
    // 我方具名 sendOut 多 `mov %al,-0x31(%ebp); cmpb $0x0,-0x31(%ebp)`。
    if (m_accId != 0 || G_GameWorld()->IsIntegratedPvPBaseChannel() ||
        G_GameWorld()->GetChannelType() == 7)
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    else
        GlobalData::s_stream_pool->Free(guard);

    if (m_network.get_str_ip()[0] != '\0' && getCurCharacName() != 0)
    {
        // b28d-CUser p167 真缺口：ORIG 0x8655915 / 0x8655955 调用
        //   GetCharacStartPlayTick **两次**（条件一次、实参一次，P157/P48 逐使用点重调）；
        //   实参求值序（cdecl 右→左）：GetLogInOutState → getCurSec → tick2 → …
        if (GetCharacStartPlayTick() > 0)
        {
            m_historyLog.Logout(getWebAddress(), m_network.get_str_ip(),
                                m_network.get_port(),
                                (unsigned short)G_CEnvironment()->m_serverEnvir.m_channelNo,
                                (int)((long)GlobalData::s_systemTime_.getCurSec() -
                                      GetCharacStartPlayTick()),
                                GetLogInOutState());
        }
    }

    DBUpdateDBLogItem();
    ResetDBLogItem();

    {
        unsigned int characNo = getCurCharacNo();
        unsigned char villageCnt = GetCurVillageAttackCount();
        unsigned char revengeCnt = GetCurRevengeDungeonCount();
        if (IsVillageAttackDBUpdate())
            DB_UpdateRevengeData::makeRequest(characNo, villageCnt, revengeCnt);
    }
    if (is_update_ontime_last_recv_idx())
    {
        DB_UpdateOntimeLastRecvIdx::makeRequest(GetUID(), get_acc_id(),
                                                get_ontime_last_recv_idx());
        reset_update_ontime_last_recv_idx();
    }
    if (((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->is_eventing(0x56))
    {
        if (get_server_fatigue_day_size() != 0)
        {
            // b28d-CUser p167 真缺口：ORIG 0x8655ac9 `call get_server_fatigue_day_data;
            //   mov %eax,%ebx; … mov %ebx,0x4(%esp)` —— 取地址走 getter，非成员直传。
            DB_UpdateFatigueAttendance::makeRequest(
                get_acc_id(),
                *(const std::map<unsigned char, short>*)
                    get_server_fatigue_day_data());
            reset_server_fatigue_data();
        }
    }
    reset_ingame_event_history();

    if (!param)
    {
        PacketGuard pkt;
        ((InterfacePacketBuf*)&pkt)->put_header(1, 3);
        ((InterfacePacketBuf*)&pkt)->put_byte(1);
        ((InterfacePacketBuf*)&pkt)->finalize(true);
        Send(pkt);
    }
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
}

// 0x0867e734：日删可堆叠物品（deleteDailyStackableItem.md）。区间 [a,b] 三重过滤
// （空槽/find_item/is_stackable/+0x3c5>0 日删标记）后删除并压 (space=0, slot)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e734 | _ZN5CUser24deleteDailyStackableItemERSt6vectorISt4pairIiiESaIS2_EEii
#include "CUser/deleteDailyStackableItem.inc"
                                     int a, int b)
{
    // 2026-09-10（G3CUser-B 复验）：槽集 ORIG/我方 各 10 槽、零平移零多余
    //   （100:105 insn）；ORIG 0x0867e7bb `je`（ci==0 跳过）与 0x0867e7d1
    //   `xor $1; jne`（!is_stackable 跳过）确认我方 `ci == 0 || !ci->is_stackable()`
    //   两条独立守卫**有 ORIG 直证**，不可删 ⇒ 残余属跨版本 codegen，判 N。
    if (getCurCharacR() == 0)
        return;
    CInventory* inven = (CInventory*)getCurCharacInvenR();
    for (int slot = a; slot <= b; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0)
            continue;
        if (!ci->is_stackable())
            continue;
        // 2026-09-10（§4.3 去裸偏移）：+0x3c5 日删标记改用具名字段访问
        //   （CStackableItem.h:543 `char m_char3c5`）。ORIG 该点 `movzbl 0x3c5(%eax),%eax`
        //   单条 disp 折叠；原裸偏移形 4.4 恒展成 `add $0x3c5; movzbl (%eax)` 两条。
        // [L9-GrpL 2026-09-11 continue 岛消除] ORIG 0x0867e874 `jle 0867e875 <+0x141>`：
        //   末位守卫（m_char3c5<=0）**直接跳循环尾**，全函数只有 3 个 `jmp +0x141` 岛
        //   （0x0867e7bb/0x0867e7d1 两个 continue + push_back 尾岛）；我方 4 个 continue
        //   各产一岛（raw 102 vs 100，`jmp +0x144;nop` ×4）⇒ 末位守卫改正向 if 包裹：
        //   条件取反后体落 fall-through，`jle` 的跳过目标 = 循环尾（++slot），与 ORIG 同形。
        if (((CStackableItem*)ci)->m_char3c5 > 0)
        {
            enableSaveInven();
            inven->delete_item((INVEN_TYPE)1, slot, item.m_addInfo2,
                               (eItemDelReason)0x13, true);
            // b28d-solo-CUser p167：ORIG 0x0867dcxx 实例化 `make_pair<ENUM_ITEMSPACE, int&>`
            // （我方 (int) cast 致 `make_pair<int, int&>`）⇒ 去掉 cast（P61/P315）。
            list.push_back(std::make_pair(ENUM_ITEMSPACE_0, slot));
        }
    }
}

// 0x0867dba0：指定物品清理编排（deleteSpecificItem.md）：关私摊 → 五域清理 →
// 邮箱超时/匹配/无效物品移除（header 0x62 包）→ 无效物品邮件补偿 → redeem 清理。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0867dba0 | _ZN5CUser18deleteSpecificItemERKSt6vectorISt4pairIiiESaIS2_EERS4_
// 定义于 CDataManagerScripts.cpp（ORIG 0x867cf8d T）；deleteSpecificItem 邮箱段先用。
// b28d-PoolCUser 档①跨 TU 同步（2026-09-09）：定义端已 bool（CDataManagerScripts.cpp:869，
//   Main P292）；ORIG 本 TU 调用点 `call; test %al,%al; je`（0x0867d472 族）无 setne
//   ⇒ 本镜像声明同步 bool，否则 5 调用点卡 setne 归一段。
#include "CUser/_checkTimeoutItem.inc"

#include "CUser/deleteSpecificItem.inc"
                               std::vector<std::pair<int, int> >& out)
{
    if (getCurCharacR() == 0)
        return;

    // ORIG 0x0867dbc4 `movl $0x0,-0x28` 预清 + 0x0867dbcb 无 mgr 具名局部
    //   （GetInstancePrivateStoreMgr 结果经 %eax 直传）⇒ 声明与赋值分两句（P198③）。
    private_store::CPrivateStore* store = 0;
    store = (private_store::CPrivateStore*)
        private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(this);
    // 2026-09-10（P225 物化复现）：ORIG 0x0867dbc8 段为
    //   `cmpl $0x0,-0x28(%ebp); setne %al; test %al,%al; je <跳过>`（4.1 bool 物化），
    //   4.4 对裸 `if (store != 0)` 恒折直跳 je（无 setcc）⇒ 改 while+break 形
    //   （P225 实证：唯一复现 setne;test;je 的源形）。
    while (store != 0)
    {
        // 真成员调用（b22 消解旧 §4.5 例外）：ISeller 槽 +0x04
        // RemovePrivateStore(bool)，CPrivateStoreMgr.h:54/217 权威声明
        // （ORIG T 0x85c5fd0，vtable @0x8cbeb60 实证），编译器自动出
        // vtbl[1] 间接调用，与原手写槽取法同形。
        store->RemovePrivateStore(false);
        break;
    }

    _deleteSpecificItemInven((INVEN_TYPE)0, list, out, 10, 0x15);
    _deleteSpecificItemInven((INVEN_TYPE)1, list, out, 3, 0x137);
    _deleteSpecificItemCargo(list, out);
    _deleteSpecificItemAccountCargo(list, out);
    _deleteSpecificItemTitieBook(list);

    // ORIG this+0x71b98 实证：无 mailBox 具名局部，每处重读 `mov 0x71b98(%eax),%eax`
    //   （0x0867dcad/0x0867dcd3/0x0867de88/0x0867dea0/0x0867dee0/0x0867dff4）。
    if (m_premium.m_mailBox != 0)
    {
        std::vector<std::pair<int, int> > mailDel;
        WongWork::CMailBox::CMail* mail = 0;   // ORIG 0x0867dccc `movl $0x0,-0x24` 预清
        ((WongWork::CMailBox*)m_premium.m_mailBox)->setMailIterator();
        // ORIG 0x0867dce4 `rep stos` 0x15 dwords ⇒ rmList[21]，rmList[0] 即计数。
        unsigned int rmList[21] = {0};
        // b28d-solo-CUser p167：ORIG 单 getNextMail 调用点（while 条件内赋值）；
        //   for(init;cond;incr) 形产 2 个调用点（EXTRA ×1）⇒ 改 while 赋值形。
        while ((mail = ((WongWork::CMailBox*)m_premium.m_mailBox)
                           ->getNextMail()) != 0)
        {
            // b28d-solo-CUser p167 修复（ORIG 0x0867dba0 实形）：超时判定用
            //   `_checkTimeoutItem(mail->GetPackageItem())` 值拷贝（ORIG -4 GetPackageItem、
            //   -1 _checkTimeoutItem；我方原用 mail->IsTimeOver(nowSec)+getCurSec）；
            //   移除索引用 `mail->getMailIdx()`（ORIG -2 getMailIdx；我方原 GetLetterId）；
            //   无效物品分支每字段重调 GetPackageItem（P157）。
            bool remove = false;
            Inven_Item pkg = mail->GetPackageItem();
            if (_checkTimeoutItem(pkg))
            {
                remove = true;
            }
            else
            {
                // ORIG 0x0867dd6a `cmp $-1,%eax; setne %al; test; je` ⇒ 先判 != -1
                if (_isMatchedItem(mail->GetPackageItem().m_addInfo, list) != -1)
                {
                    remove = true;
                }
                else
                {
                    if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                            mail->GetPackageItem().m_addInfo))
                    {
                        // b28d-solo-CUser p167：ORIG 实例化
                        // `make_pair<unsigned long, int>(unsigned long&&, int&&)`（右值形）
                        // ⇒ 显式 cast 成右值（P61/P315）。
                        mailDel.push_back(std::make_pair(
                            (unsigned long)mail->GetPackageItem().m_addInfo,
                            (int)mail->GetPackageItem().m_addInfo2));
                        remove = true;
                    }
                }
            }
            // ORIG 0x0867de56：rmList[0] 自增后写入，无上界检查
            if (remove)
            {
                rmList[++rmList[0]] = mail->getMailIdx();
                ((WongWork::CMailBox*)m_premium.m_mailBox)
                    ->RemoveMail(mail->getMailIdx());
            }
        }
        if (rmList[0] != 0)
        {
            WongWork::CMailBoxHelper::ReqDBRemoveMail(this, rmList, false);
            PacketGuard packet;
            // b28d-G3C 2026-09-10（P183 去具名指针）：ORIG 0x0867df?? 四处
            //   `lea -0x198(%ebp),%eax; mov %eax,(%esp)` **每处重算** PacketGuard
            //   地址（`mov -0x1ec(%ebp),...` 之后直接调用），无 `InterfacePacketBuf*`
            //   具名槽；我方原 `pkt` 独占 -0x20 槽（多 store+3×load，并把 PacketGuard
            //   从 -0x198 顶到 -0x160、rmList 从 -0x1ec 顶到 -0x1f4）。
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x62);
            ((InterfacePacketBuf*)&packet)->put_int((int)rmList[0]);
            for (int i = 1; i <= rmList[0]; ++i)
                ((InterfacePacketBuf*)&packet)->put_int((int)rmList[i]);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
        RewardItem2DeleteInvalidItem("Mail", mailDel);
    }
    _deleteSpecificRedeemItem(list);
}

// 0x08652c8e
// ORIG：联动角色断线处理。标志开启时：若数据加载/建角/删角被锁定则记错误日志；
// 否则把联动角色槽位移到列表尾（ChangeCharacSlot 两次）、清标志并刷新角色视图。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x08652c8e | _ZN5CUser22doLinkCharacDisconnectEv
#include "CUser/doLinkCharacDisconnect.inc"
{
    // b28d-solo-CUser：callee 已 bool 化（CUser.h:2718 Main 代落）⇒ 正向包裹复现
    //   ORIG 0x08652c8e `call; test %al,%al; je <跳过 body>`（P292 表 if(f()) bool；
    //   负向 `==0) return;` 反产 `xor $1; test; jne`）。
    if (isLinkCharacDisconnectFlag())
    {
    // ORIG 0x08652cb2..0x08652cf5：**两段** `locked = locked || f()` 各发一次
    //   `mov $1,%eax / mov $0,%eax / mov %al,-0xd`（4.1 || 汇聚物化落槽，P198/P214）。
    bool locked = isLocked4DataLoad() || isLockedCreateCharac();
    locked = locked || isLockedDeleteCharac();
    if (locked)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x1cc1,
            "LINK_LOG : checkLinkCharacDisconnect ERROR!! m_id(%s)",
            NumberToString(get_acc_id(), 0));
        return;
    }
    // ORIG 0x08652d4e `movsbl %al,%eax; mov %eax,-0xc`（getter 本体 movzbl 0x703da
    //   0x08697408 ⇒ 源码在使用点显式 (signed char) 截断）；调用点各自
    //   `mov -0xc,%eax; sub $1,%eax` + `lea -1(%eax),%edx`（size-1 直传，勿加 cast）。
    //   P48：两 ChangeCharacSlot 各自重调 size()。
    int slotIdx = (signed char)getDisconnectLinkCharacSlotIdx();
    ChangeCharacSlot((unsigned int)(slotIdx - 1), m_characList.size() - 1);
    ChangeCharacSlot((unsigned int)(slotIdx - 1), m_characList.size() - 1);
    setLinkCharacDisconnectFlag(false);
    UpdateCharacView();
    }
}

// 0x08651b7a
// ORIG：把 m_characList（vector<_Charac_info> @ +0x796e8）中下标 a 的元素移动到
// 下标 b 处（a<b 时先 insert(begin()+b+1, v[a]) 再 erase(begin()+a)；
// a>b 时 insert(begin()+b, v[a]) 再 erase(begin()+a+1)），随后
// enableSaveCharacView(&m_characterView)。越界（a/b >= size）直接返回。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651b7a | _ZN5CUser16ChangeCharacSlotEjj
#include "CUser/ChangeCharacSlot.inc"
{
    // b28d-GrpD P59/P222：ORIG +0x12..+0x3f 是**单条件取反 + 早期 return**（bool 物化），
    //   非嵌套 if：`call size; cmp 0xc(%ebp),%eax; jbe +0x31`（A 假→mov $1）→
    //   `call size; cmp 0x10(%ebp),%eax; ja +0x38`（B 真→mov $0）→ `test %al,%al;
    //   jne +0x266`（跳函数尾）。⇒ 源码 `if (!(a < size && b < size)) return 0;`
    //   我方旧双层嵌套 if 产两段独立分支、无 bool 物化（ORIG 的两段短路口才落 $1/$0 岛）。
    if (!(a < m_characList.size() && b < m_characList.size()))
        return;
    if (a < b)
    {
        // b28d-solo-CUser p167：ORIG 0x08651b7a 的 `begin()+b+1` 是**两次**
        //   operator+（迭代器 +b 再 +1；我方 `begin()+(b+1)` 仅一次 ⇒ MISSING ×2）。
        m_characList.insert(m_characList.begin() + b + 1, m_characList[a]);
        m_characList.erase(m_characList.begin() + a);
    }
    else
    {
        m_characList.insert(m_characList.begin() + b, m_characList[a]);
        m_characList.erase(m_characList.begin() + a + 1);
    }
    m_characterView.enableSaveCharacView();
    return;
}

// ORIG 0x086973dc：设置联动角色断线标志（bool 字段 this-先直写 movb）。
#include "CUser/setLinkCharacDisconnectFlag.inc"
{
    m_breakAway.m_field15 = flag;
}

// ---- UpdateCharacView 链（报告 docs/class_func_reports/CUser/UpdateCharacView.md、
// updateCharacViewVisibleData.md、checkRentedItem.md；ORIG 0x8651740/0x8651322/0x8677188）----

// _Charac_info avatar 装备槽源视图（位于每角色 CInventory 子对象内部；
// updateCharacViewVisibleData 循环 1/2 按 info + i*0x3d + 0xf6 寻址）。
// pack(1)：ORIG 0x86513b6 `mov 0x2(%eax),%eax` / 0x86513c4 `mov 0x7(%eax),%ebx`
// 非对齐 int 读实证。
#pragma pack(push, 1)
struct AvatarSlotSrcView
{
    char m_pad0[2];
    int m_itemIndex;                    // +0x02（非 0 才处理）
    char m_pad6;
    int m_expansionKey;                 // +0x07（GetExpansionInfo 键）
};
#pragma pack(pop)

// 当前角色不匹配时的可见缓存源槽（info + k*8 + 0x75e，12 槽直拷）。
// pack(1)：ORIG 以 pEntry+0x2/+0x7 非对齐读 int（0x86513b6/0x86513c4）。
#pragma pack(push, 1)
struct VisibleCacheSlotSrcView
{
    int m_itemIndex;
    short m_fieldA;
    short m_fieldB;
};
#pragma pack(pop)

// SIG_UPDATE_CHARAC_VIEW 单记录视图（stride 0x94）：首槽 rec[0] 头 = acc_id，
// rec[i>0] 头 ← 前一角色 _Charac_info+0x12ad；数据体 CHARAC_LOGIN_DATA(0x90)。
// [CUserWorker 2026-09-11 复核] `_ZNSt12_Vector_baseIN20InstanceRentalSystem10RentalInfoESaIS1_EED1Ev`
//   残差（ORIG 40 vs 我方 25）根因 = **库侧可抛性判定**，非本 TU 源码：
//   · ORIG 0x08239093 `call _M_deallocate` → 0x08239098 `jmp` 正常尾；0x0823909a 起 landing pad
//     （`mov %edx,%ebx; mov %eax,%esi` → `call ..._Vector_implD1Ev` → `call _Unwind_Resume`），
//     即 ORIG 用 4.1 头集时 `allocator::deallocate`（→ `operator delete`）被判**可抛**；
//   · 4.4 的 `operator delete(void*)` 带 `throw()` ⇒ gcc 判 nothrow ⇒ 不发射 LP / `_Vector_implD1Ev` /
//     `_Unwind_Resume`。
//   · `RentalInfo` 的 triviality 两侧一致（`_Destroy_aux<true>` ILb1；ORIG 0x0823e5c7 W
//     `_Destroy_aux<true>::__destroy<RentalInfo*>` = 我方同名实例）⇒ **非 P342 候选**，删/改
//     `~RentalInfo()` 无收益。
//   ⇒ 库头 lowering 差异，本 TU 无源码旋钮（除非替换 libstdc++ 头集，属构建口径）。判 N。
// FindRentalInfoByItemIndex：ORIG 局部仿函数（W 0x8697d2c/0x8697d3a），按
// RentalInfo.m_itemIndex 匹配。
struct FindRentalInfoByItemIndex
{
    explicit FindRentalInfoByItemIndex(int itemIndex) : m_itemIndex(itemIndex) {}
    bool operator()(const InstanceRentalSystem::RentalInfo& info) const
    {
        return info.m_itemIndex == m_itemIndex;
    }

    int m_itemIndex;
};

struct SigUpdateCharacViewRecord
{
    int m_headField;                    // +0x00（rec[0]=acc_id；rec[i>0]←前一角色 expireSec）
    CHARAC_LOGIN_DATA m_data;           // +0x04（0x90）
};
typedef char sig_charac_view_rec_size_ok[(sizeof(SigUpdateCharacViewRecord) == 0x94) ? 1 : -1];

// ORIG T 0x08677188：在 m_rentalInfo[param] 视图中查找 characNo 的租借记录里
// 是否存在 itemIndex 匹配项。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677188 | _ZN5CUser15checkRentedItemEjii
#include "CUser/checkRentedItem.inc"
{
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> > rentalMap;
    GetInstanceRentalInfo(rentalMap, param);

    // b28d-solo-CUser p167：ORIG 0x08677188 用非 const iterator
    //   （vector::begin/end、find_if<iterator>、_Rb_tree_iterator::operator*）；
    //   我方 const_iterator 致 const 版符号 ⇒ 改非 const + `(*it).second`。
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::iterator it =
        rentalMap.find(characNo);
    if (it == rentalMap.end())
        return false;

    std::vector<InstanceRentalSystem::RentalInfo>& infos = (*it).second;
    // CuC P219 求值序旋钮（ORIG 0x08677188 直证）：ORIG 序 = pred(-0x1c) → end(-0x18)
    //   → begin(-0x14) → find_if(-0x20) → end(-0x10) → op!=，即**左操作数先于右**。
    //   4.4 O0 对 `A != B` 恒先求 B（右）；把 `infos.end()` 放左侧即复现 ORIG 全序
    //   （探针 /tmp/CuC/probe/p1.cpp f2 五调用槽 -24/-20/-16/-28/-12 与 ORIG 逐槽同向）。
    //   `!=` 两侧求值互换安全：find_if 只读 vector，end() 结果在 find_if 后仍有效。
    return infos.end() != std::find_if(infos.begin(), infos.end(),
                                       FindRentalInfoByItemIndex(itemIndex));
}

// updateCharacViewVisibleData 循环 1/2 源视图：info + i*0x3d + 0xe0 元素
// （ORIG 0x865139f/0x8651495 imul $0x3d + add $0xe0 实证），字段 +0x18/+0x1d。
#pragma pack(push, 1)
struct CharacAvatarEntry
{
    char m_pre[0x18];
    int m_itemIndex;                    // +0x18
    char m_pad1c;
    int m_expansionKey;                 // +0x1d（非对齐 int，ORIG 实证）
    char m_rest[0x3d - 0x21];
};
#pragma pack(pop)
typedef char avatar_entry_size_ok[(sizeof(CharacAvatarEntry) == 0x3d) ? 1 : -1];
// expansion 数据对（ORIG 0x8651453 `movzwl (%eax),%edx` / 0x8651467
// `movzwl 0x2(%eax),%edx` 成员 disp 形态）。
struct ExpansionPair
{
    unsigned short m_a;                 // +0x00
    unsigned short m_b;                 // +0x02
};
// 循环 3 可见缓存元素（ORIG 0x86515b5 `mov 0xe(%eax,%edx,8)` scale：
// base=info+0xe，索引 k+0xea，stride 8，字段 +0/+4/+6）。
struct VisibleCachePair8
{
    unsigned int m_itemIndex;           // 绝对 +0x75e
    unsigned short m_fieldA;            // 绝对 +0x762
    unsigned short m_fieldB;            // 绝对 +0x764
};
typedef char vis_pair8_size_ok[(sizeof(VisibleCachePair8) == 8) ? 1 : -1];
// [b28d-G5] 同缓存的**成员数组视图**：ORIG 0x86515b5 为 `mov -0xc(%ebp),%edx;
//   mov 0xc(%ebp),%eax; add $0xea,%edx; mov 0xe(%eax,%edx,8),%edx` —— 索引寄存器
//   = (k + 0xea) 整表达式、常量 0xe 落位移，只有**数组下标 + 成员**形才这样发射。
//   我方原形 `(cast)((char*)&info + 0xe) + k + 0xea` 是**指针 rvalue 算术**：前端把
//   0xea*8 与 0xe 折成常量 0x75e，再 `shl $3; add $0x75e; add %edx,%eax`（+3 条/站）。
#pragma pack(push, 1)
struct CharacViewCacheView
{
    char m_pad[0xe];
    VisibleCachePair8 m_cache[0x100];   // [k + 0xea]（k<12 ⇒ 0xea..0xf5，界内）
};
#pragma pack(pop)

// ORIG T 0x08651322：填充单角色的可见数据区（avatar 外观槽 / 租赁槽 /
// 非当前角色缓存直拷 / 改名状态）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08651322 | _ZN5CUser27updateCharacViewVisibleDataERK12_Charac_infoR17CHARAC_LOGIN_DATA
// [b28d-GrpD 判词·N] ORIG 0x08651322：残差全是**同址等价形式差** ——
//   ① ORIG `imul $0x3d; add $0xe0; add $0x16`（=0xf6）vs 我方折叠 `add $0xf6`；
//   ② ORIG `add $0xea,%edx; mov 0xe(%eax,%edx,8)` vs 我方 `shl $3; add $0x75e; add`
//      （同址：0xe+0xea*8 = 0x75e，副偏移 0x12 = 0xe+4、0x14 = 0xe+6）；
//   ③ 0x75e/0x762/0x764 与 ORIG 逐字节相等 ⇒ 字段偏移/极性/符号性全一致，无 bug。
#include "CUser/updateCharacViewVisibleData.inc"
                                        struct CHARAC_LOGIN_DATA& data)
{
    memset(data.m_avatarSlots, 0, 0x60);
    if ((getCurCharacR() != 0 &&
         getCurCharacR()->m_characNo ==
             reinterpret_cast<const CharacListElemView&>(info).m_characNo)
            ? true : false)
    {
        data.m_renameOrViewState = getRequestRenameState();

        // 槽 0..8：avatar 装备槽（有物品且扩展信息存在时写索引/颜色对）。
        for (int i = 0; i < 9; ++i)
        {
            const AvatarSlotSrcView* slot =
                reinterpret_cast<const AvatarSlotSrcView*>(
                    i * 0x3d + 0xe0 + reinterpret_cast<const char*>(&info) +
                    0x16);
            if (slot->m_itemIndex != 0)
            {
                const void* expansion =
                    reinterpret_cast<CInventory*>(
                        reinterpret_cast<char*>(const_cast<_Charac_info*>(&info)) +
                        0xda)
                        ->GetAvatarItemMgrR()
                        ->GetExpansionInfo(slot->m_expansionKey);
                if (expansion != 0)
                {
                    if (reinterpret_cast<CInventory*>(
                            reinterpret_cast<char*>(
                                const_cast<_Charac_info*>(&info)) + 0xda)
                            ->GetClearAvatar(i) != 0)
                        data.m_avatarSlots[i].m_itemIndex =
                            reinterpret_cast<CInventory*>(
                                reinterpret_cast<char*>(
                                    const_cast<_Charac_info*>(&info)) + 0xda)
                                ->GetClearAvatar(i);
                    else
                        data.m_avatarSlots[i].m_itemIndex = slot->m_itemIndex;
                    data.m_avatarSlots[i].m_expansionA =
                        reinterpret_cast<const ExpansionPair*>(expansion)->m_a;
                    data.m_avatarSlots[i].m_expansionB =
                        reinterpret_cast<const ExpansionPair*>(expansion)->m_b;
                }
            }
        }

        // 槽 9..11：租赁位（任一租期类型命中则整槽清零，否则仅写索引）。
        for (int i = 9; i < 12; ++i)
        {
            if (checkRentedItem(
                    (unsigned int)reinterpret_cast<const CharacListElemView&>(info)
                        .m_characNo,
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex,
                    1) ||
                checkRentedItem(
                    (unsigned int)reinterpret_cast<const CharacListElemView&>(info)
                        .m_characNo,
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex,
                    2))
            {
                memset(&data.m_avatarSlots[i], 0, 8);
            }
            else
            {
                data.m_avatarSlots[i].m_itemIndex =
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex;
                data.m_avatarSlots[i].m_expansionA = 0;
                data.m_avatarSlots[i].m_expansionB = 0;
            }
        }
    }
    else
    {
        // 非当前角色：从可见缓存表（info + k*8 + 0x75e）12 槽直拷。
        for (int k = 0; k < 12; ++k)
        {
            data.m_avatarSlots[k].m_itemIndex =
                reinterpret_cast<const CharacViewCacheView*>(&info)
                    ->m_cache[k + 0xea].m_itemIndex;
            data.m_avatarSlots[k].m_expansionA =
                reinterpret_cast<const CharacViewCacheView*>(&info)
                    ->m_cache[k + 0xea].m_fieldA;
            data.m_avatarSlots[k].m_expansionB =
                reinterpret_cast<const CharacViewCacheView*>(&info)
                    ->m_cache[k + 0xea].m_fieldB;
        }
        data.m_renameOrViewState =
            reinterpret_cast<const CharacListElemView&>(info).m_viewState;
    }
}

// ORIG T 0x08651740：角色视图变化落库包。频道 !=7 且 isSaveCharacView() 时组
// SIG_UPDATE_CHARAC_VIEW（header 6 + UID），memset 全块后逐 _Charac_info 填
// 记录（stride 0x94）并调 updateCharacViewVisibleData，投递 DB_Q 后清保存标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651740 | _ZN5CUser16UpdateCharacViewEv
// [b28d-GrpD 判词·结构化 N] ORIG 0x08651740：残差为槽化引起的地址计算式差异 ——
//   ORIG 每次 `imul $0x94; lea (%R,%R,1); add $0x10/$0x4` 逐步折基址，我方用
//   `imul $0x94; add -0xS` + 折叠副偏移（0x1c=0x10+0xc、0x22=0x10+0x12、0x98=0x94+4）；
//   两式同址、位掩码（$BIG 的 and/or 位域改写）与字段偏移一致 ⇒ 无语义缺口。
#include "CUser/UpdateCharacView.inc"
{
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    if (!m_characterView.isSaveCharacView())
        return;

    // b28d-lane5：ORIG 0x08651740 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1b1f), true);

    // b28d-solo-CUser p167：ORIG 0x08651740 两次 `CStreamGuard::operator*()` 后
    //   `operator<<`（MISSING ×2）⇒ 显式 operator* 调用链（P132）。
    guard.operator*()->operator<<((int)6);
    guard.operator*()->operator<<((int)GetUID());

    // b28d-CUser p167：ORIG 0x086517f7 显式调 CStreamGuard::operator->()（ptEv）
    //   再 GetInBuffer（P103）；直接成员调用少一次 call。
    SIG_UPDATE_CHARAC_VIEW* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_CHARAC_VIEW>();
    memset(sig, 0, sizeof(SIG_UPDATE_CHARAC_VIEW));

    sig->m_serverGroup = (unsigned char)GetServerGroup();

    SigUpdateCharacViewRecord* recs =
        reinterpret_cast<SigUpdateCharacViewRecord*>(sig);
    recs[0].m_headField = get_acc_id();   // ORIG `mov %eax,(sig)` 4B @+0x00

    // ORIG 无任何缓存：目标记录每次都按 sig + recIdx*0x94 + 偏移重算，源字段每次
    // 都重新 it.operator->()（Ghidra C 每个字段前都有一次 __normal_iterator::
    // operator->）；begin()/end() 各调一次转成 const_iterator 后再进循环。
    int recIdx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator endIt = m_characList.end();
    for (; it != endIt; ++it)
    {
        recs[recIdx].m_data.m_characNo = it->m_characNo;
        strncpy(recs[recIdx].m_data.m_name, it->m_name, 0x1e);
        recs[recIdx].m_data.m_job = (unsigned char)it->m_field26;
        recs[recIdx].m_data.m_field19 = (unsigned char)it->m_field29.m_growthType;
        recs[recIdx].m_data.m_level = it->m_level;

        // b28d-solo-CUser p167：ORIG 0x08651740 全字段走 `it.operator->()`
        //   （MISSING ×5；我方 `*it` 走 operator*，EXTRA ×5）⇒ 改显式 operator-> 形。
        updateCharacViewVisibleData(*(it.operator->()), recs[recIdx].m_data);

        recs[recIdx].m_data.m_lastPlayTime =
            (long)(*reinterpret_cast<const CharacListElemView*>(
                        it.operator->())).m_createTime;
        recs[recIdx].m_data.m_field8c =
            (*reinterpret_cast<const CharacListElemView*>(
                 it.operator->())).m_fielde5d;

        // bit0 ← 禁玩标记（info+0x1240 非零）。
        recs[recIdx].m_data.m_flags = (unsigned char)(
            ((unsigned int)recs[recIdx].m_data.m_flags & 0xfeu) |
            (((*reinterpret_cast<const CharacListElemView*>(
                    it.operator->())).m_field1240 != 0) ? 1u : 0u));

        recs[recIdx].m_data.m_field1d = it->m_field14b7;
        recs[recIdx].m_data.m_field1e = it->m_field14b8;

        // bit4 ← info+0xe40 的 bit1（ORIG shl $6 / sar $7 取位）。
        recs[recIdx].m_data.m_flags = (unsigned char)(
            ((unsigned int)recs[recIdx].m_data.m_flags & 0xefu) |
            (((((unsigned int)(unsigned char)
                    (*reinterpret_cast<const CharacListElemView*>(
                         it.operator->())).m_fielde40
                << 6) >> 7) & 1u) << 4));

        recs[recIdx + 1].m_headField =
            reinterpret_cast<const CharacListElemView&>(*it).m_expireSec;

        ++recIdx;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_characterView.disableSaveCharacView();
}

// 0x086786be
// ORIG：放弃惩罚通知。非轻量服、+0x711d8 标志为 0（推断：惩罚冷却标志）、
// 未受影响 premium(0xc)、等级 > 0x11、moveSpace != 1 时，
// 发包(0x21)并 setCurCharacStamina(10)。
#include "CUser/giveup_panalty.inc"
{
    // ORIG 0x86786cb `test %al,%al; jne` 无 setne 物化 ⇒ IsLightServer bool 返回（声明 DataManagerInitDepends.h:42/62 待 bool 化；(bool)cast 在 char 声明下反产 setne，故不加）
    if (IsLightServer())
        return;
    // ORIG 0x086786d6 `movzbl 0x711d8; test; jne`（bool 直读；= CUser::m_gmUserFlag）
    if (m_gmUserFlag)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    if (get_charac_level() <= 0x11)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    // ORIG 0x8678742 cmp $0x1;setne;test;je merge：物化 true→fall body（probe20 b 形）
    if ((bool)(getMoveSpace() != 1))
    {
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x21);
    setCurCharacStamina((unsigned char)0xa);
    ((InterfacePacketBuf*)&packet)->put_byte(10);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
    }
}

// 0x08658910
// ORIG：登出清理——清登录时间/播放经验/多开彩票失败计数、移除 GM 标记、
// 复位 uid/槽位/自增 ID/账号名、断网并整体 reset。
#include "CUser/log_out.inc"
{
    m_tradeCount = 0;                     // CUser+0x8d10c（ORIG movl $0）
    resetPlayExpAdd();
    set_multiboxLotteryItemFailCnt(0);
    // ORIG 0x8658948/0x8658955 两次 call get_acc_id：右实参先评估存 %ebx→0x8(%esp)，左→%eax→0x4(%esp)
    // （gcc 实参右起评估；0x8 槽值非 0 常量而是第二调用结果，ORIG cmp 语义等价——原码即双调用）
    GlobalData::s_pGMAccounts_->removeGM(get_acc_id(), get_acc_id());
    m_field796f8 = 0;
    m_chState = (ch_state)0;
    m_accId = 0;
    CUserGlobalInfoHandleInstance()->reset_uniqueid_flag(m_field704a8);
    m_field704a8 = 0;
    SetSlotIDX(0);
    SetIncreID(0);
    memset(m_accName, 0, 0x15);
    m_field8cfc8 = 0;
    m_field8cfdc = 0;               // 状态标志字节（ORIG movb $0,0x8cfdc）
    m_network.disconnect();
    reset();
}

// 0x086487ea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086487ea | _ZN5CUser17prepareDisconnectEv
#include "CUser/prepareDisconnect.inc"
{
    // 0x086487ea 主分支（prepareDisconnect.md 反编译 C 全文转写）：state<2 早退；
    // 非人证用户记 hack 0x3b9、SSO 强存计数 + DB_ForceSaveSimpleSSO；安全卡/密保
    // 请求态为 1 时 Dispatcher_SecurityCardAuthCancel::process(this)；antibotChecker
    // vtable +0x08 四参注销 / +0x24 两参注销；DBUpdateExitTime(accId, uid, ts)。
    if (get_state() < 2)
        return;
    if (!isHumanCertified())
    {
        setHumanCertified(true);
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x3b9, 1, 0, 0);
        m_humanCertifyErrorCnt += 1;   // ORIG this+0x8e0fc 实证：SSO 强制保存计数
        DB_ForceSaveSimpleSSO::makeRequest((unsigned int)get_acc_id(), m_humanCertifyErrorCnt);
    }
    // b28d-lane5 复测（P226 语句表达式）：ORIG 0x08648a?? 无 cancelAuth 槽——两 getter
    //   短路后 `mov $1,%eax / mov $0,%eax / test %al,%al / je`（零槽物化岛）；
    //   旧具名 bool 形多 `movb $0x0,-0x15 / movb $0x1,-0x15` 与帧 +0x10。
    if (({ getSecurityCard()->getRequestState() == 1 ||
           getPad()->getRequestState() == 1; }))
        Dispatcher_SecurityCardAuthCancel::process(this);

    // b28d-lane5 复测（P227 全表达式临时链）：ORIG 0x08648a5x 在 cMyTrace ctor **之前**
    //   右起求 varargs 并以 %esi 跨 ctor 保活（`mov %eax,%esi` … `mov %esi,0xc(%esp)`）。
    cMyTrace(__PRETTY_FUNCTION__, 0x8ed, 0)
        ("[Taiwan, Antibot] Delete client. (sequence:%d, mid:%u)",
         G_CEnvironment()->getProcessSequence(), (unsigned int)get_acc_id());

    // §4.5 例外：antibot 类未建模（见 AntibotSend 处 b22 单），
    // Fn3=+0x08（DeleteClient 上报）/ Fn1=+0x24（DeleteClient 尾标）。
    typedef void (*Fn3)(void*, int, int, unsigned int);   // vptr+0x08
    typedef void (*Fn1)(void*, unsigned int);             // vptr+0x24
    // b28d-G3C 2026-09-10（P183 去具名缓存）：ORIG 0x086488f?/0x0864891? 对
    //   `GlobalData::s_antibotChecker_` **每次重算**——`mov 0x941f7e4,%eax;
    //   mov (%eax),%eax; add $0x8,%eax; mov (%eax),%esi`（取 vtbl[2]）以及调用前
    //   `mov 0x941f7e4,%edx`（作 this 实参）；+0x24 段同形（vtbl[9]）。
    //   我方原有 `void* checker`(-0x14) 与 `void** vtbl`(-0x10) 两个独占具名槽
    //   （每处多 store+load，帧 0x30→0x40）。改为全表达式重算。
    ((Fn3)(*(void***)GlobalData::s_antibotChecker_)[2])(
        GlobalData::s_antibotChecker_, G_CEnvironment()->getProcessSequence(), 0,
        (unsigned int)get_acc_id());
    ((Fn1)(*(void***)GlobalData::s_antibotChecker_)[9])(
        GlobalData::s_antibotChecker_, (unsigned int)get_acc_id());

    // ORIG 0x086487ea：直读 m_garenaAuthData(+0x6ef92) 调 getTimeStamp，不经 getGarenaAuthData；
    //   2026-09-11（H3CUser，P183）：ORIG 全函数无 `mov %eax,-0xc(%ebp)` ts 具名槽
    //   （我方旧形多 store+load 一对）⇒ getTimeStamp 内联进实参位。
    DBUpdateExitTime::makeRequest(
        (unsigned int)get_acc_id(),
        (unsigned int)(unsigned short)get_unique_id(),
        m_garenaAuthData.getTimeStamp());
}

// 0x0865db6c
// ORIG：按物品空间号生成并发送对应列表包：
//   0/1 → CInventory::MakeItemList(1/2)、2 → CCargo::MakeItemList、
//   7 → 生物列表 + MakeItemList(3)、0xc → CAccountCargo::SendItemList（不存在时返回 1）。
// b28d-lane5 N封顶: P0 清绑连带——同 TU callee（IsExistAccountCargo 等）清绑后帧变化，
// 4.4 按 P210 精确继承 outgoing ⇒ 本函数帧 0x20→0x30、PacketGuard 槽 -0x14→-0x1c；
// 另 v^=1 段清绑后落栈槽物化（P168a④）。| ORIG 0x0865db6c | _ZN5CUser14send_itemspaceEi
#include "CUser/send_itemspace.inc"
{
    // b28d-G3C 2026-09-10（P183 去具名槽）：ORIG 每个 case 的形态是
    //   `call MakeItemList; xor $0x1,%eax; test %al,%al; je <继续>`
    //   （0x0865dbb9/+0x4d、0x0865dbf0/+0x84、0x0865dc1f/+0xb3、0x0865dc4f/+0xe3、
    //    0x0865dc82/+0x116）——**无任何暂存槽**。我方原形用 5 个具名
    //   `unsigned char vN` 各占 1 字节槽（-0xd/-0xc/-0xb/-0xa/-0x9），每处多
    //   `mov %al,-0xN; xorb $1,-0xN; cmpb $0,-0xN` 三条，并把 PacketGuard 从
    //   -0x14 顶到 -0x1c（帧 0x20→0x30）。改为内联表达式：内层 `^1` 保 int 域
    //   32 位 xor，外层 (unsigned char) 截断保 `test %al,%al` 八位判零。
    PacketGuard packet;
    switch (space)
    {
    case 0:
    {
        if ((unsigned char)(getCurCharacInvenR()->MakeItemList((INVEN_TYPE)1,
                                                              &packet) ^ 1))
            return 0;
        break;
    }
    case 1:
    {
        if ((unsigned char)(getCurCharacInvenR()->MakeItemList((INVEN_TYPE)2,
                                                              &packet) ^ 1))
            return 0;
        break;
    }
    case 2:
    {
        if (!getCurCharacCargoR()->MakeItemList(&packet))
            return 0;
        break;
    }
    case 7:
    {
        if ((unsigned char)(getCurCharacInvenR()->GetCreatureMgrR()
                                ->SendCreatureItemList() ^ 1))
            return 0;
        if ((unsigned char)(getCurCharacInvenR()->MakeItemList((INVEN_TYPE)3,
                                                              &packet) ^ 1))
            return 0;
        break;
    }
    case 0xc:
        // ORIG 0x865dc90 段：`call IsExistAccountCargo; test %al,%al; je <ret1岛>`——
        // 真支 fall-through 直取成员对象地址（`mov 0x8(%ebp),%eax; add $0x6effc,%eax`）
        // 调 m_accountCargo.SendItemList()，⇒ 原码是成员对象直调而非 GetAccountCargo()
        // （此处 4.1 头文件内联的 getter 已在我们 TU 外联，多出一次 call）。
        if (IsExistAccountCargo())
            return m_accountCargo.SendItemList();
        return 1;
    default:
        return 0;
    }
    Send(packet);
    return 1;
}

// 0x0868c170（返回 1 已通知 / 0 未满足条件）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868c170 | _ZN5CUser31send_ontime_reward_start_notifyEv
#include "CUser/send_ontime_reward_start_notify.inc"
{
    // 0x0868c170 主分支（send_ontime_reward_start_notify.md）：事件 0x33 空 →
    // 日志+0；非活跃 / 奖励已开始（get_reward_state!=0）→ 0；日志块（文件名
    // ./log/<env>/OnTimeEvent + "On Charac Login"）后，ontime_id > 本地
    // last_recv_idx 且剩余时间 >=0 才发包 header(0,0x105) 单 int，返回 1。
    // b28d-GrpD 按 ORIG 0x0868c170 直证改写（4 处）：
    //  ① ORIG 只有**单个**事件指针槽 -0x24（GetRepeatEvent 结果直接落槽），+0x095
    //     `mov -0x24(%ebp),%eax` 直接作 this 调 get_reward_state ⇒ 无 ev 二次拷贝
    //     （我方旧 `CEventBase* eventRaw` + `COnTimeEvent* ev` 多占 -0x20 且多 2 mov）。
    COnTimeEvent* ev = (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // 2026-09-11 N8CUser P214①（正条件包裹）：ORIG 0x0868c194 `cmp DWORD PTR
    //   [ebp-0x24],0x0; je 0x0868c1c5` ⇒ fall-through 是 ev!=0 主体，而 trace
    //   早退块落 if 体之后（0x0868c1c5，紧跟 IsEventing 的 return-0 出口）；
    //   旧早退形（`if (ev==0) { trace; return 0; }`）4.4 产 `jne` + fall-through
    //   = trace 块 ⇒ 条件方向与块序双反。
    if (ev != 0)
    {
    if (!ev->IsEventing((CUser*)0))
        return 0;
    }
    else
    {
    cMyTrace(__PRETTY_FUNCTION__, 0xa7f7, 5)
        ("[ON_TIME_EVENT] send_ontime_reward_start_notify() error");
    return 0;
    }
    if (ev->get_reward_state() != 0)
        return 0;
    // b28d-solo-CUser p167：ORIG 0x0868a7f7 内联 rep stos（我方 EXTRA memset）。
    // [b28d-tu-P253] REAL-BUG：ORIG 0x0868c223-0x0868c237 `lea -0x148(%ebp),%ebx
    //   ; mov $0x0,%eax; mov $0x40,%edx; mov %ebx,%edi; rep stos` ⇒ %edx 是
    //   **dword 个数**：0x40×4 = **256 字节**（基 -0x148..-0x48），我方原
    //   `char fileName[0x40]` 只清 64B 且 sprintf("./log/%s/OnTimeEvent",
    //   get_file_name()) 的拼接路径可越过 64B ⇒ 少清 192B + 栈溢出风险。
    //   [取证 §3.2 兜底] CUser/send_ontime_reward_start_notify.md 为基线参考，
    //   rep stos 长度以 objdump 0x0868c223 直证为改据；[修正版 p167 复判]
    //   24=24 零差集（无 clone 别名）。
    char fileName[0x100] = {0};
    sprintf(fileName, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
    //  ④ ORIG +0x0fe..+0x138 顺序 = get_charac_no → get_acc_id → NumberToString →
    //     CMyFileLog ctor(-0x2c)，即实参先于 ctor 求值 ⇒ 源码是**全表达式临时链**
    //     `CMyFileLog(...)(...)`（P227/P37）；我方旧具名 log 在 sprintf 后立即构造。
    CMyFileLog(__PRETTY_FUNCTION__, 0xa803)(
        fileName, "On Time Event : On Charac Login(%s, %u)",
        NumberToString(get_acc_id(), 0), get_charac_no(-1));
    //  ② ORIG +0x17c `cmp %eax,%ebx; setg %al; test; je 0868c3ac`，而 +0x23c(0x0868c3ac)
    //     是 `mov $0x1,%eax`（公共出口）⇒ **id <= last_recv_idx 时函数返回 1**；
    //     我方旧 `if (id <= last) return 0;` 返回值相反（真语义 bug）。改正向包裹。
    if (ev->get_ontime_id() > get_ontime_last_recv_idx())
    {
        //  ③ ORIG +0x187 `movl $0x0,(%esp); call time; mov %eax,-0x20(%ebp)` 先于
        //     get_reward_end_time，随后 `sub -0x20(%ebp),%eax` ⇒ 源码具名 now（P0）；
        //     我方旧形 time 在右侧、gcc 求值序相反（且无 -0x20 槽）。
        int now = (int)time(0);
        int remain = (int)ev->get_reward_end_time() - now;
        if (remain < 0)
            return 0;
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x105);
        ((InterfacePacketBuf*)&packet)->put_int(remain);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
    }
    return 1;
}

// 0x0866c46a:技能信息包(send_skill_info.md)。
// 用具名语义字段访问：玩家技能经 SkillSlot（m_skillSlot 树0 / m_skillSlot2 树1，
// _Mastered_skill{索引,等级}），公会技能经 STGuildDBInfoOnly（m_skillLearnCnt 计数、
// m_skills[16] 5 字节条目 STGuildSkill{技能ID,等级}）。背包命令段经
// findSkillCommandData(索引) 取 STSkillCommandData{count, m_key0..}。
// ORIG 语义要点：公会槽位号 = i + 0x8a；公会技能判定 = 双 grade 非零 且 技能ID > 0xc7。
#include "CUser/send_skill_info.inc"
{
    // ORIG 0x0866c46a: send_skill_info.md
    PacketGuard packet;
    packet.put_header(0, 0x13);

    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0));
    int countIdx0 = packet.get_index();
    int totalCount0 = 0;
    packet.put_byte(0);

    for (int i = 0; i < 0xcc; ++i)
    {
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot[i].m_index);
            packet.put_byte((signed char)reinterpret_cast<const SkillSlot*>(
                getCurCharacSkillR())->m_skillSlot[i].m_level);
            totalCount0++;
            STSkillCommandData cmd0;
            if (findSkillCommandData(reinterpret_cast<const SkillSlot*>(
                    getCurCharacSkillR())->m_skillSlot[i].m_index, cmd0))
            {
                packet.put_byte((signed char)cmd0.m_count);
                for (int k = 0; k < (signed char)cmd0.m_count; ++k)
                {
                    packet.put_byte((signed char)cmd0.m_keyBytes[k]);
                }
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }

    STGuildDBInfoPacketView* guild =
        reinterpret_cast<STGuildDBInfoPacketView*>(GetGuildDBInfo());
    int guildCount = (unsigned char)guild->m_skillSection.m_skillLearnCnt;
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        guildCount = 0;
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skillSection.m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skillSection.m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skillSection.m_skills[i].m_field4);
            totalCount0++;
            packet.put_byte(0);
        }
    }
    packet.put_byte(countIdx0, totalCount0);
    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1));
    countIdx0 = packet.get_index();
    totalCount0 = 0;
    packet.put_byte(0);

    for (int i = 0; i < 0xcc; ++i)
    {
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot2[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot2[i].m_index);
            packet.put_byte((signed char)reinterpret_cast<const SkillSlot*>(
                getCurCharacSkillR())->m_skillSlot2[i].m_level);
            totalCount0++;
            STSkillCommandData cmd1;
            if (findSkillCommandData(reinterpret_cast<const SkillSlot*>(
                    getCurCharacSkillR())->m_skillSlot2[i].m_index, cmd1))
            {
                packet.put_byte((signed char)cmd1.m_count);
                for (int k = 0; k < (signed char)cmd1.m_count; ++k)
                {
                    packet.put_byte((signed char)cmd1.m_keyBytes[k]);
                }
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skillSection.m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skillSection.m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skillSection.m_skills[i].m_field4);
            totalCount0++;
            packet.put_byte(0);
        }
    }
    packet.put_byte(countIdx0, totalCount0);

    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2));
    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3));
    packet.finalize(true);
    Send(packet);
    sendComboSkillList();
}

// 0x0866cb04

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866cb04 | _ZN5CUser17CheckQuestMonsterEii21ENUM_QUEST_ENEMY_TYPE
// [b28d-GrpD 判词·结构化 N] ORIG 0x0866cb04：残差为**我方具名临时整体槽化**导致的
//   块序重排（帧 0x1c0 vs ORIG 0x1d0；-0x78/-0x74 累加器槽 vs ORIG 寄存器保活）。
//   逐块核对：字段偏移（+0x8(%eax,%edx,4) 索引、imul $0x94、0x700 跳表）、
//   极性（jle/jg、je/jne 均落点体互换 = 等价，判据⑦）、算术步长全部同址同值
//   ⇒ 无语义缺口；后续可按 P183 逐点删具名量收敛。
#include "CUser/CheckQuestMonster.inc"
{
    // 0x0866cb04 主分支（CheckQuestMonster.md）：遍历进行中任务表
    // UserQuest::m_quest[i]（<=0 跳过；0x13 上限），Quest::m_type==3/4 或
    // UserQuest::m_trigger[i] 触发位；CheckKillMonster(a,
    // m_authenData.m_characClass@+0x75da, b, type) 掷掉落：rate = 炼金/背包
    // GetQuestItemDropBouns/队伍倍率/premium 加成；掷 tryCount 次 rand(100)；
    // 入包 0xe（新槽 insert / 已有槽累加封顶；needCount<=已有则删掉落项重掷）。
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）
    // ORIG 以 getCurCharacQuestRefR()（= &CUser::m_quest，@+0x858d4）为基址：
    // +0x7538 表 = m_quest[i]、+0x7588 = m_trigger[i]、+0x75da = m_characClass。
    const UserQuest* questRef = getCurCharacQuestRefR();
    for (int i = 0; i <= 0x13; ++i)
    {
        // b28d-lane5 复测：ORIG 无 questIdx 槽（P0 右值内联）——三处使用点各重读
        //   `mov -0x60(%ebp),%edx; mov -0x64(%ebp),%eax; add $0x1d4c,%edx;
        //    mov 0x8(%eax,%edx,4),%eax`（0x0866cb29/0x0866cb41/0x0866cb6b）⇒ 删具名缓存。
        if (questRef->m_quest[i] <= 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[i]);
        if (quest == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x54f7, "G_CDataManager()->find_quest(%d) fail",
                questRef->m_quest[i]);
            continue;
        }
        if (quest->m_type != 4 && quest->m_type != 3 &&
            questRef->m_trigger[i] == 0)
            continue;

        std::vector<MonsterKillItem> kills;
        if (quest->CheckKillMonster(a, questRef->m_authenData.m_characClass, b,
                                    type, kills) != 1)
            continue;

        bool again = true;
        while (again)
        {
            again = false;
            if (kills.empty())
                break;
            // P0 声明序（ORIG idx 80-104）：MonsterKillItem 默认构造 @-0xc0 →
            //   4 个 int 清零 @-0x58/-0x54/-0x50/-0x4c → pick @-0x48 → 短路判定。
            //   结构赋值 `drop = kills[pick]`（非引用绑定——ORIG 有 C2Ev call）。
            MonsterKillItem drop;
            int itemIdx = 0;      // 掉落道具 idx
            int tryCount = 0;     // 掷骰次数
            int dropRate = 0;     // 基础掉率
            int needCount = 0;    // 需要数量（-1=无限）
            int pick = get_rand_int((int)kills.size());
            // P338 块布局：ORIG 条件为 De Morgan 正向形 `!(pick >= 0 && size > pick)`
            //   （汇编 js/jle → mov $1 → test/je SKIP），非 `pick < 0 || size <= pick`
            //   （后者 4.4 生成 jg → mov $0 → test/jne BREAK 的反向块）。
            if (!(pick >= 0 && (int)kills.size() > pick))
                break;
            drop = kills[pick];
            itemIdx = drop.m_field10;
            tryCount = drop.m_field14;
            dropRate = drop.m_field18;
            needCount = drop.m_field1c;

            int bonus = 0;
            for (int s = 0; s < 0x1a; ++s)
            {
                // ORIG 0x0866cd51：`getCurCharacInvenR()->GetInvenRef(0,s)` 走
                //   CInventory::GetInvenRef(int,int) **const**（_ZNK…Eii），返回
                //   const Inven_Item*——无 const_cast（P167 重载选型）。
                const Inven_Item* it =
                    getCurCharacInvenR()->GetInvenRef((INVEN_TYPE)0, s);
                if (it != 0 && it->m_addInfo != 0)
                    bonus += dropRate *
                             G_CDataManager()->m_itemList
                                 ->GetQuestItemDropBouns(
                                     reinterpret_cast<unsigned long&>(
                                         const_cast<Inven_Item*>(it)->m_addInfo)) /
                             100;
            }
            int rate = expert_job::CAlchemist::GetQuestItemDropRate(
                           this, dropRate) +
                       bonus;
            if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
            {
                CParty* party = (CParty*)GetParty();
                if (party != 0)
                {
                    int count = party->get_member_count();
                    float mult;
                    if (count < 1)
                        mult = 1.0f;
                    else
                        mult = *(float*)((char*)G_CDataManager() + 0x10 +
                                         (count + 0xf7) * 4);
                    rate = (int)((float)rate * mult);
                }
            }
            if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x4f))
            {
                unsigned int p =
                    getAddQuestItemDropRateFromPremium(
                        (ENUM_PREMIUM_TYPE)0x4f);
                rate = dropRate * (int)(p / 100) + rate;
            }

            int gained = 0;
            for (int t = 0; t < tryCount; ++t)
                if (get_rand_int(100) < rate)
                    ++gained;
            if (gained == 0)
                break;

            Inven_Item item;
            CItem* ci = G_CDataManager()->find_item(itemIdx);
            ci->make_item(item);
            item.set_add_info(gained);
            if (gained > 999)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x5579, 5)
                    ("CUser::CheckQuestMonster, mon_item.item.set_add_info , "
                     "ITEM #%d, %d , QUEST#%d",
                     itemIdx, gained, questRef->m_quest[i]);
                // 判据㊿：ORIG 0x0866cfdc `movl $0x3e8,-0x40(%ebp)` 写的是 **gained
                //   局部槽**（-0x40，即上方 set_add_info(gained)/cmpl $0x3e7 的同一槽），
                //   非 item 字段；Ghidra C 亦作 `local_44 = 1000;`。原写
                //   item.m_addInfo2 会污染发包内容（put_int(item.get_add_info())）。
                gained = 1000;
            }

            PacketGuard packet;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
            pkt->put_header(0, 0xe);
            pkt->put_byte(0);
            pkt->put_short(1);

            Inven_Item existItem;
            int slot = getCurCharacInvenR()->GetInvenData(itemIdx, existItem);
            if (slot < 0)
            {
                int newSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)8, true, true);
                if (newSlot > 0)
                {
                    pkt->put_short(newSlot);
                    pkt->put_int(itemIdx);
                    pkt->put_int(item.get_add_info());
                    pkt->put_byte(item.GetItemAttr());
                    pkt->put_short(item.m_fieldb);
                    pkt->put_byte(item.m_field0);
                    pkt->put_int(item.m_fieldd);
                    pkt->put_byte(item.m_amp.getAbilityType());
                    pkt->put_short(item.m_amp.getAbilityValue());
                    if (item.isAvatarItemType())
                    {
                        pkt->put_int(0x1e);
                        // ORIG 0x0866d1??：socketData / expansion 各自重调
                        //   getCurCharacInvenR()+GetAvatarItemMgrR()（无 mgr 缓存，P183）。
                        const char* socketData =
                            getCurCharacInvenR()->GetAvatarItemMgrR()
                                ->getJewelSocketData(item.get_add_info());
                        pkt->put_binary(socketData, 0x1e);
                        const char* expansion = static_cast<const char*>(
                            getCurCharacInvenR()->GetAvatarItemMgrR()
                                ->GetExpansionInfo(item.get_add_info()));
                        pkt->put_int(4);
                        if (expansion == 0)
                        {
                            stAvatarExpansionInfo_t init4;
                            init4.init();
                            pkt->put_binary(
                                reinterpret_cast<const char*>(&init4), 4);
                        }
                        else
                        {
                            pkt->put_binary(expansion, 4);
                        }
                    }
                    pkt->put_byte(0);
                    pkt->put_packet(g_emptySlot);
                    pkt->finalize(true);
                    Send(packet);
                }
            }
            else
            {
                if (needCount == -1)
                {
                    existItem.set_add_info(existItem.get_add_info() +
                                           item.get_add_info());
                }
                else if (needCount <= existItem.get_add_info())
                {
                    for (std::vector<MonsterKillItem>::iterator it =
                             kills.begin();
                         it != kills.end(); ++it)
                    {
                        // 2026-09-11 R5（门③a-2 复核 + 具体源码改法，已落地）：
                        //   disp2 报 ORIG-only `+0x4..+0x1c`（连续 4 步长 ×7）⇒ 核指令
                        //   上下文：ORIG 0x0866d42e..0x0866d469 为 8×`mov K(%eax),%edx;
                        //   mov %edx,-0xe0+K(%ebp)`（把 `*it` **按值**拷到栈临时 -0xe0），
                        //   随后 0x0866d46f/477/481/48a = `memcmp(-0xc0, -0xe0, 0x20)`
                        //   ⇒ ORIG 源码取的是**结构值**而非迭代器指针。我方原形 `&*it`
                        //   直传指针 ⇒ 少 16 条拷贝。改具名值拷贝后：本件 gap +13→−1、
                        //   ndiff 190→185、TU 四数 809/1719/5/66/586 逐项不变（零劣化）。
                        MonsterKillItem killItem = *it;
                        if (memcmp(&drop, &killItem, 0x20) == 0)
                        {
                            kills.erase(it);
                            break;
                        }
                    }
                    again = true;
                }
                else
                {
                    int cur = existItem.get_add_info();
                    if (cur + item.get_add_info() < needCount)
                        existItem.set_add_info(cur + item.get_add_info());
                    else
                        existItem.m_addInfo2 = needCount;   // ORIG 字段直写（无 set_add_info call）
                }
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot,
                                                  existItem);
                pkt->put_short(slot);
                pkt->put_int(itemIdx);
                pkt->put_int(existItem.get_add_info());
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_int(0);
                pkt->put_byte(existItem.m_amp.getAbilityType());
                pkt->put_short(existItem.m_amp.getAbilityValue());
                if (existItem.isAvatarItemType())
                {
                    pkt->put_int(0x1e);
                    // ORIG 0x0866d3??：同 slot<0 分支，两次独立重调（无 mgr 缓存）。
                    const char* socketData =
                        getCurCharacInvenR()->GetAvatarItemMgrR()
                            ->getJewelSocketData(existItem.get_add_info());
                    pkt->put_binary(socketData, 0x1e);
                    const char* expansion = static_cast<const char*>(
                        getCurCharacInvenR()->GetAvatarItemMgrR()
                            ->GetExpansionInfo(existItem.get_add_info()));
                    pkt->put_int(4);
                    if (expansion == 0)
                    {
                        stAvatarExpansionInfo_t init4;
                        init4.init();
                        pkt->put_binary(
                            reinterpret_cast<const char*>(&init4), 4);
                    }
                    else
                    {
                        pkt->put_binary(expansion, 4);
                    }
                }
                pkt->put_byte(0);
                pkt->put_packet(existItem);
                pkt->finalize(true);
                Send(packet);
            }
        }
    }
}
// §6.2 巨件核销：P167 100/100 [0缺0多]；gap 616→634（our+18）；残差归因 N82
// 槽号错位 + esm 访问器重物化族（P112），语义完整仅剩 codegen 差，N 封顶
// （详见 docs/asm_translation_patterns.md 负例表）。
// b28d-solo-CUser p167 复核（ORIG 124 / OUR 122）：已修 4 处 put_binary 重载
// （char*→const char*，2×expansion + 2×init4，P167 经典重载选错）。
// 余 6 缺口待后续：MISSING getCurCharacInvenR(const)×2 / GetAvatarItemMgrR(const)×2 /
//   MonsterKillItem::C1×1 / GetInvenRef(int,int) const×1；
//   EXTRA set_add_info(int)×2 / GetInvenRef 非 const×1 / getItemList()×1。
// （访问器 cv/重载选择差异，非行为分叉证据；形态残差归 N82/P112。）
    // 0x08655c60 主分支（FatigueUp.md）：v==0→1；BreakAway 幸运点先结算；
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08655c60 | _ZN5CUser9FatigueUpEi
// [b28d-GrpD 判词·结构化 N] ORIG 0x08655c60：残差为具名临时槽化（帧 0x64 vs ORIG
//   0x54）引起的整段 mov 重排；setcc（setne/sete）与 je/jne 落点体互换均为等价
//   （判据⑦）；字段偏移（+0x8e064/+0x8e08c/0x9c/0x32/0x64）与事件脚本 id（0x19b 等）
//   逐条对齐 ⇒ 无语义缺口。
#include "CUser/FatigueUp.inc"
{
    // Hades+GoldControl 双扣减；state==5 时混沌点扣减与累计(+0x8e064)；
    // +0x8e08c=v；prev 疲劳逢 50 上报 hack 0x263；0x9c/0x32/0x64 清动作组
    // 0x16/0x28/0x2a；事件脚本 inc/gift/bring_up(0x19b)/process_add_fatigue；
    // 学园点；英雄任务 MissionNo=2；repeat 事件 0x29(角色日)/0x2b(疲劳buff)；
    // AvatarRechargeServer 计耐久；+0x79642 累计；公会 temp exp；上限/燃烧/
    // premium 分支（详见 docs/class_func_reports/CUser/FatigueUp.md）。
    if (v == 0)
        v = 1;

    // ORIG 0x08655c88：getUserBreakAwayRewardLuckPoint 仅调一次，结果缓存到槽
    // 供 IncCurCharacLuckPoint 复用（asm +0x033 mov %eax,-0xc(%ebp) + +0x03f 重载）。
    // 逗号操作符短路形匹配 ORIG `iVar8=getUserBreakAwayRewardLuckPoint(),0<iVar8`。
    int luckPoint;
    if (getCurCharacR() != 0 &&
        (luckPoint = getUserBreakAwayRewardLuckPoint(), luckPoint > 0))
    {
        IncCurCharacLuckPoint(luckPoint);
        setUserBreakAwayRewardLuckPoint(0);
        setUsedBreakAwayLuckPoint(true);
    }

    // XNuclear::CHades::UseFatigue（ORIG T 0x084b9552；权威定义见 CHades.cpp）
    ((XNuclear::CHades*)getHades())->UseFatigue((unsigned int)v);
    GetGoldControl()->UseFatigue(v);

    if (get_state() == 5 && getCurCharacR() != 0)
    {
        IncCurCharacAssaultFatigueCount();
        if (GetCurCharacAssaultFatigueCount() > 1)
        {
            SubCurCharacChaosPoint(1);
            SetCurCharacAssaultFatigueCount(0);
            // [L9-C组J PATCH J4] ORIG 0x08655d3c 段 `mov 0x8e064(%eax),%eax; mov %eax,%edx;
            //   add 0xc(%ebp),%edx; mov %edx,0x8e064(%eax)` = 成员值读改写（disp 形）；
            //   裸指针 cast 在 4.4 产 lea 取址形（多 1 条）⇒ 具型视图成员（POD，无符号发射）。
            struct ChaosFatigueTail
            {
                char m_pad[0x8e064];
                int m_chaosFatigue;   // +0x8e064
            };
            reinterpret_cast<ChaosFatigueTail*>(this)->m_chaosFatigue += v;
        }
    }
    m_fatigueGain = v;                             // +0x8e08c
    unsigned short prevFatigue = (unsigned short)getCurCharacUsedFatigue();
    incCurCharacUsedFatigue((unsigned short)v);
    unsigned short nowFatigue = (unsigned short)getCurCharacUsedFatigue();
    // 2026-09-11 CuA A组（语义轴，ORIG 直证）：ORIG 0x08655dd0-0x08655e02 实序列为
    //   `shr %ax; movzwl; imul $0x147b; shr $0x10; mov %eax,%edx; shr %dx; mov %edx,%eax;
    //    shl $2; add %edx; lea 0x0(,%eax,4),%edx; add %edx; add %eax,%eax; mov %ecx,%edx;
    //    sub %ax,%dx; test %dx,%dx; sete` ⇒ 判定式 = **fat - 50*((fat/25)/2) == 0**，
    //   即 `fat % 50 == 0`（尾 `add %eax,%eax` 把 25x 变 50x）。我方旧形乘数写 -0x2a(42)
    //   （= fat - 42*floor(fat/50)）⇒ fat=50k 时漏报、fat=42 时误报；改回 -0x32。
    // 2026-09-11 R5 补（imm/bits 命中终判）：本段 ORIG `imul $0x147b`（5243 ≈ 2^16·2/25）
    //   vs 我方 `imul $0x47af`（18351 ≈ 2^16·7/25）。5 形探针实测（/tmp/r5/probe/p_div.cpp
    //   t1 `(f>>1&0x7fff)/0x19`｜t2 `(f>>1)/0x19`｜t3 具名 `unsigned short half` + `/0x19`
    //   ｜t4 `int half` / 25｜t5 `u16 q = half/25`）**全部**产 `imul $0x47af`（或 `imul %edx`），
    //   无一产 0x147b ⇒ ORIG 的简化 16 位除法 lowering（4.1）在 4.4 下不可造（N85 同族）
    //   ⇒ 判 N，勿再试写法。
    if ((short)((short)nowFatigue + (short)((nowFatigue >> 1 & 0x7fff) / 0x19) * -0x32) == 0)
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x263, 1, 0, 0);

    if ((short)getCurCharacUsedFatigue() == 0x9c)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x16, 0, (ENUM_PACKETCLASS)0);
    if ((short)getCurCharacUsedFatigue() == 0x32)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x28, 0, (ENUM_PACKETCLASS)0);
    else if ((short)getCurCharacUsedFatigue() == 100)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x2a, 0, (ENUM_PACKETCLASS)0);

    // ---- 事件脚本（EventClassify::CEventScriptMng）----
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->inc_user_fatigue(this, (unsigned int)v);
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->send_gift_fatigue_item(this, (unsigned int)v);
    if (static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()
            ->get_event_script_mng())
            ->is_eventing(0x19b))
    {
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->send_bring_up_item_fatigue(this, v);
    }

    if (isJoinedSchoolPointEvent())
        calcCurCharacSchoolPoint(prevFatigue);

    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, (HeroMissionCondition::MissionNo::T)2, (unsigned int)v);

    // ---- repeat 事件 0x29：角色日（job 匹配）----
    CEventBase* dayEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x29);
    if (dayEvent != 0)
    {
        if (dayEvent->IsEventing((CUser*)0))
        {
            if (get_charac_job() ==
                static_cast<CCharacterDayEvent*>(dayEvent)->GetEventJob())
            {
                _processCharacterDayEvent(prevFatigue, (unsigned int)v);
            }
        }
    }
    // ---- repeat 事件 0x2b：疲劳 buff ----
    CEventBase* buffEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x2b);
    if (buffEvent != 0 && buffEvent->IsEventing((CUser*)0))
    {
        _processFatigueBuffEvent(prevFatigue, (unsigned int)v);
    }

    ARAD::Singleton<AvatarRechargeServer>::Get()->CalcDurability(this, v);
    m_postChecksumData.m_totalFatigueUsed =
        (short)(m_postChecksumData.m_totalFatigueUsed + (short)v);   // +0x79642
    if (get_charac_level() != 0)
    {
        int weight =
            G_CDataManager()->m_guildParameter.getGuildExpWeight(get_charac_level());
        add_guild_temp_exp((weight * v) / 100);
    }
    decCurCharacFatigueGrownUpBuff((unsigned int)v);
    if (static_cast<unsigned char>(m_guildDB.m_fundLevel) + 1 > 0x10)  // +0x8d03b = m_guildDB.m_fundLevel(+0x1b)
        reset_guild_temp_exp();
    processFatigueCharac(v);

    // ---- premium 无限疲劳：直接返回总疲劳 ----
    if (m_premium.isAffectedUnlimitFatigue())
    {
        getCurCharacTotalFatigue();
        return;
    }
    // ---- repeat 事件 1：幸运点 * v ----
    CEventBase* ev1 = GlobalData::s_event_manager->GetRepeatEvent(1);
    // ORIG 0x0865619f：GetRepeatEvent(1) 后直接 IsEventing，无判空。
    if (ev1->IsEventing((CUser*)0))
    {
        IncCurCharacLuckPoint(
            // ORIG 0x086561bc `mov 0xe4(%eax)`（4B int；SPS +0x7c=m_7c）
            G_CDataManager()->m_serverParameter.m_7c * v);
        getCurCharacTotalFatigue();
        return;
    }
    // ---- repeat 事件 10：非活跃时走常规上限/燃烧/premium 结算 ----
    CEventBase* ev10 = GlobalData::s_event_manager->GetRepeatEvent(10);
    // ORIG 0x086561ff：GetRepeatEvent(10) 后直接 IsEventing，无判空。
    if (!ev10->IsEventing(this))
    {
        if ((short)getCurCharacFatigue() == -1)
        {
            getCurCharacFatigue();
            return;
        }
        if (get_charac_memberkey() != 0 && is_connect_upper_member())
            add_member_pay_tex_fatigue_to_upper(v);
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->process_add_fatigue_count(this, v);

        unsigned short refill = 0;
        CEventBase* burn =
            GlobalData::s_event_manager->GetRepeatEvent(0xf);
        // ORIG 0x086562d7：GetRepeatEvent(0xf) 后直接 IsEventing，无判空。
        if (burn->IsEventing((CUser*)0))
        {
            refill = static_cast<CBurningFatigueEvent*>(burn)->getRefillFatigue(
                getCurCharacUsedFatigue(), (unsigned short)v);
        }
        if (IsHavePremiumAdvantage())
        {
            unsigned short prem = getCurCharacPremiumFatigue();
            unsigned int maxPrem = getCurCharacMaxPremiumFatigue() & 0xffff;
            if ((int)((unsigned int)prem + v) <= (int)maxPrem)
            {
                setCurCharacPremiumFatigue((unsigned short)(prem + v));
                if (refill != 0)
                {
                    gainCurCharacFatigue((unsigned int)refill);
                    SendFatigue();
                }
                IncCurCharacLuckPoint(
                    // ORIG 0x086563ae `mov 0xe4(%eax)`（4B int；SPS +0x7c=m_7c）
                    G_CDataManager()->m_serverParameter.m_7c * v);
                getCurCharacTotalFatigue();
                return;
            }
            if (getCurCharacPremiumFatigue() <
                (unsigned short)getCurCharacMaxPremiumFatigue())
            {
                setCurCharacPremiumFatigue(
                    (unsigned short)getCurCharacMaxPremiumFatigue());
                v = ((unsigned int)prem + v) -
                    (getCurCharacMaxPremiumFatigue() & 0xffff);
            }
        }
        // ORIG 0x086565b0（P48 no-CSE）：getCurCharacMaxFatigue 在条件与赋值处
        // 各自重取，不缓存 maxFatigue 局部。
        unsigned int curFatigue = (unsigned int)getCurCharacFatigue() & 0xffff;
        int newFatigue;
        if ((int)(getCurCharacMaxFatigue() & 0xffff) < (int)(curFatigue + v))
            newFatigue = (int)(getCurCharacMaxFatigue() & 0xffff);
        else
            newFatigue = (int)(curFatigue + v);
        setCurCharacFatigue(newFatigue);
        IncCurCharacLuckPoint(
            // ORIG 0x086564a2 `mov 0xe0(%eax)`（4B int；SPS +0x78=m_78）
            (newFatigue - (int)curFatigue) * G_CDataManager()->m_serverParameter.m_78);
        if (refill != 0)
        {
            gainCurCharacFatigue((unsigned int)refill);
            SendFatigue();
        }
        getCurCharacFatigue();
        return;
    }
    getCurCharacTotalFatigue();
}

// 0x0867ba5c（SendNotiPacket.md）：header(0,cmd)；cmd==2 → byte+short(1)+
// make_basic_info；cmd==0x25 → level/exp/组队/会员 exp/SP/SFP/胜点(+0x58)/
// 疲劳/seria bonus/周末(事件 0x57 活跃才发并 reset)/premium bonus(reset)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867ba5c | _ZN5CUser14SendNotiPacketENS_11eSendTargetE15ENUM_NOTIPACKETi
#include "CUser/SendNotiPacket.inc"
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, cmd);
    // 2026-09-11 CuB（P261 if 链→switch + P324 case 块物理序）：ORIG 0x0867ba89
    //   决策树 = `mov 0x10(%ebp),%eax; cmp $0x2,%eax; je <2 体@+0x234>;
    //   cmp $0x25,%eax; jne <log@+0x27d>`（单装载寄存器比较链），且 0x25 体在 +0x42、
    //   2 体在 +0x234（物理序 = 书写序）⇒ 原码是 switch，**case 0x25 写在 case 2 之前**；
    //   if/else 链（旧形）四四产内存直折 cmp + 2 体直落（我方旧读数）。
    //   4.4 形验证（探针 /tmp/CuB/probe/p1.cpp g2 vs g3，c6root 4.4.7 -m32 -O0）：
    //   switch 同序 ⇒ `mov 12(%ebp),%eax; cmpl $2,%eax; je .L8; cmpl $37,%eax; jne .L15`
    //   + 0x25 体在前 / 2 体在后，与 ORIG 逐形一致。
    // [Main-K4C 2026-09-11 · P305 修正] **必须 `switch((int)cmd)`**：`ENUM_NOTIPACKET`
    //   全仓只定义 `ENUM_NOTIPACKET_0 = 0`（CUser.h:354），是窄枚举 ⇒ `-fstrict-enums`
    //   判定值域 {0}，把 `case (ENUM_NOTIPACKET)0x25/2` **静默整块删除**（编译零告警）
    //   —— 实测裸 `switch(cmd)` 使本函数 212→50 条（两个 case 体全丢，真语义损坏）。
    //   改 `switch((int)cmd)` + int 常量 case 即保留寄存器比较链形且不被删（S18 修法）。
    switch ((int)cmd)
    {
    case 0x25:
    {
        ((InterfacePacketBuf*)&packet)->put_byte(get_charac_level());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_exp());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_party_bonus_exp());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_member_bonus_exp());
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)0));
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)1));
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)2));
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)3));
        ((InterfacePacketBuf*)&packet)->put_int(getPVPResultR()->m_winPoint);
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_fatigue_buf_bonus_exp());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_seria_buf_bonus_exp());
        // ORIG 0x0867bc0f..0x0867bc2a：`call GetRepeatEvent` 后直接
        // `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; …; call *%edx;
        // test %al,%al; je else` —— 无具名指针槽、无判空、无 bool 物化
        // （P171 照抄不写 + 发明防御删除；同 add_guild_pvp_result 的 0x21 处置）。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)
                ->IsEventing((CUser*)0))
        {
            ((InterfacePacketBuf*)&packet)->put_int(getWeekendBonusExp());
            resetWeekendBonusExp();
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_int(0);
        }
        ((InterfacePacketBuf*)&packet)->put_int(getPremiumBonusExp());
        resetPremiumBonusExp();
        break;
    }
    case 2:
    {
        // [L9-GrpE 2026-09-11 P0 具名槽恢复] ORIG 0x0867bc90 `mov 0x14(%ebp),%eax;
        //   mov %eax,-0xc(%ebp)` 后 put_byte（0x0867bc96 从 -0xc 读回）、put_short、
        //   make_basic_info（0x0867bcbb `mov -0xc(%ebp),%eax; movsbl %al,%edx`）三处
        //   均从具名槽 -0xc 读 ⇒ 原码在本分支把 param 拷入具名局部（Ghidra C：
        //   `local_10 = param_4;`）。重建直读 `param` 形缺该槽。
        int p = param;
        ((InterfacePacketBuf*)&packet)->put_byte(p);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)p);
        break;
    }
    default:
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x76d1, "User %s - NotifyID = %d", getCurCharacName(), cmd);
        return false;   // b28c-L5 SIGFIX：ORIG 串 bool；失败臂 `mov $0x0,%ebx` 汇合尾
    }
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return true;        // ORIG 0x0867bd4e `call SendPacket; mov $0x1,%ebx; …mov %ebx,%eax`
}

// 0x0865c936
// ORIG：公会战积分累加。PvP 房间内且非观察者、公会战事件(0x21)进行中且
// 有公会时：通知公会服 + 发包(0x6e) + 累加 +0x8cfe0（m_guildWarPoint）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c936 | _ZN5CUser20add_guild_pvp_resultEi
#include "CUser/add_guild_pvp_result.inc"
{
    PvP_Room* pRoom = (PvP_Room*)GetPVPRoom();
    if (pRoom == 0)
        return;
    int seat = pRoom->get_user_seat(this);
    if (pRoom->IsPvpObserver(seat) != 0)
        return;
    // 2026-09-10 gA（A 组）ORIG 0x0865c985..0x0865c9b3：GetRepeatEvent 返回值无判空
    //   （直接 `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; ...; call *%edx`），
    //   且 guard 为早期返回形 `call; xor $1; test %al; jne END`（P16/P168a①）
    //   ⇒ 删具名 pEvent + 发明判空，改为表达式内联 + 早退。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x21)->IsEventing((CUser*)0))
        return;
    // ORIG 0x0865c9b9..0x0865c9cb：`call; test %eax,%eax; sete %al; test %al; jne END`
    if (get_charac_guildkey() == 0)
        return;
    {
        // 2026-09-10 gA（A 组）真 bug（字段错位轴）：ORIG 0x0865c9d7 构造载荷后
        //   0x0865c9e7 `mov %eax,-0x15(%ebp)`（载荷基址 -0x1f ⇒ **+0xa**）写 guildkey、
        //   0x0865c9ed `mov %al,-0x11(%ebp)`（⇒ **+0xe**）写 point，SendPacket(...,0xf)；
        //   与 shared/packet/include/Packet_Notice_Guild_War_Point_Change.h 的 packed
        //   布局 m_guildKey@+0xa / m_point@+0xe 一致 —— 我方旧写 +0/+4 是错位。
        //   ⚠ 不得为补齐载荷空间引入局部 holder 类：那会发射
        //   `_ZZN5CUser20add_guild_pvp_resultEiEN12NoticeHolderC1/C2Ev` 两个 **ORIG 无**
        //   的符号（实测 missing_in_game 588→590，已回退）。改用真实类型 + 尾随 4B
        //   缓冲承载越界 3B 写（P171 照抄 ORIG 写偏移）。Game/ 头为无字段最小镜像
        //   （sizeof 0xc，真头字段化属 .h 类型轴，另报 Main）。
        Packet_Notice_Guild_War_Point_Change notice;
        unsigned char noticeTail[4];
        // [L9-C组J PATCH J1'] ORIG 0x0865c9e7 `mov %eax,-0x15(%ebp)`（基址 −0x1f + 0xa）/
        //   0x0865c9ed `mov %al,-0x11(%ebp)`（+0xe）= 成员 disp 直存；裸指针 cast 在 4.4 产
        //   `lea 0xa(%eax),%ebx; mov %eax,(%ebx)` 双 lea（多 1 条/处）⇒ 视图 cast + 成员访问。
        //   ⚠ 必须保留真实类型实例（v1 换类型的版本实测丢失
        //   `_ZN36Packet_Notice_Guild_War_Point_ChangeC1/C2Ev` 两个 ORIG 存在的 weak 符号
        //   ⇒ ae 1712→1711、missing 588→587，已回滚）。
#pragma pack(push, 1)
        struct NoticeWarPointLayout
        {
            char m_pad[0xa];
            unsigned int m_guildKey;   // +0xa
            unsigned char m_point;     // +0xe
        };
#pragma pack(pop)
        reinterpret_cast<NoticeWarPointLayout*>(&notice)->m_guildKey =
            (unsigned int)get_charac_guildkey();
        reinterpret_cast<NoticeWarPointLayout*>(&notice)->m_point = (unsigned char)v;
        (void)noticeTail;
        // ORIG 0x0865c9fe..0x0865ca1f：代理指针无槽无判空（`mov &s_guild_proxy_mgr,%edx;
        //   mov %eax,0x4(%esp); mov %edx,(%esp); call GetServerProxy; ...; call SendPacket`）
        //   ⇒ 删具名 proxy 与发明判空，改链式内联（P183）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendPacket((char*)&notice, 0xf);
        send_pvp_record();
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x6e);
        ((InterfacePacketBuf*)&packet)->put_byte(v);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
        m_guildWarPoint = m_guildWarPoint + v;
    }
}

// 0x0865d986
// ORIG：PVP 每日/每周游玩次数累计。PvpResultType 布局（CUserCharacInfo.h）：
//   +0x60 当日游玩次数（隔天重置为 0）、+0x64 累计游玩次数、
//   +0x68 当日基准时间戳（0 点 + 0x15180）。字段未具名，按 ORIG 偏移访问（推断）。
#include "CUser/add_pvp_play_info.inc"
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    // PvpResultType：+0x60 m_todayLoseCount / +0x64 m_todayPlayCount /
    // +0x68 m_nextResetTime（ORIG add_pvp_play_info 0x0865d9a7/0x0865da2b/
    // 0x0865da3f 均 movl 4B 实证）。
    if (getPVPResultRefR()->m_nextResetTime + 0x15180 < (int)now)
    {
        getPVPResultRefW()->m_todayLoseCount = 0;
        // ORIG 0x0865d??：&tmv 存指针槽 -0xc（`lea -0x3c,%eax; mov %eax,-0xc`），
        // 三清零与 mktime 均 `mov -0xc,%eax` reload 解引用——4.4 值形会直写折掉
        // 指针槽（技巧 36 反向：ORIG 保留中转），显式指针对象还原。
        tm tmv;
        // ORIG 0x0865da??：指针 = **localtime_r 返回值直用**（call 后 %eax 即 &tmv，
        // `mov %eax,-0xc` 存槽，无二次 lea）。
        tm* pTmv = localtime_r(&now, &tmv);
        (*pTmv).tm_hour = 0;
        (*pTmv).tm_min = 0;
        (*pTmv).tm_sec = 0;
        getPVPResultRefW()->m_nextResetTime = mktime(pTmv) + 0x15180;
    }
    getPVPResultRefW()->m_todayLoseCount += a;
    getPVPResultRefW()->m_todayPlayCount += b;
}

// 0x0865c678
// ORIG：PvP 结果结算。观战者直接返回；winPointBuffer(+0x796fc) 清零并入账；
// 胜/败计数（整合 PvP 基础频道且渠道类型==0 时不计）；额外胜点补发并记 WPAdd 日志；
// 战斗模式 4/5 且 out 非空时按 mob_reward×胜点增量 + gainPowerWarRewardExp 计算奖励。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c678 | _ZN5CUser14add_pvp_resultEbPj
#include "CUser/add_pvp_result.inc"
{
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return;
    lock();
    // b28d-G3C 2026-09-10（P183 去具名指针）：ORIG 0x0865c7?? `mov 0x8(%ebp),%eax;
    //   mov %eax,(%esp); call getPVPResultR; mov 0x58(%eax),%eax; mov %eax,-0x14(%ebp)`
    //   ——**链式直读**（无中间 `const PvpResultType* before` 槽）。我方原具名独占
    //   -0x24 槽（帧 0x44→0x54），残差 +0x10 的主因。
    int beforeWinPoint = getPVPResultR()->m_winPoint;
    int delta = m_winPointBuffer;
    m_winPointBuffer = 0;
    // b28d-G3C 2026-09-10（P59/P222 分支反写）：ORIG 0x0865c74a 段是
    //   `cmpb $0x0,-0x2c(%ebp); je +0xd9` 后紧跟 `addl $0x1,-0x10(%ebp)`（delta+1）
    //   ⇒ **then = flag 真**；我方原写 `if (!flag) {...} else {delta+1...}` 产
    //   `movzbl -0x3c; xor $0x1; test %al,%al; je`（3 条 + 取反）。两分支互换。
    if (flag)
    {
        delta = delta + 1;
        // 2026-09-11 CuA（P59/P222 守卫包裹形 + 去 xor 物化）：ORIG 0x865c70b..
        //   0x865c74f 为 `call IsIntegratedPvPBaseChannel; test %al,%al; je +0x1a` /
        //   `call GetPvPChannelType; test %eax,%eax; jne +0x14` / `mov $0x1,%eax; jmp` /
        //   `mov $0x0,%eax; test %al,%al; jne <join>` —— **无 `xor $0x1`**、then 为空、
        //   body 在 else。原写 `!(A && B==0)` 令 4.4 产 `xor $0x1,%eax` + 极性镜像
        //   （我方 index 44/67 两处）。探针 /tmp/P5/p_guard h.cpp c1：正向空 then +
        //   else body 与 ORIG 逐条同形（含 mov $1/mov $0 物化）。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
            G_CDataManager()->GetPvPChannelType() == 0)
        {
        }
        else
        {
            getPVPResultW()->m_totalWinCount += 1;
        }
    }
    else
    {
        // ORIG：整合 PvP 基础频道且渠道类型 0 → 不计败场。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
            G_CDataManager()->GetPvPChannelType() == 0)
        {
        }
        else
        {
            getPVPResultW()->m_totalLoseCount += 1;
        }
    }
    getPVPResultW()->m_winPoint += delta;
    m_historyLog.WPAdd(getPVPResultR()->m_winPoint, delta, eWPAddReason_0);
    int extra =
        room->get_extra_win_point(getPVPResultW()->m_winPoint - beforeWinPoint);
    if (extra != 0)
    {
        getPVPResultW()->m_winPoint += extra;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, extra, eWPAddReason_0);
    }
    unlock();
    // b28d-G5（P0/P183 删多余具名局部）：ORIG 0x0865c857 段
    //   `mov -0x1c(%ebp),%eax; mov %eax,(%esp); call get_pvp_battle_mode;
    //    cmp $0x4,%eax; je +0x1ff` + 岛 `mov $1/mov $0; test %al,%al; je +0x280`
    //   ——battleMode / rewardMode **均无槽**（内联 || 链）；我方原具名占
    //   -0x14(4B) 与 -0xd(1B) 两槽 ⇒ 帧 0x54 vs ORIG 0x44（+0x10）。
    if ((room->get_pvp_battle_mode() == 4 ||
         room->get_pvp_battle_mode() == 5) &&
        out != 0)
    {
        unsigned int mobReward = 0;
        // ORIG 0x0865c89f `mov %eax,%ebx`（level 无槽，跨 G_CDataManager call 保活）
        register int level = get_charac_level();   // 裸 register（非绑定，P0 合规）
        G_CDataManager()->get_mob_reward(level, mobReward);
        *out = (unsigned int)((getPVPResultR()->m_winPoint - beforeWinPoint) *
                              (int)mobReward);
        *out = *out + (unsigned int)gainPowerWarRewardExp((int)*out);
    }
    send_pvp_record();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
}

// 0x0864fac4
// ORIG：公会技能已学查询。GetGuildDBInfo 内 +0x44 成员数、+0x45 起
// 每 5 字节 {skillIdx(int), level(byte)}，返回匹配项的 level 字节。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fac4 | _ZN5CUser17IsGuildSkillLearnEi
#include "CUser/IsGuildSkillLearn.inc"
{
    // P212：全头部声明时 4.4 与 4.1 同为「先声明深」⇒ 声明序按 ORIG 槽深序
    //   gdb(-0x14)→count(-0x10)→i(-0xc)。
    STGuildDBInfoOnly* gdb;
    int count;
    int i;
    gdb = (STGuildDBInfoOnly*)GetGuildDBInfo();
    // b28d-solo-CUser 试形（P0/P171/P183 三旋钮）：
    //   ① ORIG 0x864fac9 call 后直接 `mov %eax,-0x14; mov -0x14,%eax; movzbl 0x44(%eax)`
    //      ——**无判空**（原码遗留，照抄不写防御）；
    //   ② count 为 int（ORIG `movzbl %al,%eax; mov %eax,-0x10` 零扩展入槽）；
    //   ③ 无 skills 指针缓存（ORIG 每访问重算 `gdb+i*5+0x40+0x5`，P183 去缓存）。
    count = gdb->m_skillLearnCnt;   // ORIG movzbl 0x44 + movzbl %al,%eax
    for (i = 0; i < count; ++i)
    {
        // ORIG 0x0864fb01 `mov 0x5(...)`（idx int）/ 0x0864fb1c `movzbl`
        // （level 零扩展）；ORIG 直返 level 字节（非布尔规范化——见回报，
        // CUser.h 返回类型待属主核对 P9）。
        if (((const STGuildSkill*)gdb->m_skills)[i].m_field0 == skillIdx)
            return (unsigned char)(((const STGuildSkill*)gdb->m_skills)[i]
                                       .m_field4);
    }
    return 0;
}

// 0x0864fb3a
// ORIG：公会技能经验换算。无公会/非公会成员/未学技能 0xc9 → 0；
// 否则按职业 find_skill 返回的 vector<vector<int>>（+0x108）取
// 公会技能经验：ORIG 0x0864fb3a（sz 0x100，报告 gainGuildSkillExp.md）。
// 忠实转写：find_skill(job, 0xc9)；索引 = IsGuildSkillLearn(0xc9)-1（前检查
// 已保证其为 1 → 恒 0）；取权重表第 0 行的 at(gradeIdx)；无 find_skill/边界/
// empty 防御检查（ORIG 直接 operator[]+at，越界走 at 异常路径）。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0864fb3a | _ZN5CUser17gainGuildSkillExpEi
#include "CUser/gainGuildSkillExp.inc"
{
    if (get_charac_guildkey() == 0)
        return 0;
    if (getGuildMemberGrade() == 0)
        return 0;
    // ORIG 0x864fb7d-0x864fc2e 系：learned@-0x11 **dead init 0 + 赋值两段**（技巧 8）
    // → cmpb $0,-0x11; sete %al 物化 guard（技巧 22）；skill@-0x10 dead init；
    // job/gradeIdx 共用 %ebx 跨 call（块作用域分离）；gradeWeights 无具名槽
    // （skill+0x108 直发 op[]，T-CU15）；at() 结果 `mov (%eax),%eax` 落 weight 槽
    // -0xc 再 imul（具名物化补齐槽后 learned/skill 平移归位）。
    char learned = 0;
    learned = (char)IsGuildSkillLearn(0xc9);
    // P0/N96：去绑定。ORIG `cmpb $0,-0x11; sete %al; test; jne` 物化岛为 4.1
    // 槽比零产物（P197④：4.4 对槽比恒出 cmp/jne 直跳，源形不可复现）。
    // P225 修正 P197④ 边界：while(learned==0){return 0;} 复现
    // `cmpb $0; sete %al; test; je BODY` 物化形（probe p7.t5 逐条实证）。
    while (learned == 0)
    {
        return 0;
    }
    // 2026-09-10（FL2CUser 类型轴 S2）：ORIG 0x0864fbdc 段 p167 实形 = **const 重载**
    //   `_ZNKSt6vectorIS_IiSaIiEESaIS1_EEixEj`（`m_field108` 的 operator[]）+ 
    //   `_ZNKSt6vectorIiSaIiEE2atEj`（at）⇒ 原码持 **const CSkill***（find_skill 结果
    //   以 const 视角使用）；旧形非 const 指针致两处均走非 const 重载。
    const CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(get_charac_job(), 0xc9);
    // ORIG 0x0864fbdc-0x86fbfd：gradeIdx（=learned-1）跨 operator[] call 存活于
    // %ebx（callee-saved）；P200/P197② 直传形——拓宽表达式作 at() 实参直传，
    // 4.4 即出 `movsbl -0x11; sub $1; mov %eax,%ebx` 并入 %ebx（禁 register-asm
    // 中间变量：退化 cwtl+mov 两步，见 getBlueMarble/OnDisconnect probe s3）。
    int weight = (skill->m_field108)[0].at((int)learned - 1);
    return (exp * weight) / 100;
}

// ---- 0x08659faa make_charac_stat(PacketGuard&)：角色属性块序列化。
// 报告：docs/class_func_reports/CUser/make_charac_stat.md（汇编 0x8659faa..0x865a44d
// + Ghidra 反编译 C）。三通路，各尾部统一 put_int(0x53)/put_binary(0x52)/put_byte：
//   C) GameWorld::IsCharacterStatRevisionChannel()（W 0x86977da）为真（ORIG
//      落入块）→ AddInfo 副本被 FairPvPStatScript::FindCharacStat（T 0x8693c66，
//      G_CDataManager()+0x4df8 即 m_fairPvPStat）按 (job, firstGrowType) 查得的
//      CharacterStatus（0x54）非零字段覆盖：头 2 int + 4 u16、循环1 u16
//      [0x10..0x16]、循环2 u16[0x18..0x38]、尾段簇（记录尾段比 _Additioal_info
//      整体错后 2 字节：0x3c↔0x3a … 0x50↔0x4e）；
//   B) 否则有 VIP 特权 → AddInfo 副本 += WongWork::CUserPremium::GetAdditionalInfo
//      （W 0x8694a88，sret 临时）经 _Additioal_info::operator+=（W 0x8693cea）；
//   A) 否则 → 直接输出 getCurCharacAddInfoR() 原块（无副本）。
// 副本经拷贝初始化生成 ORIG 同款内联逐块复制（无 memset 构造）。
// b28d-lane5 N封顶: P11 符号扩展; P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x08659faa | _ZN5CUser16make_charac_statER11PacketGuard
#include "CUser/make_charac_stat.inc"
{
    // ORIG 0x08659fb6 `movb $0x1,-0x21(%ebp)`（Ghidra local_25=1，写后不读）：
    //   该具名局部参与栈布局，使 addInfo 副本落在 -0xc5（奇地址）⇒ 结构复制展开为
    //   「对齐 peel + rep movsl + 尾块」与 ORIG 同形（P0 槽-局部双向对应律）。
    bool localFlag = true;
    (void)localFlag;
    if (G_GameWorld()->IsCharacterStatRevisionChannel())
    {
        // 通路 C：FairPvP 表非零字段覆盖副本。
        _Additioal_info addInfo =
            *static_cast<const _Additioal_info*>(getCurCharacAddInfoR());
        // P0：ORIG 无 growType/job 具名局部（movsbl→%esi / %ebx 直传 FindCharacStat），
        // 且实参求值序 = 右→左（先 getCurCharFirstGrowType 后 get_charac_job）。
        CharacterStatus* stat = G_CDataManager()->m_fairPvPStat.FindCharacStat(
            get_charac_job(), (int)getCurCharFirstGrowType());
        if (stat != 0)
        {
            if (stat->m_field00 != 0)
                addInfo.m_field00 = stat->m_field00;
            if (stat->m_field04 != 0)
                addInfo.m_field04 = stat->m_field04;
            if (stat->m_field08[0] != 0)
                addInfo.m_field08 = stat->m_field08[0];
            if (stat->m_field08[1] != 0)
                addInfo.m_field0a = stat->m_field08[1];
            if (stat->m_field08[2] != 0)
                addInfo.m_field0c = stat->m_field08[2];
            if (stat->m_field08[3] != 0)
                addInfo.m_field0e = stat->m_field08[3];
            // P0 单槽：ORIG 两循环共用 -0x1c（声明 int i = 0; + 两次 for init）。
            int i = 0;
            for (i = 0; i <= 3; ++i)   // ORIG 循环1：+0x10..+0x16
            {
                if (stat->m_field08[i + 4] != 0)
                    addInfo.m_field10[i] = stat->m_field08[i + 4];
            }
            for (i = 0; i <= 0x10; ++i)   // ORIG 循环2：+0x18..+0x38
            {
                if (stat->m_field08[i + 8] != 0)
                    addInfo.m_field18[i] = stat->m_field08[i + 8];
            }
            if (stat->m_field3c != 0)
                addInfo.m_field3a = stat->m_field3c;
            if (stat->m_field40 != 0)
                addInfo.m_field3e = stat->m_field40;
            if (stat->m_field42 != 0)
                addInfo.m_field40 = stat->m_field42;
            if (stat->m_field44 != 0)
                addInfo.m_field42 = stat->m_field44;
            if (stat->m_field48 != 0)
                addInfo.m_field46 = stat->m_field48;
            if (stat->m_field4a != 0)
                addInfo.m_field48 = stat->m_field4a;
            if (stat->m_field4c != 0)
                addInfo.m_field4a = stat->m_field4c;
            if (stat->m_field4e != 0)
                addInfo.m_field4c = stat->m_field4e;
            if (stat->m_field50 != 0)
                addInfo.m_field4e = stat->m_field50;
        }
        packet.put_int(0x53);
        packet.put_binary(reinterpret_cast<const char*>(&addInfo), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
    else if (IsHavePremiumAdvantage())
    {
        // 通路 B：AddInfo 副本叠加 VIP 附加属性。
        _Additioal_info addInfo =
            *static_cast<const _Additioal_info*>(getCurCharacAddInfoR());
        addInfo += m_premium.GetAdditionalInfo();
        packet.put_int(0x53);
        packet.put_binary(reinterpret_cast<const char*>(&addInfo), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
    else
    {
        // 通路 A：put_int(0x53) → put_binary(AddInfo 原指针, 0x52)。
        packet.put_int(0x53);
        packet.put_binary(
            static_cast<const char*>(getCurCharacAddInfoR()), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
}

// ---- 0x0869729a incPlayExpAdd：m_gameOption.m_playExpAdd += v（ORIG W 内联）----
#include "CUser/incPlayExpAdd.inc"
{
    // ORIG 0x86972a0-0x86972a9：mov field,%edx; mov 0xc(%ebp),%eax; add %eax,%edx（不折叠 add MEM）。
    // 4.4 字段载入恒经 %eax 中转，单段直落 %edx 不可造（N18a/N96；register 绑定全域禁用）。
    m_gameOption.m_playExpAdd = m_gameOption.m_playExpAdd + v;
}

// 0x0866a9a0 gain_sp：SP 双树（0/1）clamp 0x7fffffff 后累加。
#include "CUser/gain_sp.inc"
{
    // ORIG 0x866a3fe（80 insn 无回跳 = **源码两段手动展开**；remain 无变量：
    // else 支重呼 SkillR+get_remain；cap=INT_MAX-v 每段表达式内联先算（4.1 落
    // %ebx 三行），cmp 结果 setl 物化岛（技巧 22 register bool al 方向）。
    if (0x7fffffff - v <
        ((SkillSlot*)getCurCharacSkillR())
            ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0))
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)0);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0) + v,
                (ENUM_SKILL_TREE_KIND)0);
    }
    if (0x7fffffff - v <
        ((SkillSlot*)getCurCharacSkillR())
            ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1))
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)1);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1) + v,
                (ENUM_SKILL_TREE_KIND)1);
    }
}

// 0x0866aad2 gain_sfp：SFP 双树（2/3），v==0 直接返回，clamp 0x7fffffff。
#include "CUser/gain_sfp.inc"
{
    // ORIG 0x866aad2 守卫是正向 `if (v != 0) {…}`（同已 identical 的 gain_sp 形）：
    // 写 `if (v == 0) return;` 早返形会让 4.4 在末支 else 的 call 后多发一条
    // `jmp END`（早返块的 exit 边强制跳转），正向块形则自然 fall-through。
    // 两段展开：cap=INT_MAX-v 每段先算、else 支重呼 SkillR+get_remain_sfp、无缓存变量。
    if (v != 0)
    {
        if (0x7fffffff - v <
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2))
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)2);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(
                    ((SkillSlot*)getCurCharacSkillR())
                        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2) + v,
                    (ENUM_SKILL_TREE_KIND)2);
        }
        if (0x7fffffff - v <
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3))
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)3);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(
                    ((SkillSlot*)getCurCharacSkillR())
                        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3) + v,
                    (ENUM_SKILL_TREE_KIND)3);
        }
    }
}

// 0x0866ac0e history_log_sp：before==-1 或当前值变化时记 SPAdd(tree 0)。
#include "CUser/history_log_sp.inc"
{
    // ORIG 0x866ac0e 三调用复制形（判据③直分支消费者）：==-1 真支调一次读+SPAdd；
    // 假支 cmp 直落 je；SPAdd 前**再调一次**重取（无 remain 槽）。免缓存三处展开。
    if (before == -1)
    {
        m_historyLog.SPAdd(0,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0),
            delta, reason);
    }
    else if ((((SkillSlot*)getCurCharacSkillR())
                  ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0)
              != before))
    {
        m_historyLog.SPAdd(0,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0),
            delta, reason);
    }
}

// 0x0866acd0 history_log_sfp：同上（tree 2 / SFPAdd）。
#include "CUser/history_log_sfp.inc"
{
    // ORIG 同构三调用形（tree 2 / SFPAdd，0x0866acd0）。
    if (before == -1)
    {
        m_historyLog.SFPAdd(2,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2),
            delta, reason);
    }
    else if ((((SkillSlot*)getCurCharacSkillR())
                  ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2)
              != before))
    {
        m_historyLog.SFPAdd(2,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2),
            delta, reason);
    }
}

// 0x866273c Remove_Mission：转发 charac_expand 槽 8 的任务列表。
#include "CUser/Remove_Mission.inc"
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    list->Remove_Mission((int)kind);
}

// 0x0868e9c0
#include "CUser/isCompetitionMercenary.inc"
{
    // ORIG 0x868e9c0：curCharacR 非空 → FindIndexByCharacNo(m_characNo) →
    // 0xffffffff 早退 → CheckCharacIndex → m_characs[idx].GetCompetitionState()==1
    // ORIG 两次 getCurCharacR call（无具名指针槽，技巧 37）：判空一次、
    // 取 m_characNo 再一次。
    if (getCurCharacR() == 0)
        return false;
    // b28c-L5（P218 应用）：下标局部 unsigned int → **int**。ORIG 末段
    //   `mov -0xc(%ebp),%eax / mov 0x8(%ebp),%edx / add $0x6ef6c,%edx /
    //    mov %eax,0x4(%esp) / mov %edx,(%esp) / call vector::operator[]`
    //   是「实参值先物化、容器基址后算」；4.4 对 size_t 形参的下标恒先算基址并
    //   折 `lea disp(%eax),%edx`（反例 ndiff 4 封顶判词已作废）。下标声明 int
    //   ⇒ 实参走 size_t 转换中转，装载序翻为 ORIG 的值先形（P218 探针 w4 同族）。
    //   与 -1 哨兵比较同时回归 ORIG 的 `cmpl $0xffffffff,-0xc(%ebp); jne` 槽比形。
    int idx = (int)m_mercenaryMgr.FindIndexByCharacNo(
        static_cast<unsigned int>(getCurCharacR()->m_characNo));
    if (idx == 0xffffffff)
        return false;
    if (!m_mercenaryMgr.CheckCharacIndex(static_cast<unsigned char>(idx)))
        return false;
    // 残差（ndiff 4，跨版本 N）：末段 ORIG `mov -0xc(%ebp),%eax; mov 0x8(%ebp),%edx;
    // add $0x6effc,%edx`（4.1 先载实参 RHS + mov+add 折成员基址）vs 4.4
    // `mov 0x8(%ebp),%eax; lea 0x6effc(%eax),%edx`（成员对象地址 lea 单条 + 实参后算）；
    // 同一地址两种物化形，4.1↔4.4 求值顺序+lea/add 等效（probe icm/lea 穷举至 N）。
    return m_mercenaryMgr.m_characs[idx].GetCompetitionState() == 1;
}

// 0x0865cfd8
// ORIG：PvP 积分增减。负值：事件(0x36)进行且 grade∈[10,19] 半额（0.5f）→ 0.9f →
// GetPvPPenaltyRevision 三级衰减；整合 PvP 基础渠道 ×70%；0xf 频道超档按
// GetPvPChannelGradePenalty 衰减；任务内限制升降档；重算 grade 并在变更时
// §6.2 巨件核销：P167 102/102 [0缺0多]；gap 641→645（our+4）；残差归因 N82
// 槽号错位族（x87 ROUND 岛跨版本形态差），语义完整仅剩 codegen 差，N 封顶
// （详见 docs/asm_translation_patterns.md 负例表）。
// 重发任务信息包并重建 0x1e 任务列表。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865cfd8 | _ZN5CUser16update_pvp_pointEi
#include "CUser/update_pvp_point.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（ORIG 641 vs 我方 651）：ORIG 0x0865cfd8 逐块比对
    //   无字段/调用缺口；差集特征为 (a) cMyTrace 临时槽 -0x74(ORIG) vs -0x7c(我方)
    //   ——同帧 0xbc 下的槽分配策略差（N82）；(b) 我方函数体 +0x44 字节（跳转目标
    //   0x17421 vs 0x865d97b，全为等长块平移）；字符串/浮点常量差异系重定位伪差
    //   （`flds 0x8cf396c` vs `flds 0x165c`）⇒ 维持 P167/P183 已收敛源形，判 N。
    if (v == 0)
        return 1;
    if (v < 0)
    {
        if (v < -100000 || v > 100000)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x35d6, 5)
                ("pvp_exp_error5 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
        if (getPVPResultRefR()->m_pvpGrade < 10)
            return 1;
        // ORIG 0x0865d0bb：GetRepeatEvent(0x36) 后直接解引用 IsEventing，无判空。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x36)
                ->IsEventing((CUser*)0))
        {
            // P183：ORIG 两次独立 getPVPResultRefR() 重物化（asm +0x106/+0x119），
            // 禁 int grade 缓存变元。
            if (getPVPResultRefR()->m_pvpGrade >= 10 &&
                getPVPResultRefR()->m_pvpGrade <= 0x13)
                v = (int)(0.5f * (float)v);
        }
        v = (int)(0.9f * (float)v);
        float revision =
            G_CDataManager()->m_serverParameter.GetPvPPenaltyRevision(
                getPVPResultRefR()->m_pvpGrade);
        v = (int)(revision * (float)v);
        if (v < -100000 || v > 100000)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x35ed, 5)
                ("pvp_exp_error6 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
    }
    // 整合 PvP 基础渠道且渠道类型 0 → 打七折。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetPvPChannelType() == 0)
    {
        v = (v * 70) / 100;
    }
    // 0xf 频道正向积分超过当前渠道最高档 → 按档位差衰减。
    if (G_GameWorld()->GetChannelType() == 0xf && v > 0)
    {
        int diff = getPVPResultRefR()->m_pvpGrade -
                   G_CDataManager()->GetMaxGradePvPChannel();
        if (diff > 0)
        {
            int penalty =
                G_CDataManager()->m_serverParameter.GetPvPChannelGradePenalty(
                    diff);
            v = (v * penalty) / 100;
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3607, 5)
            ("pvp_exp_error7 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return 1;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return 1;
    if (!has_within_Mission())
    {
        if (G_GameWorld()->GetChannelType() == 0xf &&
            acceptable_within_mission() && v > 0)
            v = 0;
    }
    else
    {
        PacketGuard packet;
        if (v < 1)
        {
            if (v < 0)
            {
                Remove_Mission((short)0x1b);
                Remove_Mission((short)7);
                ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
                ((InterfacePacketBuf*)&packet)->put_byte(1);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                Send(packet);
            }
        }
        else if (G_GameWorld()->GetChannelType() == 0xf)
        {
            v = 0;
        }
        else
        {
            Remove_Mission((short)0x1b);
            Remove_Mission((short)7);
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3653, 5)
            ("pvp_exp_error8 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    getPVPResultRefW()->m_expPoint += v;
    if (getPVPResultRefR()->m_expPoint < 0)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                              0x365c, "PVP correct now<%d> point<%d>",
                              getPVPResultRefR()->m_expPoint, v);
        getPVPResultRefW()->m_expPoint = 0;
    }
    if (getPVPResultRefR()->m_pvpGrade >= 0x1e)
        return 1;
    if (!has_within_Mission() && G_GameWorld()->GetChannelType() == 0xf)
        acceptable_within_mission();
    int newGrade =
        GlobalData::s_ref_pvp_grade->GetPvpGrade(
            getPVPResultRefR()->m_expPoint);
    if (newGrade < 1)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                              0x36bc, "PVP Grade Incorrect %d",
                              getPVPResultRefR()->m_expPoint);
    }
    else
    {
        // ORIG 0x0865d9..：newGrade<1 时无早退，走 else 块（Ghidra C 结构）。
        if (newGrade < getPVPResultRefR()->m_pvpGrade)
        {
            // 降档保护：grade∈[10,0xf] 时按当前档下限回填积分。
            // P183：每处字段访问独立 getPVPResultRefR() 重物化（ORIG 3 处额外 R 调用），
            // 禁用缓存变元（int grade 缓存即缺 3 call 语义缺口）。
            if (getPVPResultRefR()->m_pvpGrade >= 10 &&
                getPVPResultRefR()->m_pvpGrade <= 0xf)
            {
                getPVPResultRefW()->m_expPoint =
                    GlobalData::s_ref_pvp_grade->GetPvpPoint(
                        getPVPResultRefR()->m_pvpGrade) + 1;
                newGrade = getPVPResultRefR()->m_pvpGrade;
            }
        }
        int oldGrade = getPVPResultRefR()->m_pvpGrade;
        getPVPResultRefW()->m_pvpGrade = newGrade;
        if (has_within_Mission() && getPVPResultRefR()->m_pvpGrade > oldGrade)
            getPVPResultRefW()->m_pvpGrade = oldGrade;   // 任务中禁止升档
        if (getPVPResultRefR()->m_pvpGrade != oldGrade)
        {
            PacketGuard packet;
            getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&packet));
            if (G_GameWorld()->GetChannelType() != 0xf)
            {
                Remove_Mission((short)0x1e);
                CMissionList_Charac* list = (CMissionList_Charac*)GetCharacExpandData(
                    (ENUM_CHARAC_EXPAND_TYPE)8);
                list->MakeMissionList_JustKind(*this, 0x1e);
            }
            Send(packet);
        }
    }
    return 1;
}
// 0x08657450
// ORIG：每日重置——副本金币数据、安全卡/密保取消次数（非 0 时归零并回写 DB）、
// NPC 好感度、ActionPoint（APSystem）落库并结算今日行动/勋章奖励。
#include "CUser/resetDailyData.inc"
{
    getDungeonGainedGold()->reset();
    if (getSecurityCard()->getCancelCnt() != 0)
    {
        getSecurityCard()->resetCancelCnt();
        DB_SecurityCardUpdateCancelCnt::makeRequest(get_acc_id(), 0);
    }
    if (getPad()->getCancelCnt() > 0)
    {
        getPad()->setCancelCnt(0);
        DB_PassPadUpdateCancelCnt::makeRequest(get_acc_id(), 0,
                                               (char*)getWebAddress());
    }
    resetNPCRelationShipDailyData();
    // ORIG 0x08657450：resetNPCRelationShipDailyData 后尾调 sendNPCRelationShipFavor；
    // ActionPoint 经 m_actionPoint.GetActionPoint()（返回 this+0x08 数据体）传递，
    // 不可取 &m_actionPoint（+0x00 偏移不同）。
    sendNPCRelationShipFavor();
    APSystem::DB_UpdateActionPoint::makeRequest(GetUID(),
        *(const APSystem::_SIG_LOAD_ACTION_POINT*)m_actionPoint.GetActionPoint(), true);
    APSystem::CUserProc::SetTodayActionAndCheckMedalReward(this);
}

// 0x08657f10
// ORIG：每日币恢复。getCurCharacR() 为空直接返回；若每日币事件(4)进行中，
// 按角色等级取当日币数并读背包币；premium 有优势时加 premium 币加成；
// 最后按背包币是否足够（coin < need）设置 SetOpencoin(0/1)。
#include "CUser/RecoverCoin.inc"
{
    // ORIG 0x8657f10：getCurCharacR 结果 sete 物化 guard（技巧 22 register bool al）；
    // pEvent@-0xc（先赋后测 `cmpl $0,-0xc; je` 短路岛 @8657f46-4a，技巧 40）、
    // IsEventing 尾 `test %al; je F; mov$1;F:` 物化（@8657f66）；三处 GetCoin 结果
    // 直读 %eax `cmp 0xc(%ebp),%eax` 免槽（技巧 37，ORIG 0x8657faa/0x8657fe2/0x8657ff8）。
    if (getCurCharacR() == 0)
        return;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    // P0/N96 去绑定：短路守卫直判（P224 极性族）——具名 eventOn bool 槽反使
    // pEvent 平移 -0xc→-0x10（ORIG eventOn 为 4.1 al 物化无槽，N96 代价）。
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0))
    {
        v = pEvent->GetCoinNoPerDay(get_charac_level());
        // ORIG 0x8657faa：`call GetCoin; cmp 0xc(%ebp),%eax` 后无消费者 =
        // 4.1 发射**空 if 死比较**（P184 死块照抄；probe p15.d1 实证 4.4
        // `if (call() != v) {}` 保留裸 dead cmp 无 setne 展开）。
        if (getCurCharacInvenR()->GetCoin() != v)
        {
        }
    }
    if (IsHavePremiumAdvantage())
    {
        v += m_premium.GetAdvantageCoin();
        if (getCurCharacInvenR()->GetCoin() != v)  // ORIG 0x8657fe2 同形死比较
        {
        }
    }
    if (getCurCharacInvenR()->GetCoin() < v)
        SetOpencoin((char)0);
    else
        SetOpencoin((char)1);
}

// 0x086568fc
// ORIG：开放标志包(0xb1)。角色存在时：每日币事件(4)进行中取等级 1 的当日币数，
// premium 有优势加币加成；计算缺口（need - coin）并在 opencoin 标志下发包。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x086568fc | _ZN5CUser12SendOpenflagEv
#include "CUser/SendOpenflag.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（131 vs 133）：ORIG 0x086568fc 主体逐条同形
    //   （GetRepeatEvent 具名槽 -0xc + `cmpl $0x0,-0xc; je` 判空、两处局部 0 预置
    //   -0x14/-0x10、getCurCharacInvenR/GetCoin 无具名槽），残差为我方全帧槽号 -4 与
    //   +2 条搬运/对齐 ⇒ 槽分配/布局族，无源码级缺口可下刀，N 封顶。
    if (getCurCharacR() == 0)
        return;
    unsigned int coinNeed = 0;
    int lack = 0;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0) != 0)
        coinNeed = (unsigned int)pEvent->GetCoinNoPerDay(1);
    if (IsHavePremiumAdvantage())
        // ORIG 0x086568fc：GetPremiumInfo() 转发后 GetAdvantageCoin，不直读 m_premium
        coinNeed = coinNeed + (unsigned int)GetPremiumInfo()->GetAdvantageCoin();
    // 2026-09-11 CuB（P0 槽-局部双向对应律）：ORIG 0x086569c4 `call CInventory::GetCoin`
    //   的结果**直接** `cmp -0x14(%ebp),%eax`（无槽往返）⇒ 原码该处为表达式直读，
    //   我方旧形 `unsigned int coin = getCurCharacInvenR()->GetCoin();` 多出
    //   store+load 一对（= 我方多 2 条、全帧 +4 的唯一来源）⇒ 删具名、内联回条件位。
    //   （else 臂 ORIG 0x086569f1 重调 getCurCharacInvenR/GetCoin ⇒ 原码两次直读，保持。）
    if (!(getCurCharacInvenR()->GetCoin() < coinNeed && GetOpencoin() != (char)1))
    {
        lack = 0;
    }
    else
    {
        lack = (int)(coinNeed - getCurCharacInvenR()->GetCoin());
    }
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb1);
    // 2026-09-11 L9GrpF（P9/P11 类型轴）：ORIG 0x08656a3c `call GetOpencoin; movsbl %al,%eax`
    //   = 调用点按 **signed char** 扩宽（我方 `(int)` 只出 movzbl）。GetOpencoin 声明维持
    //   CUserCharacInfo.h:1118 `unsigned char`（非本 TU 写域）⇒ 在调用点加 `(char)`；
    //   值域 0..0xff、put_byte 仅取低字节 ⇒ 行为不变（探针 psi.cpp f0/f1 实证符号扩展形）。
    ((InterfacePacketBuf*)&packet)->put_byte((int)(char)GetOpencoin());
    ((InterfacePacketBuf*)&packet)->put_int(lack);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x086931c4
// ORIG：无效物品删除补偿。逐条记 DeleteInvaildItem 日志并按脚本换算奖励道具；
// 邮件发件人/正文按脚本字符串表（表 4）翻译，空则用 defalt_string；
// 每 10 件一批 ReqDBSendNewSystemMultiMail，余件单独一封。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086931c4 | _ZN5CUser28RewardItem2DeleteInvalidItemERKSsRKSt6vectorISt4pairIiiESaIS4_EE
#include "CUser/RewardItem2DeleteInvalidItem.inc"
    const std::string& name, const std::vector<std::pair<int, int> >& list)
{
    static const char* defalt_string = "";   // ORIG 局部静态 defalt_string(0x93ace50)

    // ORIG 0x086931db `test %al,%al; jne 尾` 与 0x086931f5 `sete/test; jne 尾`
    //   为两条独立早返（非 || 合并）。
    if (list.empty())
        return;
    if (getCurCharacR() == 0)
        return;

    // ORIG 0x086931fb..0x08693210 四条 movl $0 预清（-0x34/-0x30/-0x38/-0x3c），
    //   全部声明在 vector C1(0x08693217) 之前（P198③ 两句式）。
    CItem* itemDef = 0;
    CItem* rewardDef = 0;
    int rewardIdx = 0;
    int rewardCnt = 0;
    std::vector<Inven_Item> items;
    // ORIG 0x08693222..0x0869322d：`call size; mov %eax,-0x2c` 循环外缓存一次
    //   （P13；循环条件 0x08693396 `cmp -0x2c,%eax; setb`）。
    unsigned int listCnt = (unsigned int)list.size();
    for (unsigned int i = 0; i < listCnt; ++i)
    {
        m_historyLog.DeleteInvaildItem(name, list[i].first, list[i].second);
        itemDef = G_CDataManager()->find_item(list[i].first);
        if (itemDef == 0)
            continue;
        // ORIG 0x086932f0 `xor $1,%eax; test %al; jne 循环尾` ⇒ callee bool + if(!...)
        //   （P292/P248档①）；0x08693300 `test %eax,%eax; jle 循环尾` 独立早返。
        if (!G_CDataManager()->m_deleteInvalidItem.GetRewardItemInfo(
                list[i].first, rewardIdx, rewardCnt))
            continue;
        if (rewardCnt <= 0)
            continue;
        rewardDef = G_CDataManager()->find_item(rewardIdx);
        if (rewardDef == 0)
            continue;
        Inven_Item add;
        add.m_addInfo = rewardIdx;   // +0x02 先写奖励物品索引
        rewardDef->make_item(add);   // ORIG vtable 槽 2 填充默认字段
        if (rewardDef->is_stackable())
            add.m_addInfo2 = rewardCnt;   // +0x07 堆叠数量
        items.push_back(add);
    }
    if (items.empty())
        return;

    std::string str0;
    std::string str1;
    // b28d-solo-CUser p167：ORIG 0x0868b9xx 内联 rep stos（我方 EXTRA memset ×2）。
    char sender[0x1e] = {0};
    char body[0x100] = {0};
    G_CDataManager()->m_deleteInvalidItem.GetMailInfo(str0, str1);
    // ORIG 0x08693509..0x08693549：三目结果直接作 strncpy 实参，无 s0/s1 具名局部
    //   （具名会多 `mov %eax,-N` + 回读两条，P0/P329）。
    strncpy(sender,
            str0.empty()
                ? defalt_string
                : g_scriptStringManager_.findString(4, toTString(str0), 0),
            0x1d);
    strncpy(body,
            str1.empty()
                ? defalt_string
                : g_scriptStringManager_.findString(4, toTString(str1), 0),
            0xff);

    unsigned int count = 0;
    Inven_Item batch[10];
    // ORIG 0x0869357c..0x08693587：`call size; mov %eax,-0x24` 同样循环外缓存。
    unsigned int itemCnt = (unsigned int)items.size();
    for (unsigned int i = 0; i < itemCnt; ++i)
    {
        batch[count].setCopy(items[i]);
        count++;
        // ORIG 0x086935c7 `jne 0x485(nop); body; jmp 0x486; nop` = 4.1 if-join 岛
        //   （P223 探针 fC 形：条件假 goto + 体尾 goto 连续双标签）。
        if (count != 10)
            goto batchSkip;
        count = 0;
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, batch, 10u, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false);
        goto batchJoin;
        batchSkip:;
        batchJoin:;
    }
    if (count != 0)
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, batch, count, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false);
    }
}

// 0x08656caa
// ORIG：每日登录物品补给。等级>0x13 时道具 0x1057 补至 3 个（无则插入、有则
// 堆叠补满并记 ItemAdd 日志）；随后 ResetItemByScript/RefillItemByScript/
// deleteSpecificItem 脚本链并回发受影响槽位包；祭坛免费票补满；截止时间前
// 发放道具 0x1cab（背包满走 game_server_msg_24/25 系统邮件）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08656caa | _ZN5CUser12AddDailyItemEv
#include "CUser/AddDailyItem.inc"
{
    // 2026-09-10（G3CUser-B 负结论）：槽集对照 ORIG 48 槽 / 我方 48 槽，但整体平移
    //   +3 字节（ORIG tmp=-0x19a ↔ 我方 -0x19d；ORIG m_addInfo2 槽 -0x193 ↔
    //   我方 -0x196）⇒ 我方在 Inven_Item tmp 之前多出 3 字节对象（1~3 个 char/bool
    //   具名量）。本轮已核 def/tmp/slot/sender/body/packet 六个具名量均与 ORIG 一一
    //   对应（ORIG 各有槽），未定位到第 7 个多余量；30 余条 diff 亦全为跨版本
    //   寄存器/槽号形（含 ORIG 0x08656e34 段 `cmp $0x2; jg` 与 0x08656f60 尾部
    //   `lea -0x8(%ebp),%esp` 序）。判 N 封顶，不再动。
    // b28d-solo-CUser 试形（P0 槽往返）：ORIG 0x08656caa 无 level 槽——call 结果直比
    //   `cmp $0x13,%eax; setg %al; test; je`；我方具名 level 落 -0x20 并被 mem 直比。
    if (get_charac_level() > 0x13)
    {
        CItem* def = G_CDataManager()->find_item(0x1057);
        if (def != 0)
        {
            Inven_Item tmp;
            int slot = getCurCharacInvenR()->GetInvenData(0x1057, tmp);
            if (slot < 0)
            {
                def->make_item(tmp);
                // ORIG 0x08656d4c/0x08656d56：先 m_addInfo2(+7)=3 再 m_addInfo(+2)=0x1057
                tmp.m_addInfo2 = 3;   // 数量
                tmp.m_addInfo = 0x1057;
                slot = getCurCharacInvenW()->insertItemIntoInventory(
                    tmp, eItemAddReason_0xd, true, false);
            }
            else if (tmp.m_addInfo2 < 3)
            {
                m_historyLog.ItemAdd((INVEN_TYPE)1, 3 - tmp.m_addInfo2,
                                     tmp.get_add_info(), tmp,
                                     (eItemAddReason)0xd);
                tmp.m_addInfo2 = 3;
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, tmp);
            }
            if (slot >= 0)
                SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }
    std::vector<std::pair<int, int> > resetList;
    std::vector<std::pair<int, int> > delList;
    ResetItemByScript(resetList);
    RefillItemByScript(resetList);
    deleteSpecificItem(resetList, delList);
    // b28d-solo-CUser p167：ORIG 0x08656caa 用非 const `iterator`（我方 const_iterator
    // 多转换构造 + 混合 operator!=）⇒ 改非 const iterator。
    // ORIG 0x08656d??：循环用**后置** `it++`（call …normal_iterator…ppEi，非前置 ppEv）。
    for (std::vector<std::pair<int, int> >::iterator it = delList.begin();
         it != delList.end(); it++)
    {
        // ORIG 0x08657000..0x08657076：first 两条件 `||` 物化（mov $1/$0 + test，
        //   无槽）后与 second<0 各自落独立 nop 岛（0x3cb/0x3ce）⇒ P223 双守卫形。
        if (it->first < 0 || it->first > 0x1b)
            goto skipFirst;
        if (it->second < 0)
            goto skipSecond;
        SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)it->first,
                           it->second);
        goto skipJoin;
        skipFirst:;
        skipSecond:;
        skipJoin:;
    }

    m_advanceAltar.refillTicketFree();   // ORIG 0x0865f6dc：每日门票免费补满
    // ORIG 0x086570e1：`cmp $0x4811f1ef,%eax; setg %al; test; je 续` ⇒ 早返形
    //   （写 `< 0x4811f1f0` 会产 setl，形态不符）。
    if (GlobalData::s_systemTime_.getCurSec() > (time_t)0x4811f1ef)
        return;
    CItem* def = G_CDataManager()->find_item(0x1cab);
    if (def == 0)               // ORIG 0x0865710f `cmpl $0,-0x18; jne 续` ⇒ 早返
        return;
    Inven_Item tmp;
    def->make_item(tmp);
    // ORIG 0x08657149/0x08657153：先 m_addInfo2(+7)=1 再 m_addInfo(+2)=0x1cab
    tmp.m_addInfo2 = 1;
    tmp.m_addInfo = 0x1cab;
    int slot = -1;              // ORIG 0x0865715d `movl $0xffffffff,-0x14` 预清（P198①）
    slot = getCurCharacInvenW()->insertItemIntoInventory(
        tmp, eItemAddReason_0xd, true, true);
    if (slot == -1)
    {
        char sender[0x15];
        strncpy(sender,
                g_scriptStringManager_.findString(
                    4, "game_server_msg_24", 0),
                0x14);
        char body[0x100];
        strncpy(body,
                g_scriptStringManager_.findString(
                    4, "game_server_msg_25", 0),
                0xff);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, tmp, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false, false);
    }
    else
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        // ORIG 0x08657392 `xor $1,%eax; test %al,%al; je succ` ⇒ 原码为
        //   `if (!MakeItemPacket(...)) { 空 } else { finalize+Send }`（P292 负形）。
        if (!getCurCharacInvenR()->MakeItemPacket(
                (INVEN_TYPE)1, slot, packet))
        {
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
}

// 0x0868aaea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868aaea | _ZN5CUser24SendOneADayItemShopIndexERKSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/SendOneADayItemShopIndex.inc"
    const std::vector<std::pair<int, int> >& list)
{
    // b28d-GrpD 按 ORIG 0x0868aaea 直证改写：
    //  ① ORIG +0x18 call GetRepeatEvent 之后**无判空**，+0x1d 直接 `mov (%eax),%edx`
    //     解引用虚表（判据①：删我方发明的 `event == 0` 防御）。
    //  ② ORIG +0x24 `mov %eax,(%esp)` 复用 GetRepeatEvent 的 %eax 作 this 调虚槽
    //     +0x34 ⇒ 源码是链式调用，**无 event 具名槽**；PacketGuard 落在 -0x1c，
    //     count -0x10，i -0xc，且每次 put_* 均 `lea -0x1c(%ebp),%eax` 重算
    //     ⇒ 删我方 -0x14 的 pkt 具名引用（P183/P329）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0))
        return;

    PacketGuard packet;                                          // ORIG -0x1c
    ((InterfacePacketBuf&)packet).put_header(0, 0x155);
    int count = (int)list.size();
    ((InterfacePacketBuf&)packet).put_byte(count);
    for (int i = 0; i < count; ++i)
    {
        ((InterfacePacketBuf&)packet).put_int(list[i].first);
        ((InterfacePacketBuf&)packet).put_int(list[i].second);
    }
    ((InterfacePacketBuf&)packet).finalize(true);
    Send(packet);
}

// 0x0867c2d8
// ORIG：单槽位物品更新包(0xe)。GetInvenRef 空 → return 0；否则写槽头 +
// 物品字段（addInfo2 为 0 时首 int 写 -1）、强化选项、avatar 宝石槽 0x1e 字节
// 与扩展信息 4 字节、生物装备剩余天数（type==5），追加三转编年史与整件包体。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c2d8 | _ZN5CUser14SendUpdateItemENS_11eSendTargetE14ENUM_ITEMSPACEi
// [b28d-GrpD 判词·N] ORIG 0x0867c2d8：残差为 item 指针具名槽化（我方 -0xS 槽 vs
//   ORIG `GetInvenRef` 的 %eax 直传 `je`）+ 未解析重定位 call 标注；控制流、
//   put_* 序列、字段偏移（+2/+7/+b/+d/+11/0x1e/4/0x5）全一致 ⇒ 无语义缺口。
#include "CUser/SendUpdateItem.inc"
{
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(invenType, slot);
    if (item == 0)
        return 0;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    int uid = item->m_addInfo2;   // +0x07
    if (uid == 0)
        ((InterfacePacketBuf*)&packet)->put_int(-1);
    else
        ((InterfacePacketBuf*)&packet)->put_int(item->m_addInfo);   // +0x02
    ((InterfacePacketBuf*)&packet)->put_int(uid);
    ((InterfacePacketBuf*)&packet)->put_byte((int)item->GetItemAttr());
    ((InterfacePacketBuf*)&packet)->put_short((int)item->m_fieldb);
    ((InterfacePacketBuf*)&packet)->put_byte((int)(unsigned char)item->m_field0);
    ((InterfacePacketBuf*)&packet)->put_int(item->m_fieldd);
    ((InterfacePacketBuf*)&packet)->put_byte(
        (int)item->m_amp.getAbilityType());
    ((InterfacePacketBuf*)&packet)->put_short(
        (int)item->m_amp.getAbilityValue());
    if (item->isAvatarItemType())
    {
        ((InterfacePacketBuf*)&packet)->put_int(0x1e);
        // P183：ORIG 对 get_add_info 各次访问独立重物化 getCurCharacInvenR()
        // ->GetAvatarItemMgrR()（socket 一次、expansion 再一次），禁 mgr 缓存。
        const char* socketData =
            getCurCharacInvenR()->GetAvatarItemMgrR()
                ->getJewelSocketData(item->get_add_info());
        ((InterfacePacketBuf*)&packet)->put_binary(socketData, 0x1e);
        const char* expansion =
            static_cast<const char*>(
                getCurCharacInvenR()->GetAvatarItemMgrR()
                    ->GetExpansionInfo(item->get_add_info()));
        ((InterfacePacketBuf*)&packet)->put_int(4);
        if (expansion == 0)
        {
            stAvatarExpansionInfo_t init4;
            init4.init();
            // ORIG 3 处 put_binary 均走 const 重载（asm 0815096a）。
            ((InterfacePacketBuf*)&packet)->put_binary(
                reinterpret_cast<const char*>(&init4), 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_binary(expansion, 4);
        }
    }
    if (item->m_field1 == (unsigned char)5)
    {
        int tick = 0;
        tick = OS_API::GetDateTimeTick();
        int remainDate = getCurCharacInvenR()->GetCreatureMgrR()->GetRemainDate(
            item->get_add_info(), tick);
        ((InterfacePacketBuf*)&packet)->put_int(remainDate);
    }
    G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(this, *item,
                                                            &packet);
    ((InterfacePacketBuf*)&packet)->put_packet(*item);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return 1;
}

// 0x0868de0a
// ORIG：光环 avatar 选项变更 -> DB_UpdateAuraAvatarOption::makeRequest(uid, characNo, idx, value)
#include "CUser/UpdateAuraAvatarOption.inc"
{
    if (a >= 0 && a <= 2)
    {
        DB_UpdateAuraAvatarOption::makeRequest(GetUID(), getCurCharacNo(), a, b);
    }
}

// 0x0868dff8
// ORIG：光环 avatar 装备检查。无角色 → 1；槽 9 无物品/不可装备 → 0x11；
// 物品为光环 avatar 选项 → 回写选项值并返回 0，否则 0x17。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0868dff8 | _ZN5CUser20is_equip_aura_avatarEcRi
#include "CUser/is_equip_aura_avatar.inc"
{
    if (getCurCharacR() == 0)
        return 1;
    // b28d-solo-CUser p167 修正：ORIG 0x0868dff8 调 getCurCharacInvenRefW（非 InvenW）。
    Inven_Item* item = getCurCharacInvenRefW()->GetInvenRef(0, 9);
    if (item == 0)
        return 0x11;
    if (item->isEmpty() != 0)
        return 0x11;
    if (item->isEquipableItemType() != (char)1)
        return 0x11;
    // ORIG 0x868e088-0x868e09c：itemIdx 载 %eax 后 `mov %eax,%ebx` 跨 G_CDataManager
    // call 缓存（push %ebx/帧 0x34），find_item 结果落 -0xc（equip 槽），item 槽 -0x10。
    // P0/N96 去绑定：字段作实参直传（P200 拓宽并入 callee-saved，跨 receiver
    // call 保活），残差记 N18a。
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item->m_addInfo);
    if (equip == 0)
        return 0x11;
    if (equip->is_aura_avatar_option(slot) != (char)1)
        return 0x17;
    out = equip->get_aura_avatar_option_value();
    return 0;
}

// 0x0867e092
// ORIG：每日道具过期删除（idx 区间 [3,8] 与 [0x39,0x68]）后聚合发送：29 个 0xe 包
// 按物品空间预置包头并记录计数字段位置；cargo/账号金库直接 MakeItemPacket，
// 背包空间空槽写固定"已删"载荷；最后按非零计数回填数量并逐包下发。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e092 | _ZN5CUser19processDelDailyItemEv
// [b28d-GrpD 判词·结构化 N] ORIG 0x0867e092：残差为**局部对象整体下移 8B**
//   （我方 -0x2cc/-0xfc/-0x170 vs ORIG -0x2d4/-0x104/-0x178，同差值）+
//   26 元素循环的 `cmpl $0x1c` 边界一致；switch(sete) vs test/jne 为等价物化
//   ⇒ 无语义缺口（N82 槽基差族）。
#include "CUser/processDelDailyItem.inc"
{
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）

    std::vector<std::pair<int, int> > list;
    deleteDailyStackableItem(list, 3, 8);
    deleteDailyStackableItem(list, 0x39, 0x68);
    // ORIG 0x0867e0ff：调 vector::size() 后 test（非 empty()）—— callee 选型按 ORIG。
    if (list.size() == 0)
        return;

    PacketGuard pkts[29];
    // ORIG 0x0867e180/+0x196：两个 int[29] 各发一次 rep stos 零初始化，无 memset call；
    // 且 ORIG 栈上 indices(-0x108) 高于 counts(-0x17c) ⇒ 源码声明序 indices 在前。
    int indices[29] = {0,};
    int counts[29] = {0,};
    for (int i = 0; i < 0x1d; ++i)
    {
        ((InterfacePacketBuf*)&pkts[i])->put_header(0, 0xe);
        ((InterfacePacketBuf*)&pkts[i])->put_byte(i);
        indices[i] = ((InterfacePacketBuf*)&pkts[i])->get_index();
        ((InterfacePacketBuf*)&pkts[i])->put_short(0);
    }
    // ORIG 0x0867e268..0x0867e2bd：begin()/end() 各在循环外构造一次 const_iterator
    // 局部（local_90/local_94）；循环内只调 operator!=(const_iterator, const_iterator)。
    std::vector<std::pair<int, int> >::const_iterator it = list.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = list.end();
    for (; it != itEnd; ++it)
    {
        const int space = it->first;
        const int slot = it->second;
        if (space == 2)
        {
            getCurCharacCargoR()->MakeItemPacket(&pkts[space], slot);
            counts[space]++;
        }
        else if (space == 0xc)
        {
            if (IsExistAccountCargo())
            {
                GetAccountCargo()->MakeItemPacket(&pkts[space], slot);
                counts[space]++;
            }
        }
        else
        {
            int invenType = 1;
            if (space == 3)
                invenType = 0;
            else if (space == 7)
                invenType = 3;
            else if (space == 1)
                invenType = 2;
            Inven_Item slotItem =
                getCurCharacInvenR()->GetInvenSlot(invenType, slot);
            if (slotItem.m_addInfo == 0)   // ORIG：test dword [ret+2]
            {
                // ORIG 0x0867e3f4 起：每处调用独立重算 &pkts[space] 地址，不落具名指针。
                ((InterfacePacketBuf*)&pkts[space])->put_short(slot);
                ((InterfacePacketBuf*)&pkts[space])->put_int(-1);
                ((InterfacePacketBuf*)&pkts[space])->put_int(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_short(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_int(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_short(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_packet(g_emptySlot);
            }
            counts[space]++;
        }
    }
    for (int i = 0; i < 0x1d; ++i)
    {
        if (counts[i] != 0)
        {
            ((InterfacePacketBuf*)&pkts[i])->put_short(indices[i], counts[i]);
            ((InterfacePacketBuf*)&pkts[i])->finalize(true);
            Send(pkts[i]);
        }
    }
}

// 0x086802b8（reqHumanCertify4ClearMap.md 反编译全文转写）：
// flag 为真时按 mcap 点数概率（get_rand_int(10000) < point）触发；点数 ≥100.0
// 时按 (100-point/100) 与 0 取大降 stdDropRate（ServiceRestrict(3,1) 受限时）
// 并 DB_SaveUserHandicap 上报（DB 域待接线，暂略）；随后 reqHumanCertify()
// 通过则取消人证并 InsertTimer(0,uid,9,0x3c,key++,−1)。
// b28d-lane5 N封顶: N85 x87比较lowering; N82/P112 槽号分配 | ORIG 0x086802b8 | _ZN5CUser24reqHumanCertify4ClearMapEb
// b28d-PoolCUser P230③ -march=i486 实验读数（2026-09-09，同 _processSpeical… 条）：
//   (a) 现路由 ndiff **97**（140/149）；(b) +i486+mtune=generic ndiff **92**（140/150
//   x87 段改善但未归零，残差主体在 N82 槽阶梯）；(c) 仅 i486 整 TU 崩不可用。
// [IG1 2026-09-10 语义轴落地·删发明防御] ORIG 0x08680395..0x086803ad 调
//   ServiceRestrictManager::isRestricted 前**无** srm!=0 判空（`call Singleton::Get;
//   mov %eax,(%esp); call isRestricted` 直调）⇒ 我方 `srm != 0 &&` 系发明防御，已删；
//   同时去掉具名 srm 局部（ORIG 无该槽，P0/P183）。改后 our_insns 142（ORIG 141）、
//   gap -1（原 -10）、ndiff 94→95（difflib 噪声）、TU 四数不变 806/1684/4/101。
//   余量：N85 x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp）+ N82 槽阶。
#include "CUser/reqHumanCertify4ClearMap.inc"
{
    // ORIG 0x086802c6：`movzbl -0x1c(%ebp),%eax; xor $0x1,%eax; test %al,%al;
    //   jne <mov $1 岛>` ⇒ 非具名 bool 局部，而是短路链首项取反；且短路求值序为
    //   getPoint() 先（%ebx ← 0x086802e1）、get_rand_int() 后（0x086802ef cmp）。
    //   正向包裹复现 fall-through 主体（P214①/P226）。
    if (!flag || m_mcap.getPoint() > (unsigned int)get_rand_int(0x2710))
    {
        // b28d-solo-CUser p167：ORIG 0x08680312 调 getPoint 3 次（条件
        //   `(double)getPoint() >= 100.0` 与体内 `point = getPoint()` 分开）⇒ 条件直调。
        if ((double)m_mcap.getPoint() >= 100.0)
        {
            // ORIG 0x08680350..0x08680364：`mul $0x51eb851f`(无符号 /100) → `mov $0x64;
            // sub` → 结果直入 max 首实参槽（-0x14），**无额外 (int) 中转槽** ⇒ cast 外移。
            // b28d-CuE 2026-09-11（P0 槽-局部律直证）：ORIG 0x08680337 `mov %eax,-0x2c(%ebp)`
            //   后直接 `mul $0x51eb851f`，**无 -0x10 槽**（point 的具名局部），且
            //   0x0868033x 只有一次 getPoint 调用 ⇒ 原码 point 内联在除法表达式里；
            //   我方旧具名 `unsigned int point` 多占 -0x10/-0x18 并引入 store-load 往返
            //   （我方 -0x10 存 point / -0x18 存 0 / 读回 -0x2c）⇒ 删除具名局部。
            // b28d-CuE 2026-09-11（实参序直证）：ORIG 0x08680367 `movl $0x0,-0x10(%ebp)`
            //   后 0x0868036e `lea -0x14; mov 0x4(%esp)` / 0x08680375 `lea -0x10;
            //   mov (%esp)` ⇒ arg1=&(-0x10)=**0 的临时**、arg2=&(-0x14)=值
            //   ⇒ 原码实参序为 max(**0**, 100 - point/100)（Ghidra `max<int>(local_18+1,
            //   local_18)` 同证）。max 对称 ⇒ 语义等价。
            int rate = std::max(0, (int)(100 - m_mcap.getPoint() / 100));
            if (rate < getStdDropRate()
                && ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                       RestrictCategory::Enum_3, 1))
            {
                    setStdDropRate(rate);
                    // ORIG 0x086803d8..0x08680400：getStdDropRate 结果留 %ebx 直跨
                    // get_acc_id 调用后入实参槽（**无具名 nowRate 槽**）⇒ P183 内联。
                    // 4.4 右→左实参求值顺序使 getStdDropRate 先算，与 ORIG 一致。
                    DB_SaveUserHandicap::makeRequest(get_acc_id(), 0x9c41, 0x65,
                                                     getStdDropRate(), 1, 0);
            }
        }

        if (reqHumanCertify())
        {
            setHumanCertified(false);
            // [L9-GrpE 2026-09-11 语义修复·符号性轴⑤] ORIG 0x0868044e `cmp $0x7ffffffe,%eax`
            //   + 0x08680453 `jbe 08680462` 直证该比较按 **unsigned** 进行（Ghidra C 亦为
            //   `if (0x7ffffffe < *(uint *)(this + 0x8e0f8))`）。重建成员为 int 时 4.4 折成
            //   `cmp $0x7fffffff; jne`（仅 x==0x7fffffff 清零），对 x∈[0x80000000,0xffffffff]
            //   （高位/负位模式）行为与 ORIG 相反 ⇒ 显式 unsigned 比较。
            //   同口径探针 /tmp/L9/E/p_rc.cpp：fA（现形）产 `cmp [..],0x7fffffff; jne`；
            //   fB/fC（本形）产 `mov %eax,..; cmp $0x7ffffffe,%eax; jbe` 与 ORIG 逐条同形。
            if (0x7ffffffe < (unsigned int)m_humanCertifyTimerKey)
                m_humanCertifyTimerKey = 0;
            m_humanCertifyTimerKey += 1;
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                        (TIMER_MESSAGE)9, 0x3c,
                                        m_humanCertifyTimerKey, (unsigned int)-1);
        }
    }
}

// ---- CUser::master_new_skills @ 0x0866b53e ----
// 批量习得技能：按 _Mastered_skill 数组计算 SP/SFP 用量并扣减对应技能树余量，
// 将技能槽写入（set_skill_slot_at_index），同时记录 SP/SFP 消耗日志。
// PvP 树频道直接返回 true；普通树按 secondGrow/firstGrow 走成长型
// get_give_skill 展开（ORIG 866b844-866bcd5 逐段转写）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866b53e | _ZN5CUser17master_new_skillsEP15_Mastered_skill20ENUM_SKILL_TREE_KIND
#include "CUser/master_new_skills.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（ORIG 506 vs 我方 491）：ORIG 0x0866b53e 与我方
    //   的差**全部**来自跨版本寄存器/槽分配（ORIG `push %edi` + 帧 0xac + 差值临时经
    //   %esi；我方无 %edi + 帧 0x90 + 差值临时经 %ebx），逐块比对无字段/调用缺口
    //   （calcUsedSPE/get_remain_sp_at_index/set_remain_sp_at_index 调用序一致）
    //   ⇒ N78/N82 封顶，不为单条 mov 破坏表达式。
    int usedSp = 0;
    int usedSfp = 0;
    // [P-L11d 2026-09-11 Main-K4C] 下面 secondGrow/firstGrow **两个互斥分支**各自声明过
    //   一批同名标量/指针局部（chars/got/i/skillId/level/group/slotNo）⇒ 提升为函数级
    //   共用，促 4.4 复用同一批槽（ORIG 4.1 复用：ORIG 506 vs 我方 495/491 的槽族差）。
    //   安全性：两支互斥且每个使用点前必赋值（skill/slotNo 的 0 初始化仍在循环内保留）。
    //   不提升 `std::vector`/`std::pair`/`CSkill* skill`：前二者非平凡（P331 禁提前声明
    //   非 POD，会提前发射 ctor/dtor 改帧序）；skill 在循环内重置，保持原形。
    CCharacList* chars = 0;
    bool got = false;
    int i = 0;
    int skillId = 0;
    int level = 0;
    int group = 0;
    int slotNo = 0;
    ((SkillSlot*)getCurCharacSkillR())->calcUsedSP(skills, usedSp, usedSfp);

    // ORIG 0x0866b586..0x0866b6a9：SP/SFP 段逐处重调 getter（无 remainSp/sfpKind 缓存，
    //   P183）；SFP 按 kind 分成**两个完整分支**（SKILL_TREE_NONE/SKILL_TREE_1 → 树 2，
    //   否则树 3），各自「判余量 → set(0) / 重调减 used → set(diff)」+ 扣减后重调 SFPSub。
    if (((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind) < usedSp)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(0, kind);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
            ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind) - usedSp,
            kind);
    }

    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_1) <
            usedSfp)
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                0, SKILL_TREE_SFP_1);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sfp_at_index(SKILL_TREE_SFP_1) - usedSfp,
                SKILL_TREE_SFP_1);
        }
        // ORIG 0x866b6eb：SFPSub(扣减后余量, 树号, 消耗量, reason)
        m_historyLog.SFPSub(
            ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_1),
            (int)SKILL_TREE_SFP_1, usedSfp, (eSPSubReason)1);
    }
    else
    {
        if (((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_2) <
            usedSfp)
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                0, SKILL_TREE_SFP_2);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sfp_at_index(SKILL_TREE_SFP_2) - usedSfp,
                SKILL_TREE_SFP_2);
        }
        m_historyLog.SFPSub(
            ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_2),
            (int)SKILL_TREE_SFP_2, usedSfp, (eSPSubReason)1);
    }
    m_historyLog.SPSub(
        ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind),
        (int)kind, usedSp, (eSPSubReason)1);

    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot_at_index((char*)skills, 0x198, kind);

    if (G_GameWorld()->IsPvPSkilTreeChannel())
        return true;

    // ORIG 866b852：secondGrow 非零 → get_give_skill(flag=0) 展开。
    if (getCurCharSecondGrowType() != 0)
    {
        std::vector<std::pair<int, int> > giveVec;
        // ORIG 0x0866b53e：giveVec 后构造独立 pair 局部（C1），循环内逐字段拷入
        std::pair<int, int> pr;
        chars = G_CDataManager()->m_characList;
        // 配对（D06 定义侧已 bool 化）：ORIG 消费形 `call; mov %al,-N` 1 字节直存 ⇒
        //   消费端局部同 bool（净账必配对律）。
        got = chars->m_characs[get_charac_job()].get_give_skill(
            getCurCharFirstGrowType(), getCurCharSecondGrowType(), giveVec, 0);
        if (got == 0)
            return false;
        for (i = 0; i < (int)giveVec.size(); ++i)
        {
            pr = giveVec[i];
            skillId = pr.first;
            level = pr.second;
            CSkill* skill = 0;
            skill = G_CDataManager()->find_skill(get_charac_job(), skillId);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp",
                    __PRETTY_FUNCTION__,
                    0x52bd, "G_CDataManager()->find_skill( %d, %d )",
                    get_charac_job(), skillId);
                return false;
            }
            group = skill->get_group();
            slotNo = 0;
            slotNo = ((SkillSlot*)getCurCharacSkillR())->get_skillslot_no(skillId, group, kind, true);
            if (slotNo < 0)
            {
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillId, level, kind);
            }
        }
        return true;
    }

    // ORIG 866ba98：firstGrow 非零 → get_give_skill(flag=1) 展开。
    if (getCurCharFirstGrowType() != 0)
    {
        std::vector<std::pair<int, int> > giveVec;
        // ORIG 0x0866b53e：同第二块，giveVec 后 pair 局部（C1）+ 循环内拷入
        std::pair<int, int> pr;
        chars = G_CDataManager()->m_characList;
        got = chars->m_characs[get_charac_job()].get_give_skill(
            getCurCharFirstGrowType(), getCurCharSecondGrowType(), giveVec, 1);
        if (got == 0)
            return false;
        for (i = 0; i < (int)giveVec.size(); ++i)
        {
            pr = giveVec[i];
            skillId = pr.first;
            level = pr.second;
            CSkill* skill = 0;
            skill = G_CDataManager()->find_skill(get_charac_job(), skillId);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp",
                    __PRETTY_FUNCTION__,
                    0x52e0, "G_CDataManager()->find_skill( %d, %d )",
                    get_charac_job(), skillId);
                return false;
            }
            group = skill->get_group();
            slotNo = 0;
            slotNo = ((SkillSlot*)getCurCharacSkillR())->get_skillslot_no(skillId, group, kind, true);
            if (slotNo < 0)
            {
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillId, level, kind);
            }
        }
        return true;
    }
    return true;
}

// 0x0867cbe4（从 GameStubs_remaining.cpp 迁移，语义对齐 CUser/ProcPremiumFatigue.md）
#include "CUser/ProcPremiumFatigue.inc"
{
    if (getCurCharacR() == 0)
        return;
    // ORIG 0x0867cbe4：正向 `if (IsHavePremiumAdvantage())` 包体（Ghidra
    // `if (cVar2 != '\0')`，产 `test %al;je <ret>`；不写早退 `==0` 形以免 xor）。
    if (IsHavePremiumAdvantage())
    {
        // P48 双分支各自重取 GetAdvantageFatigueRate（不缓存）；pcRoom 命中才加
        // dm+0x51c 周末加成。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0) &&
            m_premium.GetAdvantageFatigueRate() != 0 && GetPCRoomNo() != 0)
        {
            setCurCharacMaxPremiumFatigue(
                static_cast<unsigned short>(
                    (short)((static_cast<int>(static_cast<unsigned short>(
                                m_premium.GetAdvantageFatigueRate())) *
                            DEFAULT_MAX_FATIGUE) / 100) +
                    G_CDataManager()->m_serverParameter
                        .m_weekendBouns4b4.m_bonusFatiguePercent));
        }
        else
        {
            setCurCharacMaxPremiumFatigue(
                static_cast<unsigned short>(
                    (static_cast<int>(static_cast<unsigned short>(
                         m_premium.GetAdvantageFatigueRate())) *
                     DEFAULT_MAX_FATIGUE) / 100));
        }
    }
}
// ORIG T 0x0868866e（报告 docs/class_func_reports/CUser/isSecurityServiceUsed.md）：
// 安全键盘激活 或 安全卡激活 或 secuGrade(+0x8e97f) 非零 → 已启用安全服务。
#include "CUser/isSecurityServiceUsed.inc"
{
    // ORIG 0x8688689/0x86886a0：|| 短路链双 `test; jne mov$1` 共享尾岛 + 末项
    // movzbl flag 的 `je mov$0 / fall mov$1` + bool 结果二次物化岛。
    if (getPad()->isActivate() || getSecurityCard()->isActivate() ||
        m_hades.m_secuGrade)
        return true;
    return false;
}

// ORIG W 0x0863bea2：return m_premium(+0x7121c).GetTotalPremiumCount() != 0
// （scout 报告误关联 0x867cd20 = _UpdateUserInfo4Guild，已按真实符号核实）。

// ORIG T 0x0864fc3e CheckLimitMiniumAge：唯一 owner 为 CUserOnlyOrig.cpp
// （W7_BuildHarmonize 去重：该版含 outUnique=unique_id 低16位写出，更忠实）。
#include "CUser/IsPremiumUser.inc"
{
    return m_premium.GetTotalPremiumCount() != 0;
}

// ============================================================================
// 租借物品序列化链（CUser::SaveInventory 直接依赖；m_rentalInfo@+0x6ef78 为
// map<uint, map<uint, vector<InstanceRentalSystem::RentalInfo>>>）。
// ============================================================================

// ORIG T 0x08676dba：`out = m_rentalInfo[param]`（外层 map 键 param，值
// map<uint, vector<RentalInfo>>）；仅当键存在时拷贝。
#include "CUser/GetInstanceRentalInfo.inc"
    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> >& out,
    int param)
{
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::iterator it =
        m_rentalInfo.find((unsigned int)param);
    if (it == m_rentalInfo.end())
        return;
    out = it->second;
    return;
}

// ORIG T 0x086773ea：返回 m_rentalInfo[param][charac_no] 是否存在；存在则拷贝到 out。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086773ea | _ZN5CUser26getCurrentCharacRentalInfoERSt6vectorIN20InstanceRentalSystem10RentalInfoESaIS2_EEi
#include "CUser/getCurrentCharacRentalInfo.inc"
    std::vector<InstanceRentalSystem::RentalInfo>& out, int param)
{
    // ORIG 0x086773fd..0x08677415：cur 落 -0xc 判空后**直读 cur->m_characNo**
    //   （`mov (%eax),%eax`），非 getCurCharacNo 调用（我们旧形多一次 call，p167 EXTRA）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0)
        return false;
    unsigned int characNo = (unsigned int)cur->m_characNo;
    if (characNo == 0)
        return false;

    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > inner;
    GetInstanceRentalInfo(inner, param);
    // b28d-solo-CUser p167：ORIG 0x086773ea 用 const_iterator（find 与 end()
    //   各一次转换构造 + const operator==/operator*）⇒ 改 const_iterator + `(*it).second`。
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator it =
        inner.find(characNo);
    if (it == inner.end())
        return false;
    out = (*it).second;
    return true;
}

// ORIG T 0x08677516：`m_rentalInfo[param][charac_no] = in`（vector 拷贝）。
#include "CUser/setCurrentCharacRentalInfo.inc"
    std::vector<InstanceRentalSystem::RentalInfo>& in, int param)
{
    // ORIG 0x8677516：cur 落 -0xc 槽判空（mov %eax,-0xc; cmpl $0,-0xc; je）；
    // characNo = 直读 cur->m_characNo（`mov (%eax),%eax` @0x867752x，非
    // getCurCharacNo 调用——我们旧形多一次 call 且判空被折 test;setne）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0)
        return;
    unsigned int characNo = (unsigned int)cur->m_characNo;
    if (characNo == 0)
        return;
    m_rentalInfo[(unsigned int)param][characNo] = in;
}

// ORIG T 0x08677584：遍历当前角色租借列表，对仍存在的租借物品在序列化缓冲
// （SIG_SAVE_INVENTORY）中清除对应背包槽；并回写租借条目的 inEquip 标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677584 | _ZN5CUser32removeRentalItemInfoFromInvenBufEP18SIG_SAVE_INVENTORYi
#include "CUser/removeRentalItemInfoFromInvenBuf.inc"
{
    if (sig == 0)
        return 0;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;

    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        // 2026-09-10（G3CUser-B，P183/P0 删发明具名量）：
        //   ORIG 0x08677607 `mov 0x4(%eax),%eax; mov %eax,%ebx`（rental.m_itemIndex
        //   **无槽**、%ebx 跨调用保活）与 0x0867760c `call getCurCharacInvenR; mov %eax,
        //   (%esp); call CheckExistItem...`（inven **无槽**、直传 this）⇒ 我方
        //   `int itemIndex` / `const CInventory* inven` 两个具名量 ORIG 均无；
        //   else 分支 0x08677638 亦从 rental 重算 m_itemIndex、0x0867767d 重调
        //   getCurCharacInvenR ⇒ 全部表达式内联。
        // [L9GrpF PATCH#5 负结论 · 已回滚] 极性改造（`== 1` → `!fn()` + then/else 互换）
        //   实测**反劣**：fn_diff 差异点 54 → 100、我方指令数 140 → 141（TU 四数不动）。
        //   ORIG 0x0867760c 段确为 `xor $0x1,%eax; test %al,%al; je <OK块>`，但本件残差主体是
        //   -0x28 槽块序（同族 removeRentalItemInfoFromInvenLoadBuf 能 AE 是因为其块序不同）
        //   ⇒ 恢复原 `== 1` 极性。若后续重试，须先解决槽块序。
        if (getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip) == 1)
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip == false)
            {
                sig->m_invenType1[slot].reset();   // 背包（类型1）槽
            }
            else
            {
                slot -= 10;
                sig->m_invenType0[slot].reset();   // 装备（类型0）槽
            }
        }
        else
        {
            LogManager::logFormat(
                1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x6d15,
                "Rent error. CUser::removeRentalItemInfoFromInvenBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), rental.m_itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
    }

    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
}

// 0x0864fe52（ORIG CUser::SaveInventory()）
// ORIG 反编译控制流（docs/class_func_reports/CUser/SaveInventory.md）：
//   1) isLocked4DataLoad() 非零 → cMyTrace 报错 "Can not save character until
//      loading done(m_id: %s, charac_no: %d" 并返回 0；
//   2) G_GameWorld()->GetChannelType()==7 → 返回 0；
//   3) 否则 BigStreamPool::Acquire + CStreamGuard 序列化 0xad99 字节的
//      SIG_SAVE_INVENTORY：header(0x23)+UID → memset 整块 0 → _GetSaveInvenBase 填
//      header 区 → 写 m_saveFlag=0 → GetInvenStart(0/1/3) 三块背包 + inven[22..25]
//      宠物尾 → RemoveKCItem（state==10 时再 RemoveFPItem）→ cargo 容量+数据 →
//      m_characExpand.update(this,9) → removeRentalItemInfoFromInvenBuf(1/2) →
//      MsgQueueMgr::put(DB_Q,guard) → 返回 1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fe52 | _ZN5CUser13SaveInventoryEv
#include "CUser/SaveInventory.inc"
{
    if (isLocked4DataLoad())
    {
        // ORIG 0x0864fe69-0x0864fed1：getCurCharacNo→%esi、NumberToString(get_acc_id(),0)→%ebx
        // 均在 cMyTrace ctor **之前**右→左求值并跨 ctor 保活（无 accIdStr 具名槽、无 -0xc 往返）
        // ⇒ P227 全表达式临时链（历史具名 trace+accIdStr 致帧 0x40 与槽漂移）。
        cMyTrace(__PRETTY_FUNCTION__, 0x181a, 5)(
            "Can not save character until loading done(m_id: %s, charac_no: %d",
            NumberToString(get_acc_id(), 0), getCurCharacNo());
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        return 0;
    }

    // ORIG 0x0864ff11-0x0864ff2b：Acquire 结果 %eax 直传 C1（无 stream 具名槽，
    // 槽号 -0x24 而非 -0x28）⇒ P0 内联实参，禁 Stream* 缓存。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1828), true);
    // ORIG：每条 << 前 call operator*()，GetInBuffer 前 operator->()（P167 缺 3）。
    guard.operator*()->operator<<((int)0x23);          // SIG_SAVE_INVENTORY
    guard.operator*()->operator<<((int)GetUID());

    SIG_SAVE_INVENTORY* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();
    memset(sig, 0, 0xad99);
    _GetSaveInvenBase(sig);
    sig->m_saveFlag = 0;

    // P183：ORIG 每块背包独立 getCurCharacInvenR() 重物化（4 处），禁 inven 缓存。
    memcpy(sig->m_invenType0, getCurCharacInvenR()->GetInvenStart(0), 0x53e);
    memcpy(sig->m_invenType1, getCurCharacInvenR()->GetInvenStart(1), 0x4a58);
    memcpy(sig->m_invenType3, getCurCharacInvenR()->GetInvenStart(3), 0x39aa);
    // CInventory +0x55a = &m_invenItem[22]（ORIG 原始偏移；宠物尾 4 项）
    memcpy(sig->m_creatureTail,
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);

    CInventory::RemoveKCItem(sig->m_invenType0, sig->m_invenType1);
    if (get_state() == 10)
    {
        CInventory::RemoveFPItem(sig->m_invenType0, sig->m_invenType1);
    }

    // P183：ORIG 两次独立 getCurCharacCargoR()（GetCapacity 一次、get_cargo_start
    // 再一次），禁 CCargo* 缓存变元。
    sig->m_cargoCapacity = getCurCharacCargoR()->GetCapacity();
    memcpy(sig->m_cargo, getCurCharacCargoR()->get_cargo_start(),
           ARAD::DATABASE::sourceCargoSize);

    m_characExpand.update(this, (ENUM_CHARAC_EXPAND_TYPE)9);
    removeRentalItemInfoFromInvenBuf(sig, 1);
    removeRentalItemInfoFromInvenBuf(sig, 2);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ============================================================================
// _postCheckForceChangeGrowType（免费函数，TSV 修复，ORIG 0x08668823 T）。
// ORIG：遍历 QuestList hash_map<int,Quest*>（G_CDataManager()+0x18→getQuestList），
// 逐任务按 CUserCharacInfo::get_charac_job 做 checkPossibleJob，再按 quest 内
// m_fieldB8==1（比对 firstGrow == quest.m_fieldC0）/==2（checkMatchedGrowType +
// secondGrow == quest.m_fieldC0）判定，命中的任务经 _postCheckForceChangeGrowType4AheadQuest
// → CQuestClear::setClearedQuest 清任务并发包。
// 依赖 Quest/QuestList/CQuestClear 等当前未建模类型，提供可链接的签名匹配骨架（推断）。
// ============================================================================
#include "CUser/_postCheckForceChangeGrowType.inc"
{
    // b28d-solo-CUser p167：ORIG 0x08668823 经 `getQuestList()` 一次取得 QuestList
    //   （MISSING ×1），容器按 const 引用访问（`begin() const`/`end() const`；我方非
    //   const 致 2 次转换构造）；命中块由两分支 **goto 共享**（我方重复两份 ⇒
    //   EXTRA 4AheadQuest/getCurCharacQuestW/setClearedQuest 各 ×1，P276）。
    // [H2LaneE2 2026-09-11 · P0/P183 去缓存] ORIG 0x08668830 无 questList 具名局部：
    //   `call G_CDataManager; mov 0x18(%eax),%eax; mov %eax,(%esp); call getQuestList`
    //   （m_questList 结果作 this 直传，无栈槽往返；我方多 `mov %eax,-0x18;
    //   mov -0x18,%eax` 一对待删）⇒ 内联链式表达式，仅保留 map 引用（ORIG -0x10 单槽）。
    const __gnu_cxx::hash_map<int, Quest*>& qmap =
        G_CDataManager()->m_questList->getQuestList()->m_questMap;
    __gnu_cxx::hash_map<int, Quest*>::const_iterator it = qmap.begin();
    // [Main-K4C 2026-09-11 · P13] ORIG 0x8668853 在循环**外**求值一次 end()（槽 -0x2c）：
    //   尾部 `lea -0x2c,%eax; mov %eax,0x4(%esp); lea -0x24,%eax; mov %eax,(%esp);
    //   call _ZNK...neERKS...`；我方原写 `it != qmap.end()` 致每轮重发
    //   `lea -0x18; mov -0x10,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call end; sub $4`。
    //   A/B 实测（fn_diff_tu，check_tu_routed fresh，指令数两侧均 135/134 norm）：
    //   差异行 ORIG 侧 111 → **49**（保留；残余为 N82/P212 块级对象槽向：ORIG PacketGuard
    //   浅 -0x1c / it -0x24 / end -0x2c，我方 it -0x18 / qend -0x20 / PacketGuard 深 -0x2c）。
    __gnu_cxx::hash_map<int, Quest*>::const_iterator qend = qmap.end();
    for (; it != qend; ++it)
    {
        Quest* quest = it->second;
        // ORIG 0x8668893：checkPossibleJob 取反后为真 → 跳过
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;

        if (quest->m_fieldB8 == 1)
        {
            // +0xb8==1：+0xc0 == firstGrow 命中
            if (quest->m_fieldC0 == firstGrow)
                goto HIT;
        }
        else if (quest->m_fieldB8 == 2)
        {
            // +0xb8==2：checkMatchedGrowType(job, firstGrowType, -1) 且 +0xc0 == secondGrow
            // [H2LaneE2 2026-09-11 · P0/P329 删具名局部] ORIG 0x086688e1 `movsbl %al,%ebx`
            //   = getCurCharFirstGrowType() 返回值**直接拓宽进 callee-saved**（无 -0x9 字节槽
            //   store/load；我方旧形多 `mov %al,-0x9; movsbl -0x9,%ebx` 一对）⇒ 表达式内联进
            //   实参位（实参右→左求值：该调用先于 get_charac_job 求值，与 ORIG 顺序一致）。
            if (!quest->checkMatchedGrowType((ENUM_CHARACTERJOB)user->get_charac_job(),
                                             user->getCurCharFirstGrowType(), -1))
                continue;
            if (quest->m_fieldC0 != secondGrow)
                continue;
            goto HIT;
        }
        continue;
HIT:
        _postCheckForceChangeGrowType4AheadQuest(user, quest);
        // ORIG 0x8668939：getCurCharacQuestW()+0x4 → CQuestClear::setClearedQuest(idx)
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest((unsigned int)quest->m_index);
    }

    // ORIG 0x8668984：PacketGuard → getCurCharacQuestR()->get_quest_info(pg) → Send
    PacketGuard packet;
    user->getCurCharacQuestR()->get_quest_info(
        reinterpret_cast<char*>(&packet));                       // ORIG mangled 形参 Pc
    user->Send(packet);
}
// STGuildDBInfoOnly ctor（ORIG W 0x08693f56 sz 0x16f；无报告，objdump 转写：
// 逐字段清零 → 循环构造 16×STGuildSkill（+0x45 步长 5）→ 尾部标量清零 →
// memset 子成员 → +0x1c (x|1)&~2 → memset skills 覆盖 → memset masterName）。
// 注意 ORIG 在循环构造 STGuildSkill（-1/0xff）后又 memset(+0x45,0,0x50) 覆盖清零，
// 净效果 skills 全 0（CUser.h 冻结 m_skills 为 char[0x50]，无构造语义，直接清零）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08693f56 | _ZN17STGuildDBInfoOnlyC1Ev
// 2026-09-11 CuA 探针终判（/tmp/P5/p_bits b.cpp g1..g6）：`b |= 1`、`b = b|1`、
//   具名读-改-写、`b &= ~2` 等 6 形在 4.4 下**全部**产 5 条
//   `mov %eax,%edx; movzbl 0x1c(%eax),%eax; or $1,%eax; mov 0x8(%ebp),%eax; mov %al,0x1c(%edx)`，
//   而 ORIG 0x08694069 是 4 条的 `movzbl 0x1c(%eax),%edx; or $1,%edx; mov %dl,0x1c(%eax)`
//   （this 保 %eax、值走 %edx，且后续重载 this）⇒ 4.4 不产该分配，判 N（勿再改写法）。
// b28d-GrpD 按 ORIG 0x08693f56 直证改写（两处）：
//  ① ORIG 生成序 = [0x17..0x44 逐字段 movl/movb/movw] → [16× call STGuildSkill::C1Ev
//     循环 @+0x62] → [0x95..0xb9 逐字段 @+0x7f] → [函数体 memset…]。循环夹在标量
//     赋值**中间**，只可能是**成员初始化列表**（gcc 按声明序：m_skillLearnCnt@0x44
//     → m_skills@0x45 隐式构造 → m_field95@0x95）；我方旧「体内赋值」写法会把
//     m_skills 隐式构造顶到函数最前（实测我方 -0x20 帧、循环在首条）。
//     逐条指令核对：+0x0b movl 0x17 / +0x11 movb 0x1b / +0x18 movl 0x1e / +0x1f movw 0x22
//     / +0x26 movl 0x24 / +0x2d movb 0x28 / +0x34 movl 0x29 / +0x3b movb 0x2d /
//     +0x42 movw 0x42 / +0x49 movb 0x44 —— 与下列列表顺序一一对应。
#include "CUser/STGuildDBInfoOnly.inc"
    : m_masterNo(0)                 // +0x17
    , m_fundLevel(0)                // +0x1b
    , m_field1E(0)                  // +0x1e
    , m_memberCount(0)              // +0x22
    , m_guildPoint(0)               // +0x24
    , m_guildRank(0)                // +0x28
    , m_fundExp(0)                  // +0x29
    , m_subMasterCnt(0)             // +0x2d
    , m_guildSkillPoint(0)          // +0x42
    , m_skillLearnCnt(0)            // +0x44
    , m_field95(0)                  // +0x95
    , m_powerSecedeTime(0)          // +0x96
    , m_powerWarPoint(0)            // +0x9a
    , m_agitExist(0)                // +0x9e
    , m_powerJoinCount(0)           // +0x9f
    , m_fundMoney(0)                // +0xa0
    , m_fieldB9(0)                  // +0xb9
{
    memset(m_subMasterData, 0, 0x14);   // +0x2e
    // ② ORIG +0x47/+0x53：+0x1c 为 byte 位操作（`mov 0x8(%ebp),%eax;
    //    movzbl 0x1c(%eax),%edx; or $0x1,%edx; mov %dl,0x1c(%eax)` / `…and $0xfffffffd…`），
    //    只动低字节、不触碰 +0x1d。
    //    [b28d-G5] 旧形 `*(unsigned char*)&m_ability`（指针 cast 后 deref）在 4.4 下
    //    **地址不折进位移**：产 `add $0x1c,%eax` + `add $0x1c,%edx` 双地址与
    //    `(%eax)`/`(%edx)` 间接写读（7 条/站 vs ORIG 4 条/站）⇒ 正是 gap +6 的唯一来源。
    //    解 = 具名视图（P311/P246 组件·union 具型视图）：把 `this` 视作 +0x1c 处为
    //    byte 别名的结构，成员访问即折进位移 `0x1c(%eax)`（无地址临时、无槽）。
    struct GuildAbilityByteView
    {
        char m_pad[0x1c];
        union
        {
            unsigned short m_ability;   // 与 STGuildDBInfoOnly::m_ability 同址（+0x1c）
            unsigned char m_field1C;    // 低字节（ORIG 仅动此字节）
        };
    };
    // 2026-09-11（Lane2，P311 附：复用 memset 返回值）：ORIG 0x08694061 `call memset`
    //   之后 0x08694069 直接 `movzbl 0x1c(%eax),%edx` —— 复用 memset 返回值
    //   （= m_guildName = this）寻址，**无** `mov 0x8(%ebp),%edx` 重载；我方分两句写
    //   会多发 1 条重载。把 memset 内联进同一表达式即复现（探针
    //   /tmp/lane2/probe_memsetret.cpp：f_a ⇒ `movzbl 0x1c(%edx),%edx`，
    //   f_b ⇒ `movzbl 0x1c(%eax),%eax` = ORIG 形）。
    // [CUserLane 2026-09-11 T3 实测无效] P211 档③ 同型化（RHS `(unsigned char)1`）产物
    //   与 `|= 1` 逐条相同（88/90、ndiff 2 不变）⇒ 装载序锁在 memset 返回值复用形上，
    //   维持 N 封顶（第 7 形失败）。
    ((GuildAbilityByteView*)memset(m_guildName, 0, 0x17))->m_field1C |= 1;
    ((GuildAbilityByteView*)this)->m_field1C &= ~2;
    memset(m_skills, 0, 0x50);          // +0x45
    memset(m_masterName, 0, 0x15);      // +0xa4
}

// ===================== U1 批次（FixUndefU1）=====================

// ORIG T 0x08679754 sz0x2e2：仅 level%10==0 写公会公告板；文案 = 角色名 +
// ServerString(0x3e9) + "%d"(level) + ServerString(0x3e8)；len>=0x78 丢弃。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679754 | _ZN5CUser23makeGuildLevelUpMessageEi
#include "CUser/makeGuildLevelUpMessage.inc"
{
    // 2026-09-10 gA（A 组）负结论 N（218 vs 219）：ORIG 0x08679754 全段逐条同形
    //   （含 -0x19f numBuf 的 4.1 慢路清零、string 链、SendGuildBoardWrite 实参序），
    //   唯一残差为我方多一条尾部布局 nop（ORIG 0x08679a2b `lea -0xc(%ebp),%esp`
    //   前无 nop）⇒ 对齐产物，源码不可控。
    if (level % 10 != 0)
        return;

    // ORIG 0x0867978b：board[120] 在 name 构造**之前** rep stos 清零（0x1e dword）；
    //   0x08679841：numBuf[256] 在两次 GetServerString **之后**按 0xff 字节慢路清零。
    char board[120] = {0};

    // ORIG 0x086797f5/+0x0867981b：先取 suf=ServerString(0x3e8) 再取
    //   pre=ServerString(0x3e9)；ORIG 0x165 this=-0x24(suf)/arg=numBuf、0x17a
    //   this=-0x24(suf)/arg=-0x28(pre)、0x18c this=-0x20(body)/arg=-0x24(suf)
    //   ⇒ `suf += numBuf; suf += pre; body += suf;`。
    std::string body(getCurCharacName());
    std::string suf = g_ServerString_.GetServerString(0x3e8, 0);
    std::string pre = g_ServerString_.GetServerString(0x3e9, 0);
    char numBuf[255] = {0};   // ORIG 0x08679847 rep stos 0xff 字节（-0x19f 起）
    sprintf(numBuf, "%d", level);
    suf += numBuf;
    suf += pre;
    body += suf;
    if (body.length() >= 0x78)
        return;

    memcpy(board, body.c_str(), body.length());

    // ORIG 0x0867993f..0x086799a3：无 proxy 判空，单条链式表达式（实参右→左）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                              -1, -1, (unsigned int)get_charac_guildkey());
}

// ORIG T 0x0867cd20 sz0x18d：guildkey!=0 时组 header 0x63 包广播队列 2。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x0867cd20 | _ZN5CUser21_UpdateUserInfo4GuildEv
#include "CUser/_UpdateUserInfo4Guild.inc"
{
    if (get_charac_guildkey() == 0)
        return;

    // b28d-lane5：ORIG 0x0867cd4x 无 Stream* 槽——Acquire 结果经 %eax 直传
    // CStreamGuard ctor 实参（P0 右值内联；具名 s 反产 -0xc 槽 + 回读）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x793f), true);
    // ORIG：每处写经 guard.operator*()->operator<<(…) 显式代理链（operator* 调用点
    // 补齐；SetCharacInfo/SaveInventory 判例）。
    guard.operator*()->operator<<((int)0x63);                       // header
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((int)get_charac_guildkey());
    guard.operator*()->operator<<((unsigned int)get_acc_id());
    guard.operator*()->operator<<((int)getCurCharacNo());
    // b28d-lane5：ORIG 0x0867ce2x `movsbl %al,%ebx` ⇒ 调用点按 signed char 扩展
    // （P11；getter 本体 uchar 直返不变，仅本消费点收窄）。
    guard.operator*()->operator<<((int)(signed char)getCurCharacGrowType());
    guard.operator*()->operator<<((int)get_charac_level());
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG T 0x0864c85a sz0x30b：把当前角色内存快照填入 CACHE_CHARACTER_TYPE 后交
// exchange_server 缓存。控制流/子对象偏移/错误路径逐指令核对 CacheCharacterMemory.md：
//   getCurCharacR()==0 -> false；get_state()<3 -> false；characNo=*curR(首字段)；
//   mem=exchange_server::AcquireCharacterMemory(characNo)（失败 trace + false）；
//   依次 GetRentInfo(+0x703)/GetCharacInfo(+0x0)/GetInventoryInfo(+0x703)/
//   quest(+0x15b4f)/ticket(+0x1d157)/dungeonClear(+0x1d2f3)/blacklist(+0x1eb03)/
//   actionPoint(+0x1eb13)/expand vtable 槽+8(+0x34884)/altar(+0x1f804)/etc(+0x1fd20)；
//   三个子对象内字段拷贝 + memcpy(+0x3f850, this+0x8d0fd, 0xc)；
//   CacheCharacterMemory 失败 -> FreeCharacterMemory + trace + false；成功置 lastLogin + true。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0864c85a | _ZN5CUser20CacheCharacterMemoryEv
#include "CUser/CacheCharacterMemory.inc"
{
    if (getCurCharacR() == 0)
        return 0;
    if (get_state() < 3)
        return 0;

    // ORIG 0x0864c8..：characNo = *((uint*)getCurCharacR()) 直读首字段 m_characNo
    //（P167：再次 getCurCharacR，勿经 getCurCharacNo() 访问器）。
    unsigned int characNo = (unsigned int)getCurCharacR()->m_characNo;
    int characIdx = get_charac_idx(characNo);
    if (characIdx < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x113c, 0)
            ("get_charac_idx() return = %d", characIdx);
        return 0;
    }

    CACHE_CHARACTER_TYPE* mem = exchange_server::AcquireCharacterMemory((int)characNo);
    if (mem == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1150, 0)
            ("AcquireCharacterMemory() return = NULL, charac_no(%d)", (int)characNo);
        return 0;
    }
    GetRentInfo(mem->m_inventory);
    GetCharacInfo(&mem->m_charac);
    GetInventoryInfo(&mem->m_inventory);
    getCurCharacQuestR()->get_quest(&mem->m_quest);
    GetTicket(reinterpret_cast<SIG_LOAD_TICKET*>(mem->m_ticket));
    GetDungeonClear(reinterpret_cast<SIG_LOAD_DUNGEON_CLEAR*>(mem->m_dungeonClear));
    GetBlackListCount(reinterpret_cast<SIG_REQ_BLACK_COUNT*>(mem->m_blackList));
    APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory(
        *this, mem->m_actionPoint);
    // 真实虚调用 CExpandEquipslot::getData(char*) @ vtable+8（CExpandEquipslot.h
    // 权威 slot2）。§4.5 禁 typedef 槽指针直调。
    // 2026-09-11 CuA A组 P0/P183：ORIG 0x0864c9xx 该处**无具名指针槽**
    //   （`call GetCharacExpandDataR; mov (%eax),%edx; add $0x8,%edx; mov (%edx),%ecx;
    //   … mov %eax,(%esp); call *%ecx` 结果直接留 %eax）⇒ 我方旧具名 `expand`
    //   占 -0xc 槽，令 characNo/characIdx/mem/三处 trace 槽整体深移 4
    //   （-0x18/-0x14/-0x10/-0x48/-0x38 vs ORIG -0x14/-0x10/-0xc/-0x44/-0x34）
    //   ⇒ 链式内联回实参位。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xc)->getData((char*)&mem->m_expand);
    advancealtar::DBAndExchangeServerProc::getFromMemory(
        *this, mem->m_altar);
    GetEtcInfo(&mem->m_etc);

    // 三个子对象内字段直拷（ORIG 报告偏移）
    // m+0x1fd2c=etc.m_fieldC / m+0x686=charac.m_tagCharacNo / m+0x70b=inv.m_field8
    //（均 movl 4B，CacheCharacterMemory.md 0x08670d3x 区）。
    mem->m_etc.m_fieldC = mem->m_charac.m_tagCharacNo;
    mem->m_inventory.m_field8 = mem->m_etc.m_fieldC;
    memcpy(mem->m_memberFlag, m_pad8d0fd, 0xc);

    if (exchange_server::CacheCharacterMemory((int)characNo, mem))
    {
        SetLastLoginChannelNo(G_CEnvironment()->get_channel_no());
        SetLastLoginCharacNo((int)characNo);
        return 1;
    }
    exchange_server::FreeCharacterMemory(mem);
    cMyTrace(__PRETTY_FUNCTION__, 0x1190, 0)
        ("CacheCharacterMemory() return = false");
    return 0;
}

// ============================================================================
// 角色内存快照填充（CacheCharacterMemory 依赖）——ORIG 报告逐函数实现。
// ============================================================================

// ORIG 0x0864de58 _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0864de58 | _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
#include "CUser/GetDungeonClear.inc"
{
    sig->m_selected = 0;
    // b28d-lane5：ORIG 0x0864de5f-0x0864de6d 无 cur 槽——`call getCurCharacR;
    // test %eax,%eax; sete %al; test %al,%al; je <body>`（P197④ call 结果寄存器
    // 直比产 sete）+ trace 块 fall-through、`return false` 常量岛。具名 cur 局部
    // 反产 `mov %eax,-0xc; cmpl $0,-0xc` 槽比形（P0 槽-局部双向对应律）。
    if (getCurCharacR() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1400, 5)
            ("CUser::GetDungeonClear User  m_selected is null");
        return false;
    }
    getDungeonClearRefR()->toString(sig->m_clearData, 0x1800);
    return true;
}

// ORIG 0x0867f1d4 _ZNK5CUser17GetBlackListCountEP19SIG_REQ_BLACK_COUNT
#include "CUser/GetBlackListCount.inc"
{
    sig->m_count = 0;
    // ORIG 0x867f1ef-0x867f1f4：call;mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x8(%eax)。
    // b28d-solo-CUser 试形：P211/P274 字段侧异型化——字段 int→unsigned int（SigTypes_C.h:391，
    //   同宽同偏移）+ 删冗余 (unsigned) cast（cast 被 4.4 折叠回同型）⇒ RHS-first + %edx 中转。
    sig->m_blackCount = getCurrCharacBlackCount();
    return 1;
}

// ============================================================================
// 角色内存快照填充：GetRentInfo/GetCharacInfo/GetInventoryInfo/GetTicket（CacheCharacterMemory 依赖）
// 及 helper 链（GetPcRoomRentInfo/GetDimensionRentInfo/getCharacInfoDetail/getTagCharacInfo/
// getTagCharacInventory/GetInventory/removeRentalItemInfoFromInvenLoadBuf）。
// 具名填充 SIG_LOAD_CHARAC/INVENTORY/TICKET（SigTypes.h 权威布局）。
// ============================================================================

// ORIG T 0x086769b4 sz0x35：SIG_LOAD_INVENTORY 的租借区（pcroom@+0x153e4 /
// dimension@+0x153f0）由 GetPcRoomRentInfo/GetDimensionRentInfo 填充。
#include "CUser/GetRentInfo.inc"
{
    GetPcRoomRentInfo(reinterpret_cast<SIG_SAVE_PC_ROOM_RENT_ITEM*>(&sig.m_rentPcRoom));
    GetDimensionRentInfo(reinterpret_cast<SIG_SAVE_DIMENSION_RENT_ITEM*>(&sig.m_rentDimension));
}

// ORIG T 0x086769ea：PC 房间租借物品 → SIG_SAVE_PC_ROOM_RENT_ITEM（最多 2 个 0xc 条目）。
// 逐条目读取 getCurrentCharacRentalInfo(list,1)，对仍存在物品写 [0]itemIndex/[4]byte/[5]byte/
// [6]inEquip/[8]int。参数为租借清单类型 1。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x086769ea | _ZN5CUser17GetPcRoomRentInfoEP26SIG_SAVE_PC_ROOM_RENT_ITEM
#include "CUser/GetPcRoomRentInfo.inc"
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (!m_rentalFlag1)   // ORIG 0x8676a10/0x8676c12 movzbl; xor $0x1,%eax; test;je（bool 取反形）
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 1) != 1)
        return 1;

    int index = 0;
    // 2026-09-10（FL2CUser cv 轴）：ORIG 0x086769ea p167 实形 = 迭代器**混合比较**
    //   `_ZN9__gnu_cxxneIPKN...RentalInfoEPS2_St6vectorIS2_SaIS2_EEEEbRKNS_17__normal_iteratorIT_T1_EERKNS9_IT0_SB_EE`
    //   （第一实参 const_iterator、第二实参 iterator = rentalList.end()）⇒ 原码
    //   **显式 `const_iterator it = rentalList.begin()`**（MISS `C2IPS2_E` 即
    //   iterator→const_iterator 的转换构造；EXTRA 侧非 const 的 de/pp/ne 三条消失）。
    for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        const InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        // 2026-09-10 gA（A 组）：ORIG 0x08676ab2..0x08676ad2 无 inven 指针槽
        //   （`call getCurCharacInvenR` 返回值直接 `mov %eax,(%esp)` 作 this；实参先
        //   物化：`lea -0x29(%ebp),%edx; mov %edx,0xc(%esp)`（&bInEquip）、
        //   `lea -0x30(%ebp)`（&slot）、`mov 0x4(%eax),%ebx`（rental.m_itemIndex））
        //   ⇒ 删具名 inven，改链式内联（P183 地址重物化铁律）。
        // 2026-09-11 CuA A组 P168a/P292：ORIG 0x08676ad7 `call CheckExist…;
        //   xor $0x1,%eax; test %al,%al; jne 08676b78`（bool 返回的**取反早跳**形）
        //   我方旧 `== 1` 产裸 `test %al,%al; je`（无 xor）⇒ 改 `!` 形 + continue
        //   （continue 落点 = 循环尾 operator++，与 ORIG jne 目标 0x08676b79 同址）。
        if (!getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip))
            continue;
        sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
        sig->m_entry[index].m_field4 = rental.m_field8;
        sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
        // b28d-G5（P168a 反向 / N96）：ORIG 0x08676b31 为 `movzbl -0x29(%ebp),%ebx;
        //   … mov %bl,0x6(%eax,%ecx,1)` —— **直接存 bool 原始字节**，无三目物化岛。
        //   我方原 `bInEquip ? 1 : 0` 产 `test %al,%al; je; mov $1,%edx; jmp;
        //   mov $0,%edx`（5 条岛 + 中转）⇒ 改直赋（m_inEquip 为 char，字节同宽）。
        sig->m_entry[index].m_inEquip = bInEquip;
        sig->m_entry[index].m_field8 = rental.m_rentItemId;  // ORIG mov 0xc
        ++index;
        if (index > 1)
            return 0;
    }
    return 1;
}

// ORIG T 0x08676bec：次元租借物品 → SIG_SAVE_DIMENSION_RENT_ITEM（最多 12 个 8 字节条目）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08676bec | _ZN5CUser20GetDimensionRentInfoEP28SIG_SAVE_DIMENSION_RENT_ITEM
#include "CUser/GetDimensionRentInfo.inc"
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (!m_rentalFlag1)   // ORIG 0x8676a10/0x8676c12 movzbl; xor $0x1,%eax; test;je（bool 取反形）
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 2) != 1)
        return 1;

    int index = 0;
    // 2026-09-10（FL2CUser cv 轴，同 GetPcRoomRentInfo）：ORIG 0x08676bec p167 实形
    //   为显式 `const_iterator it = rentalList.begin()`（MISS `C2IPS2_E` 转换构造；
    //   EXTRA 侧非 const de/pp/ne 三条）⇒ 改 const_iterator + const 引用。
    for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        const InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        // 2026-09-11 CuA A组 P183/P0：ORIG 0x08676cd4 调用点无 inven 指针槽
        //   （`call getCurCharacInvenR` 返回值直接 `mov %eax,(%esp)` 作 this，
        //   我方旧具名 `inven` 占 -0xc 槽 ⇒ 全帧槽号整体 +4、多 1 条 load）
        //   ⇒ 链式内联（同 GetPcRoomRentInfo 已落形）。
        if (getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
            sig->m_entry[index].m_inEquip = bInEquip;
            sig->m_entry[index].m_field7 = rental.m_fieldB;   // ORIG movzbl 0xb
            ++index;
            if (index > 11)
                return 0;
        }
    }
    return 1;
}

// ORIG T 0x0864bc04 sz0x1b5：填充 SIG_LOAD_CHARAC。
//   +0x0 acc_id / +0x4 charac_no(*curR 首字段) / +0x8 result(0 初始, 失败置 1) /
//   +0xc CHARAC_DATA（getCharacInfoDetail）/ +0x686 tag_charac_no /
//   +0x68a striker_skill_index / +0x68b TAG_CHARAC_DATA（getTagCharacInfo）。
#include "CUser/GetCharacInfo.inc"
{
    sig->m_accId = get_acc_id();
    // ORIG 0x0864bcc8：无条件 mov (%eax) 解引用（getCurCharacR 恒非空，无检查）
    sig->m_characNo = (unsigned int)getCurCharacR()->m_characNo;
    int characIdx = get_charac_idx(sig->m_characNo);
    sig->m_result = 0;

    // b28c-L5：清 `register bool asm("al")` 存量绑定（P0/N96）。ORIG 0x0864bdf2
    //   `call getCharacInfoDetail; xor $0x1,%eax; test %al,%al; je <正常路径>`
    //   = P168a① 「callee 返回 bool ⇒ 调用点一律裸形」（该 callee 的 ORIG 体尾
    //   仅 `mov $0x1/$0x0,%eax` 两出口 → P159 串/尾形双证 bool，声明已由 Main 代落
    //   CUser.h:3121 int→bool）。X1 约束形/物化 bool 局部均产 sete|setne 三段，
    //   唯有 bool callee + 裸 `!call()` 出单条 xor。
    // err trace 保具名（ORIG err@-0x30 远槽），主块 trace 临时链改全表达式形。
    if (!getCharacInfoDetail(characIdx, sig->m_characNo,
                             reinterpret_cast<CHARAC_DATA*>(&sig->m_characData)))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1052, 0)
            ("getCharacInfoDetail() return = false");
        sig->m_result = 1;
        return false;
    }

    int tagIdx = get_charac_idx((unsigned int)getCurCharacTagCharacNo());
    if (tagIdx >= 0 && tagIdx < (int)m_characList.size())
    {
        // b28c-L5：删 (unsigned int) cast。ORIG 0x0864bd1x `mov 0xc(%ebp),%edx;
        //   mov %eax,0x686(%edx)` = 值留 %eax、base 占 %edx 的**同型直存**形；
        //   原字段 unsigned int ← int 返回值异型使 4.4 多一段 edx 中转（P211）。
        //   字段已随 SigTypes_C.h 改回 int（广播）。
        sig->m_tagCharacNo = getCurCharacTagCharacNo();
        sig->m_strikerSkillIndex = getCurCharacStrikerSkillIndex();
    }
    getTagCharacInfo(tagIdx, sig->m_tagCharacNo,
                     reinterpret_cast<TAG_CHARAC_DATA*>(&sig->m_tagCharacData));
    {
        // ORIG 0x864bcf6/0x864bd01：两值先求值直落 callee-saved %esi/%ebx，
        // 跨 ctor/operator() 存活；主 trace 临时链（P227 全表达式形）翻转与
        // err-trace 的槽分配序（ORIG 主@-0x20/err@-0x30）。
        // b28c-L5：原 `register int …` 无 asm 绑定（合规），但 4.4 对具名
        // register 局部仍按 P0 落槽 ⇒ 保留具名局部写法（ORIG 两值确在 ctor 前
        // 物化并经 %esi/%ebx 跨 call 保活，P227 实证该形态由「实参位内联 +
        // 全表达式临时链」自然产生，无需绑定）。
        // b28c-L5 两处装载形修正：
        //   ① `mov 0x59(%eax),%esi`（ORIG 单 disp，%eax=sig）⇒ 必须走**真成员链**
        //      `sig->m_characData.m_field4d`（4.4 折 base+0xc+0x4d 为一条 disp32）；
        //      原 reinterpret_cast<CHARAC_DATA*>(&m_characData) 使 4.4 先 `add $0xc`
        //      物化子对象地址再读 +0x4d（+1 条）。
        //   ② `mov 0x4(%eax),%ebx`（ORIG 一步直落 callee-saved）⇒ 删多余 (int) cast：
        //      字段 unsigned int 而目标寄存器承载 int 时 4.4 恒插 `mov %eax,%ebx`
        //      中转（P211/N18a 族，varargs 实参位无需显式拓宽）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1066, 0)(
            "CharLastPlayTick char(%d) t(%d)", sig->m_characNo,
            sig->m_characData.m_field4d);
    }
    return true;
}

// ORIG T 0x0864d646：填充 SIG_LOAD_INVENTORY 的背包/金币/装备镜像/cargo/avatar 区。
// creature tail 视图：&m_invenType3+0x38b6 = sig+0x12787（ORIG 单条 add，4.4 对
// 成员地址+常量恒拆双 add ⇒ P82 常量视图折叠单 lea）。
namespace
{
struct SIG_LI_CreatureTailView
{
    char m_pad[0x12787];
    char m_tail[0xf4];
};
}   // namespace
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d646 | _ZN5CUser12GetInventoryEP18SIG_LOAD_INVENTORY
#include "CUser/GetInventory.inc"
{
    // P183：ORIG 每字段访问独立 getCurCharacInvenR()/getCurCharacCargoR() 重物化
    //（P167 缺 getCurCharacInvenR×11 + getCurCharacCargoR×1），禁指针缓存。
    sig->m_money = getCurCharacInvenR()->get_money();
    sig->m_coin = getCurCharacInvenR()->GetCoin();
    sig->m_payCoin = getCurCharacInvenR()->GetPayCoin();
    sig->m_eventCoin = getCurCharacInvenR()->GetEventCoin();
    getCurCharacInvenR()->GetInvenData(1, &sig->m_invenType1, 0x4a58);
    getCurCharacInvenR()->GetInvenData(2, &sig->m_invenType2, 0x1905);
    getCurCharacInvenR()->GetInvenData(3, &sig->m_invenType3, 0x39aa);
    // 装备镜像 = CInventory::m_invenItem 前 22 项（0x53e B）
    memcpy(&sig->m_equipMirror, getCurCharacInvenR()->m_invenItem, 0x53e);
    // creature tail：m_invenType3 区内 +0x38b6 跨条目 0xf4 B（源 m_invenItem[22..25]，
    // ORIG 0x0864d646 忠实保留跨对齐写）。
    memcpy(((SIG_LI_CreatureTailView*)sig)->m_tail,
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);
    getCurCharacInvenR()->GetClearAvatar(sig->m_clearAvatar);   // b28d-solo-CUser：数组退化免 cast（P255/g17 形）
    sig->m_cargoCapacity = getCurCharacCargoR()->GetCapacity();   // 同型直存（GetCapacity 已 unsigned 化）
    getCurCharacCargoR()->get_cargo(&sig->m_cargo);
    sig->m_avatarCoin = getCurCharacInvenR()->m_avatarCoin.Get();   // CInventory::m_avatarCoin@+0x658
    sig->m_inventoryCapacity = getCurCharacInvenR()->get_inventory_capacity();
    return true;
}

// ORIG T 0x0864d4ba：填充 SIG_LOAD_INVENTORY（含 GetInventory + creature + avatar + tag + rent）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d4ba | _ZN5CUser16GetInventoryInfoEP18SIG_LOAD_INVENTORY
#include "CUser/GetInventoryInfo.inc"
{
    sig->m_result = 0;
    // ORIG 0x864d4ba：getData 虚调用 receiver 无槽（P0 内联右值直通
    // `mov (%eax),%edx; add $8; mov (%edx),%ecx; call *%ecx`）——具名 expand
    // 局部落 -0x14/-0x10 槽生槽化 DIFF。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2)->getData(
        (char*)&sig->m_locks[0]);   // 真实虚调用 getData @ vtable+8
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9)->getData(
        (char*)&sig->m_expandEquip);

    // X 族定稿（2026-09-05 CUserC8，判据②直分支/③消费者齐）：ORIG 0x864d534-
    // 0x864d5b3 为三段 `call; xor $0x1,%eax; test %al,%al; jne 共享FAIL岛` 的
    // **短路直落链**（无 bFailed 暂存、无 (x^1)|| 逗号物化）——ORIG xor $1（非
    // 1 语义即真）⇒ 条件用 `== 1`（uchar 提升 int 零扩展，bool 返回会多 movzbl）；
    // GetItem 两出参指针先求值缓存 %esi/%ebx（lea 在 getCurCharacInvenR 调用前）。

    // ORIG 三段均 `call; xor $1,%eax; test %al,%al; jne FAIL岛` = !boolcall()
    // 直判形（P168a①）——GetInventory/GetCreatureItem/GetItem 已 bool 化；
    // 无 bFailed 暂存槽（P198② 内联直测，帧 0x10）。
    if (!GetInventory(sig) ||
        !getCurCharacInvenR()->GetCreatureMgrR()->GetCreatureItem(sig) ||
        !(((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrR()->GetItem(
              sig->m_avatarDetail,          // b28d-solo-CUser：真实类型数组退化（免 cast 出 lea）
              &sig->m_avatarCount)))
    {
        sig->m_result = 1;
        return false;   // ORIG FAIL 岛 mov$0
    }

    if (getTagCharacR() != 0)
        getTagCharacInventory(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(&sig->m_tagInven));
    removeRentalItemInfoFromInvenLoadBuf(sig, 1);
    removeRentalItemInfoFromInvenLoadBuf(sig, 2);
    sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
    return true;   // ORIG 成功尾 mov$1
}

// ORIG T 0x0869067c：TAG 角色背包 → TAG_CHARAC_INVEN_DATA。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0869067c | _ZNK5CUser21getTagCharacInventoryEP21TAG_CHARAC_INVEN_DATA
#include "CUser/getTagCharacInventory.inc"
{
    // b28c-L5 判词（N30 族·lea-vs-add 单点残留）：本波已翻正两处——
    //   ① GetInvenData 的 `add $0x1c(%esp)` 出参组（原 cast 形，与 ORIG 同）；
    //   ② `lea 0x3ff8(%eax),%ebx`（把 TAG_CHARAC_INVEN_DATA::m_clearAvatar 由
    //      char[0x28] 改真实类型 unsigned long[10] 后免 cast，4.4 即折单条 lea，
    //      与 CInventory::GetClearAvatar(unsigned long*) 权威形参同型）。
    //   唯一残留：`lea 0x1e47(%eax),%ebx` vs 4.4 `add $0x1e47,%eax; mov %eax,%ebx`。
    //   同一基址的前一个取址 `lea 0x1e43(%eax),%esi` 两侧一致 ⇒ 差异由 4.4 的
    //   第二次 reload+目标寄存器选择决定（实测 `out->m_avatarDetail` 与
    //   `&out->m_avatarDetail[0]` 两写法产物相同），属 P30 尾判的
    //   「穷举后 add-vs-lea 不齐 ⇒ 结构性伪影不强求」。
    if (getTagCharacR() == 0)
        return false;
    // ORIG：getTagCharacInvenR() 返回 CInventory*（位于 _Charac_info.m_inven，本源码返回 Inven_Item* 同址）
    // P48 逐点重调（ORIG 0x86906b1/0x86906d8/0x8690708/0x8690731 共 4 次）；
    // cargo/GetItem 各 call 结果寄存器直传（无中间槽）。
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetInvenData(2, out->m_items2, 0x1905);
    memcpy(out->m_items,
           reinterpret_cast<const CInventory*>(getTagCharacInvenR())->m_invenItem, 0x53e);
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetClearAvatar(out->m_clearAvatar);   // b28d-solo-CUser：数组退化
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetAvatarItemMgrR()->GetItem(
        out->m_avatarDetail,   // b28d-solo-CUser：真实类型数组退化（免 cast 出 lea）
        &out->m_avatarCount);
    out->m_cargoCapacity =
        (unsigned int)getTagCharacCargoR()->GetCapacity();
    return true;
}

// ORIG T 0x0867776c：从 SIG_LOAD_INVENTORY 背包镜像清除已不存在的租借物品。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867776c | _ZN5CUser36removeRentalItemInfoFromInvenLoadBufEP18SIG_LOAD_INVENTORYi
#include "CUser/removeRentalItemInfoFromInvenLoadBuf.inc"
{
    if (sig == 0)
        return 0;
    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        // b28d-G3C 2026-09-10（P59/P222 分支极性 + P183 去具名指针）：
        //   ORIG 0x08677819 `xor $0x1,%eax; test %al,%al; je +0x10e(=OK块)`
        //   —— bool 取反形（我方原写 `== 1` 只产 `test %al; je`，缺 xor）；
        //   ORIG 0x086777fa 调 getCurCharacInvenR 后**不落槽**直接
        //   `mov %eax,(%esp)`（我方原有具名 `const CInventory* inven` 独占
        //   -0xc 槽：多 1 store + 1 load，并把 rental 引用挤到 -0x10）。
        if (!getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip))
        {
            LogManager::logFormat(
                1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x6d54,
                "Rent error. CUser::removeRentalItemInfoFromInvenLoadBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), rental.m_itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
        else
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip)
                sig->m_equipMirror[slot].reset();
            else
                sig->m_invenType1[slot].reset();
        }
    }
    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
}

// ORIG T 0x0867846e sz0xb7：遍历 m_list8cf80（list<int>）票表 → SIG_LOAD_TICKET。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867846e | _ZNK5CUser9GetTicketEP15SIG_LOAD_TICKET
#include "CUser/GetTicket.inc"
{
    sig->m_field4 = 0;
    // b28d-lane5 复测：ORIG 0x0867846e——begin 直构 it@-0x10、end 直构端@-0x14 各一次，
    //   随后 **index 双初始化**（0x47/0x4e 两条 `movl $0x0,-0xc`＝声明式+for-init 式，
    //   P191）⇒ 旧 `it=m_list8cf80.end()` 每轮重取 + 单次 init 反向。
    std::list<int>::const_iterator it = m_list8cf80.begin();
    std::list<int>::const_iterator itEnd = m_list8cf80.end();
    int index = 0;
    for (index = 0; it != itEnd && index <= 99; ++it, ++index)
    {
        sig->m_tickets[index] = *it;
    }
    sig->m_count = index;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864bdba | _ZNK5CUser19getCharacInfoDetailEijP11CHARAC_DATA
#include "CUser/getCharacInfoDetail.inc"
    // 2026-09-10（G3CUser-B 复验）：栈槽集 ORIG/我方 **均为 0**（本函数无任何标量局部槽，
    //   682:686 insn）；残余 4 条来自 4.4 对 `m_characList[i]` 链式 operator[] 的
    //   调用序列/寄存器形（每字段重调 ~64 次已按 ORIG 逐点展开）⇒ N 族封顶，不动。
    // b28c-L5 SIGFIX（Main 已代落 CUser.h:3121）：ORIG @0x0864bdba 全部出口只写
    //   `mov $0x1/$0x0,%eax`（P159 尾形）+ 唯一外部消费点 GetCharacInfo @0x0864bdf2
    //   `call; xor $0x1,%eax; test %al,%al; je`（P22/P168a① bool callee 直判形）。
{
    // ORIG T 0x0864bdba：op[] 每字段重调 ~64 次（4.4 O0 不 CSE call ⇒ 逐处展开
    // 同形；具名引用缓存折成单取是 gap343 主因）。字段序 = ORIG 汇编写序
    // （报告 Ghidra C 段逐项对齐；宽度 cast 均经 ORIG 指令实证 2026-09-05 CUserC8）。
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    if ((unsigned int)m_characList[param_1].m_characNo != characNo)
        return 0;
    out->m_characNo = m_characList[param_1].m_characNo;                                   // mov (%edx),%eax←(%eax)
    strncpy(out->m_name, m_characList[param_1].m_name, 0x1e);                             // strncpy 0x1e
    out->m_field22 = m_characList[param_1].m_field22;                                     // +0x22←+0x22 movb
    out->m_field24 = m_characList[param_1].m_field26;                                     // +0x24←+0x26 movb(job)
    out->m_field25 = (unsigned short)m_characList[param_1].m_level;                       // +0x25←+0x27 movzwl 零扩展
    out->m_field28 = m_characList[param_1].m_field2b;                                     // +0x28←+0x2b movl
    out->m_field27 = m_characList[param_1].m_field29.m_growthType;                        // +0x27←+0x29 movb
    out->m_unkB2.m_fieldb2 = m_characList[param_1].m_stamina;                             // +0xb2←+0x75 movb
    // ORIG 0x0864bf67..0x0864bfb0：0x52 字节 _Additioal_info **结构整体赋值**，
    // 4.1 内联为「对齐 peel + rep movsl + 尾块」（无 memcpy call；报告 138-165 行）。
    out->m_add = m_characList[param_1].m_additioalInfo;                                   // +0x60←+0x88 0x52B
    out->m_field2c = m_characList[param_1].m_field33;
    out->m_field2e = m_characList[param_1].m_field35;
    out->m_field30 = m_characList[param_1].m_field37;
    out->m_field32 = m_characList[param_1].m_field4f;
    out->m_field4d = m_characList[param_1].m_field7b;                                     // +0x4d←+0x7b movl
    out->m_unkB2.m_field118 = m_characList[param_1].m_field7f;                            // +0x118←+0x7f movl
    out->m_guildId = m_characList[param_1].m_guildId;                                     // +0x56←+0xdbe movl
    out->m_field5a = m_characList[param_1].m_fielddc2;                                    // +0x5a←+0xdc2 movb
    out->m_field5b = m_characList[param_1].m_field77;
    out->m_field5d = m_characList[param_1].m_field79;
    out->m_field5f = (m_characList[param_1].m_fielde22 != 0) ? 1 : 0;                     // +0x5f←(e22!=0)
    ((const CommonTime*)m_characList[param_1].m_delReqTime)->GetCreateTime(out->m_field34); // +0x39→+0x34
    out->m_unkB2.m_fieldb3 = m_characList[param_1].m_field4d;                             // +0xb3←+0x4d 2B
    out->m_unkB2.m_fieldb5 = m_characList[param_1].m_field51;                             // +0xb5←+0x51 movl
    memcpy(out->m_unkB2.m_fieldb9, m_characList[param_1].m_tradeRestrictData, 0x10);      // +0xb9←+0x55 0x10B
    out->m_unkB2.m_fieldfe = m_characList[param_1].m_resetDwordDf4;                       // +0xfe←+0xdf4
    out->m_unkB2.m_field102 = m_characList[param_1].m_resetDwordDf8;                      // +0x102←+0xdf8
    out->m_unkB2.m_fieldc9 =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldTotal();
    out->m_unkB2.m_fieldcd =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldTotalBillion();
    out->m_unkB2.m_fieldd1 =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldDaily();
    out->m_unkB2.m_fieldd5 = m_characList[param_1].m_fielde49;
    out->m_unkB2.m_fieldd9 = m_characList[param_1].m_fielde4d;
    out->m_unkB2.m_fielddd = m_characList[param_1].m_fielde51;
    out->m_unkB2.m_fielde1 = m_characList[param_1].m_fielde55;
    out->m_unkB2.m_fielde5 = m_characList[param_1].m_fielde59;
    out->m_unkB2.m_fielde9 = m_characList[param_1].m_fielde5d;
    out->m_unkB2.m_fielded = m_characList[param_1].m_fielde45;
    out->m_unkB2.m_fieldf1 = m_characList[param_1].m_fielde68;
    out->m_unkB2.m_fieldf5 = m_characList[param_1].m_fielde70;                            // movb
    out->m_unkB2.m_fieldf6 = m_characList[param_1].m_fielde71;                            // movl
    out->m_unkB2.m_fieldfa = m_characList[param_1].m_fielde79;
    out->m_unkB2.m_field10a = m_characList[param_1].m_fielde81;                           // ORIG 序 10a 先于 106
    out->m_unkB2.m_field106 = m_characList[param_1].m_fielde7d;
    out->m_unkB2.m_i134 = (int)get_acc_id();                           // +0x134←get_acc_id
    out->m_unkB2.m_field10e = m_characList[param_1].m_field121f;                          // 2B
    out->m_field23 = m_characList[param_1].m_field25;                                     // +0x23←+0x25 movb（ORIG 尾段）
    out->m_unkB2.m_field110 = m_characList[param_1].m_field1221;                          // movl
    out->m_unkB2.m_field114 = m_characList[param_1].m_field1225;                          // movl
    // b28d-GrpD：ORIG 0x0864c58b..0x0864c5c1 与 0x0864c5b2..0x0864c5e8 两段**逐条同形**
    //   （各 4 条 op[] call + `mov 0x1225(%eax),%edx; mov 0x14(%ebp),%eax;
    //   mov %edx,0x114(%eax)`）；报告 Ghidra C 段亦连续两条 `param_3+0x114 = iVar1+0x1225`
    //   ⇒ 原码此处是**重复赋值**（P171 照抄 ORIG）。
    out->m_unkB2.m_field114 = m_characList[param_1].m_field1225;                          // movl
    out->m_unkB2.m_field11c = (unsigned short)m_characList[param_1].m_field122C;          // movzwl 122c
    out->m_unkB2.m_field11e = m_characList[param_1].m_field1239;
    out->m_unkB2.m_field11f = m_characList[param_1].m_field123e;
    out->m_unkB2.m_field120 = m_characList[param_1].m_field123f;
    out->m_unkB2.m_field121 = (unsigned char)m_characList[param_1].m_field1241;
    out->m_unkB2.m_field122 = m_characList[param_1].m_field126c;                          // 2B
    out->m_unkB2.m_b124 = (char)m_characList[param_1].m_field126e;
    out->m_unkB2.m_i125 = m_characList[param_1].m_field126f;                              // movl
    out->m_unkB2.m_b129 = (char)m_characList[param_1].m_field1275;
    out->m_unkB2.m_b12a = (char)m_characList[param_1].m_field1276;
    out->m_unkB2.m_b12b = (char)m_characList[param_1].m_field1277;
    out->m_unkB2.m_b12d = (char)m_characList[param_1].m_field127a;                        // ORIG 序 12d 先于 12c
    out->m_unkB2.m_b12c = (char)m_characList[param_1].m_field1278;
    out->m_unkB2.m_b12e = m_characList[param_1].m_tagVisibleFlags.m_flag;                 // movzbl e40
    out->m_unkB2.m_b12f = (char)m_characList[param_1].m_field1280;
    out->m_unkB2.m_i130 = m_characList[param_1].m_field12ad;                              // movl
    return 1;
}

// ORIG T 0x086904f0：从 m_characList[param_1] 填充 TAG_CHARAC_DATA（SIG_LOAD_CHARAC +0x68b 区）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086904f0 | _ZNK5CUser16getTagCharacInfoEijP15TAG_CHARAC_DATA
#include "CUser/getTagCharacInfo.inc"
{
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    // b28d-solo-CUser p167：ORIG 0x086904f0 每字段重调 `vector::operator[](uint) const`
    //   （共 8 次；我方缓存 `const _Charac_info& src` 仅 1 次，MISSING ×7）⇒
    //   去缓存逐字段访问（P157/P183）。
    if ((unsigned int)m_characList[param_1].m_characNo != characNo)
        return 0;
    out->m_characNo = m_characList[param_1].m_characNo;
    strncpy(out->m_name, m_characList[param_1].m_name, 0x1d);
    out->m_field21 = m_characList[param_1].m_field26;   // job（ORIG movzbl 读）
    out->m_field22 = (unsigned short)m_characList[param_1].m_level;  // ORIG movzwl 0x27
    out->m_field24 = m_characList[param_1].m_field29.m_growthType;
    out->m_field25 = m_characList[param_1].m_field1239;
    memcpy(&out->m_add, &m_characList[param_1].m_additioalInfo, 0x52);
    return 1;
}

// ORIG T 0x0864d646：CInventory/CCargo 序列化 helper（供 GetInventoryInfo）。

// ORIG 0x08677dd8 _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
// 遍历 getBattleRecordRefR() 的 map<int,GameResultType>，每元素 0x50 字节序列化到
// sig+0x3c8+idx*0x50（最多 100 个），末尾写 count 到 sig+0x3c4。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677dd8 | _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
#include "CUser/GetRecord.inc"
{
    // 2026-09-10（G3CUser-B 复验）：槽集 ORIG/我方 各 4 槽、零平移零多余
    //   （110:110 insn）⇒ 残余纯属跨版本 codegen（含 rep movsl vs memcpy lowering），判 N。
    // b28d-solo-CUser p167：ORIG 0x08677dd8 调 getBattleRecordRefR 3 次
    //   （begin/end/size，无 rec 缓存；我方缓存仅 1 次 ⇒ MISSING ×2），且序列化用
    //   结构整体赋值（rep movsl，报告实证）而非 memcpy call（我方 EXTRA memcpy ×1）
    //   ⇒ 去缓存 + `sig->m_records[index] = it->second;`（P83/P183）。
    // b28d-lane5（probe_slots f1/f2）：ORIG end() **只调一次**（0x08677ecb 循环尾无 getter/end
    //   构造）= 具名缓存；4.4 先声明更浅 ⇒ it(-0x20) 先声明、itEnd(-0x24) 后声明与 ORIG
    //   0x08677dec/0x08677e09 槽址一致。index 裸声明 + for-init 赋值 ⇒ `movl $0,-0x1c` 落到
    //   begin/end 之后（ORIG 0x08677e1b）；getBattleRecordRefR 仍调 3 次（begin/end/size）。
    int index;
    std::map<int, GameResultType>::const_iterator it =
        getBattleRecordRefR()->begin();
    std::map<int, GameResultType>::const_iterator itEnd =
        getBattleRecordRefR()->end();
    for (index = 0; it != itEnd && index <= 99; ++it, ++index)
    {
        sig->m_records[index] = it->second;
    }
    sig->m_recordCount = (int)getBattleRecordRefR()->size();
}

// ORIG 0x08688dc4 _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
// 遍历 m_limitItemUsage 向量，每元素序列化到 sig+(i+0x263)*0x10 的 0x10 字节区。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08688dc4 | _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
#include "CUser/GetLimitItemUsage.inc"
{
    if (sig == 0) return;
    sig->m_limitItemCount = 0;   // ORIG movl $0,0x2638
    // ORIG 0x08688dd5..0x08688e26：begin(-0x14)/end(-0x18) 先求值缓存（P13），
    //   随后**两条** `movl $0,-0xc`（P191 双初始化：声明 + for-init 各写一次）。
    std::vector<LimitItemUsage>::const_iterator it = m_limitItemUsage.begin();
    std::vector<LimitItemUsage>::const_iterator itEnd = m_limitItemUsage.end();
    int index = 0;
    // P157/P183：ORIG 每字段重算 (index+0x263)*0x10 并重调 operator->，禁 dst 缓存。
    // b28d-solo-CUser p167 修正（P28）：ORIG 增量段调 ppEi（后置）⇒ 写 it++。
    for (index = 0; it != itEnd; it++, ++index)
    {
        sig->m_limitItems[index].m_field8 = it->m_field15;   // ORIG mov 0x14(dst)
        sig->m_limitItems[index].m_fieldc = it->m_field19;   // ORIG mov 0x18(dst)
        sig->m_limitItems[index].m_itemIdx = it->m_fieldc;   // ORIG mov 0x10(dst)
        sig->m_limitItems[index].m_used = it->m_field10;     // ORIG movb 0xc(dst)
    }
    sig->m_limitItemCount = index;
}

// ORIG 0x086910a6 _ZNK5CUser27GetCharacCashAttendanceInfoER20SIG_ATTENDANCE_EVENT
#include "CUser/GetCharacCashAttendanceInfo.inc"
{
    *(char*)&info = 0;
}

// ---- 四个 CUser 字段访问器（CacheCharacterMemory 域；ORIG W 弱符号）----
#include "CUser/getTotalPcRoomPlayTime.inc"
{
    return m_totalPcRoomPlayTime;
}
#include "CUser/getPcRoomPlayTimeRewardCnt.inc"
{
    return m_pcRoomPlayTimeRewardCnt;
}
#include "CUser/GetCurCharacUsedGiftFatigueQuantity.inc"
{
    return m_field8ebac;
}
#include "CUser/getFlagPresentAvengerTitle.inc"
{
    return m_ceraUserInfo.m_field62;
}

// ORIG 0x0868ddc2 _ZNK5CUser19GetAuraAvatarOptionER18stAuraAvatarOption
// 读当前角色 _Charac_info+0x1281..+0x1289 三 int → option。ORIG 无 cur 局部：
// getCurCharacR() 判定(0x868ddce)与拷贝(0x868dde2)各重调一次（P48，调用点差集归零）。
// 记 N（跨版本 regmove，N18a 族）：4.1 O0 将 option 基址驻 %edx 单装载 +
// `0x1281(%eax)` 索引三连；4.4 O0 对引用形参逐条重装载，register-asm 绑定反产
// 冗余 regmove（probe 四形 21/23/26/28/30 均不达 24）——语义+调用点全对，残 +4。
#include "CUser/GetAuraAvatarOption.inc"
{
    // b28c-L5：清 `register const _Charac_info* asm("eax")` 存量绑定（P0/N96）。
    //   判词（布局视图缺口，非源形问题）：ORIG 拷贝段是
    //     `mov 0xc(%ebp),%edx`（**目标** option 基址驻 %edx）+
    //     `mov 0x1281(%eax),%ecx …`（**源**以 this 相对 disp32 直读）的 12 字节
    //   POD 结构整体赋值形。当前 `_Charac_info` 只有 `struct{int×3}`/`int[3]`
    //   两视图、无 stAuraAvatarOption 成员 ⇒ 任何写法都只能先 `add $0x1281`
    //   物化源地址（4.4 把源基址留给 callee-saved、目标挤到 %eax，装载方向反）。
    //   已按 §5.3 申请在 CUser.h:692 union 内补具名结构视图（Main 已代落
    //   `stAuraAvatarOption m_auraAvatarOption;`，+0x1281，0xc 字节，零布局影响）。
    // ORIG 0x0868ddc2 拷贝段 =
    //   `mov 0xc(%ebp),%edx / mov 0x1281(%eax),%ecx / mov %ecx,(%edx) /
    //    mov 0x1285(%eax),%ecx / mov %ecx,0x4(%edx) / mov 0x1289(%eax),%eax /
    //    mov %eax,0x8(%edx)`
    // —— 目标基址**单装载驻 %edx** + 源基址驻 %eax 连续三 dword，正是 12 字节
    // POD 结构整体赋值的 4.1 展开形（P83：结构整体赋值 vs 逐字段/ memcpy 三形互斥；
    // 逐字段写 4.4 恒每字段重装载两侧基址 ⇒ 多 4 条，原绑定期靠钉 %eax 勉强凑）。
    // 源子对象已由 CUserCharacInfo.h:499 union 具名建模（m_auraAvatarOption @+0x1281）
    // ⇒ 用具名结构成员整体拷贝，无需指针缓存亦无裸偏移（§4.3）。
    if (getCurCharacR() == 0)
        return;
    option = getCurCharacR()->m_auraAvatarOption;   // +0x1281 具名结构整体拷贝
}

// ORIG 0x0868f03c _ZNK5CUser18GetCurCharacOptionER15CharacterOption
// 0x200 字节 **结构体赋值** 自 _Charac_info+0x12b7（CharacterOption::m_pad[0x200]）：
// ORIG 4.1 内联为「对齐处理 + rep movsl + 尾块」序列（无 memcpy call），
// 且 getCurCharacR() 调两次（+0x0f 判空、+0x27 取址），不落具名 cur 缓存（P183）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f03c | _ZNK5CUser18GetCurCharacOptionER15CharacterOption
#include "CUser/GetCurCharacOption.inc"
{
    // b28d-lane5 复测（P319 整型加法形 + P222 正向早退）：
    //   ORIG 0x0868f050 `test %eax,%eax; sete %al; test %al,%al; jne <尾>`（极性取反的
    //   失败早退）与 0x0868f068 `mov %eax,%edx; add $0x12b7,%edx`（两段地址算术）。
    if (getCurCharacR() == 0)
        return;
    // 2026-09-11 L9GrpF（P83 结构整体赋值 + P183 去裸偏移）：ORIG 0x0868f068
    //   `mov %eax,%edx; mov 0xc(%ebp),%eax; add $0x12b7,%edx`（目标基址驻 %eax、源以
    //   成员偏移在 %edx 上算）；`reinterpret_cast<unsigned int>` 算术形产
    //   `add $0x12b7,%eax; mov %eax,%edx` 反序。`_Charac_info` 已具名建模
    //   `CharacterOption m_characterOption;`（CUserCharacInfo.h:593，+0x12b7）
    //   ⇒ 用具名成员整体拷贝（同 GetAuraAvatarOption 已 AE 的 m_auraAvatarOption 先例）。
    //   探针 /tmp/L9/F/ph1.cpp h1：与 ORIG 逐条同形（h0=现形不一致）。
    option = getCurCharacR()->m_characterOption;
}

// ORIG 0x0864dcf2 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864dcf2 | _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
#include "CUser/GetSkill.inc"
{
    // b28c-L5 判词（N30/N18a 族·lea-vs-add 跨版本折叠差，语义零差）：ORIG 对
    //   `&skill->m_remainSpTree{1,2}/m_remainSfpTree{1,2}` 三个 callee-saved 实参
    //   恒走「地址算术在 %eax 上做 → 再 mov 到 %edi/%esi/%ebx」两条
    //   （`mov 0xc(%ebp),%eax; add $0x340,%eax; mov %eax,%edi`），而 4.4-O0 折叠为
    //   单条 `lea 0x340(%eax),%edi`（我方 -3 insn）。同函数内**非 callee-saved**
    //   的那处 `mov 0xc(%ebp),%eax; add $0x1a8,%eax`（memcpy 目标）两侧完全一致，
    //   证明差异只在「目标寄存器是否 callee-saved」这一 lea 折叠决策上，与源形无关
    //   （cast/具名局部/语句表达式三形实测均维持 lea 单条）⇒ P30 尾判：
    //   「穷举后 add-vs-lea 与寄存器名仍不齐 → 结构性伪影不强求」。
    //   本波已翻正项：get_skill_command_buf 的具名结果指针 + 判空（原缺，
    //   ORIG `mov %eax,-0x1c(%ebp); cmpl $0x0,-0x1c(%ebp); je` 实证 P0 有槽往返）
    //   ⇒ 同时把 cmds 数组顶到 ORIG 的 -0xe4（原 -0xe0 帧差 4 即此槽缺失所致）。
    skill->m_field04 = 0;
    // ORIG：getCurCharacSkillR 在 get_remain_sp/get_skill_slot/getComboSkillSaveData
    // 三处各自重调——访问器结果禁缓存（战术 P183/P48）。
    ((SkillSlot*)getCurCharacSkillR())
        ->get_remain_sp(&skill->m_remainSpTree1, &skill->m_remainSpTree2,
                        &skill->m_remainSfpTree1, &skill->m_remainSfpTree2);
    ((SkillSlot*)getCurCharacSkillR())
        ->get_skill_slot((char*)skill->m_skillSlotTree1,
                         (char*)skill->m_skillSlotTree2, 0x198);
    STSkillCommandData::STCommand cmds[50];
    memset(cmds, 0, sizeof(cmds));
    // b28c-L5：ORIG 0x0864ddxx 在 call 之后另有一次「cmds 基址落槽 + 判空」：
    //   `call get_skill_command_buf / lea -0xe4(%ebp),%eax / mov %eax,-0x1c(%ebp) /
    //    cmpl $0x0,-0x1c(%ebp) / je +0x127 / …mov -0x1c(%ebp),%eax → memcpy 源`
    //   ⇒ 原码有具名指针局部（P0：该值有槽往返），且声明点在 call **之后**
    //   （4.1 初始化前置族 N88 的反向：此处落槽动作确实发生在 call 之后）。
    //   该具名槽同时把 cmds 数组顶到 -0xe4（我方原形 -0xe0 的 4 字节差即此槽缺失，
    //   非 N95 帧基差）。memcpy 源改用该指针（ORIG 从 -0x1c 回读）。
    get_skill_command_buf(cmds);
    STSkillCommandData::STCommand* pCmdBuf = cmds;
    if (pCmdBuf != 0)
    {
        memcpy(skill->m_commands, pCmdBuf, sizeof(cmds));
    }
    // ORIG 0x0864de1e `movzbl 0xa850(%eax)`（unsigned char 宽实证）
    skill->m_field411 = G_CDataManager()->m_pvpSkillTree.m_field50;
    ((SkillSlot*)getCurCharacSkillR())->getComboSkillSaveData(combo);
    return 1;
}

// ORIG 0x08654768 _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
// 将 CUserCharacInfo 角色内存 +0x1254 处（getSkillCommand()）的 skill command map
// （std::map<int,STSkillCommandData>）逐项压缩为 STCommand（32 位位域）写入 out[]，最多 50 项。
// map 为空指针时 out 不变；返回 out（ORIG 返回未定义值，调用方 GetSkill 忽略返回值）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配（槽 -0x10/-0x14/-0x18 与中转寄存器差，语义/调用点全同）| ORIG 0x0865459a | _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
#include "CUser/get_skill_command_buf.inc"
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return out;
    // ORIG 0x08654768：`end()` 提到循环外（循环体内无 end() 调用），`idx > 0x31`
    //   走循环尾 `jg`（= 显式 break 形，避免 `&&` 的 setne/test bool 物化，P133/P59）。
    std::map<int, STSkillCommandData>::iterator it = pMap->begin();
    std::map<int, STSkillCommandData>::iterator itEnd = pMap->end();
    int idx = 0;
    for (; it != itEnd; )
    {
        // b28d-solo-CUser p167：ORIG 0x0865459a/0x08654768 每字段各发一次
        //   `_Rb_tree_iterator::operator->()`（共 6 次，我方缓存引用 v 缺 6 次）⇒
        //   去掉具名引用，逐字段 `it->second.m_xxx`（P167 判据/P183 反向）。
        STSkillCommandData::STCommand cmd;
        cmd.m_command = it->second.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = it->second.m_count;     // bits 29-31
        cmd.m_key0    = it->second.m_key0;      // bits 14-16
        cmd.m_key1    = it->second.m_key1;      // bits 17-19
        cmd.m_key2    = it->second.m_key2;      // bits 20-22
        cmd.m_key3    = it->second.m_key3;      // bits 23-25
        cmd.m_key4    = it->second.m_key4;      // bits 26-28
        out[idx] = cmd;
        ++idx;
        if (idx > 0x31)
            break;
        ++it;
    }
    return out;
}

// ORIG 0x0865459a _ZN5CUser21get_skill_command_bufEv
// 同上，但写入成员 m_skillCommands[50]（+0x703db）并返回其指针；map 为空指针返回 0。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配（槽 -0x10/-0x14/-0x18 与中转寄存器差）| ORIG 0x0865459a | _ZN5CUser21get_skill_command_bufEv
#include "CUser/get_skill_command_buf_1.inc"
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return 0;
    // 2026-09-10 gA（A 组）P13/P133：
    //  · ORIG 0x086545cb 只调一次 map::begin()（sret -0x14 → 拷入 it -0x1c 槽），
    //    0x086545e0 只调一次 end()（-0x18 槽）；循环体 0x086545fa..0x08654753 内
    //    **无** begin/end 调用 ⇒ 具名缓存 itEnd（我方旧形每轮重算 end()，+4 条）。
    //  · ORIG 循环尾序为 `addl $0x1,-0xc(%ebp); cmpl $0x31,-0xc(%ebp); jg END;
    //    lea -0x1c; call operator++()`（++idx → 判 idx>0x31 退出 → ++it），条件
    //    `call operator!=; test %al,%al; jne body` 直跳无物化；写成
    //    `it != itEnd && idx <= 0x31` 会多出 `mov $0x1/%0x0; test` bool 物化（P133）
    //    ⇒ 拆为循环尾显式 break 形。
    // [L9-GrpE 2026-09-11 P212 声明序/物化点] ORIG 0x086545e8 `movl $0x0,-0xc(%ebp)`
    //   位于 begin()（0x086545cb）/end()（0x086545e0）**之后**、`it = tmp`（0x086545ef）
    //   之前 —— 原码 `int idx = 0;` 写在两个迭代器声明之后；我方旧形把 idx 提到
    //   begin 之前 ⇒ 该 movl 位置前移（137/134 的顺序残差之一）。
    std::map<int, STSkillCommandData>::iterator it = pMap->begin();
    std::map<int, STSkillCommandData>::iterator itEnd = pMap->end();
    int idx = 0;
    for (; it != itEnd; )
    {
        // b28d-solo-CUser p167：同带参版，ORIG 每字段各发一次 operator->（P167/P183）。
        STSkillCommandData::STCommand cmd;
        cmd.m_command = it->second.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = it->second.m_count;     // bits 29-31
        cmd.m_key0    = it->second.m_key0;      // bits 14-16
        cmd.m_key1    = it->second.m_key1;      // bits 17-19
        cmd.m_key2    = it->second.m_key2;      // bits 20-22
        cmd.m_key3    = it->second.m_key3;      // bits 23-25
        cmd.m_key4    = it->second.m_key4;      // bits 26-28
        m_skillCommands[idx] = cmd;
        ++idx;
        if (idx > 0x31)
            break;
        ++it;
    }
    return m_skillCommands;
}

// ORIG 0x0867794c _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// 依 SIG_LOAD_ETC 具名布局填充。expand 子区经 CExpandEquipslot vtable 槽 +8（getData）
// 直调（同 CacheCharacterMemory 的 0xc 槽位写法）；混合区内子对象用文档化偏移定位。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867794c | _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// [b28d-GrpD 判词·无实差异] ORIG 0x0867794c：norm 后残差仅 `lea 0xN(%R),%R` vs
//   `add $BIG,%R`（数据地址未解析重定位）与寄存器选择差异；字段偏移、PvpResultType::Get
//   成员直调序全一致 ⇒ 非可改项。
#include "CUser/GetEtcInfo.inc"
{
    sig->m_field8 = 0;
    // ORIG 0x08677979：PvpResultType::Get(source, dest) 成员直调，非裸 memcpy。
    getPVPResultR()->Get(sig->m_pvpResult);
    GetRecord(sig);
    GetContinuousItemInfo(sig);
    GetLimitItemUsage(sig);
    sig->m_oneDayLetheFlag = GetOneDayLetheFlag();
    memcpy(sig->m_backupSkill, GetCurCharacBackupSkill(), 0x199);
    sig->m_oneDayLetheFlag2ND = GetOneDayLetheFlag2ND();
    memcpy(sig->m_backupSkill2ND, GetCurCharacBackupSkill2ND(), 0x199);
    sig->m_curConditionEventStep = GetCurConditionEventStep();
    sig->m_curConditionEventRewardStep = GetCurConditionEventRewardStep();
    sig->m_properDungeonClearCount = GetProperDungeonClearCount();

    // expand type 6 → +0x40ce（真实虚调用 getData @ vtable+8）。
    // 2026-09-10（P183/P0，报告 0x08677a79..0x08677a95 实证）：ORIG 对每个 expand 段
    //   都是 `movl $type,0x4(%esp); … call GetCharacExpandDataR; mov (%eax),%edx;
    //   add $0x8,%edx; mov (%edx),%ecx; … call *%ecx` **全链内联**，帧仅 sub $0x10 ⇒
    //   原码无任何 `CExpandEquipslot* eXX` 具名局部（我方 12 个局部致帧 0x40、+40 指令）⇒
    //   全部改链式单表达式。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)6)->getData((char*)sig->m_expandData6);

    sig->m_curVillageAttackCount = GetCurVillageAttackCount();
    sig->m_curRevengeDungeonCount = GetCurRevengeDungeonCount();
    sig->m_flagPresentAvengerTitle = getFlagPresentAvengerTitle();
    sig->m_totalPcRoomPlayTime = getTotalPcRoomPlayTime();
    sig->m_pcRoomPlayTimeRewardCnt = getPcRoomPlayTimeRewardCnt();
    sig->m_curCharacUsedGiftFatigueQuantity = GetCurCharacUsedGiftFatigueQuantity();
    // ORIG 调 std::bitset<32>::set()（无参，全部 32 位置 1 → 0xffffffff）；
    // 直接赋值会经 bitset(unsigned long) ctor（EXTRA）而非 set()。
    sig->m_bitset.set();

    // skill 区（+0xc9c0）+ combo（+0x14aaa）
    GetSkill((SIG_LOAD_SKILL*)sig->m_mixedRegion, (SIG_COMBO_SKILL*)sig->m_comboSkill);

    // 混合区内 expand 子区（m_mixedRegion 具名成员 + 区内文档化下标）。
    // ORIG 顺序：type 7/0xd/8/0xb 无条件；tag 角色存在时只填充 tag 角色技能槽，
    // 其余 type 0xa/0x10/0xe/0xf/0x11/0x12、aura、option、attendance、
    // blueMarble、type 0x13 均无条件执行。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)7)
        ->getData(&sig->m_mixedRegion[0xcf1f - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xd)
        ->getData(&sig->m_mixedRegion[0xd718 - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8)
        ->getData(&sig->m_mixedRegion[0xcdd5 - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xb)
        ->getData(&sig->m_mixedRegion[0xd5a7 - 0xc9c0]);

    // tag 角色技能槽（+0x10/+0x1a8）
    if (getTagCharacR() != 0)
    {
        // SIG_LOAD_ETC 新结构：tag 区为 TAG_CHARAC_ETC_DATA（2 行 × 0x198，
        // Lane6 已将平铺对拆成真实二维数组 m_skills[2][204]，见 SigTypes_C.h:300）。
        ((SkillSlot*)getTagCharacSkillR())->get_skill_slot(
            reinterpret_cast<char*>(sig->m_tagEtc.m_skills[0]),
            reinterpret_cast<char*>(sig->m_tagEtc.m_skills[1]), 0x198);
    }

    // ORIG 0x0867794c：expand type 0xa → mix+0xd6fc（0xd6fc - 0xc9c0）。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xa)
        ->getData(&sig->m_mixedRegion[0xd6fc - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x10)
        ->getData(&sig->m_mixedRegion[0xd73c - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xe)
        ->getData((char*)sig->m_expandDataE);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xf)
        ->getData((char*)sig->m_expandDataF);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x11)
        ->getData(&sig->m_mixedRegion[0xecc8 - 0xc9c0]);
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x12)
        ->getData(&sig->m_mixedRegion[0xecd0 - 0xc9c0]);

    // aura（+0xd730）/ character option（+0xeac8）在混合区内
    GetAuraAvatarOption(
        *(stAuraAvatarOption*)&sig->m_mixedRegion[0xd730 - 0xc9c0]);
    GetCurCharacOption(
        *(CharacterOption*)&sig->m_mixedRegion[0xeac8 - 0xc9c0]);

    // attendance（+0x14a99）+ blueMarble（+0x14a98）+ expand 0x13（+0x14a9a）
    GetCharacCashAttendanceInfo(*(SIG_ATTENDANCE_EVENT*)&sig->m_attendanceEvent);
    sig->m_blueMarbleEnterCount = getBlueMarbleEnterCount();
    // type 0x13 区 = attendanceEvent 内 +1 字节起（0x14a99 起 0x11 字节，0x14a9a=+1）。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x13)
        ->getData((char*)&sig->m_attendanceEvent + 1);

    return 1;
}

// ORIG T 0x08650aa2 sz0x87f：SIG_SAVE_CHARAC(0x10d) 组包；四个脏块分支后投递
// 队列 2 并清 save 标志。个别字段写入为报告偏移直写（推断标注见注释）。
// ORIG T 0x088921c2 `_Z11setBit_ByteRhbi`（定义在 BitTools.cpp；本 TU 唯一调用点在下）。
#include "CUser/setBit_Byte.inc"

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08650aa2 | _ZN5CUser15_SaveCharacDataEv
#include "CUser/_SaveCharacData.inc"
{
    SetAccountLastPlayTime(GlobalData::s_systemTime_.getCurSec());

    if (getCurCharacVill() == 7)   // power war 村
    {
        setCurCharacLastPlayTickPowerWar(GlobalData::s_systemTime_.getCurSec());
        SetCurCharacChaosDieTime(0);
        m_characterView.enableSaveCharacView();
    }
    setCurCharacLastPlayTick(GlobalData::s_systemTime_.getCurSec());

    if (!isSaveCharacInfo() && !IsSaveCharacStat() && !isSaveCharacEtc() &&
        !IsSaveCharacManageInfo())
        return;

    // 本件反例（_SaveCharacData）：ORIG 0x0864a5xx 有 `mov %eax,-0x20(%ebp)` +
    // `mov -0x20(%ebp),%eax`（Acquire 结果**有**具名槽）⇒ 保留 Stream* 局部。
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x19c3);
    CStreamGuard guard(s, true);
    // b28d-CUser p167 真缺口：ORIG 显式调 CStreamGuard::operator*（deEv）×2 与
    //   operator->（ptEv）×1 再 GetInBuffer（P132/P103）；直接成员调用少 3 个调用点。
    (*guard)->operator<<((int)0x22);         // SIG_SAVE_CHARAC
    (*guard)->operator<<((int)GetUID());
    SIG_SAVE_CHARAC* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_CHARAC>();
    memset(sig, 0, sizeof(SIG_SAVE_CHARAC));

    sig->m_dungeonGainGold = getDungeonGainedGold()->getGainedGold();
    sig->m_ssoId = (unsigned int)get_acc_id();
    sig->m_reportCnt = m_field8d110;   /* ORIG this+0x8d110 实证 */
    sig->m_ssoTradeGoldDaily = GetOverTradeGoldCount();
    sig->m_rating = GetRating();              // ORIG fstps 0xf6（4B float 实证）
    sig->m_tutorialSkipable = GetTutorialSkipable();
    sig->m_characNo = getCurCharacNo();

    sig->m_bSaveInfo = isSaveCharacInfo();
    sig->m_bSaveStat = IsSaveCharacStat();
    sig->m_bSaveInfoSub = isSaveCharacEtc();
    sig->m_bSaveManageInfo = IsSaveCharacManageInfo();
    sig->m_bSaveCubePremiumInfo = isSavedPremiumCubeInfo();
    if (sig->m_bSaveCubePremiumInfo)
    {
        // ORIG 0x08650d30/0x08650d39 lea 直传包内字段地址（0x10b/0x10c）。
        getContractOfCubePremiumInfo(sig->m_cubeSelected, sig->m_cubeType);
    }
    if (sig->m_bSaveInfo)   // info 块
    {
        sig->m_lev = get_charac_level();             // ORIG mov %dx（2B）
        sig->m_growType = getCurCharacGrowType();
        // ORIG 0x08650d8c..0x08650da4：0x52 字节 _Additioal_info **结构整体赋值**
        // （对齐 peel + rep movsl 内联，无 memcpy call；源 _Charac_info+0x88）。
        *(_Additioal_info*)&sig->m_maxHP =
            getCurCharacR()->m_additioalInfo;
        sig->m_expertJob = GetCurCharacExpertJobType();
    }
    if (sig->m_bSaveStat)   // stat 块
    {
        sig->m_exp = getCurCharacExp();
        sig->m_village = getCurCharacVill4Save();
        sig->m_fatigue = GetFatigue();        // ORIG mov %ax（2B）
        sig->m_premiumFatigue = getCurCharacPremiumFatigue();
        sig->m_hp = getCurCharacStamina();
        sig->m_usedFatigue = getCurCharacUsedFatigue();
        sig->m_tutorialFlag = getCurCharacTutorialFlag();
        if (getCurCharacR() != 0)
            memcpy(sig->m_escaladeTutorialFlag, getCurCharacEscaladeTutorialFlag(), 0x10);
        sig->m_tradeGoldTotal = getCurCharacTradeGoldTotal();
        sig->m_tradeGoldTotalBillion = getCurCharacTradeGoldTotalBillion();
        sig->m_tradeGoldDaily = getCurCharacTradeGoldDaily();
        sig->m_helpAbuseRatio = getCurCharacDungeonHelpAbuseRatio();
        sig->m_helpAbuseExp = getCurCharacDungeonHelpAbuseExp();
        sig->m_chaosPoint = GetCurCharacChaosPoint();
        sig->m_chaosExp = GetCurCharacChaosExp();
        sig->m_chaosModeCount = GetCurCharacChaosModeCount();
        sig->m_chaosKillCount = GetCurCharacChaosKillCount();
        sig->m_chaosDieCount = GetCurCharacChaosDieCount();
        sig->m_chaosDieTime = GetCurCharacChaosDieTime();
        sig->m_openFlag = GetOpencoin();
        sig->m_assaultCount = GetAssaultCount();
        sig->m_chaosKillTime = GetCurCharacChaosKillTime();
        sig->m_luckPoint = GetCurCharacLuckPoint();
        sig->m_dungeonPlayCount = GetCurCharacDungeonPlayCount();
        sig->m_expertJobExp = GetCurCharacExpertJobExp();
        sig->m_fatigueBatteryCharging = GetFatigueBatteryCharging();
        sig->m_powerWarPoint = GetUserPowerWarPoint();
        sig->m_villagePrev = getCurCharacVillPrev();
        sig->m_powerWarAssaultCount = GetPowerWarAssaultCount();
        sig->m_powerWarAssaultVictoryCount = GetPowerWarAssaultVictoryCount();
        sig->m_fatigueGrownupBuff = getCurCharacFatigueGrownUpBuff();
        sig->m_growthPowerReward = get_growth_power_reward();
        sig->m_memberDungeonFlag = getMemberDungeonFlag();
        sig->m_memberBonusFatigue = getCurCharacMemberBonusFatigue();
        sig->m_lastPlayDungeonIndex = getCurCharacLastPlayDungeonIndex();
        sig->m_chaosResponTime = GetCurCharacChaosResponTime();
        sig->m_addEquipslotFlag = getCurEquipslotFlag();
        sig->m_channelEquipslotSwitch = getCurChannelEquipslotSwitch();
        sig->m_expandEquipslotSwitch = getCurExpandEquipslotSwitch();
        sig->m_lastPlayTimePowerwar = getCurCharacLastPlayTickPowerWar();
        sig->m_emotion = GetCurCharacEmotion();
        sig->m_skillTreeIndex = GetCurCharacSkillTreeIndex();
        sig->m_addSlotFlag = GetCurExEquipSlotStat();
        getCurCharacInvenW()->GetAvatarItemMgrW()->SaveEndurance();
    }
    if (sig->m_bSaveInfoSub)   // etc 块
    {
        sig->m_guildRight = get_guild_create_right();
        sig->m_maxPremiumFatigue = getCurCharacMaxPremiumFatigue();
        sig->m_eventCharacLevel = getCurCharacEventCharacterGrowtype();
    }
    if (sig->m_bSaveManageInfo)   // manage info 块
    {
        sig->m_strikerSkillIndex = getCurCharacStrikerSkillIndex();
        sig->m_tagCharacNo = getCurCharacTagCharacNo();
        sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
    }

    sig->m_lastPlayTime = getCurCharacLastPlayTick();
    sig->m_visibleFlags = get_charac_visible_values();
    // ORIG 0x08651298：显式调全局 setBit_Byte(&sig->m_visibleFlags, false, 0)
    // （movb 写 + lea 0x102(%eax) 取址；非 &= ~0x01 直算）。
    setBit_Byte((unsigned char&)sig->m_visibleFlags, false, 0);

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    disableSaveCharacInfo();
    disableSaveCharacStat();
    disableSaveCharacEtc();
    disableSaveCharacManageInfo();
    disableSavedPremiumCubeInfo();
}

// ORIG T 0x0864d84e sz0x2a：param!=0 时 push_back 到受限商品列表(+0x711ec)。
#include "CUser/SetRestrictedGoods.inc"
{
    if (itemIdx != 0)
    {
        m_list711ec.push_back(itemIdx);
    }
    return;
}

// ORIG T 0x0864d878 sz0x91：线性查找列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d878 | _ZN5CUser17IsRestrictedGoodsEi
#include "CUser/IsRestrictedGoods.inc"
{
    if (itemIdx == 0)
        return false;
    // b28d-solo-CUser N封顶(N86/N89): ORIG 每轮重算 this+0x711ec 且 begin/end 各占独立
    //   sret 临时槽 -0xc/-0x10；4.4 缓存基址复用 -0x14 槽（非 const iterator 形实测
    //   ndiff 不变 20，回退）。
    // b28d-solo-CUser p167：ORIG 0x0864d878 用非 const `_List_iterator<int>`（我方
    // const_iterator 多转换构造 + 混合 operator!=）⇒ 改非 const iterator（P96/P50）。
    // b28d-solo-CUser 重写（2026-09-09，P13）：ORIG 0x0864d878 在循环头**缓存 end**
    //   （begin sret -0xc / end sret -0x10，条件只调 ne）；旧形每轮重调 end 且槽反。
    for (std::list<int>::iterator it = m_list711ec.begin(),
                                  e = m_list711ec.end();
         it != e; ++it)
    {
        if (*it == itemIdx)
            return true;
    }
    return false;
}

// ORIG T 0x0864d9a4 sz0x17b：header(0,0x44)；flag=true 单条，false 全列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d9a4 | _ZN5CUser19SendRestrictedGoodsEbi
#include "CUser/SendRestrictedGoods.inc"
{
    PacketGuard packet;
    // ORIG 0x0864d9b2..0x0864dad6：无 pkt 具名指针槽（-0xc 槽不存在），每次访问都
    // 重发 `lea -0x14(%ebp),%eax; mov %eax,(%esp)` ⇒ P183 不缓存 cast 结果。
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x44);
    // ORIG 0x0864d9d8 `movzbl -0x2c(%ebp),%eax; xor $0x1,%eax; test %al,%al;
    //   je <put_byte(0) 块>`，put_byte(1) 块 fall-through ⇒ 4.1 的 then/else 物理序相反：
    //   写 `if (flag) {A} else {B}` 4.4 产 `cmpb $0,-0x2c; je <B>`（我方 then fall-through）。
    //   反写 `if (!flag) {B} else {A}` 才逐条复现（P59 双向布局律），语义等价。
    if (!flag)
    {
        ((InterfacePacketBuf*)&packet)->put_byte(1);
        ((InterfacePacketBuf*)&packet)->put_short((int)m_list711ec.size());
        // b28d-solo-CUser p167：同 IsRestrictedGoods，ORIG 用非 const `_List_iterator<int>`。
        // ORIG 0x0864da22(选 it)/0x0864da3d(选 end) 把 end 在循环**前**缓存一次（P13）；
        //   `it != m_list711ec.end()` 会让 4.4 每轮回调 end()。
        for (std::list<int>::iterator it = m_list711ec.begin(),
                                      endIt = m_list711ec.end();
             it != endIt; ++it)
        {
            ((InterfacePacketBuf*)&packet)->put_int(*it);
        }
    }
    else
    {
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        ((InterfacePacketBuf*)&packet)->put_int(itemIdx);
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// ORIG T 0x08656636 sz0xad：遍历 m_characList 统计 _Charac_info+0x27(short)==type。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08656636 | _ZN5CUser24GetSpecialLevelCharacCntEi
#include "CUser/GetSpecialLevelCharacCnt.inc"
{
    // ORIG 0x08656636 逐段：cnt=0(-0xc) → begin 临时(-0x14) → const 转换(-0x1c)
    //   → end 临时(-0x10) → const 转换(-0x20) → 循环只比一次缓存 end（P13）；
    //   m_characList 非 const ⇒ 两处 iterator→const_iterator 转换构造（P326 三级槽阶梯）。
    int cnt = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itEnd = m_characList.end();
    for (; it != itEnd; ++it)
    {
        if (it->m_level == type)
            ++cnt;
    }
    return cnt;
}

// ORIG _ZNK5CUser22HasCurCharacRentalItemEi T 0x086772d2 sz0x117：
// 外层 map 键=rental 类型，内层键=当前角色 characNo（+0x00），vector 非空即有。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086772d2 | _ZNK5CUser22HasCurCharacRentalItemEi
#include "CUser/HasCurCharacRentalItem.inc"
{
    // b28d-solo-CUser 试形（P198 指针侧孪生判据）：ORIG 0x086772d2 段为
    //   `mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp); jne …`（结果先落槽再判空）
    //   ⇒ 原码「声明与赋值分两句」（初始化式形 4.4 折叠为寄存器直比）。
    // 2026-09-10：ORIG 0x08677413..0x0867741b 对 cur->m_characNo **落槽 -0x20 再判空**
    //   （P0 槽往返 ⇒ 原码具名 characNo 局部）；旧形内联 `*(const unsigned int*)cur`
    //   无槽且需把 cur 缓存进 %ebx（ORIG 无 push ebx）。
    // 2026-09-10 复测：拆两条独立守卫 + 具名 characNo 实测 -27/+26 → -38/+39（反劣，
    //   块布局被拆散）⇒ 维持合并短路形；残余为 N126 首槽基址常数（-0xc vs -0x10）。
    const _Charac_info* cur;
    cur = getCurCharacR();
    if (cur == 0 || *(const unsigned int*)cur == 0)
        return false;
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::const_iterator outer =
        m_rentalInfo.find((unsigned int)type);
    if (outer == m_rentalInfo.end())
        return false;
    // 2026-09-10（FL2CUser p167）：外层 `->` 亦只出现 **1 次**（EXTR 内层 ptEv 消除后
    //   仍余外层 ptEv ×1）⇒ 原码先把 `outer->second` 绑成引用再复用，`end()` 亦走该引用。
    const std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> >& innerMap =
        outer->second;
    std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator inner =
        innerMap.find(*(const unsigned int*)cur);
    // 负结论（2026-09-10 复测，外层引用缓存后重试）：ORIG 0x08677413..0x0867741b
    //   的 `mov %eax,-0x20(%ebp); mov -0x20(%ebp),%eax` characNo 具名槽往返
    //   **无法复现**——`const unsigned int characNo` 具名形实测 ndiff 13→16、
    //   ours 79→82（4.4 直接内联该右值，不落槽）⇒ 回退，维持内联形。
    //   （该 -0x20 死槽属跨版本槽分配差，N 封顶。）
    // p167（2026-09-10）：EXTRA `_ZNKSt23_Rb_tree_const_iteratorI...EdeEv` ×1 而 ORIG 0
    //   ⇒ 原码写 `outer->` 非 `(*outer).`（P167 的 it-> 与 (*it) 判据）。
    if (inner == innerMap.end())
        return false;
    // 2026-09-10（FL2CUser p167 迭代器形）：差集 = MISS `_ZNKSt23_Rb_tree_const_iteratorI
    //   St4pairIKjSt6vectorIN20InstanceRentalSystem10RentalInfoESaIS4_EEEEdeEv` ×1 +
    //   EXTR `…ptEv` ×1（内层）⇒ 原码对**内层**写 `(*inner).second`（de），对
    //   **外层**仍 `outer->second`（其 2 个 ptEv 两侧相抵）。
    return !(*inner).second.empty();
}

// ORIG W 0x08230172 sz0x3b：guildkey!=0 且当前角色==m_guildDB+0x17（master）。
#include "CUser/IsGuildMaster.inc"
{
    // ORIG 0x8230183-0x82301a6：`call;test %eax,%eax;je` + `cmp %edx,%eax;jne` 双跳
    // 共享 mov $0 尾岛。4.4 对"call 结果寄存器直比"恒物化 setne/sete（P197④），
    // 嵌套 if / `==0` / goto 形均反产 sete；只有把短路 && 直接作为 bool 返回值
    // （return a && b）时，两个条件分支才直落 0/1 常量岛 ⇒ 与 ORIG 全等。
    return get_charac_guildkey() && (getCurCharacNo() == m_guildDB.m_masterNo);
}

// ORIG T 0x0866c414 sz0x55：m_guildDB+0x2e 起 5 个 int 副会长表。
#include "CUser/IsSubGuildMaster.inc"
{
    // ORIG 0x866c424-0x866c446：表按 pack(1) 的 `int m_subMasterData[5]`（+0x2e 非对齐）
    // 建模后，4.4 与 4.1 同形折成 `add $0x23410,%edx; mov 0xe(%eax,%edx,4),%ebx`
    // （成员基址 0x8d040=4*0x23410 折进 scaled 变址、余 0xe 作 disp；probe s3 实证）；
    // 之前把头里建成 char[0x14]+reinterpret_cast ⇒ 基址 0x8d04e 不可 4 整除，4.4 退化成
    // base+shl 双步。技巧 22 register bool 物化 cmp;sete;test;je 命中。
    for (int i = 0; i < 5; ++i)
    {
        // b28d-solo-CUser AE: ORIG `cmp; sete %al; test %al,%al; je` 无槽；具名 hit 落 -0x9
        // 并把 i 槽推 -0xc→-0x10 ⇒ 内联条件。
        // b28d-solo-CUser：getCurCharacNo 已 unsigned（CUserCharacInfo.h:701 Main P211 实验）
        //   而 m_subMasterData 为 int ⇒ 异型多一条 `mov %eax,%ebx` 中转；ORIG 0x0866c414
        //   单段直落 ebx（同型）⇒ 调用点收窄回 int。
        if (m_guildDB.m_subMasterData[i] == (int)getCurCharacNo())
            return true;
    }
    return false;
}

// ---------------- U1 批次第二段（deleteSpecific 域 / 杂项发送域）----------------

// ORIG T 0x0867cf0c sz0x81：找 pair.first==itemIdx，返回 second；未找到 -1。
#include "CUser/_isMatchedItem.inc"
                          const std::vector<std::pair<int, int> >& list)
{
    // ORIG 0x867cf0c：prologue 单槽 it=-0xc（begin 缓存），循环 cond 无 end 重构
    //（技巧 25 具名缓存；声明序 begin 后占近槽）。
    std::vector<std::pair<int, int> >::const_iterator it = list.begin();
    std::vector<std::pair<int, int> >::const_iterator itend = list.end();
    for (; it != itend; ++it)
    {
        if (it->first == itemIdx)
            return it->second;
    }
    return -1;
}

// ORIG T 0x0867d0be sz0x330：[begin,end] 槽位超时/匹配/无效物品删除；
// 无效物品入 mailDel，最后邮件补偿。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d0be | _ZN5CUser24_deleteSpecificItemInvenE10INVEN_TYPERKSt6vectorISt4pairIiiESaIS3_EERS5_ii
#include "CUser/_deleteSpecificItemInven.inc"
                                     const std::vector<std::pair<int, int> >& list,
                                     std::vector<std::pair<int, int> >& out,
                                     int begin, int end)
{
    // 2026-09-10（G3CUser-B 复验）：槽集对照 ORIG/我方 各 19 槽、零平移零多余
    //   （228:226 insn）⇒ 具名局部一一对应，残余属跨版本 codegen（N 族），判 N。
    // b28d-solo-CUser p167：ORIG 0x0867d0be 调用 `getCurCharacInvenR() const`
    //   （MISSING ×1；我方 getCurCharacInvenW EXTRA ×1）⇒ 改 R 版 + 去 const cast。
    CInventory* inven = (CInventory*)getCurCharacInvenR();
    int matched = -1;
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = begin; slot <= end; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(type, slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                // b28d-solo-CUser p167 修正：ORIG 实例化 make_pair<unsigned long&, int&>
                //   （左值引用版 IRmRiE）；值 cast 产右值版 ImRiE ⇒ 用引用 cast 保左值。
                mailDel.push_back(std::make_pair((unsigned long&)item.m_addInfo,
                                                 item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            enableSaveInven();
            if (matched == 0)
                inven->delete_item(type, slot, item.m_addInfo2,
                                   (eItemDelReason)0x13, true);
            else
                inven->delete_item(type, slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13, true);

            if (type == (INVEN_TYPE)0)
                // b28d-solo-CUser p167：ORIG 实例化 `make_pair<ENUM_ITEMSPACE, int&>`。
                out.push_back(std::make_pair((ENUM_ITEMSPACE)3, slot));
            else if (type == (INVEN_TYPE)1)
                out.push_back(std::make_pair((ENUM_ITEMSPACE)0, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Inven", mailDel);
}

// ORIG T 0x0867d3ee sz0x2ae：同型逻辑作用于 CCargo 槽位 0..0x97。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d3ee | _ZN5CUser24_deleteSpecificItemCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
#include "CUser/_deleteSpecificItemCargo.inc"
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    int matched = -1;
    CCargo* cargo = getCurCharacCargoW();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = 0; slot < 0x98; ++slot)
    {
        Inven_Item item = cargo->get_cargo_slot(slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            // C05-④/P225 实测（2026-09-10，ORIG 原始汇编取证 0x0867d497：
            //   `cmpl $0xffffffff,-0x14(%ebp); setne %al; test %al,%al; je <else 块>`）：
            //   `while (matched != -1){…;break;}` + 拆 `if (matched == -1)` 两形实测
            //   nd 2→2 不变、ours 189→193（+4 条），未复现 setne 段 ⇒ 回退。
            //   该 setne 物化属 4.1 对「槽比常量」的发射（P197④ 镜像族），4.4 恒折直跳 ⇒ N。
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                // b28d-solo-CUser p167 修正：同 _deleteSpecificItemInven
                //   （ORIG make_pair<unsigned long&, int&>）。
                mailDel.push_back(std::make_pair((unsigned long&)item.m_addInfo,
                                                 item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->delete_item(slot, item.m_addInfo2, (eItemDelReason)0x13);
            else
                cargo->delete_item(slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13);
            // b28d-solo-CUser p167：ORIG 实例化 `make_pair<ENUM_ITEMSPACE, int&>`。
            out.push_back(std::make_pair((ENUM_ITEMSPACE)2, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Cargo", mailDel);
}

// ORIG T 0x0867d69c sz0x1d2：GetCharacExpandData(6) 兑换列表遍历删除，
// 超时或命中即 erase + alter() 标脏。m_redeemList 私有，按 ORIG 布局经 +0x08 访问。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d69c | _ZN5CUser25_deleteSpecificRedeemItemERKSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/_deleteSpecificRedeemItem.inc"
    const std::vector<std::pair<int, int> >& list)
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
        return;

    typedef std::list<Redeem_Item::Inven_Item_Expand_RedeemInfo> RedeemList;
    // ORIG 0x0867d6c5..0x0867d716 逐段：begin(it=-0x1c) → end(itEnd=-0x20) →
    //   iterator 临时 C1Ev(-0x24) → Inven_Item item C1(-0x61，**循环外**) →
    //   movb $0,-0xd（死 bool 预清）→ count=0(-0xc)。全程重算 redeem+0x8，无 items 引用。
    RedeemList::iterator it = redeem->m_redeemList.begin();
    RedeemList::iterator itEnd = redeem->m_redeemList.end();
    // ORIG 0x0867d6f5 `lea -0x24; call _List_iterator::C1Ev` 默认构造后
    //   0x0867d7f3/0x0867d7f6 `mov -0x6c,%eax; mov %eax,-0x24` 赋后置++结果
    //   ⇒ 具名 next（P50 先声明后赋值），erase 传 next 而非 it++ 临时。
    RedeemList::iterator next;
    Inven_Item item;
    bool removed = false;   // ORIG 0x0867d70b movb $0,-0xd（原码遗留，未被读）
    // ORIG 0x0867d833 `mov -0xc,%eax; cmp $0x14,%eax; ja` 两段寄存器形 ⇒
    //   int 局部 + 显式 unsigned cast（P310 翻案形）。
    int count = 0;
    while (it != itEnd && (unsigned int)count < 0x15)
    {
        // ORIG 0x0867d71b..0x0867d783：`it->m_item` 经 operator->（ptEv）取 Inven_Item
        //   子对象后**结构赋值**（4.1 内联 15×mov + 末字节，无 memcpy call）。
        item = it->m_item;
        // ORIG 0x0867d78c：`!item.isEmpty()`（call Inven_Item::isEmpty）；isEmpty 为真时
        //   **不递增迭代器**（仅 ++count 后重判），无我方原先推断的 else ++it。
        // ORIG 0x0867d793 `test %al,%al; jne 0x196(nop)` 正极性跳过（无 xor）；
        //   if/else 两分支尾各 `jmp 0x197` + 0x196 nop ⇒ P223 连续尾标签形。
        if (item.isEmpty())
            goto skipItem;
        {
            // ORIG 0x0867d799..0x0867d7d4：|| 短路物化（mov $1/$0 + test），无 remove 槽。
            if (_checkTimeoutItem(item) ||
                (_isMatchedItem(item.m_addInfo, list) != -1))
            {
                next = it++;
                redeem->m_redeemList.erase(next);
                redeem->alter();
            }
            else
            {
                ++it;
            }
        }
        goto afterItem;
        skipItem:;
        afterItem:;
        ++count;
    }
}

// ORIG T 0x0867d8a4 sz0x2c1：账号金库（须 IsExistAccountCargo），槽位数=GetCapacity()。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d8a4 | _ZN5CUser31_deleteSpecificItemAccountCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
// [b28d-GrpD 判词·等价 N] ORIG 0x0867d8a4：实差异仅 ① ORIG `cmp $-1,matched; setne;
//   test; je`（bool 物化）vs 我方裸 `je`（判据⑦等价）；② GetCapacity 比较的寄存器
//   中转（`cmp %edx,%eax` vs `cmp %eax,%edx`，同减式）；③ 未解析重定位地址。
//   字段偏移（item+0x2/+0x7、slot -0x50、matched -0x14）与 make_pair<unsigned long&,int&>
//   /make_pair<ENUM_ITEMSPACE,unsigned&> 两处实例化全一致 ⇒ 无语义缺口。
#include "CUser/_deleteSpecificItemAccountCargo.inc"
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    int matched = -1;
    if (!IsExistAccountCargo())
        return;

    CAccountCargo* cargo = GetAccountCargo();
    std::vector<std::pair<int, int> > mailDel;

    // 2026-09-10（B 轴反向 · AxisB 表 b_U->S）：ORIG 0x0867daa5 为 `seta`（无符号序）
    //   而我方 `setg`（有符号）⇒ ORIG 该比较为无符号 ⇒ 索引局部改 unsigned 对齐
    //   （P196 前置：ORIG 本身即无符号序）；`reinterpret_cast<unsigned int&>(slot)`
    //   保持（p167 判据②的引用形实例化不变）。
    for (unsigned int slot = 0; slot < (unsigned int)cargo->GetCapacity(); ++slot)
    {
        Inven_Item item = cargo->GetSlot(slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;   // P196 复核：ORIG 0x0867d9xx 该处为 `cmp -0x14(%ebp),%eax;
                                  //   jle`（有符号，两侧一致）⇒ 维持 int 比较；同函数另一处
                                  //   `seta`（0x0867da1x）才是无符号语义，非本比较点。
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                // p167 判据②：ORIG 实例化 make_pair<unsigned long&, int&>（IRmRiE），
                //   实参即 item+0x02（m_addInfo）左值 ⇒ ORIG 源码以 unsigned long 视图
                //   取该 4 字节槽。reinterpret_cast 引用零开销（32 位同宽）。
                mailDel.push_back(std::make_pair(
                    reinterpret_cast<unsigned long&>(item.m_addInfo),
                    item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->DeleteItem(slot, item.m_addInfo2);
            else
                cargo->DeleteItem(slot, item.m_addInfo2 - matched);
            // p167 判据②：ORIG 实例化 make_pair<ENUM_ITEMSPACE, unsigned int&>
            //   （I14ENUM_ITEMSPACERjE），第二实参为 -0x50 槽左值。
            out.push_back(std::make_pair((ENUM_ITEMSPACE)0xc,
                                         reinterpret_cast<unsigned int&>(slot)));
        }
    }

    RewardItem2DeleteInvalidItem("AccountCargo", mailDel);
}

// ORIG T 0x0867db66 sz0x39：转发给 GetCharacExpandData(0xe)（CTitleBook）。
#include "CUser/_deleteSpecificItemTitieBook.inc"
    const std::vector<std::pair<int, int> >& list)
{
    CTitleBook* titleBook =
        (CTitleBook*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (titleBook != 0)
    {
        titleBook->deleteTimeoutItem(list);
    }
    return;
}

// ORIG T 0x086928c2 sz0x145：金币兑换。itemIdx<=0x28d291 → (idx-0x28d287)*10^6；
// 否则 idx*10^7-0x549a800。bReset 只做上限校验（err=10 返回 false）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086928c2 | _ZN5CUser13DoGetGoldItemEiRSt6vectorISt4pairIiiESaIS2_EEbR10ENUM_ERROR
#include "CUser/DoGetGoldItem.inc"
                          bool bReset, ENUM_ERROR& err)
{
    int amount = 0;
    if (itemIdx <= 0x28d291)
        amount = (itemIdx - 0x28d287) * 1000000;
    else
        amount = itemIdx * 10000000 - 0x549a800;

    if (bReset)
    {
        // ORIG 0x0869290a/-0x048 预清 `movl $0x7fffffff,-0xc` 后覆盖 ⇒ 两句式（P198①）
        int limit = 0x7fffffff;
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());
        // ORIG 0x0869294e..0x098：`mov -0xc,%edx; mov %edx,%ecx; sub %eax,%ecx;
        // mov %ecx,%eax; cmp -0x10(%ebp),%eax; setl %al` ⇒ 有符号 `limit - money < amount`
        // （轴 B：ORIG setl ⇒ 去掉 unsigned cast，勿写 money+amount>limit 的无符号形）
        if (limit - getCurCharacMoney() < amount)
        {
            err = (ENUM_ERROR)0xa;
            return false;
        }
    }
    else
    {
        // ORIG 0x08692978..0x086929a5：getCurCharacInvenW 结果留 %eax 直跨到 gain_money
        // （无具名指针槽）⇒ P183 不缓存 receiver 指针
        getCurCharacInvenW()->gain_money(amount, (eMoneyAddReason)0xe, true, 0);
        // b28d-solo-CUser p167：ORIG 0x086928c2 实例化
        // `make_pair<ENUM_ITEMSPACE, Inven_Item::INVEN_SLOT>`（我方两 int）。
        out.push_back(std::make_pair(ENUM_ITEMSPACE_0, (Inven_Item::INVEN_SLOT)0));
    }
    return true;
}

// ORIG T 0x08692a34 sz0xc2：!bReset 时经 s_pIPGHelper 虚调用 vtable+0x0c/+0x2c；
// WongWork::IPG::CIPGHelper 仅最小建模（仅 dtor 虚函数），虚接口待补后恢复真实调用。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08692a34 | _ZN5CUser18DoGetCeraPointItemEjbR10ENUM_ERRORm
#include "CUser/DoGetCeraPointItem.inc"
                               ENUM_ERROR& err, unsigned long itemIdx)
{
    // b28d-PoolCUser 重写（2026-09-09，P171/P133/P17）：ORIG 0x08692a34 **无判空**
    //   （`mov s_pIPGHelper,%eax` 直进 vtable）⇒ 删 `!= 0` 防御（原为 EXTRA 判空 +
    //   三目汇聚致 call *%ecx 物理点数 1≠2）；两分支**各自完整发射** IPGInputPoint
    //   （3/1 立即数分叉）；尾 call 为 vptr+0x2c = _ZTV word13 = **slot11 IPGQuery
    //   (CUser*)**（P282 槽序还原；旧注释 IPGOutputConfirm 误读），bReset 真值走
    //   P222 正向包裹（!bReset 包体），假路共享尾 `mov $1`。
    (void)err;
    if (!bReset)
    {
        if (itemIdx == 0x28ec3d)
            GlobalData::s_pIPGHelper->IPGInputPoint(this, money, 3, 0, 0);
        else
            GlobalData::s_pIPGHelper->IPGInputPoint(this, money, 1, 0, 0);
        GlobalData::s_pIPGHelper->IPGQuery(this);
    }
    return true;
}

// ORIG T 0x0868c3bc sz0xa2（返回 1 已删 / 0 事件空或非活跃）：
// OnTimeEvent(0x33) 活跃时删除奖励用户。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868c3bc | _ZN5CUser25delete_ontime_reward_listEv
// [b28d-GrpD 判词·N] ORIG 0x0868c3bc：残差仅 ① 我方多 2 条对齐 nop（§6.1 豁免）；
//   ② 虚调用实参 store 次序（ORIG `mov -0xc(%ebp),%eax; movl $0,0x4(%esp); mov %eax,(%esp)`
//   vs 我方先 store 常量再取 this）—— 同址同值等价；③ 未解析重定位 call 标注。
//   控制流（ev==0→trace/0；!IsEventing→0；else delete+1）与返回常量全一致。
#include "CUser/delete_ontime_reward_list.inc"
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // b28d-PoolCUser 重写（2026-09-09）：ORIG 0x0868c3bc 物理块序 = [main: xor;test;je
    //   DEL] [mov$0;jmp end] [TRACE 块 +0x4c] [DEL 块 +0x89 fall-through 尾]。
    //   IsEventing 真 → je 直达尾 DEL 块（return 1 fall 到 leave）；假 → 落 mov$0 岛。
    //   ⇒ 用 goto DEL 把 delete 块物理后置（P223 尾标签系）。
    //   注：改顺序 if 结构（TRACE 内联 if 体）实测反劣 5→27 行，已回退。
    if (ev == 0)
        goto TRACE;
    if (!ev->IsEventing(0))
        return 0;
    goto DEL;
TRACE:
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa828, 5)
            ("[ON_TIME_EVENT]delete_ontime_reward_list() error");
        return 0;
    }
DEL:
    ((COnTimeEvent*)ev)->delete_reward_user(this);
    return 1;
}

// ORIG T 0x086924aa sz0x119：job==9 才发包；去重 combo 技能 → 包 0x1bd。
#include "CUser/sendComboSkillList.inc"
{
    if (get_charac_job() != 9)
        return;
    ((SkillSlot*)getCurCharacSkillW())->eraseDuplicationComboSKill();
    PacketGuard packet;
    packet.put_header(0, 0x1bd);
    packet.put_byte(isComboSkillSet() ? 1 : 0);
    packet.put_byte(2);
    ((const SkillSlot*)getCurCharacSkillR())->getComboSkillSendInfo(packet);
    packet.finalize(true);
    Send(packet);
    setComboSkillSet(false);
}

// ORIG T 0x08656808 sz0xbb：发送当前金币（MakeItemPacket(1,1) → 0xe 包）。
#include "CUser/SendCoin.inc"
{
    if (getCurCharacR() == 0)
        return;
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    // ORIG 0x865689e xor $0x1,%eax; 0x86568a3 jne cleanup：bool 返回取反 + 空 if（probe 实证 f1 形）
    if (!getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 1, packet))
    {
    }
    else
    {
        packet.finalize(true);
        Send(packet);
    }
}

// ORIG T 0x086504d8 sz0x11d：频道类型 !=7 时经 BigStreamPool 构造
// header(0x23)+UID 内部流，GetInBuffer<SIG_SAVE_INVENTORY> 后 _GetSaveInvenBase
// 填充、+0x08 字节置 1，投递 DB_Q。反编译见 CUser/SaveMoneyCoin.md。
#include "CUser/SaveMoneyCoin.inc"
{
    // P185 guard 作用域块形（ORIG 0x086504d8：guard@-0x14、put 后 `mov $1,%ebx`
    // 真值先于 dtor、epilogue 统一 movzbl %bl——旧平铺形 dtor 岛位置不同）。
    // ==7 走 sete 物化岛（技巧 22）；uid 跨 operator- 存活 %ebx。
    // ORIG 0x86504d8 **ebx 聚合单出口**：假支 `mov $0,%ebx; jmp END`、真支 put 后
    // `mov $1,%ebx`，END epilogue `movzbl %bl,%eax`；uid 跨 operator* 存活同用 %ebx
    // （时序不重叠：uid 在 put 前消费完）。
    register bool rc;   // b28d-solo-CUser：ORIG rc 驻 %ebx（mov $0/%1,%ebx + 尾 movzbl %bl）
    // b28d-solo-CUser：ORIG `sete %al; test %al,%al; je` 无 ch7 槽（P292 非 bool 物化）
    //   ⇒ 内联条件（probe_bool a3）。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        rc = false;
        goto RCEND;
    }
    {
        CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x18cc),
                           true);
        guard.operator*()->operator<<(0x23);
        register int uid;   // b28d-solo-CUser：ORIG `mov %eax,%ebx` 跨 operator* 保活
        uid = (int)GetUID();
        guard.operator*()->operator<<(uid);
        // ORIG 0x8650574-582：取 buffer 前先经一次 operator->（pt）call——源形
        // `guard->GetInBuffer<...>()`（智能指针循环解引用，pt 返回值作 this）。
        SIG_SAVE_INVENTORY* sig =
            ((CStreamGuard*)(void*)guard.operator->())
                ->GetInBuffer<SIG_SAVE_INVENTORY>();
        _GetSaveInvenBase(sig);
        sig->m_saveFlag = 1;            // ORIG movb $0x1,0x8(%eax)（+0x08 标志）
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        rc = true;
    }
RCEND:
    return rc;
}

// ORIG T 0x08690e36 sz0x82：事件邮件提醒 tooltip。空包 put_header(0,0x1ad) +
// finalize(true) 后直接 Send（docs/class_func_reports/CUser/SendEventMailAlaramTooltip.md）。
#include "CUser/SendEventMailAlaramTooltip.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x1ad);
    packet.finalize(true);
    Send(packet);
}

// ORIG T 0x0869154a sz0x664：抽奖计数道具发放（事件币/道具/邮件补偿）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0869154a | _ZN5CUser20giveLotteryCountItemEib
#include "CUser/giveLotteryCountItem.inc"
{
    // 2026-09-10（G3CUser-B 复验）：ORIG 0x086915fa `cmpl $0x0,-0x38(%ebp); je`
    //   （item==0 返回）与 0x08691623 `xor $1; test %al,%al; jne`（!verifyStackable…
    //   返回）为**两条独立守卫且各有 ORIG 直证** ⇒ 我方同形，不可删；
    //   本 .inc 具名量（item/src/i/it/slot/msg/str/rewards/packet）均有 ORIG 槽对应，
    //   残余属跨版本 codegen（N 族），判 N。
    if (getCurCharacR() == 0)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0xb45f,
            "giveLotteryCountItem curchar = null id(%s) item_no(%u) match(%d)",
            NumberToString(get_acc_id(), 0), itemNo, match ? 1 : 0);
        return;
    }

    CStackableItem* item =
        (CStackableItem*)G_CDataManager()->find_item(itemNo);
    if (item == 0)
        return;
    if (!verifyStackableItemProcess((StackableAction::Type)0x36, item))
        return;

    std::vector<Inven_Item> rewards;
    // ORIG 0x086916??：m_vec3a0 以 **non-const** vector<int>& 访问
    //   （call _ZNSt6vectorIiSaIiEEixEj，非 const 重载）。
    std::vector<int>& src = item->m_vec3a0;   // ORIG +0x3a0
    // 2026-09-10（C05② 类型旋钮）：ORIG 0x0869196a 为 `cmp -0x30(%ebp),%eax; setg`
    //   （内存直比 + 有符号）⇒ 索引局部必须是有符号 int（size_t 会让 4.4 先载寄存器
    //   并退化为 setb 无符号序）。
    for (int i = 0; i < (int)src.size(); )
    {
        Inven_Item it;                              // 默认构造后按 ORIG 填 8 字节
        // 2026-09-11（Lane2，字段错位轴修正 + P183 去具名局部）：ORIG 0x08691672/
        //   0x08691696 把两次 `operator[]` 结果直接 store 到 -0x98/-0x93
        //   （it 基址 -0x9a ⇒ **+0x02 / +0x07** = Inven_Item::m_addInfo@+0x02 /
        //   m_addInfo2@+0x07，Inven_Item.h:269/280），无具名 int 中间量。
        //   我方旧形写 it+0/+4 ⇒ (a) 两字段全错位（写到 m_field0 与 m_addInfo 高位，
        //   读回的是 C1Ev 初值 ⇒ 事件币分支与邮件分支取值全错 = 真语义 bug）；
        //   (b) 多占 itemNo/itemCnt 两槽（帧 0x20c vs ORIG 0x1fc）+ 4 条 store/load。
        it.m_addInfo = (unsigned int)src[i++];
        it.m_addInfo2 = src[i++];
        rewards.push_back(it);

        // ORIG 0x086916b1 `mov -0x98(%ebp),%eax; cmp $0x1,%eax; jne`
        //   + 0x086916c0 `mov -0x93(%ebp),%eax; test %eax,%eax; js`。
        if ((int)it.m_addInfo == 1 && it.m_addInfo2 >= 0)   // 事件币分支
        {
            // ORIG 0x086916d4..0x08691715：GetEventCoin() **独立调用两次**
            //（Set 实参一次、EventCoinAdd 实参一次；O0 无 CSE，P48）——
            // 旧版缓存 now 局部少一次调用（p167 miss=2）。
            // 实参直读 it.m_addInfo2（ORIG 0x086916e1 `mov -0x93(%ebp),%edx; lea (%eax,%edx,1),%ebx`）。
            getCurCharacInvenW()->SetEventCoin(
                getCurCharacInvenR()->GetEventCoin() + (unsigned int)it.m_addInfo2);
            m_historyLog.EventCoinAdd(
                getCurCharacInvenR()->GetEventCoin(), it.m_addInfo2, (eCoinAddReason)2);
            SaveMoneyCoin();
            SendCoin();
            continue;
        }

        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, it, it.m_addInfo2);

        // ORIG 0x08691782 `mov -0x98(%ebp),%eax; test; jne`（m_addInfo!=0 短路）
        //   → 0x0869178c `lea -0x9a(%ebp),%eax; call get_add_info`。
        if (!(it.m_addInfo != 0 || it.get_add_info() != 0))
            continue;

        int slot = getCurCharacInvenW()->insertItemIntoInventory(
            it, (eItemAddReason)0xd, true, true);
        if (slot < 0)
        {
            char msg[256];
            const char* str = g_scriptStringManager_.findString(
                4, "game_server_msg_104", (bool*)0);
            strncpy(msg, str ? str : "", 0xff);
            msg[0xff] = '\0';
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                getCurCharacName(), it, 0, (unsigned int)getCurCharacNo(),
                msg, (int)strlen(msg), 0, GetServerGroup(), false, false);
        }
        else
        {
            SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1d0);
    packet.put_byte((int)rewards.size());
    // ORIG 0x086919??：BOOST_FOREACH 展开（contain/begin/end/deref/next/done +
    //   auto_any_base::cvb ×3；元素 Inven_Item& 直取 +2/+7 字段）。
    BOOST_FOREACH(Inven_Item& ri, rewards)
    {
        packet.put_int(ri.m_addInfo);     // +2
        packet.put_int(ri.m_addInfo2);    // +7
    }
    packet.finalize(true);
    Send(packet);
}

// ORIG T 0x0868f472 sz0x158：i=0..min(6,m_field796c8) 发 0x1e 字节 MoneyLogPacket；
// 最后 resetMoneyLog()。对象布局为全局 MoneyLogPacket（PacketHeader 10B +
// 5 int @+0xa..+0x1d；ORIG 字段偏移实证），ctor 写 0x2723/0x1e 头并清零。
#include "CUser/SendMoenyLog.inc"
{
    if (getCurCharacR() != 0)
    {
        for (int i = 0; i < 7 && i <= m_moneyLogIdx; ++i)
        {
            if (m_moneyLogs[i].m_money[0] == 0 &&
                m_moneyLogs[i].m_money[1] == 0)
                continue;
            MoneyLogPacket pkt;
            pkt.m_characNo = getCurCharacNo();
            pkt.m_money0   = m_moneyLogs[i].m_money[0];
            pkt.m_money1   = m_moneyLogs[i].m_money[1];
            pkt.m_accId    = get_acc_id();
            pkt.m_money2   = m_moneyLogs[i].m_money[2];
            // ORIG 0x868f556：&pkt 先求值存 %ebx（跨 GetServerGroup/GetServerProxy
            // 存活），proxy 调用结果不经具名槽，直接 SendPacket。
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(GetServerGroup())
                ->SendPacket((char*)&pkt, 0x1e);
        }
    }
    resetMoneyLog();
}

// ORIG T 0x0868152c sz0x1e0：PC 房 login 通知。填 stLogin（login[0]=3/acc 名/
// charac 名/ip/uid/accid）→ stLoginPacket 打包 → SendTcpPacket；随后
// ReadPacket 读回包（ORIG 以 pkt 自身为 this、reply 为 packet 源）。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0868152c | _ZN5CUser23SendLoginToPCRoomServerEv
// [H3CUser 2026-09-11 · Main 裁决] `nexon_packet.h` 的 `stLogin::m_accLen/m_charLen`
//   → `unsigned char` 的**头改动弃用**：本函数已用 `(unsigned char)` cast 等效复现
//   ORIG 0x08681611..0x08681636 的 `movzbl %al,%reg`（见函数体内注释），cast 形已达成
//   ORIG 形，改头收益为零且多一个跨 TU 风险面 ⇒ 维持现状、不再报该项。
#include "CUser/SendLoginToPCRoomServer.inc"
{
    // 2026-09-10 gA（A 组）：**实现体不在本 .inc**（本文件仅函数头 + login/reply
    //   声明 + memset(login)），主体在聚合器 CUser.cpp:3306-3330 ⇒ 本组写域外，未动。
    //   已定位的可落地差异（供 .cpp 属主/Main 使用；ORIG 0x0868152c，我方 136 vs 113）：
    //   ① `LP_LOGIN->m_type = 3` ORIG 0x08681556 为 `movb $0x3,-0x226(%ebp)`（数组直址
    //      写），我方生成 `lea -0x22e(%ebp),%eax; movb $0x3,(%eax)`；
    //   ② 具名 `const int ip = get_public_ip_address();` 多余——ORIG 0x086815dc
    //      `mov %eax,-0x25(%ebp)` 直接落 stLogin+0x201（我方才经 -0x20 槽再转写）；
    //   ③ printf 实参 ORIG 全为 `movzbl %al,%reg`（0x08681611..0x08681636）入
    //      callee-saved，我方多 `movsbl %al,%eax` + 栈槽（-0x144c/-0x1450/-0x1454）
    //      ⇒ 去掉 `(int)` cast 改传 `(unsigned char)`；
    //   ④ 帧 0x147c vs 0x146c、reply 起点 -0x43c(我方) vs -0x434(ORIG) 由 ①②③ 连带。
    // 2026-09-11（G2 类型轴）：ORIG 0x08681556 `movb $0x3,-0x226(%ebp)`、
    //   0x086815dc `mov %eax,-0x25(%ebp)`、0x086815e3/0x086815f1 `mov %eax,-0x20/-0x1c(%ebp)`
    //   全部是 **ebp 常量偏移直址**（无 `lea base,%R` 前置）⇒ 原码缓冲就是 stLogin 对象，
    //   不是 `char[0x20e]` + cast。改对象后我方 19 条 `lea -0x22e(%ebp),…` 消失，
    //   帧 0x147c→0x146c 由 ip 槽收敛补齐。stLogin 为 #pragma pack(1)、sizeof==0x20e
    //   （nexon_packet.h:27），与数组布局逐字节一致。
    nexon_packet::stLogin login;
    nexon_packet::stLogin reply;
    memset(&login, 0, sizeof(login));
    // b28d-lane5：ORIG 0x0868xxx 每次字段访问重发 `lea -0x226(%ebp)`（P183 地址
    // 重物化铁律）⇒ 删除 lp 指针缓存，全表达式展开。
    login.m_type = 3;
    strcpy(login.m_account, get_acc_name());
    login.m_accLen = (char)strlen(get_acc_name());
    strcpy(login.m_characName, getCurCharacName());
    login.m_charLen = (char)strlen(getCurCharacName());
    // ORIG 0x086815dc：`call get_public_ip_address; mov %eax,-0x25(%ebp)` 直落 stLogin+0x201
    // ⇒ 删具名 `const int ip`（我方原多 store/load 各 1 条 + lea 1 条）。
    login.m_ip = (unsigned int)get_public_ip_address();
    login.m_canTry = 0;
    login.m_userIdx = (unsigned int)GetUID();   // ORIG：直接写 stLogin+0x206（-0x20(%ebp)）
    login.m_accId = (unsigned int)get_acc_id(); // ORIG：直接写 stLogin+0x20a（-0x1c(%ebp)）
    printf(
        "SEND nexon_packet::stLogin login:%d AccLen:%d Acc:%s CharLen:%d Char:%d "
        // [L9·2026-09-11 §6.5 串盲区] ORIG 0x0868167d `mov [esp],0x8ceff8c` ⇒ 格式串 VA 0x8ceff8c
        //   = 文件偏移 0xca7f8c，即本串起始；.rodata 原字节以 `m_id:%d\x00` 结束（**无尾 \n**）。
        //   旧形多一个换行 ⇒ 与 ORIG 日志输出不一致（AE 盲区）。
        "Addr:%d canTry_:%d UserIdx:%d m_id:%d",
        // ORIG 0x08681611..0x08681636：char 字段实参一律 `movzbl %al,%reg`（零扩展）
        // ⇒ 加 (unsigned char) cast（我方原为 `movsbl %al,%eax` + 3 个中转栈槽）。
        (unsigned char)login.m_type, (unsigned char)login.m_accLen, login.m_account,
        (unsigned char)login.m_charLen, (int)login.m_characName,
        (int)login.m_ip, (unsigned char)login.m_canTry, (int)login.m_userIdx,
        (int)login.m_accId);

    nexon_packet::stLoginPacket pkt(login);
    // b28d-GrpD（P0/P183）：ORIG 0x086816a1 `call GetLength` → 0x086816c6 前
    //   `lea -0x1438(%ebp),%ecx; mov s_pcroom_proxy,%edx; mov %eax,0x8(%esp)`
    //   —— GetLength 返回值**无栈槽**直装载；我方 `int len` 具名局部多
    //   `mov %eax,-0x1c(%ebp)` + `mov -0x1c(%ebp),%ecx` 各 1 条 + 4B 帧
    //   （探针 /tmp/L9/D/p2.cpp 形 A vs 形 B 直证）⇒ 删局部内联实参。
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, pkt.GetLength());
    memset(&reply, 0, sizeof(reply));
    pkt.ReadPacket((const char*)&reply, reply);
}

// ORIG T 0x0868170c sz0x113：PC 房 logout 通知。stLoginPacket 打包 →
// GetLength → SendTcpPacket（nexon_packet 已建模，符号直调）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868170c | _ZN5CUser24SendLogoutToPCRoomServerEv
#include "CUser/SendLogoutToPCRoomServer.inc"
{
    // 2026-09-11（G4）：ORIG 0x0868170c 的 stLogin 基址 -0x216、末字段
    //   m_canTry 落 -0x11 ⇒ 结构实体为 0x20e 字节（nexon_packet::stLogin，
    //   packed 且 sizeof==0x20e）；我方 `char login[0x210]` 的强转指针写法使
    //   gcc 每次经 `lea -0x21c(%ebp),%ebx` 缓存基址（push %ebx + 4×lea），
    //   ORIG 全部为 ebp 相对直写。改结构实体 + 成员访问（P0/P183 正形）。
    nexon_packet::stLogin login;
    // b28d-lane5：同 SendLoginToPCRoomServer——ORIG 0x086817xx 逐字段重发
    // `lea -0x226(%ebp)`（P183），删 lp 指针缓存。
    // 2026-09-10（G3CUser-B 负结论）：本 .inc 只含 `char login[0x210]` 声明，函数的
    //   全部指令在**聚合器 CUser.cpp:3336+** 内联段。实测 diff（ORIG 0x0868170c vs
    //   ours 78 insn）：ORIG `sub $0x1238` 且**不 push %ebx**、每站点重发
    //   `lea -0x216(%ebp),%eax; add $0x2,%eax`；我方 `push %ebx; sub $0x1234` 并把
    //   `lea -0x21c(%ebp)` 缓存进 %ebx（P183 反例，每站点省 1 条 but 帧/寄存器形反）。
    //   ⇒ 修复点在 CUser.cpp 的宏段（`#define LP_LOGOUT` 后的 LP_LOGOUT->… 语句需
    //   改为每次 `((nexon_packet::stLogin*)(login))->…` 直写、不引入 `pLogin` 局部），
    //   非本 .inc 写域；已报 Main。
    login.m_type = 1;
    strcpy(login.m_account, get_acc_name());
    login.m_accLen = (char)strlen(get_acc_name());
    strcpy(login.m_account, get_acc_name());   // ORIG 重复一次（+0x049/+0x064 双 strcpy）
    login.m_accLen = (char)strlen(get_acc_name());
    strcpy(login.m_characName, getCurCharacName());
    login.m_charLen = (char)strlen(getCurCharacName());
    login.m_ip = (unsigned int)get_local_ip_address();
    login.m_canTry = 1;

    nexon_packet::stLoginPacket pkt(login);
    // ORIG 0x086817f0..0x08681818：GetLength 结果经 %eax 直传 0x8(%esp)，
    //   无 len 栈槽（P0 右值内联；我方旧写多 2 条 + 一个 -0xc 槽）。
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, pkt.GetLength());
}

// ORIG T 0x0864870e sz0xdc：LOGDB_Q(=4)：<<0xcc、<<-1、<<reason。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864870e | _ZN5CUser29SendClientCrashDownLogToLogDBEi
// b28d-solo-CUser：返回类型 void→int（CUser.h:3228 Main 代落）——ORIG 尾
//   `mov $0x1,%ebx; …; mov %ebx,%eax; ret` 返回值 1（P159 尾形判返回类型）。
#include "CUser/SendClientCrashDownLogToLogDB.inc"
{
    // b28d-lane5：ORIG 0x0864870e 无 Stream* 槽——Acquire 结果经 %eax 直传 ctor
    // （P0 右值内联；同 _UpdateUserInfo4Guild）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x8ac), true);
    guard.operator*()->operator<<(0xcc);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(reason);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    return 1;
}

// ORIG T 0x0867fd0a sz0x3bb：MCAP 扣减率 → hades 计数 → checkMaxHackCntOver 处置
// → HackDetected 历史记录。全部依赖已建模为真实 C++ 符号（§5.3 去 asm 桥）：
// CMCAPManager::getPointPerHackType/incPoint/getPoint、CHades::AddHackTypeCnt、
// CHackAnalyzer::setUserP/getServerHackCnt/getServerHackAccumulatedCnt、
// CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver、CUser::setHackUser、
// HackTypeInfo_t 真实 ctor（清零）、cUserHistoryLog::HackDetected。
// 结构 vs Ghidra C（onUpdateHackCount.md）逐段对齐，HackTypeInfo_t 字段语义
// （m_type1/m_type2→setHackUser a/c、m_value2/m_value3→type/param6）照抄。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867fd0a | _ZN5CUser17onUpdateHackCountEN8WongWork13ENUM_HACKTYPEEijj
#include "CUser/onUpdateHackCount.inc"
                              unsigned int a, unsigned int b)
{
    m_mcap.incPoint(
        WongWork::CMCAPManager::getPointPerHackType(type) * count);
    if (m_mcap.getPoint() > 1000)
        setStdDropRate(100 - (int)(m_mcap.getPoint() / 100));

    char ip[16];
    if (!isHackUserTimer())
    {
        getHades()->AddHackTypeCnt((unsigned long)type, (unsigned long)count,
                                   a, b);
        getHackAnalyzer()->setUserP(this);
        // ORIG：HackTypeInfo_t::HackTypeInfo_t 真实 ctor 清零（非 memset）。
        // 2026-09-10（G3CUser-B，声明顺序/槽位轴）：ORIG 0x0867fe0e
        //   `lea -0x28(%ebp),%eax; mov %eax,(%esp); call HackTypeInfo_t::HackTypeInfo_t()`
        //   （info 槽 **-0x28**）出现在 0x0867fe1e `call CNetwork::GetPeerIP2(ip=
        //   -0x38(%ebp), 0x10)` **之前**；O0 下先声明者得高地址槽 ⇒ 原码把 info
        //   写在 GetPeerIP2 之前（我方原顺序反置，info 落 -0x38、ip 落 -0x28，
        //   且 ctor 调用点被推到 GetPeerIP2 之后）。
        WongWork::HackTypeInfo_t info;
        m_network.GetPeerIP2(ip, 0x10);
        if (GlobalData::g_autoPunishRuleHackTypeMgr->checkMaxHackCntOver(
                type,
                (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
                getHackAnalyzer(), info,
                m_lookOuts1, m_ipCounters, m_lookOuts2, ip))
        {
            setHackUser((unsigned int)info.m_type1,
                        (unsigned int)getHackAnalyzer()->
                            getServerHackAccumulatedCnt(
                                (WongWork::ENUM_HACKTYPE)info.m_type1),
                        (unsigned int)info.m_type2,
                        (unsigned int)getHackAnalyzer()->
                            getServerHackAccumulatedCnt(
                                (WongWork::ENUM_HACKTYPE)info.m_type2),
                        info.m_value3, info.m_limitCnt);
        }
        if (info.m_type2 == 3)
        {
            WongWork::HackTypeInfo_t info2;
            if (GlobalData::g_autoPunishRuleHackTypeMgr->checkMaxHackCntOver(
                    type,
                    (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
                    getHackAnalyzer(), info2,
                    m_lookOuts1, m_ipCounters, m_lookOuts2, ip))
            {
                setHackUser((unsigned int)info2.m_type1,
                            (unsigned int)getHackAnalyzer()->
                                getServerHackAccumulatedCnt(
                                    (WongWork::ENUM_HACKTYPE)info2.m_type1),
                            (unsigned int)info2.m_type2,
                            (unsigned int)getHackAnalyzer()->
                                getServerHackAccumulatedCnt(
                                    (WongWork::ENUM_HACKTYPE)info2.m_type2),
                            info2.m_value3, info.m_limitCnt);
            }
        }
    }

    m_historyLog.HackDetected(
        (unsigned int)type, (unsigned int)count,
        (unsigned int)getHackAnalyzer()->getServerHackCnt(type),
        (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
        a, b);
}

// ORIG _ZN5CUser14setStdDropRateEi @ 0x0828715c (W)：
// *(int*)(this+0x79624) = rate，即 m_codeHackCheck(+0x795fc).m_field28。
#include "CUser/setStdDropRate.inc"
{
    m_postChecksumData.m_field28 = rate;
}

// ============================================================================
// CUser 连续/冷却道具链簇（ORIG T，单函数还原见各 .md）
// ============================================================================

// ORIG _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj @ 0x0865eb0c (T)
// cooltime 处理：CStackableItem+0x2d0(ms) 换算为秒累加当前秒，更新/插入
// CheckCoolTimeItem 所读的 cooltime 列表条目（GetCurCharacCoolTimeItemList）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865eb0c | _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj
#include "CUser/_processCoolTimeItem.inc"
                                 unsigned int curSec)
{
    // ORIG 0x0865eb15..0x0865eb35：`mov 0x2d0(%eax),%ecx; imul $0x10624dd3;
    //   sar $6; sar $0x1f; sub` ⇒ **有符号** /1000（写 unsigned 会产 shr/div）。
    unsigned int expireSec = item->m_int2d0 / 1000 + curSec;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    // ORIG 0x0865eb46 movb $0,-0x11 / 0x0865ebc6 movzbl;xor $1;test;je ⇒ bool 局部
    //   （4.4 对 char 的 !found 折 cmpb，N122 判据）。
    bool found = false;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        // ORIG 0x0865eb?? `mov (%eax),%eax; cmp 0x10(%ebp),%eax; sete` = mem 直比；
        // 冗余 (int) cast 反致 4.4 `mov 0x10(%ebp),%eax; cmp %eax,%edx`（P162/P41 族）。
        if ((*cool)[i].m_itemIdx == itemIdx)
        {
            // ORIG 0x0865eb83 `mov 0x4(%eax),%eax; mov %eax,-0xc` 缓存旧值再比
            //   （P0：具名局部，勿直写两次 operator[]）。
            unsigned int oldExpire = (*cool)[i].m_expireTime;
            // ORIG 0x0865eb7d `mov -0x1c,%eax; cmp -0xc,%eax; jbe` ⇒ 源码写
            //   `expireSec > oldExpire`（写 old < expire 会反装载序）。
            if (expireSec > oldExpire)
                (*cool)[i].m_expireTime = expireSec;
            found = true;
        }
    }
    if (!found)
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        cool->push_back(info);
    }
    UpdateCoolTimeItemInfo(curSec);
}

// ORIG _ZN5CUser18_processEffectItemEP14CStackableItemjj @ 0x0865ec0e (T)
// effect 处理：CStackableItem+0x2e0 第 0 字节非 1 → 报错；
// 否则读 +0x2dc(ms) → 秒并 insert timer(0x76)，更新/插入 effect 列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865ec0e | _ZN5CUser18_processEffectItemEP14CStackableItemjj
#include "CUser/_processEffectItem.inc"
                               unsigned int curSec)
{
    // P0/P183（b28d-G3C 复核 2026-09-10）：删具名 uchar tag 槽。ORIG 0x0865ec1d
    //   `movzbl 0x2e0(%eax),%eax; xor $0x1,%eax; test %al,%al; je <+0x58>`
    //   —— **无栈槽往返**（xor 在 %eax 全宽 = int 提升域，test 落 %al = 截断后判零）。
    //   我方原形多 3 条：`mov %al,-0x2d; xorb $1,-0x2d; cmpb $0,-0x2d`（独有 -0x2d 槽，
    //   并连带 StatChangeInfo 对象被顶到 -0x48，ORIG 在 -0x38）。改为内联表达式。
    if (!(unsigned char)(item->m_stat2dc.m_flagLo ^ 1))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x391e, 0)
            ("ERROR : Mismatch tag [stat change duration] and "
             "[effect maintenance]. There is no duration value.");
        return;
    }
    unsigned int durationSec = (unsigned int)item->m_stat2dc.sec / 1000;
    unsigned int expireSec = curSec + durationSec;
    CStackableItem::StatChangeInfo stat = item->GetStatChangeInfo();
    // 语义轴（b28d-G3C 2026-09-10）：ORIG 0x0865eca5 只读**一个**字节
    //   `movzbl -0x33(%ebp),%eax`（对象基址 -0x38 ⇒ 即 +0x5 = m_flagHi），
    //   我方原写 `m_flagLo != 0 || m_flagHi != 0` 多出 `movzbl -0x44; test; jne`
    //   两条（+0x4 的 m_flagLo）——ORIG 无此判断，删。
    if (stat.m_flagHi != 0)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                    (TIMER_MESSAGE)0x76, (int)durationSec,
                                    (int)itemIdx, (unsigned int)getCurCharacNo());
    }
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    bool found = false;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        if ((*effect)[i].m_itemIdx == (int)itemIdx)
        {
            if ((*effect)[i].m_expireTime < expireSec)
                (*effect)[i].m_expireTime = expireSec;
            found = true;
        }
    }
    if (! found)   // bool 值域聚合位，X2 免约束（判据①，ORIG 尾 testb %al 直判）
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        effect->push_back(info);
    }
    UpdateEffectItemInfo(curSec);
}

// ORIG _ZN5CUser22_processContinuousItemEjP14CStackableItem @ 0x0865ea62 (T)
#include "CUser/_processContinuousItem.inc"
{
    unsigned int curSec = (unsigned int)GlobalData::s_systemTime_.getCurSec();
    if (item != 0)
    {
        CStackableItem::CoolTimeOption coolOpt = item->GetCoolTimeOption();
        if (coolOpt.flag != 0)
            _processCoolTimeItem(item, itemIdx, curSec);
        CStackableItem::CoolTimeOption effOpt = item->GetEffectItemOption();
        if (effOpt.flag != 0)
            _processEffectItem(item, itemIdx, curSec);
    }
    _processSpeicalContinuousTimeItem(itemIdx);
}

// ORIG _ZN5CUser33_processSpeicalContinuousTimeItemEj @ 0x08659b60 (T)
// 特殊连续时间道具：查 item→is_stackable→IsExpAffect→GetExpAffectRatio>0
// 时 SetExpAffectItemEffect(true, ratio)。
// b28d-lane5 N封顶: N85 x87比较lowering | ORIG 0x08659b60 | _ZN5CUser33_processSpeicalContinuousTimeItemEj
// b28d-PoolCUser P230③ -march=i486 手工路由实验（2026-09-09，Main 批准，读数并列）：
//   (a) 现路由（4.4.6 后端，无 march）：本件 ndiff **5**（fucompp/fnstsw/sahf vs
//       fucomip/fstp 段）；整 TU identical=703/ae=1508/near=4/diff=115（合计 2211）。
//   (b) +`-march=i486 -mtune=generic`：本件 **AE 归零**；整 TU 703/1509/4/114
//       （合计 2212，净 +1，near/diff 零回退）。
//   (c) 仅 `-march=i486`（不带 -mtune）：整 TU **崩** 674/142/2/1512——默认 tune 改
//       调度，**不可单加**；b 形（带 -mtune=generic）为唯一可用组合（同 CCreatureMgr
//       先例 416/422/437 行记法）。⇒ 待 Main 落路由表。
#include "CUser/_processSpeicalContinuousTimeItem.inc"
{
    CItem* pItem = G_CDataManager()->find_item((int)itemIdx);
    if (pItem == 0)
        return;
    // ORIG 0x08659b60：is_stackable/IsExpAffect 均正向 `test %al,%al; je 共享尾
    // 出口`（无 xor/jne 反形）⇒ 源码正向嵌套 if（P59 反向）；ratio 槽 -0xc
    // fstps+flds 往返；x87 fucompp/fnstsw/sahf 段属 N85 跨版本豁免。
    if (pItem->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)pItem;
        if (stack->IsExpAffect())
        {
            float ratio = stack->GetExpAffectRatio();
            if (ratio > 0.0f)
                SetExpAffectItemEffect(true, ratio);
        }
    }
}

// ORIG _ZN5CUser17CheckCoolTimeItemEm @ 0x0865e994 (T)
// 当前角色 cooltime 列表中存在 itemIdx==param_1 且 expireTime>curSec 的条目。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865e994 | _ZNK5CUser17CheckCoolTimeItemEm
#include "CUser/CheckCoolTimeItem.inc"
{
    // ORIG 0x0865e9a6 `test %eax,%eax; setne %al; test %al,%al; je <尾 0xc4>，主体 fall-through`
    // ⇒ 正向包裹（P59/P222）；写 `if (f()==0) return false;` 会产 sete + 早退岛（多 1 条）。
    if (getCurCharacR() != 0)
    {
        const std::vector<ContinuousItemInfo>* cool =
            GetCurCharacCoolTimeItemListR();
        // 负结论（2026-09-10）：ORIG it@-0x10 / end@-0x14 与我方 it@-0x14 / end@-0x10
        //   互换；试「头部裸声明 endIt 先声明（先声明深）+ 先赋值后 for」(P212/P309)
        //   实测反劣（missing_in_game 588→590，尾段多 island），已回滚 ⇒ 维持本形，
        //   残差 = it/end 两级槽互换（N93 边界，无三级转换临时 ⇒ P326 不适用）。
        // b28d-lane5 翻案（probe_slots f1/f2，4.4.6 同路由）：仍缓存 end()，但具名序必须是
        //   **it 先声明、itEnd 后声明**（4.4 先声明更浅 ⇒ it -0x10 / itEnd -0x14 = ORIG
        //   0x0865e9c1/0x0865e9d6 槽址；前轮试的是 endIt 先声明 = 方向相反故反劣）。
        //   本函数容器在循环内只读（无 erase）⇒ 缓存 end() 语义安全。
        std::vector<ContinuousItemInfo>::const_iterator it = cool->begin();
        std::vector<ContinuousItemInfo>::const_iterator itEnd = cool->end();
        for (; it != itEnd; ++it)
        {
            // b28d-solo-CUser p167 修正：ORIG 0x0865e994 发 deEv（`(*it).`）非 ptEv。
            if ((unsigned long)(*it).m_itemIdx == itemIdx)
            {
                // ORIG 0x0865ea04 才发 getCurSec（命中后求值，%ebx 跨 operator* 保活，
                // **无具名 curSec 槽**）⇒ 内联表达式，勿提到循环外（P0/P183）。
                if ((unsigned int)GlobalData::s_systemTime_.getCurSec()
                    < (*it).m_expireTime)
                    return true;
                return false;
            }
        }
    }
    return false;
}

// ORIG _ZN5CUser22UpdateCoolTimeItemInfoEj @ 0x08658b2a (T)
// 组 SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO（0x100 占位：characNo@+0x0 +
// count@+0x4 + [0x194]条目簇）投递队列 2。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658b2a | _ZN5CUser22UpdateCoolTimeItemInfoEj
#include "CUser/UpdateCoolTimeItemInfo.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2cb8), true);
    guard.operator*()->operator<<((int)0x132);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    // b28d-solo-CUser p167 重写（ORIG 0x08658b2a 实形）：无 memset(sig)；用
    //   CONTINUOUS_ITEM_INFO 局部（C1 清零）+ 迭代器循环 + 末尾 memcpy(&sig->m_info)
    //   （MISSING：operator-> ×5 / CONTINUOUS_ITEM_INFO::C1 / operator++(int) / end() /
    //   operator!= / memcpy；EXTRA：operator[] ×4 / memset / size / operator+）。
    // ORIG：count 清零（-0x10）先于 getCurCharacNo 调用（语句序）。
    int count = 0;
    sig->m_characNo = (unsigned int)getCurCharacNo();
    CONTINUOUS_ITEM_INFO local;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    // ORIG：begin() 在 size() 检查**之前**求值（独立 sret 临时 -0x28）。
    std::vector<ContinuousItemInfo>::iterator it = cool->begin();
    // CuC P223 变体（ORIG 0x08658b2a 直证）：ORIG `cmp $0x32,%eax; seta %al; test %al,%al;
    //   je <循环头>; jmp <尾>` 是**裸早返双跳**形，不是 `if(...){…}` 单 je 形。
    //   4.4 对 `if (size() > 0x32) return;` 逐条复现该形（探针 /tmp/CuC/probe/p2.cpp f2：
    //   `cmpl $50; setg %al; testb %al,%al; je <body>; jmp <尾>`；`size()` 为无符号 ⇒ seta）。
    //   旧形 `if (size() < 0x33) {…}` 恒产 setbe 单 je（P197④ 镜像），已证不可翻。
    //   语义等价：size>0x32 时 return 只析构 guard，与旧形跳过整块后落尾一致。
    if (cool->size() > 0x32)   // 超过 0x32 条不发送
        return;
    for (; it != cool->end(); )
    {
        if (it->m_expireTime < expireTime
            || (unsigned int)it->m_itemIdx > 0x7ffffffe
            || it->m_expireTime > 2000000000u)
        {
            it = cool->erase(it);
        }
        else
        {
            local.m_entries[count].m_itemIdx = it->m_itemIdx;
            local.m_entries[count].m_expireTime = it->m_expireTime;
            ++count;
            it++;
        }
    }
    local.m_count = (unsigned char)count;
    memcpy(&sig->m_info, &local, 0x194);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG _ZN5CUser20UpdateEffectItemInfoEj @ 0x08658d90 (T)
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658d90 | _ZN5CUser20UpdateEffectItemInfoEj
// [b28d-GrpD 判词·等价 N] ORIG 0x08658d90：唯一实差异是 size 门槛的方向码
//   —— ORIG `cmp $0x32,%eax; seta %al; test; je <循环>` vs 我方 `setbe; je <退出>`
//   （判据⑦：cmp 同、落点体互换 = 等价）；其余为未解析重定位（"user.cpp"/memcpy@plt）
//   与槽号。字段偏移（-0x1b8/-0x1b4 = entries[i].item/expire）、begin 先于 size、
//   erase/end() 重调序全一致 ⇒ 无语义缺口。
#include "CUser/UpdateEffectItemInfo.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2ceb), true);
    guard.operator*()->operator<<((int)0x137);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    // b28d-solo-CUser p167 重写（ORIG 0x08658d90 实形，同 UpdateCoolTimeItemInfo）：
    //   无 memset(sig)；CONTINUOUS_ITEM_INFO 局部 + 迭代器循环 + 末尾 memcpy。
    // ORIG：同 UpdateCoolTimeItemInfo（count 清零先于 getCurCharacNo）。
    int count = 0;
    sig->m_characNo = (unsigned int)getCurCharacNo();
    CONTINUOUS_ITEM_INFO local;
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    // ORIG：begin() 在 size() 检查**之前**求值（独立 sret 临时 -0x28）。
    std::vector<ContinuousItemInfo>::iterator it = effect->begin();
    // CuC P223 变体（ORIG 0x08658d90 直证，同 UpdateCoolTimeItemInfo 0x08658b2a）：
    //   ORIG `cmp $0x32,%eax; seta %al; test %al,%al; je <循环头>; jmp <尾>` = 裸早返双跳形。
    //   4.4 对 `if (size() > 0x32) return;` 复现（探针 /tmp/CuC/probe/p2.cpp f2）；
    //   旧 `if (size() < 0x33) {…}` 恒产 setbe 单 je。语义等价（return 仅析构 guard）。
    if (effect->size() > 0x32)   // 超过 0x32 条不发送
        return;
    for (; it != effect->end(); )
    {
        if (it->m_expireTime < expireTime
            || (unsigned int)it->m_itemIdx > 0x7ffffffe
            || it->m_expireTime > 2000000000u)
        {
            it = effect->erase(it);
        }
        else
        {
            local.m_entries[count].m_itemIdx = it->m_itemIdx;
            local.m_entries[count].m_expireTime = it->m_expireTime;
            ++count;
            it++;
        }
    }
    local.m_count = (unsigned char)count;
    memcpy(&sig->m_info, &local, 0x194);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
//   @ 0x0868a782 (T)
// vec 长==2 时：元素加信息(+2 addInfo)==0x28be7b 即罐子；另一元素为产出道具。
// 组 Packet_Event_UpgradeJar_Output_Statistic_GTS(0x106e/0x12) 经 statistic
// proxy(GET_ENUM_SERVER_GROUP_NONE) 下发。罐子 id 为 0x28be7b，剩余逻辑：
//   local_e = 罐子在 vec 中的下标（0 或 1）；m_itemCode = vec[1-local_e].addInfo；
//   m_itemCount = vec[local_e].+7（m_addInfo2 罐子数量）。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0868a782 | _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
#include "CUser/SendEventJarOutputItemStatistics.inc"
    const std::vector<Inven_Item>& items)
{
    // b28d-GrpD 按 ORIG 0x0868a782 直证改写（4 处）：
    //  ① ORIG +0x07 `lea -0x1c(%ebp),%eax; call Packet_..._GTSC1Ev` 是首条语句
    //     ⇒ pkt 具名对象声明必须在 size 检查**之前**（我方旧版声明在中段）。
    //  ② ORIG +0x47 / +0x71 两处罐子判定均为 `cmp $0x28be7b,%eax; sete %al;
    //     test %al,%al; je <另一支>` ⇒ 源码是 `==` 正向两分支；我方旧 `!=`
    //     写法产 `setne/je` 反极性（P59/P222 分支布局反写）。
    //  ③ ORIG +0x96 `mov 0x7(%eax),%eax; mov %eax,-0xe(%ebp)`（pkt+0xe=m_itemCount）
    //     先于 +0xbd 的 `mov %eax,-0x12(%ebp)`（pkt+0xa=m_itemCode），且全程
    //     无 itemCode/itemCount 具名槽 ⇒ 直接赋成员、m_itemCount 先写（P183/P329）。
    //  ④ ORIG +0xd3 GetServerProxy 后**无判空**，eax 直接作 this 调 SendPacket
    //     （+0xd8 `movl $0x12,0x8(%esp)`）⇒ 删我方发明的防御判空（判据①）。
    Packet_Event_UpgradeJar_Output_Statistic_GTS pkt;
    if ((int)items.size() != 2)
        return;
    unsigned short jarIdx = 0;
    if (items[0].m_addInfo == 0x28be7b)
    {
        jarIdx = 0;
    }
    else
    {
        if (items[1].m_addInfo == 0x28be7b)
            jarIdx = 1;
        else
            return;
    }
    pkt.m_itemCount = items[(int)jarIdx].m_addInfo2;
    pkt.m_itemCode = items[1 - (int)jarIdx].m_addInfo;
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&pkt, 0x12);
}

// ============================================================================
// ORIG 0x08692b84 CUser::SendAradEventItemLog(unsigned long, int)：
// "Arad_StatisticsEventItemList" 脚本 needStatistics(itemIndex) 命中时，
// 构造 16 字节 Sig{itemIndex,count,accId,characNo(-1)} 经内部流 0x20 投递
// LOGDB_Q。反编译见 docs/class_func_reports/CUser/SendAradEventItemLog.md。
// ============================================================================

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08692b84 | _ZN5CUser20SendAradEventItemLogEmi
#include "CUser/SendAradEventItemLog.inc"
{
    // 2026-09-11（H3CUser，P183/P0）：ORIG 0x08692b84 无 mgr/script 具名槽——
    //   `call Singleton<Arad_DataManager>::Get` 结果 %eax 直接作 this 调
    //   findGameScript，其返回 %eax 又直接作 this 调 needStatistics（0x08692bc5
    //   `mov 0xc(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp)`）⇒ 两具名量
    //   内联（我方旧形 -0x10/-0xc 两对 store+load 共 +4 条）。
    // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x08692bc5 `mov 0xc(%ebp),%edx;
    //   mov %edx,0x4(%esp); mov %eax,(%esp); call needStatistics` —— %eax（findGameScript
    //   结果）**直接**当 this，无 `test %eax,%eax / je` 判空；随后 0x08692bd6
    //   `mov %eax,%ebx; xor $0x1,%ebx`（!needStatistics 物化）+ 0x08692c21
    //   `test %bl,%bl; jne 0x08692d22`（跳尾声返回）⇒ 删我方 `script == 0` 防御。
    if (!static_cast<ARAD::Arad_StatisticsEventItemList*>(
             ARAD::Singleton<ARAD::Arad_DataManager>::Get()
                 ->findGameScript("Arad_StatisticsEventItemList"))
             ->needStatistics(itemIndex))
    {
        return;
    }

    arad::SigAradEventItemLog sig;
    // 2026-09-11 CuB（P329/P0，装载序对接 ORIG 写入序）：ORIG 0x08692c34..0x08692c61
    //   的成员写入序 = +0x8(m_id: get_acc_id) → +0xc(m_characNo: get_charac_no(-1))
    //   → +0x0(m_itemId=itemIndex) → +0x4(m_itemCount=count)（两处 call 在前、
    //   两形参直写在后）；我方旧序（+0/+4 先）致整段对不上。语句重排，语义等价。
    sig.m_id = get_acc_id();
    sig.m_characNo = get_charac_no(-1);
    sig.m_itemId = itemIndex;
    sig.m_itemCount = count;

    // b28d-lane5：ORIG 0x0866xxx 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xb9c7), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, ARAD::K_ARAD_EVENT_ITEM_LOG, (int)GetUID());
    guard.operator->()->put_binary(&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ============================================================================
// 任务 QP 发送链（RestoreUserQuestStubs：Dispatcher_Debug_Clear_Quest 依赖）
// ============================================================================

// ORIG T _ZN5CUser12sendCharacQpEv @0x0868ac24：
// GetCharacExpandData(0xb)（CQuestShop）非空 → shop->sendCharacQp(this, 0)。
#include "CUser/sendCharacQp.inc"
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->sendCharacQp(this, 0);
    }
    return;
}

// ORIG T _ZN5CUser13incQuestPointEiib @0x0868ac66：
// GetCharacExpandData(0xb) 非空 → old=getQp()、addQp(qp)；!send →
// shop->sendCharacQp(this,0)；m_historyLog.QPAdd(old, questIdx, qp)。
#include "CUser/incQuestPoint.inc"
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        int before = shop->getQp();
        shop->addQp(qp);
        if (!send)
        {
            shop->sendCharacQp(this, 0);
        }
        m_historyLog.QPAdd(before, questIdx, qp);
    }
    return;
}

// ============================================================================
// ORIG T 0x0864d90a：从 m_list711ec（+0x711ec list<int>）删除 state。
// ============================================================================
#include "CUser/RemoveRestrictedGoods.inc"
{
    // ORIG 0x0864d90a：b/end/it 三迭代器各落槽 -0x10/-0x14/-0x18——end() 仅
    // call 一次（具名 e 局部缓存，it != e 复用）；find 结果槽 sret -0x18。
    std::list<int>::iterator b = m_list711ec.begin();
    std::list<int>::iterator e = m_list711ec.end();
    std::list<int>::iterator it = std::find(b, e, state);
    if (it != e)
        m_list711ec.erase(it);
}

// ============================================================================
// ORIG T 0x0867ceae：全部角色 m_needCheckOverEquipItem（+0xe3b）置 1。
// ============================================================================
#include "CUser/SetNeedCheckOverEquipItem.inc"
{
    // ORIG 0x08??：operator[] 结果存 -0xc 槽（i=-0x10 先声明远槽，技巧 21）：
    // `mov %eax,-0xc; cmpl $0,-0xc; je NEXT; mov -0xc,%eax; movb $1,0xe3b(%eax)`
    // ——单次 call + 具名引用中转（双 call 形反产 test;setne 物化+二次 sret）。
    for (unsigned int i = 0; i < m_characList.size(); ++i)
    {
        _Charac_info& r = m_characList[i];
        // P223（b28 BL9 实证形）：`if(c==e) goto L; body; L:;` 精确复现
        // ORIG `je 岛; body; jmp; nop` 空 then 占位岛。
        if (&r == 0)
            goto NEXT;
        r.m_needCheckOverEquipItem = 1;
    NEXT:;
    }
}
// ============================================================================
// LinkMisc5 批次（散点链接期未定义符号，第四阶）
// ============================================================================

// ---- CSecurityCard 状态 getter（ORIG W 0x0822ef0e/0x0822ef1a/0x0822ef26，
//      objdump 忠实转写；声明补于 CUser.h WongWork::CSecurityCard）----
#include "CUser/isActivate.inc"
{
    return m_activate2f;   // +0x2f 激活标志（bool 直返：ORIG movzbl 直返）
}

#include "CUser/isCertified.inc"
{
    return m_certified30;  // +0x30 认证标志（bool 直返：ORIG movzbl 直返）
}

#include "CUser/getFailCnt.inc"
{
    return m_failCnt34;         // +0x34 失败计数
}

// ============================================================================
// ORIG T 0x0868991c _ZN5CUser17isTradePunishTypeEi：
//   type==0 → m_tradePunishType(+0x79628)!=0；否则按位与命中。
// ============================================================================
#include "CUser/isTradePunishType.inc"
{
    if (type == 0 && m_postChecksumData.m_tradePunishType != 0)
        return 1;
    // ORIG 0x8689949 je 尾岛（0x8689952 mov $0）：命中 fall `mov $1`（probe12 B 反向布局）
    if (type & m_postChecksumData.m_tradePunishType)
        return 1;
    return 0;
}

// ============================================================================
// ORIG T 0x086624f8 sz 0x30 _ZN5CUser15send_RedeemInfoEv：
//   GetCharacExpandData(6) 取 Redeem_Item::CRedeemItem 后 Send_RedeemList。
// ============================================================================
#include "CUser/send_RedeemInfo.inc"
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)6);
    redeem->Send_RedeemList(this);
}

// ============================================================================
// ORIG T 0x08664ae8 sz 0x167 _ZN5CUser13calc_lev_statER15_Additioal_info：
//   基础块 add 后按当前等级循环叠加成长属性块（≤14 首转/≤49 二转/之后三转）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08664ae8 | _ZN5CUser13calc_lev_statER15_Additioal_info
#include "CUser/calc_lev_stat.inc"
{
    // ORIG：G_CDataManager() 在基础 add/get_growtype_stat×(3) 四处各自重调，
    // getCurCharFirstGrowType 重调两次——访问器结果禁缓存（战术 P183/P48）。
    info.add(*(const _Additioal_info*)
        &G_CDataManager()->m_characList->m_characs[get_charac_job()]);

    const void* statBase = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(0, 0);          // 无成长
    const void* statFirst = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()]
            .get_growtype_stat((signed char)getCurCharFirstGrowType(), 0);  // 一转
    // b28d-GrpD 按 ORIG 0x08664ae8 直证：ORIG 第三个 get_growtype_stat 的第二实参
    //   直接来自 `call getCurCharSecondGrowType; movsbl %al,%esi`（+0x0b7），全程
    //   **无 secondGrow 具名槽**（我方旧 `int secondGrow` 多占 -0x14 ⇒ 帧 0x30 vs
    //   ORIG 0x2c、且三个指针槽整体下移 4：我方 -0x1c/-0x18/-0x10 vs ORIG
    //   -0x28/-0x24/-0x20）。删除具名量、实参内联（P329/P183）。
    const void* statSecond = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(
            (signed char)getCurCharFirstGrowType(),
            (signed char)getCurCharSecondGrowType());                   // 二转

    for (int level = 1; get_charac_level() > level; ++level)
    {
        if (level <= 14)
            info.add(*(const _Additioal_info*)statBase);
        else if (level <= 49)
            info.add(*(const _Additioal_info*)statFirst);
        else
            info.add(*(const _Additioal_info*)statSecond);
    }
}

// ============================================================================
// ORIG T 0x0868ff04 sz 0x9c _ZN5CUser21CalcurateUserMaxLevelEv：
//   全角色 m_level 最大值 clamp 到 70 后 SetUserMaxLevel。
// ============================================================================
#include "CUser/CalcurateUserMaxLevel.inc"
{
    int maxLevel = 0;
    // ORIG 0x867f??：`cmp -0xc(%ebp),%eax; setg`（signed 比较，i 为 int）+
    // operator[] 实参 idx 先载 %eax、this 走 %edx mov+add（技巧 15 失配序）。
    for (int i = 0; i < (int)m_characList.size(); ++i)
    {
        if ((int)m_characList[i].m_level > maxLevel)
            maxLevel = m_characList[i].m_level;
    }
    if (maxLevel > 70)
        maxLevel = 70;
    SetUserMaxLevel(maxLevel);
}

// ============================================================================
// ORIG T 0x0868e704 sz 0x2bb _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard：
//   管理等级/点数 + 雇佣兵列表逐个序列化（剩余时间、状态、名字、三状态字节）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868e704 | _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard
#include "CUser/SetMercenaryInfoPacket.inc"
{
    // b28d-lane5 复测（判据⑩ 声明序 = 槽深序）：ORIG 0x0868e70b/0x0868e712 在 `sub $0x24`
    //   后立即两条 `movl $0x0,-0x18/-0x14`（循环变量置 0 **在函数头**）⇒ remainSec/nameLen
    //   须在函数头带初值声明（原形声明在循环内 ⇒ 少两槽、leveledUp 槽 -0xd 漂到 -0x15）。
    int remainSec = 0;
    int nameLen = 0;
    bool leveledUp = CalcurateManageLevel();
    if (leveledUp)
        SendManageLevelUpNoti();

    packet.put_byte(GetManageLevel());
    packet.put_int(GetManagePoint());
    packet.put_byte((signed char)m_mercenaryMgr.m_characs.size());

    for (unsigned int i = 0; i < m_mercenaryMgr.m_characs.size(); ++i)
    {
        // b28d-solo-CUser p167：ORIG 0x0868e704 每字段重调
        //   `vector<CHARAC_LOAD_MERCENARY>::operator[]`（共 9 次；我方缓存引用 merc
        //   仅 1 次，MISSING ×8）⇒ 去缓存逐字段访问（P157/P183）。
        // 剩余时间 = 到期秒 - 当前秒
        remainSec = m_mercenaryMgr.m_characs[i].m_field2c
                    - GlobalData::s_systemTime_.getCurSec();

        // 2026-09-10（P265 档③ 实测无效，已回退）：ORIG 0x0868e7b5 段该实参为
        //   `mov (%eax),%eax; mov %eax,%edx`（int 字段经异型转换的两段中转），
        //   实参位显式 `(unsigned int)` cast 实测**不产**该中转（仍单段 `mov (%eax),%edx`）
        //   ⇒ 该中转来自字段真型（enum）而非实参 cast；字段在共享头 SigTypes_C.h
        //   CHARAC_LOAD_MERCENARY::m_characNo（全仓多消费点）⇒ 按 S2 不自行改，
        //   记 N（P254 字段 enum 化旋钮待 Main 全链评估）。
        packet.put_int(m_mercenaryMgr.m_characs[i].m_characNo);

        if (&m_mercenaryMgr.m_characs[i].m_name[0] == 0)
            nameLen = 0;
        else
            nameLen = strlen(m_mercenaryMgr.m_characs[i].m_name);
        packet.put_int(nameLen);
        if (nameLen != 0)
            packet.put_str(m_mercenaryMgr.m_characs[i].m_name, nameLen);

        if (remainSec > 0)
            packet.put_byte(1);
        else if (m_mercenaryMgr.m_characs[i].m_field30 == 0xff)
            packet.put_byte(0);
        else
            packet.put_byte(2);

        packet.put_int(remainSec);
        packet.put_byte((signed char)m_mercenaryMgr.m_characs[i].m_field30);
        packet.put_byte((signed char)m_mercenaryMgr.m_characs[i].m_field31);
        packet.put_byte((signed char)m_mercenaryMgr.m_characs[i].m_avatarBonus);
    }
}
// ============================================================================
// ORIG T 0x0868fec8 _ZN5CUser14SetUserMaxLevelEi：写 m_field8ebd8（0 → 1）。
// ============================================================================
#include "CUser/SetUserMaxLevel.inc"
{
    // ORIG 0x0868fec8：>85 clamp 到 85；>0 取 level；否则 1。
    if (level > 85)
        m_field8ebd8 = 85;
    // ORIG 0x868fee4 `jg 868fef5(m=level)`：假路 fall=m=1 ⇒ 源码用反向 `<=0`（技巧 17，4.4 对 if(<=0){1}else{v} 产 jg 前进跳）
    else if (level <= 0)
        m_field8ebd8 = 1;
    else
        m_field8ebd8 = level;
}

// ============================================================================
// ORIG T 0x0868fe2c _ZN5CUser14SetManageLevelEi：超上限取脚本 m_manageLevelMax。
// ============================================================================
// b28d-solo-CUser AE: ORIG 0x0868fe32-0x0868fe61 `cmp slot,%eax; setl %al; test %al,%al; je`
//   无 bool 槽（sub $0x8）；具名 less 落槽（-0x9）⇒ 删局部内联（probe_bool a6/a7 对照）。
//   条件/体各完整重估 G_CDataManager() 链（P183）。| _ZN5CUser14SetManageLevelEi
#include "CUser/SetManageLevel.inc"
{
    if (G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax < level)
        m_field8ebd0 = G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    else
        m_field8ebd0 = level;
}
// ============================================================================
// ORIG T 0x0868fe72 _ZN5CUser14SetManagePointEi：点数封顶升级所需后写入。
// ============================================================================
#include "CUser/SetManagePoint.inc"
{
    // ORIG 0x868fe79-0x868fe9f：两次 G_CDataManager call（不折叠）；lvmax 先落
    // -0x10 槽再 `mov -0x10,%ebx`（跨 call callee-saved 持值，技巧 2/31），实参
    // `mov %ebx,0x4(%esp)` 先于 `mov %eax,(%esp)`。
    // b28d-solo-CUser N封顶: ORIG 0x868fe79 值落 -0x10 槽再 `mov -0x10,%ebx` 跨 call
    //   持值（push ebx/sub $0x24）；4.4 三形实测：具名→双槽(-0x14/-0x10)多中转、
    //   裸 register→直入 %ebx 免槽（无 push ebx）、语句表达式→值内联更差 ⇒ N7/N18a
    //   寄存器分配族（register 形为最近形 our=26 vs orig=27）。
    int lvmax =
        G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    int require = G_CDataManager()->m_altarConfig.m_characManageScript
                      ->GetLevelUpRequirePoint(lvmax);
    if (point > require)
        point = require;
    m_field8ebd4 = point;
}

// ============================================================================
// ORIG T 0x0868ffa0 sz 0x86 _ZN5CUser20CalcurateManagePointEv：
//   Σ 各角色等级对应管理点数后 SetManagePoint。
// ============================================================================
#include "CUser/CalcurateManagePoint.inc"
{
    // ORIG 0x0868ffa0：total=-0x10（先声明远槽）/i=-0xc（int，`cmp -0xc,%eax; setg`
    // 有符号比较）/level 无槽直落 %ebx（`movzwl 0x27,%eax; movswl %ax,%ebx` 跨
    // G_CDataManagerv 存活）。short→int 拓宽作为 GetPoint 实参**直传**（probe s3）；
    // 经 `register int asm("ebx")` 中间形反产 cwtl+mov。
    int total = 0;
    for (int i = 0; i < (int)m_characList.size(); ++i)
    {
        total += G_CDataManager()->m_altarConfig.m_characManageScript->GetPoint(
            m_characList[i].m_level);
    }
    SetManagePoint(total);
}

// ============================================================================
// ORIG T 0x08690026 sz 0x25 _ZN5CUser20CalcurateManageLevelEv：
//   重算点数、查等级、SetManageLevel；返回等级是否提升。
// ============================================================================
#include "CUser/CalcurateManageLevel.inc"
{
    CalcurateManagePoint();
    // ORIG 0x8690b??：newLevel 槽 -0x10；GetManageLevel() 结果**不落槽**直接
    // `cmp -0x10,%eax; setl;test;je`（真分支 movb $1,-0x9）；后 SetManageLevel；
    int newLevel = G_CDataManager()->m_altarConfig.m_characManageScript->GetLevel(
        GetManagePoint());
    bool up = false;
    if (GetManageLevel() < newLevel)
        up = true;
    SetManageLevel(newLevel);
    return up;
}

// ============================================================================
// ORIG T 0x08690b58 _ZN5CUser21SendManageLevelUpNotiEv：空包 header(0,0x19f)。
// ============================================================================
#include "CUser/SendManageLevelUpNoti.inc"
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19f);
    packet.finalize(true);
    Send(packet);
}

// ============================================================================
// CUser::ReturnToSelectCharacList @ 0x08686fee（ReturnToSelectCharacList.md）
//   返回角色选择列表：成员税上报 → 各管理器 CheckOut → 私商/突袭/专家/在线
//   团队离场 → 播放时间落库 → 复仇数据 → DB 日志/更新 → 登出缓存 → 离开
//   世界 → 重置角色/任务 → ARAD 周期数据清理 → 角色视图复位。
// ============================================================================
// 以下依赖符号均为 ORIG 真实符号；类/实现由对应域批次提供：
//   DB_UpdateCharacPlayTime::makeRequest(int,long)（DB 域）
//   DB_UpdateRevengeData::makeRequest(uint,uchar,uchar)（DB 域）
//   expert_job::CExpertJobMgr::OnLeaveUser(CUser*)（专家批次）
//   online_preliminary::COnlinePreliminaryTeamMgr::OnLeaveUser(CUser*)（在线批次）
//   WongWork::CHandlePremium::handleReturn2CharacView(CUser*)（premium 批次）
//   ARAD::Singleton<Arad_DataManager>::Get()（ARAD 域；SPECIAL）
// ============================================================================
// 注：以下依赖符号（DB_UpdateCharacPlayTime/DB_UpdateRevengeData、
// expert_job::CExpertJobMgr::OnLeaveUser、online_preliminary::
// COnlinePreliminaryTeamMgr::OnLeaveUser、WongWork::CHandlePremium::
// handleReturn2CharacView）均为 ORIG 真实符号，但对应权威类声明缺方法 /
// DB 请求类未建模，属外域缺口（BLOCKED，见报告）；其调用点在下方保留为
// 真实调用，由对应域批次补权威声明后即可编译。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08686fee | _ZN5CUser24ReturnToSelectCharacListEb
#include "CUser/ReturnToSelectCharacList.inc"
{
    UpdateCurCharacVisibleToCharacViewMemory();
    WorkPerFiveMin();

    if (get_charac_memberkey())
    {
        int payMoney = get_member_pay_tex_money_to_upper();
        int payFatigue = get_member_pay_tex_fatigue_to_upper();
        unsigned int characNo = getCurCharacNo();
        // b28d-GrpD（P183 去具名 receiver 缓存）：ORIG 0x08687079 `call GetServerProxy`
        //   返回 eax 后**无槽写**，0x08687097 直接 call SendMemberPayTax；我方具名
        //   proxy 多 `mov %eax,-0xc(%ebp)` + `mov -0xc(%ebp),%eax` 各 1 条
        //   （探针 /tmp/L9/D/p5.cpp 形 C 直证 −2 条且 C1/C2 求值序不变）⇒ 内联 receiver。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendMemberPayTax(is_connect_upper_member(), characNo, payMoney,
                               payFatigue);
        if (payMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    CGameManager* gm = G_CGameManager();
    set_uppermember_exp_level(0);
    set_connect_upper_member(0);
    gm->CheckOutTrade(this);
    gm->CheckOutParty(this, false);
    gm->CheckOutPvp(this, false);
    gm->CheckOutWarRoom(this);
    gm->checkOutBlueMarble(this);

    private_store::CPrivateStoreMgr* psMgr = private_store::GetInstancePrivateStoreMgr();
    (*(void(**)(private_store::CPrivateStoreMgr*, CUser*))(*(void**)psMgr))(psMgr, this);

    	pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(this, true);
	GlobalData::s_ExpertJobMgr->OnLeaveUser(this);
	DB_UpdateCharacPlayTime::makeRequest(get_charac_no(-1), GetCharacPlayTick());
	GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(this);
	unsigned int curCharacNo = getCurCharacNo();
	unsigned char villageCnt = GetCurVillageAttackCount();
	unsigned char revengeCnt = GetCurRevengeDungeonCount();
	if (IsVillageAttackDBUpdate())
		DB_UpdateRevengeData::makeRequest(curCharacNo, villageCnt, revengeCnt);

    DBUpdateDBLogItem();
    ResetDBLogItem();
    UpdateData();
    LogoutCachedCharacter(1);

    G_GameWorld()->leave_game_world(this);
    ResetCharacExpandData();

    {
        // 2026-09-10（FL2CUser 类型轴，同 DisConnSig）：ORIG 0x08686fee 段 p167 差集
        //   只有「我们多 1×`SsC2ERKSs` 拷贝构造 + 2×`SsD2Ev` + 1×`_Unwind_Resume`」，
        //   而形参 `findGameScriptESs` 为按值 std::string ⇒ 原码直传字面量
        //   （临时 elide 直达形参），无具名 name 局部（EH 清理块亦随之消失）。
        ((ARAD::Arad_EventPeriodDataManager*)ARAD::Singleton<ARAD::Arad_DataManager>::Get()
             ->findGameScript("Arad_EventPeriodDataManager"))
            ->delUserRewardData(this);
    }

    ResetCurCharac();
    resetSaveFlags();
    set_state((ch_state)2);
    doLinkCharacDisconnect();

    G_GameWorld()->InsertLoginUser(this);
    m_inventoryMemory.reset();
    m_quest.reset();

    getPad()->setCheckTime((long)time(0));
    	CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
	WongWork::CHandlePremium::handleReturn2CharacView(this);

    if (param)
    {
        PacketGuard pg;
        pg.put_header(1, 7);
        pg.put_byte(1);
        pg.finalize(true);
        Send(pg);
    }
}

// ============================================================================
// InventoryMemory::reset @ 0x0822d472（W 0x83）：四段 memset（0x4a58/0x1905/
// 0x39aa/0x2438，合计 0xc13f），末字节 +0xc13f 不清，逐段忠实转写。
#include "CUser/reset_4.inc"
{
    memset(m_items1, 0, 0x4a58);
    memset(m_items2, 0, 0x1905);
    memset(m_items3, 0, 0x39aa);
    memset(m_items4, 0, ARAD::DATABASE::sourceCargoSize);
}

// ============================================================================
// Sanicova::CPad::setCheckTime @ 0x084ebe20（W 0xe 字节）：写 +0x28 m_checkTime28。
// ============================================================================
namespace Sanicova
{
#include "CUser/setCheckTime.inc"
{
    m_checkTime28 = t;
}
}

// ============================================================================
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08651628 | _ZN5CUser40UpdateCurCharacVisibleToCharacViewMemoryEv
#include "CUser/UpdateCurCharacVisibleToCharacViewMemory.inc"
{
    // 2026-09-10（G3CUser-B 负结论）：ORIG 0x08651628 vs ours（88→92 insn，帧
    //   0x34→0x38）差异的根在 **CInventory 头内布局**，非本 .inc 可修：
    //   ORIG 0x0865170f `mov -0x18(%ebp),%eax; imul $0x3d,%eax,%eax; add $0x10,%eax;
    //   add -0x1c(%ebp),%eax; add $0xc,%eax`（vis = inven + i*0x3d + 0x1c）与
    //   `add $0xd0,%eax; shl $0x3,%eax; add -0x1c(%ebp),%eax; add $0x4`（m_invenItem
    //   步长 8、基址 +0x680）两侧采用**不同元素步长/基址**；我方 `m_avatarVisible[i]`
    //   走 8 字节步长视图 ⇒ 结构步长与 ORIG 不符。属 .h（CInventory.h）类型轴，
    //   按约束只报告不落，已报 Main。
    CInventory* inven = getCurCharacInvenW();
    if (inven == 0)
        return;

    for (int i = 0; i < 12; ++i)                                 // ORIG 0..0xb
    {
        CInventory::AvatarVisibleData& vis = inven->m_avatarVisible[i];  // b28d-lane5: Main 代落匿名 struct 包裹（N84）
        if (inven->m_invenItem[i].m_addInfo == 0)
        {
            vis.init();                                          // ORIG 0x0865171f 调 init
        }
        else
        {
            // ORIG：GetClearAvatar(i) 在 if 条件与 else 分支各自重调，结果禁缓存
            // （战术 P183/P48）。
            if (inven->GetClearAvatar(i) == 0)
                vis.m_field0 = inven->m_invenItem[i].m_addInfo;
            else
                vis.m_field0 = inven->GetClearAvatar(i);

            // ORIG key = item+0x07（m_addInfo2）；GetExpansionInfo 返回 {short,short}
            const WongWork::CAvatarItemMgr* mgr = inven->GetAvatarItemMgrR();
            void* info = const_cast<WongWork::CAvatarItemMgr*>(mgr)
                             ->GetExpansionInfo(inven->m_invenItem[i].m_addInfo2);
            if (info != 0)
            {
                // [L9-C组J PATCH J3] ORIG `movzwl 0x2(%eax),%edx` 单条 disp；`expansion[1]`
                //   （常量下标）在 4.4 产 `add $0x2,%eax; movzwl (%eax),%edx`（多 1 条）
                //   ⇒ 改成员访问（p_disp f_pidx vs f_member 直证）。
                struct ExpansionPair
                {
                    short m_first;   // +0x0
                    short m_second;  // +0x2
                };
                const ExpansionPair* expansion =
                    static_cast<const ExpansionPair*>(info);
                vis.m_field4 = expansion->m_first;
                vis.m_field6 = expansion->m_second;
        }
    }
            }



}
// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// 定义置于本 TU（与 W7_UserOnlyC.cpp 中 CPacketVerifier::Init 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
#include "CUser/reset_5.inc"
{
    m_lastTime = 0;
    m_count = 0;
}

#include "CUser/check.inc"
{
    ++m_count;
    long now = time(0);
    // P59 极性：ORIG jge→后置赋值块，fall-through 为节流判断块。
    if (now - m_lastTime < validSecond_)
    {
        if (m_count > validCount_)
            return false;
    }
    else
    {
        m_lastTime = now;
        m_count = 0;
    }
    return true;
}

// ============================================================================
// nexon_packet::stHeader::stHeader（ORIG 0x086941cc W，magic=0xaa; length=0;
// cmd=0）。定义原在 nexon_packet.cpp，迁入本 TU（stPacketC1 的 callee）——
// stPacketC1 所在 TU 不再含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing
// 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
// ============================================================================
#include "CUser/stHeader.inc"
    : m_magic(0xaa), m_length(0), m_cmd(0)
{
}
