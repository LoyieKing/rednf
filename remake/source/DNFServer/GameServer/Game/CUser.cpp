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
//      · getCharacInfoDetail（P171 照抄）：补 ORIG 重复赋值 `m_field114 ← m_powerWarAssaultVictoryCount`
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
//      ① [R19ty T-2b 2026-09-13 **已落**] `CStackableItem::StatChangeInfo::m_flagLo`
//         (+0x2e0 首字节) char→bool（ORIG 0x0865ec1d `movzbl/xor $0x1/test %al,%al/je`，
//         P158/P248 档②）；配套 CStackableItem.cpp 镜像 STStackableScript::m_field360 同步
//         bool（ORIG set_item 0x0850df79 单段直存，P136）；本 .inc 读点改后逐条同形，
//         CUser TU 四数 809/1728/5/58/586 逐值零劣化；
//      ② `TAG_CHARAC_DATA` +0x21 目标字段 char→uchar（ORIG 0x086905c1 单段 `movzbl` 直存）；
//      ③ `SIG_LOAD_ETC` +0xc9ae/+0xc9b3/+0xc9b7 字段与 getter 同型化（ORIG 0x08677af2 同型直存）；
//      ④ `CUserCharacInfo.h:308` `CCargo::MakeItemList(PacketGuard*)` int→bool（ORIG 0x0865dc10
//         → 0x0865dc1f `xor $0x1,%eax`）+ 定义侧 CCargo.cpp 需同刻同步（跨 TU，未落）。
//      ⑤ `CUser::GetPuUser()`（CUser.h:2858）unsigned char→bool **判 N 不改**（K8GlobalData 请求，
//         Main 本泳道复核）：ORIG 本体 0x08692b74 `movzbl 0x796d8(%eax),%eax; pop; ret`（无归一化，
//         生产端像 uchar）vs 消费点 0x08179e56 `call; xor $0x1,%eax; test %al,%al; je`（消费端像 bool）
//         ⇒ P321 T3 「两端矛盾 ⇒ 记 N」；且探针（/tmp/pu_probe.cpp，c6root 4.4.7）证明 bool 化后
//         4.4 在 `return <uchar 字段>` 处插 `testb %al,%al; setne %al` ⇒ 本体 +2 条、本 TU
//         identical→DIFF；字段 `m_puUser` 亦不可 bool 化（InterDispatcher.cpp:18163 包读可 >1、
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

#include "CUser/CDataMgr.inc"

// ---- G1-3 临时外部桩（对应 TU 翻译后移除） ----
// ORIG T 0x0808ca12：mov $0x7,%eax 直返 7
unsigned char _NS_PI_2ND_GetDefaultRandomHashKey() { return 7; }
#include "CUser/resetExposedCount.inc"
// 0x0864809c：antibot 校验/加密发送主链路（依据 AntibotSend.md 反编译；
// s_antibotChecker_ vtable +0x34 校验 / +0x28 加密，ORIG 栈缓冲此处以 vector 等价）。
extern void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user); // ORIG T 0x082751ee
#include "CUser/AntibotSend.inc"


#include "CUser/_Quest_Authen_Data.inc"
#include "CUser/reset.inc"

// [b28d-Main·检查点18] 删除 stub `mission_list_is_clear`：ORIG 无该符号、全仓无调用点。

#include "CUser/reset_1.inc"


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
#include "CUser/resetCancelCnt.inc"
#include "CUser/getCancelCnt.inc"
}


#include "CUser/AddCharac.inc"

#include "CUser/RemoveCharac.inc"

using expert_job::CCharacExpertJob;

#include "CUser/dtor_CDataMgr.inc"

#include "CUser/CharacAdvanceAltarManager.inc"
#include "CUser/dtor_CharacAdvanceAltarManager.inc"
#include "CUser/PIReceiverManager.inc"
#include "CUser/dtor_PIReceiverManager.inc"
#include "CUser/PISenderManager.inc"
#include "CUser/dtor_PISenderManager.inc"
#include "CUser/TurnStomach.inc"

#include "CUser/GetData.inc"

#include "CUser/GetDataR.inc"

#include "CUser/reset_2.inc"


#include "CUser/ResetDailyMidnight.inc"


#include "CUser/ResetDaily.inc"


#include "CUser/update.inc"


#include "CUser/update_1.inc"

#include "CUser/GrowthWeaponEvent.inc"

#include "CUser/Secu_HackLogCheck.inc"

#include "CUser/Secu_MonsterCheck.inc"

#include "CUser/Secu_GoldControl.inc"

#include "CUser/Character.inc"

#include "CUser/BingoData.inc"

#include "CUser/clear.inc"

#include "CUser/GrowthCreatureEvent.inc"

#include "CUser/CharacLevelUpGiftAccountOnce.inc"

#include "CUser/PlayInfo.inc"

#include "CUser/Reset.inc"

#include "CUser/Reset_1.inc"


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

#include "CUser/CUser.inc"


#include "CUser/dtor_CUser.inc"


#include "CUser/reset_3.inc"


#include "CUser/ResetCurCharac.inc"


#include "CUser/reset_server_fatigue_data.inc"


#include "CUser/get_server_fatigue_count.inc"


#include "CUser/get_fatigue_count_db.inc"


#include "CUser/reset_ingame_event_history.inc"


#include "CUser/reset_ingame_event_history_update.inc"


#include "CUser/reset_pvp_masterid_walkingout_me.inc"


#include "CUser/set_guildwar_point_per_pvpplay.inc"


#include "CUser/get_guildwar_point_per_pvpplay.inc"


#include "CUser/getHades.inc"


#include "CUser/GetPVPRoom.inc"


#include "CUser/SendSecretShopItemList.inc"


#include "CUser/GetPICSMap.inc"


#include "CUser/CheckMoney.inc"


#include "CUser/getDungeonIdxAfterClear.inc"


#include "CUser/resetMoneyLog.inc"


#include "CUser/ResetDBLogItem.inc"


#include "CUser/DBUpdateDBLogItem.inc"


#include "CUser/GetParty.inc"


#include "CUser/GetUserPosInParty.inc"


#include "CUser/GetWarRoom.inc"


#include "CUser/get_charac_idx.inc"

#include "CUser/get_charac_no.inc"


#include "CUser/CheckItemLock.inc"


#include "CUser/getCurCharacTotalFatigue.inc"


#include "CUser/getCurCharacTotalMaxFatigue.inc"


