// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_A.cpp — game_master 命令体系批次 A（df_game_r ORIG T 符号族还原）。
// 负责类（11）：
//   CAutoMarketConditionsControlCmd ctor 0x082a8060 / execute 0x084a8ffc（空）
//   CAssaultAbuseUserCmd         ctor 0x082a8212 / execute 0x084a9daa（空）
//   CBloodMonsterListCmd         ctor 0x082a824a / execute 0x084a9e34
//   CAddItemPollutionCmd         ctor 0x082a82e4 / parse 0x084aa9c8 / execute 0x084aaa00
//   CAddItemByMail               ctor 0x082a853e / parse 0x084ac2aa / execute 0x084ac2e2
//   CAdjustCharacStat            ctor 0x082a855a / execute 0x084ac58c
//   CBoosterTest                 ctor 0x082a863a / parse 0x084aeb5e / execute 0x084aed04
//                                + 辅助 14 函数（getRarityStr/SendChatMsg/printServer/
//                                  printClient/getBoosterResult/getCompoundEmblemResult/
//                                  getJarResult/getRandomboxResult/get_gold_lottery_result/
//                                  is_gold_lottery_item/_selectJewelTypeIndex/
//                                  makeFileContents/make_file_gold_contents/clear）
//   CBindCubeTest                ctor 0x082a8672 / parse 0x084acbe4 / execute 0x084acc1e
//   BlueMarbleSet                ctor 0x082a88ae / parse 0x084b22ca / getParam 0x084b235c /
//                                execute 0x084b25a4
//   CBingoGmTest                 ctor 0x082a893a / parse 0x084b35c4 / execute 0x084b35fc /
//                                resetData 0x084b363e / reqValue 0x084b368a
// 对照 docs/class_func_reports/game_master__<Class>/{README,parse,execute,...}.md；
// 报告缺失的 CBoosterTest::clear 由 objdump 0x084b40b0 直转。
//
// 头文件占位字段失配说明（GameMasterCmd.h 本批禁改）：占位 m_str0(string@+0x08)
// 与下列真实标量布局同尺寸（gcc4.4 32 位 std::string 仅 4 字节），本 TU 经偏移访问：
//   CAddItemByMail/CAddItemPollutionCmd/CBingoGmTest  +0x08 int
//   CBoosterTest                                      +0x08 int +0x0c int
//   CBindCubeTest                                     +0x08 byte（signed 使用）
//   BlueMarbleSet                                     +0x08 byte +0x09 byte +0x0a byte
//                                                     +0x0c int（超出占位的部分同样偏移访问）
//   CBillingRechargeCommand                           m_str0(+0x08) 真为 string +
//                                                     +0x0c int（金额）
//
// 头文件辅助函数占位签名失配说明：GameMasterCmd.h 对各辅助统一给出无参 `void f();`
// 占位声明，但 ORIG 真实签名带参（见报告）。本批不能改头，故带参辅助一律以
// 自由函数 + asm 标签对齐 ORIG mangled 实现（32 位 O0 下非虚 thiscall 与自由函数
// ABI 一致：this 即首栈参；静态成员本就不含 this）。头中无参占位声明保持
// 未定义、未被调用，不产生代码与符号。
//   注：CBingoGmTest 的 +0x08 在头中为占位 string，ORIG 真实为 int 子命令码
//   （ctor 不初始化；本 TU 因头声明 string 成员而随编译器生成其构造，差异仅限
//   构造期清零，parse 后行为一致）。reqValue/resetData 按头中无参声明实现。

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "Inven_Item.h"
#include "CItemAmplifier.h"
#include "CMailBoxHelper.h"
#include "CHandlePremium.h"
#include "RDARScriptStringManager.h"
#include "CSystemTime.h"
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CInventory.h"
#include "CItemList.h"
#include "CStackableItem.h"
#include "CompoundEmblemDeps.h"
#include "BlueMarble.h"
#include "CCompoundAvatar.h"      // WongWork::CCompoundAvatar::ProcTest（桥清零）
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent（桥清零，权威头）
#include "CGameManager.h"         // CGameManager::GetParty / G_CGameManager（桥清零）
#include "TaiwanCash.h"           // WongWork::IPG::CIPGHelper 虚接口
                                  // （CBillingRechargeCommand::execute 真虚调用）

#include "DNFFunctionLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// ORIG 数据符号（ELF .rodata 指针表，仓库尚无声明）。
extern char* CHARACTER_JOB_NAMES[];
extern char* BIND_NAMES[];
extern char* RECIPE_NAMES[];
extern char* EQUIP_NAMES[];

// ---------------------------------------------------------------------------
// [b23S3C] P201 桥转真：BingoEvent/CBingo 改 include 权威头
// （BingoEvent.h：getBingoSystem/sendOneMatchLineReward；CBingoTest.h：
//  CBingo::calNewValue(unsigned int&, int)），reqValue 调用点
//  boost::shared_ptr<CBingo>::operator-> 真调用（ORIG +0x8f call 080ce766）。
// ---------------------------------------------------------------------------
#include "BingoEvent.h"
#include "CBingoTest.h"
#include <boost/shared_ptr.hpp>

namespace game_master
{

// ---- GM 命令对象参数区一律具名字段访问（§4.3 裸偏移清零，2026-09-06）：
// GameMasterCmd.h 已按 ORIG POD 落真实字段（CAddItemPollutionCmd/CAddItemByMail
// ::m_itemIdx、CBoosterTest::m_mode(+0x08)/m_count(+0x0c)、CBindCubeTest::m_flag、
// BlueMarbleSet::m_flag0/1/2(+0x08/09/0a)+m_param(+0x0c)、CBingoGmTest::m_subCmd），
// 原 cmd_int0/cmd_int1/cmd_byte0 三宏（内部为 char*+0x08/0x0c 裸偏移）已删除。


// ===========================================================================
// CAutoMarketConditionsControlCmd — ORIG W ctor 0x082a8060：基类 vptr → 自身
// vptr(0x8c81c58)，无字段（sizeof=0x8）。
// ===========================================================================

// ORIG execute 0x084a8ffc：ret（纯空实现）。
#include "GmCmds_A/execute.inc"
{
}

// ===========================================================================
// CAssaultAbuseUserCmd — ORIG W ctor 0x082a8212：基类 vptr → 自身 vptr(0x8c81ae0)。
// ===========================================================================

// ORIG execute 0x084a9daa：ret（纯空实现）。
#include "GmCmds_A/execute_1.inc"
{
}

// ===========================================================================
// CBloodMonsterListCmd — ORIG W ctor 0x082a824a：基类 vptr → 自身 vptr(0x8c81ab0)。
// ===========================================================================

// ORIG execute 0x084a9e34：仅地下城状态(state==5)且有队伍时，把当前地图怪物
// uid 表拼成文本经 0x0c 号包回发给 GM。
// [b28d-47] verdict=DIFF(routed) [N84/N82] ORIG 0x084a9e34 orig/ours=214/185
//   修：put_str const_cast 去除（P167 重载 Pci→PKci）。残余：EXTRA memset（我方
//   char buf[0xff] 零初始化走 call memset，ORIG 为内联 rep stos，N78/N84 族）+
//   MISSING GetUser ×1（ORIG 三处独立取 user，槽/调用点差）+ 槽平移。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=7 [N82/P23] p167 CLEAR(37/37)；begin/end
// 条件块 receiver 装载先于 sret（ORIG）vs 4.4 相反，`(*monsters)` 形实测不翻 ⇒ 封顶。
// [b28d-solo-2] verdict=AE：真缺口 = GetMapInfo() 结果未具名（ORIG +0x119
// `mov %eax,-0x1c` 槽往返，循环内每次 `mov -0x1c,%eax; lea 0xc(%eax),%edx`
// 重算 m_monsterMap 地址）。改 `MapInfo* mapInfo = party->m_battleField.GetMapInfo();`
// + 循环内 `mapInfo->m_monsterMap.begin()/end()` 后 orig/ours=214/214 全等。
#include "GmCmds_A/execute_2.inc"
{
    // [b28d-solo-GmCmds_A] P0/P167：ORIG 无 user 具名槽，3 处调用点各自重调
    // GetUser()（p167 MISSING GetUser×1）。
    if (GetUser()->get_state() != 5)
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P200：ORIG `call GetPartyIndex; movswl %ax,%ebx` 直传
    // GetParty 实参（无 partyIndex 具名槽）。
    CParty* party = G_CGameManager()->GetParty(
        static_cast<int>(GetUser()->GetPartyIndex()));
    if (party == 0)
    {
        return;
    }

    std::string message("monster list : ");
    // [b28d-solo-GmCmds_A] P312：ORIG +0x0ae 对 buf[0xff] 发「对齐剥离+rep stos
    // +尾字节」慢路（无 memset call，p167 EXTRA memset）⇒ 数组零初始化。
    char buf[0xff] = {0};

    // CBattle_Field 为 CParty+0xb24 嵌入子对象（CParty.h 占位，全项目同口径视图）。
    // ORIG 0x84a9e??：GetMapInfo() 返回 MapInfo*，取其 +0x0c m_monsterMap。
    // [b28d-solo-2] P0：ORIG +0x119 把 GetMapInfo() 结果存入具名槽 -0x1c
    // （`mov %eax,-0x1c`），循环内每次 `mov -0x1c,%eax; lea 0xc(%eax),%edx`
    // 重算 m_monsterMap 地址 ⇒ 原码是 MapInfo* 具名局部，而非缓存的 map 指针。
    MapInfo* mapInfo = party->m_battleField.GetMapInfo();

    // [b28d-solo-GmCmds_A] P23：ORIG 条件块先载 receiver（`mov -0x1c,%eax;
    // lea 0xc(%eax),%edx`）再 lea sret。
    for (std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.begin();
         it != mapInfo->m_monsterMap.end(); ++it)
    {
        sprintf(buf, "(%d) ", it->first);
        message += buf;
    }

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(message.size()));
    // [b28d-47] P167 重载选错修正：ORIG 调 `put_str(char const*, int)`（PKci），
    // const_cast 使其选 `put_str(char*, int)`（Pci）。
    packet.put_str(message.c_str(), static_cast<int>(message.size()));
    packet.finalize(true);
    GetUser()->Send(packet);
}

