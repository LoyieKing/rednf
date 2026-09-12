// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InterDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "InterDispatcher.h"
#include "Inter_DispatcherAradOnly.h"  // Inter_DispatcherAradOnly（槽 732）
#include "Inter_DispatcherTaiwan.h"  // Inter_DispatcherTaiwan（槽 846）

// ============================================================================
// b20 hardcode 裸偏移清零账（本 TU，§4.3）
// 已字段化·零扰动（check_tu 计数不变，12 处）：
//   Inter_MailBox_Req_CheckReceiver +0x3fb9→m_resultFlag；Inter_LoadInven +0x14758→&m_14758、
//   +0x4044→&m_4044（拆 pad 保 sizeof）；Inter_MonitorCallMemberList +0x12→&m_12；
//   Inter_GuildCargoPushItem +0x42→m_42、Inter_GuildCargoPopItem +0x41→m_41（UpgradeSeparateInfo）；
//   Inter_LoadDeathTowerBestRecord +0x10→&m_10、Inter_DeathTowerLoadTopRanker +0xfb0→&m_padfb0；
//   Inter_NoticeGuildNameOnGuildCreate +0x16→m_pad16、Inter_UpdateSecuServiceState +0x10→m_pad10。
// 记 N·保留裸偏移（字段化实测破坏 AE/无法零扰动，各因如下，逐处见行内注释）：
//   · 视图边界外子结构基址（补字段使 sizeof(view) 变化→尺寸敏感装载破坏 AE）：
//     LoadCharac +0x68b(TAG_CHARAC_DATA)、LoadDungeonClear +0x10、NotifyAuctionMail +0x18(RandomOption)、
//     MonitorCallGuildMembers/AllMembers +0x34、MembersNext/AllMembersNext +0x17（ST_Guild_Mem_Info*）、
//     RequestTagCharacInfo +0x7d(TAG_CHARAC_INVEN_DATA)/+0x40a1(CHARAC_QUEST_SHOP_DATA)。
//   · 透传/外部结构视图：MailBox CheckReceiver m_payload(+0x35/0x46/0x63/0x64/0x67/0x68/0x6c/0x6e，
//     MSG_MAILBOX_SEND/MULTI 视图)、IPGResult reqData+0x2d8(SIPGData)、GameOption etcOpt+0x48/0x174/0x2a0
//     (CETCOption 内 CQuickChattingOption/CChattingEmoticonConfig)。
//   · 工具链求值序依赖：MonitorAddBuddy strlen (char*)pPkt+0x12（ORIG add 形 vs put_str lea 形，AE 依赖此差）。
//   · 多指针参 reload：SecurityCardIssue +0x37/+0x5e（拆 pad 使 setSecurityCardInfo 双指针参装载重排破坏 AE）。
//   · 最小镜像类隔离：NpcLimitBuyItemInfo checkError +0x16（LimitNpcBuyItemInfo 无字段，§4.3 例外同 CItemLock）。
//   · 共享头占位待属主：AuctionResultBidding PrePayInfo +0x08/0x0c/0x10/0x18/0x1c/0x24
//     （CPrePayInfoMgr.h m_data[0x28]，字段化需 pack(1)+u64 对齐验证、超本 TU 证据，交属主批次）。
// ============================================================================

// ============================================================================
// b28d-lane2 round1（2026-09-10）—— 本 TU 反模式收敛账（子代理 L2，唯一写者）
// 四数：identical 135 / AE 1354→1363 / near 5→1 / diff 127→122（check_tu_routed 实测）。
// 手法（均已 fresh 复编转 AE）：
//  ① **N22 比较操作数方向旧判词作废**：ORIG `mov <field>,%ebx; …call…; cmp %eax,%ebx`
//     只在「字段读**内联进比较**、无具名/register 局部」时复现（探针 /tmp/l2p/p2.cpp fC：
//     裸形 fC 产 `cmp %eax,%ebx`，register/int 具名局部 fA/fB/fD/fF 恒产 `cmp %ebx,%eax`
//     或 mem 直比）。属 P0（ORIG 无槽往返 ⇒ 原码无该局部）。已修 5 件：
//     Inter_NoticeGuildMemo 0x084dad96 / Inter_PowerWarProcessInfo 0x084daf30 /
//     Inter_GuildNoticeGuildMessage 0x084cc650 / Inter_MonitorGuildMemberNamingReply 0x084cc23c /
//     Inter_TodayGuildMember 0x084e37d0（后者还需视图字段 int→unsigned 同型化，
//     P248 档②：ORIG `mov 0xe(%eax),%ebx` 单段 = 字段与 getCurCharacNo() 同型）。
//  ② **删冗余具名 proxy 局部**（P183/P0）：ORIG 的 GetServerProxy 结果直落 (%esp)，
//     具名指针占 4B 并多发 store/load ⇒ Inter_DeleteCharac 0x084bfa2e、
//     Inter_FindFactoryHub 0x084dc9f4、Inter_ChangeGuildName 0x084d50f6（三件转 AE）。
//  ③ **P198 孪生判据**：`int len = 0x33;` 初始化式被 4.4 折成立即数（丢槽）；声明与赋值
//     分两句复现 ORIG `movl $0x33,-0xc(%ebp)` + `mov -0xc(%ebp),%edx`（Inter_FindFactoryHub）。
//  ④ 比较内联化（logFormat 类）：Inter_ReplyJoinPower 0x084d47f4 / Inter_SecedePower 0x084d49f6
//     的 ORIG 比较点无 characNo 槽（`call; mov -0x10(%ebp),%eax; mov 0x12(%eax),%eax; cmp`），
//     旧具名 characNo 多发 store+load 各一。
// 残余族（判据⑪ 判别后归 N；每条附 ORIG 地址 + 指令形）：
//  · R1 视图 disp 折叠族：ORIG 对 `base+i*0x14+K` 发 `lea (%ecx,%eax,1); add $K; mov (%eax)`，
//    4.4 折 `mov K(%eax)`（每处 1 条）。实证 Inter_AutoPunishRuleHackType 0x084d44e0 段
//    （11×add；ORIG 源码为 uint* 索引形）、Inter_NoticeGuildCargo 0x084dd4b2
//    （`add $0x10,%eax; mov 0x3(%eax)` 子结构基址 + disp）。修需把常量并入数组下标或嵌套
//    子结构视图（本波未收敛，P30/N107 族）。
//  · R2 N7/N18a 寄存器角色族：ORIG `mov %eax,%edx`（call 结果落 edx）+ 字段落 %eax，
//    4.4 恒相反（Inter_SecedePower 0x084d49f6）；Inter_MailBox_Req_List 0x084cf845
//    `mov 0x3332(%eax),%ebx` 单段 vs 我方两段（字段类型在权威头 SIG_MAILBOX_LIST，
//    跨 TU 改动需属主 —— 属 P265 反向，后端不成立）。
//  · R3 分支布局族：ORIG 后向 goto 用真跳转 `setg; test %al,%al; jne <body>`，
//    4.4 恒取反 `je <exit>; jmp <body>`（语义等价、仅 jcc 助记符与 jmp 目标互换）。
//    实证 Inter_TcpServerConnect：ORIG 0x084c8e44 `jne 0x84c8dfa` / 我方 `je <exit>`。
//  · R4 N126 整帧平移族：帧内相对间距不变、整体差常数 ⇒ 声明重排无效。
//    Inter_MonitorEventItemUpdate 0x084cefd2：ORIG pPkt@-0x1c / v1a@-0x40 / v16@-0x3c
//    vs 我方 -0x24/-0x20/-0x1c（帧 0x6c vs 0x5c，整组浅移 8）。
//  · R5 P292/P168a③ 跨版本 bool 归一化：ORIG 多一条 `xor $1,%eax; test %al,%al` 死物化，
//    4.4 以 `movzbl` 加宽顶掉（本 TU 表现为 `movzbl`/`setne` 助记符差）。
// ============================================================================

// ============================================================================
// N8InterDispatcher 第八轮（2026-09-11 04:10–05:00，单代理 TU 独占）
//   基线 135/1388/5/93（missing 703）→ **135/1391/4/91**（ae +3、near 5→4、diff 93→91、
//   missing 零增、零劣化；每件改后均 fresh 复编核账）。
//   A. 语义/符号轴（逐件 ORIG 直证）：
//      · P139 prett 符号补齐（父泳道指定 4 处；ORIG 各有同名函数局部静态符号）：
//        Inter_LoginInfo 的 `static const char g_interLoginInfoSig[]`（手写签名字符串）⇒ 删该静态量、
//        10 处 cMyTrace 名参改 `__PRETTY_FUNCTION__`；Inter_CreateCharac（CMyFileLog ctor 名参）、
//        Inter_AuctionResultAskRegistedItemNum（cMyTrace）、
//        Inter_MonitorNoticeOtherChannelChatHyperLink（LogManager::logFormat 第 3 参）
//        三处手写串 ⇒ `__PRETTY_FUNCTION__`。手写串与编译器展开串逐字节相同（含 `virtual ` 前缀：
//        四类 dispatch_sig 均为 ISigDispatcher 覆写虚函数）⇒ .rodata 字节不变、四数不变。
//      · Inter_UpdateServerMessage（NEAR→**AE**）：case 1 cMyTrace 两实参原写形参 `data + 0x10` /
//        `data + 0x48`，ORIG 084da5xx 两处均为具名 pPkt 槽基址（`mov -0x1c(%ebp),%eax; add $0x48,%eax`）
//        ⇒ 改 `pPkt->m_10` / `pPkt->m_48`（P0/P183 正向：ORIG 该点用视图基址、我方误用形参）。
//      · Inter_SetARSInfo（DIFF→**AE**）：Inter_SetARSInfoListView.m_a `unsigned int`→`int`
//        （P290 类型轴；ORIG 084e09xx `mov 0xa(%eax),%ebx` 单段直落 callee-saved = 与使用上下文
//        `(int)pPkt->m_a` 比较/循环上界同型 ⇒ 原码字段是 int；旧 unsigned 令 4.4 多发
//        `mov %eax,%ebx` 中转）。改后 fn_diff blocks 1→**0**、指令数 287→**286** = ORIG。
//      · Inter_OnTimeEventRewardStart（DIFF→**AE**）：Inter_OnTimeEventRewardStartPktView 的
//        m_12/m_16 `int`→`unsigned int`（P290 反向：ORIG 084e0e5x 两处实参**两段**装载
//        `mov 0x12(%eax),%eax; mov %eax,%edx` / `mov 0x16(%eax),%eax; mov %eax,%ecx` ⇒ 字段
//        unsigned 而形参 int 异型；旧 int 同型恒折单段、每处少 1 条）。改后 fn_diff blocks 2→**0**、
//        指令数 118→**120** = ORIG。
//   B. 试形回退（负结论）：
//      · _NS_NonClient_Request：`PacketGuard` 包进内层作用域块（旧头注释 [复原] 建议形）实测
//        **零变化**（blocks 2、ours 68/orig 69 不变）⇒ 维持 N104 记法；
//      · Inter_LoadEventItem：循环体 6 处 `data + i*0x10 + K` 改 `reinterpret_cast<char*>(pPkt) + …`
//        （对齐 ORIG 具名基址形）实测**反劣**（blocks 28→33、ours 158→164）⇒ 已回滚，
//        与 [L9-G PATCH#4] 旧记录同向。
//   C. 审计结论（工具 /tmp/r5/n8/{strall,p167all}.py + 自建位移多重集比对）：
//      · 串层面真缺口 **0**（632 差异函数全为 typeinfo/vtable 二进制碎片与空串假阳性；§6.5.1
//        编码轴：本 TU 已无非 ASCII 字面量 ⇒ UTF-8/EUC-KR 不一致 0 处）；
//      · 非栈帧内存位移常量多重集（字段偏移直证）逐件全等 ⇒ **包字段偏移零缺口**（108 件全扫）；
//      · p167 差集仅 C1/C2·D1/D2 别名与 _Unwind_Resume（EH 结构反向差，N 类）；
//        父泳道提示的 Inter_Terminate「MISSING exit」经核为假阳性（fn_diff blocks=0、puts+exit 两 call 齐备）。
// ============================================================================

// ============================================================================
// b28d-L9InterDispatcherCUser 第七轮（2026-09-11 02:05–03:10，6 子代理并行 + 父泳道串行复编核账）
//   TU 四数 135/1385/4/97（missing 703）→ **135/1388/5/93**（ae +3、diff −4、near 4→5 系一件
//   DIFF→NEAR 的改善方向、missing 零增、**零劣化**）。
//   A. 落地（逐件 ORIG 直证；父泳道每件 fresh 复编核账）：
//      · Inter_ReplyGuildSecede（B#1）：包视图 m_10 int→unsigned 同型化（P211/P265 档③；
//        ORIG 084c97a8 比较点 `mov %eax,%edx` 中转 + 084c997b 单段）⇒ **DIFF→AE**；
//      · Inter_AddPvPBuddyResult（B#2）：len 槽 size_t→int（P265；ORIG 084e1ba8 `lea 0x14(%eax),%edx`）
//        ⇒ **DIFF→AE**；
//      · Inter_AutoPunishRuleBlackIP（A#1）：P30/P235 stride-8 数组成员视角 + P227/P261 去 totalCount/
//        finalCount/maxIp 具名槽（ORIG 084d42b5/084d42bf/084d42fc scale 直取）⇒ **DIFF→AE**（gap −16→0）；
//      · Inter_LoadDimensionInout（A#2）：P235 真实成员数组视角替 REC_BYTE 指针算术 ⇒ 差异点 96→28；
//      · Inter_MonitorCallMemberList（A#3）：P0/P183 成员数组基址具名槽 ⇒ 191→106；
//      · Inter_LoadEventLevelupSupport（A#4）：P235 stride-4 成员视角 + P-L11c 守卫后物化 ⇒ 172→113；
//      · Inter_SetARSInfo（A#5）：P235 stride-0xc 条目视角 ⇒ 差异点 257→258（微增 1，但 gap −11→−1 保留）；
//      · Inter_GuildCargoHistory（G#1）：m_e int→unsigned ⇒ 103→71；
//      · Inter_LoadCerashopAddRestrictType（C#1）：if/else-if→switch（P251/P261）⇒ 29→9（72→71 与 ORIG 齐）；
//      · Inter_ReqGameServerInfo（C#2）：两处 if/else-if→`switch((int)(unsigned char))` ⇒ 131→127；
//      · Inter_LoadInven::_UpdateRentalItems/_UpdateDimensionRentalItems（C#3/#4）：字节索引→具型视图成员
//        （P235/P248）⇒ 178→73 / 174→171（228→225 / 222→219）；
//      · Inter_PvPChannelInfo（G#2/#3）：P-L11c 声明后置 + 循环基址具名槽 ⇒ 95→95（零变化，保留+留痕）。
//      · Inter_NoticeServerMessageInfo（I#1）：4 处基址 data→pPkt 具名槽（ORIG `mov -0x20(%ebp),%eax;
//        lea 0x47(%eax),%ebx`）⇒ 差异点 24→20；
//      · Inter_UpdateServerMessage（I#2）：去具名 proxy 改链式（P183；ORIG `call GetServerProxy;
//        movl $0x5f,0x8(%esp)`）⇒ 差异点 128→**2**，DIFF→**NEAR**；
//      · Inter_VillageAttackedEnd（I#3）：pPkt 声明移到 guard 之后（P-L11c；ORIG 084e… `call
//        IsIntegratedPvPBaseChannel; test %al,%al; je <ret0>` 后才物化）⇒ 268→261；
//      · Inter_GuildBoardData（I#4）：包视图 m_13 int→unsigned 同型化（P211/P265；ORIG 084dee8e
//        比较点 `mov %eax,%edx` 中转）⇒ 248→190。
//      · Inter_RequestJoinGuild（M#1）：两个 strlen 具名局部合并为一个复用 `int strLen`（ORIG
//        Ghidra `local_10 = strlen(...)` 两次赋值单槽）⇒ 101→98；
//      · Inter_AuctionResultItemList / MyRegistedItems / MyBidding（M#2–#4）：守卫由早退形改
//        **包裹形**（P59/P222；ORIG 084d75bc/084d7758/084d78f4 `cmp $0x3; jbe <尾 return 0>` +
//        正文 fall-through）⇒ 差异点 102→**5** ×3（TU 四数不变）。
//      · Inter_MailBox_Req_CheckReceiver（H#1）：载荷具名视图（含互斥区 union）+ 4 处
//        `reinterpret_cast<char*>(sig)+K` 消除（ORIG `mov 0x31(%eax),%eax` / `lea 0x46(%eax),%esi`）
//        ⇒ 差异点 373→367；Inter_MonitorNoticeOtherChannelChatHyperLink（H#2）：两步位移数组成员
//        stride 0x68/元素内 +0x7（ORIG `imul 0x68; add $0x30; add base; add $0x7`）⇒ 389→**233**。
//   B. 语义修复（不受闸门约束，Main 批准三步迁移）：
//      · `Inter_MonitorGuildPointDel::g_lastDeleteTime` 缺初始化器 ⇒ 运行期恒 0（ORIG 为月初 05:00
//        基准时刻）：ORIG static_init 实例 0x084e9004 尾部两条 `call 0x84ce6d2 <..._makeDefaultTimeEv>`
//        + `mov ds:0x943dd88,eax`（由 GI `_GLOBAL__I__Z7getUserj` @0x084e904e 调用）。
//        本文件 getUser() 之后承载带初值定义 + `Inter_MonitorGuildPointDel.cpp` 删无初值定义 +
//        `InterDispatcher.h` 给 `_makeDefaultTime` 加 `static` ⇒ 我方 static_init 16→**18 条**，与 ORIG
//        该 TU 实例逐条同形；四数零劣化。⚠ 该定义的**物理位置必须晚于本 TU 首函数 getUser**，
//        否则 `_GLOBAL__I_` 键名变成变量名（identical −1 / missing +1，实测）。
//   C. 回滚（负结论已写入各函数体注释）：
//      · Inter_MonitorSetGuildKey 结构重排（B#3）：差异点 145→162 ⇒ 回滚；
//      · Inter_LoadEventItem cast 删除 + 基址具名化（G#4）：135→143 ⇒ 回滚。
//   D. 报告工具撞名：`_Z41__static_initialization_and_destruction_0ii` 在本 TU 的 report 读数
//      （47 vs 16）是**工具按符号名错配**（ORIG 该名有 493 个实例，report 取首个 0x0807ee76 =
//      PacketTypes TU）⇒ 不可比；本 TU 真对照须用 GI 键定位（见 B）。
//   E. §6.5 串盲区真修（AE 不可见，按 ORIG .rodata 原字节直证）：
//      · `Inter_CutOffCallUserInfo::dispatch_sig` 主支 cMyTrace 格式串多尾 `\n`：ORIG 0x084c8af2
//        `mov [esp+0x4],0x8c87878` → .rodata 0xc3f878 处串 `"Cut Off Server Call UserInfo Arrived"`
//        （NUL 紧跟 'd'）⇒ 去 `\n`。（同 TU `Inter_MonitorCallUserInfo` 的 `"%d Server Call UserInfo
//        Arrived\n"` 与两函数的 null 支串 `"Inter_MonitorCallUserInfo::dispatch_sig()  G_CGameManager()
//        is null"` 经同法核对**本就正确**。）
//      · 方法学：新增 `/tmp/L9/str_refs.py <our.o> <ORIG> <mangled>` 逐函数比对「引用串集合」
//        （rodata ours-only 扫描只能抓「ORIG 无此串」，抓不到「用错但串存在」）。
//   证据与逐件读数：/tmp/L9/{A..M}/RESULT.md、/tmp/L9/*_sizes.tsv。
// ============================================================================

// ORIG 各 TU 一份 _ZL26GUILD_ADDRESS_DOUBLE_CHECK（r 0x2）：公会地址重复检查通过标志。
static const int GUILD_ADDRESS_DOUBLE_CHECK = 4;

// [L9·2026-09-11] g_lastDeleteTime 的带初值定义见下方 getUser() 之后（Main 批准的三步迁移之一）。

// P254 视图字段枚型（本 TU）：ORIG 0x084d2fa6 / 0x084ca02f / 0x084ca1xx / 0x084c9798 等段
// 对包视图 4 字节字段发 `mov disp(%eax),%eax; mov %eax,%edx`（4.1 对枚举字段保留 eax→edx
// 中转）；同址字段声明为 int 时 4.4 恒折单段 `mov disp(%eax),%edx`（每处 -1 条 ⇒ DIFF）。
// 枚举类型不入 mangled、不改布局、不发射符号（P254 三例实证）。
enum ENUM_VIEW_FIELD { ENUM_VIEW_FIELD_0 = 0 };

// dispatch 主体依赖（均为 ORIG 真实符号；权威声明处 include）
#include "CUser.h"             // CUser::DisConnSig / get_acc_id / DISCONN_SIG
#include "CGameManager.h"      // G_CGameManager()->getUser(int,int)
#include "LogManager.h"        // LogManager::logFormat
#include "cHistoryTrace.h"     // cMyTrace

// getUser(unsigned int) —— ORIG T _Z7getUserj @0x084bb9cf（LinkE2 批转写）。
// G_CGameManager()->m_userByAccId2（CGameManager +0x244，map<unsigned,CUser*>）
// find(uid)；end → 0，否则 it->second。
CUser* getUser(unsigned int uid)
{
    // ORIG 0x084bb9cf：无 m 引用槽，find/end 各自 call G_CGameManager + lea 0x244(%eax)（P48 双臂独立求值）
    std::map<unsigned int, CUser*>::const_iterator it = G_CGameManager()->m_userByAccId2.find(uid);
    if (it == G_CGameManager()->m_userByAccId2.end())
        return 0;
    return it->second;
}

// [L9·2026-09-11] ORIG 静态成员定义归属（Main 批准的三步迁移之一）：本 TU 承载
// `g_lastDeleteTime` 的**带初值定义** —— 直证：ORIG InterDispatcher TU 的 static_init
// 实例 0x084e9004 尾部两条 `call 0x84ce6d2 <_ZN26Inter_MonitorGuildPointDel16_makeDefaultTimeEv>`
// + `mov ds:0x943dd88,eax`；该实例由 GI `_GLOBAL__I__Z7getUserj` @0x084e904e 调用（= 本 TU 键）。
// 旧形把无初值定义放在 Inter_MonitorGuildPointDel.cpp ⇒ 运行期恒 0（应为月初 05:00 基准时刻）。
// 形态已由探针 /tmp/L9/probe/t1.cpp 复现（`call make; mov ds:g,eax`）。
// ⚠ 本定义的位置与 GET/LOCAL 键相关：必须**晚于本 TU 首个函数 getUser**，否则
// `_GLOBAL__I_` 键名会从 `_GLOBAL__I__Z7getUserj`（ORIG 键）变成变量名，丢 1 件 identical
// 并多 1 个 only-NEW 符号（实测）。
int Inter_MonitorGuildPointDel::g_lastDeleteTime =
    Inter_MonitorGuildPointDel::_makeDefaultTime();

// NumberToString —— ORIG T（_Z14NumberToStringji）；全局声明见 CInventory.h，
// 此处按 CGameManager.cpp 同例作 TU 内声明避免引入重型头。
char* NumberToString(unsigned int value, int radix);
#include <new>
#include <cstdio>              // puts (Inter_Terminate)
#include <cstdlib>             // exit (Inter_Terminate)
#include "GlobalData.h"        // B2 dispatcher singleton pointers
#include "nexon_cash.h"       // nexon::cash::MAX_NEXON_ID_LENTH
#include "CPowerManager.h"     // PowerWar result consumers
#include "CInventory.h"         // Inven_Item/RandomOption/CInventory 动作族 + user_creature::CCreatureMgr
#include "CExpandEquipslot.h"   // CExpandEquipslot::loadData 虚槽转真调用（GetCharacExpandData(2/9)）
#include "CAvatarItemMgr.h"     // WongWork::CAvatarItemMgr（jewel/expansion/tempKey）
#include "CItemLimitEdition.h"  // CItemLimitEdition/Mgr（ImplementItemLimitEdition 批权威实现）
#include "CCeraShop.h"          // WongWork::CCeraShop::IsSpecialItem
#include "CStreamGuard.h"       // CStreamGuard/StreamPool::Acquire
#include "SigTypes.h"           // stCeraShopIPGRequest_t（GetInBuffer 视图）
#include "TaiwanCash.h"         // WongWork::IPG::CIPGHelper 虚接口 IPGOutput/…Simulation
#include "CEventScriptMng.h"      // EventClassify::CEventScriptMng 完整类型（is_eventing）
#include "CTitleBook.h"          // CItemLock/stItemLockRef（GetItemLockRefVec 完整类型）
#include "AuctionProxy.h"       // CAuctionServerProxy/CCeraAuctionServerProxy::SendPacket（含 PacketDesign.h）
#include "ROI_Category.h"       // ROI_Category（m_keys/m_marks，_sort@0x0822ac30；PCK_AUCTION_REGIST_ITEM_GA._reg_roi_category_key 权威类型）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_（GuildCargoPopItem 邮件文案）
#include "CMyFileLog.h"          // JumpingCharacter 活动日志
#include "HistoryLog.h"          // WriteCreateCh（ORIG T 三参版）

#include "CDataManager.h"         // CAutoMarketConditionsControl / GetAutoMarketContitionsControl(ORIG 实名拼写)
#include "DBSelectCreateDnfUserInfo.h"
#include "DBSelectPcroomDailyReward.h"
#include "InterDispatcher/checkNewMemberHacklog.inc"

// CAradRyosikaEvent 权威声明见 CAradRyosikaEvent.h（LinkClosureB-2 建立，
// DoEvent 定义在 Inter_LoadEtc_impl.cpp）。
#include "CAradRyosikaEvent.h"
#include "InterDispatcher/CheckDailyScheduleTime.inc"
// break_away_prevent::CBreakAwayPreventSystem —— 唯一权威声明见
// BreakAwayPreventSystem.h（B2 批次与 BAFix 批次共建）。
// DB_LoadCleanPadPoint::makeRequest 仅调用视图（完整类与定义在 GlobalData.cpp；
// makeRequest 为 static，不访问对象成员，静态 ABI 精确匹配 ORIG 符号）。
class DB_LoadCleanPadPoint
{
public:
    static void makeRequest();   // ORIG T 0x0842efa0
};
#include "BreakAwayPreventSystem.h"  // loadDbData/updateUvList/startBreakAwayPreventSystem

// CUser::SetMileage/SendCashData/AddSecuRewardItem/SendCmdErrorPacket —— 直接经 CUser.h 真实成员调用。
#include "CMCAPManager.h"         // WongWork::CMCAPManager::setPointPerHackType(int*)
#include "CMonitorServerProxy.h"  // Inter_Disconnect GetServerProxy 完整类型（CServerProxyMgr<T> 实例化需要）
#include "CPrivateStoreMgr.h"     // private_store::GetInstancePrivateStoreMgr()->Enter(user)
#include "DB_InterDeps.h"         // DB_SaveCleanpadPoint/DB_ArrangeUserLoginInfo/…::makeRequest
#include "DB_SaveEventLevelupSupport.h"   // DB_SaveEventLevelupSupport::makeRequest(int,int,int,int)（ORIG Eiiii）
#include "FunctionGuildMember.h"  // FunctionGuildMember::MakePacketGuildMembersInfo/LogMake（权威类声明）
#include "ItemVendingMachine.h"  // GlobalData::s_pItemVendingMachine 完整类型（ProcessIPGUserData）
#include "CServerEvent.h"        // CServerEvent::AddExpRate/TurnOnEvent
#include "Packet_Monitor_Member_Enter_Reply.h"     // shared/packet 权威包
#include "Packet_Monitor_UDP_Logout_Complete.h"    // shared/packet 权威包
#include <algorithm>              // std::min/std::max（SaveClientHackLog 掉率钳制）
#include "CDeathTowerRanking.h"   // WongWork::CDeathTowerRanking getRankTable/registRanker 族
#include "ServiceRestrictManager.h"  // ARAD::Singleton<ServiceRestrictManager>/RestrictCategory
#include "CTimeGate.h"            // GlobalData::s_timeGate_（Inter_LoadQuest）
#include "APSystemDbDispatchers.h"  // APSystem::DB_UpdateRewardMedal + _SIG_LOAD_REWARD_MEDAL
#include "Packet_Monitor_Charac_Delete.h"  // shared/packet 权威包（Inter_DeleteCharac）
#include "CMailBoxHelper.h"       // W0 批 MailBox 链
#include "PacketTypes.h"          // MSG_MAILBOX_SEND 权威布局（Inter_MultiMailBoxReqSend 栈上组包缓冲）
#include "Arad_DataManager.h"     // W0 批 ARAD 数据管理器
#include "ServerParameterScript.h"  // W0 批 conditionEnterRevenge 视图
#include "CPremiumLetheManager.h" // W0 批
#include "CHandlePremium.h"       // W0 批
#include "CShutdowManager.h"          // GetInstanceShutdowManager（Inter_LastMsgResponse）
#include "CSyncSlangFilter.h"         // CSyncSlangFilter::GetInstance()->AddSlang
#include "CInGameAdvertisementManager.h"  // InGameADManagerInstance/stInGameADExposeCondition
#include "CSchoolMgr.h"              // g_schoolMgr/CSchoolMgr::GetUserCount（PvPChannelUserCount）
#include "CLeagueMatch.h"            // CLeagueMatch::GetPlayCountPerOneTeam/GetBattleMode
#include "COnTimeEvent.h"            // COnTimeEvent（OnTimeEventReward*）
#include "CQuestShop.h"              // CQuestShop/CHARAC_QUEST_SHOP_DATA（RequestTagCharacInfo）
#include "SkillSlot.h"               // STGuildDBInfoPacketView（GetGuildDBInfo 视图字段化）
#include "TowerOfDespairMgr.h"       // TowerOfDespairMgr/_APCInfo（TOD_Reload_APC）
#include "CHackAnalyzer.h"           // ENUM_HACKTYPE（SetARSInfo）
#include "CCirculationStatistic.h"   // GetInstanceCirculationStatistic/CIRCULATION_STATISTIC_FIELD
#include "Packet_No_Cache.h"         // shared/packet（GuildExpBookDelete 等）
#include "Packet_TOD_DoRandomSelect.h"          // shared/packet 权威包
#include "Packet_Guild_Guild_Cargo_Push_Item.h" // shared/packet 权威包
#include "DnfItemInfo.h"         // Packet_Guild_Guild_Cargo_Push_Item ctor（m_item +0x1b 完整构造）
#include "GameServerDemonManager.h"  // GameserverDemonManagerInstance（EmergencyPatchMessage）
#include "Packet_Guild_Guild_Cargo_Push_Item.h" // shared/packet 权威包
#include "DnfItemInfo.h"         // Packet_Guild_Guild_Cargo_Push_Item ctor（m_item +0x1b 完整构造）
#include "DnfItemInfo.h"         // Packet_Guild_Guild_Cargo_Push_Item ctor（m_item +0x1b 完整构造）
#include "GameServerDemonManager.h"  // GameserverDemonManagerInstance（EmergencyPatchMessage）
#include "Packet_Monitor_Add_Buddy.h"             // shared/packet 权威包（CheckAddFriend）
#include "Packet_Monitor_Find_Factory_Hub_User.h" // shared/packet 权威包（FindFactoryHub）
#include "Packet_Monitor_Server_Message_Info.h"   // shared/packet（UpdateServerMessage）
#include "GameWorld.h"               // CUserModifier/CUserResetTimeGateQuest 基类（已含则幂等）
#include "Area.h"                    // CBelong（CUserTeen TU 局部声明基类）
#include "CVillageMonsterMgr.h"      // ENUM_VILLAGE_ATTACKED_REWARD
#include "LimitNpcBuyItemUpdate.h"   // 新建权威包（NpcLimitBuyItemInfo 回执）
#include "NpServerXml.h"             // np_server_xml::g_ServerString_（跨 TU extern）
#include "Secu_DataControl.h"        // Secu_DataControl::UpdateSecurityServiceState
#include "CNexonMsgTable.h"          // g_nexonMsgTable（Inter_PcroomResponse）
#include "CPrePayInfoMgr.h"          // G_CPrePayInfoMgr/PrePayInfo（Inter_OwnerisVIP）

