// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUser/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
#include "CUser/CDataMgr.inc"
}
// ---- G1-3 临时外部桩（对应 TU 翻译后移除） ----
// ORIG T 0x0808ca12：mov $0x7,%eax 直返 7
unsigned char _NS_PI_2ND_GetDefaultRandomHashKey() { return 7; }
// ORIG W 0x08695058：movl $0,0x10(%eax) —— 清 m_field10(+0x10)。
#include "CUser/resetExposedCount.inc"
extern void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user); // ORIG T 0x082751ee
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864809c | _ZN5CUser11AntibotSendER9PacketBuf
#include "CUser/AntibotSend.inc"
}

#include "CUser/_Quest_Authen_Data.inc"
#include "CUser/reset.inc"
}
// [b28d-Main·检查点18] 删除 stub `mission_list_is_clear`：ORIG 无该符号、全仓无调用点。

#include "CUser/reset_1.inc"
}

// ORIG C1 0x086940c6：仅 movb $0,(%eax)（清 m_pad[0] 首字节）。
#include "CUser/STGuildAgitDBInfo.inc"
}
#include "CUser/resetInformNoticeFlag.inc"
}

#include "CUser/disableSaveInformNoticeFlag.inc"
}

#include "CUser/enableSaveInformNoticeFlag.inc"
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
#include "CUser/makeRequest.inc"
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
#include "CUser/getCancelCnt.inc"
}


// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b4300 | _ZN20UserMercenaryInfoMgr9AddCharacER21CHARAC_LOAD_MERCENARY
#include "CUser/AddCharac.inc"
}
// ORIG T 0x081b441a：按 characNo 线性查找并 erase（ORIG 经 FindIndexByCharacNo，
// 内联等价）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b441a | _ZN20UserMercenaryInfoMgr12RemoveCharacEj
#include "CUser/RemoveCharac.inc"
}
using expert_job::CCharacExpertJob;

// ORIG 0x0832ac48 T：遍历 0x14 个 CData* 槽，非空则经 vtable+0x14 槽
//（CData 虚表 D0/deleting 析构）删除对象。
#include "CUser/dtor_CDataMgr.inc"
}
#include "CUser/CharacAdvanceAltarManager.inc"
#include "CUser/dtor_CharacAdvanceAltarManager.inc"
#include "CUser/PIReceiverManager.inc"
#include "CUser/dtor_PIReceiverManager.inc"
#include "CUser/PISenderManager.inc"
#include "CUser/dtor_PISenderManager.inc"
#include "CUser/TurnStomach.inc"
}
// ORIG T 0x0832ac94：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 GetDataR 同形（ORIG 两函数字节一致）；enum 底层无符号 → jbe。
#include "CUser/GetData.inc"
}
// ORIG T 0x0832acb2：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 update/ResetDaily 一致的 CData*[0x14] 槽解释；enum 底层无符号 → jbe。
#include "CUser/GetDataR.inc"
}
// ORIG T 0x0832ad78：对 20 个 CData* 槽（+0x00..+0x4c）依次调用非虚 CData::reset。
#include "CUser/reset_2.inc"
}

// ORIG 0x0832ade8：对 20 个成员（+0x00..+0x4c，各为 charac_expand::CData*）
// 依次调用虚表槽 0（ResetDailyMidnight）。
// ORIG 0x0832adaa：同上，调用虚表槽 3（ResetDaily）。
#include "CUser/ResetDailyMidnight.inc"
}

#include "CUser/ResetDaily.inc"
}

// ORIG T 0x0832acd0：对 20 个 CData* 槽（m_pad）isSave→saveData 全链；任一失败返回 0。
#include "CUser/update.inc"
}

#include "CUser/update_1.inc"
}
// ORIG C1 0x08697068：清 byte+0、int+4/+8/+0xc、short+0x10/+0x12。
#include "CUser/GrowthWeaponEvent.inc"
}
// ORIG C1 0x08695ad0：基类 C1（同 this）后置 +0x38/+0x39/+0x3a=1。
#include "CUser/Secu_HackLogCheck.inc"
}
// ORIG C1 0x08695ab0：清 +4/+5/+6（m_active223/224/225）。
// 定义挪至 Secu_HackLogCheck C1 之后（技巧 31 规则2：callee 同 TU 前置定义
// 会收紧 caller 帧；挪后 caller 0x8695ad3 ORIG sub $0x18 宽松帧恢复）。
#include "CUser/Secu_MonsterCheck.inc"
}
// ORIG C1 0x08694f68：构造 3 个 user_gold_info 子对象（+0x04/+0x40/+0x7c）。
#include "CUser/Secu_GoldControl.inc"
}
// ORIG C1 0x0848683c：IData C1 + PlayInfo C1（基类）+ Character::Reset()（0x08486818
// = PlayInfo::Reset() + movl $0,0x3c）。ORIG 不初始化 Character 自身字段。
#include "CUser/Character.inc"
}
// ORIG W 0x080cbc6c：movl $0,0x0 + movb $0,0x4/0x5/0x6 —— 清 eventId/m_pad4/isWin/needSave。
// ORIG C1 0x08697128：movl $0,+0 / movb $0,+4/+5/+6（m_eventId/m_pad4/m_isWin/m_needSave）。
#include "CUser/BingoData.inc"
}
#include "CUser/clear.inc"
}
#include "CUser/GrowthCreatureEvent.inc"
}
#include "CUser/CharacLevelUpGiftAccountOnce.inc"
}
// ORIG C1 0x084866d4：仅 call PlayInfo::Reset()。
#include "CUser/PlayInfo.inc"
}
// ORIG W 0x08486818：PlayInfo::Reset() 后清 +0x3c。
#include "CUser/Reset.inc"
}
// 定义挪至 Character::Reset 之后（技巧 31 规则2：恢复 caller 0x848681b
// ORIG sub $0x18 宽松帧；PlayInfo 自身 ctor 0x84866d4 亦在定义之前，不受影响）。
#include "CUser/Reset_1.inc"
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
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864ef9c | _ZN5CUserD1Ev
#include "CUser/dtor_CUser.inc"
}

#include "CUser/reset_3.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865851c | _ZN5CUser14ResetCurCharacEv
#include "CUser/ResetCurCharac.inc"
}

#include "CUser/reset_server_fatigue_data.inc"
}

// ORIG 0x0818d3fe：在 map<uchar,short>(+0x8eb50) 中按 type 查疲劳值，未命中返回 0。
#include "CUser/get_server_fatigue_count.inc"
}

// ORIG 0x0818d4c4：若当前疲劳 DB 类型(+0x8eb4d) == type 返回 (+0x8eb4c) 否则 0。
#include "CUser/get_fatigue_count_db.inc"
}

#include "CUser/reset_ingame_event_history.inc"
}

#include "CUser/reset_ingame_event_history_update.inc"
}

#include "CUser/reset_pvp_masterid_walkingout_me.inc"
}

#include "CUser/set_guildwar_point_per_pvpplay.inc"
}

#include "CUser/get_guildwar_point_per_pvpplay.inc"
}

#include "CUser/getHades.inc"
}

#include "CUser/GetPVPRoom.inc"
}

// ORIG T 0x0868860a：转发 party->SendSecretShopItemList(this)（声明 CUser.h:3432）
#include "CUser/SendSecretShopItemList.inc"
}

#include "CUser/GetPICSMap.inc"
}

#include "CUser/CheckMoney.inc"
}

#include "CUser/getDungeonIdxAfterClear.inc"
}

#include "CUser/resetMoneyLog.inc"
}

#include "CUser/ResetDBLogItem.inc"
}

#include "CUser/DBUpdateDBLogItem.inc"
}

#include "CUser/GetParty.inc"
}

// CUser::GetUserPosInParty @ 0x08688638 —— 返回 own 在队伍中的槽位，不在返回 -1。
#include "CUser/GetUserPosInParty.inc"
}

#include "CUser/GetWarRoom.inc"
}

// ORIG T 0x0864df12：遍历 m_characList（this+0x796e8），首字段 m_characNo
// 与 characNo 相等则返回下标，否则返回 -1。
#include "CUser/get_charac_idx.inc"
}
#include "CUser/get_charac_no.inc"
}

#include "CUser/CheckItemLock.inc"
}

#include "CUser/getCurCharacTotalFatigue.inc"
}

#include "CUser/getCurCharacTotalMaxFatigue.inc"
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
}

#include "CUser/checkSpecialCharName.inc"
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
}
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865dd14 | _ZN5CUser10send_equipEi
#include "CUser/send_equip.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x086893ac | _ZN5CUser12AddDBLogItemEjj20ENUM_DBLOG_ITEM_TYPE
#include "CUser/AddDBLogItem.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681224 | _ZN5CUser13addAvatarItemEmccPKc20eAvatarItemAddReason
#include "CUser/addAvatarItem.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c65a | _ZN5CUser18SendUpdateItemListENS_11eSendTargetE14ENUM_ITEMSPACEi
#include "CUser/SendUpdateItemList.inc"
}

// 0x0868bc7c
// ORIG：无角色 → -1；无专家职业 → 0；否则按 GetExpertJobScript 的
// isBoundaryExpValue/GetLevel（_ZN17STExpertJobScript*）计算等级，
// 边界值时用 ExpertJobEtcScript 内 map<uchar,short>（等级→所需角色等级）修正：
// 查不到或角色等级不足 → 等级 -1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868bc7c | _ZN5CUser20GetCurExpertJobLevelEi
#include "CUser/GetCurExpertJobLevel.inc"
}

#include "CUser/is_clear_stealingSkillMission.inc"
}

// ORIG T 0x08679736：公会技能公告。文案 = ServerString(0x3ea) + "%d "(count) +
// skillName + ServerString(0x3eb)；总长 ≥0x78 丢弃；公会公告板写入
// (boardNo=0, charNo, job=-1, growType=-1, guildKey)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679a36 | _ZN5CUser21makeGuildSkillMessageEPKci
#include "CUser/makeGuildSkillMessage.inc"
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
}

// ---- 基础访问器（G1-3 桩替换） ----
#include "CUser/get_acc_id.inc"
}

#include "CUser/get_acc_name.inc"
}

#include "CUser/get_state.inc"
}

#include "CUser/set_state.inc"
}

#include "CUser/GetUID.inc"
}

#include "CUser/get_unique_id.inc"
}

#include "CUser/getDungeonGainedGold.inc"
}

#include "CUser/getHackAnalyzer.inc"
}

#include "CUser/getGmQuestFlag.inc"
}

#include "CUser/GetAccountLastPlayTime.inc"
}

#include "CUser/GetServerGroup.inc"
}

#include "CUser/GetCharacExpandData.inc"
}

#include "CUser/GetCharacExpandDataR.inc"
}


// ---- quest point / piece helpers（ORIG：CUser 0x868ad88 / 0x868afd0 / 0x868adc2 / 0x868af66）----

#include "CUser/clearInitQuestPoint.inc"
}

#include "CUser/clearQuestPiece.inc"
}

#include "CUser/incInitQuestPoint.inc"
}

#include "CUser/incQuestPiece.inc"
}

#include "CUser/gainWinPoint.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868adfc | _ZN5CUser17setInitQuestPointEv
#include "CUser/setInitQuestPoint.inc"
}

#include "CUser/GetPremiumInfo.inc"
}

// ORIG W 0x0864f8e0：mov 0x854(%eax),%eax 直读 +0x854（outPremiumCount）。
#include "CUser/GetAdvantageCoin.inc"
}

// ORIG 0x08694a54：movzbl 0x85c(%eax),%eax 直读 +0x85c（overEquipFlag byte）。
#include "CUser/isAffectedUnlimitFatigue.inc"
}

#include "CUser/getCurCharacQuestR.inc"
}

// ORIG W 0x0814aa5e：enableSaveQuest() 后返回 m_quest（+0x858d4）。
#include "CUser/getCurCharacQuestW.inc"
}

#include "CUser/DecreaseGuildPowerWarPoint.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867ed68 | _ZN5CUser11is_fightingEv
#include "CUser/is_fighting.inc"
}

#include "CUser/set_before_area.inc"
}

#include "CUser/set_area.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086813be | _ZN5CUser8get_areaEb
#include "CUser/get_area.inc"
}

#include "CUser/get_pvp_WinningRate_relateMission.inc"
}