// ===========================================================================
// CAddItemPollutionCmd — ORIG W ctor 0x082a82e4：基类 vptr → 自身 vptr(0x8c81a20)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================

// ORIG parse 0x084aa9c8：trim(args)；+0x08 = atoi(args.c_str())。
#include "GmCmds_A/parse.inc"
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084aaa00：itemId==-1 先开通 PC 房特权；随后发放物品并在
// 无强化属性时随机投一次强化投资，最后日志 + 物品更新包。
// [b28d-47] verdict=DIFF(routed) [N82/P112] ORIG 0x084aaa00 orig/ours=187/180
//   残差：全帧槽号平移 + 寄存器角色差（跨版本分配）。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=40 [N82] p167 CLEAR(30/30)；limit/stackable/
// space/invest 槽位差 16，语义完成（嵌套 if 拆 && 物化、GetUser 内联、死守卫照抄）。
// [b28d-solo-2] verdict=DIFF：真缺口 = ①三处早退岛未复现（ORIG +0x2aa nop;jmp /
// +0x2ad nop;jmp / +0x2b0 nop 三岛 + 正常路径尾 jmp，共 7 条）⇒ 负条件 goto 尾
// 连续空标签（P223 多岛形，标签序=守卫序）；②SendUpdateItemList 必须在第二个
// slot 死守卫**之后**（ORIG `js +0x2b0` 命中即返回）；③trace 改全表达式临时链
// （P227/P328⑩，ORIG 三串实参先于 cMyTrace ctor 求值）。修后 orig/ours=187/187，
// 残余 2 条：investAmplifyOption 实参装载序（ORIG 右→左逐个压栈、4.4 先物化 arg0
// 到 %edx，P75 跨版本，探针 p3/p4 五形不翻）。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%edx; mov -0x18(%ebp),%edx;
//   0xc(%esp)…` vs 4.4.7 `%ecx/%edx` 角色互换（间接 call 目标寄存器族，P230 补）。
// [round5-A20 复核·N126] fresh .o 184/184、判据⑪ PURE SHIFT c=0x0（frame 96=96、
//   槽集全等）⇒ 残差纯为 N7 间接调用目标寄存器角色（含 investAmplifyOption 实参
//   装载序，P75）⇒ 与 CAddItemByMail 同族封顶。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 唯一串 `"%s:'%s' GM Mode Add Item(%d)."`
//   （0x084aac6c，实参 0x4(%esp)）与源码全表达式 trace 链一致（变参序
//   NumberToString→getCurCharacName→slot 对齐报告 +0x26c 段）；§6.5.1 enc 0 命中。
#include "GmCmds_A/execute_3.inc"
{
    if (m_itemIdx == -1)
    {
        // ORIG：getCurSec→lea +0x15180 直传，start/end 无栈槽（右值内联）
        WongWork::CHandlePremium::handleSetUserPCRoom(
            GetUser(),
            static_cast<long>(GlobalData::s_systemTime_.getCurSec()),
            static_cast<long>(GlobalData::s_systemTime_.getCurSec()) + 0x15180);
    }

    int limit = 100;
    CStackableItem* stackable =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(m_itemIdx));
    // [b28d-solo-GmCmds_A] P133/P225：ORIG 各条件各自 `je` 直跳统一出口；
    // 合并 `&&` 使 4.4 物化 bool（mov $1/$0 + test）⇒ 拆嵌套 if。
    if (stackable != 0)
    {
        if (stackable->is_stackable())
        {
            if (stackable->getStackableLimit() < limit)
            {
                limit = stackable->getStackableLimit();
            }
        }
    }

    ENUM_ITEMSPACE space;
    // [b28d-solo-GmCmds_A] P0/P167：ORIG 无 user 具名槽，6 处调用点各自重调
    // GetUser()（p167 MISSING GetUser×4）。
    int slot = GetUser()->AddItem(m_itemIdx, limit, static_cast<eItemAddReason>(6),
                                  space, 0);
    // [b28d-solo-2] P223 多岛形：ORIG 三处早退各落独立 nop 岛
    // （+0x2aa nop;jmp / +0x2ad nop;jmp / +0x2b0 nop），4.4 复现形 =
    // 负条件 goto 到尾部连续空标签（标签序=守卫序），正常路径尾 jmp 自动生成。
    if (slot < 0) goto L1;

    {
        // [b28d-solo-GmCmds_A] P0：ORIG 无 invenType/inven 具名槽
        // （invenType 在 %ebx 直传、inven 在 %eax 直传）。
        Inven_Item* ref = GetUser()->getCurCharacInvenW()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot);
        if (ref == 0) goto L2;
        if (ref->m_amp.getAbilityType() == 0)
        {
            int type = get_rand_int(4) + 1;
            if (0 < type && type < 5)
            {
                stInvestAmplifyOption_t invest;
                invest.m_amount = type;
                // [b28d-solo-GmCmds_A] P184：ORIG +0x181 二次 `cmpl $0,stackable`
                // 死守卫照抄（P171 不写即少分支）。
                if (stackable != 0)
                {
                    GlobalData::s_itemAmplifier_->investAmplifyOption(stackable, *ref,
                                                                      invest);
                    unsigned char abilityType = 0;
                    unsigned short abilityValue = 0;
                    ref->m_amp.getValues(abilityType, abilityValue);
                    ref->m_amp.assign(abilityType | 0x80, abilityValue);
                }
            }
        }
        // [b28d-solo-2] P184：ORIG +0x1f6 二次 `cmpl $0,slot; js` 死守卫，
        // 命中即返回（跳过 trace 与 SendUpdateItemList）⇒ 早退 goto 第三岛。
        if (slot < 0) goto L3;
        // [b28d-solo-2] P227/P328⑩：ORIG 三串实参在 cMyTrace ctor 之前求值
        // （getCurCharacName→%esi、NumberToString→%ebx，其后才 C1）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x5b0, 0)("%s:'%s' GM Mode Add Item(%d).",
                                                NumberToString(GetUser()->get_acc_id(), 0),
                                                GetUser()->getCurCharacName(), slot);
        GetUser()->SendUpdateItemList(static_cast<CUser::eSendTarget>(1), space, slot);
    }
L1:;
L2:;
L3:;
}

// ===========================================================================
// CAddItemByMail — ORIG W ctor 0x082a853e：基类 vptr → 自身 vptr(0x8c81840)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================