#include "TimerDispatcher.h"      // Timer_HadesHeartBeat/TimerConnectMonitorServer registNextTimer
#include "CServerProxyMgr.h"      // CServerProxyMgr<T>::GetServerProxy（模板成员，头内已内联）
#include "CGuildServerProxy.h"    // SendGuildExpLimit

#include "GameWorld.h"            // GameWorld 全量成员 + G_GameWorld()（ORIG T _Z11G_GameWorldv）


// ======== 主分发器（逐指令转写自 docs/class_func_reports/InterDispatcher/）=====

#include "InterDispatcher/InterDispatcher.inc"


#include "InterDispatcher/dtor_InterDispatcher.inc"


#include "InterDispatcher/get_dispatcher.inc"


#include "InterDispatcher/register_dispatcher.inc"


#include "InterDispatcher/dispatch.inc"


// ======== 具体分发器生命周期 + dispatch_sig stub ========
// stub 体待后续批次按 docs/class_func_reports 还原；地址=该函数 ORIG 起点。

#include "InterDispatcher/dtor_Inter_Disconnect.inc"
#include "InterDispatcher/dispatch_sig.inc"


#include "InterDispatcher/dtor_Inter_SessionClose.inc"
// ORIG 0x084c7af0 T dispatch_sig —— 监控侧会话强制关闭。
// 报告 docs/class_func_reports/Inter_SessionClose/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SessionClosePktView —— Inter_SessionClose::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SessionClosePktView
{
    unsigned int m_0;         // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_1.inc"


#include "W6_HackPunishDeps.h"     // WongWork::CAutoPunishRuleBlackIPMgr::isBlackIP
#include "DB_AccountCargoSync.h"   // DB_PassPadUpdateFailCnt::makeRequest(static)

// Inter_LoginInfo 下游 DB/Timer/GameWorld 入口改经真实类型声明调用：
// Timer_ReturnUserTimeout::registNextTimer → TimerDispatcher.h（已 include）
// break_away_prevent 三入口 → BreakAwayPreventSystem.h（权威头，见文件尾 include 区）

// 仅调用视图：makeRequest 在 ORIG 为 static（调用点无 this，见
// df_game_r 0x84355ca/0x843b6ec/0x815bee0 首槽均传显式实参）；完整类与
// 实现见 W8LT_DBTask.cpp / GlobalData.cpp。
class DB_BlackipPrivateSecurityValuation { public: static void makeRequest(unsigned int); };     // T 0x084355ca
class DB_LoadAccountCargo              { public: static void makeRequest(int, unsigned int); }; // T 0x0843b6ec
#include "Inter_MonitorGuildPointDel.h"  // setLastDeleteTime/getLastDeleteTime（Inter_MonitorGuildPointDel 调用）

#include "RestrictGeolocation.h"   // 地区限制检测
#include "CBlackIPMonitor.h"       // 黑名单 IP 位标志查询
#include "TaiwanQueries.h"         // Taiwan::GarenaAuthData / GarenaCyberCafe
#include "CHandlePremium.h"        // handleSetUserPremium
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"          // GetServerProxy 模板完整类型
#include "CDoubleConnCheckServerProxy.h"
#include "Packet_Monitor_UDP_Login.h"

#include "InterDispatcher/dispatch_sig_2.inc"


#include "InterDispatcher/dtor_Inter_LoginInfo.inc"

#include "InterDispatcher/dtor_Inter_CreateCharac.inc"
#include "InterDispatcher/dispatch_sig_3.inc"


#include "InterDispatcher/process_event.inc"


#include "InterDispatcher/process_createGift.inc"


#include "InterDispatcher/dtor_Inter_DeleteCharac.inc"
// ORIG 0x084bf810 T dispatch_sig —— 删除角色应答分发：结果码 1/2/3 直接回执，
// 其余构造 Packet_Monitor_Charac_Delete 上报 monitor/guild 后本地删角色并刷基础信息。
// 报告 docs/class_func_reports/Inter_DeleteCharac/dispatch_sig.md 全文转写。
// msg 布局：+4 charNo(int) +8 result(int) +0xc guildKey(int) +0x10 eventCharacFlag(char)。
#pragma pack(push, 1)
// Inter_DeleteCharacPktView —— Inter_DeleteCharac::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_DeleteCharacPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned int m_4;               // +0x04
    int m_8;                        // +0x08
    unsigned int m_guildKey;        // +0x0c 语义名←函数头注释
    unsigned char m_eventCharacFlag;// +0x10 语义名←函数头注释
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_4.inc"


#include "InterDispatcher/dtor_Inter_LoadCharac.inc"
// ORIG 0x084c3398 T dispatch_sig —— 登录加载角色。
// 报告 docs/class_func_reports/Inter_LoadCharac/dispatch_sig.md 全文转写
// （## 汇编 + ## 反编译 C）。
#pragma pack(push, 1)
// Inter_LoadCharacPktView —— Inter_LoadCharac::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCharacPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned int m_4;               // +0x04
    int m_8;                        // +0x08
    unsigned int m_c;               // +0x0c
    char m_pad10[0x1e];  // +0x10..未消费
    char m_2e;                      // +0x2e
    char m_pad2f[0xf9];  // +0x2f..未消费
    short m_128;                    // +0x128
    char m_pad12a[0x1];
    signed char m_12b;              // +0x12b
    char m_pad12c[0xc];  // +0x12c..未消费
    bool m_138;                     // +0x138
    unsigned char m_139;            // +0x139
    char m_13a;                     // +0x13a
    char m_pad13b[0x5];  // +0x13b..未消费
    unsigned int m_140;             // +0x140
    char m_pad144[0x542];  // +0x144..未消费
    unsigned int m_686;             // +0x686
    unsigned char m_68a;            // +0x68a
};
#pragma pack(pop)

// §6.2 巨件核销：P167 106/106 [0缺0多]；gap -16→-16；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。batch-20「Inter 巨兽」已核销，本次维持。
// [b28d-lane2] verdict=DIFF ndiff=578（ORIG 0x084c3398）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-16。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=394（ORIG 0x84c3398）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N101 imul/lea 折叠。
// [lane2 2026-09-10] 真 bug 修复：`*(unsigned char*)(&user->m_characList[slotIdx]) = pPkt->m_68a;`
//   （写偏移 0）→ `user->m_characList[slotIdx].m_field14b7`（CUser.h:719 权威成员 +0x14b7；
//   ORIG 084c364e `mov %dl,0x14b7(%eax)` 单条 disp 直证）。
#pragma pack(push, 1)
// [G2A-PC 手法1/P248] 本件两处取址，ORIG 皆发结构体成员 disp 形：
//   · 084c34b9 / 084c3563 `lea 0xc(%eax),%edx`（第三实参）
//   · 报告 +0x1cb `lea 0x68b(%eax),%edx`
//   我方旧写 `data + 0xc` / `data + 0x68b`（指针算术）会发 `mov 0x10(%ebp),%eax;
//   add $0xNN,%eax` 两段（本 diff N 侧 insert）。聚合器 Inter_LoadCharacPktView
//   只覆盖 +0x0c（无 +0x68b），故本件自带同布局局部视图补齐。
struct Inter_LoadCharacAt0cViewI
{
    char m_pad0[0xc];
    unsigned int m_c;                               // +0x0c
};
struct Inter_LoadCharacAt68bViewI
{
    char m_pad0[0x68b];
    unsigned char m_68b;                            // +0x68b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_5.inc"


#include "InterDispatcher/_verifyInventory.inc"
                      std::vector<item_lock::stItemLockRef>& lockRefs,
                      CUser* user);   // 定义 LinkClosureD.cpp（ORIG W 0x084f0145）