#include "CUser/get_aura_avatar_option_value.inc"
}

#include "CUser/SetTradeSpace.inc"
}

#include "CUser/CheckFatigue.inc"
}

// ORIG T 0x0864f7fe sz0xeb：扣除 1 点疲劳（先消耗高级会员疲劳额度）。
//   min(premiumFatigue, maxPremiumFatigue) → 不足时回落到普通疲劳。
// b28d-lane5 N封顶: N97/P230 cmov; N82/P112 槽号分配 | ORIG 0x0864f7fe | _ZN5CUser20gainCurCharacFatigueEi
#include "CUser/gainCurCharacFatigue.inc"
}

#include "CUser/has_within_Mission.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086552e6 | _ZN5CUser13getBlueMarbleEv
#include "CUser/getBlueMarble.inc"
}

#include "CUser/checkLogOutCorrectly.inc"
}

#include "CUser/find_pvp_masterid_walkingout_me.inc"
}

#include "CUser/insert_pvp_masterid_walkingout_me.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865cfac | _ZN5CUser20update_old_pvp_pointEv
#include "CUser/update_old_pvp_point.inc"
}

#include "CUser/SetSaveRentalInfoToExchange.inc"
}

#include "CUser/DeleteRentalItemInfo.inc"
}


#include "CUser/isAffectedPremium.inc"
}

#include "CUser/IsPermissionPrivateStore.inc"
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
#include "CUser/RecoverFatigue.inc"
}

#include "CUser/LogHistory.inc"
}

#include "CUser/isBlackUser.inc"
}

#include "CUser/IsHavePremiumAdvantage.inc"
}

// b28d-solo-CUser AE: ORIG 0x0867?? `call; test %eax,%eax; setne %al; test %al,%al; je`
//   = 条件内联形（探针 /tmp/probe_bool.cpp a3/a5 逐条同形）；具名 bool 局部 4.4 恒
//   落槽（a2/a4：mov %al,-0x9; cmpb $0,-0x9）⇒ 删局部内联即归位。| _ZN5CUser13IsEquipAvatarEv
#include "CUser/IsEquipAvatar.inc"
}

#include "CUser/getWebAddress.inc"
}

#include "CUser/getSecurityCard.inc"
}

#include "CUser/GetGameOptionRef.inc"
}

#include "CUser/getPad.inc"
}

#include "CUser/getBeforeAreaTime.inc"
}

#include "CUser/setBeforeAreaTime.inc"
}

#include "CUser/getLastPacketID.inc"
}

#include "CUser/setLastPacketID.inc"
}

#include "CUser/getPlayingDungeonIndex.inc"
}

#include "CUser/setPlayingDungeonIndex.inc"
}

#include "CUser/getMoveSpace.inc"
}

#include "CUser/setMoveSpace.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08678526 | _ZN5CUser13CheckMoveTownEi
#include "CUser/CheckMoveTown.inc"
}

#include "CUser/isUseCraneStart.inc"
}

#include "CUser/setUseCraneStart.inc"
}

#include "CUser/getForceDropFlag.inc"
}

#include "CUser/setForceDropFlag.inc"
}

#include "CUser/isDungeonClearHackCheck.inc"
}

#include "CUser/setDungeonClearHackCheck.inc"
}

#include "CUser/isAllowLocationGeoState.inc"
}

#include "CUser/setAllowLocationGeoState.inc"
}

#include "CUser/isComboSkillSet.inc"
}

#include "CUser/setComboSkillSet.inc"
}

#include "CUser/lock4DataLoad.inc"
}

#include "CUser/unlock4DataLoad.inc"
}

#include "CUser/lockCreateCharac.inc"
}

#include "CUser/unlockCreateCharac.inc"
}

#include "CUser/lockDeleteCharac.inc"
}

#include "CUser/unlockDeleteCharac.inc"
}

#include "CUser/isLocked4DataLoad.inc"
}

#include "CUser/isLockedCreateCharac.inc"
}

#include "CUser/isLockedDeleteCharac.inc"
}

#include "CUser/get_growth_power_exp_reward.inc"
}

#include "CUser/setAntibotKey.inc"
}

#include "CUser/setClientVersion.inc"
}

#include "CUser/getClientVersion.inc"
}

#include "CUser/ResetUsedCoinInDungeon.inc"
}

#include "CUser/ClearItemSellCount.inc"
}

// ---- 发包族（PacketGuard 局部 + InterfacePacketBuf） ----
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086485ba | _ZN5CUser4SendER11PacketGuard
#include "CUser/Send.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867b8fe | _ZN5CUser10SendPacketENS_11eSendTargetER11PacketGuard
#include "CUser/SendPacket.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c1d4 | _ZN5CUser13SendCmdPacketENS_11eSendTargetE14ENUM_CMDPACKETi
#include "CUser/SendCmdPacket.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bea0 | _ZN5CUser15SendCmdOkPacketE14ENUM_CMDPACKET
#include "CUser/SendCmdOkPacket.inc"
}

#include "CUser/SendCmdErrorPacket.inc"
}

// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步（ORIG 尾 call Send 直返）| ORIG 0x0867c022 | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKETh8PAY_TYPE
#include "CUser/SendCmdErrorPacket_1.inc"
}

// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步 | ORIG 0x0867bfae | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKEThR11PacketGuard
#include "CUser/SendCmdErrorPacket_2.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c110 | _ZN5CUser23SendCmdErrorPacket2ByteE14ENUM_CMDPACKETt
#include "CUser/SendCmdErrorPacket2Byte.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x08692824 | _ZN5CUser21SendCmdErrorPacketJpnE14ENUM_CMDPACKETt
#include "CUser/SendCmdErrorPacketJpn.inc"
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
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bd84 | _ZN5CUser22SendCmdPeerErrorPacketE14ENUM_CMDPACKET22ENUM_PEER_REQUEST_TYPEh
#include "CUser/SendCmdPeerErrorPacket.inc"
}

#include "CUser/SendCmdPeerErrorPacket_1.inc"
}

#include "CUser/SendZeroExpPacket.inc"
}

#include "CUser/SendMaxEquipLevel.inc"
}

#include "CUser/SendFatigue.inc"
}

// ============================================================================
// G1-4 批次：索引 / 状态访问器（ORIG 逐符号转录）
// ============================================================================

#include "CUser/CheckInTrade.inc"
}

#include "CUser/isEnableAvatarSocketAction.inc"
}

#include "CUser/CheckInParty.inc"
}

#include "CUser/checkInBlueMarble.inc"
}

#include "CUser/CheckInWarRoom.inc"
}

#include "CUser/checkInDeathTower.inc"
}

#include "CUser/checkInBossTower.inc"
}

#include "CUser/checkInAdvanceAltar.inc"
}

#include "CUser/getAdvanceAltarIndex.inc"
}

#include "CUser/getBlueMarbleIndex.inc"
}

#include "CUser/setBlueMarbleIndex.inc"
}
#include "CUser/getDeathTowerIndex.inc"
}

#include "CUser/setDeathTowerIndex.inc"
}

#include "CUser/getDeathTower.inc"
}

// ---- Redeem_Item::CRedeemItem（GetCharacExpandData(6) 返回，未建模为
// CExpandEquipslot* 槽位；权威声明见 Redeem_Item__CRedeemItem.h，实现于
// Redeem_Item__CRedeemItem.cpp，ORIG 0x085f76f0 / 0x085f71a8）----

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086472c0 | _ZN5CUser14Add_RedeemInfoERK10Inven_Itemib
#include "CUser/Add_RedeemInfo.inc"
}

#include "CUser/getBossTowerIndex.inc"
}

#include "CUser/setBossTowerIndex.inc"
}

#include "CUser/GetPartyIndex.inc"
}

#include "CUser/CheckInPvp.inc"
}

#include "CUser/GetPvpIndex.inc"
}

#include "CUser/GetWarRoomIndex.inc"
}

#include "CUser/GetTradeSpace.inc"
}

#include "CUser/SetPartyIndex.inc"
}

#include "CUser/SetWarRoomIndex.inc"
}

#include "CUser/SetSlotIDX.inc"
}

#include "CUser/GetIncreID.inc"
}

#include "CUser/SetIncreID.inc"
}

#include "CUser/set_unique_id.inc"
}

#include "CUser/set_position.inc"
}

#include "CUser/get_posX.inc"
}

#include "CUser/get_posY.inc"
}

#include "CUser/get_direction.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x082f0e70 | _ZN5CUser12GetUserStateEv
#include "CUser/GetUserState.inc"
}

#include "CUser/GetAge.inc"
}

#include "CUser/IsOverBirthDay.inc"
}

#include "CUser/IsProgLogout.inc"
}

#include "CUser/GetSeedFromDate.inc"
}

#include "CUser/SetGMUpgradeMode.inc"
}

#include "CUser/isGMUser.inc"
}

#include "CUser/IsGameMasterMode.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086796dc | _ZN5CUser17SetGameMasterModeEb
#include "CUser/SetGameMasterMode.inc"
}
#include "CUser/GetEventCreateDnfReward.inc"
}

#include "CUser/SetETC.inc"
}

// ============================================================================
// G1-4 批次：CodeHackCheckStorage / 返回用户 / 预登录数据
// ============================================================================

#include "CUser/setTournamentAccount.inc"
}

#include "CUser/isTournamentAccount.inc"
}

#include "CUser/setJoinedSchoolPointEvent.inc"
}

#include "CUser/setOnlinePreliminaryServerGroup.inc"
}

#include "CUser/setOnlinePreliminaryCharacNo.inc"
}

#include "CUser/setOnlinePreliminaryPlayType.inc"
}

#include "CUser/getOnlinePreliminaryPlayType.inc"
}

#include "CUser/getOnlinePreliminaryCharacNo.inc"
}

#include "CUser/SetPCRoomNo.inc"
}

#include "CUser/SetFirstLogin.inc"
}

#include "CUser/SetReturnUser.inc"
}

#include "CUser/ResetReturnUser.inc"
}

#include "CUser/IsReturnUser.inc"
}

#include "CUser/SetReturnUserFirstUserLogin.inc"
}

#include "CUser/IsReturnUserFirstLogin.inc"
}

#include "CUser/GetReturnUserExpireTime.inc"
}

#include "CUser/SetReturnUserExpireTime.inc"
}

#include "CUser/EnableSaveMemberBonusFatigue.inc"
}

#include "CUser/incAccountMemberBonusFatigue.inc"
}

#include "CUser/resetAccountMemberBonusFatigue.inc"
}

#include "CUser/getAccountMemberBonusFatigue.inc"
}

#include "CUser/resetAccountUsedFatigue.inc"
}

#include "CUser/incChattingMessageCount.inc"
}

#include "CUser/setChattingMessageCount.inc"
}

#include "CUser/getChattingMessageCount.inc"
}

#include "CUser/SetGameMasterCharacter.inc"
}

#include "CUser/getStdDropRate.inc"
}

#include "CUser/getAcccountHackInfo.inc"
}

#include "CUser/setHackUserTimer.inc"
}

#include "CUser/setHackUserFlag.inc"
}

// P292/T3：ORIG W 0x0822fd3c（0xf）；消费点 0x081be54c 无 setcc ⇒ 返回 bool。
#include "CUser/isHackUser.inc"
}

#include "CUser/isHackUserTimer.inc"
}

// ============================================================================
// G1-4 批次：突破奖励 / 账号货物 / 商城 / 日常
// ============================================================================

#include "CUser/STBreakAwayRewards.inc"
}

#include "CUser/clear_1.inc"
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
}

#include "CUser/setBreakAwayRewardItemId.inc"
}

#include "CUser/getBreakAwayRewardAItemCnt.inc"
}

#include "CUser/setBreakAwayRewardAItemCnt.inc"
}

#include "CUser/getUserBreakAwayRewardLuckPoint.inc"
}

#include "CUser/setUserBreakAwayRewardLuckPoint.inc"
}

#include "CUser/getBreakAwayRewardItemOrder.inc"
}

#include "CUser/getBreakAwayRewards.inc"
}

#include "CUser/setBreakAwayRewards.inc"
}

#include "CUser/getBreakAwayQuestTime.inc"
}

#include "CUser/setBreakAwayQuestTime.inc"
}

#include "CUser/getBreakAwayDungeonClearCnt.inc"
}