// ORIG parse 0x084ac2aa：trim(args)；+0x08 = atoi(args.c_str())。
#include "GmCmds_A/parse_1.inc"
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084ac2e2：合法物品以系统邮件发放（0 号特例 count=1，1 号特例
// 金币条目手工填充，其余走 CItem::make_item 虚拷贝并附加 add_info=1）。
// [b28d-47] verdict=DIFF(routed) [N78/N82] ORIG 0x084ac2e2 orig/ours=187/132
//   残差：EXTRA memset ×2（我方数组零初始化 call vs ORIG 内联 rep stos，N78）
//   + 槽/寄存器分配跨版本差。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=9 [N7/N18a] p167 CLEAR(16/16)；仅剩
// 寄存器角色差（ORIG `mov -0x28,%ebx` vs 4.4 `mov -0x28,%edx`）与虚调用 receiver
// 装载序（N7 族），语义完成封顶。
// [b28d-solo-2] verdict=DIFF：真缺口 = itemId 预清零缺失。ORIG +0x03e 在数组清零
// **之前**发 `movl $0,-0x28`（itemId 预清）、+0x16f 才写 m_itemIdx ⇒ 补两句式
// `int itemId = 0; … itemId = m_itemIdx;`（P198①）后 orig/ours=187/187。
// 残余 2 条：`G_CDataManager()->find_item(itemId)` ORIG 先装实参到 %ebx 再调
// receiver，4.4 恒 receiver 先（P75/P23 跨版本求值序，探针 p2/p3 五形不翻）。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%edx; lea -0x82(%ebp),%eax;
//   mov %eax,0x4(%esp); mov -0x1c(%ebp),%eax; mov %eax,(%esp); call *%edx` vs 4.4.7 用 %eax 目标；
//   另 `mov -0x28(%ebp),%ebx` 保活（ORIG）vs 4.4.7 直读（P226/N18a）。
// [round5-A20 复核·N126] fresh .o 185/185、判据⑪ PURE SHIFT c=0x0（frame 428=428、
//   槽集全等）⇒ 残差纯为 N7 间接调用 receiver 寄存器角色（ORIG `mov (%eax),%edx;
//   …call *%edx` vs 4.4.7 全 %eax）+ `mov -0x28(%ebp),%ebx` 保活（P226/N18a）。
//   P230 实测 4.4.6 后端亦不达 ⇒ 属路由域，封顶。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：ORIG 两串 `"game_server_msg_144"`
//   （0x084ac3da）/`"game_server_msg_143"`（0x084ac418）与源码 findString(4,…) 逐条一致；
//   §6.5.1 enc 0 命中。
#include "GmCmds_A/execute_4.inc"
{
    CUser* user = GetUser();
    // [b28d-solo-GmCmds_A] ORIG +0x02c 把 find_item 结果存入具名槽 -0x2c
    // （`mov %eax,-0x2c; cmpl $0,-0x2c; je`），内联 test/sete 形不符；
    // 实参用 m_itemIdx(+8) 直读（无 itemId 槽，其槽 -0x28 在两次 strncpy 后写）。
    CItem* found = G_CDataManager()->find_item(m_itemIdx);
    if (found == 0)
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P312：ORIG 对 title[0x100](-0x182)/sender[0x15](-0x45)
    // 发内联对齐剥离清零（无 memset call，p167 EXTRA memset×2）。
    // [b28d-solo-2] P198①：ORIG +0x03e 在数组清零**之前**发 `movl $0,-0x28`
    // （itemId 预清），+0x16f 才写 m_itemIdx ⇒ 两句式（声明=0 + 后赋值）。
    int itemId = 0;
    char title[0x100] = {0};
    char sender[0x15] = {0};
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_144", 0),
            0xff);
    strncpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_143", 0),
            0x14);

    // [b28d-solo-GmCmds_A] ORIG 声明/初始化序：itemId(-0x28) → addInfo(-0x24)
    // → Inven_Item C1(-0x82) → count(-0x20)。
    itemId = m_itemIdx;
    int addInfo = 1;
    Inven_Item item;
    int count = 0;
    if (itemId == 0)
    {
        count = addInfo;
    }
    else if (itemId == 1)
    {
        item.m_addInfo = 1;          // +0x02
        item.m_field1 = 2;           // +0x01
        item.m_addInfo2 = addInfo;   // +0x07
        item.m_fieldb = 1;           // +0x0b
        item.ResetItemAttr();
    }
    else
    {
        CItem* proto = G_CDataManager()->find_item(itemId);
        if (proto == 0)
        {
            return;
        }
        item.m_addInfo = itemId;     // +0x02
        proto->make_item(item);      // ORIG vtbl[2]
        item.set_add_info(addInfo);
    }

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, static_cast<unsigned int>(count),
        static_cast<unsigned int>(user->getCurCharacNo()), title,
        static_cast<int>(strlen(title)), 0u, user->GetServerGroup(), false, false);
}

// ===========================================================================
// CAdjustCharacStat — ORIG W ctor 0x082a855a：基类 vptr → 自身 vptr(0x8c81828)。
// ===========================================================================

// ORIG execute 0x084ac58c：重算角色属性后回发 basic_info 包。
#include "GmCmds_A/execute_5.inc"
{
    CUser* user = GetUser();
    user->adjust_charac_stat();   // 真实定义 UserPartyLink4.cpp（T 0x08664766）

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 1);
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// CBoosterTest — ORIG W ctor 0x082a863a：基类 vptr → 自身 vptr(0x8c81750)。
// 布局：+0x08 int 物品索引（clear 置 -1）、+0x0c int 统计次数。
// ===========================================================================

// ORIG parse 0x084aeb5e：clear() 后 sscanf(args,"%d%d",&+0x08,&+0x0c)。
#include "GmCmds_A/parse_2.inc"
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d", &m_mode, &m_count);
}

// ORIG clear 0x084b40b0（报告缺失，objdump 直转）：+0x08=-1、+0x0c=0。
#include "GmCmds_A/clear.inc"
{
    m_mode = -1;
    m_count = 0;
}