// ============================================================================
// CUser::make_basic_info —— ORIG _ZN5CUser15make_basic_infoEPcc @0x0865a44e
// 报告：docs/class_func_reports/CUser/make_basic_info.md（## 汇编 + ## 反编译 C
// 全文核对）。按 type 分派：0=ENUM_USERINFO_MINIMUM、1=ENUM_USERINFO_ADDITION、
// 2=账号角色列表（m_characList 循环）、3=ENUM_USERINFO_OTHER_CHARACTER；
// 其余值直接返回 1。三个需当前角色的分支在 getCurCharacR()==0 时
// LogManager::logFormat 报错并返回 0；各分支 lock → 填包 → unlock。
// ============================================================================

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

#include "CUser/make_basic_info.inc"

#include "CUser/send_equip.inc"


#include "CUser/AddDBLogItem.inc"


#include "CUser/addAvatarItem.inc"


#include "CUser/SendUpdateItemList.inc"


#include "CUser/GetCurExpertJobLevel.inc"


#include "CUser/is_clear_stealingSkillMission.inc"


#include "CUser/makeGuildSkillMessage.inc"


#include "CUser/makeGuildItemUpgradMessage.inc"


#include "CUser/set_grow_type.inc"


#include "CUser/get_acc_id.inc"


#include "CUser/get_acc_name.inc"


#include "CUser/get_state.inc"


#include "CUser/set_state.inc"


#include "CUser/GetUID.inc"


#include "CUser/get_unique_id.inc"


#include "CUser/getDungeonGainedGold.inc"


#include "CUser/getHackAnalyzer.inc"


#include "CUser/getGmQuestFlag.inc"


#include "CUser/GetAccountLastPlayTime.inc"


#include "CUser/GetServerGroup.inc"


#include "CUser/GetCharacExpandData.inc"


#include "CUser/GetCharacExpandDataR.inc"



// ---- quest point / piece helpers（ORIG：CUser 0x868ad88 / 0x868afd0 / 0x868adc2 / 0x868af66）----

#include "CUser/clearInitQuestPoint.inc"


#include "CUser/clearQuestPiece.inc"


#include "CUser/incInitQuestPoint.inc"


#include "CUser/incQuestPiece.inc"


#include "CUser/gainWinPoint.inc"


#include "CUser/setInitQuestPoint.inc"


#include "CUser/GetPremiumInfo.inc"


#include "CUser/GetAdvantageCoin.inc"


#include "CUser/isAffectedUnlimitFatigue.inc"


#include "CUser/getCurCharacQuestR.inc"


#include "CUser/getCurCharacQuestW.inc"


#include "CUser/DecreaseGuildPowerWarPoint.inc"


#include "CUser/is_fighting.inc"


#include "CUser/set_before_area.inc"


#include "CUser/set_area.inc"


#include "CUser/get_area.inc"


#include "CUser/get_pvp_WinningRate_relateMission.inc"


#include "CUser/get_aura_avatar_option_value.inc"


#include "CUser/SetTradeSpace.inc"


#include "CUser/CheckFatigue.inc"


#include "CUser/gainCurCharacFatigue.inc"


#include "CUser/has_within_Mission.inc"


#include "CUser/getBlueMarble.inc"


#include "CUser/checkLogOutCorrectly.inc"


#include "CUser/find_pvp_masterid_walkingout_me.inc"


#include "CUser/insert_pvp_masterid_walkingout_me.inc"


#include "CUser/update_old_pvp_point.inc"


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

#include "CUser/RecoverFatigue.inc"


#include "CUser/LogHistory.inc"


#include "CUser/isBlackUser.inc"


#include "CUser/IsHavePremiumAdvantage.inc"


#include "CUser/IsEquipAvatar.inc"


#include "CUser/getWebAddress.inc"


#include "CUser/getSecurityCard.inc"


#include "CUser/GetGameOptionRef.inc"


#include "CUser/getPad.inc"


#include "CUser/getBeforeAreaTime.inc"


#include "CUser/setBeforeAreaTime.inc"


#include "CUser/getLastPacketID.inc"


#include "CUser/setLastPacketID.inc"


#include "CUser/getPlayingDungeonIndex.inc"


#include "CUser/setPlayingDungeonIndex.inc"


#include "CUser/getMoveSpace.inc"


#include "CUser/setMoveSpace.inc"


#include "CUser/CheckMoveTown.inc"


#include "CUser/isUseCraneStart.inc"


#include "CUser/setUseCraneStart.inc"


#include "CUser/getForceDropFlag.inc"


#include "CUser/setForceDropFlag.inc"


#include "CUser/isDungeonClearHackCheck.inc"


#include "CUser/setDungeonClearHackCheck.inc"


#include "CUser/isAllowLocationGeoState.inc"


#include "CUser/setAllowLocationGeoState.inc"


#include "CUser/isComboSkillSet.inc"


#include "CUser/setComboSkillSet.inc"


#include "CUser/lock4DataLoad.inc"


#include "CUser/unlock4DataLoad.inc"


#include "CUser/lockCreateCharac.inc"


#include "CUser/unlockCreateCharac.inc"


#include "CUser/lockDeleteCharac.inc"


#include "CUser/unlockDeleteCharac.inc"


#include "CUser/isLocked4DataLoad.inc"


#include "CUser/isLockedCreateCharac.inc"


#include "CUser/isLockedDeleteCharac.inc"


#include "CUser/get_growth_power_exp_reward.inc"


#include "CUser/setAntibotKey.inc"


#include "CUser/setClientVersion.inc"


#include "CUser/getClientVersion.inc"


#include "CUser/ResetUsedCoinInDungeon.inc"


#include "CUser/ClearItemSellCount.inc"


#include "CUser/Send.inc"


#include "CUser/SendPacket.inc"


#include "CUser/SendCmdPacket.inc"


#include "CUser/SendCmdOkPacket.inc"


#include "CUser/SendCmdErrorPacket.inc"


#include "CUser/SendCmdErrorPacket_1.inc"


#include "CUser/SendCmdErrorPacket_2.inc"


#include "CUser/SendCmdErrorPacket2Byte.inc"


#include "CUser/SendCmdErrorPacketJpn.inc"


#include "CUser/AuctionPreCheck.inc"


#include "CUser/SendCmdPeerErrorPacket.inc"


#include "CUser/SendCmdPeerErrorPacket_1.inc"


