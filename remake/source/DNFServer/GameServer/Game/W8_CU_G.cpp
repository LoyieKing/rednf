// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W8_CU_G/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W8_CU_G.cpp — CUser ORIG Wave8-C 残余 G 批：hack/secu/capcha/exp/inform 族
//
// 本 TU 符号清单（25 个成员函数 + 2 个支撑小类 ctor；来源=报告 md 或 objdump）：
//   1. _ZN5CUser21gainMoneyFromHackUserEjyjy          0x0867fbba CUser/gainMoneyFromHackUser.md
//   2. _ZN5CUser11setHackUserEjjjjtj                  0x0867f748 CUser/setHackUser.md
//   3. _ZN5CUser11getHashCodeEPh                      0x084fa298 CUser/getHashCode.md
//   4. _ZN5CUser17checkHumanCertifyEiPt               0x0867f4c8 CUser/checkHumanCertify.md
//   5. _ZN5CUser22VerifyCleanPadVeriDataEj            0x08649b44 CUser/VerifyCleanPadVeriData.md
//   6. _ZN5CUser18reqSendMailCertifyEv                0x0868a51a CUser/reqSendMailCertify.md
//   7. _ZN5CUser24incTotalHackUserTradeCntEj          0x086972ee objdump（m_379.m_hackTradeCnt += n）
//   8. _ZN5CUser25incTotalHackUserTradeGoldEy         0x086972b6 objdump（m_379 hackTradeGold 64 位累加）
//   9. _ZNK5CUser24getTotalHackUserTradeCntEv         0x0853208c objdump（读 m_379.m_hackTradeCnt）
//  10. _ZNK5CUser25getTotalHackUserTradeGoldEv        0x08532078 objdump（edx:eax 读 lo/hi）
//  11. _ZNK5CUser25getCharacAntibotSerialNumEv        0x0823089c objdump（读 m_field8ec24）
//  12. _ZN5CUser18sendSecuRewardItemEP10Inven_Item25ENUM_SECURITY_REWARD_TYPEPi 0x0868833c CUser/sendSecuRewardItem.md
//  13. _ZN5CUser21SetAccountCargoDBDataEP10Inven_Itemjj 0x0867d86e CUser/SetAccountCargoDBData.md
//  14. _ZN5CUser25VerifyPresentAvengerTitleEv         0x0868b552 CUser/VerifyPresentAvengerTitle.md
//  15. _ZN5CUser9rewardExpEiRiS0_S0_S0_13eExpAddReasonb 0x0868b20c CUser/rewardExp.md
//  16. _ZN5CUser11gain_exp_spEiRiS0_13eExpAddReasonib 0x0866a3fe CUser/gain_exp_sp.md
//  17. _ZN5CUser22handleInformNoticeFlagEj            0x0868f956 CUser/handleInformNoticeFlag.md
//  18. _ZN5CUser20loadInformNoticeFlagEPc             0x0868f900 CUser/loadInformNoticeFlag.md
//  19. _ZN5CUser19getInformNoticeFlagEi               0x0868fa76 CUser/getInformNoticeFlag.md
//  20. _ZN5CUser24getInformNoticeFlagIndexERSt6vectorIhSaIhEE 0x0868fa8a 同上目录
//  21. _ZN5CUser20sendInformNoticeFlagEv              0x0868fb22 CUser/sendInformNoticeFlag.md
//  22. _ZN5CUser18SendEffectItemInfoEj                0x08659366 CUser/SendEffectItemInfo.md
//  23. _ZN5CUser18LoadEffectItemInfoEP20CONTINUOUS_ITEM_INFO 0x086590f0 CUser/LoadEffectItemInfo.md
//  24. _ZN5CUser20LoadCoolTimeItemInfoEP20CONTINUOUS_ITEM_INFO 0x08658ff6 CUser/LoadCoolTimeItemInfo.md
//  25. _ZN5CUser20SendCoolTimeItemInfoEj              0x086591fe CUser/SendCoolTimeItemInfo.md
//  支撑：_ZN10stLookOutsC1Ejj 0x0869380e、_ZN12tyIPCountersC1Ejj 0x08693824
//        （ORIG W ctor 仅置 +0/+4；声明位于 CUser.h，W 域批次未产出时由本 TU 定义）
//
// 本 TU 独占/已广播的共享头改动：
//   CUser.h      — stLookOuts/tyIPCounters 拆 pad+ctor 声明；+0x8e101 区拆
//                  m_field8e05c/m_field8e068；SendEffectItemInfo→int；
//                  追加 LoadEffectItemInfo 声明（均 sizeof 不变）
//   CMCAPManager.h — m_pad560 → bool m_hackIpLogged（sizeof 不变）
//   CEnvironment.h — m_llsKey[0x2d] → {0x28 键体, int m_field1a8, 1 pad}
//   新增头 Game/CStatisticMgr.h — s_statisticMgr 视图（+0x00 子对象实证）
// ============================================================================