#include "InterDispatcher/GetItemIdByAddInfo.inc"
// 报告 docs/class_func_reports/Inter_LoadInven/dispatch_sig.md 两段全读。
// 错误码：0x1835/0x183c/0x1841/0x1872/0x1892。
#pragma pack(push, 1)
// Inter_LoadInvenPktView —— Inter_LoadInven::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadInvenPktView
{
    char m_pad0[0x8];  // +0x0..未消费
    unsigned int m_8;               // +0x08
    char m_padc[0x4024];  // +0xc..未消费
    int m_4030;                     // +0x4030
    char m_pad4034[0x10];           // +0x4034..未消费
    Inven_Item m_4044;              // +0x4044 §4.3 字段化（Inven_Item 0x3d；拆 pad 保 sizeof 不变）
    char m_pad4081[0x8c9b];  // +0x4081..未消费
    int m_cd1c;                     // +0xcd1c
    char m_padcd20[0x5b5c];  // +0xcd20..未消费
    int m_1287c;                    // +0x1287c
    char m_pad12880[0x1ed8];  // +0x12880..未消费
    unsigned long m_14758;          // +0x14758
    char m_pad1475c[0xbe0];  // +0x1475c..未消费
    int m_1533c;                    // +0x1533c
    int m_15340;                    // +0x15340
    char m_pad15344[0x104];  // +0x15344..未消费
    int m_15448;                    // +0x15448
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_6.inc"

// ---- ORIG 私有辅助实现（本 TU 唯一 owner；其余两个后续批转写）----

#include "InterDispatcher/_CheckOverEquipItem.inc"


#include "InterDispatcher/_CheckUsableExpertJobEquipment.inc"


#include "InterDispatcher/_UpdateRentalItems.inc"


#include "InterDispatcher/_UpdateDimensionRentalItems.inc"


#include "InterDispatcher/dtor_Inter_LoadInven.inc"

#include "InterDispatcher/dtor_Inter_LoadSkill.inc"
#include "InterDispatcher/dispatch_sig_7.inc"
{
    return 0;
}

#include "InterDispatcher/dtor_Inter_LoadQuest.inc"
// ORIG 0x084c5054 T dispatch_sig —— DB 任务数据装载。
// 报告 docs/class_func_reports/Inter_LoadQuest/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadQuestPktView —— Inter_LoadQuest::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadQuestPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_8.inc"


#include "InterDispatcher/dtor_Inter_LoadTicket.inc"
// ORIG 0x084c515c T dispatch_sig —— 点券 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadTicket/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadTicketPktView —— Inter_LoadTicket::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadTicketPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
    int m_8;                        // +0x08
    int m_ids[1];                   // +0x0c 线格式数组区（ORIG 0x084c51c3 mov 0xc(%eax,%edx,4) 下标 i 循环消费）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_9.inc"


#include "InterDispatcher/dtor_Inter_LoadDungeonClear.inc"
// ORIG 0x084c7c42 T dispatch_sig —— 副本通关记录 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadDungeonClear/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDungeonClearPktView —— Inter_LoadDungeonClear::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadDungeonClearPktView
{
    char m_pad0[0x8];  // +0x0..未消费
    bool m_8;                       // +0x08（ORIG `xor $0x1` 判非 ⇒ bool，非 unsigned char）
    char m_pad9[0x3];  // +0x9..未消费
    int m_c;                        // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_10.inc"


#include "InterDispatcher/dtor_Inter_LoadActionPoint.inc"
#include "InterDispatcher/dispatch_sig_11.inc"


#include "InterDispatcher/dtor_Inter_EmptyActionPoint.inc"
#include "InterDispatcher/dispatch_sig_12.inc"


#include "InterDispatcher/dtor_Inter_LoadRewardMedal.inc"
#include "InterDispatcher/dispatch_sig_13.inc"


#include "InterDispatcher/dtor_Inter_Load.inc"
#include "InterDispatcher/dispatch_sig_14.inc"


#include "InterDispatcher/dtor_Inter_DeleteChannel.inc"
#pragma pack(push, 1)
// Inter_DeleteChannelPktView —— Inter_DeleteChannel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DeleteChannelPktView
{
    char m_pad0[0x8];  // 未消费
    int m_8;                  // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_15.inc"
{
    const Inter_DeleteChannelPktView* pPkt = reinterpret_cast<const Inter_DeleteChannelPktView*>(data);
    // ORIG +0x0b：单级带偏移读（无中间 int* 槽）。
    if (pPkt->m_8 == 1)
    {
        return 0x806;
    }
    return 0;
}

#include "InterDispatcher/dispatch_sig_16.inc"
{
    struct NoticePacketView
    {
        char m_pad[0x51];
        char m_len;
    };
    char* packet = data;
    G_GameWorld()->send_chat_msg(0, 0, 0, 0, packet + 1,
                                 static_cast<int>(reinterpret_cast<NoticePacketView*>(packet)->m_len), 0, 0,
                                 static_cast<ENUM_NOTIPACKET>(0x0c));
    return 0;
}
#include "InterDispatcher/dtor_Inter_Notice.inc"

#include "InterDispatcher/dtor_Inter_Terminate.inc"
#include "InterDispatcher/dispatch_sig_17.inc"


#include "InterDispatcher/dtor_Inter_ReadyToTerminate.inc"
#include "InterDispatcher/dispatch_sig_18.inc"


#include "InterDispatcher/dtor_Inter_MonitorBuddyInOut.inc"
// ORIG 0x084c8ec0 T dispatch_sig —— 好友上下线通知（0x36/0x37 包），上线且当前角色存在时补发系统邮件。
// 报告 docs/class_func_reports/Inter_MonitorBuddyInOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorBuddyInOutPktView —— Inter_MonitorBuddyInOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorBuddyInOutPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    unsigned char m_12;       // +0x12
    unsigned char m_13;       // +0x13
    char m_14;                // +0x14
    char m_15[1];             // +0x15 串/字节区首元素（容量未建模）
    char m_pad16[0x1d];  // 未消费
    char m_33;                // +0x33
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_19.inc"


#include "InterDispatcher/dtor_Inter_MonitorLoginOK.inc"
#include "CTimeGate.h"           // CTimeGate::isOpen/getNpcIndex（GlobalData 前向的完整类型）
#include "TimerQueue.h"          // TimerEntry::OBJ_TYPE/TIMER_MESSAGE/G_TimerQueue()
#include "CEnvironment.h"        // getProcessSequence/m_serverEnvir 字段（get_server_group）
// ---- ORIG 辅助符号（本 TU 唯一 owner，其余批未实现）----

#include "InterDispatcher/_NS_NonClient_Request.inc"


// ORIG T DB_LoadDnfHackLog::makeRequest(int,uint) @0x084224ce（本 TU 唯一 owner；
// 报告 docs/class_func_reports/DB_LoadDnfHackLog/makeRequest.md 全文转写）：
// Acquire("DBThread.cpp",0x5abf) + guard<<0x106<<uid；GetInBuffer 前四字节=accId；
// MsgQueueMgr::put(DB_Q, guard)。
class DB_LoadDnfHackLog
{
public:
    static void makeRequest(int uid, unsigned int accId);   // ORIG _ZN17DB_...Eij
};

#include "InterDispatcher/makeRequest.inc"


// ORIG 0x084c7e0c T dispatch_sig —— 监控服登录应答处理。
// 报告 docs/class_func_reports/Inter_MonitorLoginOK/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorLoginOKPktView —— Inter_MonitorLoginOK::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorLoginOKPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
    char m_pad12[0x11];  // +0x12..未消费
    unsigned char m_23;             // +0x23
    unsigned int m_24;              // +0x24
    unsigned int m_28;              // +0x28
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_20.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallUserInfo.inc"
// ORIG 0x084c89b2 T dispatch_sig —— 监控服索取用户信息上报。
// 报告 docs/class_func_reports/Inter_MonitorCallUserInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallUserInfoPktView —— Inter_MonitorCallUserInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallUserInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_21.inc"


#include "InterDispatcher/dtor_Inter_CutOffCallUserInfo.inc"
#include "InterDispatcher/dispatch_sig_22.inc"


#include "InterDispatcher/dtor_Inter_MonitorUserGetout.inc"
// ORIG 0x084c8b0c T dispatch_sig —— 监控侧用户强制退出。
// 报告 docs/class_func_reports/Inter_MonitorUserGetout/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorUserGetoutPktView —— Inter_MonitorUserGetout::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorUserGetoutPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_23.inc"


#include "InterDispatcher/dtor_Inter_WebNoticeIngameAdvertisement.inc"
#include "InterDispatcher/dispatch_sig_24.inc"


#include "InterDispatcher/dtor_Inter_ReloadIngameAdvertisement.inc"
#include "InterDispatcher/dispatch_sig_25.inc"


#include "InterDispatcher/dtor_Inter_TcpServerConnect.inc"
// ORIG 0x084c8dca T dispatch_sig —— 公会/监控服代理 TCP 重连。
// 报告 docs/class_func_reports/Inter_TcpServerConnect/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TcpServerConnectPktView —— Inter_TcpServerConnect::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TcpServerConnectPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_26.inc"


#include "InterDispatcher/dtor_Inter_MonitorReplyBuddyList.inc"
// ORIG 0x084c9148 T dispatch_sig —— 监控服好友在线列表应答广播。
// 报告 docs/class_func_reports/Inter_MonitorReplyBuddyList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorReplyBuddyListPktView —— Inter_MonitorReplyBuddyList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorReplyBuddyListPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned char m_e;              // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_27.inc"


#include "InterDispatcher/dtor_Inter_MonitorCeraUpdate.inc"
// ORIG 0x084cea92 T dispatch_sig —— IPG 塞拉更新：按 accId 定位用户并触发 IPG 查询。
// 报告 docs/class_func_reports/Inter_MonitorCeraUpdate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCeraUpdatePktView —— Inter_MonitorCeraUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCeraUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_28.inc"


#include "InterDispatcher/dtor_Inter_IPGResult.inc"
// ORIG 0x084ceaee T dispatch_sig —— IPG 计费结果分发：数据流分支（ProcessIPGUserData）
// 与查询结果分支（SendCashData / 点数查询 / CERA 查询 / 错误状态包）。
// 报告 docs/class_func_reports/Inter_IPGResult/dispatch_sig.md 全文转写。
// msg 布局：+0 isData(char)；数据分支 data+1 req / data+0x2dd res；查询分支 data+1 起。
#pragma pack(push, 1)
// Inter_IPGResultPktView —— Inter_IPGResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_IPGResultPktView
{
    char m_0;                 // +0x00
    // [R7a03 2026-09-12 ④ 真语义修·基准差 1 撤销 Q5 误修] ORIG 的视图基准不是 `data` 而是
    //   `data+1`：0x084ceb0d `mov 0x10(%ebp),%eax; add $0x1,%eax; mov %eax,-0x20(%ebp)` ⇒ 其后
    //   084ceb58 `mov 0x1c(%eax)` = **data+0x1d**、084ceb19 `mov 0x20(%eax)` = **data+0x21**、
    //   084cebb2 `movzbl 0xb1(%eax)` = **data+0xb2**、084cec?? `movzbl 0x26d(%eax)` = **data+0x26e**。
    //   本视图以 `data` 为基准 ⇒ 字段偏移必须比 ORIG disp **+1**：m_0(1)+pad(0x1c)=0x1d。
    //   Q5 曾按 ORIG disp 原值把 pad 改成 0x1b ⇒ 四字段整体 -1（m_1d 是 ==4/==6/==0 分支选择子，
    //   读错走错分支 = 运行期真 bug）。本条恢复 pad=0x1c，其后 pad25/padb3 保持不动即全部对齐。
    char m_pad1[0x1c];  // 未消费
    int m_1d;                 // +0x1d（ORIG 基准 data+1 直读 0x1c(%eax)）
    int m_21;                 // +0x21（ORIG 基准 data+1 直读 0x20(%eax)）
    char m_pad25[0x8d];  // 未消费
    char m_b2;                // +0xb2（ORIG 基准 data+1 直读 0xb1(%eax)）
    char m_padb3[0x1bb];  // 未消费
    unsigned char m_26e;      // +0x26e（ORIG 基准 data+1 直读 0x26d(%eax)）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_29.inc"


#include "InterDispatcher/dtor_Inter_GenCeraTicket.inc"
#include "InterDispatcher/dispatch_sig_30.inc"


#include "InterDispatcher/dtor_Inter_LoadEventItem.inc"
// ORIG 0x084cf0c4 T dispatch_sig —— Web 活动物品入库：RecvInfoFromDB 逐项校验后入 DB 队列。
// 报告 docs/class_func_reports/Inter_LoadEventItem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadEventItemPktView —— Inter_LoadEventItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadEventItemPktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
    int m_8;                  // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_31.inc"


#include "InterDispatcher/dtor_Inter_MonitorEventItemUpdate.inc"
// ORIG 0x084cefd2 T dispatch_sig —— 网页活动道具更新（CWebEvent::RecvInfoFromWeb）。
// 报告 docs/class_func_reports/Inter_MonitorEventItemUpdate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorEventItemUpdatePktView —— Inter_MonitorEventItemUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorEventItemUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
    unsigned int m_16;              // +0x16
    unsigned int m_1a;              // +0x1a
    unsigned int m_1e;              // +0x1e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_32.inc"


#include "InterDispatcher/dtor_Inter_MonitorServerEventStart.inc"
// ORIG 0x084cf3ca T dispatch_sig —— 服务端事件启动：圣诞事件开启、广播与定时器注册。
// 报告 docs/class_func_reports/Inter_MonitorServerEventStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorServerEventStartPktView —— Inter_MonitorServerEventStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorServerEventStartPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned short m_e;       // +0x0e
    short m_10;               // +0x10
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=64（ORIG 0x084cf3ca）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84cf3ca）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化：ORIG 084cf439/084cf465 对 +0x10 两处发 `movzwl %ax,%reg`（零扩展，用于
//   int 形参）⇒ 该字段语义 = unsigned short；(short) 形参处 ORIG 084cf3fa 为
//   `movzwl 0x10(%eax),%eax; movswl %ax,%edx`（无符号读 + 符号截断）⇒ 与 unsigned short 自洽。
//   聚合器 Inter_MonitorServerEventStartPktView.m_10 声明为 short（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned short）。
struct Inter_MonitorServerEventStartPktViewI
{
    char m_pad0[0xa];               // +0x00..未消费
    unsigned int m_a;               // +0x0a  (P248: ORIG 084cf451 `mov 0xa(%eax),%edi` 单段 = 与 InsertTimer 第 6 形参 unsigned int 同型)
    unsigned short m_e;             // +0x0e
    unsigned short m_10;            // +0x10
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_33.inc"


#include "InterDispatcher/dtor_Inter_FatigueResetDBReply.inc"
#include "InterDispatcher/dispatch_sig_34.inc"


#include "InterDispatcher/dtor_Inter_SuicideWhenQueueIsEmpty.inc"
#include "InterDispatcher/dispatch_sig_35.inc"
{
    return 0;
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildEnter.inc"
// ORIG 0x084c9194 T dispatch_sig —— 公会加入通知（0x3a 包 + GuildJoin 历史日志）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildEnter/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildEnterPktView —— Inter_MonitorNoticeGuildEnter::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildEnterPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    unsigned short m_12;      // +0x12
    int m_14;                 // +0x14
    char m_pad18[0x4];  // 未消费
    unsigned int m_1c;        // +0x1c
    char m_20[1];             // +0x20 串/字节区首元素（容量未建模）
    char m_pad21[0x16];  // 未消费
    char m_37[1];             // +0x37 串/字节区首元素（容量未建模）
    char m_pad38[0x1d];  // 未消费
    int m_55;                 // +0x55
    unsigned char m_59;       // +0x59
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=166（ORIG 0x084c9194）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=117（ORIG 0x84c9194）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化：ORIG 084c92xx 比较处发 4 行
//   `mov %eax,%edx; mov -0x10(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证，同 38/39/40）。
//   聚合器 Inter_MonitorNoticeGuildEnterPktView.m_e 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildEnterPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
    unsigned short m_12;            // +0x12
    int m_14;                       // +0x14
    char m_pad18[0x4];              // 未消费
    unsigned int m_1c;              // +0x1c
    char m_20[1];                   // +0x20 串/字节区首元素（容量未建模）
    char m_pad21[0x16];             // 未消费
    char m_37[1];                   // +0x37 串/字节区首元素（容量未建模）
    char m_pad38[0x1d];             // 未消费
    // ORIG 084c9259 段 `mov 0x55(%eax),%ebx`（单段直读，无中转）＝ 与
    //   CUserCharacInfo::getCurCharacNo()（unsigned int，CUserCharacInfo.h:783）**同型**
    //   ⇒ 本字段 unsigned int（我方旧 int 会多一条 `mov %eax,%ebx` 中转，P211 档）。
    unsigned int m_55;              // +0x55
    unsigned char m_59;             // +0x59
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_36.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildSecede.inc"
// ORIG 0x084c957e T dispatch_sig —— 公会退出通知：校验角色号后组 0x3b 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildSecede/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildSecedePktView —— Inter_MonitorNoticeGuildSecede::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildSecedePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较处两步 spill＝unsigned 字段→get_charac_no(int) 失配）
    int m_12;                 // +0x12
    char m_pad16[0x4];  // 未消费
    int m_1a;                 // +0x1a
    unsigned short m_1e;      // +0x1e
    char m_20[1];             // +0x20 串/字节区首元素（容量未建模）
    char m_pad21[0x16];  // 未消费
    char m_37[1];             // +0x37 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_37.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildMarkChange.inc"
// ORIG 0x084ca1e6 T dispatch_sig —— 公会徽标变更：校验 charac_no 后转发公会 key。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildMarkChange/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildMarkChangePktView —— Inter_MonitorNoticeGuildMarkChange::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildMarkChangePktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    int m_e;                  // +0x0e（P254 枚型实测劣化 ⇒ 回退 int，N22 封顶）
    unsigned int m_12;        // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=105（ORIG 0x084ca1e6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=57（ORIG 0x84ca1e6）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化：ORIG 084ca226~084ca22e 比较处发 4 行
//   `mov %eax,%edx; mov -0x1c(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（c6root 4.4.7 探针 t3：
//   int 字段给 3 行 `mov <slot>,%edx; mov 0xe(%edx),%edx; cmp %edx,%eax`）。
//   聚合器 Inter_MonitorNoticeGuildMarkChangePktView.m_e 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildMarkChangePktViewI
{
    char m_pad0[0xa];               // +0x00..未消费
    int m_a;                        // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_38.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildDismiss.inc"
// ORIG 0x084ca00c T dispatch_sig —— 公会解散通知：清公会标记并组 0x3d 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildDismiss/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildDismissPktView —— Inter_MonitorNoticeGuildDismiss::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeGuildDismissPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e（P254 枚型实测劣化 ⇒ 回退 int，N22 封顶）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=98（ORIG 0x084ca00c）：寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=28（ORIG 0x84ca00c）：N22 cmp 操作数方向。
#pragma pack(push, 1)
// P248 同型化：ORIG 084ca04b~084ca053 比较处发 4 行
//   `mov %eax,%edx; mov -0xc(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 int 返回值失配 = unsigned int（c6root 4.4.7 实测：int 字段给 3 行形
//   `mov <slot>,%edx; mov 0xe(%edx),%edx; cmp %edx,%eax`，unsigned int 才给 ORIG 的 4 行形）。
//   聚合器 Inter_MonitorNoticeGuildDismissPktView.m_e 声明为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildDismissPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_39.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildChatMsg.inc"
// ORIG 0x084c9e30 T dispatch_sig —— 公会聊天消息转发：校验角色号后组 0x41 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildChatMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildChatMsgPktView —— Inter_MonitorNoticeGuildChatMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildChatMsgPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    char m_31[1];             // +0x31 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=118（ORIG 0x084c9e30）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=79（ORIG 0x84c9e30）：N22 cmp 操作数方向+N18a 单段 regmove+P264 .o 重定位/立即数。
#pragma pack(push, 1)
// P248 同型化：ORIG 084c9e6f~084c9e77 比较处发 4 行
//   `mov %eax,%edx; mov -0x10(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证，同 38/39）。
//   聚合器 Inter_MonitorNoticeGuildChatMsgPktView.m_e 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildChatMsgPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
    char m_12[1];                   // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];             // 未消费
    unsigned char m_30;             // +0x30
    char m_31[1];                   // +0x31 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_40.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallGuildMembers.inc"
// ORIG 0x084ca7e6 T dispatch_sig —— 公会成员列表下发（0x46 包 + MakePacketGuildMembersInfo）。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildMembers/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildMembersPktView —— Inter_MonitorCallGuildMembers::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorCallGuildMembersPktView
{
    char m_pad0[0xe];  // 未消费
    // §4.3 N（记不回退）：+0x34 ST_Guild_Mem_Info* 数组基址——补 view 具名基址槽会使
    // sizeof(view) 变化，触发本函数内对 view 尺寸敏感的立即数装载（movl $size），破坏 AE，故保留 reinterpret_cast。
    unsigned int m_e;         // +0x0e（unsigned→int 返回失配的 4 行 spill 形态）
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x16];  // 未消费
    unsigned char m_2d;       // +0x2d
    unsigned int m_2e;                 // +0x2e
    unsigned short m_32;      // +0x32
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_41.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallGuildMembersNext.inc"
// ORIG 0x084caa16 T dispatch_sig —— 公会成员信息续查：双校验后组 0x8e 包回发。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildMembersNext/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildMembersNextPktView —— Inter_MonitorCallGuildMembersNext::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallGuildMembersNextPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    // §4.3 N：+0x17 ST_Guild_Mem_Info* 基址——补 view 基址槽改 sizeof(view) 触发尺寸敏感装载破 AE，保留 reinterpret_cast。
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_42.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallGuildAllMembers.inc"
// ORIG 0x084cabda T dispatch_sig —— 公会全成员列表下发（0x8f 包 + MakePacketGuildMembersInfo）。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildAllMembers/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildAllMembersPktView —— Inter_MonitorCallGuildAllMembers::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorCallGuildAllMembersPktView
{
    char m_pad0[0xe];  // 未消费
    // §4.3 N：+0x34 ST_Guild_Mem_Info* 基址——同上，view 基址槽破坏 AE，保留 reinterpret_cast。
    unsigned int m_e;         // +0x0e（同上）
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x16];  // 未消费
    int m_2d;                 // +0x2d
    unsigned short m_31;      // +0x31
    unsigned char m_33;       // +0x33
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_43.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallGuildAllMembersNext.inc"
// ORIG 0x084cae0a T dispatch_sig —— 公会全体成员信息续查：双校验后组 0x90 包回发。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildAllMembersNext/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildAllMembersNextPktView —— Inter_MonitorCallGuildAllMembersNext::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallGuildAllMembersNextPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    // §4.3 N：+0x17 ST_Guild_Mem_Info* 基址——同上，view 基址槽破坏 AE，保留 reinterpret_cast。
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_44.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildNames.inc"
// ORIG 0x084cafce T dispatch_sig —— 公会成员名列表通知：双重校验后组 0x3f 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildNames/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildNamesPktView —— Inter_MonitorNoticeGuildNames::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
#pragma pack(push, 1)
// MonitorNoticeGuildNames 名区元素视图（stride 0x1e；ORIG 0x084cb11f/0x084cb125
// 两步位移 `lea 0x10(%edx)` + `add $0x7`：名绝对基 = union@0x12 + 元素内 +5 =
// 0x17，4.4 按 (0x10 | 余量) 切分常量）。
struct MonitorGuildNameElemView { char m_pad0[5]; char m_name[0x17]; char m_padt[2]; };
#pragma pack(pop)
struct Inter_MonitorNoticeGuildNamesPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（ORIG 0x084cb012 mov 0xe(%eax)）
    union
    {
        struct { int m_12; unsigned char m_16; };  // +0x12 / +0x16
        MonitorGuildNameElemView m_names[1];       // union@0x12（容量未建模）
    };
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_45.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildLoginOut.inc"
// ORIG 0x084c93e4 T dispatch_sig —— 公会成员登录/登出广播：校验 charac_no 后转发登出包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildLoginOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildLoginOutPktView —— Inter_MonitorNoticeGuildLoginOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildLoginOutPktView
{
    char m_pad0[0xa];  // 未消费
    signed char m_a;          // +0x0a
    char m_padb[0x4];  // 未消费
    unsigned int m_f;         // +0x0f（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_13;       // +0x13
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_46.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeOtherChannelChat.inc"
// ORIG 0x084cb1ac T dispatch_sig —— 其它频道聊天通知：按消息类型（1/2/3/私聊回执）
// 构造 0x41 聊天包转发给目标用户。
// 报告 docs/class_func_reports/Inter_MonitorNoticeOtherChannelChat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeOtherChannelChatPktView —— Inter_MonitorNoticeOtherChannelChat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeOtherChannelChatPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    signed char m_e;          // +0x0e
    char m_padf[0x4];  // 未消费
    int m_13;                 // +0x13
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    unsigned char m_35;       // +0x35
    unsigned char m_36;       // +0x36
    char m_37[1];             // +0x37 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=354（ORIG 0x084cb1ac）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=293（ORIG 0x84cb1ac）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化：ORIG 084ca0xx（else 支）比较处发 4 行
//   `mov %eax,%edx; mov <msg 槽>,%eax; mov 0x13(%eax),%eax; cmp %eax,%edx`
//   ⇒ +0x13 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorNoticeOtherChannelChatPktView.m_13 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeOtherChannelChatPktViewI
{
    char m_pad0[0xa];               // +0x00..未消费
    int m_a;                        // +0x0a
    signed char m_e;                // +0x0e
    char m_padf[0x4];               // 未消费
    unsigned int m_13;              // +0x13
    char m_17[1];                   // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];             // 未消费
    unsigned char m_35;             // +0x35
    unsigned char m_36;             // +0x36
    char m_37[1];                   // +0x37 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_47.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallGuildInfo.inc"
// ORIG 0x084cc006 T dispatch_sig —— 公会信息查询结果下发（0x4d 包，0x1d 步长成员数组）。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildInfoPktView —— Inter_MonitorCallGuildInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallGuildInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上）
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_48.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildInfo.inc"
// ORIG 0x084cb726 T dispatch_sig —— 监控服公会信息通知：落 SetGuildDBInfo 后组
// 0x46 公会信息包（成员等级过滤、公告/说明串）回发用户。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildInfoPktView —— Inter_MonitorNoticeGuildInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildInfoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    char m_pad12[0x4];  // 未消费
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x16];  // 未消费
    unsigned int m_2d;        // +0x2d（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    unsigned char m_31;       // +0x31
    unsigned char m_32;       // +0x32
    char m_pad33[0x5];  // 未消费
    unsigned short m_38;      // +0x38
    int m_3a;                 // +0x3a
    unsigned char m_3e;       // +0x3e
    int m_3f;                 // +0x3f
    char m_pad43[0x15];  // 未消费
    unsigned short m_58;      // +0x58
    unsigned char m_5a;       // +0x5a
    char m_pad5b[0x50];  // 未消费
    unsigned char m_ab;       // +0xab
    char m_padac[0x9];  // 未消费
    unsigned char m_b5;       // +0xb5
    int m_b6;                 // +0xb6
    char m_ba[1];             // +0xba 串/字节区首元素（容量未建模）
    char m_padbb[0x19];  // 未消费
    char m_d4[1];             // +0xd4 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_49.inc"


#include "InterDispatcher/dtor_Inter_MonitorSetGuildKey.inc"
// ORIG 0x084cbd26 T dispatch_sig —— 设置公会 key：state==2 登录期直写，state>2 校验角色号后发包。
// 报告 docs/class_func_reports/Inter_MonitorSetGuildKey/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorSetGuildKeyPktView —— Inter_MonitorSetGuildKey::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorSetGuildKeyPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned int m_12;        // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_50.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildCreate.inc"
// ORIG 0x084cbb94 T dispatch_sig —— 公会创建通知：写 guildkey 后转发公会名。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildCreate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildCreatePktView —— Inter_MonitorNoticeGuildCreate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildCreatePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a
    unsigned int m_e;         // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_51.inc"


#include "InterDispatcher/dtor_Inter_MonitorGuildMemberNamingReply.inc"
// ORIG 0x084cc23c T dispatch_sig —— 公会成员命名应答。
// 报告 docs/class_func_reports/Inter_MonitorGuildMemberNamingReply/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorGuildMemberNamingReplyPktView —— Inter_MonitorGuildMemberNamingReply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorGuildMemberNamingReplyPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_52.inc"


#include "InterDispatcher/dtor_Inter_MonitorSetSubGuildMasterReply.inc"
// ORIG 0x084cc35c T dispatch_sig —— 副会长设置应答（0x81 包 + GuildChangeGrade 日志）。
// 报告 docs/class_func_reports/Inter_MonitorSetSubGuildMasterReply/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorSetSubGuildMasterReplyPktView —— Inter_MonitorSetSubGuildMasterReply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorSetSubGuildMasterReplyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    unsigned char m_16;       // +0x16
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    unsigned char m_35;       // +0x35
    unsigned int m_36;        // +0x36
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_53.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildMemberInfo.inc"
// ORIG 0x084cc768 T dispatch_sig —— 公会成员信息通知：设成员等级并发 0x7f/0x02 包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildMemberInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildMemberInfoPktView —— Inter_MonitorNoticeGuildMemberInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeGuildMemberInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    char m_pad12[0x15];  // +0x12..未消费
    unsigned char m_27;             // +0x27
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_54.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeChangeGuildMemberGrade.inc"
// ORIG 0x084cc55e T dispatch_sig —— 监控服公会成员等级变更通知。
// 报告 docs/class_func_reports/Inter_MonitorNoticeChangeGuildMemberGrade/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeChangeGuildMemberGradePktView —— Inter_MonitorNoticeChangeGuildMemberGrade::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeChangeGuildMemberGradePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_55.inc"


#include "InterDispatcher/dtor_Inter_GuildNoticeGuildMessage.inc"
// ORIG 0x084cc650 T dispatch_sig —— 公会公告消息转发。
// 报告 docs/class_func_reports/Inter_GuildNoticeGuildMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildNoticeGuildMessagePktView —— Inter_GuildNoticeGuildMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildNoticeGuildMessagePktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_56.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberLoginOut.inc"
// ORIG 0x084cc90a T dispatch_sig —— 公会员登录/登出通知（0x48 包 + 上线成员连接字段设置）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberLoginOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberLoginOutPktView —— Inter_MonitorNoticeMemberLoginOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberLoginOutPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a（ORIG 0x084cc8xx put_byte 处 movzbl %al,%eax 零扩展 ⇒ unsigned）
    char m_padb[0x4];  // 未消费
    unsigned int m_f;         // +0x0f（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_13;       // +0x13
    unsigned char m_14;       // +0x14（同上 movzbl %al）
    char m_15[1];             // +0x15 串/字节区首元素（容量未建模）
    char m_pad16[0x1d];  // 未消费
    unsigned char m_33;       // +0x33
    unsigned int m_34;        // +0x34
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_57.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeRequestMemberEnterToRequester.inc"
// ORIG 0x084ccba0 T dispatch_sig —— 会员入会申请通知（发给申请者）：组 0x4f 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeRequestMemberEnterToRequester/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeRequestMemberEnterToRequesterPktView —— Inter_MonitorNoticeRequestMemberEnterToRequester::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeRequestMemberEnterToRequesterPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（unsigned 字段→get_charac_no(int) 失配两步 spill 形态）
    unsigned char m_12;       // +0x12（ORIG 0x084cc9xx movzbl %al,%eax 零扩展 ⇒ unsigned）
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_58.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeRequestMemberEnterToResponser.inc"
// ORIG 0x084ccd54 T dispatch_sig —— 公会员进入请求应答：单人模式/游戏选项→monitor 0x4b9 包，否则 0x49 拒绝包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeRequestMemberEnterToResponser/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeRequestMemberEnterToResponserPktView —— Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeRequestMemberEnterToResponserPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    short m_12;               // +0x12
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=159（ORIG 0x084ccd54）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84ccd54）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化：ORIG 084ccdxx 比较处发 4 行
//   `mov %eax,%edx; mov -0x10(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorNoticeRequestMemberEnterToResponserPktView.m_e 为 int
//   （R1 禁改聚合器，故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeRequestMemberEnterToResponserPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
    short m_12;                     // +0x12
    char m_14[1];                   // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_59.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberEnterOk.inc"
// ORIG 0x084cd218 T dispatch_sig —— 会员进入应答分发：solo 拒绝 / 上线会员设置 / 0x4a 状态包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberEnterOk/dispatch_sig.md 全文转写。
// msg 布局：+0xa result(byte) +0xb sub(byte) +0xc flag(byte) +0x11 charID(int)
//           +0x15 u8 +0x16 字符串 +0x34 u8 +0x35 uint。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberEnterOkPktView —— Inter_MonitorNoticeMemberEnterOk::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberEnterOkPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_result;   // +0x0a
    unsigned char m_sub;      // +0x0b
    unsigned char m_flag;     // +0x0c
    char m_padd[0x4];  // 未消费
    unsigned int m_11;        // +0x11（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_15;       // +0x15
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x1d];  // 未消费
    unsigned char m_34;       // +0x34
    unsigned int m_uint;      // +0x35
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_60.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberEnterReply.inc"
// ORIG 0x084cd5ca T dispatch_sig —— 公会员进入应答（0x50 包 + 上线成员字段设置）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberEnterReply/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberEnterReplyPktView —— Inter_MonitorNoticeMemberEnterReply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberEnterReplyPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a
    unsigned char m_b;        // +0x0b（ORIG `movzbl %al` 无符号扩展）
    unsigned char m_c;        // +0x0c
    char m_padd[0x4];  // 未消费
    unsigned int m_11;        // +0x11（ORIG set_charac_memberkey 实参单段 `mov 0x11(%eax),%edx`）
    unsigned char m_15;       // +0x15
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x1d];  // 未消费
    unsigned char m_34;       // +0x34
    unsigned int m_35;        // +0x35
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_61.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberSecede.inc"
// ORIG 0x084cd83a T dispatch_sig —— 会员退订通知（重置上位会员状态 + 0x51 包）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberSecede/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberSecedePktView —— Inter_MonitorNoticeMemberSecede::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberSecedePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned char m_12;       // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_62.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberSecedeToSeceder.inc"
// ORIG 0x084cda74 T dispatch_sig —— 会员退会通知（发给退会者）：清标记并组 0x4d 包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberSecedeToSeceder/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberSecedeToSecederPktView —— Inter_MonitorNoticeMemberSecedeToSeceder::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberSecedeToSecederPktView
{
    char m_pad0[0xe];  // 未消费
    // ORIG +0x5c 两步 spill（`mov %eax,%edx`+`mov -0x14(%ebp),%eax`）＝本字段 unsigned 与
    // get_charac_no(int) 返回类型 signedness 失配形态（探针 t1/g1 实证）。
    unsigned int m_e;         // +0x0e
    unsigned char m_12;       // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_63.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeDeleteMemberId.inc"
// ORIG 0x084cdc2c T dispatch_sig —— 删除公会成员通知：清成员字段并刷新队伍信息。
// 报告 docs/class_func_reports/Inter_MonitorNoticeDeleteMemberId/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeDeleteMemberIdPktView —— Inter_MonitorNoticeDeleteMemberId::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeDeleteMemberIdPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_64.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberChatMsg.inc"
// ORIG 0x084ccfda T dispatch_sig —— 会员聊天消息下发（memberkey 门 + 0x41 包）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberChatMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberChatMsgPktView —— Inter_MonitorNoticeMemberChatMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberChatMsgPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    char m_31[1];             // +0x31 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_65.inc"


#include "InterDispatcher/dtor_Inter_MonitorCallMemberList.inc"
// ORIG 0x084cddac T dispatch_sig —— 公会成员列表下发：头条目 + 0x2a 字节 stride 成员循环。
// 报告 docs/class_func_reports/Inter_MonitorCallMemberList/dispatch_sig.md 全文转写。
// msg 布局：+0xe charID(int)；+0x12 起条目区。
#pragma pack(push, 1)
// Inter_MonitorCallMemberListPktView —— Inter_MonitorCallMemberList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallMemberListPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_charID;                   // +0x0e 语义名←函数头注释
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_MonitorCallMemberListMemberWinView —— 同环成员记录（entry+0x2b 起，stride 0x2a；
//   字段宽度按 ORIG 084d0f30 段 disp 指令核实）。
struct Inter_MonitorCallMemberListMemberWinView
{
    unsigned char m_flags0;   // +0x00
    unsigned char m_flags1;   // +0x01
    char m_name[1];           // +0x02 名字串区首元素
    char m_pad3[0x1d];
    unsigned char m_grade20;  // +0x20
    unsigned char m_grade21;  // +0x21
    int m_22;                 // +0x22
    int m_26;                 // +0x26
};
static_assert(sizeof(Inter_MonitorCallMemberListMemberWinView) == 0x2a,
              "member record stride");
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_MonitorCallMemberListEntryWinView —— Inter_MonitorCallMemberList::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_MonitorCallMemberListEntryWinView
{
    unsigned char m_flags0;   // +0x00（ORIG entry[0] 直读）
    unsigned char m_flags1;   // +0x01（ORIG entry[1] 直读）
    char m_name[1];           // +0x02 成员名字符串区首元素（strlen/put_str 实参）
    char m_pad3[0x1d];  // 未消费
    unsigned char m_grade20;  // +0x20（ORIG entry[0x20]）
    unsigned char m_grade21;  // +0x21（ORIG entry[0x21]）
    int m_22;                 // +0x22
    int m_26;                 // +0x26
    unsigned char m_memberCnt;// +0x2a 同环成员数
    Inter_MonitorCallMemberListMemberWinView
        m_members[1];         // +0x2b 成员记录区（动态长度；[1]=首元素）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=233（ORIG 0x084cddac）：混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=106（ORIG 0x84cddac）：N22 cmp 操作数方向+N101 imul/lea 折叠+N18a 单段 regmove。
#pragma pack(push, 1)
// P248 同型化：ORIG 084d0fxx 比较处发 4 行
//   `mov %eax,%edx; mov -0x20(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ +0x0e 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorCallMemberListPktView.m_charID 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorCallMemberListPktViewI
{
    char m_pad0[0xe];               // +0x0..未消费
    unsigned int m_charID;          // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_66.inc"


#include "InterDispatcher/dtor_Inter_MonitorPayTaxToUpper.inc"
// ORIG 0x084ce164 T dispatch_sig —— 会员税额金币入账 + 会员奖励疲劳发放（上限钳制）。
// 报告 docs/class_func_reports/Inter_MonitorPayTaxToUpper/dispatch_sig.md 全文转写。
// msg 布局：+0xe charID(int) +0x12 税额金币(int) +0x16 奖励疲劳(uint) +0x1a 字符串。
#pragma pack(push, 1)
// Inter_MonitorPayTaxToUpperPktView —— Inter_MonitorPayTaxToUpper::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorPayTaxToUpperPktView
{
    char m_pad0[0xe];  // 未消费
    int m_charID;             // +0x0e
    int m_12;                 // +0x12
    unsigned int m_16;        // +0x16
    char m_1a[1];             // +0x1a 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=226（ORIG 0x084ce164）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=26（ORIG 0x84ce164）：N22 cmp 操作数方向。
#pragma pack(push, 1)
// P248 同型化：ORIG 084ce1xx 比较处发 4 行
//   `mov %eax,%edx; mov -0x28(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ +0x0e 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorPayTaxToUpperPktView.m_charID 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorPayTaxToUpperPktViewI
{
    char m_pad0[0xe];               // +0x0..未消费
    unsigned int m_charID;          // +0x0e
    // ORIG 084ce279 `mov 0x12(%eax),%eax; mov %eax,%ebx`（gain_money 前中转）
    //   ＝与 int 形参失配；084ce415 `put_int(pPkt->m_12)` 单段 ⇒ put_int 形参同为 unsigned
    //   （探针 t4/t6 实证）。
    unsigned int m_12;              // +0x12
    // ORIG 084ce36b `mov -0x10(%ebp),%edx; mov -0x28(%ebp),%eax; mov 0x16(%eax),%eax;
    //   cmp %eax,%edx; jbe` ⇒ 比较左=n、右=m_16 且为**异型**（故 gcc 不交换书写序）；
    //   探针 t5/t7：同型（unsigned）会归一化成先 m_16。故本字段为 int，条件写 n 在左。
    int m_16;                       // +0x16
    char m_1a[1];                   // +0x1a 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_67.inc"


#include "InterDispatcher/dtor_Inter_LoadPrivateStore.inc"
#include "InterDispatcher/dispatch_sig_68.inc"


#include "InterDispatcher/dtor_Inter_UpdateLogout.inc"
// ORIG 0x084ce548 T dispatch_sig —— 用户登出完成通知监控服。
// 报告 docs/class_func_reports/Inter_UpdateLogout/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdateLogoutPktView —— Inter_UpdateLogout::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_UpdateLogoutPktView
{
    int m_0;                  // +0x00
    char m_pad4[0xa];  // 未消费
    // [b28d-lane2] 档②字段类型旋钮（P211/P254）：ORIG 0x084ce581 段
    // `mov 0xe(%eax),%eax; mov %eax,%edx` 两段中转，探针 /tmp/l2probe/p6.cpp
    // 实证 int 字段 + cast 到 enum 实参才复现（unsigned 字段 4.4 恒折单段）。
    int m_e;                  // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_69.inc"


#include "InterDispatcher/dtor_Inter_UserRepel.inc"
#include "InterDispatcher/dispatch_sig_70.inc"
{
    user->DisConnSig(static_cast<DISCONN_SIG>(0x14), true, 0);
    return 0;
}

#include "InterDispatcher/dtor_Inter_AvatarInsert.inc"
// ORIG 0x084cf5a4 T dispatch_sig —— 跨服插入时装：槽位寻位+赋 key+回写槽。
// 报告 docs/class_func_reports/Inter_AvatarInsert/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AvatarInsertPktView —— Inter_AvatarInsert::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_AvatarInsertPktView
{
    char m_pad0[0x8];  // +0x0..未消费
    unsigned char m_8;              // +0x08
    char m_pad9[0x42];  // +0x9..未消费
    unsigned int m_4b;              // +0x4b
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=79（ORIG 0x084cf5a4）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=98（ORIG 0x84cf5a4）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化：ORIG 0x084cf60d `mov 0x4b(%eax),%esi` 为**单段**（无 unsigned→int 转换指令）
//   ⇒ +0x4b 字段语义是 int；聚合器 Inter_AvatarInsertPktView.m_4b 声明为 unsigned int
//   （R1 禁改聚合器，故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 int）。
struct Inter_AvatarInsertPktViewI
{
    char m_pad0[0x8];               // +0x00..未消费
    unsigned char m_8;              // +0x08
    char m_pad9[0x42];              // +0x09..未消费
    int m_4b;                       // +0x4b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_71.inc"


#include "InterDispatcher/dtor_Inter_AvatarChangeStat.inc"
// ORIG 0x084cf6ec T dispatch_sig —— 跨服时装属性变更登记。
// 报告 docs/class_func_reports/Inter_AvatarChangeStat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AvatarChangeStatPktView —— Inter_AvatarChangeStat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AvatarChangeStatPktView
{
    ENUM_VIEW_FIELD m_0;      // +0x00（P254 枚型：ORIG 0x084cf7xx 三处保留 eax→edx 中转）
    char m_pad4[0x4];  // 未消费
    unsigned char m_8;        // +0x08
    char m_pad9[0x1];  // 未消费
    ENUM_VIEW_FIELD m_a;      // +0x0a
    ENUM_VIEW_FIELD m_e;      // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_72.inc"


#include "InterDispatcher/dtor_Inter_TriggerEventStart.inc"
// ORIG 0x084ce5f4 T dispatch_sig —— 事件开始触发。
// 报告 docs/class_func_reports/Inter_TriggerEventStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TriggerEventStartPktView —— Inter_TriggerEventStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TriggerEventStartPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    // ORIG +0x15 两步装载（Ghidra `*(undefined4 *)(param_3 + 10)` → TriggerEventStart(int,...)）
    // ＝unsigned 字段→int 形参的 signedness 失配形态（探针 t1 实证）。
    unsigned int m_a;               // +0x0a
    Word_Param m_wp;                // +0x0e（ORIG `mov 0xe(%ecx),%ecx` 单指令折叠，P1）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_73.inc"


#include "InterDispatcher/dtor_Inter_TriggerEventEnd.inc"
// ORIG 0x084ce646 T dispatch_sig —— 事件结束触发。
// 报告 docs/class_func_reports/Inter_TriggerEventEnd/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TriggerEventEndPktView —— Inter_TriggerEventEnd::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_TriggerEventEndPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_74.inc"


#include "InterDispatcher/dtor_Inter_MailBox_Req_List.inc"
#include "InterDispatcher/dispatch_sig_75.inc"


#include "InterDispatcher/dtor_Inter_MailBox_Req_CheckReceiver.inc"
// ORIG 0x084cf95a T dispatch_sig —— 邮箱查验收件人请求：按检查类型（普通邮件 /
// cera 直购 / 多附件邮件）完成校验、购买或入 DB 线程，并回 ACK 错误包。
// 报告 docs/class_func_reports/Inter_MailBox_Req_CheckReceiver/dispatch_sig.md 全文转写。
// SendErrorPacket/SendDBThread_forchina 为本类既有方法（NEEDS_DECL 见 meta）。
// [b28d-lane2] verdict=DIFF ndiff=373（ORIG 0x084cf95a）：槽序/帧差(N82/P212/P221)+混合残差；gap-9。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=303（ORIG 0x84cf95a）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
// [L9-H 2026-09-11] P235/P30「具名载荷视图」：ORIG 084cf95a 段对载荷偏移全发单条 disp 折叠
//   （`mov 0x31(%eax),%eax`、`mov 0x1d(%eax),%eax`、`lea 0x46(%eax),%esi`、`mov 0x63(%eax)`…），
//   我方 `reinterpret_cast<char*>(sig) + 0xNN` 的 char* 算术每条多 1 条 `add $0xNN,%eax`
//   （探针 /tmp/L9/H/p1.cpp：char*+常量折 add+mov 两条；成员访问折单条 disp）。
//   真实 SIG_MAILBOX_CHECK_RECEIVER 只具名了 DB 回填区（+0x1d..+0x2d）与 m_payload[0x3f88]，
//   载荷内偏移无成员 ⇒ 本件自带同布局视图（+0x63..+0x6e 区在两条路径下互斥复用 ⇒ union）。
#pragma pack(push, 1)
struct Inter_MailBox_Req_CheckReceiverLoadView
{
    char m_receiverName[0x1d];        // +0x00（同权威 SIG）
    unsigned int m_receiverCharacNo;  // +0x1d
    unsigned int m_receiverLevel;     // +0x21
    unsigned int m_receiverDeleteFlag;// +0x25
    unsigned int m_receiverAccId;     // +0x29
    unsigned int m_senderCharacNo;    // +0x2d
    int m_checkType;                  // +0x31 ⇒ ORIG `mov 0x31(%eax),%eax`（1=cera/2=多附件）
    char m_payload35[0x11];           // +0x35..+0x45 MSG_BUY_CERASHOP_ITEM / MSG_MAILBOX_SEND 载荷
    char m_name46[0x1d];              // +0x46..+0x62 GetMoneyLimitPerLevel 名称实参
    union                             // +0x63..+0x66：三条路径互斥复用（ORIG 各为独立 disp 直取）
    {
        unsigned int m_63;            // +0x63 单附件金额阈值
        unsigned int m_64;            // +0x64 多附件金额阈值
        unsigned int m_67;            // +0x67 cera 路径目标槽
        unsigned char m_68;           // +0x68 多附件计数
        char m_items6c[1];            // +0x6c 多附件 itemIdx 数组（stride 0xb）
        unsigned int m_6e;            // +0x6e 单附件 itemIdx
    } m_tail;
    char m_pad67[0x3fb9 - 0x67];      // 到 m_resultFlag
    unsigned int m_resultFlag;        // +0x3fb9 DB 结果（0 ok / 0x4d 名称非法）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_76.inc"

#include "InterDispatcher/SendErrorPacket.inc"


#include "InterDispatcher/SendDBThread_forchina.inc"


#include "InterDispatcher/dtor_Inter_MailBox_Req_Send.inc"
// ORIG 0x084d0130 T dispatch_sig —— 邮箱新邮件通知转发监控服。
// 报告 docs/class_func_reports/Inter_MailBox_Req_Send/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MailBox_Req_SendPktView —— Inter_MailBox_Req_Send::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MailBox_Req_SendPktView
{
    int m_0;                        // +0x00
    unsigned int m_4;               // +0x04
    // [b28d-lane2] 档②字段类型旋钮（P211/P254）：ORIG 0x084d014f 段为
    // `mov 0x8(%eax),%eax; mov %eax,%edx`（两段中转），探针 /tmp/l2probe/p6.cpp
    // 实证 4.4.7 仅「int 字段 + cast 到 enum 实参」产两段（unsigned 字段恒折单段）。
    int m_8;                        // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_77.inc"


#include "InterDispatcher/dtor_Inter_Monitor_Notify_New_Mail.inc"
// ORIG 0x084d017c T dispatch_sig —— 监控侧新邮件到达通知。
// 报告 docs/class_func_reports/Inter_Monitor_Notify_New_Mail/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_Notify_New_MailPktView —— Inter_Monitor_Notify_New_Mail::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Monitor_Notify_New_MailPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_78.inc"


#include "InterDispatcher/dtor_Inter_InsertCreatureItemResult.inc"
// ORIG 0x084d027e T dispatch_sig —— 生物蛋孵化结果插入（校验当前角色后透传）。
// 报告 docs/class_func_reports/Inter_InsertCreatureItemResult/dispatch_sig.md 全文转写。
// 包视图直接用 SIG_CREATURE_ITEM（m_characNo@+0x48 单级带偏移读，ORIG 0x084d02a4）。

#include "InterDispatcher/dispatch_sig_79.inc"


#include "InterDispatcher/dtor_Inter_UpdateCreatureItemResult.inc"
#include "InterDispatcher/dispatch_sig_80.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMessage.inc"
// ORIG 0x084d035e T dispatch_sig —— 监控侧公告全员播发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMessagePktView —— Inter_MonitorNoticeMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMessagePktView
{
    char m_pad0[0xa];  // 未消费
    char m_a;                 // +0x0a
    char m_b[1];              // +0xb 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_81.inc"


#include "InterDispatcher/dtor_Inter_LastMsgResponse.inc"
#include "InterDispatcher/dispatch_sig_82.inc"


#include "InterDispatcher/dtor_Inter_MonitorGuildPointDel.inc"
#include "InterDispatcher/_makeDefaultTime.inc"
{
    time_t now = time(0);
    struct tm localTm;
    struct tm* pTm = localtime_r(&now, &localTm);
    pTm->tm_mday = 1;
    pTm->tm_hour = 5;
    pTm->tm_min = 0;
    pTm->tm_sec = 0;
    if (pTm->tm_mon == 6 && pTm->tm_year == 0x6a)
        pTm->tm_mday = 6;
    now = mktime(pTm);
    return now;
}
#include "InterDispatcher/dispatch_sig_83.inc"
{
    std::vector<std::pair<int, int> > items;
    items.push_back(std::make_pair(0xc84, 0));
    G_GameWorld()->deleteSpecificItems(&items);
    Inter_MonitorGuildPointDel::setLastDeleteTime();
    return 0;
}

#include "InterDispatcher/dtor_Inter_MonitorGuildWarInfo.inc"
// ORIG 0x084ce7f6 T dispatch_sig —— 公会战信息包（0x70，频道 6 专用）。
// 报告 docs/class_func_reports/Inter_MonitorGuildWarInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorGuildWarInfoPktView —— Inter_MonitorGuildWarInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorGuildWarInfoItemView
{
    int m_12;                         // +0x00（abs +0x12，ORIG `add $0x10; mov 0x2(%eax)`）
    int m_16;                         // +0x04（abs +0x16）
    unsigned short m_1a;              // +0x08（abs +0x1a）
    char m_1c[0x17];                  // +0x0a（abs +0x1c，stride 0x21）
};
struct Inter_MonitorGuildWarInfoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;                  // +0x0e
    Inter_MonitorGuildWarInfoItemView m_items[10];  // +0x12，stride 0x21
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_84.inc"


#include "InterDispatcher/dtor_Inter_CodeChecksumRange.inc"
#include "InterDispatcher/dispatch_sig_85.inc"


#include "InterDispatcher/dtor_Inter_MonitorForbidChat.inc"
// ORIG 0x084d0466 T dispatch_sig —— 监控侧禁言指令落地。
// 报告 docs/class_func_reports/Inter_MonitorForbidChat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorForbidChatPktView —— Inter_MonitorForbidChat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorForbidChatPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    size_t m_e;               // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_86.inc"


#include "InterDispatcher/dtor_Inter_MouseRegister.inc"
// ORIG 0x084d07d2 T dispatch_sig —— 密保鼠标注册/注销结果处理，回发 0x74 应答。
// 报告 docs/class_func_reports/Inter_MouseRegister/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MouseRegisterPktView —— Inter_MouseRegister::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MouseRegisterPktView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    char m_pad8[0xe];  // 未消费
    int m_16;                 // +0x16
    char m_pad1a[0x1];  // 未消费
    int m_1b;                 // +0x1b
    int m_1f;                 // +0x1f
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_87.inc"


#include "InterDispatcher/dtor_Inter_Monitor_MegaphoneMsg.inc"
// ORIG 0x084d09aa T dispatch_sig —— 大喇叭广播：构造广播包后按频道类型全服/分组发送。
// 报告 docs/class_func_reports/Inter_Monitor_MegaphoneMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_MegaphoneMsgPktView —— Inter_Monitor_MegaphoneMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_MegaphoneMsgPktView
{
    char m_pad0[0xa];  // 未消费
    char m_a;                 // +0x0a 服务器组（send_server_group 第二实参，ORIG +0x126 movzbl/movsbl）
    unsigned char m_b;        // +0x0b
    unsigned short m_c;       // +0x0c
    unsigned char m_e;        // +0x0e
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
    char m_pad10[0x1d];  // 未消费
    unsigned char m_2d;       // +0x2d
    char m_2e[1];             // +0x2e 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_88.inc"


#include "InterDispatcher/dtor_Inter_Monitor_RegisterBlackListResult.inc"
// ORIG 0x084d0de0 T dispatch_sig —— 黑名单登记结果通知（0x79 包，按结果码填错误值）。
// 报告 docs/class_func_reports/Inter_Monitor_RegisterBlackListResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_RegisterBlackListResultPktView —— Inter_Monitor_RegisterBlackListResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_RegisterBlackListResultPktView
{
    char m_pad0[0xe];  // 未消费
    char m_e[1];              // +0xe 串/字节区首元素（容量未建模）
    char m_padf[0x1d];  // 未消费
    unsigned int m_2c;        // +0x2c
    unsigned char m_30;       // +0x30
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_89.inc"


#include "InterDispatcher/dtor_Inter_Monitor_DeleteBlackListResult.inc"
// ORIG 0x084d1034 T dispatch_sig —— 黑名单删除结果应答：按结果码组 0x7a 包回发。
// 报告 docs/class_func_reports/Inter_Monitor_DeleteBlackListResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_DeleteBlackListResultPktView —— Inter_Monitor_DeleteBlackListResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_DeleteBlackListResultPktView
{
    char m_pad0[0xe];  // 未消费
    char m_e[1];              // +0xe 串/字节区首元素（容量未建模）
    char m_padf[0x1d];  // 未消费
    unsigned int m_2c;        // +0x2c
    unsigned char m_30;       // +0x30
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_90.inc"


#include "InterDispatcher/dtor_Inter_Monitor_RequestBlackListResult.inc"
// ORIG 0x084d0ba6 T dispatch_sig —— 黑名单查询结果下发（名字+时间+注册黑名单）。
// 报告 docs/class_func_reports/Inter_Monitor_RequestBlackListResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_RequestBlackListResultPktView —— Inter_Monitor_RequestBlackListResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Monitor_RequestBlackListResultPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned char m_e;              // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_91.inc"


#include "InterDispatcher/dtor_Inter_DB_RequestBlackCount.inc"
// ORIG 0x084d1218 T dispatch_sig —— DB 黑名单计数应答。
// 报告 docs/class_func_reports/Inter_DB_RequestBlackCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DB_RequestBlackCountPktView —— Inter_DB_RequestBlackCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DB_RequestBlackCountPktView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    int m_8;                  // +0x08（ORIG +0x2f 两步 spill＝signed 字段→setCurrCharacBlackCount(unsigned) 失配）
    int m_c;                  // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_92.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeBlackList.inc"
// ORIG 0x084d0b3a T dispatch_sig —— 监控服黑名单列表登记。
// 报告 docs/class_func_reports/Inter_MonitorNoticeBlackList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeBlackListPktView —— Inter_MonitorNoticeBlackList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// [b28d-lane2] P235 双视角：id 数组与头字段同址重叠，合入一个完整类型的匿名 union
//   （原两个独立视图经 reinterpret_cast 访问使 4.4 恒「base 先载」；union 完整类型成员
//   直访问复现 ORIG 0x084d0b73 的「index 先载」序，探针 /tmp/l2probe/p11.cpp fa 实证）。
struct Inter_MonitorNoticeBlackListPktView
{
    char m_pad0[3];               // +0x0
    union
    {
        // P311 二维视图：`m_ids[1][i]` 地址等价旧 `m_ids[i+4]`，但 4.4 对常量首维发
        // `add $0x4,%edx`（ORIG 形）而非 `lea 0x4(%eax),%edx`；sizeof 1040 不变。
        unsigned int m_ids[65][4];  // +0x3（ORIG `mov 0x3(%eax,%edx,4),%eax`；容量上界 len/4）
        struct
        {
            char m_pad3[0xf];     // +0x3..+0x11
            unsigned char m_12;   // +0x12 id 个数
        } m_hdr;
    };
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_93.inc"


#include "InterDispatcher/dtor_Inter_WebNoticeSingle.inc"
// ORIG 0x084d1344 T dispatch_sig —— 网页单条公告全员播发。
// 报告 docs/class_func_reports/Inter_WebNoticeSingle/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_WebNoticeSinglePktView —— Inter_WebNoticeSingle::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_WebNoticeSinglePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a
    char m_b[1];              // +0xb 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_94.inc"


#include "InterDispatcher/dtor_Inter_MemberExpLevelUp.inc"
// ORIG 0x084d1448 T dispatch_sig —— 会员经验等级上限同步。
// 报告 docs/class_func_reports/Inter_MemberExpLevelUp/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MemberExpLevelUpPktView —— Inter_MemberExpLevelUp::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MemberExpLevelUpPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_95.inc"


#include "InterDispatcher/dtor_Inter_MonitorAddBuddy.inc"
// ORIG 0x084d14b0 T dispatch_sig —— 好友添加结果通知（0x3f 包，成功带好友信息/失败带错误码）。
// 报告 docs/class_func_reports/Inter_MonitorAddBuddy/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorAddBuddyPktView —— Inter_MonitorAddBuddy::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorAddBuddyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    short m_30;               // +0x30
    signed char m_32;         // +0x32
    signed char m_33;         // +0x33
    int m_34;                 // +0x34
    signed char m_38;         // +0x38
    unsigned char m_39;       // +0x39
    char m_3a;                // +0x3a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_96.inc"


#include "InterDispatcher/dtor_Inter_MonitorDelBuddy.inc"
// ORIG 0x084d16f6 T dispatch_sig —— 好友删除结果应答：按错误码组 0x40 包回发。
// 报告 docs/class_func_reports/Inter_MonitorDelBuddy/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorDelBuddyPktView —— Inter_MonitorDelBuddy::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorDelBuddyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    signed char m_30;         // +0x30
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_97.inc"


#include "InterDispatcher/dtor_Inter_ChangeLetterStat.inc"
// ORIG 0x084d18ac T dispatch_sig —— 信件状态变更（keep 计数增删 + 0x89 回包）。
// 报告 docs/class_func_reports/Inter_ChangeLetterStat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ChangeLetterStatPktView —— Inter_ChangeLetterStat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ChangeLetterStatPktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04
    unsigned short m_8;       // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_98.inc"


#include "InterDispatcher/dtor_Inter_SaveClientHackLog.inc"
// ORIG 0x084d1ab6 T dispatch_sig —— 客户端外挂日志：按类型表计算掉率惩罚并入库。
// 报告 docs/class_func_reports/Inter_SaveClientHackLog/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SaveClientHackLogPktView —— Inter_SaveClientHackLog::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SaveClientHackLogPktView
{
    unsigned int m_0;         // +0x00
    char m_pad4[0x4];  // 未消费
    unsigned short m_8;       // +0x08
    char m_pada[0x4];  // 未消费
    int m_e;                  // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_99.inc"


#include "InterDispatcher/dtor_Inter_ReqChangeCharacName.inc"
#pragma pack(push, 1)
// Inter_ReqChangeCharacNamePktView —— Inter_ReqChangeCharacName::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqChangeCharacNamePktView
{
    int m_0;                  // +0x00
    char m_pad4[0x20];  // 未消费
    int m_24;                 // +0x24
    int m_28;                 // +0x28
    int m_2c;                 // +0x2c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_100.inc"


#include "InterDispatcher/dtor_Inter_ReqQueryCharacInfo.inc"
// ORIG 0x084d22fc T dispatch_sig —— 角色信息查询应答：组 0x8b 包回发。
// 报告 docs/class_func_reports/Inter_ReqQueryCharacInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqQueryCharacInfoPktView —— Inter_ReqQueryCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqQueryCharacInfoPktView
{
    char m_pad0[0x20];  // 未消费
    int m_20;                 // +0x20
    char m_pad24[0x4];  // 未消费
    signed char m_28;         // +0x28
    char m_pad29[0x1];  // 未消费
    short m_2a;               // +0x2a
    signed char m_2c;         // +0x2c
    signed char m_2d;         // +0x2d
    signed char m_2e;         // +0x2e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_101.inc"


#include "InterDispatcher/dtor_Inter_ReportMannerless.inc"
// ORIG 0x084d24a2 T dispatch_sig —— 无礼行为举报：按类型回错误码或转发 LOGDB 队列 + 确认包。
// 报告 docs/class_func_reports/Inter_ReportMannerless/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReportMannerlessPktView —— Inter_ReportMannerless::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReportMannerlessPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    char m_pad12[0xc];  // +0x12..未消费
    unsigned int m_1e;              // +0x1e
    char m_pad22[0x13c];  // +0x22..未消费
    unsigned char m_15e;            // +0x15e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_102.inc"


#include "InterDispatcher/dtor_Inter_ChagneUserHandicap.inc"
// ORIG 0x084d271e T dispatch_sig —— 用户掉落率调整（handicap）。
// 报告 docs/class_func_reports/Inter_ChagneUserHandicap/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ChagneUserHandicapPktView —— Inter_ChagneUserHandicap::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ChagneUserHandicapPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_103.inc"


#include "InterDispatcher/dtor_Inter_GMRequestMid.inc"
// ORIG 0x084d279e T dispatch_sig —— GM mid 请求转发。
// 报告 docs/class_func_reports/Inter_GMRequestMid/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GMRequestMidPktView —— Inter_GMRequestMid::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GMRequestMidPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_104.inc"


#include "InterDispatcher/dtor_Inter_UDP_Incoming_Capcha_Data.inc"
#include "InterDispatcher/dispatch_sig_105.inc"


#include "InterDispatcher/dtor_Inter_Add_Slang.inc"
#include "InterDispatcher/dispatch_sig_106.inc"
{
    char* packet = data;
    std::string slang(packet + 0xa);
    CSyncSlangFilter::GetInstance()->AddSlang(slang);
    return 0;
}

#include "InterDispatcher/dtor_Inter_ReplyGuildLetter.inc"
// ORIG 0x084d28ca T dispatch_sig —— 公会信件回复应答转发。
// 报告 docs/class_func_reports/Inter_ReplyGuildLetter/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildLetterPktView —— Inter_ReplyGuildLetter::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyGuildLetterPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    char m_12;                      // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_107.inc"


#include "InterDispatcher/dtor_Inter_CallGuildInviteToCaller.inc"
// ORIG 0x084d2a26 T dispatch_sig —— 公会邀请回执：按结果码回执发起者。
// 报告 docs/class_func_reports/Inter_CallGuildInviteToCaller/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CallGuildInviteToCallerPktView —— Inter_CallGuildInviteToCaller::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_CallGuildInviteToCallerPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_108.inc"


#include "InterDispatcher/dtor_Inter_CallGuildInviteToInvited.inc"
// ORIG 0x084d2b96 T dispatch_sig —— 被邀请方公会邀请应答转发（0x93 包 + 拒绝码上报）。
// 报告 docs/class_func_reports/Inter_CallGuildInviteToInvited/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CallGuildInviteToInvitedPktView —— Inter_CallGuildInviteToInvited::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CallGuildInviteToInvitedPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    char m_30[1];             // +0x30 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_109.inc"


#include "InterDispatcher/dtor_Inter_ReplyGuildInviteToInvited.inc"
// ORIG 0x084d2dfe T dispatch_sig —— 公会邀请回应：被邀者按结果码回执。
// 报告 docs/class_func_reports/Inter_ReplyGuildInviteToInvited/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildInviteToInvitedPktView —— Inter_ReplyGuildInviteToInvited::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyGuildInviteToInvitedPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_110.inc"


#include "InterDispatcher/dtor_Inter_ReplyGuildInviteToCaller.inc"
// ORIG 0x084d2f6e T dispatch_sig —— 公会邀请应答转发（SoloPlay 拒绝包 / 0x94 通知包）。
// 报告 docs/class_func_reports/Inter_ReplyGuildInviteToCaller/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildInviteToCallerPktView —— Inter_ReplyGuildInviteToCaller::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReplyGuildInviteToCallerPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a（P342 异型轴一次尝试：callee int ↔ 字段 unsigned 触发
                              //   ORIG `mov %eax,%edx` 中转；S1 协议单形试）
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_111.inc"


#include "InterDispatcher/dtor_Inter_LoadDeathTowerBestRecord.inc"
// ORIG 0x084d316a T dispatch_sig —— 死亡之塔个人最优纪录批量登记。
// 报告 docs/class_func_reports/Inter_LoadDeathTowerBestRecord/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDeathTowerBestRecordPktView —— Inter_LoadDeathTowerBestRecord::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadDeathTowerBestRecordPktView
{
    unsigned int m_0;         // +0x00
    unsigned int m_4;         // +0x04
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_LoadDeathTowerBestRecordRecWinView —— Inter_LoadDeathTowerBestRecord::dispatch_sig 窗口 rec 元素视图
//   （rec 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_LoadDeathTowerBestRecordRecWinView
{
    char m_pad0[0x10];  // 未消费
    unsigned char m_10;         // +0x10 §4.3 成员数组基址槽（stDeathTowerRecordMemberInfo_t*）
    char m_pad11[0x5b];  // 未消费
    int m_6c;                 // +0x6c
    unsigned int m_70;        // +0x70
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_112.inc"


#include "InterDispatcher/dtor_Inter_DeathTowerLoadTopRanker.inc"
// ORIG 0x084d3364 T dispatch_sig —— 死亡之塔榜首记录批量装载。
// 报告 docs/class_func_reports/Inter_DeathTowerLoadTopRanker/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DeathTowerLoadTopRankerPktView —— Inter_DeathTowerLoadTopRanker::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DeathTowerLoadTopRankerPktView
{
    unsigned int m_0;         // +0x00
    char m_4;                 // +0x04
};
#pragma pack(pop)

#pragma pack(push, 1)
// [R5·InterDisp 形态收口 2026-09-11] 负结论（已回滚，反劣）：ORIG 对 rec 内字段发
//   `add $0xfa0,%eax` + `movzwl 0xc(%eax)`/`movzwl 0xe(%eax)`（0x084d340c/0x084d3424）、
//   `add $0xfa0; add -0x24(%ebp); add $0x10`（0x084d33e4，记录体 +0xfb0），
//   而 `add $0x1010`/`add $0x100c`（0x084d33c1/0x084d33d7）为独立裸偏移。
//   试「rec +0xfa0 具名子结构视图（m_c/m_e/m_pad10）+ +0x100c/+0x1010 裸偏移」复现该两步形：
//   实测 ndiff 18→22（反劣，our_insns 87 不变）⇒ 回滚，判 gcc 指令选择/寄存器分配族。
struct Inter_DeathTowerLoadTopRankerRecWinView   // rec 元素视图（扁平绝对偏移）
{
    char m_pad0[0xfac];  // 未消费
    unsigned short m_fac;     // +0xfac
    unsigned short m_fae;     // +0xfae
    char m_padfb0[0x5c];  // 未消费
    unsigned int m_100c;      // +0x100c
    unsigned int m_1010;      // +0x1010
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_113.inc"


#include "InterDispatcher/dtor_Inter_MonitorDeathTowerRanking.inc"
// ORIG 0x084d3290 T dispatch_sig —— 死亡之塔排行登记。
// 报告 docs/class_func_reports/Inter_MonitorDeathTowerRanking/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=59（ORIG 0x084d3290）：槽序/帧差(N82/P212/P221)+混合残差；gap+12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=78（ORIG 0x84d3290）：N82/P112 帧槽平移+P30/N4 add-vs-lex。
// [lane2 2026-09-10] 裸偏移指针算术 → 视图数组成员（`pPkt->m_ranks[i*4+j+4]`）复现
//   ORIG 084d32c3 `add $0x4,%edx; mov 0x2(%eax,%edx,4)`；补包基址具名局部（ORIG -0x24）。
//   残余 = 索引计算寄存器分配/label 顺序（gap -6，N18a/LRA 族）。
#pragma pack(push, 1)
// Inter_MonitorDeathTowerRankingPktView —— 排行表（ORIG 084d32c3 `add $0x4,%edx;
//   mov 0x2(%eax,%edx,4)` ⇒ base=msg、数组元素 @+0x02、索引 (i*4+j+4) 单条 scaled）。
struct Inter_MonitorDeathTowerRankingPktView
{
    char m_pad0[0x2];
    unsigned int m_ranks[20];
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_114.inc"


#include "InterDispatcher/dtor_Inter_MonitorTowerReloadTopRanker.inc"
#include "InterDispatcher/dispatch_sig_115.inc"
{
    DB_DeathTowerLoadTopRanker::makeRequest(true);
    return 0;
}

#include "InterDispatcher/dtor_Inter_ReplyGuildSecede.inc"
// ORIG 0x084c9768 T dispatch_sig —— 退出公会应答：校验角色号后发 0x9c 包并刷 item lock 列表。
// 报告 docs/class_func_reports/Inter_ReplyGuildSecede/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildSecedePktView —— Inter_ReplyGuildSecede::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReplyGuildSecedePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a
    short m_e;                // +0x0e
    // [L9·2026-09-11 子代理 B-PATCH#1] int→unsigned int 同型化（P211/P265 档③；sizeof/偏移/mangled
    // 零影响）：ORIG 084c97a8 `mov %eax,%edx; mov -0x24(%ebp),%eax; mov 0x10(%eax),%eax; cmp %eax,%edx`
    // 比较点中转直证 + 084c997b `mov 0x10(%eax),%edx` 单段；探针 /tmp/L9/B/pb_cmp.cpp fu 逐条同形。
    unsigned int m_10;        // +0x10
    char m_pad14[0x4];  // 未消费
    int m_18;                 // +0x18
    signed char m_1c;         // +0x1c
    char m_1d[1];             // +0x1d 串/字节区首元素（容量未建模）
    char m_pad1e[0x1d];  // 未消费
    char m_3b[1];             // +0x3b 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_116.inc"


#include "InterDispatcher/dtor_Inter_NoticeGuildMasterDelegate.inc"
// ORIG 0x084c9a32 T dispatch_sig —— 会长委任通知：转发新会长名字。
// 报告 docs/class_func_reports/Inter_NoticeGuildMasterDelegate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildMasterDelegatePktView —— Inter_NoticeGuildMasterDelegate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeGuildMasterDelegatePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;       // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_117.inc"


#include "InterDispatcher/dtor_Inter_GuildReplyGuildMasterDelegate.inc"
// ORIG 0x084c9b9a T dispatch_sig —— 会长委任回应：按结果码回执。
// 报告 docs/class_func_reports/Inter_GuildReplyGuildMasterDelegate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildReplyGuildMasterDelegatePktView —— Inter_GuildReplyGuildMasterDelegate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildReplyGuildMasterDelegatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_118.inc"


#include "InterDispatcher/dtor_Inter_NoticeHasBeenGuildMaster.inc"
// ORIG 0x084c9d0a T dispatch_sig —— 已成为公会会长通知。
// 报告 docs/class_func_reports/Inter_NoticeHasBeenGuildMaster/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeHasBeenGuildMasterPktView —— Inter_NoticeHasBeenGuildMaster::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeHasBeenGuildMasterPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_119.inc"


#include "InterDispatcher/dtor_Inter_DBCheckDoubleGuildName.inc"
// ORIG 0x084d347e T dispatch_sig —— 公会名重复检查应答。
// 报告 docs/class_func_reports/Inter_DBCheckDoubleGuildName/dispatch_sig.md 全文转写。
// [N 判词·2026-09-11 复核] 残 2 块 = **N104 EH 尾块相对序**：ORIG 0x084d3559/0x084d355f
//   （正常路径 ~PacketGuard 在 `jmp +0x103` 之前）vs 我方把该 D1 块压到函数最末
//   （0x084d357c `_Unwind_Resume` 之后）。两侧均 2 次 D1、指令逐条全等 ⇒ 仅块位互换，
//   跨版本（4.1 正常路径尾块就地 / 4.4 提取共享尾），无源码旋钮。
#pragma pack(push, 1)
// Inter_DBCheckDoubleGuildNamePktView —— Inter_DBCheckDoubleGuildName::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DBCheckDoubleGuildNamePktView
{
    char m_0;                 // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_120.inc"


#include "InterDispatcher/dtor_Inter_DBCheckDoubleGuildAddress.inc"
// ORIG 0x084d358e T dispatch_sig —— 公会地址重复检查应答。
// 报告 docs/class_func_reports/Inter_DBCheckDoubleGuildAddress/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DBCheckDoubleGuildAddressPktView —— Inter_DBCheckDoubleGuildAddress::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DBCheckDoubleGuildAddressPktView
{
    char m_0;                 // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_121.inc"


#include "InterDispatcher/dtor_Inter_ReplyGuildCreate.inc"
// ORIG 0x084d368a T dispatch_sig —— 建会应答：成功 → GuildCreate 日志 + 公会代理缓存 +
// 全服基础信息广播；失败 → 退还建会费用并刷新物品列表。
// 报告 docs/class_func_reports/Inter_ReplyGuildCreate/dispatch_sig.md 全文转写。
// msg 布局：+0xa charID(int) +0x12 result(int) +0x16 公会名(0x16)。
#pragma pack(push, 1)
// Inter_ReplyGuildCreatePktView —— Inter_ReplyGuildCreate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReplyGuildCreatePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;       // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // 未消费
    int m_result;             // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_122.inc"


#include "InterDispatcher/dtor_Inter_NoticeGuildNameOnGuildCreate.inc"
// ORIG 0x084d39ae T dispatch_sig —— 公会创建时公会名登记到代理。
// 报告 docs/class_func_reports/Inter_NoticeGuildNameOnGuildCreate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildNameOnGuildCreatePktView —— Inter_NoticeGuildNameOnGuildCreate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeGuildNameOnGuildCreatePktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned int m_12;              // +0x12
    char m_pad16[0x17];  // +0x16..未消费
    unsigned char m_2d;             // +0x2d
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_123.inc"


#include "InterDispatcher/dtor_Inter_SecurityCardIssue.inc"
// ORIG 0x084d3a22 T dispatch_sig —— 安全卡发放：加密序列号/账号/答案串并下发 0xa4 包，
// 本地初始化 CSecurityCard。msg 布局：+4 serial +0x25 answers +0x37 serial 原文
// +0x5e answers 原文 +0x8c 错误码(int)。
// 报告 docs/class_func_reports/Inter_SecurityCardIssue/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecurityCardIssuePktView —— Inter_SecurityCardIssue::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SecurityCardIssuePktView
{
    char m_pad0[0x8c];  // 未消费（N：+0x04/+0x25/+0x37/+0x5e 串区基址——拆 pad 建具名槽使 setSecurityCardInfo 多指针参装载重排、破坏 AE，保留 reinterpret_cast）
    int m_8c;                 // +0x8c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_124.inc"


#include "InterDispatcher/dtor_Inter_SecurityCardActivate.inc"
// ORIG 0x084d3d56 T dispatch_sig —— 安全卡激活应答。
// 报告 docs/class_func_reports/Inter_SecurityCardActivate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecurityCardActivatePktView —— Inter_SecurityCardActivate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SecurityCardActivatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_125.inc"


#include "InterDispatcher/dtor_Inter_SecurityCardRetransfer.inc"
// ORIG 0x084d3e78 T dispatch_sig —— 安全卡补发：加密串下发 0xca 包并重置 CSecurityCard。
// msg 布局：+4 serial +0x2b serial 原文 +0x52 answers +0x88 错误码(int)。
// 报告 docs/class_func_reports/Inter_SecurityCardRetransfer/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecurityCardRetransferPktView —— Inter_SecurityCardRetransfer::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SecurityCardRetransferPktView
{
    char m_pad0[0x88];  // 未消费
    int m_88;                 // +0x88
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_126.inc"


#include "InterDispatcher/dtor_Inter_LoadDnfHackLog.inc"
// ORIG 0x084d41aa T dispatch_sig —— DNF hack 日志累计计数同步。
// 报告 docs/class_func_reports/Inter_LoadDnfHackLog/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDnfHackLogPktView —— Inter_LoadDnfHackLog::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadDnfHackLogPktView
{
    int m_0;                        // +0x00
    unsigned int m_4[1];            // +0x04 累计计数表（ORIG `mov 0x4(%eax,%edx,4)` scaled-index）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_127.inc"


#include "InterDispatcher/dtor_Inter_AutoPunishRuleBlackIP.inc"
// ORIG 0x084d422a T dispatch_sig —— 黑名单 IP 表载入（htonl/inet_ntoa + addBlackIP）。
// 报告 docs/class_func_reports/Inter_AutoPunishRuleBlackIP/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AutoPunishRuleBlackIPPktView —— Inter_AutoPunishRuleBlackIP::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AutoPunishRuleBlackIPPktView
{
    unsigned short m_0;       // +0x00
};
#pragma pack(pop)

// [K3 2026-09-11] P30/P235「数组成员视角」：ORIG 三条数据访问全是 stride-8 scale 直取
//   （084d42b5 `mov 0x4(%eax,%edx,8),%ebx`、084d42bf `movzbl 0x8(%eax,%edx,8),%eax`、
//     084d42fc `movzbl 0x9(%eax,%edx,8),%eax`）⇒ 原码是「+0x4 起的 8 字节条目数组」成员访问，
//   而非 char* 算术（后者折 add/shl 序列，每次访问多 5 条）。既有聚合器
//   Inter_AutoPunishRuleBlackIPPktView 只含 m_0 且不在本件写域，故本件自带同布局视图。
#pragma pack(push, 1)
struct Inter_AutoPunishRuleBlackIPEntryView
{
    unsigned int m_ip;         // 条目 +0x0 ⇒ msg+i*8+4（084d42b5 直证）
    unsigned char m_startIp;   // 条目 +0x4 ⇒ msg+i*8+8（084d42bf 直证）
    unsigned char m_maxIp;     // 条目 +0x5 ⇒ msg+i*8+9（084d42fc 直证）
    char m_pad6[0x2];
};
struct Inter_AutoPunishRuleBlackIPListView
{
    unsigned short m_totalCount;                        // +0x0
    char m_pad2[0x2];
    Inter_AutoPunishRuleBlackIPEntryView m_entries[1];  // +0x4 起，stride 8
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_128.inc"


#include "InterDispatcher/dtor_Inter_AutoPunishRuleHackType.inc"
// ORIG 0x084d4440 T dispatch_sig —— 从 monitor 消息装载外挂自动惩罚规则表（HackType 规则）。
// 报告 docs/class_func_reports/Inter_AutoPunishRuleHackType/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AutoPunishRuleHackTypePktView —— Inter_AutoPunishRuleHackType::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AutoPunishRuleHackTypePktView
{
    unsigned int m_0;         // +0x00
    unsigned long long m_4;   // +0x04
    unsigned short m_c;       // +0x0c
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_AutoPunishRuleHackTypeEntryWinView —— Inter_AutoPunishRuleHackType::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_AutoPunishRuleHackTypeEntryWinView
{
    char m_pad0[0x10];  // 未消费
    unsigned short m_10;      // +0x10
    unsigned short m_12;      // +0x12
    unsigned short m_14;      // +0x14
    unsigned short m_16;      // +0x16
    unsigned short m_18;      // +0x18
    char m_pad1a[0x2];  // 未消费
    unsigned int m_1c;        // +0x1c
    unsigned int m_20;        // +0x20
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=202（ORIG 0x084d4440）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+68。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=234（ORIG 0x84d4440）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
// P30/P248 R1 视图 disp 折叠族解法（ORIG 0x084d44xx 逐处实证）：
//   ORIG 对条目 5 个 unsigned short 发 `add $0x10,%eax` + `movzwl {0,2,4,6,8}(%eax),%eax`
//   ⇒ 原码里条目 +0x10 处是**子结构基址**（指针算术里带 +0x10，字段偏移按子结构自 0 起算）；
//   对 2 个 int 字段发 `add $0x1c,%eax`/`add $0x20,%eax` + `mov (%eax)` ⇒ 独立的直接指针算术。
//   我方原用扁平 EntryWinView 的绝对偏移（`movzwl 0x18(%eax)` 等），每处少 1 条 add。
//   EntryWinView 定义不在本件写域，故本件自带同布局子结构视图。
struct Inter_AutoPunishRuleHackTypeEntrySubView
{
    unsigned short m_0;            // 条目 +0x10  ORIG `add $0x10,%eax; movzwl (%eax),%eax`
    unsigned short m_2;            // 条目 +0x12  ORIG `add $0x10,%eax; movzwl 0x2(%eax),%eax`
    unsigned short m_4;            // 条目 +0x14  ORIG `add $0x10,%eax; movzwl 0x4(%eax),%eax`
    unsigned short m_6;            // 条目 +0x16  ORIG `add $0x10,%eax; movzwl 0x6(%eax),%eax`
    unsigned short m_8;            // 条目 +0x18  ORIG `add $0x10,%eax; movzwl 0x8(%eax),%eax`
};

// 条目基址 = pPkt + local_20 * 0x14；子结构基址另加 +0x10。必须逐处内联：
// 具名指针局部会在 ORIG 无槽处多出 `mov %eax,<S>` 往返（P0），故用宏保持表达式链。
#define G2_HKT_SUB(field) \
    (reinterpret_cast<Inter_AutoPunishRuleHackTypeEntrySubView*>( \
        reinterpret_cast<char*>(pPkt) + local_20 * 0x14 + 0x10)->field)

#include "InterDispatcher/dispatch_sig_129.inc"


#include "InterDispatcher/dtor_Inter_ReplyPowerWarScore.inc"
// ORIG 0x084d46fa T dispatch_sig —— 力量战争得分全网广播。
// 报告 docs/class_func_reports/Inter_ReplyPowerWarScore/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyPowerWarScorePktView —— Inter_ReplyPowerWarScore::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyPowerWarScorePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_130.inc"


#include "InterDispatcher/dtor_Inter_ReplyJoinPower.inc"
// ORIG 0x084d47d4 T dispatch_sig —— 加入势力应答（0xb4 包 + 公会代理刷新）。
// 报告 docs/class_func_reports/Inter_ReplyJoinPower/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyJoinPowerPktView —— Inter_ReplyJoinPower::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyJoinPowerPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    unsigned int m_12;              // +0x12（P342 异型轴一次尝试：callee int ↔ 字段 unsigned
                                    //   ⇒ ORIG 084d47f4 `mov %eax,%edx` 中转；S1 单形试）
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=124（ORIG 0x084d47d4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=81（ORIG 0x84d47d4）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
#pragma pack(push, 1)
// [G2A-PC 手法1/P248] ORIG 084d4942 对 guildDbInfo+0x1b 发单段 `movzbl 0x1b(%eax),%eax`；
//   我方旧写 `*(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b)` 多发
//   `add $0x1b,%eax` 一条 ⇒ 原码是视图成员访问（pack(1) 常量偏移）。
struct Inter_ReplyJoinPowerGuildDbByteViewI
{
    char m_pad0[0x1b];
    unsigned char m_c;                              // +0x1b (ORIG 084d4942 movzbl 单段 = unsigned char)
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_131.inc"


#include "InterDispatcher/dtor_Inter_SecedePower.inc"
// ORIG 0x084d49d6 T dispatch_sig —— 退出势力应答（0xb5 包 + 公会代理刷新，name 置空）。
// 报告 docs/class_func_reports/Inter_SecedePower/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecedePowerPktView —— Inter_SecedePower::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SecedePowerPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=110（ORIG 0x084d49d6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84d49d6）：本轮 GetGuildDBInfo 单次调用+具名缓存（p167 CLEAR）；余 N22/N18a/N82。
#pragma pack(push, 1)
// [G2A-PC 手法1/P248] ORIG 084d4b3a 对 guildDbInfo+0x1b 发单段 `movzbl 0x1b(%eax),%eax`
//   （结构体成员 = base+disp 形）；我方旧写 `*(reinterpret_cast<const unsigned char*>
//   (guildDbInfo) + 0x1b)` 会多发 `add $0x1b,%eax` 一条 ⇒ 原码是视图成员访问。
//   pack(1) 布局与聚合器无关（本件自带，只覆盖 +0x1b 一字节）。
struct Inter_SecedePowerGuildDbByteViewI
{
    char m_pad0[0x1b];
    unsigned char m_c;                              // +0x1b (P248: ORIG 084d4b3a movzbl 单段 = unsigned char)
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_132.inc"


#include "InterDispatcher/dtor_Inter_NoticePowerWarStartTime.inc"
// ORIG 0x084d4bd2 T dispatch_sig —— 权力战开始时间：设置时间并全员广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarStartTime/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarStartTimePktView —— Inter_NoticePowerWarStartTime::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarStartTimePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    unsigned char m_b;              // +0x0b
    unsigned char m_c;              // +0x0c
    unsigned char m_d;              // +0x0d
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_133.inc"


#include "InterDispatcher/dtor_Inter_NoticePowerWarEndTime.inc"
// ORIG 0x084d4d46 T dispatch_sig —— 力量战争结束时间全网广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarEndTime/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarEndTimePktView —— Inter_NoticePowerWarEndTime::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarEndTimePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    int m_b;                        // +0x0b
    int m_f;                        // +0x0f
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_134.inc"


#include "InterDispatcher/dtor_Inter_NoticePowerWarInfo.inc"
// ORIG 0x084d4e18 T dispatch_sig —— 权力战信息通知全员广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarInfoPktView —— Inter_NoticePowerWarInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
    signed char m_12;               // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_135.inc"


#include "InterDispatcher/dtor_Inter_NoticePowerWarProlong.inc"
// ORIG 0x084d4fce T dispatch_sig —— 力量战争延长公告广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarProlong/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarProlongPktView —— Inter_NoticePowerWarProlong::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarProlongPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_136.inc"


#include "InterDispatcher/dtor_Inter_ChangeGuildName.inc"
// ORIG 0x084d5072 T dispatch_sig —— 公会改名：成功改本地与 proxy 或回执错误码。
// 报告 docs/class_func_reports/Inter_ChangeGuildName/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ChangeGuildNamePktView —— Inter_ChangeGuildName::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ChangeGuildNamePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    char m_pad12[0x4];  // 未消费
    unsigned char m_16;       // +0x16
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_137.inc"


#include "InterDispatcher/dtor_Inter_NoticeChangeGuildNameToMembers.inc"
// ORIG 0x084d51dc T dispatch_sig —— 公会改名成员通知：校验 charac_no 后转发新名并改名。
// 报告 docs/class_func_reports/Inter_NoticeChangeGuildNameToMembers/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeChangeGuildNameToMembersPktView —— Inter_NoticeChangeGuildNameToMembers::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeChangeGuildNameToMembersPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（P211：与 get_charac_no 返回值异型触发 regmove 中转）
    char m_pad12[0x4];  // 未消费
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_138.inc"


#include "InterDispatcher/dtor_Inter_SendAllUserInfoMiniumForGuildSystem.inc"
// ORIG 0x084d533a T dispatch_sig —— 公会系统最小用户信息全员广播。
// 报告 docs/class_func_reports/Inter_SendAllUserInfoMiniumForGuildSystem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendAllUserInfoMiniumForGuildSystemPktView —— Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendAllUserInfoMiniumForGuildSystemPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_139.inc"


#include "InterDispatcher/dtor_Inter_LoadCleanPadPointReq.inc"
#include "InterDispatcher/dispatch_sig_140.inc"


#include "InterDispatcher/dtor_Inter_LoadCleanPadPoint.inc"
#include "InterDispatcher/dispatch_sig_141.inc"
{
    int* point = reinterpret_cast<int*>(data);       // 整包首槽指针视图（非成员访问）
    WongWork::CMCAPManager::setPointPerHackType(point);
    return 0;
}

#include "InterDispatcher/dtor_Inter_NotifyAuctionMail.inc"
// ORIG 0x084d55a4 T dispatch_sig —— 拍卖邮件通知：未读邮件计数 +1 并组 0xa9 包回发。
// 报告 docs/class_func_reports/Inter_NotifyAuctionMail/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NotifyAuctionMailPktView —— Inter_NotifyAuctionMail::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NotifyAuctionMailPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    unsigned char m_12;             // +0x12
    int m_13;                       // +0x13
    unsigned char m_17;             // +0x17
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_142.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultAskPrice.inc"
// ORIG 0x084d5740 T dispatch_sig —— 拍卖询价应答：GM 延迟追踪后组 0xb9 包回发。
// 报告 docs/class_func_reports/Inter_AuctionResultAskPrice/dispatch_sig.md 全文转写。
// ORIG d _ZL6gmList @0x093701d8（延迟追踪 GM 角色号表；定义先移至首个使用点前）
static int gmList[5] =
    { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

#pragma pack(push, 1)
// Inter_AuctionResultAskPricePktView —— Inter_AuctionResultAskPrice::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultAskPricePktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=106（ORIG 0x084d5740）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=70（ORIG 0x84d5740）：P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化 + P0 去具名局部 + P324 语句序（ORIG 0x084d5740）：
//   +0x018 `mov 0x10(%ebp),%eax; mov %eax,-0x18(%ebp)` ⇒ pPkt 赋值在 user==0 检查之后；
//   +0x021 `mov 0x4(%eax),%eax`（单段 32 位）⇒ +0x04 int；+0x109 `mov (%eax),%eax` ⇒ +0x00 int；
//   +0x11d `mov 0x8(%eax),%eax`（32 位送 put_int）⇒ +0x08 int；
//   +0x13e `mov 0xc(%eax,%edx,4),%eax`（edx=循环变量 i 无加法）⇒ 原码 `pPkt->m_c[i]`，无 `const int* msg` 具名指针；
//   +0x172 `mov 0x8(%eax,%edx,4),%eax`（edx=i+4，ORIG +0x16f `add $0x4,%edx`）⇒ 第二循环实访 +0x18..+0x20。
//   聚合器 Inter_AuctionResultAskPricePktView 只声明了 m_0（R1 禁改聚合器），故本件自带同布局视图。
struct Inter_AuctionResultAskPricePktViewI
{
    int m_0;                       // +0x00  ORIG 084d5849 +0x109 `mov (%eax),%eax`
    int m_4;                       // +0x04  ORIG 084d5761 +0x021 `mov 0x4(%eax),%eax`
    int m_8;                       // +0x08  ORIG 084d585d +0x11d `mov 0x8(%eax),%eax`（送 put_int）
    int m_c[3];                    // +0x0c  ORIG 084d587e +0x13e `mov 0xc(%eax,%edx,4),%eax`
    int m_18[3];                   // +0x18  ORIG 084d58b2 +0x172 `mov 0x8(%eax,%edx,4),%eax`（edx=i+4）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_143.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultAskRegistedItemNum.inc"
// ============================================================================
// ORIG 0x084d5930 T dispatch_sig —— 拍卖行"已登记物品数查询应答（登记执行）"。
// 报告 docs/class_func_reports/Inter_AuctionResultAskRegistedItemNum/dispatch_sig.md
// 全文转写（1073 insn，Gold 与 Cera 双通道）。包布局 PCK_AUCTION_REGIST_ITEM_GA
// 依 ORIG ctor @0x84e9a12 / DnfItemInfo::reset @0x84e9094 逐条 store 还原；
// ROI_Category::_sort @0x0822ac30。gmList ORIG d @0x093701d8 实测五值。
// ============================================================================

// ---- ORIG 辅助类型（同源弱符号；Game 侧唯一声明点在本 TU）----
#pragma pack(push, 1)
namespace {

struct RegPckGuid  // 10B：ORIG pck+0x8b..+0x95（=auction itemGloballyUniqueIdentifier_t 10B 版；AuctionItem.h 与 Inven_Item.h 冲突故本地定义）
{
    char data_[10];
};

}  // namespace

struct PCK_AUCTION_REGIST_ITEM_GA : public nsl::PACKET_HEADER
{
    int char_idx;                             // @18
    int owner_id;                             // @22
    char owner_name[13];                      // @26
    unsigned char user_type;                  // @39
    int price;                                // @40
    int instant_price;                        // @44
    DnfItemInfo item_info;                    // @48 (0x35)：权威类型（ORIG ctor call `_ZN11DnfItemInfoC1Ev` T @0x84e910a 实证；DnfItemInfo.h 同布局）
    stAvatarEmblemInfo_t jewel_info;          // @101
    stAvatarExpansionInfo_t expansion;        // @131
    unsigned int black_point;                 // @135
    char guid[10];                            // @139（10B；ORIG dispatch_sig +0x8b..+0x95）
    int unit_price;                           // @149
    ROI_Category _reg_roi_category_key;       // @153（12B，ORIG ROI_Category：m_keys[3]+pad+m_marks[3]）
    char owner_nexon_id[nexon::cash::MAX_NEXON_ID_LENTH];  // @165

    PCK_AUCTION_REGIST_ITEM_GA()
    {
        memset(this, 0, 0xc5);   // ORIG ctor memset(this,0,0xc5)
        setCategory(0);
        setPacketID(3);
        setSize(0xc5);
    }
};
#pragma pack(pop)

// 布局自检（ORIG 实证：guid @0x8b..+0x95、roi @0x99、sizeof 0xc5；字段已按 ORIG 补齐）



#pragma pack(push, 1)
// Inter_AuctionResultAskRegistedItemNumPktView —— Inter_AuctionResultAskRegistedItemNum::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultAskRegistedItemNumPktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
    short m_8;                // +0x08
    char m_pada[0x1];  // 未消费
    unsigned short m_b;       // +0x0b
    unsigned int m_d;         // +0x0d
    int m_11;                 // +0x11
    int m_15;                 // +0x15
    int m_19;                 // +0x19
    int m_1d;                 // +0x1d
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_144.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultRegist.inc"
// ORIG 0x084d6ad6 T dispatch_sig —— 拍卖登记结果应答：GM 延迟追踪 + 0xba 包 + AP 清理。
// 报告 docs/class_func_reports/Inter_AuctionResultRegist/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AuctionResultRegistPktView —— Inter_AuctionResultRegist::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultRegistPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x4];  // 未消费
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=106（ORIG 0x084d6ad6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d6ad6）：N18a 单段 regmove+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化 + P0 去具名局部（ORIG 0x084d6ad6）：
//   +0x021 `mov 0x4(%eax),%eax` 单段 32 位 ⇒ 视图 +0x04 为 int（聚合器该槽是 char[4] 未消费，R1 禁改聚合器，本件自带同布局视图）；
//   +0x0f6/+0x10f/+0x16c `movzbl 0x8(%eax),%eax` ⇒ +0x08 unsigned char；+0x11a `movzbl 0x9(%eax),%eax` ⇒ +0x09 unsigned char；
//   +0x0fa `movzbl %al,%eax`（我方原 `mov %al,<S>; movzbl <S>,%eax` 两跳）= 原码无 `const unsigned char` 具名中转。
struct Inter_AuctionResultRegistPktViewI
{
    int m_0;                 // +0x00  ORIG 084d6c09 +0x133 `mov (%eax),%eax`
    int m_4;                 // +0x04  ORIG 084d6af7 +0x021 `mov 0x4(%eax),%eax`
    unsigned char m_8;       // +0x08  ORIG 084d6bcc +0x0f6 `movzbl 0x8(%eax),%eax`
    unsigned char m_9;       // +0x09  ORIG 084d6bf0 +0x11a `movzbl 0x9(%eax),%eax`
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_145.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultBidding.inc"
#include "InterDispatcher/dispatch_sig_146.inc"
{
    if (user == 0)
        return 0;
    return dispatch_sig_taiwan(user, data, len);
}

// ORIG 0x084d6cda T _ZN26Inter_AuctionResultBidding19dispatch_sig_taiwanEP5CUserPci
// 报告 docs/class_func_reports/Inter_AuctionResultBidding/dispatch_sig_taiwan.md：
// 角色号校验→GM 延迟追踪→组 0xbc 包→按 pay 结果写确认/退款/金返还→
// IPG cancel/confirm 双段计费→发包→IPGQuery。
#pragma pack(push, 1)
// Inter_AuctionResultBiddingPktView —— Inter_AuctionResultBidding::dispatch_sig_taiwan 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultBiddingPktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04（角色号；ORIG 0x084d6ceb `mov 0x4(%eax),%eax` 直读）
    int m_8;                  // +0x08（退款金额 msg[2]）
    unsigned char m_c;        // +0x0c
    unsigned char m_d;        // +0x0d
};
#pragma pack(pop)

// PrePayInfoView —— FindPrePayInfo 返回值的本 TU 消费视图（CPrePayInfoMgr.h 的
//   PrePayInfo 为 char[0x28] 字节区）。ORIG 0x084d6f9a..0x084d6fe4（三组）:
//   `mov -0x28(%ebp),%eax; mov 0x10(%eax),%edx; mov %edx,-0x54(%ebp)…` 逐段 disp
//   直读 ⇒ 源码须为成员访问（cast+算术会让 4.4 产 add 型）；两组 12B 整块
//   （BillTransaction 4/4/4 展开）+ 尾随 0x8/0xc 两 int。
struct PrePayInfoView
{
    char m_pad0[0x8];                  // +0x00..未消费
    unsigned int m_amount1;            // +0x08（ORIG 0x084d6fe4 段）
    unsigned int m_amount2;            // +0x0c
    Taiwan::BillTransaction m_tran1;   // +0x10（12B，ORIG 0x084d6fcd 段）
    Taiwan::BillTransaction m_tran2;   // +0x1c（12B，ORIG 0x084d6ff2 段）
};

#include "InterDispatcher/dispatch_sig_taiwan.inc"

#include "InterDispatcher/dtor_Inter_AuctionResultRegistCancel.inc"
// ORIG 0x084d741c T dispatch_sig —— 拍卖登记取消结果：GM 延迟追踪后组 0xbb 包回发。
// 报告 docs/class_func_reports/Inter_AuctionResultRegistCancel/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AuctionResultRegistCancelPktView —— Inter_AuctionResultRegistCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultRegistCancelPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x4];  // 未消费
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=95（ORIG 0x084d741c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=64（ORIG 0x84d741c）：N18a 单段 regmove+P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P248 同型化 + P0 去具名局部 + P324 语句序（ORIG 0x084d741c）：
//   +0x018 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` ⇒ pPkt 赋值在 user==0 检查之后；
//   +0x021 `mov 0x4(%eax),%eax`（单段 32 位）⇒ +0x04 int；+0x0f6 `movzbl 0x8(%eax),%eax` ⇒ +0x08 unsigned char；
//   +0x0fa `movzbl %al,%eax`（我方原 `mov %al,<S>; movzbl <S>,%eax` 两跳）⇒ 原码无 `const unsigned char` 具名中转。
//   聚合器 Inter_AuctionResultRegistCancelPktView 的 +0x04 是 char[4] 未消费（R1 禁改聚合器），故本件自带同布局视图。
struct Inter_AuctionResultRegistCancelPktViewI
{
    int m_0;                 // +0x00
    int m_4;                 // +0x04  ORIG 084d743d +0x021 `mov 0x4(%eax),%eax`
    unsigned char m_8;       // +0x08  ORIG 084d74?? `movzbl 0x8(%eax),%eax`
    unsigned char m_9;       // +0x09
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_147.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultItemList.inc"
#include "InterDispatcher/dispatch_sig_148.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultMyRegistedItems.inc"
#include "InterDispatcher/dispatch_sig_149.inc"


#include "InterDispatcher/dtor_Inter_AuctionResultMyBidding.inc"
#include "InterDispatcher/dispatch_sig_150.inc"


#include "InterDispatcher/dtor_Inter_AuctionLogMessage.inc"
#include "InterDispatcher/dispatch_sig_151.inc"


#include "InterDispatcher/dtor_Inter_AuctionNotifyAuctionService.inc"
#include "InterDispatcher/dispatch_sig_152.inc"


#include "InterDispatcher/dtor_Inter_PointAuctionLogMessage.inc"
#include "InterDispatcher/dispatch_sig_153.inc"


#include "InterDispatcher/dtor_Inter_PointAuctionChargeCeraPoint.inc"
#include "InterDispatcher/dispatch_sig_154.inc"
{
    return 0;
}

#include "InterDispatcher/dtor_Inter_AuctionResultBuyItemApiece.inc"
// ORIG 0x084d7c8e T dispatch_sig —— 拍卖逐件购买结果（金币返还 + 0x14f 包）。
// 报告 docs/class_func_reports/Inter_AuctionResultBuyItemApiece/dispatch_sig.md 全文转写。
// gmList（ORIG _ZL6gmList d @0x093701d8）定义已存在于
// InterDispatcher.cpp:6527（五值 GM characNo 延迟追踪表），本函数直接引用。
#pragma pack(push, 1)
// Inter_AuctionResultBuyItemApiecePktView —— Inter_AuctionResultBuyItemApiece::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultBuyItemApiecePktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_155.inc"


#include "InterDispatcher/dtor_Inter_LoadGameOption.inc"
#include "InterDispatcher/dispatch_sig_156.inc"
{
    if (user->get_state() <= 1)                        // cmp $1; setle
        return 0;

    // ORIG 0x084d7ea6 起 4 处 `mov -0x10(%ebp),%eax` 先于参数 `movl $…,(,4|8)(%esp)`
    // —— this 装载提前 = 「struct 成员指针作调用对象」形（probe this1 e25：裸指针局部
    // 在 4.4/4.1 均产常量先、load 后形；struct 成员形产 ORIG 序）。
    struct { CETCOption* etcOpt; } etcWrap;
    etcWrap.etcOpt = reinterpret_cast<CETCOption*>(data);
    etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)1, 1);

    CGameOption* opt = user->GetGameOptionRef();
    opt->SetEtcOption(*etcWrap.etcOpt);
    opt->SetQuickChatOption(
        *reinterpret_cast<CQuickChattingOption*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x48), 0);   // ORIG 经 -0x10 槽
    opt->SetQuickChatOption(
        *reinterpret_cast<CQuickChattingOption*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x174), 1);  // ORIG 经 -0x10 槽
    opt->SetEmoticonInfo(
        *reinterpret_cast<CChattingEmoticonConfig*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x2a0));     // ORIG 经 -0x10 槽

    PacketGuard packet;
    packet.put_header(0, 0xad);
    packet.put_int(0x48);
    if (G_GameWorld()->IsSchoolPvPChannel())
    {
        etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)0xa, 1);
        etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)0xb, 0);
    }
    // ORIG 0x084d7f4x：基址取 etcOpt 槽 -0x10(%ebp)（非 data 形参 0x10）。
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt), 0x48);
    packet.put_int(300);
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt) + 0x48, 300);
    packet.put_int(300);
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt) + 0x174, 300);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

#include "InterDispatcher/dtor_Inter_PvPChannelInfo.inc"
// ORIG 0x084d8096 T dispatch_sig —— PvP 频道信息同步：组 0xc6 包逐频道回发。
// 报告 docs/class_func_reports/Inter_PvPChannelInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PvPChannelInfoPktView —— Inter_PvPChannelInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_PvPChannelInfoPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x6];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    char m_pad16[0x1];  // 未消费
    unsigned char m_17;       // +0x17
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_157.inc"


#include "InterDispatcher/dtor_Inter_PvPChannelUserCount.inc"
// ORIG 0x084d8262 T dispatch_sig —— PvP 频道用户数统计回填并转发监控服。
// 报告 docs/class_func_reports/Inter_PvPChannelUserCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PvPChannelUserCountPktView —— Inter_PvPChannelUserCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PvPChannelUserCountPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned int m_12;              // +0x12
    unsigned int m_16;              // +0x16
    unsigned int m_1a;              // +0x1a
    unsigned int m_1e;              // +0x1e
    unsigned int m_22;              // +0x22
    unsigned int m_26;              // +0x26
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_158.inc"


#include "InterDispatcher/dtor_Inter_LoadBloodDungeonBestRecord.inc"
// ORIG 0x084d82f6 T dispatch_sig —— 血轮副本个人最佳记录 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadBloodDungeonBestRecord/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadBloodDungeonBestRecordPktView —— Inter_LoadBloodDungeonBestRecord::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadBloodDungeonBestRecordPktView
{
    unsigned int m_0;         // +0x00
    unsigned int m_4;         // +0x04
    int m_8;                  // +0x08
    int m_c;                  // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_159.inc"


#include "InterDispatcher/dtor_Inter_LoadBreakAwayPreventSystem.inc"
#include "InterDispatcher/dispatch_sig_160.inc"


#include "InterDispatcher/dtor_Inter_LoadBreakAwayPreventMessage.inc"
// ORIG 0x084d92b4 T dispatch_sig —— 防流失活动状态通知（0x111 状态门 + 0x136 活动开启包）。
// 报告 docs/class_func_reports/Inter_LoadBreakAwayPreventMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadBreakAwayPreventMessagePktView —— Inter_LoadBreakAwayPreventMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadBreakAwayPreventMessagePktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_161.inc"


#include "InterDispatcher/dtor_Inter_LoadDimensionInout.inc"
// ORIG 0x084d83d2 T dispatch_sig —— 维度进出标志载入（更新角色数组并回传差异）。
// 报告 docs/class_func_reports/Inter_LoadDimensionInout/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDimensionInoutPktView —— Inter_LoadDimensionInout::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadDimensionInoutPktView
{
    unsigned int m_0;         // +0x00
    char m_pad4[0x8];  // 未消费
    unsigned char m_c;        // +0x0c
};
#pragma pack(pop)

// [K3 2026-09-11] P235「真实成员数组视角」：ORIG 目标侧三条写全是**真实成员/成员数组**直取
//   （`mov %cl,0xeb9(%eax,%edx,1)`、`movb $0x0,0xeb8(%eax)`、`movb $0x0,0xebf(%eax)`），
//   源侧为 +0x4 起的 stride-1 字节数组（`movzbl 0x4(%ebx,%ecx,1),%ecx`）⇒ 改用 _Charac_info
//   真实成员名与数组成员访问，替代 REC_BYTE 宏的指针算术（后者折 add/lea，每次访问多 2-3 条）。
//   既有 Inter_LoadDimensionInoutPktView.m_pad4 为 signed char（会发 movsbl），故本件自带同布局视图。
#pragma pack(push, 1)
struct Inter_LoadDimensionInoutSrcWinView
{
    unsigned int m_0;                 // +0x00
    unsigned char m_demension[0x8];   // +0x04（消费 [0..5]；ORIG stride-1 索引直证）
    unsigned char m_c;                // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_162.inc"


#include "InterDispatcher/dtor_Inter_BlackIPMonitorReq.inc"
#include "InterDispatcher/dispatch_sig_163.inc"
{
    DB_BlackIPMonitor::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__, 0x3f28, 0)("Get BlackIPMonitor Request");
    return 0;
}

#include "InterDispatcher/dtor_Inter_BlackIPMonitor.inc"
// ORIG 0x084d8650 T dispatch_sig —— 黑名单 IP 惩罚表全量装载。
// 报告 docs/class_func_reports/Inter_BlackIPMonitor/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_BlackIPMonitorPktView —— Inter_BlackIPMonitor::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_BlackIPMonitorPktView
{
    int m_0;                  // +0x00
    char m_pad4[0xc80];  // 未消费
    char m_c84;               // +0xc84
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_BlackIPMonitorRecWinView —— Inter_BlackIPMonitor::dispatch_sig 窗口 rec 元素视图
//   rec 基址 = data + (i*2+1)*0x10（ORIG 0x84d8693 寻址链 `add %edx,%edx; add $0x1,%edx;
//   shl $0x4,%edx; add %edx,%eax; add $0xK,%eax` 直证；记录 stride 0x20，本视图取后半单元，
//   故字段偏移 = 记录偏移 - 0x10）。字段宽度=ORIG 访问指令。
struct Inter_BlackIPMonitorRecWinView
{
    char m_pad0[0x4];  // 未消费（记录 +0x10..+0x13）
    unsigned int m_4;         // +0x04 ← 记录 +0x14
    unsigned int m_8;         // +0x08 ← 记录 +0x18
    long m_c;                 // +0x0c ← 记录 +0x1c
    long m_10;                // +0x10 ← 记录 +0x20
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_164.inc"


#include "InterDispatcher/dtor_Inter_BlackIPMonitorPartLoadReq.inc"
#include "InterDispatcher/dispatch_sig_165.inc"
{
    DB_BlackIPMonitorPartLoad::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__,
        0x3f63, 0)("Get BlackIPMonitor Request Part Load");
    return 0;
}

#include "InterDispatcher/dtor_Inter_BlackIPMonitorDeleteIP.inc"
// ORIG 0x084d87e8 T dispatch_sig —— 黑名单 IP 表批量删除。
// 报告 docs/class_func_reports/Inter_BlackIPMonitorDeleteIP/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_BlackIPMonitorDeleteIPPktView —— Inter_BlackIPMonitorDeleteIP::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_BlackIPMonitorDeleteIPPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_BlackIPMonitorDeleteIPRecWinView —— Inter_BlackIPMonitorDeleteIP::dispatch_sig 窗口 rec 元素视图
//   （rec 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_BlackIPMonitorDeleteIPRecWinView
{
    char m_pad0[0x1e];  // 未消费
    unsigned int m_1e;        // +0x1e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=84（ORIG 0x084d87e8）：混合残差；gap+18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=90（ORIG 0x84d87e8）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→-2（87 vs 89，TU 121/1331/5/133 不回退）：真修三处——
//   ① 循环变量 `unsigned int i = 0;` 前置预清 + `for (i=0;...)` 双初始化（复现 ORIG `movl $0,-0xc`×2，P191）；
//   ② 两个 cMyTrace 改全表达式临时链（P221/P227，m_a 在 ctor 前落 %ebx）；
//   ③ 保留 RecWinView 具名视图（P94/P235）。残余=ORIG 常量拆分 `add $0x10; mov 0xe(%eax)` vs 4.4 单 disp（P30/N4）。
// P30/P248（ORIG 0x084d8??）：
//   ORIG 每处访问发 `mov <pPkt槽>,%ecx` + `add <pPkt>,%eax`（%eax 已是 i*0x14）、
//   `lea 0xe(%eax),%edx` / `add $0x10,%eax; mov 0xe(%eax),%ecx`
//   ⇒ 原码基址用**具名 pPkt**（非裸 data 形参），且条目 +0x10 处是子结构（字段 +0x0e 相对子结构）。
//   我方原写 `data + i * 0x14 + 0x1e`（扁平，`add $0xe; add data` 顺序相反，且多用形参而非 pPkt）。
//   RecWinView 定义不在本件写域，故本件自带同布局子结构视图。
// [H2·2026-09-11 laneD 语义轴修] 本 struct 原落在上一条 `#pragma pack(pop)` **之后** ⇒
//   默认对齐把 m_val 由子结构 +0x0e 推到 +0x10；函数体按 +0x10 基址 cast ⇒ 实读条目 +0x20，
//   而 ORIG 084d8859 `lea (%ecx,%eax,1),%eax; add $0x10,%eax; mov 0xe(%eax),%ecx` 读 **+0x1e**
//   （真·错读 2 字节）⇒ 补 pack(1)（局部，仅包住该 struct）。
#pragma pack(push, 1)
struct Inter_BlackIPMonitorDeleteIPRecLeafView
{
    char m_pad0[0xe];              // 子结构 +0x00..+0x0d
    unsigned int m_val;            // 子结构 +0x0e == 条目 +0x1e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_166.inc"


#include "InterDispatcher/dtor_Inter_LoadAutoMarketConditionsControl.inc"
#include "InterDispatcher/dispatch_sig_167.inc"


#include "InterDispatcher/dtor_Inter_UpdateOneDayLethe.inc"
// ORIG 0x084d894c T dispatch_sig —— 一日忘却（OneDayLethe）状态更新：按
// SIG+0x4 的状态机字节切换 USE/SKILL_CONFIRM/INIT/NOT_USE 四分支。
// 报告 docs/class_func_reports/Inter_UpdateOneDayLethe/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdateOneDayLethePktView —— Inter_UpdateOneDayLethe::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_UpdateOneDayLethePktView
{
    int m_0;                  // +0x00
    unsigned char m_4;        // +0x04
    char m_pad5[0x198];  // 未消费
    unsigned char m_19d;      // +0x19d（ORIG movzbl 装载+moval 存储：无符号字节）
    signed char m_19e;        // +0x19e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_168.inc"


#include "InterDispatcher/dtor_Inter_DisconnectMonitorServer.inc"
#pragma pack(push, 1)
// Inter_DisconnectMonitorServerPktView —— Inter_DisconnectMonitorServer::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DisconnectMonitorServerPktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_169.inc"


#include "InterDispatcher/dtor_Inter_ResposncePCRoomPlyerList.inc"
// ORIG 0x084d9cbe T dispatch_sig —— 网吧在线玩家列表下发：角色号校验 + 0x25 字节
// stride 条目（名字/短值/属性字节）打包。
// 报告 docs/class_func_reports/Inter_ResposncePCRoomPlyerList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ResposncePCRoomPlyerListPktView —— Inter_ResposncePCRoomPlyerList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ResposncePCRoomPlyerListPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_ResposncePCRoomPlyerListEntryWinView —— Inter_ResposncePCRoomPlyerList::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_ResposncePCRoomPlyerListEntryWinView
{
    char m_pad0[0x25];  // 未消费
    unsigned char m_25;       // +0x25
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_170.inc"


#include "InterDispatcher/dtor_Inter_ResposncePCRoomPlyerCount.inc"
// ORIG 0x084da0aa T dispatch_sig —— 网吧玩家数应答转发。
// 报告 docs/class_func_reports/Inter_ResposncePCRoomPlyerCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ResposncePCRoomPlyerCountPktView —— Inter_ResposncePCRoomPlyerCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ResposncePCRoomPlyerCountPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_171.inc"


#include "InterDispatcher/dtor_Inter_NoticePCRoomPlayerLoginOut.inc"
// ORIG 0x084da1fa T dispatch_sig —— PC 房玩家登出通知：校验后组 0xba 包回发。
// 报告 docs/class_func_reports/Inter_NoticePCRoomPlayerLoginOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePCRoomPlayerLoginOutPktView —— Inter_NoticePCRoomPlayerLoginOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticePCRoomPlayerLoginOutPktView
{
    char m_pad0[0xa];  // 未消费
    signed char m_a;          // +0x0a
    char m_padb[0x4];  // 未消费
    unsigned int m_f;         // +0x0f（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_13;       // +0x13
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_172.inc"


#include "InterDispatcher/dtor_Inter_NoticePowerWarRank.inc"
// ORIG 0x084d4f46 T dispatch_sig —— 力量战争前三名排行加载。
// 报告 docs/class_func_reports/Inter_NoticePowerWarRank/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarRankPktView —— Inter_NoticePowerWarRank::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarRankPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_173.inc"


#include "InterDispatcher/dtor_Inter_LoadPowerWarStatueRanker.inc"
#pragma pack(push, 1)
// Inter_LoadPowerWarStatueRankerPktView —— Inter_LoadPowerWarStatueRanker::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadPowerWarStatueRankerPktView
{
    // ORIG +0x0f/+0x11 两步装载且求值先于 this：signedness 失配的数组 decay 实参
    // （`int[N]` → `unsigned int*` 形参，偏移 0）形态，探针 g1 实证。
    int m_charNos[1];       // +0x00 排行角色号表
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_174.inc"


#include "InterDispatcher/dtor_Inter_LoadPowerWarStatueInfo.inc"
#include "InterDispatcher/dispatch_sig_175.inc"


#include "InterDispatcher/dtor_Inter_UpdateServerMessage.inc"
// ORIG 0x084da3e6 T dispatch_sig —— 全服公告转发：type 0 → 0xc0 包全频道广播；
// type 1 → Packet_Monitor_Server_Message_Info 上报 monitor 服务器。
// 报告 docs/class_func_reports/Inter_UpdateServerMessage/dispatch_sig.md 全文转写。
// msg 布局：+4 sourceChannel(int) +8 kind(u8) +9 index(u8) +0xc msgLen(int) +0x10 msg
//           +0x44 nameLen(int) +0x48 name +0x5c time(int)。
#pragma pack(push, 1)
// Inter_UpdateServerMessagePktView —— Inter_UpdateServerMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_UpdateServerMessagePktView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
    char m_pada[0x2];  // 未消费
    int m_msgLen;             // +0x0c
    char m_10[1];             // +0x10 串/字节区首元素（容量未建模）
    char m_pad11[0x33];  // 未消费
    int m_nameLen;            // +0x44
    char m_48[1];             // +0x48 串/字节区首元素（容量未建模）
    char m_pad49[0x13];  // 未消费
    int m_time;               // +0x5c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_176.inc"


#include "InterDispatcher/dtor_Inter_LoadServerMessage.inc"
#include "InterDispatcher/dispatch_sig_177.inc"


#include "InterDispatcher/dtor_Inter_NoticeServerMessageInfo.inc"
// ORIG 0x084da726 T dispatch_sig —— 共享服务器公告下发（0xc0 包，全网 state3 广播）。
// 报告 docs/class_func_reports/Inter_NoticeServerMessageInfo/dispatch_sig.md 全文转写。
// [N 判词·2026-09-11 复核] 残 2 块 = **N104 EH 尾块相对序**：ORIG 正常路径以
//   `jmp <tail>` 结束、尾块 = [unwind 段] + [`lea -0x3c;call D1` + `mov $0,%eax` + epilogue]；
//   我方把 [D1+epilogue] 就地内联在正常路径、unwind 压函数末。指令逐条全等仅块位互换 ⇒ N。
#pragma pack(push, 1)
// Inter_NoticeServerMessageInfoPktView —— Inter_NoticeServerMessageInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeServerMessageInfoPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a
    unsigned char m_b;        // +0x0b
    unsigned char m_c;        // +0x0c
    int m_d;                  // +0x0d
    char m_11[1];             // +0x11 串/字节区首元素（容量未建模）
    char m_pad12[0x31];  // 未消费
    unsigned int m_43;        // +0x43
    char m_47[1];             // +0x47 串/字节区首元素（容量未建模）
    char m_pad48[0x13];  // 未消费
    int m_5b;                 // +0x5b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_178.inc"


#include "InterDispatcher/dtor_Inter_NoticeGuildMemo.inc"
// ORIG 0x084dad96 T dispatch_sig —— 公会 memo 通知双字符串转发。
// 报告 docs/class_func_reports/Inter_NoticeGuildMemo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildMemoPktView —— Inter_NoticeGuildMemo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeGuildMemoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    char m_30[1];             // +0x30 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_179.inc"


#include "InterDispatcher/dtor_Inter_PowerWarProcessInfo.inc"
// ORIG 0x084daf30 T dispatch_sig —— 权力战进行信息下发。
// 报告 docs/class_func_reports/Inter_PowerWarProcessInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PowerWarProcessInfoPktView —— Inter_PowerWarProcessInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PowerWarProcessInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
    int m_16;                       // +0x16
    int m_1a;                       // +0x1a
    int m_1e;                       // +0x1e
    int m_22;                       // +0x22
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_180.inc"


#include "InterDispatcher/dtor_Inter_PowerWarEndInfo.inc"
// ORIG 0x084db070 T dispatch_sig —— 权力战结束信息：胜方判定后转发战果统计。
// 报告 docs/class_func_reports/Inter_PowerWarEndInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PowerWarEndInfoPktView —— Inter_PowerWarEndInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PowerWarEndInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
    int m_13;                       // +0x13
    int m_17;                       // +0x17
    int m_1b;                       // +0x1b
    int m_1f;                       // +0x1f
    int m_23;                       // +0x23
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_181.inc"


#include "InterDispatcher/dtor_Inter_NoticeCreateGuildAgit.inc"
// ORIG 0x084da98e T dispatch_sig —— 公会聚点创建通知：置标记后按匹配关系回执/同步 proxy。
// 报告 docs/class_func_reports/Inter_NoticeCreateGuildAgit/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeCreateGuildAgitPktView —— Inter_NoticeCreateGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeCreateGuildAgitPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_182.inc"


#include "InterDispatcher/dtor_Inter_NoticeDeleteGuildAgit.inc"
// ORIG 0x084dab18 T dispatch_sig —— 公会驻地删除通知：清标记并发 0xe8/0xbe 包。
// 报告 docs/class_func_reports/Inter_NoticeDeleteGuildAgit/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeDeleteGuildAgitPktView —— Inter_NoticeDeleteGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeDeleteGuildAgitPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_183.inc"


#include "InterDispatcher/dtor_Inter_NoticeLoadGuildAgit.inc"
// ORIG 0x084dacda T dispatch_sig —— 公会行会据点加载通知。
// 报告 docs/class_func_reports/Inter_NoticeLoadGuildAgit/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeLoadGuildAgitPktView —— Inter_NoticeLoadGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeLoadGuildAgitPktView
{
    char m_pad0[0x16];  // +0x0..未消费
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_184.inc"


#include "InterDispatcher/dtor_Inter_LoadExpertJob.inc"
#include "InterDispatcher/dispatch_sig_185.inc"


#include "InterDispatcher/dtor_Inter_MonitorPunishUserReq.inc"
// ORIG 0x084db294 T dispatch_sig —— 监控踢人：按账号表逐一断线。
// 报告 docs/class_func_reports/Inter_MonitorPunishUserReq/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorPunishUserReqPktView —— Inter_MonitorPunishUserReq::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorPunishUserReqPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    signed char m_a;                // +0x0a
    unsigned short m_b;             // +0x0b
    unsigned int m_accIds[1];       // +0x0d（ORIG 084db31c `mov 0xd(%eax,%edx,4)` scaled 直证）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_186.inc"


#include "InterDispatcher/dtor_Inter_PcroomResponse.inc"
#include "InterDispatcher/IsShutdonTimeOverLogin.inc"
{
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTm;
    struct tm* pTm = localtime_r(&curSec, &localTm);
    if (pTm->tm_hour < 6)
        return true;
    return false;
}
// ORIG 0x084db452 T dispatch_sig —— PC 房响应：鉴权/封禁/公告与 PC 房计费链。
// 报告 docs/class_func_reports/Inter_PcroomResponse/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PcroomResponsePktView —— Inter_PcroomResponse::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_PcroomResponsePktView
{
    char m_pad0[0x12];  // 未消费
    unsigned int m_12;        // +0x12
    long m_16;                // +0x16（ORIG 084db82c `mov 0x16(%eax),%eax` 4 字节读；
                              //   原 char 槽为转写宽度误，已按 handleSetUserPCRoom 形参 long 修正）
    // ORIG 0x84db571/0x84db597/0x84db5ac/0x84db5d2 全 `movzbl` 直证 ⇒ 四字段均为 unsigned char
    // （旧 signed char 生 movsbl，P11/T1 宽度扩展判据）。
    unsigned char m_1a;       // +0x1a
    unsigned char m_1b;       // +0x1b
    unsigned char m_1c;       // +0x1c
    unsigned char m_1d;       // +0x1d
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_187.inc"


#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Sell_Start.inc"
// ORIG 0x084db878 T dispatch_sig —— 限购版道具开卖：清旧登记（广播移除包）、逐条目
// 注册、广播信息包。msg 布局：+0xa flag(u8) +0xb count(u32) +0xf 起条目 stride 0x48。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Sell_Start/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_Sell_StartPktView —— Inter_ItemLimitEdition_Sell_Start::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ItemLimitEdition_Sell_StartPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_flag;     // +0x0a
    unsigned int m_b;         // +0x0b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_188.inc"


#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Sell_End.inc"
// ORIG 0x084dbbb2 T dispatch_sig —— 限量版道具销售结束：逐项移除并广播 IPG 列表。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Sell_End/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_Sell_EndPktView —— Inter_ItemLimitEdition_Sell_End::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ItemLimitEdition_Sell_EndPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=90（ORIG 0x084dbbb2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=103（ORIG 0x84dbbb2）：P30/N4 add-vs-lea。
#pragma pack(push, 1)
// P30 索引寻址同型化（ORIG 0x084dbbc2）：
//   ORIG 对数据区发 `mov <S>,%edx; add $0x4,%edx; mov 0x2(%eax,%edx,4),%ebx`
//   ⇒ 视图 +0x02 处是 int 数组（pack(1) 非对齐），每处访问的原码形为 `pPkt->m_2[i + 4]`（%edx=i 保留 `add $0x4`）；
//   我方原写 `data + 0x12 + i * 4` 走指针算术，每处多 `shl $0x2/add $0x12/add data` 三条（地址相同）。
//   +0x0e 计数槽（ORIG `mov 0xe(%eax),%eax`）即 m_2[3]（2 + 3*4 = 0x0e，常量下标折叠）。
//   聚合器 Inter_ItemLimitEdition_Sell_EndPktView 未声明该数组（R1 禁改聚合器），故本件自带同布局视图。
struct Inter_ItemLimitEdition_Sell_EndPktViewI
{
    unsigned char m_0;             // +0x00
    unsigned char m_1;             // +0x01
    int m_2[8];                    // +0x02..+0x21 数据数组；m_2[i + 4] 覆盖 +0x12 起，m_2[3] = +0x0e 计数
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_189.inc"


#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Update.inc"
// ORIG 0x084dbd16 T dispatch_sig —— 限售物品数量更新：逐条 updateItem 后广播 0xe1 包。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Update/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_UpdatePktView —— Inter_ItemLimitEdition_Update::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ItemLimitEdition_UpdatePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_190.inc"


#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Reply.inc"
// ORIG 0x084dbef0 T —— 限时道具购买应答。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Reply/dispatch_sig.md 全文转写
// （## 汇编 + ## 反编译 C）。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_ReplyPktView —— Inter_ItemLimitEdition_Reply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ItemLimitEdition_ReplyPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned int m_12;        // +0x12
    unsigned char m_16;       // +0x16
    unsigned char m_17;       // +0x17
    unsigned char m_18;       // +0x18
    char m_pad19[0xaf];  // 未消费
    unsigned char m_c8;       // +0xc8
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_191.inc"


#include "InterDispatcher/dtor_Inter_FindFactoryHub.inc"
// ORIG 0x084dc97a T dispatch_sig —— 工厂枢纽查找：计 hack 键并上报监视服。
// 报告 docs/class_func_reports/Inter_FindFactoryHub/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_FindFactoryHubPktView —— Inter_FindFactoryHub::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_FindFactoryHubPktView
{
    int m_0;                  // +0x00
    unsigned char m_4;        // +0x04
    char m_5[1];              // +0x5 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_192.inc"


#include "InterDispatcher/dtor_Inter_GuildExpLimit.inc"
#include "InterDispatcher/dispatch_sig_193.inc"


#include "InterDispatcher/dtor_Inter_SetIPCount.inc"
// ORIG 0x084dcb18 T dispatch_sig —— 同 IP 连接计数表同步。
// 报告 docs/class_func_reports/Inter_SetIPCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetIPCountPktView —— Inter_SetIPCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// 元素内 +0x4 的子结构（相对包体 +0x10）必须具名：ORIG 0x084dcb5e 段对子结构成员发
//   `lea (%ecx,%eax,1); add $0x10,%eax; mov 0xc(%eax)`（子结构基址 + 成员 disp 两步），
//   而元素内标量成员（+0xc/+0xe）走 `movzwl 0xc(%eax,%ecx,1)` 折叠 disp——两者共存
//   证明原码是「具名子结构成员」而非裸偏移（裸偏移 4.4 恒折 add $0x1c + mov (%eax)）。
struct Inter_SetIPCountSub
{
    char m_ip[0xc];             // 相对包体 +0x10（setIPCounterData 第 3 参 char* 原文）
    unsigned int m_counter;     // 相对包体 +0x1c（第 4 参）
};
// m_counter 的等址尾视图：ORIG 第 4 参走「子结构地址 +0x10」→「成员 disp +0xc」两步
//   （0x084dcb58 `lea (%ecx,%eax,1); add $0x10,%eax; mov 0xc(%eax)`），4.4 对直接成员访问
//   恒折单条 `mov 0x1c(%eax,%ecx,1)`。经「m_ip 数组名 + 尾视图成员」访问可复现两步形。
struct Inter_SetIPCountSubTail
{
    char m_pad[0xc];            // 与 m_ip 同址
    unsigned int m_counter;     // 相对包体 +0x1c
};
struct Inter_SetIPCountEntry
{
    unsigned short m_c;         // 相对包体 +0x0c + i*0x14
    unsigned short m_e;         // 相对包体 +0x0e + i*0x14
    Inter_SetIPCountSub m_sub;  // 相对包体 +0x10 + i*0x14
};
struct Inter_SetIPCountPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    unsigned char m_b;              // +0x0b
    Inter_SetIPCountEntry m_entries[1];  // +0x0c 起，元素 stride 0x14
};
static_assert(sizeof(Inter_SetIPCountEntry) == 0x14, "SetIPCountEntry stride");
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_194.inc"


#include "InterDispatcher/dtor_Inter_SetFullIPCount.inc"
// ORIG 0x084dcbea T dispatch_sig —— 全 IP 连接计数表同步。
// 报告 docs/class_func_reports/Inter_SetFullIPCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetFullIPCountPktView —— Inter_SetFullIPCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// 与 SetIPCount 同族：元素 stride 0x18、数组基址 +0xc；子结构（元素内 +0x4 ⇒ 相对包体 +0x10）
//   必须具名（ORIG 0x084dcc2f `add $0x10,%eax; mov 0x10(%eax)` 两步），元素内标量走折叠 disp。
struct Inter_SetFullIPCountSub
{
    char m_ip[0x10];            // 相对包体 +0x10（setFullIPCounterData 第 3 参 char* 原文）
    unsigned int m_counter;     // 相对包体 +0x20（第 4 参）
};
// m_counter 等址尾视图（同上 SetIPCountSubTail 依据）。
struct Inter_SetFullIPCountSubTail
{
    char m_pad[0x10];           // 与 m_ip 同址
    unsigned int m_counter;     // 相对包体 +0x20
};
struct Inter_SetFullIPCountEntry
{
    unsigned short m_c;         // 相对包体 +0x0c + i*0x18
    unsigned short m_e;         // 相对包体 +0x0e + i*0x18
    Inter_SetFullIPCountSub m_sub;  // 相对包体 +0x10 + i*0x18
};
struct Inter_SetFullIPCountPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    unsigned char m_b;              // +0x0b
    Inter_SetFullIPCountEntry m_entries[1];  // +0x0c 起，元素 stride 0x18
};
static_assert(sizeof(Inter_SetFullIPCountEntry) == 0x18, "SetFullIPCountEntry stride");
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_195.inc"


#include "InterDispatcher/dtor_Inter_CancelRestingUser.inc"
#include "InterDispatcher/dispatch_sig_196.inc"
{
    if (user != 0)
    {
        user->setRestingUserRestrict(false);
    }
    return 0;
}

#include "InterDispatcher/dtor_Inter_ApplyTempPowerSide.inc"
// ORIG 0x084dccde T dispatch_sig —— 临时势力侧应用（公会代理修改 + 全服广播 basic_info）。
// 报告 docs/class_func_reports/Inter_ApplyTempPowerSide/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ApplyTempPowerSidePktView —— Inter_ApplyTempPowerSide::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ApplyTempPowerSidePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上）
    char m_pad12[0x4];  // +0x12..未消费
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=131（ORIG 0x084dccde）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84dccde）：本轮 GetGuildDBInfo 单次调用+具名缓存（p167 CLEAR）；余 N22/N18a/P30。
#pragma pack(push, 1)
// [G2A-PC 手法1/P248] ORIG 084dcd33 对 guildDbInfo+0x1b 发单段 `movzbl 0x1b(%eax),%eax`；
//   我方旧写 `*(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b)` 多发
//   `add $0x1b,%eax` 一条 ⇒ 原码是 pack(1) 视图成员访问。
struct Inter_ApplyTempPowerSideGuildDbByteViewI
{
    char m_pad0[0x1b];
    unsigned char m_c;                              // +0x1b (ORIG 084dcd33 movzbl 单段 = unsigned char)
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_197.inc"


#include "InterDispatcher/dtor_Inter_ReloadPowerWarRanker.inc"
#include "InterDispatcher/dispatch_sig_198.inc"
{
    cMyTrace(__PRETTY_FUNCTION__,
        0x482b, 0)("Receive Packet_Request_Reload_Power_War_Ranker");
    GlobalData::s_power_manager->ClearMVPInfo();
    GlobalData::s_power_manager->LoadRankerInfo();
    return 0;
}

#include "InterDispatcher/dtor_Inter_LoadPowerWarInfo.inc"
// ORIG 0x084dcf4a T dispatch_sig —— 力量战争胜负信息加载。
// 报告 docs/class_func_reports/Inter_LoadPowerWarInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadPowerWarInfoPktView —— Inter_LoadPowerWarInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadPowerWarInfoPktView
{
    int m_0;                  // +0x00（P211：与 SetPowerInfo 形参同型 ⇒ 4.4 单段直落 callee-saved）
    int m_4;                  // +0x04
    unsigned char m_8;        // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_199.inc"


#include "InterDispatcher/dtor_Inter_SetFactoryHubUser.inc"
// ORIG 0x084dcfec T dispatch_sig —— 工厂中枢用户登记/查询。
// 报告 docs/class_func_reports/Inter_SetFactoryHubUser/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetFactoryHubUserPktView —— Inter_SetFactoryHubUser::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SetFactoryHubUserPktView
{
    char m_pad0[0xf];  // 未消费
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
    char m_pad10[0x1d];  // 未消费
    unsigned char m_2d;       // +0x2d
    unsigned short m_2e;      // +0x2e
    ENUM_VIEW_FIELD m_30;     // +0x30（P254 枚型：ORIG 0x084dd012 保留 eax→edx 中转）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_200.inc"


#include "InterDispatcher/dtor_Inter_SetFactoryHubUserNotConn.inc"
// ORIG 0x084dd0c4 T dispatch_sig —— 工厂中枢用户未连接记录 DB 保存。
// 报告 docs/class_func_reports/Inter_SetFactoryHubUserNotConn/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetFactoryHubUserNotConnPktView —— Inter_SetFactoryHubUserNotConn::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SetFactoryHubUserNotConnPktView
{
    unsigned int m_0;               // +0x00
    int m_4;                        // +0x04（ORIG +0x3f 两步 spill＝signed 字段→unsigned 形参失配）
    unsigned short m_8;             // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_201.inc"


#include "InterDispatcher/dtor_Inter_Set_CleanPad_Point.inc"
// ORIG 0x084dd12e T dispatch_sig —— CleanPad 点数设置（CMCAPManager）。
// 报告 docs/class_func_reports/Inter_Set_CleanPad_Point/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Set_CleanPad_PointPktView —— Inter_Set_CleanPad_Point::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Set_CleanPad_PointPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned short m_e;             // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_202.inc"


#include "InterDispatcher/dtor_Inter_LoadNoCacheData.inc"
// ORIG 0x084dd1ee T dispatch_sig —— 无缓存角色数据加载（扩展数据 loadData 虚调用）。
// 报告 docs/class_func_reports/Inter_LoadNoCacheData/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadNoCacheDataPktView —— Inter_LoadNoCacheData::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadNoCacheDataPktView
{
    int m_0;                        // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_203.inc"


#include "InterDispatcher/dtor_Inter_GuildCargo.inc"
// ORIG 0x084dd26c T dispatch_sig —— 公会仓库结果下发（状态门→历史日志→错误包/回填计数包）。
// 报告 docs/class_func_reports/Inter_GuildCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoPktView —— Inter_GuildCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令核实）。
// entry 区视图：ORIG 0x084dd3dc `imul $0x35 → lea (base,i) → add $0x10 → mov 0x3(%eax)`
// 与 0x084dd405 `imul → add $0x10 → add base → add $0x2` 的两段拆分布局实证
// （flag 绝对 +0x13、blob 绝对 +0x12，entry stride 0x35）。
struct GuildCargoPayloadV
{
    char m_pad0[2];               // +0x00..+0x01（ORIG blob 起点在 payload+0x02）
    unsigned char m_blob0;        // +0x02（put_binary 起点，绝对 +0x12；
                                  //   ORIG 0x084dd405 `add $0x10 → add base → add $0x2`）
    int m_flag;                   // +0x03（绝对 +0x13，ORIG `mov 0x3(%eax)` 实证）
    unsigned char m_rest[0x1e];   // +0x07..0x24（payload 0x25 = 0x35−0x10）
};
struct GuildCargoEntryV
{
    char m_head[0x10];            // +0x00 entry 头
    GuildCargoPayloadV m_payload; // +0x10
};
static_assert(sizeof(GuildCargoEntryV) == 0x35,
              "GuildCargoEntryV ORIG stride 0x35（imul $0x35 实证）");
struct Inter_GuildCargoPktView
{
    char m_pad0[0xe];  // +0x0..未消费（entry 区经 GuildCargoEntryV cast 视图消费）
    unsigned int m_e;         // +0x0e（同上）
    char m_pad12[0x18d8];  // 未消费
    unsigned int m_18ea;      // +0x18ea
    unsigned char m_18ee;     // +0x18ee
};
#pragma pack(pop)

// [b28c-27] N94/N82附：entry 区两处地址形残差——①`add $0x10; mov 0x3(%eax)`
// 被 4.4 折成 `mov 0x13(%eax)`（0x10 与 int 字段 disp 3 合并；N94 判定 C%8 分支
// 不适用非 scaled 形）；②循环体首值装载序 `mov -0xc,%eax; mov -0x14,%edx` 跨版本
// 对调（N82附）。blob 起点偏移已按 ORIG 0x084dd405 `add $0x10 → add base → add $0x2`
// 修正为 payload+0x02（真语义 bug 修复，原 +0x00 少 2 字节）。
// [b28d-lane2] verdict=DIFF ndiff=62（ORIG 0x084dd26c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=11（ORIG 0x84dd26c）：P30/N4 add-vs-lea。
// [R5·InterDisp 形态收口 2026-09-11] payload 基址（entry + 0x10）独立成步（复现 ORIG 两段地址）：
//   ORIG 0x084dd3dc `imul $0x35 → lea (base,i) → add $0x10,%eax → mov 0x3(%eax),%eax`、
//   0x084dd405 `imul → add $0x10 → add base → add $0x2` ⇒ 成员偏移按 payload 自 0 起算；
//   扁平形（entry + 0x13 / +0x12）被 4.4 折成单条 `mov 0x13(%eax)` / `add $0x12`。
//   逐处内联：具名基址局部会占槽顶深 pPkt（技巧36）。
#define G2_GC_PAYLOAD() \
    (reinterpret_cast<GuildCargoPayloadV*>( \
        reinterpret_cast<char*>(pPkt) + i * 0x35 + 0x10))
#include "InterDispatcher/dispatch_sig_204.inc"

#undef G2_GC_PAYLOAD

#include "InterDispatcher/dtor_Inter_NoticeGuildCargo.inc"
// ORIG 0x084dd4b2 T dispatch_sig —— 公会仓库容量应答：记历史、设容量、组 0xed 包回发。
// 报告 docs/class_func_reports/Inter_NoticeGuildCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildCargoPktView —— Inter_NoticeGuildCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeGuildCargoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    char m_pad12[0x18d8];  // 未消费
    unsigned int m_18ea;      // +0x18ea
};
// 载荷元素 stride 0x35（ORIG 0x084dd4xx 循环 `imul $0x35,%eax,%eax; lea (%edx,%eax,1)`）；
// 元素内 +0x10 起的载荷子结构必须具名，否则 4.4 把 +0x10/+0x3 折成单条 `add $0x13`
//（ORIG 保留 `add $0x10,%eax; mov 0x3(%eax)` 两步 —— 与 SetIPCount 同族判据）。
struct Inter_NoticeGuildCargoBody
{
    char m_bytes[0x25];   // +0x10..+0x34 载荷字节区（put_binary 源 = 本区 +0x2）
};
struct Inter_NoticeGuildCargoEntry
{
    char m_pad10[0x10];              // 元素内 +0x00..+0x0f
    Inter_NoticeGuildCargoBody m_body;   // 元素内 +0x10
};
static_assert(sizeof(Inter_NoticeGuildCargoEntry) == 0x35, "NoticeGuildCargoEntry stride");
// body 内 +0x3 的 4 字节字段等址视图（ORIG `add $0x10,%eax; mov 0x3(%eax),%eax`）
struct Inter_NoticeGuildCargoTail
{
    char m_pad3[3];
    int m_3;
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_205.inc"


#include "InterDispatcher/dtor_Inter_GuildCargoHistory.inc"
// ORIG 0x084dd69e T dispatch_sig —— 公会仓库历史查询：组 0x10a 包回发并记历史。
// 报告 docs/class_func_reports/Inter_GuildCargoHistory/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoHistoryPktView —— Inter_GuildCargoHistory::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildCargoHistoryPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    // [L9-G PATCH#1] P211/P265 档③ 同型化：ORIG 比较点 `mov %eax,%edx; mov -0x14(%ebp),%eax;
    //   mov 0xe(%eax),%eax; cmp %eax,%edx`；探针 /tmp/L9/B/pb_cmp.cpp fu(unsigned) 同形。
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_206.inc"


#include "InterDispatcher/dtor_Inter_GuildCargoCheckPushItem.inc"
#include "InterDispatcher/Packet_Guild_Guild_Cargo_Push_Item.inc"
    : PacketHeader(0x70d, 0x53),
      m_guildKey(0), m_charNo(0), m_b(0), m_d(0), m_itemType(0), m_item()
{
    m_e = 0;
    m_f = 0;
}

#pragma pack(push, 1)
// Inter_GuildCargoCheckPushItemPktView —— Inter_GuildCargoCheckPushItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoCheckPushItemPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    unsigned char m_12;       // +0x12
    int m_13;                 // +0x13
    int m_17;                 // +0x17
    int m_1b;                 // +0x1b
    unsigned int m_1f;        // +0x1f
    unsigned char m_23;       // +0x23（ORIG 0x084ddcc2 `movzbl 0x23(%eax),%eax` 直读）
    unsigned char m_24;       // +0x24
    unsigned short m_25;      // +0x25
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_207.inc"


#include "InterDispatcher/dtor_Inter_GuildCargoPushItem.inc"
// ORIG 0x084ddea2 T dispatch_sig —— 公会仓库上架结果：成功回 0xfb 确认包；
// 失败则把物品从 msg 载荷重建入背包（失败再走系统邮件回收）。
// 报告 docs/class_func_reports/Inter_GuildCargoPushItem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoPushItemPktView —— Inter_GuildCargoPushItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoPushItemPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    unsigned char m_12;       // +0x12
    int m_13;                 // +0x13
    char m_pad17[0x1];  // 未消费
    int m_18;                 // +0x18
    unsigned char m_1c;       // +0x1c
    int m_1d;                 // +0x1d
    unsigned short m_21;      // +0x21
    int m_23;                 // +0x23
    unsigned char m_27;       // +0x27
    unsigned short m_28;      // +0x28
    // [R5·InterDisp 形态收口 2026-09-11] 负结论（已回滚）：ORIG 0x084de058..071 复用 pPkt 基址
    //   逐字段 `mov 0x34/0x38/0x3c(%eax),%edx` + `movzwl 0x40(%eax),%eax`；我方裸偏移形
    //   `data + 0x34` 被 4.4 提升为 `add $0x34,%eax` 后以 +0/+4/+8/+0xc 访问（**绝对地址逐值相同**
    //   ⇒ 门③b 等价，非 bug）。试改视图成员 `RandomOption m_random34`（+0x34，14B 与 ORIG 逐段吻合）
    //   消除提升：实测 insn 310→309 但 ndiff 53→55 **反劣** ⇒ 回滚。
    char m_pad2a[0x18];       // +0x2a..未消费
    UpgradeSeparateInfo m_42; // +0x42 §4.3 字段化（1B class，ORIG 读 UpgradeSeparate）
    char m_pad43[0x9];        // +0x43..未消费
    unsigned char m_4c;       // +0x4c
    unsigned short m_4d;      // +0x4d
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_208.inc"


#include "InterDispatcher/dtor_Inter_GuildCargoPopItem.inc"
#pragma pack(push, 1)
// Inter_GuildCargoPopItemPktView —— Inter_GuildCargoPopItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildCargoPopItemPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
    char m_pad16[0x1];
    int m_17;                       // +0x17
    unsigned char m_1b;             // +0x1b
    int m_1c;                       // +0x1c
    unsigned short m_20;            // +0x20
    int m_22;                       // +0x22
    unsigned char m_26;             // +0x26
    unsigned short m_27;            // +0x27
    char m_pad29[0xa];  // +0x29..未消费
    RandomOption m_33;              // +0x33..0x40（14B；ORIG 0x084de580..0x084de596
                                    // 整块结构赋值，disp 0x33/0x37/0x3b/0x3f 直读）
    unsigned char m_41;             // +0x41
    char m_pad42[0x9];  // +0x42..未消费
    unsigned char m_4b;             // +0x4b
    int m_4c;                       // +0x4c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_209.inc"


#include "InterDispatcher/dtor_Inter_GuildCargoMoveItem.inc"
// ORIG 0x084debb4 T dispatch_sig —— 公会仓库移动结果：记历史日志并回执客户端。
// 报告 docs/class_func_reports/Inter_GuildCargoMoveItem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoMoveItemPktView —— Inter_GuildCargoMoveItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoMoveItemPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（[b28c-31] P211 字段侧旋钮：ORIG 0x084debd7
                              // `call get_charac_no; mov %eax,%edx; … cmp %eax,%edx`
                              // 为异型中转形 ⇒ 字段 unsigned 与 callee int 异型）
    unsigned char m_12;       // +0x12
    int m_13;                 // +0x13
    int m_17;                 // +0x17
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_210.inc"


#include "InterDispatcher/dtor_Inter_GuildCargoUpgrade.inc"
// ORIG 0x084ded42 T dispatch_sig —— 公会仓库升级应答。
// 报告 docs/class_func_reports/Inter_GuildCargoUpgrade/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoUpgradePktView —— Inter_GuildCargoUpgrade::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoUpgradePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned char m_12;       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_211.inc"


#include "InterDispatcher/dtor_Inter_GuildBoardData.inc"
// ORIG 0x084dee6a T dispatch_sig —— 公会留言板数据下发：按请求码 0x232a/0x232e
// 组 0x155/0x156 回包，逐条 0xa5 记录转写。
// 报告 docs/class_func_reports/Inter_GuildBoardData/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildBoardDataPktView —— Inter_GuildBoardData::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildBoardDataPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned short m_a;             // +0x0a
    unsigned short m_c;             // +0x0c
    unsigned char m_e;              // +0x0e
    char m_padf[0x4];  // +0xf..未消费
    unsigned int m_13;              // +0x13（P211/P265 档③：ORIG 比较点 `mov %eax,%edx` 中转）
    signed char m_17;               // +0x17
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_212.inc"


#include "InterDispatcher/dtor_Inter_GuildBoardDelete.inc"
// ORIG 0x084df2ac T dispatch_sig —— 公会留言板删除应答。
// 报告 docs/class_func_reports/Inter_GuildBoardDelete/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildBoardDeletePktView —— Inter_GuildBoardDelete::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildBoardDeletePktView
{
    char m_pad0[0xa];  // 未消费
    short m_a;                // +0x0a
    char m_padc[0x7];  // 未消费
    unsigned int m_13;        // +0x13
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_213.inc"


#include "InterDispatcher/dtor_Inter_TakeScreenShot.inc"
// ORIG 0x084df3d6 T dispatch_sig —— 客户端截屏请求全网广播。
// 报告 docs/class_func_reports/Inter_TakeScreenShot/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TakeScreenShotPktView —— Inter_TakeScreenShot::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TakeScreenShotPktView
{
    char m_pad0[0xb];  // +0x0..未消费
    int m_b;                        // +0x0b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_214.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedStart.inc"
// ORIG 0x084df47a T dispatch_sig —— 村庄袭击开始：关闭极限地下城 + 全服广播 0xf1 包。
// 报告 docs/class_func_reports/Inter_VillageAttackedStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedStartPktView —— Inter_VillageAttackedStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedStartPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_215.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedEnd.inc"
// ORIG 0x084df6a4 T dispatch_sig —— 村庄攻防结束：开/关复仇副本、遍历在线用户
// 发 0x1ec 结束包（疲劳结算、复仇次数落库），最后广播奖励类型。
// 报告 docs/class_func_reports/Inter_VillageAttackedEnd/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedEndPktView —— Inter_VillageAttackedEnd::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedEndPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_216.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedCountdown.inc"
// ORIG 0x084dfae0 T dispatch_sig —— 村庄攻击倒计时广播。
// 报告 docs/class_func_reports/Inter_VillageAttackedCountdown/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedCountdownPktView —— Inter_VillageAttackedCountdown::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedCountdownPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_217.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedUpdate.inc"
// ORIG 0x084dfbe6 T dispatch_sig —— 村庄攻击状态全员更新广播。
// 报告 docs/class_func_reports/Inter_VillageAttackedUpdate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedUpdatePktView —— Inter_VillageAttackedUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_218.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedScore.inc"
// ORIG 0x084dfd16 T dispatch_sig —— 村庄袭击比分：多门校验后转发四方比分。
// 报告 docs/class_func_reports/Inter_VillageAttackedScore/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedScorePktView —— Inter_VillageAttackedScore::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedScorePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    int m_12;                       // +0x12
    int m_16;                       // +0x16
    int m_1a;                       // +0x1a
    int m_1e;                       // +0x1e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_219.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedReward.inc"
// ORIG 0x084dfea6 T dispatch_sig —— 村庄攻防奖励发放：按奖励类型（win point /
// 排名 / 参战）检索 Arad_VillageAttackRewardItem 脚本并发奖。
// 报告 docs/class_func_reports/Inter_VillageAttackedReward/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedRewardPktView —— Inter_VillageAttackedReward::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedRewardPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
    int m_16;                       // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_220.inc"


#include "InterDispatcher/dtor_Inter_VillageAttackedRewardServer.inc"
#include "InterDispatcher/dispatch_sig_221.inc"


#include "InterDispatcher/dtor_Inter_FullLevelBroadCast.inc"
// ORIG 0x084e0816 T dispatch_sig —— 全服广播字符串（满级公告等）。
// 报告 docs/class_func_reports/Inter_FullLevelBroadCast/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_FullLevelBroadCastPktView —— Inter_FullLevelBroadCast::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_FullLevelBroadCastPktView
{
    char m_pad0[0xa];  // 未消费
    char m_a[1];              // +0xa 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_222.inc"


#include "InterDispatcher/dtor_Inter_Hack_CheckMyAccount.inc"
// ORIG 0x084e08fc T dispatch_sig —— 账号自查异常登记 hack 计数。
// 报告 docs/class_func_reports/Inter_Hack_CheckMyAccount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Hack_CheckMyAccountPktView —— Inter_Hack_CheckMyAccount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Hack_CheckMyAccountPktView
{
    int m_0;                        // +0x00
    char m_pad4[0x4];  // +0x4..未消费
    int m_8;                        // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_223.inc"


#include "InterDispatcher/dtor_Inter_SetSecounUserData.inc"
// ORIG 0x084e0972 T dispatch_sig —— 记录黑金没收用户的钱款入账。
// 报告 docs/class_func_reports/Inter_SetSecounUserData/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetSecounUserDataPktView —— Inter_SetSecounUserData::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SetSecounUserDataPktView
{
    unsigned long long m_0;   // +0x00
    unsigned long long m_8;   // +0x08
    unsigned int m_10;        // +0x10
    unsigned int m_14;        // +0x14
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_224.inc"


#include "InterDispatcher/dtor_Inter_SetARSInfo.inc"
// ORIG 0x084e09ce T dispatch_sig —— 网吧自动惩罚规则(ARS)批量下发：reset 门 + 每条目
// setTotalTrade/setHackCnt 分发与 trace，条目 stride 0xc。
// 报告 docs/class_func_reports/Inter_SetARSInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetARSInfoPktView —— Inter_SetARSInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SetARSInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned char m_e;              // +0x0e
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_SetARSInfoEntryWinView —— Inter_SetARSInfo::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_SetARSInfoEntryWinView
{
    char m_pad0[0xf];  // 未消费
    unsigned short m_f;       // +0x0f
    unsigned short m_11;      // +0x11
    unsigned short m_13;      // +0x13
    unsigned short m_15;      // +0x15
    unsigned char m_17;       // +0x17
    unsigned char m_18;       // +0x18
    unsigned short m_19;      // +0x19
};
#pragma pack(pop)

// [K3 2026-09-11] P235「数组成员视角」：ORIG 084e09ce 段每条目发
//   `mov -0x1c(%ebp),%edx; mov -0x20(%ebp),%ecx; mov %edx,%eax; add %eax,%eax; add %edx,%eax;
//    shl $0x2,%eax; movzwl 0xf(%eax,%ecx,1),%eax`
//   —— i*0xc 算进寄存器后以 `disp(%idx,%base,1)` 直取（字段只在 disp 里），
//   而我方 char* 算术式每条目多一条 `lea (%ecx,%eax,1),%eax`（10 处 ⇒ +10 条）。
//   元素布局相对条目基址 +0xf：f/11/13/15/17/18/19 ⇒ 元素 stride 0xc；既有
//   Inter_SetARSInfoEntryWinView 用 pPkt 绝对偏移建模，不能作数组成员，故本件自带元素视图。
#pragma pack(push, 1)
struct Inter_SetARSInfoElemView
{
    unsigned short m_f;    // 元素 +0x00 ⇒ pPkt + i*0xc + 0x0f
    unsigned short m_11;   // +0x02 ⇒ +0x11
    unsigned short m_13;   // +0x04 ⇒ +0x13
    unsigned short m_15;   // +0x06 ⇒ +0x15
    unsigned char m_17;    // +0x08 ⇒ +0x17
    unsigned char m_18;    // +0x09 ⇒ +0x18
    unsigned short m_19;   // +0x0a ⇒ +0x19
};
struct Inter_SetARSInfoListView
{
    char m_pad0[0xa];           // +0x0..+0x9
    int m_a;                    // +0x0a（P290：ORIG 084e09xx `mov 0xa(%eax),%ebx` 单段 ⇒ 与
                                //   使用上下文(int 比较/上界)同型 ⇒ int）
    unsigned char m_e;          // +0x0e
    Inter_SetARSInfoElemView m_entries[1];  // +0x0f 起，stride 0xc
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_225.inc"


#include "InterDispatcher/dtor_Inter_OnTimeEventRewardStart.inc"
// ORIG 0x084e0dc6 T dispatch_sig —— 定时活动奖励开始：0x33 号 repeat 事件广播 0x105 包。
// 报告 docs/class_func_reports/Inter_OnTimeEventRewardStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_OnTimeEventRewardStartPktView —— Inter_OnTimeEventRewardStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_OnTimeEventRewardStartPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned int m_e;         // +0x0e
    // P290：ORIG 084e0e5x 两处实参装载为**两段**（`mov 0x12(%eax),%eax; mov %eax,%edx` /
    //   `mov 0x16(%eax),%eax; mov %eax,%ecx`）⇒ 字段 unsigned、形参 int ⇒ 异型中转；
    //   我方旧 int 同型恒折单段（每处少 1 条，gap +2）。
    unsigned int m_12;        // +0x12
    unsigned int m_16;        // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_226.inc"


#include "InterDispatcher/dtor_Inter_OnTimeEventRewardEnd.inc"
#include "InterDispatcher/dispatch_sig_227.inc"


#include "InterDispatcher/dtor_Inter_LoadPvPBuddy.inc"
#include "InterDispatcher/dispatch_sig_228.inc"
{
    return 0;
}

#include "InterDispatcher/dtor_Inter_PvPBuddyConnList.inc"
// ORIG 0x084e16fe T dispatch_sig —— PvP 伙伴连接列表下发。
// 报告 docs/class_func_reports/Inter_PvPBuddyConnList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PvPBuddyConnListPktView —— Inter_PvPBuddyConnList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PvPBuddyConnListPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_229.inc"


#include "InterDispatcher/dtor_Inter_ReqAddPvPBuddy.inc"
// ORIG 0x084e1832 T dispatch_sig —— PvP 伙伴添加请求应答。
// 报告 docs/class_func_reports/Inter_ReqAddPvPBuddy/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqAddPvPBuddyPktView —— Inter_ReqAddPvPBuddy::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqAddPvPBuddyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12;                // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_230.inc"


#include "InterDispatcher/dtor_Inter_AddPvPBuddyResult.inc"
// ORIG 0x084e1978 T dispatch_sig —— PvP 好友添加结果通知（0x121/0x122 两包 + 错误码分支）。
// 报告 docs/class_func_reports/Inter_AddPvPBuddyResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AddPvPBuddyResultPktView —— Inter_AddPvPBuddyResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AddPvPBuddyResultPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12;                // +0x12
    signed char m_13;         // +0x13
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
    char m_pad15[0x1d];  // 未消费
    char m_32;                // +0x32
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_231.inc"


#include "InterDispatcher/dtor_Inter_RemovePvPBuddyResult.inc"
// ORIG 0x084e1c1e T dispatch_sig —— PVP 好友删除结果：成功转发名字或错误码。
// 报告 docs/class_func_reports/Inter_RemovePvPBuddyResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RemovePvPBuddyResultPktView —— Inter_RemovePvPBuddyResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RemovePvPBuddyResultPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    signed char m_12;         // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
    char m_pad14[0x1d];  // 未消费
    signed char m_31;         // +0x31
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_232.inc"


#include "InterDispatcher/dtor_Inter_NoticePvPBuddyInOut.inc"
// ORIG 0x084e1d96 T dispatch_sig —— PVP 好友进出通知：登录/登出两种包头转发名字。
// 报告 docs/class_func_reports/Inter_NoticePvPBuddyInOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePvPBuddyInOutPktView —— Inter_NoticePvPBuddyInOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticePvPBuddyInOutPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    unsigned char m_12;       // +0x12
    unsigned char m_13;       // +0x13
    signed char m_14;         // +0x14
    char m_15[1];             // +0x15 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_233.inc"


#include "InterDispatcher/dtor_Inter_LoadPvPLiveEventData.inc"
#include "InterDispatcher/dispatch_sig_234.inc"


#include "InterDispatcher/dtor_Inter_Antibot_SendBufferClient.inc"
// ORIG 0x084e43bc T dispatch_sig —— ANTIBOT/DPROTO 缓冲透传客户端：getUser 状态门、
// 异常计数日志、0x230/0x231 包发送与历史记录。
// 报告 docs/class_func_reports/Inter_Antibot_SendBufferClient/dispatch_sig.md 全文转写。
// msg 布局：+0 kind(int,0=ANTIBOT/1=DPROTO) +4 qqID(uint) +8 size(u16) +0xc payload。
#pragma pack(push, 1)
// Inter_Antibot_SendBufferClientPktView —— Inter_Antibot_SendBufferClient::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Antibot_SendBufferClientPktView
{
    int m_0;                  // +0x00
    unsigned int m_4;         // +0x04
    unsigned short m_8;       // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_235.inc"


#include "InterDispatcher/dtor_Inter_Antibot_PunishClient.inc"
// ORIG 0x084e4774 T dispatch_sig —— antibot 惩罚客户端指令。
// 报告 docs/class_func_reports/Inter_Antibot_PunishClient/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Antibot_PunishClientPktView —— Inter_Antibot_PunishClient::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Antibot_PunishClientPktView
{
    unsigned int m_0;         // +0x00
    unsigned char m_4;        // +0x04
    char m_pad5[0x7];  // 未消费
    int m_c;                  // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_236.inc"


#include "InterDispatcher/dtor_Inter_Antibot_ProcessProtectMsg.inc"
// ORIG 0x084e48c2 T dispatch_sig —— 反外挂保护消息：按事件类型回 SAFEMODE 通知包。
// 报告 docs/class_func_reports/Inter_Antibot_ProcessProtectMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Antibot_ProcessProtectMsgPktView —— Inter_Antibot_ProcessProtectMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Antibot_ProcessProtectMsgPktView
{
    // [R14a21] ORIG 0x84e491b 段 `movzwl (%eax),%eax; movzwl %ax,%eax`（**零扩展**，
    // 无 cwtl/movswl）⇒ 该字段为 unsigned short（§T1/T2 直证；switch 值不参与
    // protstatLevel/extra 的语义，仅决定扩展指令）。
    unsigned short m_0;       // +0x00
    unsigned int m_2;         // +0x02
    short m_6;                // +0x06
    unsigned char m_8;        // +0x08
    char m_pad9[0x1];  // 未消费
    short m_a;                // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_237.inc"


#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryMember.inc"
#include "InterDispatcher/dispatch_sig_238.inc"


#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryTeam.inc"
#include "InterDispatcher/dispatch_sig_239.inc"


#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryMemberMatchList.inc"
#include "InterDispatcher/dispatch_sig_240.inc"


#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryTeamMatchList.inc"
#include "InterDispatcher/dispatch_sig_241.inc"
    CUser* user, char* data, int /*len*/)
{
    GlobalData::s_onlinePreliminaryTeamMgr->OnLoadTeamInfo(user, data);
    return 0;
}

#include "InterDispatcher/dtor_Inter_LoadCharacLinkBonus.inc"
// ORIG 0x084e1f02 T dispatch_sig —— 角色联动奖励领取：经验直加、金币走系统邮件，
// 更新联动状态并广播 0x123 通知包。
// 报告 docs/class_func_reports/Inter_LoadCharacLinkBonus/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadCharacLinkBonusPktView —— Inter_LoadCharacLinkBonus::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadCharacLinkBonusPktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_242.inc"


#include "InterDispatcher/dtor_Inter_CreateAccountCargo.inc"
#include "InterDispatcher/dispatch_sig_243.inc"
{
#pragma pack(push, 1)
    struct CreateAccountCargoView
    {
        unsigned char m_errCode;    // +0x0
        char m_pad[3];
        unsigned int m_money;       // +0x4
    };
#pragma pack(pop)
    CreateAccountCargoView* p = reinterpret_cast<CreateAccountCargoView*>(data);
    if (user->get_state() <= 2)
        return 0;
    if (p->m_errCode != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, p->m_errCode);
        return 0;
    }
    user->SetAccountCargoDBData((Inven_Item*)0, 0, p->m_money);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x132);
    return 0;
}

#include "InterDispatcher/dtor_Inter_UpgradeAccountCargo.inc"
// ORIG 0x084e243a T dispatch_sig —— 账号仓库扩容应答。
// 报告 docs/class_func_reports/Inter_UpgradeAccountCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpgradeAccountCargoPktView —— Inter_UpgradeAccountCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpgradeAccountCargoPktView
{
    unsigned char m_0;              // +0x00
    char m_pad1[0x3];  // +0x1..未消费
    unsigned int m_4;               // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_244.inc"


#include "InterDispatcher/dtor_Inter_LoadAccountCargo.inc"
// ORIG 0x084e24ec T dispatch_sig —— 账号仓库 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadAccountCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadAccountCargoPktView —— Inter_LoadAccountCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadAccountCargoPktView
{
    unsigned int m_0;               // +0x00
    unsigned int m_4;               // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_245.inc"


#include "InterDispatcher/dtor_Inter_MultiMailBoxReqSend.inc"
// ORIG 0x084e27b8 T dispatch_sig —— 多邮件批量发送：逐收件人构造 MSG_MAILBOX_SEND 请求 DB 发信。
// 报告 docs/class_func_reports/Inter_MultiMailBoxReqSend/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MultiMailBoxReqSendItemView —— 包内条目区（+0x60 起，stride 0xb）视图；
//   ORIG 084e2896/084e28b8/084e28d8 = `add $0x60` 后 disp 0xa/0xc/0x10
//   （条目基址为动态表达式 ⇒ ORIG 保留 add 0x60 与成员 disp 两段）。
struct Inter_MultiMailBoxReqSendItemView
{
    char m_pad0[0xa];
    unsigned short m_slot;          // +0x0a
    unsigned int m_itemId;          // +0x0c
    unsigned int m_extra;           // +0x10
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_MultiMailBoxReqSendPktView —— Inter_MultiMailBoxReqSend::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MultiMailBoxReqSendPktView
{
    char m_pad0[0x1d];  // 未消费
    unsigned int m_1d;        // +0x1d
    char m_pad21[0x8];  // 未消费
    unsigned int m_29;        // +0x29
    char m_pad2d[0x15];  // 未消费
    unsigned int m_42;        // +0x42
    char m_46[1];             // +0x46 串/字节区首元素（容量未建模）
    char m_pad47[0x1d];  // 未消费
    unsigned int m_64;        // +0x64
    unsigned char m_68;       // +0x68
    char m_pad69[0x3f54];  // 未消费
    int m_3fbd;               // +0x3fbd
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_246.inc"


#include "InterDispatcher/dtor_Inter_ReqQueryCharacInfoMailbox.inc"
// ORIG 0x084e2a06 T dispatch_sig —— 邮箱查询角色信息：转发角色名与等级/职业等摘要。
// 报告 docs/class_func_reports/Inter_ReqQueryCharacInfoMailbox/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqQueryCharacInfoMailboxPktView —— Inter_ReqQueryCharacInfoMailbox::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqQueryCharacInfoMailboxPktView
{
    char m_pad0[0x20];  // 未消费
    int m_20;                 // +0x20
    char m_pad24[0x4];  // 未消费
    signed char m_28;         // +0x28
    char m_pad29[0x1];  // 未消费
    short m_2a;               // +0x2a
    signed char m_2c;         // +0x2c
    signed char m_2d;         // +0x2d
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_247.inc"


#include "InterDispatcher/dtor_Inter_ReqGameServerInfo.inc"
// ORIG 0x084e2b94 T dispatch_sig —— 应答服务器信息查询（monitor/guild 代理 SendGameServerInfo）。
// 报告 docs/class_func_reports/Inter_ReqGameServerInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqGameServerInfoPktView —— Inter_ReqGameServerInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReqGameServerInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
    unsigned char m_b;              // +0x0b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_248.inc"


#include "InterDispatcher/dtor_Inter_MonitorRegistGameServerAck.inc"
// ORIG 0x084e2dee T dispatch_sig —— 监控服注册应答处理。
// 报告 docs/class_func_reports/Inter_MonitorRegistGameServerAck/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorRegistGameServerAckPktView —— Inter_MonitorRegistGameServerAck::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorRegistGameServerAckPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
    char m_b;                       // +0x0b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_249.inc"


#include "InterDispatcher/dtor_Inter_LoadMileage.inc"
#pragma pack(push, 1)
// Inter_LoadMileagePktView —— Inter_LoadMileage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadMileagePktView
{
    char m_pad0[0x8];  // 未消费
    int m_8;                  // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_250.inc"


#include "InterDispatcher/dtor_Inter_GuildExpBookDelete.inc"
// ORIG 0x084e2f1e T dispatch_sig —— 公会经验书删除：目标在线现场删除，离线走 DB+NoCache 包。
// 报告 docs/class_func_reports/Inter_GuildExpBookDelete/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildExpBookDeletePktView —— Inter_GuildExpBookDelete::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildExpBookDeletePktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned int m_e;         // +0x0e
    unsigned int m_12;        // +0x12
    int m_16;                 // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_251.inc"


#include "InterDispatcher/dtor_Inter_NoCache.inc"
#include "InterDispatcher/dispatch_sig_252.inc"
{
#pragma pack(push, 1)
    struct NoCacheMsgView
    {
        char m_pad0[2];
        unsigned short m_len;       // +0x2
        char m_pad1[6];
        unsigned int m_characId;    // +0xa
        unsigned int m_serverGroup; // +0xe
        int m_cacheType;            // +0x12
    };
#pragma pack(pop)
    NoCacheMsgView* msg = reinterpret_cast<NoCacheMsgView*>(data);
    if (msg->m_cacheType == 0 || msg->m_cacheType == 1)
    {
        if (msg->m_characId == 0)
            G_GameWorld()->ResetCacheCharactorMemory();
        else
            G_GameWorld()->ResetCacheCharactorMemory(msg->m_characId);
    }
    if (msg->m_cacheType == 0 || msg->m_cacheType == 2)
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)msg->m_serverGroup
            )->SendPacket(reinterpret_cast<char*>(msg), (unsigned int)msg->m_len);
    return 0;
}

#include "InterDispatcher/dtor_Inter_Send_Charac_no_from_monitor.inc"
// ORIG 0x084e312a T dispatch_sig —— 监控侧角色号下发。
// 报告 docs/class_func_reports/Inter_Send_Charac_no_from_monitor/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Send_Charac_no_from_monitorPktView —— Inter_Send_Charac_no_from_monitor::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Send_Charac_no_from_monitorPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_253.inc"


#include "InterDispatcher/dtor_Inter_SendPeriodicMessage.inc"
// ORIG 0x084e3232 T dispatch_sig —— 周期性公告按状态播发。
// 报告 docs/class_func_reports/Inter_SendPeriodicMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendPeriodicMessagePktView —— Inter_SendPeriodicMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendPeriodicMessagePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_254.inc"


#include "InterDispatcher/dtor_Inter_SendTimeSync.inc"
// ORIG 0x084e3326 T dispatch_sig —— 时间同步按状态广播。
// 报告 docs/class_func_reports/Inter_SendTimeSync/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendTimeSyncPktView —— Inter_SendTimeSync::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendTimeSyncPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned short m_a;             // +0x0a
    unsigned short m_c;             // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_255.inc"


#include "InterDispatcher/dtor_Inter_SendTimeSyncLogin.inc"
// ORIG 0x084e33ec T dispatch_sig —— 登录时时间同步下发。
// 报告 docs/class_func_reports/Inter_SendTimeSyncLogin/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendTimeSyncLoginPktView —— Inter_SendTimeSyncLogin::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendTimeSyncLoginPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned short m_12;            // +0x12
    unsigned short m_14;            // +0x14
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_256.inc"


#include "InterDispatcher/dtor_Inter_AddGuildFundResult.inc"
#include "InterDispatcher/dispatch_sig_257.inc"
{
#pragma pack(push, 1)
    struct AddGuildFundResultView
    {
        char m_pad[0xa];
        unsigned char m_bSuccess;   // +0xa
        int m_characNo;             // +0xb
        char m_pad2[0x4];
        // ORIG +0x58 两步装载（`mov 0x13(%eax),%eax`+`mov %eax,%ebx`）＝unsigned 字段
        // → gain_money(int) 形参的 signedness 失配形态（Ghidra uVar1；探针 t1 实证）。
        unsigned int m_money;       // +0x13
    };
#pragma pack(pop)
    AddGuildFundResultView* p = reinterpret_cast<AddGuildFundResultView*>(data);
    if (user->get_state() <= 2)
        return 0;
    register int characNoMsg = p->m_characNo;
    if (characNoMsg != user->getCurCharacNo())
        return 0;
    if (p->m_bSuccess != 0)
    {
        user->getCurCharacInvenW()->gain_money(
            p->m_money, (eMoneyAddReason)0x1d, true, 0);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, p->m_bSuccess);
    }
    else
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x15b);
    }
    return 0;
}

#include "InterDispatcher/dtor_Inter_TodayGuildMember.inc"
// ORIG 0x084e358a T dispatch_sig —— 今日公会成员登记（SetTodayGuildMember + 0x130 包）。
// 报告 docs/class_func_reports/Inter_TodayGuildMember/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TodayGuildMemberPktView —— Inter_TodayGuildMember::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_TodayGuildMemberPktView
{
    char m_pad0[0xe];  // 未消费
    // [b28d-lane2] P248 档②：ORIG 0x084e37d0 段两处 `mov 0xe/0x16(%eax),%ebx` 均单段
    // 直落 callee-saved ⇒ 字段与 getCurCharacNo() 返回值同型（unsigned）；int 字段会使
    // 4.4 插 `mov %eax,%ebx` 中转（2 处 × 1 条）。
    unsigned int m_e;         // +0x0e
    char m_pad12[0x4];  // 未消费
    unsigned int m_16;        // +0x16
    char m_1a[1];             // +0x1a 串/字节区首元素（容量未建模）
    char m_pad1b[0x1d];  // 未消费
    signed char m_38;         // +0x38
    signed char m_39;         // +0x39
    signed char m_3a;         // +0x3a
    signed char m_3b;         // +0x3b
    signed char m_3c;         // +0x3c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_258.inc"


#include "InterDispatcher/dtor_Inter_CheckJoinGuild.inc"
// ORIG 0x084e3790 T dispatch_sig —— 公会加入检查：错误码转发或公会信息包。
// 报告 docs/class_func_reports/Inter_CheckJoinGuild/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CheckJoinGuildPktView —— Inter_CheckJoinGuild::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CheckJoinGuildPktView
{
    int m_0;                  // +0x00
    char m_4[1];              // +0x4 串/字节区首元素（容量未建模）
    char m_pad5[0x1d];  // 未消费
    char m_22[1];             // +0x22 串/字节区首元素（容量未建模）
    char m_pad23[0x19];  // 未消费
    int m_3c;                 // +0x3c
    int m_40;                 // +0x40
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_259.inc"


#include "InterDispatcher/dtor_Inter_RequestJoinGuild.inc"
// ORIG 0x084e3924 T dispatch_sig —— 入会申请结果通知：成功组 0x15d 信息包，失败发错误包。
// 报告 docs/class_func_reports/Inter_RequestJoinGuild/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RequestJoinGuildPktView —— Inter_RequestJoinGuild::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestJoinGuildPktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04
    char m_8[1];              // +0x8 串/字节区首元素（容量未建模）
    char m_pad9[0x1d];  // 未消费
    char m_26[1];             // +0x26 串/字节区首元素（容量未建模）
    char m_pad27[0x19];  // 未消费
    int m_40;                 // +0x40
    int m_44;                 // +0x44
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_260.inc"


#include "InterDispatcher/dtor_Inter_JoinGuildInfo.inc"
// ORIG 0x084e3ace T dispatch_sig —— 公会加入信息：按结果码选择字段转发。
// 报告 docs/class_func_reports/Inter_JoinGuildInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_JoinGuildInfoPktView —— Inter_JoinGuildInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_JoinGuildInfoPktView
{
    signed char m_0;          // +0x00
    char m_pad1[0x3];  // 未消费
    int m_4;                  // +0x04
    char m_8[1];              // +0x8 串/字节区首元素（容量未建模）
    char m_pad9[0x1d];  // 未消费
    char m_26[1];             // +0x26 串/字节区首元素（容量未建模）
    char m_pad27[0x19];  // 未消费
    int m_40;                 // +0x40
    int m_44;                 // +0x44
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_261.inc"


#include "InterDispatcher/dtor_Inter_ApproveJoinGuild.inc"
// ORIG 0x084e3f0e T dispatch_sig —— 公会入会审批应答。
// 报告 docs/class_func_reports/Inter_ApproveJoinGuild/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ApproveJoinGuildPktView —— Inter_ApproveJoinGuild::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ApproveJoinGuildPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    int m_e;                        // +0x0e
    char m_pad12[0x4];  // +0x12..未消费
    int m_16;                       // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_262.inc"


#include "InterDispatcher/dtor_Inter_GuildJoinList.inc"
// ORIG 0x084e3c60 T dispatch_sig —— 公会加入申请列表下发（0x161 包，逐申请项序列化）。
// 报告 docs/class_func_reports/Inter_GuildJoinList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildJoinListEntryWinView —— Inter_GuildJoinList::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_GuildJoinListEntryWinView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    char m_pad8[0x1e];  // 未消费
    signed char m_26;         // +0x26
    signed char m_27;         // +0x27
    signed char m_28;         // +0x28
    signed char m_29;         // +0x29
    char m_pad2a[0x1a];  // 未消费
    int m_44;                 // +0x44
};
#pragma pack(pop)

// [R5·InterDisp 形态收口 2026-09-11] 本件两项形态实验的负结论（均 routed 口径实测）：
//   ① 回滚：ORIG 4 字节字段发 `add $0x20,%eax` + `movzbl 0x{6,7,9,8}(%eax),%eax`
//      （0x084e3d79/d7a5/d7d1/d7fd）、第 2 串发 `add $0x20,%eax; add -0x18(%ebp),%eax;
//      add $0xa,%eax`（0x084e3e23/e26/e29）。试「entry +0x20 具名子结构视图 + 逐处内联宏」
//      复现该两步形：形态确实产出 `add $0x20; movzbl 0x6(%eax)`，但 our 206→214、gap +6→-2、
//      ndiff 25→31 ⇒ 反劣，回滚（视图形态不如扁平偏移 + 两步裸偏移）。
//   ② 回滚：ORIG 每处基址首指令为 `mov -0x14(%ebp),%eax`（先取 local_18）—— 试以
//      `local_18 * 0x44 + local_1c`（索引项作左操作数）复现该加载序：codegen 逐字节不变
//      （4.4 前端 gimplify 重排加法树）⇒ 加载序不可造，判 N 档。
//   保留：m_4/m_44 的裸指针算术两步形（ORIG 0x084e3cf9 `add $0x4,%eax; mov (%eax),%eax`、
//      0x084e3e8b `add $0x44,%eax; mov (%eax),%eax`）⇒ our +2 条、gap +8→+6、ndiff 26→25。

#include "InterDispatcher/dispatch_sig_263.inc"


#include "InterDispatcher/dtor_Inter_GuildAttendanceInfo.inc"
// ORIG 0x084e4052 T dispatch_sig —— 公会签到信息：校验当前角色号后转发签到统计字段。
// 报告 docs/class_func_reports/Inter_GuildAttendanceInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildAttendanceInfoPktView —— Inter_GuildAttendanceInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildAttendanceInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    signed char m_12;               // +0x12
    int m_13;                       // +0x13
    int m_17;                       // +0x17
    int m_1b;                       // +0x1b
    int m_1f;                       // +0x1f
    int m_23;                       // +0x23
    int m_27;                       // +0x27
    int m_2b;                       // +0x2b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_264.inc"


#include "InterDispatcher/dtor_Inter_AchieveGuildAttendance.inc"
// ORIG 0x084e41e4 T dispatch_sig —— 公会签到达成应答。
// 报告 docs/class_func_reports/Inter_AchieveGuildAttendance/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AchieveGuildAttendancePktView —— Inter_AchieveGuildAttendance::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_AchieveGuildAttendancePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
    int m_16;                       // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_265.inc"


#include "InterDispatcher/dtor_Inter_UpdatePurchaseCashItemBonus.inc"
// ORIG 0x084e42e0 T dispatch_sig —— 商城消费奖励邮件触发。
// 报告 docs/class_func_reports/Inter_UpdatePurchaseCashItemBonus/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdatePurchaseCashItemBonusPktView —— Inter_UpdatePurchaseCashItemBonus::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpdatePurchaseCashItemBonusPktView
{
    unsigned int m_0;               // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_266.inc"


#include "InterDispatcher/dtor_Inter_HadesDisConnect.inc"
#include "InterDispatcher/dispatch_sig_267.inc"


#include "InterDispatcher/dtor_Inter_GoblinPadChagePwdReward.inc"
#include "InterDispatcher/dispatch_sig_268.inc"


#include "InterDispatcher/dtor_Inter_GoblinPadSamePwdCheck.inc"
#include "InterDispatcher/dispatch_sig_269.inc"
    CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    if (user != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x74),
                                 static_cast<unsigned char>(0x8b));
    }
    return 0;
}

#include "InterDispatcher/dtor_Inter_PunishCancel.inc"
// ORIG 0x084e4b02 T dispatch_sig —— 交易惩罚撤销。
// 报告 docs/class_func_reports/Inter_PunishCancel/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PunishCancelPktView —— Inter_PunishCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_PunishCancelPktView
{
    char m_pad0[0xe];  // 未消费
    short m_e;                // +0x0e
    short m_10;               // +0x10
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_270.inc"


#include "InterDispatcher/dtor_Inter_BroadcastMsg.inc"
// ORIG 0x084e4c2a T dispatch_sig —— 少年频道公告组播。
// 报告 docs/class_func_reports/Inter_BroadcastMsg/dispatch_sig.md 全文转写。
// CBelong 唯一声明点 = CSchoolMgr.h（本 TU 已 include；NL4 合并铁律迁移）。

// ORIG CUserTeen（CBelong 派生，C1 W 0x084f07d0 / IsRight W；BroadcastMsg 消费；
// 定义属主未落地，U 引用合规——InterWave1 批）
class CUserTeen : public CBelong
{
public:
    CUserTeen();
    bool IsRight(CUser* user);   // bool 化随 CBelong::IsRight 权威签名（mangled 不编码返回类型）
};

#include "InterDispatcher/CUserTeen.inc"

#include "InterDispatcher/IsRight.inc"
{
    // ORIG +0x06：无 `movl $0x0,-0xc(%ebp)` ⇒ 局部不初始化（仅取址传引用）；
    int dummy;
    if (!user->CheckLimitMiniumAge(0xf, dummy))
        return 0;
    return 1;
}

#pragma pack(push, 1)
// Inter_BroadcastMsgPktView —— Inter_BroadcastMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_BroadcastMsgPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned char m_e;        // +0x0e
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_271.inc"


#include "InterDispatcher/dtor_Inter_TOD_ReturnToMonitor_APCSettingComplete.inc"
#include "InterDispatcher/dispatch_sig_272.inc"


#include "InterDispatcher/dtor_Inter_TOD_Reload_APC.inc"
#include "InterDispatcher/dispatch_sig_273.inc"


#include "InterDispatcher/dtor_Inter_UpdateSecuServiceState.inc"
// ORIG 0x084e4e84 T dispatch_sig —— 安全服务状态更新：msg[0x14]=0 后转 Secu_DataControl。
// 报告 docs/class_func_reports/Inter_UpdateSecuServiceState/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdateSecuServiceStateMsgWinView —— Inter_UpdateSecuServiceState::dispatch_sig 窗口 msg 元素视图
//   （msg 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_UpdateSecuServiceStateMsgWinView
{
    char m_pad0[0xe];  // 未消费
    unsigned char m_e;        // +0x0e
    unsigned char m_f;        // +0x0f
    char m_pad10[0x4];  // 未消费
    unsigned char m_14;       // +0x14
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_274.inc"


#include "InterDispatcher/dtor_Inter_LoadKillMonsterInfo.inc"
#include "InterDispatcher/dispatch_sig_275.inc"


#include "InterDispatcher/dtor_Inter_LevelUpReward.inc"
#include "InterDispatcher/dispatch_sig_276.inc"
{
    if (user->get_state() == 0)
        return 0x62a6;
    unsigned int field0;
    unsigned int field4;
    memcpy(&field0, data, 4);
    memcpy(&field4, data + 4, 4);
    (void)field0; (void)field4;
    return 0;
}

#include "InterDispatcher/dtor_Inter_GetUserRegdate.inc"
#pragma pack(push, 1)
// Inter_GetUserRegdatePktView —— Inter_GetUserRegdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GetUserRegdatePktView
{
    unsigned int m_0;         // +0x00
    unsigned int m_4;         // +0x04
    unsigned int m_8;         // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_277.inc"


#include "InterDispatcher/dtor_Inter_GoldControlMoneyActionCancel.inc"
// ORIG 0x084e4fe8 T dispatch_sig —— 金币控制邮件操作取消/完成应答。
// 报告 docs/class_func_reports/Inter_GoldControlMoneyActionCancel/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GoldControlMoneyActionCancelPktView —— Inter_GoldControlMoneyActionCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GoldControlMoneyActionCancelPktView
{
    unsigned char m_0;              // +0x00
    bool m_1;                       // +0x01
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_278.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildChatMsgHyperLink.inc"
// ORIG 0x084e503c T dispatch_sig —— 监控公会聊天超链接广播（字符串+超链接数组）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildChatMsgHyperLink/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildChatMsgHyperLinkPktView —— Inter_MonitorNoticeGuildChatMsgHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildChatMsgHyperLinkPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    // ORIG 0x084e51xx 循环体走数组寻址 `imul $0x68 → add $0x30 → add 基址槽 → add $0x1`
    //（结构位移 0x30 + 元素首字段 0x1 两步；Member 版同配方 probe addr4 t18 实证）。
    struct ItemView { char m_0[1]; char m_pad[0x67]; };  // stride 0x68
    ItemView m_items[1];      // +0x31 链接项区（实际个数=m_30；容量未建模）
    char m_pad31[0xd0];  // 未消费
    unsigned char m_169;      // +0x169
    char m_16a[1];            // +0x16a 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_279.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberChatMsgHyperLink.inc"
// ORIG 0x084e5282 T dispatch_sig —— 公会员聊天超链接公告包（0x173）下发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberChatMsgHyperLink/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberChatMsgHyperLinkPktView —— Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberChatMsgHyperLinkPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    // ORIG 0x084e56xx 循环体走数组寻址 `imul $0x68 → add $0x30 → add 基址槽 → add $0x1`；
    // gcc 把数组基 0x31 折成 0x30(结构位移)+0x1(元素首字段)两步（probe addr4 t18 实证）。
    struct ItemView { char m_0[1]; char m_pad[0x67]; };  // stride 0x68
    ItemView m_items[1];        // +0x31 链接项区（实际个数=m_30；容量未建模）
    char m_pad99[0xd0];    // 未消费
    unsigned char m_169;      // +0x169
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_280.inc"


#include "InterDispatcher/dtor_Inter_MonitorNoticeOtherChannelChatHyperLink.inc"
#pragma pack(push, 1)
// Inter_MonitorNoticeOtherChannelChatHyperLinkPktView —— Inter_MonitorNoticeOtherChannelChatHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeOtherChannelChatHyperLinkPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x5];  // 未消费
    int m_13;                 // +0x13
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    unsigned char m_35;       // +0x35
    unsigned char m_36;       // +0x36
    char m_pad37[0x138];  // 未消费
    unsigned char m_16f;      // +0x16f
    char m_170[1];            // +0x170 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [L9-H 2026-09-11] P30/P235「数组成员两步位移」：ORIG 084e5528 段循环体发
//   `imul …,0x68; add $0x30; add <基址槽>; add $0x7`（4 条 = 结构位移 0x30 + 元素内 0x7 两步），
//   我方 `data + i*0x68 + 0x37` 把 0x30+7 折成单常量 ⇒ `imul; add $0x37; add data`（3 条）。
//   同族先例：Inter_MonitorNoticeGuildChatMsgHyperLinkPktView.ItemView（stride 0x68，两步配方）。
//   既有本件 PktView 的 m_pad37 不能在 0x30 处再挂数组（与 m_35/m_36 重叠），故本件自带
//   含 ItemView 的组合视图；m_items[0].m_linkType/m_itemCount 与既有 m_35/m_36 同址
//   （探针 /tmp/L9/H/p4.cpp：`m_items[0].m_linkType` ⇒ `movzx eax,[eax+0x35]` 单条等价）。
#pragma pack(push, 1)
struct Inter_MonitorNoticeOtherChannelChatHyperLinkListView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x5];  // 未消费
    int m_13;                 // +0x13
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x18];  // 未消费
    struct ItemView
    {
        char m_pad0[0x5];           // 元素 +0x00..0x04（元素基 0x30）
        unsigned char m_linkType;   // +0x05 ⇒ 绝对 +0x35（同 m_35）
        unsigned char m_itemCount;  // +0x06 ⇒ 绝对 +0x36（同 m_36）
        char m_binary[1];           // +0x07 ⇒ 绝对 +0x37（put_binary 载荷）
        char m_pad[0x60];           // 补齐 stride 0x68
    };
    ItemView m_items[1];   // +0x30 起，stride 0x68
    char m_pad98[0xd7];  // +0x98..0x16e 未消费
    unsigned char m_16f;      // +0x16f
    char m_170[1];            // +0x170 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_281.inc"


#include "InterDispatcher/dtor_Inter_Monitor_MegaphoneMsgHyperLink.inc"
// ORIG 0x084e5c26 T dispatch_sig —— 大喇叭超链接广播（0x174 包，按频道整合判定分发）。
// 报告 docs/class_func_reports/Inter_Monitor_MegaphoneMsgHyperLink/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_MegaphoneMsgHyperLinkPktView —— Inter_Monitor_MegaphoneMsgHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_MegaphoneMsgHyperLinkPktView
{
    char m_pad0[0xa];  // 未消费
    signed char m_a;          // +0x0a
    unsigned char m_b;        // +0x0b
    unsigned short m_c;       // +0x0c
    unsigned char m_e;        // +0x0e
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
    char m_pad10[0x1d];  // 未消费
    unsigned char m_2d;       // +0x2d
    char m_pad2e[0x138];  // 未消费
    unsigned char m_166;      // +0x166
    char m_167[1];            // +0x167 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_282.inc"


#include "InterDispatcher/dtor_Inter_SendSecuReward.inc"
#include "InterDispatcher/dispatch_sig_283.inc"


#include "InterDispatcher/send_alert.inc"
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0, 0x1bf);
    guard.put_int(static_cast<int>(code));
    guard.finalize(true);
    user->Send(guard);
}

#include "InterDispatcher/dtor_Inter_HadesAction.inc"
// ORIG 0x084e5f02 T dispatch_sig —— Hades 行为处理：登录用户现场执行，未登录用户落惩罚 DB。
// 报告 docs/class_func_reports/Inter_HadesAction/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_HadesActionPktView —— Inter_HadesAction::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_HadesActionPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    unsigned int m_12;        // +0x12
    unsigned int m_16;        // +0x16
    unsigned int m_1a;        // +0x1a
    unsigned int m_1e;        // +0x1e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_284.inc"


#include "InterDispatcher/dtor_Inter_Check3rdPartyConcent.inc"
#include "InterDispatcher/dispatch_sig_285.inc"


#include "InterDispatcher/dtor_Inter_SelectCharacMid.inc"
// ORIG 0x084e6170 T dispatch_sig —— 角色选择中间校验/hack 计数与金币追踪。
// 报告 docs/class_func_reports/Inter_SelectCharacMid/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SelectCharacMidPktView —— Inter_SelectCharacMid::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SelectCharacMidPktView
{
    int m_0;                  // +0x00
    unsigned int m_4;         // +0x04
    unsigned int m_8;         // +0x08
    char m_padc[0x4];  // 未消费
    unsigned int m_10;        // +0x10
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_286.inc"


#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemInfoAll.inc"
// ORIG 0x084e6258 T dispatch_sig —— NPC 限购全部条目信息下发（0x193 包）。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfoAll/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemInfoAllPktView —— Inter_NpcLimitBuyItemInfoAll::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NpcLimitBuyItemInfoAllPktView
{
    char m_pad0[0x16];  // +0x0..未消费
    int m_16;                       // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_287.inc"


#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemChangeInfo.inc"
// ORIG 0x084e6448 T dispatch_sig —— NPC 限购道具变更全网广播。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemChangeInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemChangeInfoPktView —— Inter_NpcLimitBuyItemChangeInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NpcLimitBuyItemChangeInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_288.inc"


#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemInfoRenew.inc"
// ORIG 0x084e6e82 T dispatch_sig —— NPC 限购物品余量刷新：组 0x193 包全服广播。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfoRenew/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemInfoRenewPktView —— Inter_NpcLimitBuyItemInfoRenew::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// [R9form4 2026-09-12] 记录数组具名化（ORIG 0x084e6ed7 实证：`mov -0xc(%ebp),%edx; mov
//   -0x10(%ebp),%ecx; mov %edx,%eax; add %eax,%eax; add %edx,%eax; shl $0x2,%eax;
//   mov 0xe(%eax,%ecx,1),%eax` = base(具名 pPkt) + i*0xc + 0xe 折叠 disp）；
//   递归记录步长 0xc、元素 3×int（+0x0/+0x4/+0x8）。
struct Inter_NpcLimitBuyItemInfoRenewRec
{
    int m_f0;   // +0x00
    int m_f1;   // +0x04
    int m_f2;   // +0x08
};
struct Inter_NpcLimitBuyItemInfoRenewPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    Inter_NpcLimitBuyItemInfoRenewRec m_recs[1];  // +0x0e，步长 0xc（越界读照抄 ORIG，P171）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_289.inc"


#include "InterDispatcher/dtor_Inter_MonitorReplyCharacInfo.inc"
// ORIG 0x084e7024 T dispatch_sig —— 监控服角色信息查询应答：组 0x1d6 包回发。
// 报告 docs/class_func_reports/Inter_MonitorReplyCharacInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorReplyCharacInfoPktView —— Inter_MonitorReplyCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorReplyCharacInfoPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    unsigned char m_16;       // +0x16
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    short m_35;               // +0x35
    signed char m_37;         // +0x37
    signed char m_38;         // +0x38
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_290.inc"


#include "InterDispatcher/dtor_Inter_CeraShopPurchaseCount.inc"
// ORIG 0x084e7bfe T dispatch_sig —— 塞拉商城购买加成道具处理。
// 报告 docs/class_func_reports/Inter_CeraShopPurchaseCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CeraShopPurchaseCountPktView —— Inter_CeraShopPurchaseCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_CeraShopPurchaseCountPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned long m_4;              // +0x04
    unsigned short m_8;             // +0x08
    char m_pada[0x2];  // +0xa..未消费
    int m_c;                        // +0x0c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_291.inc"


#include "InterDispatcher/dtor_Inter_OwnerisVIP.inc"
// ORIG 0x084e7c90 T dispatch_sig —— 点数拍卖 VIP 购买：计费/组包/预付登记。
// 报告 docs/class_func_reports/Inter_OwnerisVIP/dispatch_sig.md 全文转写。
// 实参口径（Ghidra 同证）：第二槽=CUser*，第三槽=int 位置实际传 msg 指针。
// ORIG PCK_AUCTION_BIDDING_GP（nsl::PACKET_HEADER 派生 pack(1)，
// C1/C2 W 0x084e9a98 sz0x67）。ctor 反汇编：nsl::PACKET_HEADER(this)；
// memset(this, 0, 0x33)；setCategory(0x12)；setPacketID(5)；setSize(0x33)
// —— 与 PACKET_CTOR_BODY(0x12, 5, 0x33) 宏语义逐指令一致。
struct PCK_AUCTION_BIDDING_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_BIDDING_GP() { PACKET_CTOR_BODY(0x12, 5, 0x33); }
};

#pragma pack(push, 1)
// Inter_OwnerisVIPPktView —— Inter_OwnerisVIP::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_OwnerisVIPPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned int m_4;               // +0x04
    unsigned int m_8;               // +0x08
    bool m_c;                       // +0x0c
    int m_d;                        // +0x0d
    char m_pad11[0x11];  // +0x11..未消费
    unsigned int m_22;              // +0x22
    int m_26;                       // +0x26
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_OwnerisVIPMsgWinView —— Inter_OwnerisVIP::dispatch_sig 窗口 msg 元素视图
//   （msg 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_OwnerisVIPMsgWinView
{
    char m_pad0[0x4];  // 未消费
    unsigned long long m_4;   // +0x04
};
#pragma pack(pop)

// Inter_OwnerisVIPTailView —— needInfo+0xc..+0x27（0x1c）尾部块视图：ORIG 用 7 条 movl
//   逐 dword 赋值（无 memcpy call、无 rep movsl，p167 实证）⇒ 结构整体赋值形（N84/P83）。
struct Inter_OwnerisVIPTailView
{
    unsigned int m_0, m_4, m_8, m_c, m_10, m_14, m_18;
};

#include "InterDispatcher/dispatch_sig_292.inc"


#include "InterDispatcher/dtor_Inter_LoadPuUserCheck.inc"
#include "InterDispatcher/dispatch_sig_293.inc"


#include "InterDispatcher/dtor_Inter_CollectItemsResult.inc"
#include "InterDispatcher/dispatch_sig_294.inc"
{
#pragma pack(push, 1)
    struct CollectItemsResultView
    {
        char m_pad[0xa];
        int m_total;        // +0xa
        int m_fieldE;       // +0xe
    };
#pragma pack(pop)
    CollectItemsResultView* p = reinterpret_cast<CollectItemsResultView*>(data);
    if (p->m_total != 0)
    {
        int level = (p->m_fieldE * 100) / p->m_total;
        if (100 < level)
            level = 100;

        GlobalData::s_timeGate_->SetLevel(level);

        PacketGuard packet;
        packet.put_header(0, 0x1a6);
        packet.put_byte(level);
        packet.finalize(true);

        G_GameWorld()->send_to_area(
            GlobalData::s_timeGate_->getVillageIndex(),
            GlobalData::s_timeGate_->getAreaIndex(),
            packet);
    }
    return 0;
}

#include "InterDispatcher/dtor_Inter_CollectItemsReward.inc"
// ORIG 0x084e71ee T dispatch_sig —— 收集品奖励邮件：result 分支选道具 + 脚本文案 +
// CItem::make_item 生成物品经系统邮件发放。
// 报告 docs/class_func_reports/Inter_CollectItemsReward/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CollectItemsRewardPktView —— Inter_CollectItemsReward::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_CollectItemsRewardPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_295.inc"


#include "InterDispatcher/dtor_Inter_CollectItemsRewardBroadcast.inc"
// ORIG 0x084e7598 T dispatch_sig —— 收集活动奖励广播：时间之门等级联动全员通知。
// 报告 docs/class_func_reports/Inter_CollectItemsRewardBroadcast/dispatch_sig.md 全文转写。
// ORIG CUserModifier 最小镜像（vtable 槽0=Modify(CUser*)，无虚析构，同 ORIG 口径）
class CUserModifier
{
public:
    virtual void Modify(CUser* user) = 0;
};

// ORIG CUserResetTimeGateQuest : CUserModifier（C1 W 0x084f0886 / Modify W 0x084f07ec；
// CollectItemsRewardBroadcast 消费；定义属主未落地，U 引用合规——InterWave1 批）
class CUserResetTimeGateQuest : public CUserModifier
{
public:
    CUserResetTimeGateQuest();
    void Modify(CUser* user);
};

#include "InterDispatcher/CUserResetTimeGateQuest.inc"

#include "InterDispatcher/Modify.inc"


#pragma pack(push, 1)
// Inter_CollectItemsRewardBroadcastPktView —— Inter_CollectItemsRewardBroadcast::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CollectItemsRewardBroadcastPktView
{
    char m_pad0[0x12];  // 未消费
    unsigned char m_12;       // +0x12
    char m_pad13[0x1];  // 未消费
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_296.inc"


#include "InterDispatcher/dtor_Inter_RequestCharacSkillInfo.inc"
// ORIG 0x084e7884 T dispatch_sig —— 请求 tag 角色技能：写入技能树编号并重发技能信息。
// 报告 docs/class_func_reports/Inter_RequestCharacSkillInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RequestCharacSkillInfoPktView —— Inter_RequestCharacSkillInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestCharacSkillInfoPktView
{
    unsigned int m_0;         // +0x00
    char m_pad4[0x198];  // 未消费
    unsigned char m_19c;      // +0x19c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_297.inc"


#include "InterDispatcher/dtor_Inter_RequestTagCharacInfo.inc"
// ORIG 0x084e79f2 T dispatch_sig —— 切换 tag 角色：设置 tag 状态与全部子对象 parent 后广播。
// 报告 docs/class_func_reports/Inter_RequestTagCharacInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RequestTagCharacInfoPktView —— Inter_RequestTagCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestTagCharacInfoPktView
{
    unsigned int m_0;         // +0x00
    unsigned char m_4;        // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_298.inc"


#include "InterDispatcher/dtor_Inter_EmergencyPatchMessage.inc"
// ORIG 0x084e7b84 T dispatch_sig —— 紧急补丁公告（禁止新连接+广播 10 分钟关服）。
// 报告 docs/class_func_reports/Inter_EmergencyPatchMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_EmergencyPatchMessagePktView —— Inter_EmergencyPatchMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_EmergencyPatchMessagePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_299.inc"


#include "InterDispatcher/dtor_Inter_AuctionSubMoney.inc"
// ORIG 0x084e8004 T dispatch_sig —— 拍卖扣款异常登记服务器 hack 计数。
// 报告 docs/class_func_reports/Inter_AuctionSubMoney/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AuctionSubMoneyPktView —— Inter_AuctionSubMoney::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_AuctionSubMoneyPktView
{
    unsigned int m_0;               // +0x00
    // ORIG +0x14/+0x17 两步装载（`mov 0x4(%eax),%eax`+`mov %eax,%esi`）而 +0x1f 的 m_0
    // 一步直落 %ebx ⇒ 本字段为 signed int（→addServerHackCnt(unsigned) 形参失配）。
    int m_4;                        // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_300.inc"


#include "InterDispatcher/dtor_Inter_UpdateServerSnapShot.inc"
#include "InterDispatcher/dispatch_sig_301.inc"
{
    G_GameWorld()->UpdateServerSnapShot();
    return 0;
}

#include "InterDispatcher/dtor_Inter_LoadCharacTypeHotKey.inc"
// ORIG 0x084e87de T dispatch_sig —— DB 热键配置装载。
// 报告 docs/class_func_reports/Inter_LoadCharacTypeHotKey/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadCharacTypeHotKeyPktView —— Inter_LoadCharacTypeHotKey::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCharacTypeHotKeyPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned char m_4;              // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_302.inc"


#include "InterDispatcher/dtor_Inter_UpdateMiniCraneSeed.inc"
#pragma pack(push, 1)
// Inter_UpdateMiniCraneSeedPktView —— Inter_UpdateMiniCraneSeed::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpdateMiniCraneSeedPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_303.inc"


#include "InterDispatcher/dtor_Inter_GetPremiumSelectedCubeInfo.inc"
// ORIG 0x084e8936 T dispatch_sig —— 高级方块合约信息初始化。
// 报告 docs/class_func_reports/Inter_GetPremiumSelectedCubeInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GetPremiumSelectedCubeInfoPktView —— Inter_GetPremiumSelectedCubeInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GetPremiumSelectedCubeInfoPktView
{
    char m_pad0[0x4];  // 未消费
    char m_4;                 // +0x04
    unsigned char m_5;        // +0x05
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_304.inc"


#include "InterDispatcher/dtor_Inter_CheckAddFriend.inc"
// ORIG 0x084e8a42 T dispatch_sig —— 添加好友检查：空指针告警或转发监视服添加请求。
// 报告 docs/class_func_reports/Inter_CheckAddFriend/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CheckAddFriendPktView —— Inter_CheckAddFriend::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CheckAddFriendPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x1];  // 未消费
    char m_5[1];              // +0x5 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_305.inc"


#include "InterDispatcher/dtor_Inter_LoadCerashopAddRestrictType.inc"
// ORIG 0x080e2104 T dispatch_sig —— DB 限购类型数据装载。
// 报告 docs/class_func_reports/Inter_LoadCerashopAddRestrictType/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadCerashopAddRestrictTypePktView —— Inter_LoadCerashopAddRestrictType::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCerashopAddRestrictTypePktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_306.inc"


// ORIG 0x084e8ce4 T dispatch_sig —— 从 monitor 装载角色升级支援事件状态（m_levelupEvents 同步 + DB 上报）。
// 报告 docs/class_func_reports/Inter_LoadEventLevelupSupport/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadEventLevelupSupportPktView —— Inter_LoadEventLevelupSupport::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadEventLevelupSupportPktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

// [K3 2026-09-11] P235「数组成员视角」+ P-L11c：
//   ① ORIG 084e8ce4 段对 msg 条目发 stride-4 成员直取：`movzwl 0x4(%eax,%edx,4),%eax`
//      （条目 i 的 m_level）与 `movzwl 0x6(%edx,%ecx,4),%edx`（同条目 m_state ⇒ +0x4+2）⇒
//      原码是 `{ int count; EventInfo entries[]; }` 数组成员访问（EventInfo 布局 u16 level/u16 state
//      与 wire 一致）；我方 char* 算术折 shl/add，每处多 4 条。
//   ② 守卫先行、指针物化在守卫之后（ORIG 先 `cmpl $0x0,0xc(%ebp)`/`0x10(%ebp)`，随后才
//      `mov 0x10(%ebp),%eax; mov %eax,-0x28(%ebp)`）⇒ 唯一指针槽，无 local_2c 副本（P-L11c）。
//   ③ 循环 1 条件 ORIG `cmp i,size; seta`（无符号）⇒ 去 `(int)`；
//      `m_level <= get_charac_level()` ORIG `cmp %eax,%ebx; setle`（有符号 int）
//      ⇒ 去 `(unsigned int)`（语义修复：负等级不再被当大数）。
//   ④ 第三循环 state/level 无具名槽（ORIG 直入 %edi/%esi 跨 makeRequest call 保活）⇒ 内联实参。
#pragma pack(push, 1)
struct Inter_LoadEventLevelupSupportEntryPktView
{
    int m_count;                                        // +0x0（ORIG `mov (%eax),%eax`）
    LevelupSupportEventManger::EventInfo m_entries[1];  // +0x4 起，stride 4
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_307.inc"


#include "InterDispatcher/dispatch_sig_308.inc"



#include "InterDispatcher/Inter_NpcLimitBuyItemInfo.inc"


#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemInfo.inc"


#include "InterDispatcher/reset.inc"


#include "InterDispatcher/checkError.inc"
{
    // ORIG 084e6ae1/084e6b22 单条 `mov 0x12(%eax),%eax` / `mov 0x16(%eax),%edx`（disp 折叠，
    //   无 add 两步）⇒ 偏移访问须经具型视图成员（局部类型不发射符号）。
    // [H2·2026-09-11 laneD 语义轴修] 该局部 struct 未 pack ⇒ 默认对齐把 m_charNo/m_itemId
    //   由声明的 +0x12/+0x16 推到 **+0x14/+0x18**（int 需 4B 对齐，m_pad0[0x12] 后插 2B pad），
    //   而 ORIG 084e6ae1/084e6b22 读 `mov 0x12(%eax),%eax` / `mov 0x16(%eax),%edx`
    //   ⇒ 真·错读 2 字节。补 pack(1)（仅包住该本地类型）。
#pragma pack(push, 1)
    struct InfoView
    {
        char m_pad0[0x12];
        int m_charNo;   // +0x12
        int m_itemId;   // +0x16
    };
#pragma pack(pop)
    if (user == 0)
        return 3;
    if (user->get_state() <= 2)
        return 3;
    if (user->get_charac_no(-1) !=
        reinterpret_cast<InfoView*>(info)->m_charNo)
        return 3;

    m_error = reinterpret_cast<InfoView*>(info)->m_itemId;
    if (m_error == 0)
        return 0x11;

    m_itemRef =
        reinterpret_cast<int>(G_CDataManager()->find_item(m_error));
    if (m_itemRef == 0)
        return 0x11;

    const CItem* mat = reinterpret_cast<CItem*>(m_itemRef)->GetNeedMaterial();
    m_needItem.first  = mat->m_index;    // GetNeedMaterial().first（CItem* 首 int）
    m_needItem.second = mat->m_grade;    // GetNeedMaterial().second
    if (m_needItem.first == 0 || m_needItem.first == -1)
        return 0x17;

    const CInventory* invenR = user->getCurCharacInvenRefR();
    m_param2 = invenR->GetInvenData(m_needItem.first, m_invenItem2);
    if (m_param2 < 0)
        return 0x15;
    if (m_needItem.second < 1 ||
        m_invenItem2.get_add_info() < m_needItem.second)
        return 0x16;
    return 0;
}

// ORIG 0x084e65a2 T dispatch_sig —— NPC 限量购买下单：定位用户、校验后生成物品
// 入背包（失败走系统邮件兜底），扣减源物品并回执 monitor。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfo/dispatch_sig.md 全文转写。
// LimitNpcBuyItemUpdate NEEDS_DECL（PacketHeader(0x27dd,0x1a) 派生，见 meta）。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemInfoPktView —— Inter_NpcLimitBuyItemInfo::dispatch_sig 包视图（偏移/命名与
//   Monitor/LimitNpcBuyItem.h 权威类 LimitNpcBuyItemInfo 逐字段一致；该头不在本 TU 的 -I 路径内，
//   故本 TU 以同布局镜像视图承载，字段表见下方逐项 ORIG 证据）。
//   ORIG 084e65a2 逐访问：+0x0a `mov 0xa(%eax),%ebx`（getUser 第1参）/+0x12 `mov 0x12(%eax)`/03
//   +0x16 `mov 0x16(%eax)`/+0x1a `mov 0x1a(%eax)`/+0x22 `mov 0x22(%eax); test`（全字判 + movzbl %al 传参）。
struct Inter_NpcLimitBuyItemInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_itemIdReq;       // +0x0a（权类同名）
    unsigned int m_dbid;            // +0x0e（本函数未消费）
    unsigned int m_charNo;          // +0x12
    unsigned int m_itemId;          // +0x16
    unsigned int m_count;           // +0x1a
    unsigned int m_pad1e;           // +0x1e（本函数未消费）
    int m_errorNo;                  // +0x22
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_309.inc"


#include "InterDispatcher/dtor_ISigDispatcher.inc"