#include "CUser/SendZeroExpPacket.inc"


#include "CUser/SendMaxEquipLevel.inc"


#include "CUser/SendFatigue.inc"


// ============================================================================
// G1-4 批次：索引 / 状态访问器（ORIG 逐符号转录）
// ============================================================================

#include "CUser/CheckInTrade.inc"


#include "CUser/isEnableAvatarSocketAction.inc"


#include "CUser/CheckInParty.inc"


#include "CUser/checkInBlueMarble.inc"


#include "CUser/CheckInWarRoom.inc"


#include "CUser/checkInDeathTower.inc"


#include "CUser/checkInBossTower.inc"


#include "CUser/checkInAdvanceAltar.inc"


#include "CUser/getAdvanceAltarIndex.inc"


#include "CUser/getBlueMarbleIndex.inc"


#include "CUser/setBlueMarbleIndex.inc"

#include "CUser/getDeathTowerIndex.inc"


#include "CUser/setDeathTowerIndex.inc"


#include "CUser/getDeathTower.inc"


// ---- Redeem_Item::CRedeemItem（GetCharacExpandData(6) 返回，未建模为
// CExpandEquipslot* 槽位；权威声明见 Redeem_Item__CRedeemItem.h，实现于
// Redeem_Item__CRedeemItem.cpp，ORIG 0x085f76f0 / 0x085f71a8）----

#include "CUser/Add_RedeemInfo.inc"


#include "CUser/getBossTowerIndex.inc"


#include "CUser/setBossTowerIndex.inc"


#include "CUser/GetPartyIndex.inc"


#include "CUser/CheckInPvp.inc"


#include "CUser/GetPvpIndex.inc"


#include "CUser/GetWarRoomIndex.inc"


#include "CUser/GetTradeSpace.inc"


#include "CUser/SetPartyIndex.inc"


#include "CUser/SetWarRoomIndex.inc"


#include "CUser/SetSlotIDX.inc"


#include "CUser/GetIncreID.inc"


#include "CUser/SetIncreID.inc"


#include "CUser/set_unique_id.inc"


#include "CUser/set_position.inc"


#include "CUser/get_posX.inc"


#include "CUser/get_posY.inc"


#include "CUser/get_direction.inc"


#include "CUser/GetUserState.inc"


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


#include "CUser/GetSeedFromDate.inc"


#include "CUser/SetGMUpgradeMode.inc"


#include "CUser/isGMUser.inc"


#include "CUser/IsGameMasterMode.inc"


#include "CUser/SetGameMasterMode.inc"

#include "CUser/GetEventCreateDnfReward.inc"


#include "CUser/SetETC.inc"


// ============================================================================
// G1-4 批次：CodeHackCheckStorage / 返回用户 / 预登录数据
// ============================================================================

#include "CUser/setTournamentAccount.inc"


#include "CUser/isTournamentAccount.inc"


#include "CUser/setJoinedSchoolPointEvent.inc"


#include "CUser/setOnlinePreliminaryServerGroup.inc"


#include "CUser/setOnlinePreliminaryCharacNo.inc"


#include "CUser/setOnlinePreliminaryPlayType.inc"


#include "CUser/getOnlinePreliminaryPlayType.inc"


#include "CUser/getOnlinePreliminaryCharacNo.inc"


#include "CUser/SetPCRoomNo.inc"


#include "CUser/SetFirstLogin.inc"


#include "CUser/SetReturnUser.inc"


#include "CUser/ResetReturnUser.inc"


#include "CUser/IsReturnUser.inc"


#include "CUser/SetReturnUserFirstUserLogin.inc"


#include "CUser/IsReturnUserFirstLogin.inc"


#include "CUser/GetReturnUserExpireTime.inc"


#include "CUser/SetReturnUserExpireTime.inc"


#include "CUser/EnableSaveMemberBonusFatigue.inc"


#include "CUser/incAccountMemberBonusFatigue.inc"


#include "CUser/resetAccountMemberBonusFatigue.inc"


#include "CUser/getAccountMemberBonusFatigue.inc"


#include "CUser/resetAccountUsedFatigue.inc"


#include "CUser/incChattingMessageCount.inc"


#include "CUser/setChattingMessageCount.inc"


#include "CUser/getChattingMessageCount.inc"


#include "CUser/SetGameMasterCharacter.inc"


#include "CUser/getStdDropRate.inc"


#include "CUser/getAcccountHackInfo.inc"


#include "CUser/setHackUserTimer.inc"


#include "CUser/setHackUserFlag.inc"


#include "CUser/isHackUser.inc"


#include "CUser/isHackUserTimer.inc"


// ============================================================================
// G1-4 批次：突破奖励 / 账号货物 / 商城 / 日常
// ============================================================================

#include "CUser/STBreakAwayRewards.inc"


#include "CUser/clear_1.inc"


void STBreakAwayRewards::operator=(const STBreakAwayRewards& other)
{
    m_rewardItemId = other.m_rewardItemId;
    m_rewardAItemCnt = other.m_rewardAItemCnt;
    m_luckPoint = other.m_luckPoint;
    m_fieldc = other.m_fieldc;
    m_itemOrder = other.m_itemOrder;
}

#include "CUser/getBreakAwayRewardItemId.inc"


#include "CUser/setBreakAwayRewardItemId.inc"


#include "CUser/getBreakAwayRewardAItemCnt.inc"


#include "CUser/setBreakAwayRewardAItemCnt.inc"


#include "CUser/getUserBreakAwayRewardLuckPoint.inc"


#include "CUser/setUserBreakAwayRewardLuckPoint.inc"


#include "CUser/getBreakAwayRewardItemOrder.inc"


#include "CUser/getBreakAwayRewards.inc"


#include "CUser/setBreakAwayRewards.inc"


#include "CUser/getBreakAwayQuestTime.inc"


#include "CUser/setBreakAwayQuestTime.inc"


#include "CUser/getBreakAwayDungeonClearCnt.inc"


#include "CUser/setBreakAwayDungeonClearCnt.inc"


#include "CUser/subBreakAwayDungeonClearCnt.inc"


#include "CUser/setBreakAwayDungeonClear.inc"


#include "CUser/isBreakAwayRewardData.inc"


#include "CUser/setGmQuestFlag.inc"


#include "CUser/setWebAddress.inc"



#include "CUser/SetCera.inc"


#include "CUser/UseCera.inc"