#include "CUser.h"
#include "Inven_Item.h"
#include "CDataManager.h"                    // G_CDataManager / getItemList(+0x0c)
#include "GlobalData.h"                      // s_systemTime_/s_capchaDataManager/
                                             // g_autoPunishRuleHackTypeMgr/s_guild_proxy_mgr/s_monitor_proxy_mgr/s_statisticMgr
#include "CStatisticMgr.h"                   // 本 TU 新建权威视图
#include "CGuildServerProxy.h"               // SendCharLevelGrowType
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"                 // GetServerProxy<T> 完整模板
#include "CInventory.h"                      // insertItemIntoInventory / md5_* 声明（md5_context 兼容视图）
#include "GameWorld.h"
#include "TimerQueue.h"                      // TIMER_MESSAGE/InsertTimer/G_TimerQueue
#include "GameTypes.h"                       // cMyTrace
#include "Divide_C_ClassIP_EndIP.h"
#include "CUserCharacInfo.h"                 // ContinuousItemInfo/CSystemTime 等
#include "CSystemTime.h"
#include "CCapchaDataManager.h"              // WongWork::CCapchaDataManager + MSG_UDP_CAPCHA_CERTIFY
#include "CMailBoxHelper.h"                  // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"         // g_scriptStringManager_/findString(4,key,0)
#include "Arad_DataManager.h"                // ARAD::Singleton<Arad_DataManager>::Get/findGameScript
#include "Arad_EventPeriodDataManager.h"     // isApplied/checkCreateTime（producer 批提供定义）
#include "CUserProc.h"                       // APSystem::CUserProc::ClearActionAndSendtoUser
#include "CLevelDungeonPlayStatistic.h"
#include "CExpertJob.h"                      // expert_job::CExpertJob::IncreaseExpertJobExp(static)
#include "SkillSlot.h"                      // SkillSlot::get_remain_*_at_index
#include "CParty.h"                         // send_to_party（GetParty 完整类型）
#include "W6_HackPunishDeps.h"               // CAutoPunishRuleHackTypeMgr::checkTotalTradeRuleOver

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// 外部 DB 请求类最小声明区（owner=DB 域批次，定义不在本批）；
// namespace/class 形态与 ORIG mangled 对齐，均 nm 实证。
// ---------------------------------------------------------------------------
// b28d-solo-W8_CU_G：权威声明见 W7LinkDB.cpp:28/44 与 CUserOnlyOrig.cpp:93 ——
// 两者均 **static void**。ORIG 调用点 0x0867fc32/0x0867fcfb 的 (%esp) 是第 1 实参
// （get_acc_id()），非 this 槽 ⇒ 静态调用（P88）；原非静态声明多压临时对象 this，
// 使全部实参右移 4B。
struct DB_AutoPunishedSecondUserData {       // ORIG makeRequest T 0x0843a790 _ZN...makeRequestEjyjj
    static void makeRequest(unsigned int accId, unsigned long long gold,
                            unsigned int uid, unsigned int who);
};
struct DB_InsertAutoPunishSecondUser {       // ORIG makeRequest T 0x0842e34e _ZN...makeRequestEjjyb
    static void makeRequest(unsigned int accId, unsigned int who,
                            unsigned long long gold, bool punishFlag);
};
// 其余五类同 W7LinkDB.cpp:36/50/60/66/72 权威声明：均 static void（原 int 非静态
// 声明使调用点多压临时对象 this + 返回值形态错，P88）。
struct DB_HackIPSave {                       // ORIG makeRequest T 0x08436ff8 _ZN...makeRequestEPKcS1_jj
    static void makeRequest(const char* startIp, const char* rawIp,
                            unsigned int a, unsigned int b);
};
struct DB_InsertAutoPunishFirstUser {        // ORIG makeRequest T 0x0842dd7c _ZN...makeRequestEjPKcjjjjtj
    static void makeRequest(unsigned int accId, const char* ip,
                            unsigned int p1, unsigned int p2, unsigned int p3,
                            unsigned int p4, unsigned short type, unsigned int p6);
};
struct DB_ForceSaveSimpleSSO {               // ORIG makeRequest T 0x084281ae _ZN...makeRequestEji
    static void makeRequest(unsigned int accId, int errCnt);
};
struct DB_CountOfHumanCertify {              // ORIG makeRequest T 0x084283ca _ZN...makeRequestEj
    static void makeRequest(unsigned int accId);
};
struct DB_SaveGiveAvengerTitleFlag {         // ORIG makeRequest T 0x084422dc _ZN...makeRequestEijj
    static void makeRequest(int uid, unsigned int accId, unsigned int characNo);
};