#include "CUser/setBreakAwayDungeonClearCnt.inc"
}

#include "CUser/subBreakAwayDungeonClearCnt.inc"
}

#include "CUser/setBreakAwayDungeonClear.inc"
}

#include "CUser/isBreakAwayRewardData.inc"
}

#include "CUser/setGmQuestFlag.inc"
}

#include "CUser/setWebAddress.inc"
}


#include "CUser/SetCera.inc"
}

#include "CUser/UseCera.inc"
}

#include "CUser/setRegDate.inc"
}

#include "CUser/setRegTime.inc"
}

#include "CUser/getRegTime.inc"
}

#include "CUser/resetUsedCera.inc"
}

#include "CUser/addUsedCera.inc"
}

#include "CUser/getUsedCera.inc"
}

#include "CUser/SetVillageAttackedReward.inc"
}

#include "CUser/setPcRoomAuth.inc"
}

#include "CUser/fillUserInfo4Antibot.inc"
}

#include "CUser/getUserInfo4Antibot.inc"
}

#include "CUser/SetAccountLastPlayTime.inc"
}

#include "CUser/SetLoginTime.inc"
}

#include "CUser/GetLoginTime.inc"
}

#include "CUser/SetLogInOutState.inc"
}

#include "CUser/GetLogInOutState.inc"
}

#include "CUser/get_public_ip_address.inc"
}

#include "CUser/set_public_ip_address.inc"
}

#include "CUser/set_local_ip_address.inc"
}

#include "CUser/setHangameUser.inc"
}

#include "CUser/isHangameUser.inc"
}

#include "CUser/IsPowerUp.inc"
}

#include "CUser/SetPowerUp.inc"
}

#include "CUser/SetRating.inc"
}

#include "CUser/SetSchoolNo.inc"
}

#include "CUser/GetSchoolNo.inc"
}

#include "CUser/SetSchoolName.inc"
}

#include "CUser/GetTutorialSkipable.inc"
}

#include "CUser/SetTutorialSkipable.inc"
}

#include "CUser/getHumanCertifyTimerKey.inc"
}

#include "CUser/setHumanCertifyErrorCnt.inc"
}

#include "CUser/isSendMailCertified.inc"
}

#include "CUser/setRestingUserRestrict.inc"
}

#include "CUser/setPunishTradeAlert.inc"
}

#include "CUser/getPunishTradeAlert.inc"
}

#include "CUser/resetDungeonClearHackCheck.inc"
}

#include "CUser/IsRecvEvent.inc"
}

#include "CUser/GetAutoClientRate.inc"
}

#include "CUser/isARSUserKick.inc"
}

#include "CUser/setSecuReward.inc"
}

#include "CUser/NeedSecuReward.inc"
}

#include "CUser/setSecuType.inc"
}

#include "CUser/getSecuType.inc"
}

#include "CUser/getRewardType.inc"
}

#include "CUser/setRewardType.inc"
}

#include "CUser/setSecuGrade.inc"
}

#include "CUser/setFlagPresentAvengerTitle.inc"
}

#include "CUser/incCharacSlotLimit.inc"
}

// ORIG 0x08230860 (W)：movzbl +0x8d245（m_ceraUserInfo.m_field61）
#include "CUser/getCharacSlotLimit.inc"
}

// ORIG 0x08230870 (W)：movzbl +0x8d244（m_ceraUserInfo.m_field60）
#include "CUser/getSlotEffectCount.inc"
}

// ORIG 0x08230880 (W)：写 +0x8d244（m_ceraUserInfo.m_field60）
#include "CUser/setSlotEffectCount.inc"
}

// ORIG 0x08676826 (T)：组 Packet_Randombox_statistic（header 0x17b6/0xc，boxKind@+0xa、
// actionKind@+0xb）经 CStatisticServerProxy::SendPacket(pkt, 0xc) 发统计服务器（组 0）。
#include "CUser/SendRandomBoxStatistic.inc"
}

// ORIG 0x0868f6bc (T)：find_item(itemIdx)→verifyStackableItemProcess
#include "CUser/verifyStackableItemActionPlace.inc"
}

// ORIG 0x0868f60e (T)：is_stackable→verifyStackableAction→verifyUsablePlace→verifyUsableServerType
#include "CUser/verifyStackableItemProcess.inc"
}

#include "CUser/set_ontime_last_recv_idx.inc"
}

#include "CUser/get_ontime_last_recv_idx.inc"
}

#include "CUser/set_update_ontime_last_recv_idx.inc"
}

#include "CUser/setCheckPickUpRandomOptionItem.inc"
}

#include "CUser/getCheckPickUpRandomOptionItem.inc"
}

#include "CUser/set_cera_item_order_no.inc"
}

#include "CUser/get_cera_item_order_no.inc"
}

#include "CUser/setPcRoomPlayStartTime.inc"
}

#include "CUser/IsDeleteEventDungeonInvitation.inc"
}

#include "CUser/IsUsedCoinInDungeon.inc"
}

#include "CUser/ResetCoinCount.inc"
}

#include "CUser/GetUsedCoinCount.inc"
}

// ORIG W 0x082604ec _ZNK5CUser21getCurCharacQuestRefR Ev（const）
// 返回 m_quest（+0x858d4）引用指针；与 getCurCharacQuestR 同址、独立符号。
#include "CUser/getCurCharacQuestRefR.inc"
}

// ORIG W 0x0830edaa _ZN5CUser36getAddIndependentDropRateFromPremiumE17ENUM_PREMIUM_TYPEj
// 转发 m_premium（+0x7121c）独立掉落率查询（参数直传，ORIG 无交换）。
#include "CUser/getAddIndependentDropRateFromPremium.inc"
}

// ORIG T 0x08689576 _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE
// 事件地下城清剿奖励发放：逐 (itemId,count)；find_item 失败直接结束；
// 背包无该物品 → make_item 构造后 insertItemIntoInventory（reason 13，log=1，
// stackable=false）；已有 → ItemAdd 日志 + 数量累加 update_item；
// 成功落位后 SendUpdateItemList(1, 0, slot)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689576 | _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/EventDungeonClearReward.inc"
}

#include "CUser/set_trick_area.inc"
}

#include "CUser/get_before_area.inc"
}

// ============================================================================
// G1-4 批次：锁 / 账号货物 / 其余访问器
// ============================================================================

#include "CUser/GetAccountCargo.inc"
}

#include "CUser/IsExistAccountCargo.inc"
}

#include "CUser/DeleteCargo.inc"
}

#include "CUser/lock.inc"
}

#include "CUser/unlock.inc"
}

#include "CUser/getBackupSeed.inc"
}

#include "CUser/getPowerSide.inc"
}

#include "CUser/getClientHWSpec.inc"
}

#include "CUser/SetGuildDBInfo.inc"
}

#include "CUser/ChangeGuildName.inc"
}

#include "CUser/SetGuildAgitFlag.inc"
}

#include "CUser/SetGuildAgitDBInfo.inc"
}

#include "CUser/SetGuildCargoCapacity.inc"
}

#include "CUser/SetTodayGuildMember.inc"
}

#include "CUser/IsTodayGuildMember.inc"
}

#include "CUser/SetLastLoginCharacNo.inc"
}

#include "CUser/SetLastLoginChannelNo.inc"
}

#include "CUser/SetValidLastLoginCharac.inc"
}

#include "CUser/SetReportMannerlessUserCount.inc"
}

#include "CUser/GetChecksumAnswerCount.inc"
}

#include "CUser/IncChecksumAnswerCount.inc"
}

#include "CUser/GetCheckConnInterval.inc"
}

#include "CUser/SetResumeChecksumTime.inc"
}

#include "CUser/incre_check_count.inc"
}

#include "CUser/get_connect_count.inc"
}

#include "CUser/setTotalHackUserTradeGold.inc"
}

#include "CUser/setTotalHackUserTradeCnt.inc"
}

#include "CUser/SetRevivalTime.inc"
}

#include "CUser/GetRevivalTime.inc"
}

#include "CUser/SetUseAPCPotionTime.inc"
}

#include "CUser/GetUseAPCPotionTime.inc"
}

#include "CUser/SetDailyBadge.inc"
}

#include "CUser/SetUserDetailInfo.inc"
}

// ============================================================================
// G1-4 批次：角色列表 / 视图
// ============================================================================

#include "CUser/get_charac_name.inc"
}

#include "CUser/GetCharacJob.inc"
}

#include "CUser/getCharacLevelByCharacNo.inc"
}

#include "CUser/get_charac_name_by_charac_no.inc"
}

#include "CUser/setCharacName.inc"
}

#include "CUser/SetCharacViewState.inc"
}

#include "CUser/SetCharacViewState_1.inc"
}

#include "CUser/GetCharacViewState.inc"
}

#include "CUser/SetCharacViewStateByCharacNo.inc"
}

// ============================================================================
// 批次 A 访问器（ORIG 反汇编逐偏移补全：0x8101048 / 0x82301ae / 0x827565a /
// 0x85a932a / 0x868a99a / 0x868fa46 / 0x86912b0 / 0x8691cd0 / 0x8692af6 等）
// ============================================================================

#include "CUser/sendEventInfo.inc"
}

#include "CUser/questAutoClear.inc"
}

#include "CUser/_onCompoundItem.inc"
}

#include "CUser/_updateNPCRelationship.inc"
}

#include "CUser/processNPCGiftOnLevelUp.inc"
}

#include "CUser/GetUserName.inc"
}

#include "CUser/GetUserEMail.inc"
}

#include "CUser/GetSsnString.inc"
}

#include "CUser/getSex.inc"
}

#include "CUser/GetCSHashSet.inc"
}

#include "CUser/GetGuildName.inc"
}

#include "CUser/GetGuildLevel.inc"
}

#include "CUser/GetHackCheck.inc"
}

#include "CUser/getBingoData.inc"
}

#include "CUser/GetGuildDBInfo.inc"
}

#include "CUser/getGarenaAuthData.inc"
}

#include "CUser/GetPISenderManager.inc"
}

#include "CUser/GetPIReceiverManager.inc"
}

#include "CUser/getGrowthWeaponEventdata.inc"
}

#include "CUser/get_ingame_event_history.inc"
}

#include "CUser/get_ingame_event_history_update.inc"
}

#include "CUser/getGrowthCreatureEventdata.inc"
}

#include "CUser/get_server_fatigue_day_data.inc"
}

#include "CUser/getCharacLevelUpGift_AccountOnce.inc"
}

#include "CUser/GetMailBox.inc"
}

#include "CUser/GetMileage.inc"
}

#include "CUser/GetPCRoomNo.inc"
}

#include "CUser/GetCeraPoint.inc"
}

#include "CUser/getAntibotKey.inc"
}

#include "CUser/GetManageLevel.inc"
}

#include "CUser/GetManagePoint.inc"
}

#include "CUser/GetUserMaxLevel.inc"
}

#include "CUser/onDungeonClear.inc"
}

#include "CUser/GetDebugCommand.inc"
}

#include "CUser/GetTradePunishType.inc"
}

#include "CUser/getLastLotteryTime.inc"
}

#include "CUser/GetNonClientRandInt.inc"
}

#include "CUser/GetNonClientFlag.inc"
}

#include "CUser/GetLastLoginCharacNo.inc"
}

#include "CUser/GetLastLoginChannelNo.inc"
}

#include "CUser/IsValidLastLoginCharac.inc"
}

#include "CUser/getRecipeProbability.inc"
}

#include "CUser/get_local_ip_address.inc"
}

#include "CUser/GetGuildCargoCapacity.inc"
}

#include "CUser/GetGuildPowerWarPoint.inc"
}

#include "CUser/GetResumeChecksumTime.inc"
}

#include "CUser/GetGameMasterCharacter.inc"
}

#include "CUser/getBreakAwayAccureCera.inc"
}

#include "CUser/getBreakAwayRewardOrder.inc"
}

#include "CUser/get_multiboxLotteryItemFailCnt.inc"
}

#include "CUser/GetCurCharacUsedFatigueQuantity.inc"
}

#include "CUser/isHumanCertified.inc"
}

#include "CUser/isLoadingHackGold.inc"
}

#include "CUser/isSaveInformNoticeFlag.inc"
}

#include "CUser/CheckMaxLuckyLevel.inc"
}