#include "CUser/setRegDate.inc"


#include "CUser/setRegTime.inc"


#include "CUser/getRegTime.inc"


#include "CUser/resetUsedCera.inc"


#include "CUser/addUsedCera.inc"


#include "CUser/getUsedCera.inc"


#include "CUser/SetVillageAttackedReward.inc"


#include "CUser/setPcRoomAuth.inc"


#include "CUser/fillUserInfo4Antibot.inc"


#include "CUser/getUserInfo4Antibot.inc"


#include "CUser/SetAccountLastPlayTime.inc"


#include "CUser/SetLoginTime.inc"


#include "CUser/GetLoginTime.inc"


#include "CUser/SetLogInOutState.inc"


#include "CUser/GetLogInOutState.inc"


#include "CUser/get_public_ip_address.inc"


#include "CUser/set_public_ip_address.inc"


#include "CUser/set_local_ip_address.inc"


#include "CUser/setHangameUser.inc"


#include "CUser/isHangameUser.inc"


#include "CUser/IsPowerUp.inc"


#include "CUser/SetPowerUp.inc"


#include "CUser/SetRating.inc"


#include "CUser/SetSchoolNo.inc"


#include "CUser/GetSchoolNo.inc"


#include "CUser/SetSchoolName.inc"


#include "CUser/GetTutorialSkipable.inc"


#include "CUser/SetTutorialSkipable.inc"


#include "CUser/getHumanCertifyTimerKey.inc"


#include "CUser/setHumanCertifyErrorCnt.inc"


#include "CUser/isSendMailCertified.inc"


#include "CUser/setRestingUserRestrict.inc"


#include "CUser/setPunishTradeAlert.inc"


#include "CUser/getPunishTradeAlert.inc"


#include "CUser/resetDungeonClearHackCheck.inc"


#include "CUser/IsRecvEvent.inc"


#include "CUser/GetAutoClientRate.inc"


#include "CUser/isARSUserKick.inc"


#include "CUser/setSecuReward.inc"


#include "CUser/NeedSecuReward.inc"


#include "CUser/setSecuType.inc"


#include "CUser/getSecuType.inc"


#include "CUser/getRewardType.inc"


#include "CUser/setRewardType.inc"


#include "CUser/setSecuGrade.inc"


#include "CUser/setFlagPresentAvengerTitle.inc"


#include "CUser/incCharacSlotLimit.inc"


#include "CUser/getCharacSlotLimit.inc"


#include "CUser/getSlotEffectCount.inc"


#include "CUser/setSlotEffectCount.inc"


#include "CUser/SendRandomBoxStatistic.inc"


#include "CUser/verifyStackableItemActionPlace.inc"


#include "CUser/verifyStackableItemProcess.inc"


#include "CUser/set_ontime_last_recv_idx.inc"


#include "CUser/get_ontime_last_recv_idx.inc"


#include "CUser/set_update_ontime_last_recv_idx.inc"


#include "CUser/setCheckPickUpRandomOptionItem.inc"


#include "CUser/getCheckPickUpRandomOptionItem.inc"


#include "CUser/set_cera_item_order_no.inc"


#include "CUser/get_cera_item_order_no.inc"


#include "CUser/setPcRoomPlayStartTime.inc"


#include "CUser/IsDeleteEventDungeonInvitation.inc"


#include "CUser/IsUsedCoinInDungeon.inc"


#include "CUser/ResetCoinCount.inc"


#include "CUser/GetUsedCoinCount.inc"


#include "CUser/getCurCharacQuestRefR.inc"


#include "CUser/getAddIndependentDropRateFromPremium.inc"


#include "CUser/EventDungeonClearReward.inc"


#include "CUser/set_trick_area.inc"


#include "CUser/get_before_area.inc"


// ============================================================================
// G1-4 批次：锁 / 账号货物 / 其余访问器
// ============================================================================

#include "CUser/GetAccountCargo.inc"


#include "CUser/IsExistAccountCargo.inc"


#include "CUser/DeleteCargo.inc"


#include "CUser/lock.inc"


#include "CUser/unlock.inc"


#include "CUser/getBackupSeed.inc"


#include "CUser/getPowerSide.inc"


#include "CUser/getClientHWSpec.inc"


#include "CUser/SetGuildDBInfo.inc"


#include "CUser/ChangeGuildName.inc"


#include "CUser/SetGuildAgitFlag.inc"


#include "CUser/SetGuildAgitDBInfo.inc"


#include "CUser/SetGuildCargoCapacity.inc"


#include "CUser/SetTodayGuildMember.inc"


#include "CUser/IsTodayGuildMember.inc"


#include "CUser/SetLastLoginCharacNo.inc"


#include "CUser/SetLastLoginChannelNo.inc"


#include "CUser/SetValidLastLoginCharac.inc"


#include "CUser/SetReportMannerlessUserCount.inc"


#include "CUser/GetChecksumAnswerCount.inc"


#include "CUser/IncChecksumAnswerCount.inc"


#include "CUser/GetCheckConnInterval.inc"


#include "CUser/SetResumeChecksumTime.inc"


#include "CUser/incre_check_count.inc"


#include "CUser/get_connect_count.inc"


#include "CUser/setTotalHackUserTradeGold.inc"


#include "CUser/setTotalHackUserTradeCnt.inc"


#include "CUser/SetRevivalTime.inc"


#include "CUser/GetRevivalTime.inc"


#include "CUser/SetUseAPCPotionTime.inc"


#include "CUser/GetUseAPCPotionTime.inc"


#include "CUser/SetDailyBadge.inc"


#include "CUser/SetUserDetailInfo.inc"


// ============================================================================
// G1-4 批次：角色列表 / 视图
// ============================================================================

#include "CUser/get_charac_name.inc"


#include "CUser/GetCharacJob.inc"


#include "CUser/getCharacLevelByCharacNo.inc"


#include "CUser/get_charac_name_by_charac_no.inc"


#include "CUser/setCharacName.inc"


#include "CUser/SetCharacViewState.inc"


#include "CUser/SetCharacViewState_1.inc"


#include "CUser/GetCharacViewState.inc"


#include "CUser/SetCharacViewStateByCharacNo.inc"


// ============================================================================
// 批次 A 访问器（ORIG 反汇编逐偏移补全：0x8101048 / 0x82301ae / 0x827565a /
// 0x85a932a / 0x868a99a / 0x868fa46 / 0x86912b0 / 0x8691cd0 / 0x8692af6 等）
// ============================================================================