// ORIG execute 0x084aed04：LOTTERY 类物品抽样统计并把结果文件内容分包回显。
// [b28d-47] verdict=DIFF(routed) [N78/N82] ORIG 0x084aed04 orig/ours=269/241
//   残差：EXTRA memset（N78 内联清零差）+ 全帧槽号平移。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=29 [N7/memset-edx] p167 CLEAR(33/33)；
// memset 第三实参 edx 中转 + 虚调用 receiver 装载序（N7）+ 跳表基址折叠。
// [b28d-solo-2] verdict=DIFF：orig/ours=269/273。残余（逐条核对）：①首 memset
// 实参序；②`if (!goldLottery)` ORIG `cmpb $0,-0x1a; je` vs 4.4 `movzbl; xor $1; test; je`
// （+2 条，P122/N122 封顶，探针 p17 g1..g6 全为 xor 物化）；③虚调用 receiver
// 寄存器角色（N7）；④跳表基址 `.rodata` 重定位（AE）。
// [b28e-solo-2] ①已修：同 CBindCubeTest::execute，ORIG 函数头 fileBuf 清零是
// 数组初始化形（size 经 %edx 中转），`char fileBuf[95000] = {0};` 精确复现。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%edx; mov -0x20(%ebp),%eax;
//   mov %eax,(%esp); call *%edx` vs 4.4.7 `mov (%eax),%eax; … call *%eax`（residual 4 行）。
#include "GmCmds_A/execute_6.inc"
{
    CUser* user = GetUser();

    // [b28d-solo-GmCmds_A] P312：ORIG 对 fileName[0xff](-0x153) 发内联对齐剥离
    // 清零（无 memset call，p167 EXTRA memset）。
    char fileName[0xff] = {0};
    char fileBuf[95000] = {0};

    static char sMsgStart[] = "LOTTERY \xbe\xc6\xc0\xcc\xc5\xdb \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb8\xa6 \xbd\xc3\xc0\xdb\xc7\xd5\xb4\xcf\xb4\xd9";
    CBoosterTest::SendChatMsg(user, sMsgStart);

    if (m_mode == -1)
    {
        static char sNeedIdx[] = "\xbe\xc6\xc0\xcc\xc5\xdb \xc0\xce\xb5\xa6\xbd\xba\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xbc\xbf\xe4";
        CBoosterTest::SendChatMsg(user, sNeedIdx);
        return;
    }

    CItem* item = G_CDataManager()->find_item(m_mode);
    if (item == 0)
    {
        static char sNoItem[] = "\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9";
        CBoosterTest::SendChatMsg(user, sNoItem);
        return;
    }

    if (m_count > 1000000 || m_count < 0)
    {
        static char sRange[] = "0 ~ 1000000\xb9\xf8 \xc5\xeb\xb0\xe8\xb1\xee\xc1\xf6\xb8\xb8 \xc1\xf6\xbf\xf8\xb5\xcb\xb4\xcf\xb4\xd9";
        CBoosterTest::SendChatMsg(user, sRange);
        return;
    }
    if (m_count == 0)
    {
        m_count = 10000;
    }

    char countMsg[21];
    sprintf(countMsg, "%d\xb9\xf8 \xc5\xeb\xb0\xe8\xc0\xd4\xb4\xcf\xb4\xd9", m_count);
    CBoosterTest::SendChatMsg(user, countMsg);

    bool goldLottery = this->is_gold_lottery_item(item->get_index());

    std::map<int, int> result;
    switch (item->GetItemType())                       // ORIG vtbl[3]
    {
    // [b28e-solo-2] P324：ORIG 叶块物理序 = 0xf/0x10 → 6/7/0xd → 0x1a/0x1b
    // → 0x14 → default（ORIG 84aeedb/84aeee5/84aef38/84aef53/84aef82）⇒ 按
    // ORIG 块地址序重排 case（case 顺序不改语义，仅决定 .text 块序）。
    case 0xf:
    case 0x10:
        this->getBoosterResult(item, result);
        break;
    case 6:
    case 7:
    case 0xd:
        // [b28e-solo-2] ORIG 正向 `cmpb $0,-0x1a; je <getJar>`（4.4 对
        // `if (bool)` 直发 cmpb 2 条；`if (!bool)` 恒 xor 物化 3 条，N122）+
        // get_gold 块在前 ⇒ 写正极性。
        if (goldLottery)
        {
            this->get_gold_lottery_result(item, result);
        }
        else
        {
            this->getJarResult(item, result);
        }
        break;
    case 0x1a:
    case 0x1b:
        this->getRandomboxResult(item, result);
        break;
    case 0x14:
        if (!this->getCompoundEmblemResult(item, result))
        {
            static char sNotEmblem[] = "\xbf\xa5\xba\xed\xb7\xbd \xc7\xd5\xbc\xba \xb4\xeb\xbb\xf3\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.";
            CBoosterTest::SendChatMsg(GetUser(), sNotEmblem);
        }
        break;
    default:
    {
        static char sNotLottery[] = "LOTTERY \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9";
        CBoosterTest::SendChatMsg(GetUser(), sNotLottery);
        return;
    }
    }

    bool more = true;
    memcpy(fileBuf, "item_id\titem_name\trarity\titem_cnt\tprice\tsell_price\tusable_lv\n",
           0x3e);
    while (more)
    {
        // [b28e-solo-2] 同 switch：ORIG `cmpb $0,-0x1a; je <makeFileContents>`
        // 且 make_file_gold_contents 块在前 ⇒ 正极性。
        if (goldLottery)
        {
            CBoosterTest::make_file_gold_contents(user, fileBuf, result);
        }
        else
        {
            CBoosterTest::makeFileContents(user, fileBuf, result);
        }
        sprintf(fileName, "lottery_%d_%d.txt", m_mode, m_count);
        if (result.empty())
        {
            more = false;
        }
        CBoosterTest::printClient(user, more, fileName, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    static char sDone[] = "LOTTERY \xbe\xc6\xc0\xcc\xc5\xdb \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9";
    CBoosterTest::SendChatMsg(user, sDone);
}

// ---- CBoosterTest 辅助函数（ORIG 逐符号转写）------------------------------

// ORIG getRarityStr 0x084aea80：稀有度 → 文件列名。
#include "GmCmds_A/getRarityStr.inc"
{
    switch (rarity)
    {
    case RARITY_COMMON:    return "common";
    case RARITY_UNCOMMON:  return "uncommon";
    case RARITY_RARE:      return "rare";
    case RARITY_UNIQUE:    return "unique";
    case RARITY_EPIC:      return "epic";
    case RARITY_CHRONICLE: return "chronicle";
    default:               return "unknown";
    }
}

// ORIG SendChatMsg 0x084aeac6：make_packet_chat_msg(type=2,len=12) + Send。
#include "GmCmds_A/SendChatMsg.inc"
{
    PacketGuard packet;
    // ORIG：strlen 先求值物化 %ebx 跨 G_GameWorld call；gw 经 eax 直通 receiver，
    // 无具名 gw 槽（Ghidra C 顺序 strlen→G_GameWorld→make_packet_chat_msg）。
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, static_cast<int>(strlen(msg)), 2,
        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
}

// ORIG printServer 0x084aebae：fopen(path,"wt") + fputs + fclose。
#include "GmCmds_A/printServer.inc"
{
    FILE* fp = fopen(path, "wt");
    if (fp == 0)
    {
        goto done;
    }
    fputs(contents, fp);
    fclose(fp);
done:
    return;
}

// ORIG printClient 0x084aebf2：0x13e 包（more 标志 + 双字符串）。
#include "GmCmds_A/printClient.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(more);
    packet.put_int(static_cast<int>(strlen(name)));
    packet.put_str(name, static_cast<int>(strlen(name)));
    packet.put_int(static_cast<int>(strlen(contents)));
    packet.put_str(contents, static_cast<int>(strlen(contents)));
    packet.finalize(true);
    user->Send(packet);
}

// ORIG is_gold_lottery_item 0x084afccc：lotto 表项 id(+2)==0 且数量(+7)>0。
#include "GmCmds_A/is_gold_lottery_item.inc"
{
    Inven_Item lotto;
    G_CDataManager()->m_itemList->get_lotto_item(idx, reinterpret_cast<char*>(&lotto));
    return lotto.m_addInfo == 0 &&
           lotto.m_addInfo2 > 0;
}

// ORIG getJarResult 0x084afba8：统计 lotto 表产出物品次数。
// [b28d-47] verdict=DIFF(routed) [N7/N82] ORIG 0x084afba8 orig/ours=88/87
//   修：idx 实参内联（P0 去 -0xc 槽）+ `++pos->second`（单 ptEv）。
//   残余：callee-saved 角色（ORIG %esi 缓存 lotto vs 4.4 %ebx）+ 槽差（N7/N82）。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P167+P220+P61]
// m_itemList 直读 + 正极性 ne + make_pair<int,int> 值对 + 键重读 lotto.m_addInfo。
#include "GmCmds_A/getJarResult.inc"
{
    Inven_Item lotto;
    for (int i = 0; i < m_count; ++i)
    {
        // [b28d-47] P0：ORIG 0x084afbe8 `call get_index; mov %eax,%ebx`（无槽）⇒
        // 实参内联，禁具名 idx（4.4 落 -0xc 槽并使帧/槽整体漂移）。
        // [b28d-solo-GmCmds_A] P167：ORIG 直读 CDataManager::m_itemList(+0xc)
        // （`call G_CDataManager; mov 0xc(%eax),%eax`），getItemList() 是 EXTRA call。
        G_CDataManager()->m_itemList->get_lotto_item(
            const_cast<CItem*>(item)->get_index(),
            reinterpret_cast<char*>(&lotto));
        int itemId = lotto.m_addInfo;
        std::map<int, int>::iterator pos = result.find(itemId);
        // [b28d-solo-GmCmds_A] P220：ORIG 用 ne（`call ne; test; je <insert>`）。
        if (pos != result.end())
        {
            ++pos->second;   // ORIG 单次 ptEv + mov/add/mov（`= pos->second + 1` 4.4 双 ptEv）
        }
        else
        {
            // [b28d-solo-GmCmds_A] P61：ORIG 调 make_pair<int,int>（IiiE 值对）；
            // 左值实参生成 make_pair<int&,int>（IRiiE，p167 EXTRA）。
            // [b28d-solo-GmCmds_A] ORIG +0x0b1 从 lotto+2 重读（`mov -0x73(%ebp),%eax`），
            // 非从 itemId 槽（-0x34）读。
            result.insert(std::make_pair(static_cast<int>(lotto.m_addInfo), 1));
        }
    }
}

// ORIG get_gold_lottery_result 0x084afd1c：统计金币产出（key=+7 数量，value=1）。
// [b28d-47] verdict=DIFF(routed) [N7/N82] ORIG 0x084afd1c orig/ours=86/87
//   修：idx 实参内联（P0）+ `++pos->second`。残余：寄存器角色/槽差。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=21 [N82/N95] p167 CLEAR(11/11)；
// 全帧槽差 4（lotto -0x6d vs -0x71、pos -0x30 vs -0x34），语义完成结构性封顶。
// [b28d-solo-2] verdict=DIFF：make_pair 第一实参改 `static_cast<int&&>(lotto.m_addInfo2)`
// （P315/P234，ORIG 调 `make_pair<int,int>(int&&,int&&)` 且直接传 `lea lotto+7`）
// 后 orig/ours=87/87；残余仅 4.4 对右值 cast 恒物化临时拷贝（2 条，探针 p16
// f1/f2/f4 均拷贝，4.1 直接传原地址）⇒ N130/P75 封顶。
// [round5-A20 复核] fresh .o 87/86（gap +1）唯一残差=make_pair 首实参：
//   ORIG 0x084afaf0 `lea -0x6d(%ebp),%edx; add $0x7,%edx; mov %edx,0x4(%esp)`（把
//   **字段本身的地址**作 int&& 绑定）vs 4.4.7 对 `static_cast<int&&>(lotto.m_addInfo2)`
//   恒先物化 4B 临时（`mov -0x6a(%ebp),%eax; mov %eax,-0x10(%ebp); lea -0x10,%edx`）
//   ⇒ 4.1 引用直接绑字段、4.4.7 绑临时（`std::move` 形会多发一次 out-of-line call，
//   （P234 禁）⇒ N18a/P75 封顶。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：本件 ORIG 无串引用、无 PRETTY；§6.5.1 enc 0 命中。
#include "GmCmds_A/get_gold_lottery_result.inc"
                                           std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < m_count; ++i)
    {
        // [b28d-solo-GmCmds_A] P167/P0：ORIG 直读 m_itemList(+0xc)；key 为
        // `lea lotto+7` 直传（无 itemId 具名槽）。
        G_CDataManager()->m_itemList->get_lotto_item(
            const_cast<CItem*>(item)->get_index(),
            reinterpret_cast<char*>(&lotto));
        std::map<int, int>::iterator pos = result.find(lotto.m_addInfo2);
        // [b28d-solo-GmCmds_A] P220：ORIG 用 ne。
        if (pos != result.end())
        {
            ++pos->second;   // ORIG 单次 ptEv（同 getJarResult）
        }
        else
        {
            // [b28d-solo-2] P315/P234：ORIG +0x0cc 直接把 `lea lotto+7` 作第一实参
            // （无右值临时拷贝）且调 `make_pair<int,int>(int&&,int&&)`
            // （_ZSt9make_pairIiiE…OS2_OS5_）⇒ 用 static_cast<int&&> 显式右值化，
            // 避免 4.4 为值拷贝多发 mov 槽往返。
            result.insert(std::make_pair(static_cast<int&&>(lotto.m_addInfo2),
                                         static_cast<int&&>(1)));
        }
    }
}

// ORIG getBoosterResult 0x084af9d6：统计 booster 产出（stStackableBoosterItemInfo_t）。
// [b28d-47] verdict=DIFF(routed) [N7/P96] ORIG 0x084af9d6 orig/ours=139/131
//   修：make_pair 实参右值化（P61 值对）+ `++pos->second`。
//   残余：vector 遍历迭代器实例差（EXTRA ptEv/ppEv/ne，P175 族）+ 槽平移。
// [b28d-solo-GmCmds_A] verdict=NEAR ndiff=36 [N82] p167 CLEAR(21/21)；items 槽差 8 +
// 迭代器装载序；de/while/C1 转换构造/pair 值对已对齐，语义完成封顶。
#include "GmCmds_A/getBoosterResult.inc"
                                    std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] P0：ORIG +0x008 `mov 0xc(%ebp),%eax; mov %eax,-0x10`
    // 具名副本槽（getBoosterItem receiver 从 -0x10 读），声明先于 items。
    CStackableItem const* stackable = static_cast<CStackableItem const*>(item);
    std::vector<stStackableBoosterItemInfo_t> items;
    for (int i = 0; i < m_count; ++i)
    {
        stackable->getBoosterItem(items);
        // [b28d-solo-GmCmds_A] P167/P28：ORIG 先 begin()→C1<iterator> 转换构造，
        // 再 end()→C1 转换构造，两个 const_iterator 具名后 while 循环；写成
        // `it != items.end()` 会选混合 ne（p167 EXTRA）。
        std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
        std::vector<stStackableBoosterItemInfo_t>::const_iterator end = items.end();
        while (it != end)
        {
            // [b28d-solo-GmCmds_A] P167：ORIG 用 operator*（deEv），非 ptEv。
            // [round5-GmCmdsA] P157/P0（无槽律）：ORIG 全程**无 itemId 具名槽**
            // ——0x084afa60 deEv 后直存 -0x38 仅作 find 实参临时，0x084afad4 处
            // 再 deEv 一次存 -0x14 作 make_pair 实参；具名 itemId 会多占一槽
            // 并连带整组槽深偏 4（旧读数 items -0x48 vs ORIG -0x4c）。
            // ⇒ 两个使用点各自重读成员。
            std::map<int, int>::iterator pos = result.find((*it).m_itemIdx);
            if (pos != result.end())
            {
                ++pos->second;
            }
            else
            {
                // [b28d-47] P61：实参右值化 ⇒ 实例化 make_pair<int,int>（值对），
                // 左值 itemId 会实例化 make_pair<int&,int>（P167 EXTRA）。
                // [b28d-solo-GmCmds_A] ORIG 此处重读 (*it).m_itemIdx（第二个 deEv）。
                result.insert(std::make_pair(static_cast<int>((*it).m_itemIdx), 1));
            }
            ++it;
        }
        items.clear();
    }
}