// ===========================================================================
// 支撑小类 ctor（ORIG W，仅写前两个 uint；objdump 0x869380e/0x8693824 直转写）
// ===========================================================================
#include "W8_CU_G/stLookOuts.inc"
#include "W8_CU_G/tyIPCounters.inc"
#include "W8_CU_G/getTotalHackUserTradeGold.inc"
}

#include "W8_CU_G/getTotalHackUserTradeCnt.inc"
}

#include "W8_CU_G/incTotalHackUserTradeCnt.inc"
}

#include "W8_CU_G/incTotalHackUserTradeGold.inc"
}

#include "W8_CU_G/getCharacAntibotSerialNum.inc"
}

// ORIG 0x0867fbba：已 loading 时累加并按规则判定惩罚入库；否则置 loading 标志
// 并记首条未加载流水（DB_AutoPunishedSecondUserData::makeRequest(accId,gold,uid,who)）。
// b28d-solo-W8_CU_G: DIFF/N7 封顶（100/101）— 本波修 5 处：
// ①真 ABI bug：两个 DB 请求类局部声明非静态（多压临时对象 this，实参整体右移 4B）
//   ⇒ 改 `static void`（权威声明 W7LinkDB.cpp:28/44、CUserOnlyOrig.cpp:93；ORIG
//   调用点 0x0867fc32/0x0867fcfb 的 (%esp) 为第 1 实参 get_acc_id()，P88）；
// ②真语义 bug：末尾 DB_InsertAutoPunishSecondUser::makeRequest 第 3 参原写 totalNow，
//   ORIG 0x0867fce6 读 -0x10/-0xc = 形参 gold ⇒ 改传 gold；
// ③isLoadingHackGold uchar→bool（P292，Main 代落 CUser.h:2695/CUser.cpp:5017）⇒
//   首条件 `xor $1; test; je` 复现 0x0867fbe5；
// ④去 totalNow/cntNow/punish8 具名局部（ORIG 全程 ebx:esi/eax 寄存器直传，P0）⇒
//   帧 0x54→0x30，spill 槽 -0x10/-0xc/-0x18/-0x14 与 ORIG 全等；
// ⑤`gold + totalGold` 交换书写序复现 ORIG 0x0867fc3f 先载 totalGold 入 eax:edx。
// 余 1 条：this=全局指针 g_autoPunishRuleHackTypeMgr 装载 ORIG `mov g,%edx` 直落
// vs 4.4 `mov %eax,%edx; mov g,%eax`（N7 寄存器角色互换）；b28d-W8b 复测去
// `static_cast<unsigned int>`（隐式 int→unsigned 实参旋钮）同 101 ⇒ 封 N。
#include "W8_CU_G/gainMoneyFromHackUser.inc"
}