#include "CUser/sendEventInfo.inc"


#include "CUser/questAutoClear.inc"


#include "CUser/_onCompoundItem.inc"


#include "CUser/_updateNPCRelationship.inc"


#include "CUser/processNPCGiftOnLevelUp.inc"


#include "CUser/GetUserName.inc"


#include "CUser/GetUserEMail.inc"


#include "CUser/GetSsnString.inc"


#include "CUser/getSex.inc"


#include "CUser/GetCSHashSet.inc"


#include "CUser/GetGuildName.inc"


#include "CUser/GetGuildLevel.inc"


#include "CUser/GetHackCheck.inc"


#include "CUser/getBingoData.inc"


#include "CUser/GetGuildDBInfo.inc"


#include "CUser/getGarenaAuthData.inc"


#include "CUser/GetPISenderManager.inc"


#include "CUser/GetPIReceiverManager.inc"


#include "CUser/getGrowthWeaponEventdata.inc"


#include "CUser/get_ingame_event_history.inc"


#include "CUser/get_ingame_event_history_update.inc"


#include "CUser/getGrowthCreatureEventdata.inc"


#include "CUser/get_server_fatigue_day_data.inc"


#include "CUser/getCharacLevelUpGift_AccountOnce.inc"


#include "CUser/GetMailBox.inc"


#include "CUser/GetMileage.inc"


#include "CUser/GetPCRoomNo.inc"


#include "CUser/GetCeraPoint.inc"


#include "CUser/getAntibotKey.inc"


#include "CUser/GetManageLevel.inc"


#include "CUser/GetManagePoint.inc"


#include "CUser/GetUserMaxLevel.inc"


#include "CUser/onDungeonClear.inc"


#include "CUser/GetDebugCommand.inc"


#include "CUser/GetTradePunishType.inc"


#include "CUser/getLastLotteryTime.inc"


#include "CUser/GetNonClientRandInt.inc"


#include "CUser/GetNonClientFlag.inc"


#include "CUser/GetLastLoginCharacNo.inc"


#include "CUser/GetLastLoginChannelNo.inc"


#include "CUser/IsValidLastLoginCharac.inc"


#include "CUser/getRecipeProbability.inc"


#include "CUser/get_local_ip_address.inc"


#include "CUser/GetGuildCargoCapacity.inc"


#include "CUser/GetGuildPowerWarPoint.inc"


#include "CUser/GetResumeChecksumTime.inc"


#include "CUser/GetGameMasterCharacter.inc"


#include "CUser/getBreakAwayAccureCera.inc"


#include "CUser/getBreakAwayRewardOrder.inc"


#include "CUser/get_multiboxLotteryItemFailCnt.inc"


#include "CUser/GetCurCharacUsedFatigueQuantity.inc"


#include "CUser/isHumanCertified.inc"


#include "CUser/isLoadingHackGold.inc"


#include "CUser/isSaveInformNoticeFlag.inc"


#include "CUser/CheckMaxLuckyLevel.inc"


// ---- 批次 A 第 2 组（ORIG 0x82300e8 / 0x82301d8 / 0x8230246 / 0x8692a08 /
// 0x8692b74 / 0x86971ce / 0x86975b6 / 0x8568e48 等） ----

#include "CUser/OnRecvEvent.inc"


#include "CUser/GetBuyingGold.inc"


#include "CUser/SetBuyingGold.inc"


#include "CUser/GetFirstLogin.inc"


#include "CUser/SetProgLogout.inc"


#include "CUser/isRestingUser.inc"


#include "CUser/InitReceivedRequestType.inc"


#include "CUser/isBreakAwayDungeonClear.inc"


#include "CUser/isUsedBreakAwayLuckPoint.inc"


#include "CUser/isJoinedSchoolPointEvent.inc"


#include "CUser/DisableSaveMemberBonusFatigue.inc"


#include "CUser/IsEnableSaveMemberBonusFatigue.inc"


#include "CUser/SetProperLevelDungeonUser.inc"


#include "CUser/ClearProperLevelDungeonUser.inc"


#include "CUser/IsProperLevelDungeonUser.inc"


#include "CUser/isCharacLinkMessageFlag.inc"


#include "CUser/isLinkCharacDisconnectFlag.inc"


#include "CUser/getDisconnectLinkCharacSlotIdx.inc"


#include "CUser/is_update_ontime_last_recv_idx.inc"


#include "CUser/reset_update_ontime_last_recv_idx.inc"


#include "CUser/CheckLoadRentalInfoFromExchange.inc"


#include "CUser/GetPuUser.inc"


#include "CUser/SetCeraPoint.inc"


#include "CUser/SetDebugCommand.inc"


#include "CUser/decre_check_count.inc"


#include "CUser/setLastLotteryTime.inc"


#include "CUser/SetNonClientRandInt.inc"


#include "CUser/recipeForceProbability.inc"


#include "CUser/setBreakAwayAccureCera.inc"


#include "CUser/setCharacAntibotSerialNum.inc"


#include "CUser/set_multiboxLotteryItemFailCnt.inc"


#include "CUser/resetPlayExpAdd.inc"


#include "CUser/resetCleanpadFailCnt.inc"


#include "CUser/isCleanPadVerifyLimit.inc"


#include "CUser/resetTotalPcRoomPlayTime.inc"


#include "CUser/ResetCurCharacUsedFatigueQuantity.inc"


#include "CUser/set_server_fatigue_count.inc"


#include "CUser/IncCurCharacUsedFatigueQuantity.inc"


#include "CUser/IncCurCharacUsedGiftFatigueQuantity.inc"


#include "CUser/ProcProperLevelDungeonClear.inc"


#include "CUser/GiveMeBoxReward.inc"


#include "CUser/SetTradePunishType.inc"


#include "CUser/SetMileage.inc"


#include "CUser/AddMoneyPlus.inc"


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

#include "CUser/exceptedStateMoneyLog.inc"



#include "CUser/UseWinPoint.inc"


#include "CUser/AddSecuRewardItem.inc"


#include "CUser/GenerateCleanPadVeriData.inc"


#include "CUser/reqHumanCertify.inc"


#include "CUser/sendNotiPcRoomPlayTime.inc"


#include "CUser/SendCashData.inc"


#include "CUser/recover_stamina.inc"


#include "CUser/WorkPerTenMin.inc"