// ORIG getRandomboxResult 0x084afe3a：统计 multibox 随机盒产出（pair<Inven_Item,bool>）。
// [b28d-47] verdict=DIFF(routed) [P220/P83] ORIG 0x084afe3a orig/ours=148/123
//   修：P220 正极性 `pos != end`（消 EXTRA eq/MISSING ne）+ P61 make_pair 右值化
//   + `++pos->second`。残余：EXTRA memcpy（我方 memcpy vs ORIG 结构整体赋值
//   rep movsl，N84）+ 槽平移。
// [b28d-solo-GmCmds_A] verdict=NEAR ndiff=10 [N82] p167 CLEAR(16/16)；drops 槽差 4
// （-0x44 vs -0x40），语义完成（结构整体赋值替 memcpy、正极性 ne、make_pair 值对）。
#include "GmCmds_A/getRandomboxResult.inc"
                                      std::map<int, int>& result)
{
    Inven_Item dropped;
    std::vector<std::pair<Inven_Item, bool> > drops;
    for (int i = 0; i < m_count; ++i)
    {
        drops.clear();
        const_cast<CStackableItem*>(static_cast<CStackableItem const*>(item))->get_multibox_lottery_item(&drops);
        for (unsigned int j = 0; j < drops.size(); ++j)
        {
            // [b28d-solo-GmCmds_A] N84③：ORIG 为结构整体赋值逐字段浅拷贝
            // （+0x6b 段 16×mov dword + movzbl），memcpy 是 EXTRA call。
            dropped = drops[j].first;
            // [round5-GmCmdsA] P157/P0：ORIG 无 itemId 具名槽（0x084aff79 读
            // dropped+2 直入 find 实参临时、0x084affb3 处再读一次作 make_pair
            // 实参）⇒ 两个使用点各自重读 dropped.m_addInfo。
            std::map<int, int>::iterator pos = result.find(dropped.m_addInfo);
            // [b28d-47] P220 正极性：ORIG 用 `ne`（pos != end 时 ++），旧 `==` 形
            // 产 eq（P167 MISSING ne / EXTRA eq）。
            if (pos != result.end())
            {
                ++pos->second;   // [b28d-47] 单次 ptEv
            }
            else
            {
                result.insert(std::make_pair(static_cast<int>(dropped.m_addInfo), 1));  // [b28d-47] P61 右值化
            }
        }
    }
}

// ORIG getCompoundEmblemResult 0x084af0d6：合成徽章统计；目标不是 16 型物品返回 false。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af0d6 orig/ours=218/200
//   残差：槽/寄存器分配跨版本差 + STL 内联实例（N89）。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=61 [N82/N4] p167 CLEAR(31/31)；sockets 槽差
// 0x10 + ORIG 单条 scaled disp `mov 0xc(%ebx,%eax,4)` vs 4.4 shl+lea 折叠 + sete 形。
// （S298 将 stStackableBoosterItemInfo_t::m_itemIdx 改 unsigned int 后本件 ndiff 76→61。）
// [b28d-solo-2] verdict=DIFF：修 sete 物化（ORIG +0x05d `cmpl $-1; sete; test; je`
// ⇒ `while (jewelType == -1) { return false; }`，P295/P225）后 orig/ours=219/219。
// 残余：全帧槽序差 8（sockets -0x48 vs -0x40，4.1 先声明深 vs 4.4 分区分配，N82）
// + 跳表 `mov 0xc(%ebx,%eax,4)` vs shl+lea（N4）+ 虚调用 receiver 寄存器角色（N7）。
//
// [round5-GmCmdsA] P291/P311 具名数组成员视图：ORIG 0x084af148 先 `call G_CDataManager`
// 入 %ebx（无槽），0x084af155 才 `call get_grade`，随后
// `add $0x208,%eax; mov 0xc(%ebx,%eax,4),%eax` —— 即「%ebx = 管理器基址、disp=0xc、
// 索引寄存器已含 +0x208」的 scaled 索引形。旧源码 `int grade = item->get_grade();`
// 具名槽 + `(char*)mgr + (grade*6+jewelType)*4 + 0x82c` 裸算术 ⇒ 4.4 折成
// `shl $2; lea (%ecx,%eax,1)` 且 get_grade 先于 G_CDataManager 求值（差 2 条）。
// 改用视图后：基址先求值、索引常量入寄存器、scaled disp 折叠，与 ORIG 逐条一致。
struct CBoosterEmblemIdTableView
{
    char m_pad00[0x0c];       // +0x00（ORIG 索引基址 = G_CDataManager() + 0xc）
    int  m_ids[0x1000];       // +0x0c（ORIG 恒 `mov 0xc(%ebx,%eax,4)`）
};

// [L1FreeTop12.S2 2026-09-11] §6.5 复核：本件 ORIG 无串引用（`$"` 0 条）、无 PRETTY；
//   §6.5.1 enc 全库 0 命中。
#include "GmCmds_A/getCompoundEmblemResult.inc"
                                           std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] ORIG +0x00e..+0x039：两个具名 item 副本
    // （-0x28/-0x24）+ 死槽 count=2（-0x20）+ jewelType 预置 -1（-0x1c）。
    // [round5-GmCmds_A] P212/判据⑩：ORIG sockets 槽 -0x48 **深于** p1(-0x28)
    // ⇒ 4.1「先声明者深」⇒ 原码 sockets 声明在 p1/p2 之前（仅赋值在后）。
    unsigned short sockets[2];
    CItem const* p1 = item;
    CItem const* p2 = item;
    int count = 2;
    sockets[0] = static_cast<CStackableItem const*>(p1)->getJewelTargetSocket();
    sockets[1] = static_cast<CStackableItem const*>(p2)->getJewelTargetSocket();

    int jewelType = -1;
    jewelType = this->_selectJewelTypeIndex(2, sockets);
    (void)count;
    // [b28d-solo-2] P295/P225：ORIG +0x05d `cmpl $-1,槽; sete; test; je` 为
    // 4.1 槽比较 bool 物化形，4.4 对 if 恒折 `jne` 直跳 ⇒ while+return 复现。
    while (jewelType == -1)
    {
        return false;
    }

    // 徽章映射表：ORIG = G_CDataManager() + 0xc + (0x208 + grade*6 + jewelType)*4
    // （P0：无 grade 具名槽，get_grade 内联进下标表达式；P291 具名数组视角）。
    // [round5-GmCmds_A] 判据⑩/帧对齐：ORIG 帧 0x70 与 p1/p2/count/jewelType
    // 槽（-0x28/-0x24/-0x20/-0x1c）要求本处存在一个具名 grade 槽（去掉后帧塌
    // 0x60 且全组浅移 4）；get_grade 实参用 p1（ORIG 0x084af155 `mov -0x28(%ebp)`)
    int grade = p1->get_grade();
    int emblemId =
        reinterpret_cast<const CBoosterEmblemIdTableView*>(G_CDataManager())
            ->m_ids[grade * 6 + jewelType + 0x208];

    CStackableItem const* emblem =
        static_cast<CStackableItem const*>(G_CDataManager()->find_item(emblemId));
    if (emblem == 0 || emblem->GetItemType() != 0x10)
    {
        return false;
    }

    for (int i = 0; i < m_count; ++i)
    {
        std::vector<stStackableBoosterItemInfo_t> items;
        emblem->getBoosterItem(items);
        // [b28d-solo-GmCmds_A] P167/P28：同 getBoosterResult，两个具名
        // const_iterator（begin/end 各一次 C1<iterator> 转换构造）+ while 形。
        std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
        std::vector<stStackableBoosterItemInfo_t>::const_iterator end = items.end();
        while (it != end)
        {
            // [b28d-solo-GmCmds_A] P167：本件 ORIG 用 ptEv（08111bc0），与
            // getBoosterResult 的 deEv（084b4a16）相反；条件取反避免第二个
            // ppEv 调用点（ORIG 仅 +0x267 一处 ppEv）。
            // [round5-A20 C05④ 归因核 · 实测回退] ORIG 0x084af25f `mov %eax,-0xc(%ebp);
            //   cmpl $0x0,-0xc(%ebp); je`（find_item 结果有槽再比较，P0/P115）。
            //   试补具名局部 `CItem* foundInner = …` 实测：setne 消除 ✓，但 4.4 把该槽
            //   分配到**浅区 -0x2c**，挤出 p1/p2/count/sockets 整组错位（p1 多一份 -0x2c
            //   副本）、指令数 217→**216**（gap 0→−1）、归一 ndiff 191 vs 旧 254。
            //   与 A28/P0「我方多槽不可控」同族：**净账不达标（gap 由 0 退化 −1 +
            //   头部槽集新增 ORIG 无的 -0x2c）⇒ 已回退**，保留 gap 0 与头/尾槽集全等的旧形。
            if (G_CDataManager()->find_item(it->m_itemIdx) != 0)
            {
                int key = it->m_itemIdx;
                std::map<int, int>::iterator pos = result.find(key);
                // [b28d-solo-GmCmds_A] P220：ORIG 用 ne（`call ne; test; je <else>`）；
                // operator[] 实参重读 it->m_itemIdx（p167 ptEv×6）。
                if (pos != result.end())
                {
                    result[it->m_itemIdx] += it->m_addInfo;
                }
                else
                {
                    result[it->m_itemIdx] = it->m_addInfo;
                }
            }
            ++it;
        }
    }
    return true;
}