// ---- 批次 A 第 2 组（ORIG 0x82300e8 / 0x82301d8 / 0x8230246 / 0x8692a08 /
// 0x8692b74 / 0x86971ce / 0x86975b6 / 0x8568e48 等） ----

#include "CUser/OnRecvEvent.inc"
}

#include "CUser/GetBuyingGold.inc"
}

#include "CUser/SetBuyingGold.inc"
}

#include "CUser/GetFirstLogin.inc"
}

#include "CUser/SetProgLogout.inc"
}

#include "CUser/isRestingUser.inc"
}

#include "CUser/InitReceivedRequestType.inc"
}

#include "CUser/isBreakAwayDungeonClear.inc"
}

#include "CUser/isUsedBreakAwayLuckPoint.inc"
}

#include "CUser/isJoinedSchoolPointEvent.inc"
}

#include "CUser/DisableSaveMemberBonusFatigue.inc"
}

#include "CUser/IsEnableSaveMemberBonusFatigue.inc"
}

#include "CUser/SetProperLevelDungeonUser.inc"
}

#include "CUser/ClearProperLevelDungeonUser.inc"
}

#include "CUser/IsProperLevelDungeonUser.inc"
}

#include "CUser/isCharacLinkMessageFlag.inc"
}

// b28d-solo-CUser：CUser.h:2718 经 Main 代落 unsigned char→bool（P292：ORIG 调用点
//   0x08652c9a `call; test %al,%al; je` 无 setcc）。ORIG 定义体 0x086973f8 直返
//   `movzbl 0x703d9; pop; ret`（P9 直返字节）⇒ 体写 (bool) 转换试直返形。
#include "CUser/isLinkCharacDisconnectFlag.inc"
}

#include "CUser/getDisconnectLinkCharacSlotIdx.inc"
}

// b28d-CUser P292：ORIG 0x08697598 体 movzbl 直返（无 setne）⇒ bool。
#include "CUser/is_update_ontime_last_recv_idx.inc"
}

#include "CUser/reset_update_ontime_last_recv_idx.inc"
}

#include "CUser/CheckLoadRentalInfoFromExchange.inc"
}

#include "CUser/GetPuUser.inc"
}

#include "CUser/SetCeraPoint.inc"
}

#include "CUser/SetDebugCommand.inc"
}

#include "CUser/decre_check_count.inc"
}

#include "CUser/setLastLotteryTime.inc"
}

#include "CUser/SetNonClientRandInt.inc"
}

#include "CUser/recipeForceProbability.inc"
}

#include "CUser/setBreakAwayAccureCera.inc"
}

#include "CUser/setCharacAntibotSerialNum.inc"
}

#include "CUser/set_multiboxLotteryItemFailCnt.inc"
}

#include "CUser/resetPlayExpAdd.inc"
}

#include "CUser/resetCleanpadFailCnt.inc"
}

#include "CUser/isCleanPadVerifyLimit.inc"
}

#include "CUser/resetTotalPcRoomPlayTime.inc"
}

#include "CUser/ResetCurCharacUsedFatigueQuantity.inc"
}

// ORIG _ZN5CUser24set_server_fatigue_countEs T 0x0868d32a：按当日
// get_today_time_day 在 m_map8eb50 累加，无表项则插入。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868d32a | _ZN5CUser24set_server_fatigue_countEs
#include "CUser/set_server_fatigue_count.inc"
}

// ORIG _ZN5CUser31IncCurCharacUsedFatigueQuantityEj W 0x08110e1e：m_field8eba8 += n。
#include "CUser/IncCurCharacUsedFatigueQuantity.inc"
}

// ORIG _ZN5CUser35IncCurCharacUsedGiftFatigueQuantityEj W 0x08110e3a：m_field8ebac += n。
#include "CUser/IncCurCharacUsedGiftFatigueQuantity.inc"
}

// ORIG _ZN5CUser27ProcProperLevelDungeonClearEv T 0x0868f5fa（0x13 字节）：单行转发。
#include "CUser/ProcProperLevelDungeonClear.inc"
}

// ORIG _ZN5CUser15GiveMeBoxRewardEv T 0x08693758：单行转发 DB 请求。
#include "CUser/GiveMeBoxReward.inc"
}

// ORIG _ZN5CUser18SetTradePunishTypeEi T 0x08689890：printf 日志后
// m_postChecksumData.m_tradePunishType |= type。
#include "CUser/SetTradePunishType.inc"
}

// ORIG _ZN5CUser10SetMileageEi T 0x086509c6：LogHistory 记录新旧值后写 m_mileage。
#include "CUser/SetMileage.inc"
}

// ORIG _ZN5CUser12AddMoneyPlusEi T 0x0868f266：当日滚窗 m_money[0]+=value，m_money[2]=now。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868f266 | _ZN5CUser12AddMoneyPlusEi
#include "CUser/AddMoneyPlus.inc"
}

// ORIG _ZN5CUser13AddMoneyMinusEi T 0x0868f310：当日滚窗 m_money[1]+=value，m_money[2]=now。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868f310 | _ZN5CUser13AddMoneyMinusEi
#include "CUser/AddMoneyMinus.inc"
}

// ORIG _ZN5CUser18NextCharacMoneyLogEi T 0x0868f1c0：当前槽含时间且与
// 传入时刻不同日 → 槽推进（钳 6）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f1c0 | _ZN5CUser18NextCharacMoneyLogEi
#include "CUser/NextCharacMoneyLog.inc"
}

// ORIG _ZN5CUser21exceptedStateMoneyLogEv T 0x0868f3ba：组队村庄怪标记/搬运副本/
// 状态 8/村庄 7 任一命中 → 跳过货币滚窗。
#include "CUser/exceptedStateMoneyLog.inc"
}


#include "CUser/UseWinPoint.inc"
}

// ORIG _ZN5CUser17AddSecuRewardItemEh T 0x08688328（0x14 字节）：仅参数存
// 未使用局部（ORIG 即空操作）。
#include "CUser/AddSecuRewardItem.inc"
}

// ORIG _ZN5CUser24GenerateCleanPadVeriDataERtRhS0_ T 0x08649ac4：生成三段
// CleanPad 验证种子（ushort randInt(0xffff)+1 / uchar randInt(6) /
// ushort randInt(0xffff)+1）存 m_mcap 并回填出参。
#include "CUser/GenerateCleanPadVeriData.inc"
}

// ORIG _ZN5CUser15reqHumanCertifyEv T 0x0867f26e（0x252 字节）：未认证时发起
// CleanPad 人工认证——记日志/暴露计数、从验证队列随机取一条填 0x7d 包
// （byte 5 + int + binary + 认证种子三段 + 错误计数 + 0），发送后记 tick。
#include "CUser/reqHumanCertify.inc"
}

// ORIG _ZN5CUser22sendNotiPcRoomPlayTimeEv T 0x0868f83c：包 0x1a8（网吧时长通知）。
#include "CUser/sendNotiPcRoomPlayTime.inc"
}

// ORIG _ZN5CUser12SendCashDataEb T 0x0865082a：包 0x35（商城货币数据；
// flag=1 时全 0，否则 Cera/Mileage/CeraPoint）。
#include "CUser/SendCashData.inc"
}

// ORIG _ZN5CUser15recover_staminaEv T 0x0865784e：满体力外每分钟恢复
// （+9，钳 100）并回发包 0x4。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865784e | _ZN5CUser15recover_staminaEv
#include "CUser/recover_stamina.inc"
}

// ORIG _ZN5CUser13WorkPerTenMinEv T 0x08653196：无角色时 trace；有激活角色
// 则向监视代理上报角色在线（characNo）。
#include "CUser/WorkPerTenMin.inc"
}

// ORIG _ZN5CUser14WorkPerFiveMinEv T 0x08652f0c：五/十分钟公会上报族
// （一般经验与今日经验两段同构）；无角色时 trace 返回。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08652f0c | _ZN5CUser14WorkPerFiveMinEv
#include "CUser/WorkPerFiveMin.inc"
}

#include "CUser/get_server_fatigue_day_size.inc"
}

#include "CUser/IncreaseUsedCoinCount.inc"
}

#include "CUser/setLoadHackGold.inc"
}

#include "CUser/SetNonClientFlag.inc"
}

#include "CUser/setHumanCertified.inc"
}

#include "CUser/incTradeCount.inc"
}

#include "CUser/GetRating.inc"
}

#include "CUser/backupSeed.inc"
}

#include "CUser/ResetCharacExpandData.inc"
}

#include "CUser/IsExistGuildAgit.inc"
}

#include "CUser/GetDailyBadge.inc"
}

// ORIG @0x08568e0c（W）：out[i]=m_dailyBadge[i]，i=0..2
// （ORIG 寻址 (i+0x47040)*2+4 即 m_dailyBadge 逐字读）。
#include "CUser/GetDailyBadge_1.inc"
}

// ORIG @0x08681c30。On Time Event 奖励发放：
// 双日志 ./log/%s/OnTimeEvent → itemNo==0 短路返 0 → 包头(1,0x120)+byte1+
// get_index(记录未用)+byte1 → find_item(itemNo)（无表项返 0）→
// create_item(NONE,item,count)→有效性(m_addInfo||m_addInfo2)→插背包
// （reason 0xd；失败系统邮件 msg_132/133 + item.reset，成功 SendUpdateItem）
// → put_int/put_short → finalize 发送，返 1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681c30 | _ZN5CUser23InsertOnTimeEventRewardEjj
#include "CUser/InsertOnTimeEventReward.inc"
}

#include "CUser/get_charac_count.inc"
}

#include "CUser/isTradePunishOverThirtyDays.inc"
}


#include "CUser/sendBlueMarbleEnterCount.inc"
}

#include "CUser/ResetDailyCharacExpandDataMidnight.inc"
}

#include "CUser/ResetDailyCharacExpandData.inc"
}

#include "CUser/resetNPCRelationShipDailyData.inc"

#include "CUser/sendNPCRelationShipFavor.inc"

#include "CUser/SendProperDungeonClearCount.inc"
}

#include "CUser/acceptable_within_mission.inc"
}

#include "CUser/isDuplicationMessage.inc"
}

#include "CUser/DimensionInoutUpdate.inc"
}

#include "CUser/send_pvp_record.inc"
}

#include "CUser/send_clear_quest_list.inc"
}

#include "CUser/processReturnUserQuestAutoClear.inc"
}

#include "CUser/update_pvp_rank.inc"
}

#include "CUser/saveTaxMoneyForUpperMember.inc"
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fa08 | _ZN5CUser20gainExpAsUpperMemberEi
#include "CUser/gainExpAsUpperMember.inc"
}

#include "CUser/gainPowerWarRewardExp.inc"
}

#include "CUser/add_guild_point_item.inc"
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c844 | _ZN5CUser19SendMoneyFullReasonE22ENUM_MONEY_FULL_REASONmm
#include "CUser/SendMoneyFullReason.inc"
}

#include "CUser/AddDungeonClear.inc"
}
#include "CUser/SendConditionEventInfo.inc"
}

#include "CUser/ResetCurCharacUsedGiftFatigueQuantity.inc"
}

#include "CUser/SetChangedGiftFatigueQuantity.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08657570 | _ZN5CUser15ResetDailyQuestEv
#include "CUser/ResetDailyQuest.inc"
}

// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x08657662 | _ZN5CUser18ResetTrainingQuestEv
#include "CUser/ResetTrainingQuest.inc"
}

// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x085df9b2 | _ZN5CUser11SetPvpIndexEs
#include "CUser/SetPvpIndex.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0865cadc | _ZN5CUser18add_inventory_itemEj
#include "CUser/add_inventory_item.inc"
}

#include "CUser/send_MissionList.inc"
}

#include "CUser/send_aura_avatar_option.inc"
}

// ===================== GameStubs 迁移（ORIG 强符号 T；最小正确语义） =====================
// 全部签名经 nm -C /home/wangyilei/下载/df_game_r 核实。

// 0x086489f4（DisConnSig.md 汇编+反编译 C 全文核对）
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x086489f4 | _ZN5CUser10DisConnSigE11DISCONN_SIGbi
#include "CUser/DisConnSig.inc"
}

