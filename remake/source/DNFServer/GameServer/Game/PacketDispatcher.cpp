// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PacketDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— PacketDispatcher / CLocalChinaErrorDispatcher
// 全部方法逐指令转写自 docs/class_func_reports/PacketDispatcher/ 与 objdump；
// 注册表取自 ORIG ctor 0x8590a2e 的 new+槽位序列（顺序一致）。
// ============================================================================
// b28d-solo-S400 终态（fresh /tmp/tu_PacketDispatcher.o，check_tu_routed COMPILE OK，
//   compare_tu clean 口径，2026-09-09）：
//   本泳道清单 83 件 = **83/83 达标**（TU 计数 identical=393 ae=1645 near=0 diff=2；
//   基线 393/1562/1/84。余 2 DIFF 均为清单外：`__static_initialization_and_destruction_0ii`
//   =N123 噪音、`_ZNSt12_Vector_baseIN38MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON...D1Ev`
//   =Main 另派）。TU 内全部 171 个 dispatch_template 实例 + doDispatch 亦全 AE/IDENTICAL。
//   · **N7 族（82 件 dispatch_template）全歼**：S153/S161/S177/S316/S339 的「N7 结构性
//     封顶」判词作废。根因不是「POD msg 物化序不可翻」，而是 **MSG_* 的继承建模**：
//     ORIG 的 MSG_* 派生自**非空** MSG_BASE（基类含 char m_head[0xd]），调用点写隐式
//     派生转换 `process(user, msg, param)`；我方原为 standalone POD（自带 m_head[0xd]）
//     + `*reinterpret_cast<MSG_BASE*>(&msg)`。446 后端对后者恒 msg-first + 虚目标 %ecx，
//     对前者恒 param-first + %edx（= ORIG 0x825ab73..0x825ab93 逐条同构）。
//     探针 /tmp/pd_p5.cpp 三形定案：x1 空基类+派生自带 m_head ⇒ 仍 msg-first；
//     x2 非空基类+派生不含 m_head ⇒ ORIG 形；x3 空基类+static_cast ⇒ 仍 msg-first。
//     /tmp/pd_p2.cpp v1/v7..v14（C-cast / 双 cast / 语句表达式 / 逗号 / void* 两层 /
//     先用 param）全部仍 msg-first ⇒ 唯一开关是「基类非空」。
//     MSG=MSG_BASE 实例在隐式 `msg` 形下与 cast 形产物全等（/tmp/pd_p3.cpp w1≡w2）
//     ⇒ N-MSG_BASE 族零回归。落地（Main 代落 + S400 复核）：PacketDispatcher.h
//     MSG_BASE 保留 char m_head[0xd]；PacketTypes.h 79 类 `: public MSG_BASE` 且删除
//     自带 m_head（MSG_CHANGE_SKILLSLOT 的 0xe 包头 → 基类 0xd + unsigned char m_head0d
//     保 +0xe 负载）；模板实参改隐式 `msg`。
//   · **doDispatch AE**：S339「DenyPacket 物化点不可翻」判词作废——P225 推广「拆嵌套 if」
//     是钥匙：`if (A||B) { if (DenyPacket(cmd)) return 1; }` 逐条复现 ORIG
//     0x8594b9a..0x8594bd5 的 mov$1/mov$0/test/je 物化岛（探针 /tmp/pd_p4.cpp n1；
//     n2..n9 的三目/`!!`/语句表达式/while/`==true`/`!=0` 六形确实全败）。p167 40/40 CLEAR。
//   · 两件曾 NEAR 的头级件均已经跨泳道修复（已上报 Main，S400 复核 AE）：
//     ① MSG_DELETE_ITEM（116:116）：Main 落「本体 +0x4bf int m_checkSum，sizeof 0x4c3
//        + 删 DisPatcherCore DeleteItemMsgTailView」⇒ AE（相关 check_state/C1/D0/D1 全 AE）。
//     ② MSG_CLIENT_SPEC_STATISTIC（119:119）：缩 CUser.h Client_Spec −0x18 ⇒ AE。
//   · P0：本文件 0 处 `register T x asm("reg")` 绑定（S400 grep 复核）；dispatch/
//     doDispatch 的裸 `register` 为 P231/P307 合规存储类提示（非绑定形）。
//   · 无子子代理（R4）。
// b28d-solo-S153 终态（fresh /tmp/tu_PacketDispatcher.o，compare_tu clean 口径，2026-09-09）：
//   清单 115 件 = 7 IDENTICAL_AE + 1 NEAR + 107 DIFF。
//   · N-SIZE 族已落地：Main 落 PacketDispatcher.h `ParamBase` pack(1)；本泳道落 PacketTypes.h
//     20 类尾部 pad/对齐（21 类实测槽差归零，逐类证据见各类注释与
//     /tmp/b28d_solo_PacketDispatcher_sizepatch.md）⇒ MSG_RENAME_CREATURE/MSG_ENCHANT/
//     MSG_COMPATIBILITY_INDEX 等 3 件 DIFF/NEAR→AE。
//   · 余 2 类曾封 N：MSG_CLIENT_SPEC_STATISTIC（需缩 CUser.h Client_Spec，冻结头）；
//     MSG_DELETE_ITEM 已于检查点14 并入本体（m_checkSum@+0x4bf，sizeof 0x4c3）⇒ AE。
//   · 剩余 107 DIFF = N7（4.4.6 虚目标 %ecx vs ORIG %edx + &msg/&param 物化序；三后端 +
//     13 源形穷举封顶）与 N-MSG_BASE（空体 ctor 独立对象多发 3 条 call；P298 承载器因发明
//     242 符号回退）。
//   已落地修法：①doDispatch 返回 bool（P292，ORIG 0x8594f7e xor $1 调用点实证）；
//     ②dispatch 逐条重排（int packetClass/short cmd/内联 checker+history/嵌套 if）；
//     ③doDispatch 去 clientCrc 局部（P183）+ antibot && 拆嵌套 if。
//   已回退：P298 承载器 DispatchMsgHolder（消 N-MSG_BASE 3 条 call，但为每个 MSG
//     实例化出 ORIG 无的 C1/C2 共 242 符号，违反禁发明符号 ⇒ Main 回退）。
//   下表为 patch 落地前快照（行名/ORIG 地址仍有效）；逐件最新 verdict/ndiff 以
//   /tmp/b28d_solo_PacketDispatcher_result.tsv 为准。
//   行格式：[ORIG 地址] verdict 归因 MSG/Param/state
// [825aae2] DIFF N7                     MSG_BUY_ITEM/BuyItemParam/3
// [8259b8c] DIFF N7                     MSG_GET_ITEM/stGetItemParam/5
// [8257dc4] DIFF N7                     MSG_USE_COIN/stUseCoinParam/0
// [8259d7a] DIFF N7                     MSG_DROP_ITEM/stDropItemParam/3
// [825819c] DIFF N7                     MSG_SORT_ITEM/stOnSortItemParam/3
// [8257bd8] DIFF N7                     MSG_USE_SKILL/stUseSkillParam/0
// [8261a2a] NEAR AE-PENDING-PATCH       MSG_BINGO_QUIZ/stBingoQuiz/3
// [82484f4] NEAR N7+SIZE(patch后10)      MSG_PURIFY_ITEM/ParamBase/0
// [8258d3c] DIFF N7                     MSG_REPAIR_EQUIP/stRepairEquipParam/0
// [82597aa] DIFF N7                     MSG_USERINFO_REQ/stOnRecoverStaminaParam/3
// [8258576] DIFF N7                     MSG_USER_SET_POS/ParamBase/0
// [8258768] DIFF N7                     MSG_SET_USER_AREA/stSetAreaParam/3
// [82579ea] DIFF N7                     MSG_WALKOUT_PARTY/stWalkoutPartyParam/3
// [8265abc] DIFF N7                     MSG_FINISH_LOADING/stFinishLoadParam/0
// [8258956] DIFF N7                     MSG_SELECT_DUNGEON/stSelectDungeonParam/0
// [825838a] DIFF N7                     MSG_SET_READY_STATE/stOnSetPvPReadyParam/0
// [824e786] NEAR AE-PENDING-PATCH       MSG_FATIGUE_ATTENDANCE/stFatigueAttendance/0
// [825a428] DIFF N7                     MSG_DECREASE_DURABILITY/stOnUseEquipParam/0
// [824ff82] DIFF N7                     MSG_REGENERATION_RANDOM_OPTION/stRegenROIResult/0
// [8260d4c] NEAR N7+SIZE(patch后10)      MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE/ParamBase/0
// [8254bf4] NEAR N7+SIZE(patch后10)      MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON/ParamBase/2
// [812c546] NEAR N7+SIZE(patch后10)      CMDPacketStruct::_STReqEventDungeonDestoryObject/CMDPacketStruct::_STResEventDungeonDestoryObject/3
// [8259302] DIFF N7                     MSG_MOVE_MAP/stMoveMapParam/0
// [8259462] DIFF N7                     MSG_PARTY_SET_INFO/stSetPartyInfo/3
// [82523a0] DIFF N7+SIZE(patch后10)      MSG_ENCHANT/stEnchantParam/0
// [8261ee6] DIFF AE-PENDING-PATCH       MSG_STACKABLE_ACTION_USE/stStackableActionUse/3
// [82577fc] DIFF N7+SIZE(patch后12)      MSG_ADD_FRIEND/stAddFriendParam/3
// [8258f28] DIFF N7+SIZE(patch后12)      MSG_PVP_TIMEOUT/stOnPvPTimeOut/0
// [825760e] DIFF N7+SIZE(patch后12)      MSG_REMOVE_FRIEND/stRemoveFriendParam/3
// [8259114] DIFF N7                     MSG_MOVE_ITEMSPACE/stMoveItemParam/3
// [825a614] DIFF N7                     MSG_CREATE_CHARACTER/stCreateCharac/2
// [825a2ce] DIFF N7+SIZE(patch后12)      MSG_DELETE_CHARACTER/stDeleteCharac/2
// [8248cc8] NEAR AE-PENDING-PATCH       MSG_OPEN_CERAPACKAGE/stOpenCeraPackage/0
// [824b272] NEAR N7+SIZE(patch后16)      MSG_COMPATIBILITY_INDEX/ParamBase/0
// [825a76e] DIFF N7                     MSG_CHANGE_SKILLSLOT/stOnChangeSkillParam/3
// [82556c2] NEAR N7+SIZE(patch后10)      MSG_CLIENT_SPEC_STATISTIC/stClientSpecStatisticParam/2
// [8259998] DIFF N7                     MSG_USERINFO_REQ/stOnGetUserInfoParam/1
// [826153a] DIFF AE-PENDING-PATCH       MSG_COMBO_SKILL/stComboSkill/3
// [8259f68] DIFF N7+SIZE(patch后20)      MSG_DIE_CHARACTER/stDieCharacInDungeon/0
// [824ba44] DIFF N7+SIZE(patch后24)      MsgLimitNpcBuyItem/LimitNpcBuyItem/3
// [824d69c] DIFF N7+SIZE(patch后20)      MsgMercenaryReturn/SendMercenaryReturnResult/0
// [824d24a] DIFF N7+SIZE(patch后20)      MsgMercenaryCompetition/SendMercenaryCompetitionResult/0
// [8257fb0] DIFF N7                     MSG_BASE/stStartGameParam/0
// [8259650] DIFF N7                     MSG_BASE/stLeavePartyParam/0
// [824cdf8] DIFF N7                     MSG_BASE/stQuickPartyParam/5
// [824e336] DIFF N7                     MSG_BASE/stGrowthPowerParam/0
// [824d436] DIFF N7+SIZE(patch后16)      MSG_BASE/ParamBase/2
// [824aa8c] DIFF N7+SIZE(patch后16)      MSG_BASE/ParamBase/3
// [8266ba4] DIFF N7                     MSG_SECU_RPY/SecuCardRpyParam/2
// [825a0c2] DIFF N7+SIZE(patch后12)      MSG_DELETE_ITEM/stDeleteItemParam/3
// [8256854] DIFF N7                     MSG_MAILBOX_SEND/stMailboxSendParam/0
// [82549c2] DIFF N7+SIZE(patch后12)      MSG_CREATE_DISJOINT_STORE/stCreateDisjointStoreParam/3
// [824edec] DIFF N7                     MSG_ITEM_HYPER_LINK_MESSAGE/stItemHyperLinkMessage/3
// [8261c8e] DIFF N7                     MSG_BASE/stBingoReward/3
// [8252506] DIFF N7+SIZE(patch后16)      MSG_BASE/stEnchantParam/0
// [82500dc] DIFF N7                     MSG_BASE/STQuestShopParam/0
// [82488e0] DIFF N7                     MSG_BASE/stGrowthCreature/3
// [8253c42] DIFF N7                     MSG_BASE/stGuildCargoParam/0
// [8256a4c] DIFF N7                     MSG_BASE/stMailboxOpenParam/0
// [8256e44] DIFF N7                     MSG_BASE/stCallMemberListParam/3
// [8266000] DIFF N7                     MSG_BASE/CheckSecurityProtection/0
// [8255ab8] DIFF N7                     MSG_BASE/stRequestBlackListParam/3
// [824f3da] DIFF N7                     MSG_BASE/stCharacSlotExtendEffect/0
// [8253a56] DIFF N7                     MSG_BASE/stGuildCargoHistoryParam/0
// [8249d0a] DIFF N7                     MSG_BASE/PcRoomPlayTimeRewardParam/3
// [825401c] DIFF N7                     MSG_BASE/stCloseDisjointStoreParam/3
// [82545e6] DIFF N7                     MSG_BASE/stRepairDisjointMachineParam/3
// [824c162] DIFF N7                     MSG_BASE/stTournamentRewardSelectState/0
// [82543f8] DIFF N7                     MSG_BASE/stUpgradeDisjointMachineParam/3
// [824b470] DIFF N7                     MSG_BASE/stImageCommunicationEquipmentUse/3
// [824c568] DIFF N7                     MSG_BASE/ReqResult/3
// [82665d0] DIFF N7                     MSG_SCAN_DLL/ScanBotByDllParam/0
// [82661ec] DIFF N7                     MSG_SCAN_DRV/ScanBotByDrvParam/0
// [8253e2e] DIFF N7+SIZE(patch后24)      MSG_TELEPORT/stTeleportParam/3
// [82663de] DIFF N7+SIZE(patch后16)      MSG_MOTION_HACK/ParamBase/0
// [82667c2] DIFF N7                     MSG_SCRIPT_HASH/ScriptHashParam/0
// [824a8a0] DIFF N7                     MSG_COLLECT_ITEMS/stCollectItems/0
// [8257038] DIFF N7+SIZE(patch后12)      MSG_MEMBER_SECEDE/stMemberSecedeParam/3
// [824f9ae] DIFF N7                     MSG_TITLEBOOK_GET/ParamBase/3
// [824fba2] DIFF N7                     MSG_TITLEBOOK_PUT/ParamBase/3
// [8256094] DIFF N7+SIZE(patch后20)      MSG_BOSS_DIE_CHECK/stBossDieCheckParam/5
// [82532a4] DIFF N7+SIZE(patch后16)      MSG_CHANGE_EMOTION/stChangeEmotionParam/3
// [8256282] DIFF N7                     MSG_HATCH_CREATURE/stHatchCreature/3
// [82513e6] DIFF N7+SIZE(patch后16)      MSG_DENY_JOIN_GUILD/ParamBase/0
// [8252e54] DIFF N7                     MSG_ENCHANT_BY_BEAD/stEnchantByBeadParam/3
// [824d888] DIFF N7                     MSG_ITEM_DICTIONARY/stItemDictionary/3
// [824b856] DIFF N7+SIZE(patch后20)      MSG_OBJECT_BRING_UP/stObjectBringUp/3
// [82504c0] DIFF N7                     MSG_EQUIPSLOT_SWITCH/stEquipslotSwitch/3
// [82511f4] DIFF N7+SIZE(patch后16)      MSG_GUILD_ATTENDANCE/ParamBase/0
// [82517ca] DIFF N7+SIZE(patch后16)      MSG_CANCEL_JOIN_GUILD/ParamBase/0
// [8251db8] DIFF N7+SIZE(patch后16)      MSG_DONATE_GUILD_FUND/ParamBase/3
// [82515d8] DIFF N7+SIZE(patch后16)      MSG_APPROVE_JOIN_GUILD/ParamBase/0
// [82558ca] DIFF N7                     MSG_CHANGE_LETTER_STAT/stChangeLetterStatParam/3
// [825722c] DIFF N7                     MSG_MEMBER_ENTER_REPLY/stMemberEnterReplyParam/3
// [824f5c6] DIFF N7+SIZE(patch后16)      MSG_ACHIEVEMENT_TRIGGER/ParamBase/3
// [824fd96] DIFF N7+SIZE(patch后20)      MSG_CHANGE_HOST_WARROOM/stChangeHostWarRoom/0
// [8255cac] DIFF N7+SIZE(patch后12)      MSG_DELETE_TO_BLACKLIST/stDeleteToBlackListParam/3
// [824f7ba] DIFF N7+SIZE(patch后16)      MSG_TITLEBOOK_OTHERUSER/ParamBase/3
// [825420a] DIFF N7                     MSG_ENTER_DISJOINT_STORE/stEnterDisjointStoreParam/3
// [825367e] DIFF N7                     MSG_GUILD_CARGO_POP_ITEM/stGuildCargoPopItemParam/0
// [8256668] DIFF N7+SIZE(patch后20)      MSG_MAILBOX_EXTRACT_ITEM/stMailboxExtractParam/0
// [8257420] DIFF N7+SIZE(patch后12)      MSG_REQUEST_MEMBER_ENTER/stRequestMemberEnterParam/3
// [8252c68] DIFF N7                     MSG_SECRET_SHOP_BUY_ITEM/stSecretShopBuyItemParam/0
// [82496c4] DIFF N7                     MSG_SERIAROOM_DECO_EVENT/stSeriaRoomDecoEvent/0
// [82486f2] DIFF N7                     MSG_SERIAROOM_DECO_EVENT/stSeriaRoomDecoEvent/3
// [8253492] DIFF N7                     MSG_GUILD_CARGO_MOVE_ITEM/stGuildCargoMoveItemParam/0
// [825386a] DIFF N7                     MSG_GUILD_CARGO_PUSH_ITEM/stGuildCargoPushItemParam/0
// [8255ea0] DIFF N7+SIZE(patch后12)      MSG_REGISTER_TO_BLACKLIST/stRegisterToBlackListParam/3
// [82547d4] DIFF N7+SIZE(patch后20)      MSG_REQUEST_DISJOINT_ITEM/stRequestDisjointItemParam/3
// [8265e0e] DIFF N7                     MSG_ABNORMAL_USE_STACKABLE/AbnormalUseStackable/0
// [8265c1c] DIFF N7                     MSG_FLOAT_RDATA_MODULATION/ParamBase/0
// [8252a7c] DIFF N7                     MSG_SECRET_SHOP_OPEN_CLOSE/stSecretShopOpenCloseParam/0
// [825089c] DIFF N7                     MSG_WARROOM_WP_PER_MONSTER/stWarRoomWp/0
// [8250a88] DIFF N7+SIZE(patch后20)      MSG_PVP_MISSION_COMBO_CLEAR/ParamBase/3
// [8251008] DIFF N7                     MSG_GOLD_TAKE_INCREASING_AMOUNT/stGoldTakeIncreasingAmount/0
// [82506ae] DIFF N7                     MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE/stExpandEquipslotFlagUpdate/3
// [824c9ba] DIFF N7                     MSG_AVATAR_DISJOINT_RANDOM_REWARD/stAvatarDisjointRandomReward/0
// [8594e52] AE   S161 三修               PacketDispatcher::dispatch
// [8594922] DIFF S339(gap 0,300:300;残余 DenyPacket 块序 6 行) PacketDispatcher::doDispatch
// ============================================================================
// b28d-solo-S161 终态（fresh /tmp/tu_PacketDispatcher.o，check_tu_routed/compare_tu clean，2026-09-09）：
//   本泳道清单 111 件 = **4 AE + 1 NEAR + 106 DIFF**（S153 移交 7AE+1NEAR+107DIFF 的 115 件
//   清单口径不同：本清单仅含 IPacketDispatcher 实例 + dispatch/doDispatch）。
//   TU 全局：identical=394 ae=1537 near=1 diff=109（无回归；S153 移交基线 ae=1533/near=4）。
//   · **N-SIZE 落地（Main 授权，PacketTypes.h 14 类尾部 pad）**：stSelectStriker 0x4→0x5、
//     stStackableActionUse 0x7→0xb、stTournamentRewardSelect 0x4→0x5、MSG_TELEPORT +2、
//     MSG_BOSS_DIE_CHECK +2、MSG_DIE_CHARACTER +1、MSG_CHANGE_HOST_WARROOM +1、
//     MSG_OBJECT_BRING_UP +1、MSG_MAILBOX_EXTRACT_ITEM +2、MSG_REQUEST_DISJOINT_ITEM +1、
//     MSG_PVP_MISSION_COMBO_CLEAR +1、MsgLimitNpcBuyItem +1、MsgMercenaryCompetition +1、
//     MsgMercenaryReturn +1 ⇒ 3 件 NEAR→AE（MSG_SELECT_STRIKER / MSG_STACKABLE_ACTION_USE /
//     MsgTournamentRewardSelect）+ 11 件 DIFF 槽差归零。余类封 N：MSG_CLIENT_SPEC_STATISTIC
//     （需缩 CUser.h Client_Spec）；MSG_DELETE_ITEM 已于检查点14 并入本体
//     （m_checkSum@+0x4bf，sizeof 0x4c3）⇒ AE。
//   · **dispatch DIFF→AE**（本波最大收口，三处新范式，见函数头判词与 /tmp/pd_probe6.cpp）：
//     ① N18a 翻转：`void***` 下标形恒两段 ⇒「单层解引用 `*reinterpret_cast<CheckFn*>((char*)*(void**)p + 0x3c)`
//        + 纯 `register` 存储类提示」得 `mov (%eax),%esi` 单段直落（P231/P307 扩展）；
//     ② N7 翻转：CheckFn 第 2 形参改 `unsigned int` ⇒ `mov %eax,%edx` 中转（P211 档①）；
//     ③ 实参序翻转：局部改 `ENUM_PACKETCLASS packetClass`（实参无转换）⇒ packetClass 载入
//        推迟到 cmd 压栈之后（ORIG 0x8594f65 序）。
//   · doDispatch：S161 收 fn 单段直落 + N109 角色（gap 2），残余 N82/P1974 封顶。
//   · 模板 110 件：N7（POD msg 物化序）85 件 + N-SIZE 16 件 + N-MSG_BASE 24 件（有交叠，
//     逐件以 /tmp/b28d_solo_PacketDispatcher_result.tsv 为准）；详细复核见
//     PacketDispatcher.h 模板判词 [b28d-solo-S161 复核终态] 段。
// ============================================================================
// b28d-solo-PacketDispatcher（S177 复核终态，fresh /tmp/tu_PacketDispatcher.o，
// check_tu_routed/compare_tu clean 口径，2026-09-09）：
//   清单 /tmp/b28d_tu/PacketDispatcher.tsv 107 件 = **106 DIFF + 1 NEAR**（无 AE/IDENTICAL）；
//   逐件归因见 /tmp/b28d_solo_PacketDispatcher_result.tsv：
//     · N7（81 件，ndiff=5/8/10，116:116）：POD msg 的 &msg/&param 物化序——4.4.6 对
//       **trivial 默认构造**类型预计算 &msg 到 %eax 再算 &param 到 %edx（msg-first），
//       ORIG 4.1 恒「算一个压一个」（param-first）且虚目标落 %edx（我方 %ecx）。
//       S177 独立复核（探针 /tmp/pd_probe8..13.cpp，全部 4.4.6 后端 `-B cc1plus446bin/`）：
//       ① 空基类派生（仍 trivial）⇒ 仍 msg-first（pd_probe8 f_der）；
//       ② 用户拷贝构造 / 用户空析构（non-trivial 但默认构造 trivial）⇒ 仍 msg-first
//          （pd_probe9 f_copy/f_dtor）；
//       ③ always_inline 空 ctor（GCC 折叠为 trivial）⇒ 仍 msg-first 且帧 16 对齐进位
//          +0x10（pd_probe10 f_ai 0x38→0x48）；
//       ④ 用户空 ctor + throw()（P216 折叠 call）⇒ 帧不变但仍 msg-first（pd_probe12）；
//       ⑤ **唯一 param-first 通路 = 默认构造 non-trivial**（真实继承有用户 ctor 的基类，
//          pd_probe8 f_derbase / pd_probe11 f_nocast/f_cast）——其代价恒为 3 条
//          `lea+mov+call MSG::C1Ev`，即落入 N-MSG_BASE，净负；
//       ⑥ 4.4.7 后端（无 -B）整 TU near=67（ae 1537→1471）更差 ⇒ 现行 4.4.6 路由已最优。
//       ⇒ 结构性封顶（S161 判词复核成立，补 6 探针硬证据）。
//     · N-MSG_BASE（24 件，ndiff=96，116:119）：MSG=MSG_BASE 实例（MSG_BASE 有用户空
//       ctor ⇒ non-trivial ⇒ 物化序已与 ORIG 一致），仅多 3 条 msg ctor call；ORIG 4.1
//       前端把空体 ctor 展开为 0 条（P215）。S177 复核：`__attribute__((always_inline))`
//       可消 3 条 call（DIFF→NEAR）但帧 16 对齐进位 +0x10 且丢失 `_ZN8MSG_BASEC1Ev`
//       符号（其他 TU STBaseRequest 消费）；`throw()` 实测 TU 计数零变化（无效）；
//       char 缓冲载体（存储缓冲实验）对 82 件 MSG_* 实例大幅回退（ae −87）⇒ 回退，封顶。
//     · N-SIZE（2 件）：MSG_CLIENT_SPEC_STATISTIC（NEAR，msg 槽差 0x18，需缩冻结头
//       CUser.h Client_Spec）——Main 已否决，封顶；MSG_DELETE_ITEM 已并入本体
//       （m_checkSum@+0x4bf，sizeof 0x4c3）⇒ AE。
//   doDispatch（本波增量，DIFF 保留）：把 S161 的 `char framePad[4]` 改为末尾
//     `const int limit = 0x7d0;`——同时复现 ORIG 0x08594dd0 的 `movl $0x7d0,-0xc(%ebp)`
//     死写入（P171）与浅槽整体深 4（ORIG dispatcher@-0x20 vs 我方 -0x1c），且两者共用
//     同一 4 字节槽 ⇒ 帧仍 0x17390（framePad+limit 并存会使帧 16 对齐进位到 0x173a0，
//     已实测回退）；并把 errResult 的 if-else 链改 `switch`——复现 ORIG 0x08594c9a 的
//     `mov -0x18(%ebp),%eax; cmp $0x1,%eax; je; cmp $0x2,%eax; jne` 寄存器决策链
//     （P251/P286；改前 4.4 恒内存直比 `cmpl $1,-0x18`）。结果：orig 300 / our 301，
//     p167 callsite 40/40 CLEAR，行级 diff 347→253；残余仅 head+7 的 disp-vs-lea/add
//     折叠（ORIG `mov -0x2a(%ebp),%edx` vs 我方 lea+add+mov；`head+7`/`&head[7]` 两形
//     实测同形 = N102/N4 族）与日志串/全局地址重定位显示（N124 假报），verdict 仍 DIFF（封顶）。
//   P0：本文件无 `register T x asm("reg")` 绑定；dispatch/doDispatch 的裸 `register`
//     为 P231/P307 合规存储类提示（非绑定形）。
// ============================================================================
// b28d-solo-S316 独立复核终态（fresh /tmp/tu_PacketDispatcher.o，check_tu_routed/
//   compare_tu clean 口径，2026-09-09）：TU identical=394 ae=1537 near=1 diff=109
//   missing_in_game=952（与 S177 基线逐值一致，零回归）；清单 107 件逐件复核结论：
//     · 81 件 N7（ndiff=5，116:116）：POD msg 的 &msg/&param 物化序 + 虚目标寄存器。
//       S316 复跑 MSG_BUY_ITEM 实例确认差异仅 `mov (%eax),%edx` vs `%ecx` +
//       两次 lea 的求值序（4.4 预计算两地址到 %eax/%edx ⇒ 虚目标落 %ecx；ORIG
//       逐个 lea+store ⇒ %edx 空闲）。封顶依据不变（pd_probe8..13 六件 +
//       446h46/446h47/447 三后端穷举）。
//     · 24 件 N-MSG_BASE（ndiff=96，116:119）：4.4 对 MSG_BASE 独立对象恒发
//       lea+mov+call `_ZN8MSG_BASEC1Ev` 3 条。**S316 实测了「消 call」通路并回退**：
//       PacketDispatcher.h 加纯 typedef 模板 PacketMsgStorage<T>（MSG_BASE 特化为
//       char[sizeof(MSG_BASE)]）可消 3 条 call（ndiff 96→5，指令数 116:116 与 ORIG
//       全等），但 ORIG 4.1 按 P204 无条件发射该 W 符号而 4.4 只在使用点发射 ⇒
//       本 TU 丢 `_ZN8MSG_BASEC1Ev`，identical 394→393（Main 条件「identical+ae
//       不得下降」⇒ 已完整回退，.h 与基线逐字节一致）。残余 N7 差异仍在，故即便
//       保留也换不到 AE ⇒ 结构性封顶（消 call 与保符号不可兼得）。
//     · 1 件 MSG_CLIENT_SPEC_STATISTIC（NEAR）：N-SIZE，Main 已否决（需缩冻结头
//       CUser.h Client_Spec）⇒ 封顶；MSG_DELETE_ITEM 已并入本体（m_checkSum@+0x4bf，
//       sizeof 0x4c3）⇒ AE。
//     · doDispatch：head+7 三处地址折叠用具名 pack(1) 视图收口（our 301→297，
//       详见函数头判词），verdict 仍 DIFF（残余 setne 物化/短路岛/switch jmp）。
//   P0：无 `register T x asm("reg")` 绑定（本波 grep 复核 0 处）。
//   消费 TU fresh 复编（Main 条件，check_tu_routed 全部 COMPILE OK 0 错误）：
//     Dispatchers 997/2352/2/388（=S292 基线逐值一致）、GlobalData 832/1637/9/54
//     （=S292 基线）、GmCore 59/128/0/4（=S292 基线）、DispLinkA2 43/87/0/3、
//     GameMain 63/200/3/12、CmdPacketStructCtors 1/47/0/1、advancealtar_Manager
//     183/262/1/3、InterDispatcher 121/1320/5/144 ⇒ 零回归（本波最终 .h 与基线
//     逐字节一致，git status 仅 PacketDispatcher.cpp 为 M）。
// ============================================================================
// b28d-solo-PacketDispatcher S339 终态（fresh /tmp/tu_PacketDispatcher.o，check_tu_routed/compare_tu
//   clean 口径 + p167 调用点差集，2026-09-09）：
//   TU：identical=394 ae=1537 near=1 diff=109 missing_in_game=952（与 S316/S177 基线
//   逐值一致，零回归）；COMPILE OK 0 错误；本波未改任何 .h。
//   清单 /tmp/b28d_tu/PacketDispatcher.tsv 107 件逐件 verdict 与归因见
//   /tmp/b28d_solo_PacketDispatcher_result.tsv（本轮全部复核，无未处理件）：
//     · 81 件 N7（DIFF ndiff=5，116:116）——POD msg 的 &msg/&param 物化序 + 虚目标
//       寄存器；抽样 MSG_BUY_ITEM/MSG_GET_ITEM 逐条比对确认（仅 5 行差）。封顶依据
//       不变（13 源形 + 446h46/446h47/447 三后端 + pd_probe8..13 六探针），并新增
//       S339 四变体探针 /tmp/pd_probe_real2.cpp（A 基线 / B `(void)&param` /
//       C `reinterpret_cast<ParamBase*>(&param)` / D 具名 `ParamBase& pb`）——四者
//       输出逐字节相同（msg-first + 虚目标 %ecx）；对照 /tmp/pd_probe_n7c.cpp 简化
//       类型得 param-first ⇒ 该 lowering 由 Param/ParamBase 真实类型布局触发，
//       非源码形可控 ⇒ 结构性封顶。
//     · 24 件 N-MSG_BASE（DIFF ndiff=94，116:119）——MSG_BASE 独立对象多 3 条
//       lea+mov+call `_ZN8MSG_BASEC1Ev`（ORIG 4.1 前端展开空 ctor 为 0 条，P215）。
//       抽样 MSG_BASE/stEnchantParam(state3) 确认 116:119。封顶不变。
//     · 1 件 MSG_CLIENT_SPEC_STATISTIC（NEAR，119:119）——N-SIZE，Main 已否决
//       （需缩 CUser.h Client_Spec）；MSG_DELETE_ITEM 已并入本体（m_checkSum@+0x4bf）⇒ AE。
//     · doDispatch（DIFF ndiff 205→6，**300:300 gap 归零**，p167 40/40 CLEAR）：
//       三项新钥匙落地（详见函数头 [b28d-solo-PacketDispatcher S339 终态]）：
//       ① P225 `while (v != 0) { …; return 0; }` 复现两处槽比零 setne/test 物化（+4）；
//       ② P171 `unsigned int tick0;` 去初始化（ORIG 无该死 store，−1）；
//       ③ P198① `int errResult = 0; errResult = …;` 预清零（+1）+ P46 switch 降序书写
//          （ORIG 物理叶块 case2 先、case1 走 je 目标）⇒ 4.4 从 `je+jmp` 翻为
//          `jne` fall-through（−1）。残余 6 行 = DenyPacket 段 `&&` 左操作数物化点
//          （ORIG 先物化 (A||B) 再 call DenyPacket；4.4 恒把 DenyPacket 内联进短路链），
//          五形穷举（P226 两形/三目/`!!`/while 条件）不可翻 ⇒ P197④ 封顶。
//   P0：本文件无 `register T x asm("reg")` 绑定（S339 grep 复核 0 处）；dispatch/
//     doDispatch 的裸 `register` 为 P231/P307 合规存储类提示。
//   本 TU 复编期间 S334 曾广播 CDataManager.h 两处 ctor 加 throw()（P216）；本 TU 复编
//     TU 计数与基线逐值一致 ⇒ 该头改动对本泳道零影响（本文件无消费点）。
// ============================================================================

