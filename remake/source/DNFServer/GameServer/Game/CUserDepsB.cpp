// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CUserDepsB.cpp —— CUser 第二组恢复（独占 TU；不触碰 CUser.cpp）。
//
// 八个方法均逐指令对照 docs/class_func_reports/CUser/<方法>.md 的
// ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   1. change_equip_expand          ORIG T 0x0865f382 _ZN5CUser19change_equip_expandEiiiiPi
//   2. eventDeliveryBuyItem         ORIG T 0x0866463c _ZN5CUser20eventDeliveryBuyItemEiRK10Inven_Item
//   3. _processLevelReward          ORIG T 0x0868745e _ZN5CUser19_processLevelRewardEjjc
//   4. _postQuestTriggerItem        ORIG T 0x086689ee _ZN5CUser21_postQuestTriggerItemEPK5Quest
//   5. processLevelUpEventReward    ORIG T 0x08663cc0 _ZN5CUser25processLevelUpEventRewardEb
//   6. processLevelUpEvent          ORIG T 0x0869115a _ZN5CUser19processLevelUpEventEv
//   7. getCurCharacInvenCheckSum    ORIG T 0x0864e0b6 _ZNK5CUser25getCurCharacInvenCheckSumEv
//   8. getCurCharacSkillCheckSum    ORIG T 0x0864e114 _ZNK5CUser25getCurCharacSkillCheckSumEv
//
// 另提供上述函数直接依赖且当前无任何定义的 ORIG 叶子（无报告者均以 objdump
// 反汇编转写并在注释标注地址；全部符号经 ORIG nm 核实存在）：
//   - CUser::GetServerGroupToPvP                     T 0x0864a97c
//   - DeliveryMsg C2/setBaseData                     W 0x086978c0 / 0x086978ec
//   - BuyItemDeliveryMsg C2                          W 0x0869795a
//   - LevelUpRewardItemTableKey(int,int,int)         W 0x081b8896
//   - stLevelUpRewardItemScript::get_data            W 0x081b89de（无报告，objdump）
//   - stLevelUpRewardItemScript::get_data_when_growup W 0x081b895a（无报告，objdump）
//   - CLevelRewardTable::GetRewardItemNo/GetItemCount/GetMailTitle/GetMailString
//                                                    W 0x086976f6/0x08697712/0x08697730/0x08697750（无报告，objdump）
//   - CLevelRewardTableMgr::IsOn                     W 0x08697792（无报告，objdump）
//   - CLevelRewardTableMgr::IsChallengingJob         T 0x08687792（有报告 docs/class_func_reports/CLevelRewardTableMgr/IsChallengingJob.md）
//   - CLevelRewardTableMgr::GetProperLevelRewardTableR T 0x08687e62（有报告 GetProperLevelRewardTableR.md）
//   - NewAccountLevelUpEventToJob::processLevelUpToJob T 0x081b82a4（有报告
//     NewAccountLevelUpEventToJob/processLevelUpToJob.md；定义已迁 NewAccountLevelUpEventToJob.cpp）
//   - LevelupSupportEventManger::CheckNewStep        T 0x08147164（无报告，objdump）
//   - LevelupSupportEventManger::UpdateEventInfo     T 0x081471c6（无报告，objdump）
//   - LevelupSupportEventManger::SendCharacState     T 0x0814701e（无报告，objdump；静态，操作 user->m_levelupEvents）
//   - LevelupSupport2ndEventManger::Process          T 0x08147880（无报告，objdump）
//   - DB_SaveEventLevelupSupport::makeRequest        T 0x0844cc2e（无报告，objdump）
//   - DB_EventAccountCharacterLevelUpOnceGift::makeRequest T 0x08449f66（无报告，objdump）
//
// [S426 单文件泳道 2026-09-09 复验/翻正] 清单 7 件：
//   * _postQuestTriggerItem      DIFF→AE：else 内层 if 改 P223 负条件 goto 形，
//     复现 ORIG 0x08668bfd `je <nop 岛>` + body 尾 `jmp <join>` + nop（203=203）。
//   * GetRewardItemNo/GetItemCount  [b28d-PoolSmall 归零] S426 的「P231 纯 register
//     形参」旋钮实为**反效果**：register 令 4.4 把 jobIdx→%edx / levelIdx→%eax，
//     this 再要 %eax ⇒ 多 `mov %edx,%ecx; mov %eax,%edx` 两条 regmove（13 条 DIFF）。
//     **删 register** 后 4.4 自然分配 job→%ecx / lv→%edx / this→%eax，两件各
//     11=11 **IDENTICAL**（ORIG 0x086976f6 / 0x08697712）⇒ TU diff 5→3。
//     ⇒ 补 P248「裸 register」反例：二维下标 `m[j][l]` 形参加 register 恒退化。
//   * processLevelUpEventReward  [L29] DIFF→AE：真因是**实参求值序**（旧 union +
//     reinterpret_cast<MSG_MAILBOX_SEND*>(history.raw) 的 cast 令 4.4 先物化
//     实参再 add 算 this）；改 MSG_MAILBOX_SEND 真类型局部 + `&history` 直传即
//     复现 ORIG 的 this-first+lea（旧判 N107 取址 lea-vs-add **作废**）。
//   * eventDeliveryBuyItem       [L29] NEAR→AE：裸 `register` 具名 serverGroup
//     复现 ORIG callee-saved 角色（旧判 N7 封顶**作废**）。
//   * _processLevelReward        [F3b-C05③ 2026-09-10] DIFF→**AE（164=164 逐条
//     全等）**：旧判 N121 作废 —— 3 处残差（table==0 / levelIdx<0 / jobIdx<0）
//     是平铺 `if (c) return;` 在 4.4 各产一组 nop+jmp 中继岛；改正向包裹
//     （`if (table != 0) {…}` / `if (levelIdx >= 0 && jobIdx >= 0) {…}`）后
//     4.4 把 fail 路径 jump-thread 直达共享尾出口（ours 175→164 = ORIG）。
//   * UpdateEventInfo            [L29] 维持 **N7/P75**：uid/characNo 装载寄存器
//     （ORIG characNo→%eax、我们 →%edx）；register/unsigned 两形实测均打成 DIFF。
//   * _Z41__static_initialization_and_destruction_0ii  [L29] **N123 撞名假象**：
//     本 TU GI 与 ORIG 0x0869376b 副本逐条同构（compare_tu 按名索引取到他 TU 的
//     0x0807ee76 副本）⇒ 结构性噪音，verdict 豁免。
//   L29 收口四数：identical=74 ae=107 near=1 diff=2（p167 七符号全 CLEAR）；
//   [F3b-C05③ 2026-09-10] 现四数：identical=74 ae=108 near=1 diff=1
//   （_processLevelReward AE；diff 仅剩 _Z41 撞名假象）；
//   无 .h 改动（仅本 TU 顶部补 `#include "PacketTypes.h"`）；无子子代理。
// ============================================================================