// ORIG 0x0867f748：黑名单/惩罚注册入口。
// type==2 lookOuts1；type==3 IP 监视登记（一次性）；type==10 ARS 延迟惩罚定时器；
// 其它走即时 OnPunishUser + 0x81 定时器；尾部对非惩罚状态账号补发首犯入库包。
// b28d-W8(2026-09-09): **AE 276/276**（TU 四数 diff−1）— 本波 6 修：
// ①P227 全表达式临时链 + 删 notifyType 复制槽（直接用形参 type，尾部 `type=10`）；
// ②startIp/rawIp 16 字节（ORIG 各 4×movl，原 17 字节多 2 条 movb）；
// ③声明序 rawIp 先（0x867f832 组）→ GetPeerIP2 → startIp（0x867f86e 组）；
// ④P133 四条独立 `if(type==7/9/12/5)`（合并短路形产 4×je 同落点）；
// ⑤P59 守卫 `if(!m_hackIpLogged){body;return}else return`（正写少 1 条 xor）；
// ⑥ip[16]={0} + GetPeerIP2 返回值直传 makeRequest arg1（P0）。
// ⑦**真语义 bug**：IPCounterLog_InsertIp 第三实参原传 startIp，ORIG 0x867f8ee
//   `lea -0x58(%ebp),%ecx` 传的是 rawIp（GetPeerIP2 目标缓冲）⇒ 记录的是原始 IP。
// p167 CLEAR(31/31)，gap 0，语义完成。ORIG 0x0867f748。
#include "W8_CU_G/setHackUser.inc"
}

// ORIG 0x084fa298：对角色列表首元素起 size()*0x14d1 字节做 MD5 摘要。
// b28d-solo-W8_CU_G: NEAR/N95 封顶 — ORIG 0x084fa298 md5_context 槽基 -0xe0 vs
// 重建 -0xe8：4.4-O0 局部槽基恒从 ebp-8 起（帧顶留 8B gap），4.1 大局部可顶到 ebp
//（N95 定性；double framePad 反致下移、int pad/声明序调整不可上移）。帧同 0xf4、
// 指令/助记符逐条全等，仅 3 处 lea disp 差（32/32）。语义等价。
#include "W8_CU_G/getHashCode.inc"
}

// ORIG 0x0867f4c8：CleanPad 人机认证应答比对（验证码头 5×ushort 逐项日志后 memcmp）。
// b28d-solo-W8_CU_G: DIFF/N 封顶（160/160，ndiff 148→37）— 本波修 3 处：
// ①P227 全表达式临时链 `cMyTrace(f,l,v)(...)`：ORIG 0x0867f4d1..0x0867f598 在 ctor
//   前右起求值全部 varargs + NumberToString（具名 trace + 语句形恒 ctor 先）；
// ②CUser.h:2445 匿名 union 双视角 `m_capchaCertifyW[5]`（P235，Main 代落）⇒ 5 处
//   读取由 add+movzwl 三步折回 ORIG 单条 `movzwl 0x8e0fxx(%eax),%eax`；
// ③varargs 中 orig 组实参顺序改为 [0..4]（ORIG 压栈序 0xc/%esi=orig[0] … 0x1c/-0x34
//   =orig[4]，P23/压栈序判读）⇒ 求值序 [4]→[0] 与 ORIG 全等。
// b28d-W8b: **DIFF→AE**（160/160）— 块布局判据（P47/P59）：ORIG 0x0867f62e
// `test %eax,%eax; je +0x203` 把成功块尾置、失败块直落；负极性 `==0` 写法产
// `jne <失败块>` + 成功块直落（指令数相同但 jcc/块位反）。翻转 if 顺序后逐条对齐。
// 余：@0x0867f5e7 字符串地址归一化伪差（§6.3）。15/15 调用点全等，语义完成。
#include "W8_CU_G/checkHumanCertify.inc"
}

// ORIG 0x08649b44：验证三段 CleanPad 种子和是否等于上报校验值。
#include "W8_CU_G/VerifyCleanPadVeriData.inc"
}