#include "CUser/WorkPerFiveMin.inc"


#include "CUser/get_server_fatigue_day_size.inc"


#include "CUser/IncreaseUsedCoinCount.inc"


#include "CUser/setLoadHackGold.inc"


#include "CUser/SetNonClientFlag.inc"


#include "CUser/setHumanCertified.inc"


#include "CUser/incTradeCount.inc"


#include "CUser/GetRating.inc"


#include "CUser/backupSeed.inc"


#include "CUser/ResetCharacExpandData.inc"


#include "CUser/IsExistGuildAgit.inc"


#include "CUser/GetDailyBadge.inc"


#include "CUser/GetDailyBadge_1.inc"


#include "CUser/InsertOnTimeEventReward.inc"


#include "CUser/get_charac_count.inc"


#include "CUser/isTradePunishOverThirtyDays.inc"



#include "CUser/sendBlueMarbleEnterCount.inc"


#include "CUser/ResetDailyCharacExpandDataMidnight.inc"


#include "CUser/ResetDailyCharacExpandData.inc"


#include "CUser/resetNPCRelationShipDailyData.inc"

#include "CUser/sendNPCRelationShipFavor.inc"

#include "CUser/SendProperDungeonClearCount.inc"


#include "CUser/acceptable_within_mission.inc"


#include "CUser/isDuplicationMessage.inc"


#include "CUser/DimensionInoutUpdate.inc"


#include "CUser/send_pvp_record.inc"


#include "CUser/send_clear_quest_list.inc"


#include "CUser/processReturnUserQuestAutoClear.inc"


#include "CUser/update_pvp_rank.inc"


#include "CUser/saveTaxMoneyForUpperMember.inc"


#include "CUser/gainExpAsUpperMember.inc"


#include "CUser/gainPowerWarRewardExp.inc"


#include "CUser/add_guild_point_item.inc"


#include "CUser/SendMoneyFullReason.inc"


#include "CUser/AddDungeonClear.inc"

#include "CUser/SendConditionEventInfo.inc"


#include "CUser/ResetCurCharacUsedGiftFatigueQuantity.inc"


#include "CUser/SetChangedGiftFatigueQuantity.inc"


#include "CUser/ResetDailyQuest.inc"


#include "CUser/ResetTrainingQuest.inc"


#include "CUser/SetPvpIndex.inc"


#include "CUser/add_inventory_item.inc"


#include "CUser/send_MissionList.inc"


#include "CUser/send_aura_avatar_option.inc"


// ===================== GameStubs 迁移（ORIG 强符号 T；最小正确语义） =====================
// 全部签名经 nm -C /home/wangyilei/下载/df_game_r 核实。

#include "CUser/DisConnSig.inc"


#include "CUser/givePvPSkillTree.inc"


// 0x0867a95c

#include "CUser/OnDisconnect.inc"


#include "CUser/SetCharacLevel.inc"


#include "CUser/master_new_skill.inc"


// 0x0867b048

#include "CUser/ChangeGrowType_GM.inc"


#include "CUser/AddItem.inc"


#include "CUser/LogoutCachedCharacter.inc"


#include "CUser/ReCalcChattingEmoticon.inc"


#include "CUser/ResetItemByScript.inc"


#include "CUser/SendChattingEmoticon.inc"


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

#include "CUser/UpdateData.inc"


#include "CUser/UpdateLogout.inc"


#include "CUser/deleteDailyStackableItem.inc"


#include "CUser/_checkTimeoutItem.inc"

#include "CUser/deleteSpecificItem.inc"


#include "CUser/doLinkCharacDisconnect.inc"


#include "CUser/ChangeCharacSlot.inc"


#include "CUser/setLinkCharacDisconnectFlag.inc"
{
    m_breakAway.m_field15 = flag;
}

// ---- UpdateCharacView 链（报告 docs/class_func_reports/CUser/UpdateCharacView.md、
// updateCharacViewVisibleData.md、checkRentedItem.md；ORIG 0x8651740/0x8651322/0x8677188）----

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

#include "CUser/checkRentedItem.inc"