#include <string.h>
#include <stdlib.h>

#include "CUser.h"                    // CUser/cUserHistoryLog/m_checkSumSeed/
                                      // m_historyLog/m_codeHackCheck/getCharacLevelUpGift_AccountOnce
#include "CUserCharacInfo.h"          // getCurCharacR/getPVPResultR/getCurCharacInvenR/
                                      // getCurCharacSkillR/getCurCharacNo/get_charac_job/
                                      // get_charac_level/getCurCharFirstGrowType/PvpResultType
#include "Quest.h"                    // Quest 完整类型（m_mailTitle +0x10c /
                                      // m_mailContents +0x110 / m_mailItems +0x114）
#include "Inven_Item.h"               // Inven_Item（m_field1/m_addInfo/m_addInfo2/m_fieldb）
#include "CInventory.h"               // makeCheckSum(int,int) const / OS_API::snprintf
#include "SkillSlot.h"                // SkillSlot::makeCheckSum(int) const
#include "CDataManager.h"             // G_CDataManager/find_item/m_levelUpReward/getItemList
#include "DataManagerInitDepends.h"   // CLevelRewardTable/CLevelRewardTableMgr
#include "CGuildServerProxy.h"        // CEventManager/CEventBase::IsEventing/DeliveryMsg/
                                      // BuyItemDeliveryMsg