#include "PacketDispatcher.h"
#include "Dispatchers.h"           // 具体业务 Dispatcher_* 完整类型
#include "GlobalData.h"            // s_msgq_mgr / s_antibotChecker_ / s_systemTime_
#include "DisPatcherCore.h"         // 本批补齐的 79 个 DisPatcher_*/ARAD 等声明
#include "MsgQueueMgr.h"
#include "CAppLoadChecker.h"       // 叶子批次（ORIG CAppLoadChecker 簇）
#include "PacketTracer.h"          // 叶子批次（CPacketTracerInstance/AddTcpLog）
#include "PacketSecurity.h"        // 叶子批次（DenyPacket/_NS_PI_Check_CMD/
                                   //  _NS_PI_2ND_MakeChecksumTo1Byte2）
#include "UserPacketCounter.h"     // message_counter::CUserPacketCounter
#include "Encryption.h"            // Encryption::crc32N
#include "CSystemTime.h"           // getCurTickCount/update
#include "GameWorld.h"             // G_GameWorld()->GetChannelType 等
#include "CGameManager.h"          // G_CGameManager()->getUser
#include "LogManager.h"
#include "cHistoryTrace.h"         // cMyTrace
#include <string.h>
#include "CEnvironment.h"    // GlobalInstance<T>