// ORIG 0x0868a51a：向客户端下发人机认证图（capcha 队列随机一条 + 新鲜种子三段
// + 错误计数），并重置请求态/失败标志与 60 秒认证超时定时器（TIMER_MESSAGE 9）。
// [b22判词·N] ORIG 155 vs 重建 154：唯一真差异在 memcpy 首参（dst = this+0x8e0ec，
// m_capchaCertify 数组）地址物化——ORIG 4.1.2 `mov 0x8(%ebp),%edx; add $0x8e0ec,%edx`
// （mov+add 两条），4.4.7 折叠为 `lea 0x8e0ec(%eax),%edx`（单条），差 1 条属跨版本
// 寻址伪影（范式库 memcpy/memset 首参 lea-vs-add 族，禁 register asm 强凑）。其余指令/
// 控制流/栈槽/EH landing pad 全对齐；call 目标占位、数据地址 `movl $0x0,(%esp)`
// （&s_capchaDataManager）、字符串立即数为 --no-compile .o 重定位假阳性（§6.3），
// relink 后转 AE。语义等价。
// b28d-W8(2026-09-09): DIFF 155/154（gap 1）— 本波修**真 bug**（p167 重载选错）：
// `put_binary(reinterpret_cast<char*>(m_image), size)` 选中 `EPci` 非 const 重载；
// ORIG 调 `_ZN18InterfacePacketBuf10put_binaryEPKci`（const char*）⇒ 改
// `reinterpret_cast<const char*>`，p167 转 CLEAR。余 1 条 N 族：
// ①memcpy 首参 dst(this+0x8e0ec) ORIG `mov;add` 两步 vs 4.4 `lea` 单条（N107 负例）；
//    b28d-W8b 复测 `&m_capchaCertify[0]` / `reinterpret_cast<char*>(&数组)` 两形
//    仍产 lea（154 不变）⇒ 封 N；
// ②+0x8e0f8 成员读 ORIG `mov mem,%eax; mov %eax,%esi` 两段 vs 4.4 单段（N18a）；
//    b28d-W8b 复测实参 `static_cast<unsigned int>(m_humanCertifyTimerKey)` 形
//    无效（154 不变）⇒ 封 N。
#include "W8_CU_G/reqSendMailCertify.inc"
}

// ===========================================================================
// 安全奖励物品发放（sendSecuRewardItem）
// ORIG 0x0868833c：交易中或背包满 → 系统邮件退还；否则入背包并回填落位。
// ===========================================================================
// b28d-W8(2026-09-09): **NEAR 149/149**（DIFF→NEAR）— 本波 5 修：
// ①返回类型 void→bool（CUser.h:2976 同步，P159 尾 `mov $0/%1,%eax`）；
// ②语句序照 ORIG：先拷贝 Inven_Item 局部（0x8688347）再判空（0x86883aa，P171 照抄）；
// ③body 0x100（非 0xff）⇒ 帧/槽位与 ORIG 只差 1B；failed 改 char（`cmpb $0` 直比）；
// ④CheckInTrade 正向 `if(...)` + else 成功路径（`test %al,%al; je` 无 xor）；
// ⑤P47 块序：`if(failed){失败; return false}` + 成功 fall-through；ReqDBSend 内联实参。
// b28d-W8b: **NEAR→AE**（149/149）— title 数组 0x14→0x15（ORIG -0x22..-0xe 共
// 0x15 字节 = strncpy 长度 0x14 + 终止符）⇒ body 起始精确落 -0x15f，1B 槽平移消失。
// p167 CLEAR(9=9)，gap 0。ORIG 0x0868833c。
#include "W8_CU_G/sendSecuRewardItem.inc"
}

// ORIG 0x0867d86e：账务仓库 DB 同步转发给内嵌 CAccountCargo。
#include "W8_CU_G/SetAccountCargoDBData.inc"
}

// ===========================================================================
// Avenger 计划（复仇者转职前礼物）事件
// ORIG 0x0868b552：事件码 0x8e + 创建时间窗判定，job==4 且未成长一级角色首次
// 登录 → 发放 0x28d3af 物品邮件并置 flag/DB 记录。
// ===========================================================================
// b28d-W8(2026-09-09): **gap 2→0（340/340）** — 本波 5 修：
// ①P0/P183 删 period/period2 具名局部（findGameScript 结果 %eax 直传 receiver，
//   省 2 对 store/reload）；
// ②P28 声明序 body(0x100)先于 subject(0x15)（清零块序照 ORIG 0x867b7e2/82c）；
// ③P52/P134 ReqDBSendNewSystemMail 与 makeRequest 三值内联实参（%ebx/%esi/%eax
//   跨调用保活，删 6 对槽往返）；
// ④P227 trace 全表达式临时链 + itemIdx 去 const（复现 0x868b9ae 槽读 %edi）；
// ⑤getFlagPresentAvengerTitle 返回 uchar→bool（CUser.h:3240/CUser.cpp:11353，
//   SweepCUser2 代落）+ `!f()` ⇒ 0x868b68b `xor $1` 复现。
// p167 CLEAR（仅 C1/C2 别名）；余 isApplied 段 1 条 xor（P168a③b 跨版本 bool ABI）
// + 槽号整体平移。ORIG 0x0868b552。
#include "W8_CU_G/VerifyPresentAvengerTitle.inc"
}