#include "NewAccountLevelUpEventToJob.h"  // processLevelUpEvent 调用的完整类
#include "GlobalData.h"               // s_event_manager/s_stream_pool/s_msgq_mgr/s_GM_Manager
#include "LevelupSupportEventManger.h"
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CItemList.h"                // create_item/ENUM_ITEM_CREATE_TYPE
#include "CItem.h"                    // CItem::make_item/get_index
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf clear/put_*/finalize）
#include "CStreamGuard.h"             // CStreamGuard（operator*/operator<<）
#include "PacketTypes.h"              // MSG_MAILBOX_SEND 完整定义（sizeof 0x145，
                                      // m_name11@+0x11/m_itemId39@+0x39/m_extra3d@+0x3d）
#include "MsgQueueMgr.h"              // MsgQueueMgr::put/DB_Q
#include "DB_SaveEventLevelupSupport.h"
#include "DB_EventAccountCharacterLevelUpOnceGift.h"
#include "GameWorld.h"                // G_GameWorld/IsIntegratedPvPBaseChannel
#include "CEnvironment.h"             // G_CEnvironment/m_serverEnvir.m_serverGroup
#include "CGM_Manager.h"              // CGM_Manager::GetCurrentGmMode
#include "CParty.h"                   // CParty::Reset_party_overlapped_drop_ratio
#include "CExpandEquipslot.h"         // CExpandEquipslot::Change_Equip
#include "HeroMissionEvent.h"         // HeroMissionEvent::processMission/MissionNo

// ----------------------------------------------------------------------------
// 1. ORIG T 0x0865f382 sz 0x291。
// space==3：slot∈[10,0x15] 校验 → dstSpace!=0 拒绝 → 交易中拒(0x13) →
// change_equip(dstSpace,(short)dstSlot,slot)，成功写 *outSlot=slot。
// space==0x12：dstSpace!=0 拒绝 → 交易中拒(0x13) → 扩展槽 Change_Equip(this,
// dstSpace,dstSlot,space,slot)，成功写 *outSlot=dstSlot。
// space==0 且 dstSpace==3：dstSlot∈[10,0x15] 与 slot∈[3,0x38] 双重校验 → 交易
// 中拒(0x13) → change_equip(space,(short)slot,dstSlot)，成功写 *outSlot=
// dstSlot 并对队伍重算重叠掉率。space==0 且 dstSpace==0x12：交易中拒(0x13)
// → slot∈[3,0x38] 校验 → Change_Equip(this,space,slot,dstSpace,dstSlot)，
// 成功写 *outSlot=dstSlot。其余返回 1。
#include "CUserDepsB/change_equip_expand.inc"
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x0866463c sz 0x85。
// 构造 BuyItemDeliveryMsg（W 0x0869795a），setBaseData(2, accId, characNo,
// serverGroupToPvP) 后填 +0x10 deliveryType、+0x14 &item，经
// CEventManager::eventDeliveryNotify(msg.m_type, msg) 转发（首参取消息体
// 首字段）。
#include "CUserDepsB/eventDeliveryBuyItem.inc"
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868745e sz 0x269。
// 等级奖励邮件：CLevelRewardTableMgr 开启且表就绪、非挑战职业时按 (jobIdx,
// levelIdx) 取 itemNo/count，组 Inven_Item 后经 ReqDBSendNewSystemMail 投递
// （itemNo==0 仅金额走第三参；==1 固定道具直写字段后 ResetItemAttr；其余经
// find_item 的虚槽 2 make_item 复制并 set_add_info）。
#include "CUserDepsB/_processLevelReward.inc"
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x086689ee sz 0x32a。
// Quest 触发物品邮寄：m_mailItems(+0x114) 按 (itemNo,count) 成对遍历；
// itemNo==0 且 count>0 走金额路径（count 入第三参）；否则 find_item 经
// make_item 组道具（count 写 m_addInfo2，第三参置 0）。空列表时寄一封默认
// 道具邮件。标题/正文取 m_mailTitle(+0x10c)/m_mailContents(+0x110)。
#include "CUserDepsB/_postQuestTriggerItem.inc"
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x08663cc0 sz 0x3b9。
// 升级奖励发放：repeat 事件 0x5d IsEventing 门控后按 (level, job, growType)
// 查 stLevelUpRewardItemScript（growup 时聚合收集，否则单项），逐条 create_item
// 并邮寄（标题 game_server_msg_187 / 正文 game_server_msg_188）；邮寄成功回执
// 为 0 时补写 MSG_MAILBOX_SEND 历史（+0x11 标题/+0x39 item idx/+0x3d add_info）
// 经 cUserHistoryLog::SendMail 落日志。
#include "CUserDepsB/processLevelUpEventReward.inc"
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x0869115a sz 0x113。
// 升级事件分发：repeat 0x68 IsEventing 门控后二次取事件调
// processLevelUpToJob(user,false)；0x9e → LevelupSupportEventManger::
// UpdateEventInfo；0xa3 → LevelupSupport2ndEventManger::Process；最后无条件
// HeroMissionEvent::processMission(user, MissionNo 4, 0)。
#include "CUserDepsB/processLevelUpEvent.inc"
}