char* NumberToString(unsigned int value, int radix);  // ORIG _Z14NumberToStringji

// ======== CLocalChinaErrorDispatcher ========

// ORIG 0x8128c34：map 构造后按 ORIG 顺序插入 60 组命令检查开关。
#include "PacketDispatcher/CLocalChinaErrorDispatcher.inc"
}

#include "PacketDispatcher/_PacketErrorCheck.inc"
}

// ORIG 0x8595094 W：仅析构 map。
#include "PacketDispatcher/dtor_CLocalChinaErrorDispatcher.inc"
#include "PacketDispatcher/ProcAntiAddiction.inc"
#include "PacketDispatcher/ProcSafeModeUser.inc"
#include "PacketDispatcher/ProcSaefModeAfterLogin.inc"
#include "PacketDispatcher/ProcLockInSeriaRoom.inc"
#include "PacketDispatcher/doErrorDispatcher.inc"
}

// ======== PacketDispatcher ========


message_counter::CUserPacketCounter* PacketDispatcher::pUserPacketCounter_ = 0;
    // ORIG B 0x9472d18：静态指针；ORIG 由 DispatchThread::dispatch(0x847e2d2) 每轮
    // 以栈上局部 CUserPacketCounter(get_file_name(), "UserPacketCounter") 地址赋值（0x847e34f 实证）。