// ===========================================================================
// 经验主链
// ===========================================================================
// ORIG 0x0868b20c：结算前后 SP 快照 + gain_exp_sp；升级后按场景广播基础信息包，
// 成长事件触发 decide_growth_power_reward_system（producer=CUserOnlyOrig.cpp）。
#include "W8_CU_G/rewardExp.inc"
}

// ORIG 0x0866a3fe：经验结算主流程 —— 入口节流（环境 workmode==2 时 49 级以上不
// 入经验）、累计字段、升级判定（_check_level_up，producer=CUserCheckLevel.cpp）、
// SP/SFP 返还与日志、升级后任务/公会/监控器通知、疲劳奖励 channel 组收尾、
// 专家职业经验结算与 55 级清动作点。
// b28d-W8(2026-09-09): DIFF 388/389（gap 4→1）— 本波 4 修：
// ①P0/P183 去 questR 具名局部（ORIG 0x0866a5xx getCurCharacQuestR 结果 %eax 直传
//   get_quest_info receiver，省 store+load 2 条）；
// ②guild/monitor 两段 getCurCharacGrowType 加 uchar→char 转换复现 ORIG
//   0x0866a67d/0x0866a6fb `movsbl %al,%eax`（具名局部形 396 反劣已弃）；
// ③P227 全表达式临时链复现 VerifyPresentAvengerTitle 失败分支 trace
//   （0x0866a77b..0x0866a7e0 ctor 前右起求值 characNo→NumberToString）；
// ④VerifyPresentAvengerTitle 返回 char→bool（CUser.h:3065 同步）⇒ 调用点
//   `call; xor $1; test; je` 与 ORIG 0x0866a774 全等。
// b28d-W8b: **DIFF→AE**（388/388）— 帧 0x7c→0x6c 精确对齐：①P329 去
// playCount/level/expertExp 三具名局部（ORIG 0x0866a8a0 段全程寄存器/实参位直传）；
// ②P184/P171 补第二个独立 `if (levelUp)`（ORIG 0x0866a96c `je +0x56e` 与检查 A
// 的 `je +0x522` 分离）+ 两条死局部常量 `movl $0x55,-0x20; movl $0x50,-0x1c`。
// 66/66 调用点全等（p167 CLEAR）。ORIG 0x0866a3fe。
#include "W8_CU_G/gain_exp_sp.inc"
}

// ===========================================================================
// Inform notice flag 族（m_informNoticeFlags 16 字节 × 8 bit 标志板）
// ===========================================================================
#include "W8_CU_G/getInformNoticeFlag.inc"
}

// b28d-solo-W8_CU_G: DIFF/N 封顶 — ORIG 0x0868f956 三失败出口各落独立 nop 岛
//（+0x83 nop;jmp / +0x86 nop;jmp / +0x89 nop，标签序=守卫序）⇒ P223 负条件
// goto 连续尾标签形复现三岛（探针同 CCharacInformNotice::handleInformNoticeFlag
// 同族定案 0x080e4806）。残差 N：①ORIG 读值/和值各多一条寄存器中转
//（`movzbl→%edx; mov %edx,%ebx` / `lea→%edx; mov %edx,%ecx`），4.4 恒折单段
//（N18a 正向，字段/RHS 均已异型仍不可造）；②第三守卫 ORIG `test %al,%al; je`
// vs 4.4 `xor $0x1; test; jne`（P168a①/P292 bool 归一，无绑定不可造）。
// 55/54，语义等价。
#include "W8_CU_G/handleInformNoticeFlag.inc"
}