// ----------------------------------------------------------------------------
// 7. ORIG T 0x0864e0b6 sz 0x5d。
// 无当前角色返回 0；否则 makeCheckSum(pvpWinPoint(+0x58), m_checkSumSeed(+0x796e0))，
// 结果即 makeCheckSum 尾部 eax（CInventory::makeCheckSum 已改为 int 以复现）。
#include "CUserDepsB/getCurCharacInvenCheckSum.inc"
}

// ----------------------------------------------------------------------------
// 8. ORIG T 0x0864e114 sz 0x49。
// 同上种子；对当前角色 SkillSlot 调 makeCheckSum(seed)。
#include "CUserDepsB/getCurCharacSkillCheckSum.inc"
}

// ============================================================================
// 依赖叶子（ORIG 直调；地址与语义见文件头清单）
// ============================================================================

// ORIG T 0x0864a97c sz 0x30：PvP 统合频道取 CUser+0x79630（CodeHackCheckStorage
// +0x34 整数覆盖值），否则 CEnvironment+0x378 = m_serverEnvir.m_serverGroup。
#include "CUserDepsB/GetServerGroupToPvP.inc"
}

// DeliveryMsg::DeliveryMsg（ORIG W 0x086978c0，四字段清零）定义在
// CtorProducers_1.cpp（NL4 unit-at-a-time 塌缩机制：同 TU 可见 callee 使
// BuyItemDeliveryMsgC1 outgoing 帧塌缩 sub $0x4，ORIG sub $0x18）。

// ORIG W 0x086978ec：四参顺序写 +0x00..+0x0c。
#include "CUserDepsB/setBaseData.inc"
}

// ORIG W 0x0869795a：基类构造后清零两扩展字段。
#include "CUserDepsB/BuyItemDeliveryMsg.inc"
}

// ORIG W 0x081b8896：(level 截断 ushort, job 字节, grow 字节)。
#include "CUserDepsB/LevelUpRewardItemTableKey.inc"
}

// ORIG W 0x081b89de：find 命中拷贝两 int 返回 1，未命中返回 0。
#include "CUserDepsB/get_data.inc"
}

// ORIG W 0x081b895a：等价于嵌套 functor GetGrowUpRewardItemFunctor（W 0x081b88fe
// operator()：同 grow(+3)、同 job(+2)、pair.lv <= key.lv 才 push_back）的
// for_each（0x081b8b11）；返回 !out.empty()。
#include "CUserDepsB/get_data_when_growup.inc"
}

// ---- CLevelRewardTable 访问器（偏移与权威布局逐指令核对）----

// ORIG W 0x086976f6：this+8+(jobIdx*0x46+levelIdx+0x44)*4 → m_rewardPrimary 下标。
#include "CUserDepsB/GetRewardItemNo.inc"
}