// ORIG _selectJewelTypeIndex 0x084af3b2：按凹槽数据加权随机选宝石类型槽。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af3b2 orig/ours=94/87
//   残差：全帧槽号整体平移 4（ORIG -0x38.. vs 我方 -0x34..）+ push ebx 差异
//   （4.1 不按 lifetime 复用栈槽，N82/P112）。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=12 [N82/N93+P184] p167 CLEAR(2/2)；
// push %ebx（4.4 需 callee-saved）+ 槽序差 + ORIG 空 then 死比较 `cmpl $-1,-0x18`
// 被 4.4 DCE（P184 照抄不可复现）。
// [b28d-solo-2] verdict=DIFF：真缺口 = 权重乘法书写序。`counts[i]*(10000/total)`
// 令 4.4 空 push %ebx（+push/pop 2 条、帧 0x74 vs ORIG 0x78）；交换为
// `(10000/total)*counts[i]` 后 push/pop 消失、槽位与 ORIG 全等（探针 p13 A1 逐条同形）。
// 残余仅 ORIG 空 then 死块 3 条（`cmpl $-1,-0x18; jmp; nop`，4.1 空 if 发射，
// 4.4 DCE；while+break 反产 sete/test 形）⇒ P184 封顶。
// [b28e-solo-2] 复核（fresh .o 94/91，jcc 目标逐位核对：除该 3 条外 84af415..084af501
// 与 19bd..1aa2 逐条同目标）：该 3 条为 ORIG 084af44c `83 7d e8 ff` +
// 084af450 `eb 01` + 084af452 `90`，比较结果**无消费者**（jmp 无条件跳过 nop）⇒
// 4.1 空 then `if (ret==-1){}` 发射产物。4.4 前端整块 DCE：探针 /tmp/pg/p2.cpp
// a1..a9（空 if/else 空/switch/while-break/do-while/goto 尾标签/嵌套/for-break）+
// /tmp/pg/p4.cpp b1..b8（双 goto 同标签/else goto/双标签串/自赋值/(void)）共 17 形
// 全部无 cmp/jmp/nop 输出 ⇒ 无源码解，P184 封顶。
// [round5-A20 本轮新修·配方㉓] 空 `switch (ret == -1) { }` 复现 ORIG 0x084af44c 的
//   死比较 `cmpl $0xffffffff,-0x18(%ebp)`（旧 `if(ret==-1){}` 被 4.4 前端整块 DCE，
//   17 形探针全败）⇒ 92/93（gap -1，改前 91/93 gap -2）。残余仅 ORIG 0x084af450
//   `jmp +1; nop` 空 then 中继岛（P184/P223 边界：该 jmp 为 4.1 空块占位产物，
//   4.4 无对应发射点，`switch(ret){case -1:break;}` 形实测更劣 91）⇒ 封顶。
// [L1FreeTop12.S2 2026-09-11] N-JMP 再试：`switch(ret==-1){default:break;}` 实测同 92
//   （4.4 仍 DCE 该无条件 jmp+nop）⇒ 维持封顶。§6.5 复核：本件 ORIG 无串引用、无 PRETTY。
#include "GmCmds_A/_selectJewelTypeIndex.inc"
                                        unsigned short const* sockets) const
{
    int counts[5] = {0, 0, 0, 0, 0};
    // [b28d-solo-GmCmds_A] ORIG +0x029..+0x045 对 cumul[5] 也逐条清零
    // （-0x4c..-0x3c 五条 movl $0）⇒ 数组带零初始化列表。
    int cumul[5] = {0, 0, 0, 0, 0};
    int total = 0;
    // [b28d-solo-GmCmds_A] ORIG 清零序 counts→cumul→total(-0x50)→running(-0x24)
    // →i(-0x1c) ⇒ running 声明在首个循环之前。
    int running = 0;

    for (unsigned int i = 0; i < count; ++i)
    {
        if (sockets[i] != 0)
        {
            // [b28d-solo-GmCmds_A] P184：ORIG +0x097 保存返回值到 -0x18 并
            // `cmpl $0xffffffff,-0x18`（then 为空块，4.1 仍发射）⇒ 照抄死比较。
            int ret = getAvatarEmblemTypeIdxForCompountEmblem(sockets[i], counts,
                                                              total);
            // [round5-GmCmdsA] 配方㉓（Main 2026-09-10，A14 实证）：ORIG 0x084af44c
            // `cmpl $0xffffffff,-0x18; jmp +1; nop` 是**无分支消费者的死比较**，
            // 4.4 前端对 `if(ret==-1){}`/`ret==-1;`/while-break 等 17 形恒 DCE
            // 或产 sete/test ⇒ 旧判 N99/P184。改**空 switch** 即产纯比较后直落。
            switch (ret == -1)
            {
            }
        }
    }
    if (total == 0)
    {
        return -1;
    }

    for (int i = 0; i <= 4; ++i)
    {
        // [b28d-solo-2] 乘法书写序：`counts[i] * (10000/total)` 使 4.4 空
        // push %ebx（+1 条、帧 0x74 vs ORIG 0x78）；交换为
        // `(10000/total) * counts[i]` 后逐条复现 ORIG（探针 p13 A1）。
        int weight = (10000 / total) * counts[i];
        cumul[i] = weight + running;
        running += weight;
    }

    int picked = get_rand_int(running);
    for (int i = 0; i <= 4; ++i)
    {
        if (picked < cumul[i])
        {
            return i;
        }
    }
    return -1;
}

// ORIG makeFileContents 0x084af504：map 结果追加为 TSV 行（超限提示续接并中止）。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af504 orig/ours=192/174
//   残差：槽/寄存器分配 + 字符串物化链差。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P214③+P251+P0]
// 数组零初始化 + switch(it->first) 单次 ptEv 分发 + 各分支重读 it->first。
#include "GmCmds_A/makeFileContents.inc"
                                    std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] P214③：ORIG 首清零为内联 rep stos 0x100 dwords
    // （=0x400 字节，ORIG +0x00c 段）⇒ 数组零初始化；旧 `memset(line,0,0x100)`
    // 既多一次 call（p167 EXTRA memset）又少清 0x300 字节。
    char line[1024] = {0};

    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        // [b28d-solo-GmCmds_A] P0/P167：ORIG 无 itemId 具名槽，各分支重读
        // `it->first`（p167 MISSING ptEv×5：ORIG 11 次 ptEv）。
        CItem* info = G_CDataManager()->find_item(it->first);
        // [b28d-solo-GmCmds_A] P251：ORIG 分发段 `call ptEv; mov (%eax),%eax;
        // test; je <0>; cmp $1; je <1>; jmp <default>`（first 只读一次）
        // ⇒ 源形是 switch(it->first)，if 链会多一次 ptEv（p167 EXTRA ptEv）。
        switch (it->first)
        {
        case 0:
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", it->first, "gold", "unknown",
                    it->second);
            break;
        case 1:
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", it->first, "coin", "unknown",
                    it->second);
            break;
        default:
            if (info == 0)
            {
                sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", it->first, "unknown", "unknown",
                        it->second);
            }
            else
            {
                sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\t%d\t%d\t%d\n",
                        it->first, info->GetItemName(),
                        getRarityStr(static_cast<ENUM_RARITY>(info->get_rarity())),
                        it->second, info->get_price(), info->GetSellPrice(),
                        info->getUsableLevel());
            }
            break;
        }

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "\xb0\xe1\xb0\xfa \xc6\xc4\xc0\xcf \xc0\xcc\xbe\xee\xba\xd9\xc8\xfb";
            SendChatMsg(user, sAppend);
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }
}