// b28d-solo-W8_CU_G: NEAR/N — P285 空 else{return;} 复现 ORIG 0x0868f900 尾
// `jmp;nop` 岛（ndiff 13→3）。b28d-W8 复验：28/29（ORIG 28），残 1 条 = 循环体
// ORIG 单段 `movzbl (%edx),%ecx`，4.4 恒 `movzbl (%edx),%edx; mov %edx,%ecx` 两段
//（N18a 反向）；b28d-W8b 复测三形（隐式转换 / 显式 (unsigned char) cast /
// P266 `*reinterpret_cast<unsigned char*>(src + i)`）同为 29 ⇒ 封 N，不再试形。
#include "W8_CU_G/loadInformNoticeFlag.inc"
}

#include "W8_CU_G/getInformNoticeFlagIndex.inc"
}

#include "W8_CU_G/sendInformNoticeFlag.inc"
}

// ===========================================================================
// 连续效果道具（effect / cooltime）载入与下发
// ===========================================================================
// ORIG 0x08659366：0xae 包头 + 数量 + 逐条目{idx, expire-now}下发。
// b28d-solo-W8_CU_G: IDENTICAL/AE — ①循环体恢复两个具名局部（ORIG 0x086593de/
// 0x086593f5 两次 operator[] 结果落 -0x10/-0xc 槽，P0 槽往返判据），帧/PacketGuard
// 槽随之回到 -0x28；②循环变量 unsigned→int（P218 扩展：4.4 尾测由 `mov i,%edx;
// cmp %eax,%edx; setl` 折回 ORIG `movzbl count,%eax; cmp i,%eax; setg`）；
// ③返回类型 int→bool（P248 档①，Main 代落 CUser.h:2835）⇒ `mov %eax,%ebx` 直传。
// 90/90 逐条 EQ。
#include "W8_CU_G/SendEffectItemInfo.inc"
}

// ORIG 0x086591fe：0xac 包头 + cooltime 表项迭代下发。
// b28d-solo-W8_CU_G: IDENTICAL/AE — ①`it = list->begin()` 提前到 size()/put_byte
// 之前（ORIG 0x0865923a begin 的 sret 直落 -0x30 it 槽）；②循环体恢复两个具名
// 局部（ORIG 0x0865927d/0x0865928d 两次 operator-> 结果落 -0x10/-0xc 槽）；
// ③返回类型 unsigned int→bool（P248 档①，Main 代落 CUser.h:3574）。帧 0x40、
// 槽位（-0x2c guard/-0x30 it/-0x18 list/-0x11 count）与 ORIG 全等，106/106 EQ
//（ORIG 尾 1 条对齐 nop，§6.1 豁免）。
#include "W8_CU_G/SendCoolTimeItemInfo.inc"
}

// ORIG 0x086590f0：DB 载入 effect 连用表 → 过滤有效条目 → 特效回调 → 清理无效并发送。
// b28d-solo-W8_CU_G: AE — ①P183（删 `const CONT_ENTRY& entry` 缓存引用，每字段重发
// `mov i,%edx; mov info,%eax; mov 0x8(%eax,%edx,8),%eax` 全链）；②守卫「1 直落 + 2 岛」
// 用 P223 多岛形：正向 if 包裹第一守卫（jbe 直跳循环尾）+ 第二/三守卫独立 goto 连续尾
// 标签（L2/L3/NEXT），ORIG 0x8659097/0x865909a 两岛逐条复现；③尾 `!=1` 改 bool 返回
// 旋钮（P248 档①：SendEffectItemInfo int→bool，Main 已代落 CUser.h）⇒ `xor $1; test; je`
// 复现 0x86591e9。81/81，compare_tu 判 AE。
#include "W8_CU_G/LoadEffectItemInfo.inc"
}

// ORIG 0x08658ff6：DB 载入 cooltime 表 → 过滤 → 清理无效 → 下发 0xac 包。
// b28d-solo-W8_CU_G: AE — 同 LoadEffectItemInfo：P183 去缓存引用 + P223 多岛形
//（ORIG 0x08659097/0x0865909a 两岛）+ SendCoolTimeItemInfo bool 返回旋钮
//（P248 档①，Main 代落 CUser.h:3574）⇒ `xor $1; test; je` 复现 0x086590da。
// 76/76 指令（ORIG 尾多 1 条对齐 nop，§6.1 豁免），compare_tu 判 AE。
#include "W8_CU_G/LoadCoolTimeItemInfo.inc"
}