// 0x08665400：PvP 技能树发放主循环（givePvPSkillTree.md）：
// secondGrow∈[0..CurSecondGrow]、grade∈[a..pvp_grade]；脚本 miss 跳过；
// mode 1/2 只发树 0/树 1，mode 3 双树；b=true 且已有等级≥目标跳过。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08665400 | _ZN5CUser16givePvPSkillTreeEibi
#include "CUser/givePvPSkillTree.inc"
}

// 0x0867a95c

// 0x086804ce
// ORIG：断线回调。state==7（PvP 房间）时通知 PvP_Room::Disconnect；
// 在袭击区域（GetAssaultPlace != 0）时通知 CAssaultMgr::OnLeaveAssaultPlace(user, false)。
#include "CUser/OnDisconnect.inc"
}

// 0x0867a95c（返回 1 成功 / 0 角色为空；DisPatcher_DebugCommand 以 !=1 判失败）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867a95c | _ZN5CUser14SetCharacLevelEi
#include "CUser/SetCharacLevel.inc"
}

// 0x0866afa2：习得单技能主分支（master_new_skill.md）：
// skillIdx∈[0xc8,0xd0] 走公会技能（需会长/副会长）并经 guild proxy 上报；
// 否则普通 SkillSlot::master_new_skill。回包 header(1,0x1f)：失败字节0+错误码
// 映射{-1:0x15,-2:0x16,-3:0x12,-4:0x01,-5:0x5b,-6:0x5c}；成功 byte1+treeIdx+
// 剩余SP/SFP(short)+新槽位+skillIdx+等级。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0866afa2 | _ZN5CUser16master_new_skillER14stBuySkillInfob
#include "CUser/master_new_skill.inc"
}

// 0x0867b048

// ORIG：GM 改成长类型。无角色/参数越界(0..5, 0..1) → 0 并 cerr 报错；
// 否则 debugCheckGrowTypeSkill 校验 → set_grow_type(reason=2) →
// _postCheckForceChangeGrowType 后处理，返回 1。
// 返回 int：ORIG 0x0867b048 尾 `mov eax,0x1; ret`、早退 `mov eax,0x0`
// （Model22 桥清零实证；声明 CUser.h:3330 由 Main 代落）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b048 | _ZN5CUser17ChangeGrowType_GMEii
#include "CUser/ChangeGrowType_GM.inc"
}

// 0x0867b6d4
// ORIG：添加物品。角色空/交易中/物品不存在/count<0 → -1；
// 构造 Inven_Item（make_item），stackable 则 set_add_info(count)；
// *space 默认 0（生物物品 → 7）；slot∈(0,0x20) 且为装备 → SetUpgrade(slot)；
// insertItemIntoInventory(reason, true, true)，失败 -1；
// 生物物品再 InsertCreatureItem。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b6d4 | _ZN5CUser7AddItemEii14eItemAddReasonR14ENUM_ITEMSPACEi
#include "CUser/AddItem.inc"
}

// 0x0864cb66
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864cb66 | _ZN5CUser21LogoutCachedCharacterEh
#include "CUser/LogoutCachedCharacter.inc"
}

// 0x08689a22：表情配置重算（ReCalcChattingEmoticon.md）。读 GameOption 表情配置 →
// CDataManager 重算 → 写回；变化则组 SIG_SAVE_CHATTING_EMOTICON（0x214：+0 accId、
// +4.. 配置 0x210）投递队列 2。SIG 当前 pad 布局按偏移写入（推断）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689a22 | _ZN5CUser22ReCalcChattingEmoticonEv
#include "CUser/ReCalcChattingEmoticon.inc"
}

// 0x08689010：表驱动重置列表（ResetItemByScript.md）：元素 +0 itemspace、
// +4 到期秒；到期则压 (space,0)。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08689010 | _ZN5CUser17ResetItemByScriptERSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/ResetItemByScript.inc"
}

// 0x08689b90：聊天表情配置包(0x19b)。24 行：index、m_nCount(行内 +0x14)、
// 名字（行首 0x14 字节）。行大小 0x16（CChattingEmoticonConfigRow, pack(1)）。
#include "CUser/SendChattingEmoticon.inc"
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
}

// 0x086554aa：登出主链路（UpdateLogout.md）。SIG_UPDATE_LOGOUT(0x2c, 0x8b 字节)：
// accId/characNo/gcNo/fatigue；state>1 记 playTime/IP/机器码/会员奖励疲劳并复位；
// 两条 trace（0x22e0/0x22e2）；写登录时间戳；历史日志 Logout；DB 杂项（复仇/
// ontime/疲劳出勤）；!param 发包(1,3)；清商城内购限制。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086554aa | _ZN5CUser12UpdateLogoutEb
#include "CUser/UpdateLogout.inc"
}

// 0x0867e734：日删可堆叠物品（deleteDailyStackableItem.md）。区间 [a,b] 三重过滤
// （空槽/find_item/is_stackable/+0x3c5>0 日删标记）后删除并压 (space=0, slot)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e734 | _ZN5CUser24deleteDailyStackableItemERSt6vectorISt4pairIiiESaIS2_EEii
#include "CUser/deleteDailyStackableItem.inc"
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
}

// 0x08652c8e
// ORIG：联动角色断线处理。标志开启时：若数据加载/建角/删角被锁定则记错误日志；
// 否则把联动角色槽位移到列表尾（ChangeCharacSlot 两次）、清标志并刷新角色视图。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x08652c8e | _ZN5CUser22doLinkCharacDisconnectEv
#include "CUser/doLinkCharacDisconnect.inc"
}

// 0x08651b7a
// ORIG：把 m_characList（vector<_Charac_info> @ +0x796e8）中下标 a 的元素移动到
// 下标 b 处（a<b 时先 insert(begin()+b+1, v[a]) 再 erase(begin()+a)；
// a>b 时 insert(begin()+b, v[a]) 再 erase(begin()+a+1)），随后
// enableSaveCharacView(&m_characterView)。越界（a/b >= size）直接返回。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651b7a | _ZN5CUser16ChangeCharacSlotEjj
#include "CUser/ChangeCharacSlot.inc"
}

// ORIG 0x086973dc：设置联动角色断线标志（bool 字段 this-先直写 movb）。
#include "CUser/setLinkCharacDisconnectFlag.inc"
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

// ORIG T 0x08651322：填充单角色的可见数据区（avatar 外观槽 / 租赁槽 /
// 非当前角色缓存直拷 / 改名状态）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08651322 | _ZN5CUser27updateCharacViewVisibleDataERK12_Charac_infoR17CHARAC_LOGIN_DATA
#include "CUser/updateCharacViewVisibleData.inc"
}

// ORIG T 0x08651740：角色视图变化落库包。频道 !=7 且 isSaveCharacView() 时组
// SIG_UPDATE_CHARAC_VIEW（header 6 + UID），memset 全块后逐 _Charac_info 填
// 记录（stride 0x94）并调 updateCharacViewVisibleData，投递 DB_Q 后清保存标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651740 | _ZN5CUser16UpdateCharacViewEv
#include "CUser/UpdateCharacView.inc"
}

// 0x086786be
// ORIG：放弃惩罚通知。非轻量服、+0x711d8 标志为 0（推断：惩罚冷却标志）、
// 未受影响 premium(0xc)、等级 > 0x11、moveSpace != 1 时，
// 发包(0x21)并 setCurCharacStamina(10)。
#include "CUser/giveup_panalty.inc"
}

// 0x08658910
// ORIG：登出清理——清登录时间/播放经验/多开彩票失败计数、移除 GM 标记、
// 复位 uid/槽位/自增 ID/账号名、断网并整体 reset。
#include "CUser/log_out.inc"
}

// 0x086487ea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086487ea | _ZN5CUser17prepareDisconnectEv
#include "CUser/prepareDisconnect.inc"
}

// 0x0865db6c
// ORIG：按物品空间号生成并发送对应列表包：
//   0/1 → CInventory::MakeItemList(1/2)、2 → CCargo::MakeItemList、
//   7 → 生物列表 + MakeItemList(3)、0xc → CAccountCargo::SendItemList（不存在时返回 1）。
// b28d-lane5 N封顶: P0 清绑连带——同 TU callee（IsExistAccountCargo 等）清绑后帧变化，
// 4.4 按 P210 精确继承 outgoing ⇒ 本函数帧 0x20→0x30、PacketGuard 槽 -0x14→-0x1c；
// 另 v^=1 段清绑后落栈槽物化（P168a④）。| ORIG 0x0865db6c | _ZN5CUser14send_itemspaceEi
#include "CUser/send_itemspace.inc"
}

// 0x0868c170（返回 1 已通知 / 0 未满足条件）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868c170 | _ZN5CUser31send_ontime_reward_start_notifyEv
#include "CUser/send_ontime_reward_start_notify.inc"
}

// 0x0866c46a:技能信息包(send_skill_info.md)。
// 用具名语义字段访问：玩家技能经 SkillSlot（m_skillSlot 树0 / m_skillSlot2 树1，
// _Mastered_skill{索引,等级}），公会技能经 STGuildDBInfoOnly（m_skillLearnCnt 计数、
// m_skills[16] 5 字节条目 STGuildSkill{技能ID,等级}）。背包命令段经
// findSkillCommandData(索引) 取 STSkillCommandData{count, m_key0..}。
// ORIG 语义要点：公会槽位号 = i + 0x8a；公会技能判定 = 双 grade 非零 且 技能ID > 0xc7。
#include "CUser/send_skill_info.inc"
}

// 0x0866cb04

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866cb04 | _ZN5CUser17CheckQuestMonsterEii21ENUM_QUEST_ENEMY_TYPE
#include "CUser/CheckQuestMonster.inc"
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
#include "CUser/FatigueUp.inc"
}

// 0x0867ba5c（SendNotiPacket.md）：header(0,cmd)；cmd==2 → byte+short(1)+
// make_basic_info；cmd==0x25 → level/exp/组队/会员 exp/SP/SFP/胜点(+0x58)/
// 疲劳/seria bonus/周末(事件 0x57 活跃才发并 reset)/premium bonus(reset)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867ba5c | _ZN5CUser14SendNotiPacketENS_11eSendTargetE15ENUM_NOTIPACKETi
#include "CUser/SendNotiPacket.inc"
}

// 0x0865c936
// ORIG：公会战积分累加。PvP 房间内且非观察者、公会战事件(0x21)进行中且
// 有公会时：通知公会服 + 发包(0x6e) + 累加 +0x8cfe0（m_guildWarPoint）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c936 | _ZN5CUser20add_guild_pvp_resultEi
#include "CUser/add_guild_pvp_result.inc"
}

// 0x0865d986
// ORIG：PVP 每日/每周游玩次数累计。PvpResultType 布局（CUserCharacInfo.h）：
//   +0x60 当日游玩次数（隔天重置为 0）、+0x64 累计游玩次数、
//   +0x68 当日基准时间戳（0 点 + 0x15180）。字段未具名，按 ORIG 偏移访问（推断）。
#include "CUser/add_pvp_play_info.inc"
}

// 0x0865c678
// ORIG：PvP 结果结算。观战者直接返回；winPointBuffer(+0x796fc) 清零并入账；
// 胜/败计数（整合 PvP 基础频道且渠道类型==0 时不计）；额外胜点补发并记 WPAdd 日志；
// 战斗模式 4/5 且 out 非空时按 mob_reward×胜点增量 + gainPowerWarRewardExp 计算奖励。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c678 | _ZN5CUser14add_pvp_resultEbPj
#include "CUser/add_pvp_result.inc"
}

// 0x0864fac4
// ORIG：公会技能已学查询。GetGuildDBInfo 内 +0x44 成员数、+0x45 起
// 每 5 字节 {skillIdx(int), level(byte)}，返回匹配项的 level 字节。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fac4 | _ZN5CUser17IsGuildSkillLearnEi
#include "CUser/IsGuildSkillLearn.inc"
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
}

// ---- 0x0869729a incPlayExpAdd：m_gameOption.m_playExpAdd += v（ORIG W 内联）----
#include "CUser/incPlayExpAdd.inc"
}

// 0x0866a9a0 gain_sp：SP 双树（0/1）clamp 0x7fffffff 后累加。
#include "CUser/gain_sp.inc"
}

// 0x0866aad2 gain_sfp：SFP 双树（2/3），v==0 直接返回，clamp 0x7fffffff。
#include "CUser/gain_sfp.inc"
}