// ORIG make_file_gold_contents 0x084af7fc：金币分布行 + 尾部 max gold 合计。
// （格式串 %I64u 按 ORIG 原样保留；glibc 下由 %llu 语义承载 64 位实参。）
// [b28d-47] verdict=DIFF(routed) [N82/P112] ORIG 0x084af7fc orig/ours=121/113
//   p167 CLEAR（18/18 调用点全等）⇒ 语义完成；残差为槽/寄存器分配跨版本差。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P214③+P171+P0]
// 首清零 rep stos 0x400B（数组零初始化）+ 末尾死写入 memset（P171）+ maxGold 前置。
#include "GmCmds_A/make_file_gold_contents.inc"
                                           std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] ORIG +0x00b 先清零 maxGold（-0x10/-0xc）再 rep stos
    // ⇒ 声明序 maxGold 在前（P0 槽序）。
    unsigned long long maxGold = 0;
    // [b28d-solo-GmCmds_A] P214③：ORIG 首清零为内联 rep stos 0x100 dwords
    // （=0x400 字节，ORIG +0x019 段）⇒ 源形是数组零初始化，且长度真值 0x400
    // （旧 `memset(line,0,0x100)` 少清 0x300 字节，属真 bug）。
    char line[1024] = {0};

    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        sprintf(line, "%d gold count = %d \n", it->first, it->second);
        // [b28d-solo-GmCmds_A] P0：ORIG 无 gold 具名槽（imul 结果直入
        // `mov %eax,%edx; sar $0x1f; add %eax,-0x10; adc %edx,-0xc`）。
        maxGold += it->first * it->second;

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "\xb0\xe1\xb0\xfa \xc6\xc4\xc0\xcf \xc0\xcc\xbe\xee\xba\xd9\xc8\xfb";
            SendChatMsg(user, sAppend);
            break;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }

    sprintf(line, "max gold = %llu \n", maxGold);
    strcat(fileBuf, line);
    // [b28d-solo-GmCmds_A] P171：ORIG +0x1cb 末尾死写入 memset(line,0,0x400)
    // （结果从不被读）照抄，缺失即少一个调用点（p167 MISSING memset）。
    memset(line, 0, 0x400);
}

// ===========================================================================
// CBindCubeTest — ORIG W ctor 0x082a8672：基类 vptr → 自身 vptr(0x8c81780)，
// string@+0x08 默认构造（占位；真实 +0x08 为 byte 职业）。
// ===========================================================================

// ORIG parse 0x084acbe4：trim(args)；+0x08 = (byte)atoi(args.c_str())。
#include "GmCmds_A/parse_3.inc"
{
    trim(m_parser->m_args);
    m_flag = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084acc1e：职业 0..10 时遍历 bind×recipe×equip 组合跑合成测试，
// 每组结果落一份 bind_<job>_<bind>_<recipe>_<equip>.txt 回显给 GM。
// [b28d-47] verdict=DIFF(routed) [N78/N84] ORIG 0x084acc1e orig/ours=166/134
//   残差：EXTRA memset（我方缓冲零初始化 call vs ORIG 内联 rep stos，N78）+
//   槽/寄存器分配差。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=5 [N-待解] p167 CLEAR(14/14)。
// [b28e-solo-2] 真因定位：ORIG 函数头 fileBuf 清零是**数组初始化**形而非显式
// memset —— +0x085 段 `mov $0x17318,%edx; mov %edx,0x8(%esp)`（size 经 %edx
// 中转），而循环内 +0x101 段才是 `movl $0x17318,0x8(%esp)` 直写（同一常量两形
// 并存 ⇒ 原码两处写法不同）。4.4 对 `char buf[N] = {0};` 精确复现中转形
// （探针 /tmp/pg/p1.cpp init_arr vs init_arr2），改后 gap 1→0。
#include "GmCmds_A/execute_7.inc"
{
    if (!(static_cast<signed char>(m_flag) >= 0 &&
          static_cast<signed char>(m_flag) <= 10))
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P312：ORIG 对 fileName[0xff](-0x13b) 发内联对齐剥离
    // 清零（无 memset call，p167 EXTRA memset）。
    char fileName[0xff] = {0};
    // [b28e-solo-2] 函数头清零 = 数组初始化形（size 经 %edx 中转，见上）；
    // 循环内仍是显式 memset(fileBuf,0,95000) 直写形。
    char fileBuf[95000] = {0};

    std::map<int, int> result;
    for (int bind = 0; bind <= 1; ++bind)
    {
        for (int recipe = 0; recipe <= 2; ++recipe)
        {
            for (int equip = 0; equip <= 7; ++equip)
            {
                memset(fileName, 0, 0xff);
                memset(fileBuf, 0, 95000);
                static_cast<WongWork::CCompoundAvatar*>(
                    GlobalData::s_pCompoundAvatar)
                    ->ProcTest(static_cast<ENUM_CHARACTERJOB>(
                                   static_cast<int>(static_cast<signed char>(m_flag))),
                               static_cast<ENUM_EQUIPMENTTYPE>(equip),
                               recipe, bind, result);
                CBoosterTest::makeFileContents(GetUser(), fileBuf, result);
                sprintf(fileName, "bind_%s_%s_%s_%s.txt",
                        CHARACTER_JOB_NAMES[static_cast<int>(
                            static_cast<signed char>(m_flag))],
                        BIND_NAMES[bind], RECIPE_NAMES[recipe], EQUIP_NAMES[equip]);
                CBoosterTest::printClient(GetUser(), false, fileName, fileBuf);
                result.clear();
            }
        }
    }
}

// ===========================================================================
// BlueMarbleSet — ORIG W ctor 0x082a88ae：基类 vptr → 自身 vptr(0x8c81588)。
// 布局：+0x08 byte 步数、+0x09 byte move 标记、+0x0a byte enter 重置标记、
//       +0x0c int token 计数（getParam 工作区）。
// ===========================================================================

// ORIG parse 0x084b22ca：三标记清零、token 计数清零，trim 后整串交给 getParam。
#include "GmCmds_A/parse_4.inc"
{
    m_flag1 = 0;
    m_flag2 = 0;
    m_flag0 = 0;
    m_param = 0;

    trim(m_parser->m_args);
    this->getParam(m_parser->m_args);
}

// ORIG getParam 0x084b235c：按空格切 token（≤16），tokens[0]=="move"/"enter" 置
// 对应标记，atoi(tokens[1]) 写步数字节。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084b235c orig/ours=183/178
//   残差：槽号平移 + std::string 局部构造/拷贝实例差。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P0/P212]
// 删局部 used 改用成员 m_param(+0xc) + pos 声明先于 tokens。
#include "GmCmds_A/getParam.inc"
{
    int pos = 0;
    // [b28d-solo-GmCmds_A] ORIG +0x009 `movl $0,-0x1c` 在 tokens 构造循环之前
    // ⇒ pos 声明在 tokens 之前。
    std::string tokens[16];
    // [b28d-solo-GmCmds_A] ORIG 用成员 m_param(+0xc) 作 tokens 下标与计数
    // （`mov 0x8(%ebp),%eax; mov 0xc(%eax),%edx`，parse 已清零），非局部 used
    // ⇒ 删 `int used = 0;`（ORIG 函数头无该清零）。
    while ((pos = static_cast<int>(param.find(' ', 0))) != -1)
    {
        tokens[m_param] = param.substr(0, pos);
        param = param.substr(pos + 1, std::string::npos);
        ++m_param;
    }
    tokens[m_param] = param;

    if (tokens[0].compare("move") == 0)
    {
        m_flag1 = 1;
    }
    else if (tokens[0].compare("enter") == 0)
    {
        m_flag2 = 1;
    }
    m_flag0 = static_cast<unsigned char>(atoi(tokens[1].c_str()));
}

// ORIG execute 0x084b25a4：enter 标记优先重置入场次数；否则 move 标记 + 步数
// 有效时驱动大富翁移动。
#include "GmCmds_A/execute_8.inc"
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    if (m_flag2 != 0)
    {
        user->resetBlueMarbleEnterCount();
        user->sendBlueMarbleEnterCount();
        return;
    }

    BlueMarble* blueMarble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (blueMarble == 0)
    {
        return;
    }
    if (m_flag1 == 0)
    {
        return;
    }
    if (m_flag0 == 0)
    {
        return;
    }
    // P196：ORIG 对 m_flag0(+0x08, char 存 GM 参数 0-255, parse 里 static_cast<unsigned char>)
    // 作实参直传时 `movzbl %al,%eax`（无符号扩展），我方 static_cast<int> 使 4.4 出 movsbl
    // （符号）⇒ 值≥0x80 时 movePosition 收负 pos，真 bug。显式经 unsigned char 提升复现 movzbl。
    blueMarble->movePosition(user, static_cast<int>(static_cast<unsigned char>(m_flag0)));
}

// ===========================================================================
// CBingoGmTest — ORIG W ctor 0x082a893a：基类 vptr → 自身 vptr(0x8c81510)，
// 无成员初始化（+0x08 int 未初始化，ORIG 如此）。类声明见 GameMasterCmd.h
// （占位 std::string m_str0@+0x08，真实为 int 子命令码，本 TU 偏移访问）。
// ===========================================================================

// ORIG parse 0x084b35c4：trim(args)；+0x08 = atoi(args.c_str())。
#include "GmCmds_A/parse_5.inc"
{
    trim(m_parser->m_args);
    m_subCmd = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084b35fc：user 有效时按子命令分流。
#include "GmCmds_A/execute_9.inc"
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    if (m_subCmd == 0)
    {
        resetData();
    }
    else
    {
        reqValue();
    }
}

// ORIG resetData 0x084b363e：清 bingo 数据、置脏并发送全量数据。
#include "GmCmds_A/resetData.inc"
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    static_cast<BingoData*>(user->getBingoData())->clear();
    static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
    user->sendBingoData();
}