struct ExpansionPair
{
    unsigned short m_a;                 // +0x00
    unsigned short m_b;                 // +0x02
};
// 循环 3 可见缓存元素（ORIG 0x86515b5 `mov 0xe(%eax,%edx,8)` scale：
// base=info+0xe，索引 k+0xea，stride 8，字段 +0/+4/+6）。
#include "CUser/updateCharacViewVisibleData.inc"
                                        struct CHARAC_LOGIN_DATA& data)
{
    memset(data.m_avatarSlots, 0, 0x60);
    if ((getCurCharacR() != 0 &&
         getCurCharacR()->m_characNo ==
             info.m_characNo)
            ? true : false)
    {
        data.m_renameOrViewState = getRequestRenameState();

        CInventory& inven = const_cast<_Charac_info&>(info).m_curCharacInven;
        // 槽 0..8：avatar 装备槽（有物品且扩展信息存在时写索引/颜色对）。
        for (int i = 0; i < 9; ++i)
        {
            const Inven_Item& slot = inven.m_invenItem[i];
            if (slot.m_addInfo != 0)
            {
                const void* expansion =
                    inven.GetAvatarItemMgrR()->GetExpansionInfo(slot.m_addInfo2);
                if (expansion != 0)
                {
                    if (inven.GetClearAvatar(i) != 0)
                        data.m_avatarSlots[i].m_itemIndex = inven.GetClearAvatar(i);
                    else
                        data.m_avatarSlots[i].m_itemIndex = slot.m_addInfo;
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
            const int itemIndex = (int)inven.m_invenItem[i].m_addInfo;
            if (checkRentedItem((unsigned int)info.m_characNo, itemIndex, 1) ||
                checkRentedItem((unsigned int)info.m_characNo, itemIndex, 2))
            {
                memset(&data.m_avatarSlots[i], 0, 8);
            }
            else
            {
                data.m_avatarSlots[i].m_itemIndex = itemIndex;
                data.m_avatarSlots[i].m_expansionA = 0;
                data.m_avatarSlots[i].m_expansionB = 0;
            }
        }
    }
    else
    {
        // 非当前角色：可见缓存 = CInventory::m_avatarVisible[12]
        for (int k = 0; k < 12; ++k)
        {
            const CInventory::AvatarVisibleData& vis =
                info.m_curCharacInven.m_avatarVisible[k];
            data.m_avatarSlots[k].m_itemIndex = vis.m_field0;
            data.m_avatarSlots[k].m_expansionA = vis.m_field4;
            data.m_avatarSlots[k].m_expansionB = vis.m_field6;
        }
        data.m_renameOrViewState =
            info.m_viewState;
    }
}

#include "CUser/UpdateCharacView.inc"


#include "CUser/giveup_panalty.inc"


#include "CUser/log_out.inc"


#include "CUser/prepareDisconnect.inc"


#include "CUser/send_itemspace.inc"


#include "CUser/send_ontime_reward_start_notify.inc"


#include "CUser/send_skill_info.inc"


// 0x0866cb04

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
                        mult = G_CDataManager()->m_serverParameter.m_expGroup388[count - 1];
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
#include "CUser/FatigueUp.inc"


#include "CUser/SendNotiPacket.inc"


#include "CUser/add_guild_pvp_result.inc"


#include "CUser/add_pvp_play_info.inc"


#include "CUser/add_pvp_result.inc"


#include "CUser/IsGuildSkillLearn.inc"


#include "CUser/gainGuildSkillExp.inc"


#include "CUser/make_charac_stat.inc"


#include "CUser/incPlayExpAdd.inc"


#include "CUser/gain_sp.inc"


#include "CUser/gain_sfp.inc"


#include "CUser/history_log_sp.inc"


#include "CUser/history_log_sfp.inc"


#include "CUser/Remove_Mission.inc"


#include "CUser/isCompetitionMercenary.inc"


#include "CUser/update_pvp_point.inc"

#include "CUser/resetDailyData.inc"


#include "CUser/RecoverCoin.inc"


#include "CUser/SendOpenflag.inc"


#include "CUser/RewardItem2DeleteInvalidItem.inc"


#include "CUser/AddDailyItem.inc"


#include "CUser/SendOneADayItemShopIndex.inc"


#include "CUser/SendUpdateItem.inc"


#include "CUser/UpdateAuraAvatarOption.inc"


#include "CUser/is_equip_aura_avatar.inc"


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

#include "CUser/reqHumanCertify4ClearMap.inc"


#include "CUser/master_new_skills.inc"


#include "CUser/ProcPremiumFatigue.inc"

#include "CUser/isSecurityServiceUsed.inc"


// ORIG W 0x0863bea2：return m_premium(+0x7121c).GetTotalPremiumCount() != 0
// （scout 报告误关联 0x867cd20 = _UpdateUserInfo4Guild，已按真实符号核实）。

#include "CUser/IsPremiumUser.inc"


// ============================================================================
// 租借物品序列化链（CUser::SaveInventory 直接依赖；m_rentalInfo@+0x6ef78 为
// map<uint, map<uint, vector<InstanceRentalSystem::RentalInfo>>>）。
// ============================================================================

#include "CUser/GetInstanceRentalInfo.inc"


#include "CUser/getCurrentCharacRentalInfo.inc"


#include "CUser/setCurrentCharacRentalInfo.inc"


#include "CUser/removeRentalItemInfoFromInvenBuf.inc"


#include "CUser/SaveInventory.inc"


#include "CUser/_postCheckForceChangeGrowType.inc"

#include "CUser/STGuildDBInfoOnly.inc"


// ===================== U1 批次（FixUndefU1）=====================

#include "CUser/makeGuildLevelUpMessage.inc"


#include "CUser/_UpdateUserInfo4Guild.inc"


#include "CUser/CacheCharacterMemory.inc"


// ============================================================================
// 角色内存快照填充（CacheCharacterMemory 依赖）——ORIG 报告逐函数实现。
// ============================================================================

#include "CUser/GetDungeonClear.inc"


#include "CUser/GetBlackListCount.inc"


// ============================================================================
// 角色内存快照填充：GetRentInfo/GetCharacInfo/GetInventoryInfo/GetTicket（CacheCharacterMemory 依赖）
// 及 helper 链（GetPcRoomRentInfo/GetDimensionRentInfo/getCharacInfoDetail/getTagCharacInfo/
// getTagCharacInventory/GetInventory/removeRentalItemInfoFromInvenLoadBuf）。
// 具名填充 SIG_LOAD_CHARAC/INVENTORY/TICKET（SigTypes.h 权威布局）。
// ============================================================================

#include "CUser/GetRentInfo.inc"


#include "CUser/GetPcRoomRentInfo.inc"


#include "CUser/GetDimensionRentInfo.inc"


#include "CUser/GetCharacInfo.inc"


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
#include "CUser/GetInventory.inc"


#include "CUser/GetInventoryInfo.inc"


#include "CUser/getTagCharacInventory.inc"


#include "CUser/removeRentalItemInfoFromInvenLoadBuf.inc"


#include "CUser/GetTicket.inc"


#include "CUser/getCharacInfoDetail.inc"


#include "CUser/getTagCharacInfo.inc"


// ORIG T 0x0864d646：CInventory/CCargo 序列化 helper（供 GetInventoryInfo）。

#include "CUser/GetRecord.inc"


#include "CUser/GetLimitItemUsage.inc"


#include "CUser/GetCharacCashAttendanceInfo.inc"


#include "CUser/getTotalPcRoomPlayTime.inc"

#include "CUser/getPcRoomPlayTimeRewardCnt.inc"

#include "CUser/GetCurCharacUsedGiftFatigueQuantity.inc"

#include "CUser/getFlagPresentAvengerTitle.inc"


#include "CUser/GetAuraAvatarOption.inc"


#include "CUser/GetCurCharacOption.inc"


#include "CUser/GetSkill.inc"


#include "CUser/get_skill_command_buf.inc"


#include "CUser/get_skill_command_buf_1.inc"


#include "CUser/GetEtcInfo.inc"


#include "CUser/setBit_Byte.inc"

#include "CUser/_SaveCharacData.inc"


#include "CUser/SetRestrictedGoods.inc"


#include "CUser/IsRestrictedGoods.inc"


#include "CUser/SendRestrictedGoods.inc"


#include "CUser/GetSpecialLevelCharacCnt.inc"


#include "CUser/HasCurCharacRentalItem.inc"


#include "CUser/IsGuildMaster.inc"


#include "CUser/IsSubGuildMaster.inc"


// ---------------- U1 批次第二段（deleteSpecific 域 / 杂项发送域）----------------

#include "CUser/_isMatchedItem.inc"


#include "CUser/_deleteSpecificItemInven.inc"


#include "CUser/_deleteSpecificItemCargo.inc"


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

#include "CUser/_deleteSpecificItemAccountCargo.inc"


#include "CUser/_deleteSpecificItemTitieBook.inc"


#include "CUser/DoGetGoldItem.inc"


#include "CUser/DoGetCeraPointItem.inc"


#include "CUser/delete_ontime_reward_list.inc"


#include "CUser/sendComboSkillList.inc"


#include "CUser/SendCoin.inc"


#include "CUser/SaveMoneyCoin.inc"


#include "CUser/SendEventMailAlaramTooltip.inc"


#include "CUser/giveLotteryCountItem.inc"


#include "CUser/SendMoenyLog.inc"


#include "CUser/SendLoginToPCRoomServer.inc"


#include "CUser/SendLogoutToPCRoomServer.inc"


#include "CUser/SendClientCrashDownLogToLogDB.inc"


#include "CUser/onUpdateHackCount.inc"


#include "CUser/setStdDropRate.inc"


// ============================================================================
// CUser 连续/冷却道具链簇（ORIG T，单函数还原见各 .md）
// ============================================================================

#include "CUser/_processCoolTimeItem.inc"


#include "CUser/_processEffectItem.inc"


#include "CUser/_processContinuousItem.inc"


#include "CUser/_processSpeicalContinuousTimeItem.inc"


#include "CUser/CheckCoolTimeItem.inc"


#include "CUser/UpdateCoolTimeItemInfo.inc"


#include "CUser/UpdateEffectItemInfo.inc"


#include "CUser/SendEventJarOutputItemStatistics.inc"


// ============================================================================
// ORIG 0x08692b84 CUser::SendAradEventItemLog(unsigned long, int)：
// "Arad_StatisticsEventItemList" 脚本 needStatistics(itemIndex) 命中时，
// 构造 16 字节 Sig{itemIndex,count,accId,characNo(-1)} 经内部流 0x20 投递
// LOGDB_Q。反编译见 docs/class_func_reports/CUser/SendAradEventItemLog.md。
// ============================================================================

#include "CUser/SendAradEventItemLog.inc"


// ============================================================================
// 任务 QP 发送链（RestoreUserQuestStubs：Dispatcher_Debug_Clear_Quest 依赖）
// ============================================================================

#include "CUser/sendCharacQp.inc"


#include "CUser/incQuestPoint.inc"


#include "CUser/RemoveRestrictedGoods.inc"


#include "CUser/SetNeedCheckOverEquipItem.inc"

// ============================================================================
// LinkMisc5 批次（散点链接期未定义符号，第四阶）
// ============================================================================

#include "CUser/isActivate.inc"


#include "CUser/isCertified.inc"


#include "CUser/getFailCnt.inc"


#include "CUser/isTradePunishType.inc"


#include "CUser/send_RedeemInfo.inc"


#include "CUser/calc_lev_stat.inc"


#include "CUser/CalcurateUserMaxLevel.inc"


#include "CUser/SetMercenaryInfoPacket.inc"

#include "CUser/SetUserMaxLevel.inc"


#include "CUser/SetManageLevel.inc"

#include "CUser/SetManagePoint.inc"


#include "CUser/CalcurateManagePoint.inc"


#include "CUser/CalcurateManageLevel.inc"


#include "CUser/SendManageLevelUpNoti.inc"


#include "CUser/ReturnToSelectCharacList.inc"


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

}

#include "CUser/UpdateCurCharacVisibleToCharacViewMemory.inc"

#include "CUser/reset_5.inc"


#include "CUser/check.inc"


#include "CUser/stHeader.inc"


// ============================================================================
// encryptSecurityCardTEA —— ORIG T 0x086805ef（nm size 0x117；user.cpp:32907，
// 报告缺失 ⇒ 按 §3.2 objdump 切片转写）。ORIG 该 TU 三函数地址相邻：
//   CUser::GetUserListByParty 0x0868056a / encryptSecurityCardTEA 0x086805ef /
//   CUser::sendBackSecurityAuthRequest 0x08680706（本仓该二者分居
//   W7_UserLinkTail.cpp / Sanicova.cpp；本件按任务书落 CUser.cpp）。
// 与 encryptAES 家族同骨架（strlen 换成形参副本），差异在收尾：
//   成功时只把 paddedLen 写回出参引用并返回 dst（不转 hex、无 '\0'）。
// logFormat 常量组：file = "user.cpp"（0x08cee1dd）、line = 0x808b = 32907、
//   func = __PRETTY_FUNCTION__（ORIG 局部串符号
//   _ZZ22encryptSecurityCardTEAP4CTEAPcS1_jRjE19__PRETTY_FUNCTION__ @0x08cf2740
//   r size 0x4f = "char* encryptSecurityCardTEA(CTEA*, char*, char*,
//   unsigned int, unsigned int&)"，变参首项再次传该串）、
//   末项 "Encrypt Error"（0x08cefeb7）、fmt "[%s][%d][%s]"（0x08cefec5）。
// 语义（ORIG 0x086805ef..0x08680705）：len 副本 → GetBlockSize(NULL) →
//   块对齐 paddedLen → blockSize>=0 时 Pad(src, len, NULL) → 虚槽 +0xc
//   Encrypt(src, dst, paddedLen) → 结果 != 0x6fffffff 即 logFormat 并返 NULL；
//   否则 *(unsigned int*)&outLength = paddedLen 且返回 dst。
// ============================================================================
char* encryptSecurityCardTEA(CTEA* tea, char* src, char* dst,
                             unsigned int length, unsigned int& outLength)
{
    int srcLen = length;
    int paddedLen = 0;
    int blockSize = tea->IMethod::GetBlockSize(NULL);
    if (blockSize >= 0)
    {
        if (srcLen % blockSize != 0)
        {
            paddedLen = (srcLen / blockSize + 1) * blockSize;
        }
        else
        {
            paddedLen = srcLen;
        }
    }
    else
    {
        paddedLen = srcLen;
    }
    if (blockSize >= 0)
    {
        tea->IMethod::Pad(src, srcLen, NULL);
    }
    int result = tea->Encrypt(src, dst, paddedLen);
    if (result != 0x6fffffff)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x808b,
            "[%s][%d][%s]", __PRETTY_FUNCTION__, 0x808b, "Encrypt Error");
        return NULL;
    }
    outLength = paddedLen;
    return dst;
}