// 0x0866ac0e history_log_sp：before==-1 或当前值变化时记 SPAdd(tree 0)。
#include "CUser/history_log_sp.inc"
}

// 0x0866acd0 history_log_sfp：同上（tree 2 / SFPAdd）。
#include "CUser/history_log_sfp.inc"
}

// 0x866273c Remove_Mission：转发 charac_expand 槽 8 的任务列表。
#include "CUser/Remove_Mission.inc"
}

// 0x0868e9c0
#include "CUser/isCompetitionMercenary.inc"
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
}
// 0x08657450
// ORIG：每日重置——副本金币数据、安全卡/密保取消次数（非 0 时归零并回写 DB）、
// NPC 好感度、ActionPoint（APSystem）落库并结算今日行动/勋章奖励。
#include "CUser/resetDailyData.inc"
}

// 0x08657f10
// ORIG：每日币恢复。getCurCharacR() 为空直接返回；若每日币事件(4)进行中，
// 按角色等级取当日币数并读背包币；premium 有优势时加 premium 币加成；
// 最后按背包币是否足够（coin < need）设置 SetOpencoin(0/1)。
#include "CUser/RecoverCoin.inc"
}

// 0x086568fc
// ORIG：开放标志包(0xb1)。角色存在时：每日币事件(4)进行中取等级 1 的当日币数，
// premium 有优势加币加成；计算缺口（need - coin）并在 opencoin 标志下发包。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x086568fc | _ZN5CUser12SendOpenflagEv
#include "CUser/SendOpenflag.inc"
}

// 0x086931c4
// ORIG：无效物品删除补偿。逐条记 DeleteInvaildItem 日志并按脚本换算奖励道具；
// 邮件发件人/正文按脚本字符串表（表 4）翻译，空则用 defalt_string；
// 每 10 件一批 ReqDBSendNewSystemMultiMail，余件单独一封。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086931c4 | _ZN5CUser28RewardItem2DeleteInvalidItemERKSsRKSt6vectorISt4pairIiiESaIS4_EE
#include "CUser/RewardItem2DeleteInvalidItem.inc"
}

// 0x08656caa
// ORIG：每日登录物品补给。等级>0x13 时道具 0x1057 补至 3 个（无则插入、有则
// 堆叠补满并记 ItemAdd 日志）；随后 ResetItemByScript/RefillItemByScript/
// deleteSpecificItem 脚本链并回发受影响槽位包；祭坛免费票补满；截止时间前
// 发放道具 0x1cab（背包满走 game_server_msg_24/25 系统邮件）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08656caa | _ZN5CUser12AddDailyItemEv
#include "CUser/AddDailyItem.inc"
}

// 0x0868aaea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868aaea | _ZN5CUser24SendOneADayItemShopIndexERKSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/SendOneADayItemShopIndex.inc"
}

// 0x0867c2d8
// ORIG：单槽位物品更新包(0xe)。GetInvenRef 空 → return 0；否则写槽头 +
// 物品字段（addInfo2 为 0 时首 int 写 -1）、强化选项、avatar 宝石槽 0x1e 字节
// 与扩展信息 4 字节、生物装备剩余天数（type==5），追加三转编年史与整件包体。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c2d8 | _ZN5CUser14SendUpdateItemENS_11eSendTargetE14ENUM_ITEMSPACEi
#include "CUser/SendUpdateItem.inc"
}

// 0x0868de0a
// ORIG：光环 avatar 选项变更 -> DB_UpdateAuraAvatarOption::makeRequest(uid, characNo, idx, value)
#include "CUser/UpdateAuraAvatarOption.inc"
}

// 0x0868dff8
// ORIG：光环 avatar 装备检查。无角色 → 1；槽 9 无物品/不可装备 → 0x11；
// 物品为光环 avatar 选项 → 回写选项值并返回 0，否则 0x17。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0868dff8 | _ZN5CUser20is_equip_aura_avatarEcRi
#include "CUser/is_equip_aura_avatar.inc"
}

// 0x0867e092
// ORIG：每日道具过期删除（idx 区间 [3,8] 与 [0x39,0x68]）后聚合发送：29 个 0xe 包
// 按物品空间预置包头并记录计数字段位置；cargo/账号金库直接 MakeItemPacket，
// 背包空间空槽写固定"已删"载荷；最后按非零计数回填数量并逐包下发。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e092 | _ZN5CUser19processDelDailyItemEv
#include "CUser/processDelDailyItem.inc"
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
}

// ---- CUser::master_new_skills @ 0x0866b53e ----
// 批量习得技能：按 _Mastered_skill 数组计算 SP/SFP 用量并扣减对应技能树余量，
// 将技能槽写入（set_skill_slot_at_index），同时记录 SP/SFP 消耗日志。
// PvP 树频道直接返回 true；普通树按 secondGrow/firstGrow 走成长型
// get_give_skill 展开（ORIG 866b844-866bcd5 逐段转写）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866b53e | _ZN5CUser17master_new_skillsEP15_Mastered_skill20ENUM_SKILL_TREE_KIND
#include "CUser/master_new_skills.inc"
}

// 0x0867cbe4（从 GameStubs_remaining.cpp 迁移，语义对齐 CUser/ProcPremiumFatigue.md）
#include "CUser/ProcPremiumFatigue.inc"
}
// ORIG T 0x0868866e（报告 docs/class_func_reports/CUser/isSecurityServiceUsed.md）：
// 安全键盘激活 或 安全卡激活 或 secuGrade(+0x8e97f) 非零 → 已启用安全服务。
#include "CUser/isSecurityServiceUsed.inc"
}

// ORIG W 0x0863bea2：return m_premium(+0x7121c).GetTotalPremiumCount() != 0
// （scout 报告误关联 0x867cd20 = _UpdateUserInfo4Guild，已按真实符号核实）。

// ORIG T 0x0864fc3e CheckLimitMiniumAge：唯一 owner 为 CUserOnlyOrig.cpp
// （W7_BuildHarmonize 去重：该版含 outUnique=unique_id 低16位写出，更忠实）。
#include "CUser/IsPremiumUser.inc"
}

// ============================================================================
// 租借物品序列化链（CUser::SaveInventory 直接依赖；m_rentalInfo@+0x6ef78 为
// map<uint, map<uint, vector<InstanceRentalSystem::RentalInfo>>>）。
// ============================================================================

// ORIG T 0x08676dba：`out = m_rentalInfo[param]`（外层 map 键 param，值
// map<uint, vector<RentalInfo>>）；仅当键存在时拷贝。
#include "CUser/GetInstanceRentalInfo.inc"
}

// ORIG T 0x086773ea：返回 m_rentalInfo[param][charac_no] 是否存在；存在则拷贝到 out。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086773ea | _ZN5CUser26getCurrentCharacRentalInfoERSt6vectorIN20InstanceRentalSystem10RentalInfoESaIS2_EEi
#include "CUser/getCurrentCharacRentalInfo.inc"
}

// ORIG T 0x08677516：`m_rentalInfo[param][charac_no] = in`（vector 拷贝）。
#include "CUser/setCurrentCharacRentalInfo.inc"
}

// ORIG T 0x08677584：遍历当前角色租借列表，对仍存在的租借物品在序列化缓冲
// （SIG_SAVE_INVENTORY）中清除对应背包槽；并回写租借条目的 inEquip 标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677584 | _ZN5CUser32removeRentalItemInfoFromInvenBufEP18SIG_SAVE_INVENTORYi
#include "CUser/removeRentalItemInfoFromInvenBuf.inc"
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
}
// STGuildDBInfoOnly ctor（ORIG W 0x08693f56 sz 0x16f；无报告，objdump 转写：
// 逐字段清零 → 循环构造 16×STGuildSkill（+0x45 步长 5）→ 尾部标量清零 →
// memset 子成员 → +0x1c (x|1)&~2 → memset skills 覆盖 → memset masterName）。
// 注意 ORIG 在循环构造 STGuildSkill（-1/0xff）后又 memset(+0x45,0,0x50) 覆盖清零，
// 净效果 skills 全 0（CUser.h 冻结 m_skills 为 char[0x50]，无构造语义，直接清零）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08693f56 | _ZN17STGuildDBInfoOnlyC1Ev
#include "CUser/STGuildDBInfoOnly.inc"
}

// ===================== U1 批次（FixUndefU1）=====================

// ORIG T 0x08679754 sz0x2e2：仅 level%10==0 写公会公告板；文案 = 角色名 +
// ServerString(0x3e9) + "%d"(level) + ServerString(0x3e8)；len>=0x78 丢弃。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679754 | _ZN5CUser23makeGuildLevelUpMessageEi
#include "CUser/makeGuildLevelUpMessage.inc"
}

// ORIG T 0x0867cd20 sz0x18d：guildkey!=0 时组 header 0x63 包广播队列 2。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x0867cd20 | _ZN5CUser21_UpdateUserInfo4GuildEv
#include "CUser/_UpdateUserInfo4Guild.inc"
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
}

// ============================================================================
// 角色内存快照填充（CacheCharacterMemory 依赖）——ORIG 报告逐函数实现。
// ============================================================================

// ORIG 0x0864de58 _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0864de58 | _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
#include "CUser/GetDungeonClear.inc"
}

// ORIG 0x0867f1d4 _ZNK5CUser17GetBlackListCountEP19SIG_REQ_BLACK_COUNT
#include "CUser/GetBlackListCount.inc"
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
}

// ORIG T 0x086769ea：PC 房间租借物品 → SIG_SAVE_PC_ROOM_RENT_ITEM（最多 2 个 0xc 条目）。
// 逐条目读取 getCurrentCharacRentalInfo(list,1)，对仍存在物品写 [0]itemIndex/[4]byte/[5]byte/
// [6]inEquip/[8]int。参数为租借清单类型 1。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x086769ea | _ZN5CUser17GetPcRoomRentInfoEP26SIG_SAVE_PC_ROOM_RENT_ITEM
#include "CUser/GetPcRoomRentInfo.inc"
}

// ORIG T 0x08676bec：次元租借物品 → SIG_SAVE_DIMENSION_RENT_ITEM（最多 12 个 8 字节条目）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08676bec | _ZN5CUser20GetDimensionRentInfoEP28SIG_SAVE_DIMENSION_RENT_ITEM
#include "CUser/GetDimensionRentInfo.inc"
}

// ORIG T 0x0864bc04 sz0x1b5：填充 SIG_LOAD_CHARAC。
//   +0x0 acc_id / +0x4 charac_no(*curR 首字段) / +0x8 result(0 初始, 失败置 1) /
//   +0xc CHARAC_DATA（getCharacInfoDetail）/ +0x686 tag_charac_no /
//   +0x68a striker_skill_index / +0x68b TAG_CHARAC_DATA（getTagCharacInfo）。
#include "CUser/GetCharacInfo.inc"
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
}

// ORIG T 0x0864d4ba：填充 SIG_LOAD_INVENTORY（含 GetInventory + creature + avatar + tag + rent）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d4ba | _ZN5CUser16GetInventoryInfoEP18SIG_LOAD_INVENTORY
#include "CUser/GetInventoryInfo.inc"
}

// ORIG T 0x0869067c：TAG 角色背包 → TAG_CHARAC_INVEN_DATA。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0869067c | _ZNK5CUser21getTagCharacInventoryEP21TAG_CHARAC_INVEN_DATA
#include "CUser/getTagCharacInventory.inc"
}

// ORIG T 0x0867776c：从 SIG_LOAD_INVENTORY 背包镜像清除已不存在的租借物品。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867776c | _ZN5CUser36removeRentalItemInfoFromInvenLoadBufEP18SIG_LOAD_INVENTORYi
#include "CUser/removeRentalItemInfoFromInvenLoadBuf.inc"
}

// ORIG T 0x0867846e sz0xb7：遍历 m_list8cf80（list<int>）票表 → SIG_LOAD_TICKET。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867846e | _ZNK5CUser9GetTicketEP15SIG_LOAD_TICKET
#include "CUser/GetTicket.inc"
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864bdba | _ZNK5CUser19getCharacInfoDetailEijP11CHARAC_DATA
#include "CUser/getCharacInfoDetail.inc"
}