// ORIG reqValue 0x084b368a：repeat 事件(106)有效且连线 <3 时抽新棋盘值，
// 结算连线奖励并以 0x1c3 包回报当前棋盘字。
// [b28d-47] verdict=DIFF(routed) [N18b] ORIG 0x084b368a orig/ours=152/150
//   残差：`if (gained > 3) gained = 3;` 4.4 恒折内存比较+内存写
//   （`cmpl $3,-0x10; movl $3,-0x10`）vs ORIG 寄存器中转 4 条
//   （`mov slot,%eax; cmp; mov $3,%eax; mov %eax,slot`）⇒ N18b 封顶。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=4 [N18b] p167 CLEAR(24/24)；clamp 段
// 4.4 恒折 `cmpl $3,-0x10; movl $3,-0x10`，ORIG 为寄存器中转；if/三目/副本三形实测
// ndiff=4/5/42 ⇒ 取 if 形封顶（4 条内存-寄存器形差）。
// [b28d-solo-2] 复核：orig/ours=153/150，gap 2 全在 clamp 段（ORIG 5 条
// `mov slot,%eax; cmp $3,%eax; jle; mov $3,%eax; mov %eax,slot` vs 4.4 3 条内存形）。
// 追加探针 p5/p6/p8 共 12 形（if/反转/三目/副本/goto/unsigned/长整/枚举/指针写）
// 4.4 恒折内存比较+内存写 ⇒ N18b 封顶；尾部三岛（nop;jmp ×2 + nop）已对齐。
// [b28e-solo-2] 复核（fresh .o 152/150，jcc 目标逐位核对：084b36a4/36c6/36e2 三岛、
// 084b37cc/37a9/378b 循环与块出口与 266a/268c/26a8/278b/2768 逐一同目标，唯 clamp 段
// `jle 084b378c`（回写点）vs 我方 `jle 274e`（跳过内存写）形异）：clamp 原码是三目/
// min 形（无条件回写）。4.4.7 对 `x>3?3:x` 走 MIN_EXPR 发 cmovle（探针 c2/c3/c6/d6/d11），
// 对 if 恒内存比较+内存写（c1/c8/c10），unsigned 比较出 `mov slot,%eax; cmpl; jbe; movl`
// 仍无回写（c4/d2），`+0`/cast/局部副本等 11 形均不产寄存器回写 ⇒ 后端 cmov 选择差异，
// N18b/P321-T5 封顶（禁为装载序改类型）。
// [round5-A20 复核·N18b] fresh .o 147/149（gap -2）唯一残差=clamp 段 5 vs 3 条：
//   ORIG 0x084b37xx `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle; mov $0x3,%eax;
//   mov %eax,-0x10(%ebp)`（寄存器比较+常量经 eax 统一回写）vs 4.4 `cmpl $0x3,-0x10;
//   jle; movl $0x3,-0x10`（内存形）。判据⑪ PURE SHIFT c=0（frame 64=64、槽集全等）
//   ⇒ 非槽问题；前轮 12 形探针 + 本轮复核结论一致：寄存器回写形仅 `-march=i486`
//   （消 cmov/MIN_EXPR，P230①）可达，属路由域 ⇒ N18b/P321-T5 封顶。
// [round5-A20 配方②复查] **禁按 C05-F2（unsigned→int 内存直比）反向套用本件**：
//   ORIG 条件码为 `jle`（**有符号序**）⇒ gained 必为 int；改 unsigned 会得 `jbe`，
//   触发 P196「符号性/比较方向反演」真 bug 家族（值域含负时行为分叉）。本件缺的是
//   ORIG 的寄存器**回写** `mov $0x3,%eax; mov %eax,-0x10(%ebp)`，F2 只翻比较侧
//   ⇒ 无交集，保留 N18b + 路由域判词。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：本件 ORIG 无串引用（`$"` 0 条）、无 PRETTY；
//   §6.5.1 enc 全库 0 命中 ⇒ 无语义轴缺口，维持 N18b/路由域封顶。
#include "GmCmds_A/reqValue.inc"
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    BingoEvent* event = static_cast<BingoEvent*>(
        static_cast<void*>(GlobalData::s_event_manager->GetRepeatEvent(0x6a)));
    if (event == 0)
    {
        return;
    }

    // [b23S3C] ORIG +0x48 单次 getBingoData→movzbl+cmp$2,setg 寄存器直测（无
    // lines 具名槽）；+0x74 每次 calNewValue 第 1 实参 = 独立 getBingoData() 的
    // bd 指针直传 uint&（ORIG mov %eax,%ebx；无 board 拷贝槽 -0x30）；
    // receiver 链 shared_ptr::operator-> 真调用（+0x8f）。
    if (static_cast<signed char>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4) > 2)
    {
        return;
    }

    int oldBoard = static_cast<int>(
        static_cast<BingoData*>(user->getBingoData())->m_eventId);
    int newValue = (*reinterpret_cast<boost::shared_ptr<CBingo>*>(
                        event->getBingoSystem()))
                       ->calNewValue(
                           static_cast<BingoData*>(user->getBingoData())
                               ->m_eventId,
                           m_subCmd);

    if (*reinterpret_cast<int*>(user->getBingoData()) != oldBoard)
    {
        static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
        int gained = newValue - static_cast<int>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4);
        if (gained > 0)
        {
            // [b23S3C] ORIG +0x0f5 分支形 clamp（读槽→cmp reg→jle→mov $3→写回）。
            // [b29-Lane21] 实测：if 形残差 4 行（4.4 折 `cmpl $3,-0x10; movl $3,-0x10`）；
            //   三目形 `(gained>3)?3:gained` 残差 5 行（4.4 发 `cmovle -0x10,%eax`，本 TU
            //   无 -march=i486 ⇒ cmov 族不可消，P230①/Main 路由域）⇒ 保留 if 形。
            //   残差 = N18b/P321-T5（禁为装载序改类型）：ORIG `mov -0x10,%eax; cmp $3,%eax;
            //   jle; mov $3,%eax; mov %eax,-0x10`（寄存器比较 + 常量经 eax 统一写回）。
            if (gained > 3)
                gained = 3;
            static_cast<BingoData*>(user->getBingoData())->m_pad4 =
                static_cast<char>(newValue);
            for (int i = 0; i < gained; ++i)
            {
                event->sendOneMatchLineReward(*user);
            }
        }
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(m_subCmd);
    packet.put_int(*reinterpret_cast<int*>(user->getBingoData()));
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// CBillingRechargeCommand — ORIG W ctor 0x082a898e：基类 vptr → 自身
// vptr(0x8c814c8)，string@+0x08 默认构造（货币名）；+0x0c int 数额。
// ===========================================================================

// ORIG parse 0x084b3a60：parser 有效时 m_str0=args 向量[1]，+0x0c=StringToNumber([2])。
#include "GmCmds_A/parse_6.inc"
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        // [round5-GmCmdsA] 写侧用 union 的 int 视角（与 RHS(int) 同型 ⇒ 4.4 复现
        // ORIG 0x084b3ac8 的 LHS-first 直存；unsigned 视角会使 4.4 翻成 RHS-first）。
        m_amount = StringToNumber(m_parser->m_extra[2].c_str());
        return;                                    // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}

// ORIG execute 0x084b3ad4：cera→虚表[1](type=5)、point→虚表[3](type=4) 充值下单，
// 最后统一走虚表[11] 通知。CIPGHelper 仅有前向声明，按 ORIG 虚表槽间接调用。
// [b28d-47] verdict=NEAR(routed) [N7] ORIG 0x084b3ad4 orig/ours=67/67
//   残差：间接 call 目标寄存器——ORIG 4.1 %ecx；4.4.7 恒 %eax、4.4.6 恒 %edx
//   （P230 实测 446 后端亦不达 %ecx）⇒ N7 封顶，连锁实参/数据寄存器名互换。
// [b28d-solo-GmCmds_A] verdict=NEAR ndiff=21 [N7] p167 CLEAR(3/3)；三处间接 call
// 目标寄存器 ORIG %ecx vs 4.4.7 %eax / 4.4.6 %edx（P230 实测），语义完成封顶。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%ecx; … call *%ecx` / `call *%edx`
//   vs 4.4.7 全 %eax；槽/条数两侧全等，仅寄存器角色（residual 6 行）。
// [round5-GmCmdsA] verdict=IDENTICAL：旧形为 §4.3 禁的裸指针 cast
//   `(*reinterpret_cast<void***>(p))[n]` —— 4.4 对 `void*`→函数指针的转换恒多发
//   `mov (%eax),%eax; mov %eax,%ecx` 两段（N18a 反向）。改用 TaiwanCash.h 权威虚接口
//   真成员调用（P5 虚槽直调形，Slot1=IPGInput/3=IPGInputPoint/11=IPGQuery）后
//   4.4 直发 ORIG 的 `mov (%eax),%eax; add $0x4,%eax; mov (%eax),%ecx` 单段链，
//   orig/ours=67/67 逐条全等（零新增符号）。
#include "GmCmds_A/execute_10.inc"
{
    CUser* user = GetUser();
    // ORIG（P157 逐点重取形）：每个调用表达式独立重读 s_pIPGHelper → 取 vptr →
    // add 槽 → deref fn，随后再读一次 helper 作 this；禁 vtable 缓存局部
    //（缓存少发 ORIG 的逐点 fetch 行且引入额外栈槽）。
    if (m_str0 == "cera")
    {
        // 读侧用 union 的 unsigned 视角（与 callee 形参 unsigned int 同型 ⇒ 单段直落）。
        GlobalData::s_pIPGHelper->IPGInput(user, 5, m_amountU, "", "", "GM", 0, 0, 0);
    }
    else if (m_str0 == "point")
    {
        GlobalData::s_pIPGHelper->IPGInputPoint(user, m_amountU, 4, 0, 0);
    }
    GlobalData::s_pIPGHelper->IPGQuery(user);
}

} // namespace game_master