// ORIG W 0x08697712：this+(jobIdx*0x46+levelIdx+0x2bc)*4 → m_rewardSecondary 下标。
#include "CUserDepsB/GetItemCount.inc"
}

// ORIG W 0x08697730：this+0x14c8+levelIdx*21（21B/级 strncpy 槽）。
#include "CUserDepsB/GetMailTitle.inc"
}

// ORIG W 0x08697750：this+0x1a86+levelIdx*0x100（0x100/级 strncpy 槽）。
#include "CUserDepsB/GetMailString.inc"
}

// ORIG W 0x08697782 sz 0xf：movzbl 0x6086(%eax) —— 读 m_jobAffection。
#include "CUserDepsB/GetJobAffection.inc"
}

// ORIG W 0x08697792 sz 0xc：movzbl (this) 直返全字（无 setne）——m_flag/返回
// 均 bool 时同形（P32 直返）；调用点 xor 形按 P292 要求 bool 返回。
#include "CUserDepsB/IsOn.inc"
}

// ORIG T 0x08687792 sz 0x96：表缺失或 job∉[0,10] 返回 0；否则以 (job+1, 0) 查
// 奖励项号/数量，任一非 0 即挑战职业。
#include "CUserDepsB/IsChallengingJob.inc"
}

// ORIG T 0x08687e62 sz 0xc：return m_table。
#include "CUserDepsB/GetProperLevelRewardTableR.inc"
}

// ORIG T 0x08687e6e sz 0xa：mov $0x0,%eax —— 恒返回 0（参数未使用）。
#include "CUserDepsB/GetProperLevelRewardTableR_1.inc"
}

// ORIG T 0x08687d18 sz 0x54：遍历 m_table 指针数组（0x46 项），
// 元素为 0 → 空槽终止返回 false；元素 ==1 → 有起始奖励。
#include "CUserDepsB/IsHaveStartLevelReward.inc"
}

// ORIG T 0x08687d6c：size()==1 或 v[0]==0 ⇒ 全服生效；否则遍历含 serverIdx 才 true。
#include "CUserDepsB/IsOnServer.inc"
}

// ---- LevelupSupport 域 ----

// ORIG T 0x08147164 sz 0x62：level ∈ m_script.m_stepItems 即新档位可达。
#include "CUserDepsB/CheckNewStep.inc"
}

// ORIG T 0x081471c6 sz 0x19c。操作对象是 user->m_levelupEvents（CUser+0x8d0e4
// vector<EventInfo>，lea 0x8d0e4(%eax) 实证）：门槛达标且未完成(≠2)的档位标记
// state=1 并 DB 上报；末尾按 CheckNewStep 发送状态包（level 或 0）。
#include "CUserDepsB/UpdateEventInfo.inc"
}

// ORIG T 0x0814701e sz 0x146（静态）。包头 0x22a：byte 数量 + 每档 (int level,
// byte state) + byte state 参数，finalize(true) 后 Send。
#include "CUserDepsB/SendCharacState.inc"
}

// ORIG T 0x08147880 sz 0x24d。
// 二档升级支持：脚本职业匹配当前角色后按等级取奖励列表（map find 于
// m_script.m_stepItems），逐项 find_item → get_index 写 item idx → make_item →
// set_add_info((short)count) → ReqDBSendNewSystemMail（第七参固定 0xf）。
#include "CUserDepsB/Process.inc"
}

// ---- DB 请求封装 ----

// ORIG T 0x0844cc2e sz 0x10a："DBThread.cpp"/0xc25a 流，头 0x34f + 四 int，
// 入 DB_Q（QUEUE_IDX 2）。流序 = (accId, characNo, level, state)：
// W8LT_DBTask.cpp DB_SaveEventLevelupSupport::dispatch 读流 characNo→level→state
// （ORIG 0x0844cafe）实证第 2/3 参语义，形参名据此修正。
#include "CUserDepsB/makeRequest.inc"
}

// ORIG T 0x08449f66 sz 0x158："DBThread.cpp"/0xb9d9 流，头 0x319、int -1、
// uint accId、short serverGroup、int characNo、short job、short level，入 DB_Q。
#include "CUserDepsB/makeRequest_1.inc"
}