#include "PacketDispatcher/PacketDispatcher.inc"
}

#include "PacketDispatcher/dtor_PacketDispatcher.inc"
}

// ORIG 0x85948e2 逐指令转写。
#include "PacketDispatcher/get_dispatcher.inc"
}

// ORIG 0x8231f2e W：尾调 get_dispatcher。
#include "PacketDispatcher/getDispatcher.inc"
}

// ORIG 0x825f72e：CPacketTracer 单例记录 (cls, cmd) TCP 日志。
#include "PacketDispatcher/log_recv_packet.inc"
}

// ORIG 0x825f658：五队列余量 → CAppLoadChecker 检查/请求 DB。
#include "PacketDispatcher/server_load_monitoring.inc"
}

// ORIG 0x8594e52 逐指令转写。
#include "PacketDispatcher/dispatch.inc"
}

// b28d-solo-S161 判词 [ORIG 0x8594922]：DIFF，orig 300 / our 298 insn（gap=2），
//   p167 callsite 40/40 CLEAR（0 missing / 0 extra）⇒ 语义完整（§6.2 三件套口径）。
//   本波已收：①clientCrc 局部删除，`*(unsigned int*)(head+7)` 两处内联（P183 去缓存，
//   消 -0x34 槽）；②antibot `fn(...)!=0 && flag!=0x25c` 拆嵌套 if（P225 推广，消 jmp 岛）。
//   ③S161 新收（同 dispatch）：vtbl+0x30 取函数指针改「单层解引用 + 纯 register」形 ⇒
//   `mov (%eax),%esi` 单段直落（N18a 消除，0x859497d 复现），并连带恢复 cmd→%ebx /
//   fn→%esi 的 callee-saved 角色（N109 消除）；④CheckFn 第 2 形参 unsigned int 保
//   `mov %eax,%edx` 中转（P211 档①）。
//   残余 N 族（本波穷举实测）：①短路物化岛缺失——ORIG 0x08594ba8 段
//   `mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`（a/b 双落点）；P226 语句表达式
//   （our 303）与 P231 register bool（our 304）虽可造该岛，但 4.4 恒多 `mov %eax,%ebx`
//   中转且 a/b 共落点，并连锁 gap 2→4/5 ⇒ 回退直短路形（gap 最小），并入 N7/P197④ 族；
//   ②局部槽整体平移 4（ORIG -0x20/-0x24/-0x31 vs 我方 -0x1c/-0x20/-0x2d）：S161 曾用
//   P232 前置 `char framePad[4]` 撑槽（行级 diff 342→308；`[8]`/`double` 反劣 380）；
//   **S177 已改为末尾 `const int limit = 0x7d0;`（一槽两用：复现 ORIG -0xc 死 store +
//   浅槽整体深 4），行级 diff 再降至 253**；残余仅 head+7 的 disp-vs-lea/add 折叠
//   （N102/N4）与 errResult 已修（switch 复现寄存器决策链）。返回类型 int→bool 见 PacketDispatcher.h
//   （P292，dispatch 调用点 0x8594f7e xor $1 实证）。
//   [b28d-solo-S316 复核终态] head+7 的 disp-vs-lea/add 残余**已收**：旧形
//   `*(unsigned int*)(head + 7)` 三处恒 lea+add+mov（9 条），ORIG 首次 lea+add 后
//   第二/三处为 disp 直读（0x08594ab1 `mov -0x2a(%ebp),%edx`、0x08594ada
//   `mov -0x2a(%ebp),%ecx`）⇒ 改具名 pack(1) 视图 `DispatchPacketHead`（7B 前缀 +
//   dword 校验和 + 2B 尾，sizeof 0xd / align 1 ⇒ 槽基 -0x31 与帧 0x17390 零变化），
//   首次仍用指针算术 `*(unsigned int*)((char*)&head + 7)` 保 lea+add、后两处改
//   `head.m_checksum` 折叠 disp ⇒ 该段逐条同形，our 301→297（orig 300）。
//   残余（封顶，不因本波引入）：①`cmpl $0,-0x18/-0x1c(%ebp); setne %al; test %al,%al;
//   je` 两处 bool 物化（ORIG 4.1 槽比零物化；4.4 恒直跳，P197④/N122 族）；
//   ②DenyPacket 短路岛块序（P226/P197④）；③switch 无 default 时 4.4 恒 `je`+`jmp`
//   而 ORIG 用 `jne` fall-through（1 条）。verdict 仍 DIFF（gap -3 全部来自 ① 的
//   4 条 setne/test 减 ③ 的 1 条 jmp；§6.2 三件套：p167 40/40 CLEAR）。
//   [b28d-solo-PacketDispatcher S339 终态] **gap 归零：orig 300 / our 300**（S316 的 our 297 + 3）。
//     三项新钥匙（均本波实测，fresh /tmp/tu_PacketDispatcher.o）：
//     ① P225 槽比零物化：`if (v != 0)` 4.4 恒直跳 `je`，改 `while (v != 0) { …; return 0; }`
//        （循环体全路径 return ⇒ 语义等价）即精确复现 ORIG 0x08594c8d/0x08594ce8 两处
//        `cmpl $0,槽; setne %al; test %al,%al; je`（+4 条；探针 /tmp/pd_probe_while.cpp
//        f_if vs f_while 对照）。
//     ② P171 照抄：ORIG 0x08594bd5 `cmpw $0x25b` 前**无** `movl $0,-0x1c(%ebp)`
//        ⇒ `unsigned int tick0;` 去初始化（非 0x25b 路径不读该槽，语义等价；−1 条）。
//     ③ P198① 预清必写 + P46 叶块序：ORIG 0x08594bef 有 `movl $0,-0x18(%ebp)`
//        （errResult 声明预清，位于 tick0 分支之后）⇒ `int errResult = 0; errResult = …;`
//        两句式（+1 条）；且 ORIG 物理叶块 case2(0x8594c40) 在 case1(0x8594cbb) 之前、
//        case2 走 `jne` fall-through（0x8594c3a）⇒ switch 改**降序书写**（case 2 先），
//        4.4 即从 `je case2; jmp 出口`（多 1 条）翻为 ORIG 的 jne fall-through（−1 条）。
//     残余 6 行（封顶）：DenyPacket 段 ORIG 先物化 `(A||B)` 再 `call DenyPacket`，
//        4.4 恒把 DenyPacket 内联进 `&&` 短路链（块内 6 行互换，指令数 300:300 全等）。
//        五形穷举（P226 只包左项 305 / 整条件语句表达式 300 同形 / 三目 297 /
//        `!!` 300 同形 / while 条件 303）均不能移动物化点 ⇒ P197④ 镜像族封顶。
//     三件套：p167 callsite 40/40 CLEAR；clean 逐行比对 6/300 行差（全为该块内互换）；
//        verdict 仍 DIFF（compare_tu 逐行口径），TU 计数零回归（identical 394 / ae 1537）。
#include "PacketDispatcher/doDispatch.inc"
}

// ---- 全局单例访问器（ORIG W 0x0823453e）----
PacketDispatcher* G_PacketDispatcher()
{
    return GlobalInstance<PacketDispatcher>::inst_ptr();
}