// ORIG T 0x086904f0：从 m_characList[param_1] 填充 TAG_CHARAC_DATA（SIG_LOAD_CHARAC +0x68b 区）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086904f0 | _ZNK5CUser16getTagCharacInfoEijP15TAG_CHARAC_DATA
#include "CUser/getTagCharacInfo.inc"
}

// ORIG T 0x0864d646：CInventory/CCargo 序列化 helper（供 GetInventoryInfo）。

// ORIG 0x08677dd8 _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
// 遍历 getBattleRecordRefR() 的 map<int,GameResultType>，每元素 0x50 字节序列化到
// sig+0x3c8+idx*0x50（最多 100 个），末尾写 count 到 sig+0x3c4。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677dd8 | _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
#include "CUser/GetRecord.inc"
}

// ORIG 0x08688dc4 _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
// 遍历 m_limitItemUsage 向量，每元素序列化到 sig+(i+0x263)*0x10 的 0x10 字节区。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08688dc4 | _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
#include "CUser/GetLimitItemUsage.inc"
}

// ORIG 0x086910a6 _ZNK5CUser27GetCharacCashAttendanceInfoER20SIG_ATTENDANCE_EVENT
#include "CUser/GetCharacCashAttendanceInfo.inc"
}

// ---- 四个 CUser 字段访问器（CacheCharacterMemory 域；ORIG W 弱符号）----
#include "CUser/getTotalPcRoomPlayTime.inc"
}
#include "CUser/getPcRoomPlayTimeRewardCnt.inc"
}
#include "CUser/GetCurCharacUsedGiftFatigueQuantity.inc"
}
#include "CUser/getFlagPresentAvengerTitle.inc"
}

// ORIG 0x0868ddc2 _ZNK5CUser19GetAuraAvatarOptionER18stAuraAvatarOption
// 读当前角色 _Charac_info+0x1281..+0x1289 三 int → option。ORIG 无 cur 局部：
// getCurCharacR() 判定(0x868ddce)与拷贝(0x868dde2)各重调一次（P48，调用点差集归零）。
// 记 N（跨版本 regmove，N18a 族）：4.1 O0 将 option 基址驻 %edx 单装载 +
// `0x1281(%eax)` 索引三连；4.4 O0 对引用形参逐条重装载，register-asm 绑定反产
// 冗余 regmove（probe 四形 21/23/26/28/30 均不达 24）——语义+调用点全对，残 +4。
#include "CUser/GetAuraAvatarOption.inc"
}

// ORIG 0x0868f03c _ZNK5CUser18GetCurCharacOptionER15CharacterOption
// 0x200 字节 **结构体赋值** 自 _Charac_info+0x12b7（CharacterOption::m_pad[0x200]）：
// ORIG 4.1 内联为「对齐处理 + rep movsl + 尾块」序列（无 memcpy call），
// 且 getCurCharacR() 调两次（+0x0f 判空、+0x27 取址），不落具名 cur 缓存（P183）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f03c | _ZNK5CUser18GetCurCharacOptionER15CharacterOption
#include "CUser/GetCurCharacOption.inc"
}

// ORIG 0x0864dcf2 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864dcf2 | _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
#include "CUser/GetSkill.inc"
}

// ORIG 0x08654768 _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
// 将 CUserCharacInfo 角色内存 +0x1254 处（getSkillCommand()）的 skill command map
// （std::map<int,STSkillCommandData>）逐项压缩为 STCommand（32 位位域）写入 out[]，最多 50 项。
// map 为空指针时 out 不变；返回 out（ORIG 返回未定义值，调用方 GetSkill 忽略返回值）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配（槽 -0x10/-0x14/-0x18 与中转寄存器差，语义/调用点全同）| ORIG 0x0865459a | _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
#include "CUser/get_skill_command_buf.inc"
}

// ORIG 0x0865459a _ZN5CUser21get_skill_command_bufEv
// 同上，但写入成员 m_skillCommands[50]（+0x703db）并返回其指针；map 为空指针返回 0。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配（槽 -0x10/-0x14/-0x18 与中转寄存器差）| ORIG 0x0865459a | _ZN5CUser21get_skill_command_bufEv
#include "CUser/get_skill_command_buf_1.inc"
}

// ORIG 0x0867794c _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// 依 SIG_LOAD_ETC 具名布局填充。expand 子区经 CExpandEquipslot vtable 槽 +8（getData）
// 直调（同 CacheCharacterMemory 的 0xc 槽位写法）；混合区内子对象用文档化偏移定位。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867794c | _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
#include "CUser/GetEtcInfo.inc"
}

// ORIG T 0x08650aa2 sz0x87f：SIG_SAVE_CHARAC(0x10d) 组包；四个脏块分支后投递
// 队列 2 并清 save 标志。个别字段写入为报告偏移直写（推断标注见注释）。
// ORIG T 0x088921c2 `_Z11setBit_ByteRhbi`（定义在 BitTools.cpp；本 TU 唯一调用点在下）。
#include "CUser/setBit_Byte.inc"
#include "CUser/_SaveCharacData.inc"
}

// ORIG T 0x0864d84e sz0x2a：param!=0 时 push_back 到受限商品列表(+0x711ec)。
#include "CUser/SetRestrictedGoods.inc"
}

// ORIG T 0x0864d878 sz0x91：线性查找列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d878 | _ZN5CUser17IsRestrictedGoodsEi
#include "CUser/IsRestrictedGoods.inc"
}

// ORIG T 0x0864d9a4 sz0x17b：header(0,0x44)；flag=true 单条，false 全列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d9a4 | _ZN5CUser19SendRestrictedGoodsEbi
#include "CUser/SendRestrictedGoods.inc"
}

// ORIG T 0x08656636 sz0xad：遍历 m_characList 统计 _Charac_info+0x27(short)==type。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08656636 | _ZN5CUser24GetSpecialLevelCharacCntEi
#include "CUser/GetSpecialLevelCharacCnt.inc"
}

// ORIG _ZNK5CUser22HasCurCharacRentalItemEi T 0x086772d2 sz0x117：
// 外层 map 键=rental 类型，内层键=当前角色 characNo（+0x00），vector 非空即有。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086772d2 | _ZNK5CUser22HasCurCharacRentalItemEi
#include "CUser/HasCurCharacRentalItem.inc"
}

// ORIG W 0x08230172 sz0x3b：guildkey!=0 且当前角色==m_guildDB+0x17（master）。
#include "CUser/IsGuildMaster.inc"
}

// ORIG T 0x0866c414 sz0x55：m_guildDB+0x2e 起 5 个 int 副会长表。
#include "CUser/IsSubGuildMaster.inc"
}

// ---------------- U1 批次第二段（deleteSpecific 域 / 杂项发送域）----------------

// ORIG T 0x0867cf0c sz0x81：找 pair.first==itemIdx，返回 second；未找到 -1。
#include "CUser/_isMatchedItem.inc"
}

// ORIG T 0x0867d0be sz0x330：[begin,end] 槽位超时/匹配/无效物品删除；
// 无效物品入 mailDel，最后邮件补偿。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d0be | _ZN5CUser24_deleteSpecificItemInvenE10INVEN_TYPERKSt6vectorISt4pairIiiESaIS3_EERS5_ii
#include "CUser/_deleteSpecificItemInven.inc"
}

// ORIG T 0x0867d3ee sz0x2ae：同型逻辑作用于 CCargo 槽位 0..0x97。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d3ee | _ZN5CUser24_deleteSpecificItemCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
#include "CUser/_deleteSpecificItemCargo.inc"
}

// ORIG T 0x0867d69c sz0x1d2：GetCharacExpandData(6) 兑换列表遍历删除，
// 超时或命中即 erase + alter() 标脏。m_redeemList 私有，按 ORIG 布局经 +0x08 访问。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d69c | _ZN5CUser25_deleteSpecificRedeemItemERKSt6vectorISt4pairIiiESaIS2_EE
#include "CUser/_deleteSpecificRedeemItem.inc"
}

// ORIG T 0x0867d8a4 sz0x2c1：账号金库（须 IsExistAccountCargo），槽位数=GetCapacity()。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d8a4 | _ZN5CUser31_deleteSpecificItemAccountCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
#include "CUser/_deleteSpecificItemAccountCargo.inc"
}

// ORIG T 0x0867db66 sz0x39：转发给 GetCharacExpandData(0xe)（CTitleBook）。
#include "CUser/_deleteSpecificItemTitieBook.inc"
}

// ORIG T 0x086928c2 sz0x145：金币兑换。itemIdx<=0x28d291 → (idx-0x28d287)*10^6；
// 否则 idx*10^7-0x549a800。bReset 只做上限校验（err=10 返回 false）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086928c2 | _ZN5CUser13DoGetGoldItemEiRSt6vectorISt4pairIiiESaIS2_EEbR10ENUM_ERROR
#include "CUser/DoGetGoldItem.inc"
}

// ORIG T 0x08692a34 sz0xc2：!bReset 时经 s_pIPGHelper 虚调用 vtable+0x0c/+0x2c；
// WongWork::IPG::CIPGHelper 仅最小建模（仅 dtor 虚函数），虚接口待补后恢复真实调用。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08692a34 | _ZN5CUser18DoGetCeraPointItemEjbR10ENUM_ERRORm
#include "CUser/DoGetCeraPointItem.inc"
}

// ORIG T 0x0868c3bc sz0xa2（返回 1 已删 / 0 事件空或非活跃）：
// OnTimeEvent(0x33) 活跃时删除奖励用户。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868c3bc | _ZN5CUser25delete_ontime_reward_listEv
#include "CUser/delete_ontime_reward_list.inc"
}

// ORIG T 0x086924aa sz0x119：job==9 才发包；去重 combo 技能 → 包 0x1bd。
#include "CUser/sendComboSkillList.inc"
}

// ORIG T 0x08656808 sz0xbb：发送当前金币（MakeItemPacket(1,1) → 0xe 包）。
#include "CUser/SendCoin.inc"
}

// ORIG T 0x086504d8 sz0x11d：频道类型 !=7 时经 BigStreamPool 构造
// header(0x23)+UID 内部流，GetInBuffer<SIG_SAVE_INVENTORY> 后 _GetSaveInvenBase
// 填充、+0x08 字节置 1，投递 DB_Q。反编译见 CUser/SaveMoneyCoin.md。
#include "CUser/SaveMoneyCoin.inc"
}

// ORIG T 0x08690e36 sz0x82：事件邮件提醒 tooltip。空包 put_header(0,0x1ad) +
// finalize(true) 后直接 Send（docs/class_func_reports/CUser/SendEventMailAlaramTooltip.md）。
#include "CUser/SendEventMailAlaramTooltip.inc"
}

// ORIG T 0x0869154a sz0x664：抽奖计数道具发放（事件币/道具/邮件补偿）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0869154a | _ZN5CUser20giveLotteryCountItemEib
#include "CUser/giveLotteryCountItem.inc"
}

// ORIG T 0x0868f472 sz0x158：i=0..min(6,m_field796c8) 发 0x1e 字节 MoneyLogPacket；
// 最后 resetMoneyLog()。对象布局为全局 MoneyLogPacket（PacketHeader 10B +
// 5 int @+0xa..+0x1d；ORIG 字段偏移实证），ctor 写 0x2723/0x1e 头并清零。
#include "CUser/SendMoenyLog.inc"
}

// ORIG T 0x0868152c sz0x1e0：PC 房 login 通知。填 stLogin（login[0]=3/acc 名/
// charac 名/ip/uid/accid）→ stLoginPacket 打包 → SendTcpPacket；随后
// ReadPacket 读回包（ORIG 以 pkt 自身为 this、reply 为 packet 源）。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0868152c | _ZN5CUser23SendLoginToPCRoomServerEv
#include "CUser/SendLoginToPCRoomServer.inc"
#define LP_LOGIN ((nexon_packet::stLogin*)login)
    LP_LOGIN->m_type = 3;
    strcpy(LP_LOGIN->m_account, get_acc_name());
    LP_LOGIN->m_accLen = (char)strlen(get_acc_name());
    strcpy(LP_LOGIN->m_characName, getCurCharacName());
    LP_LOGIN->m_charLen = (char)strlen(getCurCharacName());
    const int ip = get_public_ip_address();
    LP_LOGIN->m_ip = (unsigned int)ip;
    LP_LOGIN->m_canTry = 0;
    LP_LOGIN->m_userIdx = (unsigned int)GetUID();   // ORIG：直接写 stLogin+0x206（-0x20(%ebp)）
    LP_LOGIN->m_accId = (unsigned int)get_acc_id(); // ORIG：直接写 stLogin+0x20a（-0x1c(%ebp)）
    printf(
        "SEND nexon_packet::stLogin login:%d AccLen:%d Acc:%s CharLen:%d Char:%d "
        "Addr:%d canTry_:%d UserIdx:%d m_id:%d\n",
        LP_LOGIN->m_type, LP_LOGIN->m_accLen, LP_LOGIN->m_account, LP_LOGIN->m_charLen,
        (int)LP_LOGIN->m_characName,
        ip, LP_LOGIN->m_canTry, (int)LP_LOGIN->m_userIdx, (int)LP_LOGIN->m_accId);
#undef LP_LOGIN

    nexon_packet::stLoginPacket pkt(*(nexon_packet::stLogin*)login);
    int len = pkt.GetLength();
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, len);
    memset(reply, 0, sizeof(reply));
    pkt.ReadPacket((const char*)reply, *(nexon_packet::stLogin*)reply);
}

// ORIG T 0x0868170c sz0x113：PC 房 logout 通知。stLoginPacket 打包 →
// GetLength → SendTcpPacket（nexon_packet 已建模，符号直调）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868170c | _ZN5CUser24SendLogoutToPCRoomServerEv
#include "CUser/SendLogoutToPCRoomServer.inc"
#define LP_LOGOUT ((nexon_packet::stLogin*)login)
    LP_LOGOUT->m_type = 1;
    strcpy(LP_LOGOUT->m_account, get_acc_name());
    LP_LOGOUT->m_accLen = (char)strlen(get_acc_name());
    strcpy(LP_LOGOUT->m_account, get_acc_name());   // ORIG 重复一次（+0x049/+0x064 双 strcpy）
    LP_LOGOUT->m_accLen = (char)strlen(get_acc_name());
    strcpy(LP_LOGOUT->m_characName, getCurCharacName());
    LP_LOGOUT->m_charLen = (char)strlen(getCurCharacName());
    LP_LOGOUT->m_ip = (unsigned int)get_local_ip_address();
    LP_LOGOUT->m_canTry = 1;
#undef LP_LOGOUT

    nexon_packet::stLoginPacket pkt(*(nexon_packet::stLogin*)login);
    int len = pkt.GetLength();
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, len);
}

// ORIG T 0x0864870e sz0xdc：LOGDB_Q(=4)：<<0xcc、<<-1、<<reason。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864870e | _ZN5CUser29SendClientCrashDownLogToLogDBEi
// b28d-solo-CUser：返回类型 void→int（CUser.h:3228 Main 代落）——ORIG 尾
//   `mov $0x1,%ebx; …; mov %ebx,%eax; ret` 返回值 1（P159 尾形判返回类型）。
#include "CUser/SendClientCrashDownLogToLogDB.inc"
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
}

// ORIG _ZN5CUser14setStdDropRateEi @ 0x0828715c (W)：
// *(int*)(this+0x79624) = rate，即 m_codeHackCheck(+0x795fc).m_field28。
#include "CUser/setStdDropRate.inc"
}

// ============================================================================
// CUser 连续/冷却道具链簇（ORIG T，单函数还原见各 .md）
// ============================================================================

// ORIG _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj @ 0x0865eb0c (T)
// cooltime 处理：CStackableItem+0x2d0(ms) 换算为秒累加当前秒，更新/插入
// CheckCoolTimeItem 所读的 cooltime 列表条目（GetCurCharacCoolTimeItemList）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865eb0c | _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj
#include "CUser/_processCoolTimeItem.inc"
}

// ORIG _ZN5CUser18_processEffectItemEP14CStackableItemjj @ 0x0865ec0e (T)
// effect 处理：CStackableItem+0x2e0 第 0 字节非 1 → 报错；
// 否则读 +0x2dc(ms) → 秒并 insert timer(0x76)，更新/插入 effect 列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865ec0e | _ZN5CUser18_processEffectItemEP14CStackableItemjj
#include "CUser/_processEffectItem.inc"
}

// ORIG _ZN5CUser22_processContinuousItemEjP14CStackableItem @ 0x0865ea62 (T)
#include "CUser/_processContinuousItem.inc"
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
}

// ORIG _ZN5CUser17CheckCoolTimeItemEm @ 0x0865e994 (T)
// 当前角色 cooltime 列表中存在 itemIdx==param_1 且 expireTime>curSec 的条目。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865e994 | _ZNK5CUser17CheckCoolTimeItemEm
#include "CUser/CheckCoolTimeItem.inc"
}

// ORIG _ZN5CUser22UpdateCoolTimeItemInfoEj @ 0x08658b2a (T)
// 组 SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO（0x100 占位：characNo@+0x0 +
// count@+0x4 + [0x194]条目簇）投递队列 2。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658b2a | _ZN5CUser22UpdateCoolTimeItemInfoEj
#include "CUser/UpdateCoolTimeItemInfo.inc"
}

// ORIG _ZN5CUser20UpdateEffectItemInfoEj @ 0x08658d90 (T)
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658d90 | _ZN5CUser20UpdateEffectItemInfoEj
#include "CUser/UpdateEffectItemInfo.inc"
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
}

// ============================================================================
// ORIG 0x08692b84 CUser::SendAradEventItemLog(unsigned long, int)：
// "Arad_StatisticsEventItemList" 脚本 needStatistics(itemIndex) 命中时，
// 构造 16 字节 Sig{itemIndex,count,accId,characNo(-1)} 经内部流 0x20 投递
// LOGDB_Q。反编译见 docs/class_func_reports/CUser/SendAradEventItemLog.md。
// ============================================================================

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08692b84 | _ZN5CUser20SendAradEventItemLogEmi
#include "CUser/SendAradEventItemLog.inc"
}

// ============================================================================
// 任务 QP 发送链（RestoreUserQuestStubs：Dispatcher_Debug_Clear_Quest 依赖）
// ============================================================================

// ORIG T _ZN5CUser12sendCharacQpEv @0x0868ac24：
// GetCharacExpandData(0xb)（CQuestShop）非空 → shop->sendCharacQp(this, 0)。
#include "CUser/sendCharacQp.inc"
}

// ORIG T _ZN5CUser13incQuestPointEiib @0x0868ac66：
// GetCharacExpandData(0xb) 非空 → old=getQp()、addQp(qp)；!send →
// shop->sendCharacQp(this,0)；m_historyLog.QPAdd(old, questIdx, qp)。
#include "CUser/incQuestPoint.inc"
}

// ============================================================================
// ORIG T 0x0864d90a：从 m_list711ec（+0x711ec list<int>）删除 state。
// ============================================================================
#include "CUser/RemoveRestrictedGoods.inc"
}

// ============================================================================
// ORIG T 0x0867ceae：全部角色 m_needCheckOverEquipItem（+0xe3b）置 1。
// ============================================================================
#include "CUser/SetNeedCheckOverEquipItem.inc"
}
// ============================================================================
// LinkMisc5 批次（散点链接期未定义符号，第四阶）
// ============================================================================

// ---- CSecurityCard 状态 getter（ORIG W 0x0822ef0e/0x0822ef1a/0x0822ef26，
//      objdump 忠实转写；声明补于 CUser.h WongWork::CSecurityCard）----
#include "CUser/isActivate.inc"
}

#include "CUser/isCertified.inc"
}

#include "CUser/getFailCnt.inc"
}

// ============================================================================
// ORIG T 0x0868991c _ZN5CUser17isTradePunishTypeEi：
//   type==0 → m_tradePunishType(+0x79628)!=0；否则按位与命中。
// ============================================================================
#include "CUser/isTradePunishType.inc"
}

// ============================================================================
// ORIG T 0x086624f8 sz 0x30 _ZN5CUser15send_RedeemInfoEv：
//   GetCharacExpandData(6) 取 Redeem_Item::CRedeemItem 后 Send_RedeemList。
// ============================================================================
#include "CUser/send_RedeemInfo.inc"
}

// ============================================================================
// ORIG T 0x08664ae8 sz 0x167 _ZN5CUser13calc_lev_statER15_Additioal_info：
//   基础块 add 后按当前等级循环叠加成长属性块（≤14 首转/≤49 二转/之后三转）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08664ae8 | _ZN5CUser13calc_lev_statER15_Additioal_info
#include "CUser/calc_lev_stat.inc"
}

// ============================================================================
// ORIG T 0x0868ff04 sz 0x9c _ZN5CUser21CalcurateUserMaxLevelEv：
//   全角色 m_level 最大值 clamp 到 70 后 SetUserMaxLevel。
// ============================================================================
#include "CUser/CalcurateUserMaxLevel.inc"
}

// ============================================================================
// ORIG T 0x0868e704 sz 0x2bb _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard：
//   管理等级/点数 + 雇佣兵列表逐个序列化（剩余时间、状态、名字、三状态字节）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868e704 | _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard
#include "CUser/SetMercenaryInfoPacket.inc"
}
// ============================================================================
// ORIG T 0x0868fec8 _ZN5CUser14SetUserMaxLevelEi：写 m_field8ebd8（0 → 1）。
// ============================================================================
#include "CUser/SetUserMaxLevel.inc"
}

// ============================================================================
// ORIG T 0x0868fe2c _ZN5CUser14SetManageLevelEi：超上限取脚本 m_manageLevelMax。
// ============================================================================
// b28d-solo-CUser AE: ORIG 0x0868fe32-0x0868fe61 `cmp slot,%eax; setl %al; test %al,%al; je`
//   无 bool 槽（sub $0x8）；具名 less 落槽（-0x9）⇒ 删局部内联（probe_bool a6/a7 对照）。
//   条件/体各完整重估 G_CDataManager() 链（P183）。| _ZN5CUser14SetManageLevelEi
#include "CUser/SetManageLevel.inc"
}
// ============================================================================
// ORIG T 0x0868fe72 _ZN5CUser14SetManagePointEi：点数封顶升级所需后写入。
// ============================================================================
#include "CUser/SetManagePoint.inc"
}

// ============================================================================
// ORIG T 0x0868ffa0 sz 0x86 _ZN5CUser20CalcurateManagePointEv：
//   Σ 各角色等级对应管理点数后 SetManagePoint。
// ============================================================================
#include "CUser/CalcurateManagePoint.inc"
}

// ============================================================================
// ORIG T 0x08690026 sz 0x25 _ZN5CUser20CalcurateManageLevelEv：
//   重算点数、查等级、SetManageLevel；返回等级是否提升。
// ============================================================================
#include "CUser/CalcurateManageLevel.inc"
}

// ============================================================================
// ORIG T 0x08690b58 _ZN5CUser21SendManageLevelUpNotiEv：空包 header(0,0x19f)。
// ============================================================================
#include "CUser/SendManageLevelUpNoti.inc"
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
}

// ============================================================================
// InventoryMemory::reset @ 0x0822d472（W 0x83）：四段 memset（0x4a58/0x1905/
// 0x39aa/0x2438，合计 0xc13f），末字节 +0xc13f 不清，逐段忠实转写。
#include "CUser/reset_4.inc"
}

// ============================================================================
// Sanicova::CPad::setCheckTime @ 0x084ebe20（W 0xe 字节）：写 +0x28 m_checkTime28。
// ============================================================================
namespace Sanicova
{
#include "CUser/setCheckTime.inc"
}
}

// ============================================================================
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08651628 | _ZN5CUser40UpdateCurCharacVisibleToCharacViewMemoryEv
#include "CUser/UpdateCurCharacVisibleToCharacViewMemory.inc"



}
// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// 定义置于本 TU（与 W7_UserOnlyC.cpp 中 CPacketVerifier::Init 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
#include "CUser/reset_5.inc"
}

#include "CUser/check.inc"
}

// ============================================================================
// nexon_packet::stHeader::stHeader（ORIG 0x086941cc W，magic=0xaa; length=0;
// cmd=0）。定义原在 nexon_packet.cpp，迁入本 TU（stPacketC1 的 callee）——
// stPacketC1 所在 TU 不再含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing
// 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
// ============================================================================
#include "CUser/stHeader.inc"
}
