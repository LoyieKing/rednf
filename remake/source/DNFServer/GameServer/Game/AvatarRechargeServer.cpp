// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarRechargeServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarRechargeServer 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 依据 docs/class_func_reports/AvatarRechargeServer/、ARAD__SCRIPT__AvatarRecharge/
// 与 ORIG 反汇编转写。
#include "AvatarRechargeServer.h"
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn

#include <cstdio>
#include <string>
#include <string.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"        // cMyTrace
#include "MoreTypes.h"        // arad::SigArad*
#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile / Scan*
#include "CUser.h"            // CUser（含 eSendTarget/get_state/SendPacket 等）
#include "CInventory.h"       // CInventory::GetInvenRef/delete_item/GetAvatarItemMgrR
#include "CAvatarItemMgr.h"   // WongWork::CAvatarItemMgr::getJewelSocketData
#include "CParty.h"           // CParty::IsUseFatigueDungeon/CheckMapClear
#include "Inven_Item.h"       // Inven_Item::isEmpty/m_addInfo/m_addInfo2/m_fieldSplit
#include "GamePODTypes.h"     // stAvatarEmblemInfo_t
#include "PacketGuard.h"      // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "LogManager.h"       // LogManager::logFormat

// make_internal_stream_jpn 定义在 Arad_InternalDispatcher.cpp（ORIG 0x081977b6，
// 签名 (CStreamGuard&, ENUM_INTERNALPACKET_JPN, int)）。

// ============================================================================
// getSocketCount —— ORIG l 0x08190699 _ZL14getSocketCountPK20stAvatarEmblemInfo_t
// （文件局部静态）。遍历 5 个徽章槽（每槽 6 字节，取首 ushort emblemSocketType_），
// 非零计数。ORIG 不判空（调用方经 getJewelSocketData 保证非空），忠实转写。
// [b24F] DONE：改 break 结构复现 ORIG 双出口循环（指令序列一致）。
// ============================================================================
static int getSocketCount(const stAvatarEmblemInfo_t* emblem)
{
    int count = 0;
    for (int i = 0; i <= 4; ++i)
    {
        if (emblem->stEmblemSocket[i].emblemSocketType_ == 0)
            break;
        ++count;
    }
    return count;
}

// ORIG C1：SCRIPT 子对象构造 + 清 +0x54 与字节 +0x58（+0x00 上限字段不初始化，忠实 ORIG）。
#include "AvatarRechargeServer/AvatarRechargeServer.inc"
    : m_enduranceReduceRate(0),
      m_bUseEnduranceReduceRate(false)
{
    // [b28c-L9] 旧「构造序跨版本 N」判词作废：ORIG C1 0x08190200 的
    // `mov this; add $0x4,%eax; call ARAD::SCRIPT::AvatarRecharge::C1` 先于 vptr、
    // 且 vptr 在三个 call 之后才写 ⇒ 脚本子对象是**基类**而非成员。
    // 按 Itanium primary-base 规则把 AvatarRecharge 提为第一基类（非动态 ⇒ 不占
    // vptr；ScriptInterface 动态 ⇒ primary base，vptr@+0x00），
    // sizeof 0x5c/全偏移不变（头内 static_assert 自证）⇒ 21=21 AE。
}

// ============================================================================
// SendDurability —— ORIG T 0x08190416 _ZN...14SendDurabilityEP5CUsercsci
// ============================================================================
#include "AvatarRechargeServer/SendDurability.inc"
                                          char param4, int param5)
{
    // [b28d-L5a 2026-09-11 落地] 旧「P223 负例 N 封顶（49 vs 45）」判词**作废**。
    // ORIG 0x08190494 岛 A(`nop;jmp exit`)+0x08190497 岛 B(`nop`) 仅前两级早返有岛、
    // 第 3/4 级 `jne exit` 直落 epilogue ⇒ 唯一复现形 = **混合形**：
    //   前两级「负向 goto 共享尾标签」(`if(user==0) goto end; if(state!=5) goto end;`)
    //   + 第 3/4 级「正向嵌套」(`if(param2==1){ if(param4==3){ 主流程 } }`，失败直落函数尾)。
    // 逐条同构（`je 岛A` / `jne 岛B` / 两条 `jne exit` / 主流程尾 `jmp exit`），
    // 45 = 45，TU 读数 ae +1 / diff −1；P167 callsite 3=3 CLEAR。
    if (user == 0)
        goto end;
    if (user->get_state() != 5)
        goto end;
    if (param2 == 1)
    {
        if (param4 == 3)
        {
            // 目标：默认单人(1)，在队伍中改队伍(2)。
            CUser::eSendTarget target = (CUser::eSendTarget)1;
            if (user->GetParty() != 0)
                target = (CUser::eSendTarget)2;
            UpdateAvatarEndurance(target, user, (short)param5);
        }
    }
end:
    ;
}

// ============================================================================
// CalcDurability —— ORIG T 0x0819049a _ZN...14CalcDurabilityEP5CUseri
// [b28d-solo-AvatarRechargeServer] AE（86=86）：①槽 0..9 循环体
// `if (!CalcDurabilityDetail(...)) continue;` 复现 ORIG 0x081904f5
// `xor $1,%eax; test %al,%al`（je 落循环增量处被 4.1 jump-thread）；
// ②rates 遍历缓存 size（P13）：`for(i=0,n=rates.size(); i<n; ++i)`。
// ============================================================================
#include "AvatarRechargeServer/CalcDurability.inc"
{
    for (int round = 0; round < count; ++round)
    {
        std::vector<int> rates;
        rates.reserve(10);
        for (int slot = 0; slot <= 9; ++slot)
        {
            // [b28d-solo-AvatarRechargeServer] ORIG 0x081904f5 `xor $1,%eax; test %al,%al`
            // （je 落循环增量处被 4.1 jump-thread）⇒ 原码是 `if (!Detail(...)) continue;`。
            if (!CalcDurabilityDetail(user, slot, rates))
                continue;
        }
        if (!rates.empty())
        {
            // [b28d-solo-AvatarRechargeServer] P13 循环 end 缓存：ORIG 0x08190562
            // `call size; mov %eax,-0xc` 每轮不重算，条件 `cmp -0xc,%eax; setb`。
            for (std::vector<int>::size_type i = 0, n = rates.size(); i < n; ++i)
                UpdateAvatarEndurance((CUser::eSendTarget)2, user,
                                      (short)rates[i]);
        }
    }
}

// CalcDurabilityChangeEquip —— ORIG T 0x081905b4：定义 UserCoreDepsB2.cpp（去重保留）。

// ============================================================================
// CheckConditionUseRecharge —— ORIG T 0x081906e2
// 充值石(区1)/头像(区2)存在性、IsEmblemRechargeStone、CheckItemLock×2 校验；
// 通过时 sig->maxEndurance = m_rechargeLimit × socket 数，返 true。
// ============================================================================
#include "AvatarRechargeServer/CheckConditionUseRecharge.inc"
    CUser* user, arad::SigAradUseAvatarRecharge* sig)
{
    const Inven_Item* stone =
        user->getCurCharacInvenR()->GetInvenRef(1, (int)sig->stoneSlot);
    if (stone == 0 || stone->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }
    // b28d-lane25：P167 差集实证 ORIG 0x081907b8/0x081907cd 直接内联
    // map find/end + eq（end() 在左控实参序，P74 补充），不调 IsEmblemRechargeStone。
    // [b28d-solo-AvatarRechargeServer] ORIG 0x081907b8 先 `call end`（sret -0x20）后
    // `call find`（sret -0x1c，key 物化 -0x18）⇒ 具名 end 迭代器声明在前。
    // [b28d-L5a 2026-09-11] 比较操作数序已按 ORIG 对齐（`it == itEnd`：ORIG
    // 0x081907c0 `lea -0x20→0x4(%esp)` + `lea -0x1c→(%esp)` ⇒ this=it、参数=itEnd）。
    // 残余 = **两迭代器槽互换**：ORIG end→-0x20 / find→-0x1c，我方 end→-0x1c / find→-0x20
    // （4.1「先声明深」vs 4.4「先声明浅」，N93/N212 族）。三形实测均不闭合：
    //  ①内层块包裹 it（P212 块作用域）⇒ 槽不变；②P326 显式转换右值临时形不适用
    //  （本处无 const 转换、ORIG 侧无三级槽）；③全头部声明（P212 同序律）需默认构造
    //  `_Rb_tree_iterator()` 产生 2 条 `movl $0x0` ⇒ 226→228 条，条数反劣，不采纳。
    // 保持 NEAR（226=226，P167 25=25 CLEAR），语义等价。
    // [P8S4 2026-09-11] P139：ORIG 侧有 prett 符号
    // _ZZN20AvatarRechargeServer25CheckConditionUseRechargeEP5CUserPN4arad24SigAradUseAvatarRechargeEE19__PRETTY_FUNCTION__
    // （size=94，串内容与原手写串逐字节同）；原手写字面串不产生该符号 ⇒ 改用 __PRETTY_FUNCTION__。
    std::map<int, int>::iterator itEnd = m_emblemRechargeStoneMap.end();
    std::map<int, int>::iterator it = m_emblemRechargeStoneMap.find(stone->m_addInfo);
    if (it == itEnd)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0xad,
            "USE AVATAR RECHANGE : [item_id:%d] isn't RechargeStone [charac_no:%d]",
            stone->m_addInfo, user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->stoneSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->stoneSlot);
        return false;
    }

    const Inven_Item* avatar =
        user->getCurCharacInvenR()->GetInvenRef(2, (int)sig->avatarSlot);
    if (avatar == 0 || avatar->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->avatarSlot);
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->avatarSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->avatarSlot);
        return false;
    }

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
            avatar->m_addInfo2);
    // [b28d-solo-AvatarRechargeServer] 具名化基类上限字段（原 `*(int*)((char*)this+4)`
    // 裸偏移，§4.3 禁）；ORIG 0x08190a66 `jl <成功块>` 先 call 后读 +4 的求值序。
    // ORIG 0x08190a66 段：`call getSocketCount; mov 0x4(%edx),%edx; imul %eax,%edx`
    // ⇒ 书写序 count * limit（左操作数先求值，与 UseEmblem 同）。
    sig->maxEndurance = getSocketCount(socketData) * m_rechargeLimit;
    if ((int)(unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur >=
        sig->maxEndurance)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd8);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0xcd,
            "USE AVATAR RECHANGE : already max endurance [ui_id:%d] [charac_no:%d]",
            avatar->m_addInfo2, user->getCurCharacNo());
        return false;
    }
    return true;
}

// ============================================================================
// Recharge —— ORIG T 0x08190a70
// 删充值石 → 按 [emblem recharge stone info][itemId] 加耐久（clamp 上限）
// → 回传 addEndurance/enduranceAfter → DB 保存流。
// [b28d-solo-AvatarRechargeServer] NEAR（231=231，仅 4 行槽号差）：
// ①P59/P168a 负极性 guard（delete_item 失败块前置，ORIG 0x08190b5a `xor$1;test;je`）；
// ②P227 两处 cMyTrace 全表达式临时链 + P139 __PRETTY_FUNCTION__；
// ③P171 照抄 ORIG 只判 avatar->isEmpty()（无判空比较）；
// ④map 键具名物化槽（ORIG 0x08190cae `mov %eax,-0x3c`）。
// [b28d-L5a 2026-09-11] 残余 = 4.1/4.4 临时槽分配方向互换（N82/N93 族）：
//   ORIG  槽图 = save -0x54 / cMyTrace#1 -0x4c / key -0x3c / cMyTrace#2 -0x38
//   我方  槽图 = save -0x54 / key -0x4c / cMyTrace#1 -0x48 / cMyTrace#2 -0x38
// （4.1「先出现占深」，4.4 相反）。已试并回滚：①`int mapKey;` 声明提前到函数头
// （原形）与就地声明 ⇒ **槽不变**（-0x4c 恒定，与声明位置无关）；②其余槽完全一致
// （-0x38/-0x54 两侧相同）。cMyTrace 为匿名临时链（P227，ORIG 同形），无法具名化
// 翻槽；P167 callsite 25=25 CLEAR ⇒ 保持 NEAR（231=231），语义等价。
// ============================================================================
#include "AvatarRechargeServer/Recharge.inc"
                                    arad::SigAradUseAvatarRecharge* sig)
{
    const Inven_Item* stoneRef =
        user->getCurCharacInvenR()->GetInvenRef(1, (int)sig->stoneSlot);
    const int stoneItemId = stoneRef->m_addInfo;   // ORIG 判空前读取，忠实保序
    if (stoneRef == 0 || stoneRef->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }

    // [b28d-solo-AvatarRechargeServer] P59/P168a：ORIG 0x08190b5a
    // `xor $1,%eax; test %al,%al; je <成功块>` ⇒ 负极性 guard（成功路径 fall-through）。
    if (!user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, sig->stoneSlot, 1,
                            (eItemDelReason)3, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x13);
        ARAD::PRINT_ERROR::delete_item((char*)"USE AVATAR RECHANGE", user,
                                       sig->stoneSlot);
        return false;
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             sig->stoneSlot);
    // [b28d-solo-AvatarRechargeServer] P227 全表达式临时链 + P139 __PRETTY_FUNCTION__
    // （ORIG 0x08190bd0..0x08190c20 先求 varargs 再 call cMyTrace::C1）。
    cMyTrace(__PRETTY_FUNCTION__, 0xe8, 0)(
        "USE AVATAR RECHANGE : delete_item() success. [slot:%d] "
        "[charac_no:%d]",
        (int)sig->stoneSlot, user->getCurCharacNo());

    Inven_Item* avatar = user->getCurCharacInvenW()->GetInvenRef(2, (int)sig->avatarSlot);
    // ORIG 0x08190c50 只判 isEmpty（无 `avatar==0` 比较，原码直接解引用）⇒ P171 照抄。
    if (avatar->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->avatarSlot);
        return false;
    }

    // ORIG 0x08190cae `mov -0x24(%ebp),%eax; mov %eax,-0x3c(%ebp); lea -0x3c(%ebp),%eax`
    // ⇒ map 键经临时槽中转（4.1 对 operator[] const key& 的物化）。
    int mapKey;   // ORIG 0x08190cae 的 key 物化槽（-0x3c）
    int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur +
                   m_emblemRechargeStoneMap[mapKey = stoneItemId];
    if (newEndur > sig->maxEndurance)
        newEndur = sig->maxEndurance;
    sig->addEndurance =
        newEndur - (int)avatar->m_fieldSplit.m_avatarEndur;
    avatar->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;
    sig->enduranceAfter =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                             sig->avatarSlot);
    cMyTrace(__PRETTY_FUNCTION__, 0xfe, 0)(
        "USE AVATAR RECHARGE : success [slot:%d] [item_id:%d] "
        "[Endurance:%d] [charac_no:%d]",
        (int)sig->avatarSlot, avatar->m_addInfo,
        (int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur,
        user->getCurCharacNo());

    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)avatar->m_addInfo2;
    save.endurance =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;
    SaveAvatarEnduranceUnit(save);
    return true;
}

// ============================================================================
// UseEmblem —— ORIG T 0x08190ed8
// 徽章合成加耐久：累加 grades 在 [emblem recharge] 的数值并 clamp。
// [b28d-solo-AvatarRechargeServer] AE（181=181）：真语义 bug 修正 —— ORIG
// 0x08190f39 `lea 0x20(%eax),%edx` = Server+0x20 = m_emblemRechargeMap
// （旧源码误用 m_useEmblemGradeRateMap this+0x38）；另 P52 链式去 invenR +
// P227 trace 全表达式临时链 + P13 end 缓存 / P50 迭代器先声明后赋值。
// ============================================================================
#include "AvatarRechargeServer/UseEmblem.inc"
                                     Inven_Item* emblemItem, short slot)
{
    // [b28d-solo-AvatarRechargeServer] P52 链式：ORIG 0x08190ee8 `mov 0x14(%ebp),%eax;
    // mov 0x7(%eax),%ebx` 把 m_addInfo2 升格 %ebx 跨两 call 保活，无 invenR 具名槽。
    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
            emblemItem->m_addInfo2);
    // ORIG 0x08190f1b `call getSocketCount; mov 0x4(%edx),%edx; imul %edx,%eax`
    // ⇒ 原码书写序为 count * limit（具名基类字段，原裸偏移 §4.3 禁）。
    const int maxEndurance =
        getSocketCount(socketData) * m_rechargeLimit;

    int addSum = 0;
    // [b28d-solo-AvatarRechargeServer] 真语义修正：ORIG 0x08190f39/0x08190f7d
    // `lea 0x20(%eax),%edx` = Server+0x20 = m_emblemRechargeMap（[emblem recharge]），
    // 旧源码误用 m_useEmblemGradeRateMap（this+0x38）。循环形照 ORIG：迭代器先声明
    // 后赋值（P50，C1Ev 默认构造）+ end 缓存（P13）+ i/n 缓存。
    std::map<int, int>::iterator it;
    std::map<int, int>::iterator itEnd = m_emblemRechargeMap.end();
    for (std::vector<int>::size_type i = 0, n = grades.size(); i < n; ++i)
    {
        it = m_emblemRechargeMap.find(grades[i]);
        if (it != itEnd)
            addSum += it->second;
    }

    Inven_Item* avatar = user->getCurCharacInvenW()->GetInvenRef(2, (int)slot);
    if (avatar == 0 || avatar->isEmpty())
    {
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   (unsigned short)slot);
        return false;
    }

    int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur + addSum;
    if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    const int added = newEndur - (int)avatar->m_fieldSplit.m_avatarEndur;
    avatar->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);

    // [b28d-solo-AvatarRechargeServer] P227 全表达式临时链：ORIG 0x081910a2..0x081910e4
    // 先把 getCurCharacNo→%ebx、emblem 耐久→-0x64、item_id→%edi、slot→%esi 全部求值，
    // 再 call cMyTrace::C1，之后逐槽压 operator() 实参（P139 用 __PRETTY_FUNCTION__）。
    cMyTrace(__PRETTY_FUNCTION__, 0x142, 0)(
        "USE EMBLEM + RECHARGE : success [slot:%d] [item_id:%d] "
        "[Endurance:%d] [charac_no:%d]",
        (int)slot, avatar->m_addInfo,
        (int)(unsigned short)emblemItem->m_fieldSplit.m_avatarEndur,
        user->getCurCharacNo());

    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)avatar->m_addInfo2;
    save.endurance =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;
    SaveAvatarEnduranceUnit(save);
    return true;
}

// ============================================================================
// GmReduceEndurance —— ORIG T 0x081911b6 _ZN...17GmReduceEnduranceEP5CUserss
// ============================================================================
#include "AvatarRechargeServer/GmReduceEndurance.inc"
                                             short delta)
{
    if (slot < 0 || slot > 9)
        return;
    // [b23S3C] P48 去缓存：ORIG getCurCharacInvenW 结果 eax 直落实参槽
    // （0x32 mov %eax,(%esp); call GetInvenRef），无 invenW 槽 ⇒ 右值直调。
    Inven_Item* item = user->getCurCharacInvenW()->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                item->m_addInfo2);
    // [b23S3C] ORIG 无 sockets/limit 槽（getSocketCount→%ebx 跨 call 缓存 +
    // getLimit 返回 eax 直 imul），仅乘积落 -0x14 ⇒ 合并为单局部；调用顺序
    // getSocketCount 先、Singleton::Get 后（左→右求值同 ORIG）。
    const int maxEndurance =
        getSocketCount(socketData) *
        ARAD::Singleton<AvatarRechargeServer>::Get()
            ->getAvatarRecharageLimit();

    int newEndur = (int)item->m_fieldSplit.m_avatarEndur + (int)delta;
    if (newEndur < 0)
        newEndur = 0;
    else if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    item->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    // [b23S3C] ORIG 0x111 直压 3 参（target/user/slot）无 Get() call ⇒ static 调。
    UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);
    SaveAvatarEnduranceUnit(item->m_addInfo2,
                            (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);

    // [b23S3C] ORIG -0xc 槽存串指针两条（movl $str; mov →0x4 esp）⇒ char*
    // 具名局部（P0 槽往返），非 char 数组逐字节初始化。
    char* message = (char*)"succ avatar Endurance change";
    ARAD::notifySystemMessage(user, message);
}

// ============================================================================
// GmChangeEnduranceReduceRate —— ORIG T 0x08191318
// GM 前置校验后转发 GmSetEnduranceReduceRate（ORIG 经 Singleton::Get 取 this，
// Meyers 单例同一对象，语义一致）。
// ============================================================================
#include "AvatarRechargeServer/GmChangeEnduranceReduceRate.inc"
                                                       int rate)
{
    if (!user->isGMUser())
        return;
    // ORIG 0x08191318：静态（无 this），经 Singleton::Get 取实例后转发
    ARAD::Singleton<AvatarRechargeServer>::Get()->GmSetEnduranceReduceRate(
        user, enable, rate);
}

// ============================================================================
// GmSetEnduranceReduceRate —— ORIG T 0x08191364
// enable 写 +0x58；rate<0→0 / ≤100→×10 / >100→1000 写 +0x54；系统提示。
// ============================================================================
#include "AvatarRechargeServer/GmSetEnduranceReduceRate.inc"
                                                    int rate)
{
    // [b28c-L9] ORIG 0x0819136a `mov 0x10(%ebp),%eax; mov %al,-0x1c(%ebp)` 是 bool
    // 形参的单次截断 spill（非具名局部——再加 `bool bEn` 会多一对
    // `movzbl -0x1c,%eax; mov %al,-0x11` 中转）。
    m_bUseEnduranceReduceRate = enable;
    if (enable)
    {
        if (rate < 0)
            rate = 0;
        else if (rate > 100)
            rate = 1000;
        else
            rate *= 10;
        m_enduranceReduceRate = rate;

        // [b28c-L9] 旧「字面量直填 N 封顶」判词作废：ORIG 0x081913b6
        // `movl $str,-0x10(%ebp); mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)`
        // 与 0x081913db 的 -0xc 同形 ⇒ 原码是具名 char* 局部（P0），
        // 两臂各一枚（-0x10 深 / -0xc 浅）。
        char* msgOn = (char*)"succ avatar Endurance reduce rate mode on! change rate";
        ARAD::notifySystemMessage(user, msgOn);
    }
    else
    {
        m_enduranceReduceRate = 0;
        char* msgOff = (char*)"avatar Endurance reduce rate mode off";
        ARAD::notifySystemMessage(user, msgOff);
    }
}

// ============================================================================
// UpdateAvatarEndurance —— ORIG T 0x081913f6
// 组 0x220 包：uid 低 16 位 / slot / item_idx / 耐久计数，SendPacket 上报。
// [b28d-solo-AvatarRechargeServer] AE：P48 去 invenW 具名缓存
// （ORIG 0x0819142a `mov %eax,(%esp); call GetInvenRef` 结果右值直传实参槽）。
// ============================================================================
#include "AvatarRechargeServer/UpdateAvatarEndurance.inc"
                                                 CUser* user, short slot)
{
    if (slot < 0 || slot > 9)
        return;
    // [b28d-solo-AvatarRechargeServer] P48 去缓存：ORIG 0x0819142a `mov %eax,(%esp);
    // call GetInvenRef` 无 invenW 槽（getCurCharacInvenW 结果右值直传实参槽）。
    Inven_Item* item = user->getCurCharacInvenW()->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x220);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short((int)slot);
    packet.put_int(item->m_addInfo);
    packet.put_short(
        (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);
    packet.finalize(true);
    user->SendPacket(target, packet);
}

// ============================================================================
// AvatarRechargeServer —— ORIG C1 0x08190200 / D1 0x08190246 /
// Load 0x0819026c / Destroy 0x081902de / Test 0x08190266
// ============================================================================

#include "AvatarRechargeServer/dtor_AvatarRechargeServer.inc"
{
}

// ORIG 0x0819026c：cMyTrace(__PRETTY_FUNCTION__,0x22,9,true,false) +
// "\t- Loading Avatar Recharge script - "；loadScript()==0 即成功。
#include "AvatarRechargeServer/Load.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x22, 9, true, false);
    trace("\t- Loading Avatar Recharge script - ");
    if (loadScript())
        return false;
    return true;
}

// ORIG 0x081902de：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
#include "AvatarRechargeServer/Destroy.inc"
{
    return 1;
}

// ORIG 0x08190266：纯 ret。
#include "AvatarRechargeServer/Test.inc"
{
}

// ORIG 0x0819114c：非静态成员，find 于 this+0x38（= m_useEmblemGradeRateMap）。
#include "AvatarRechargeServer/GetRateFromEmblemGrade.inc"
{
    // [b28c-L9] ORIG 0x0819114c：find→-0x10、end→-0xc 两匿名槽 + 外链
    // `_Rb_tree_iterator::operator!=`（非 const 迭代器，P96）+ 正极性
    // `it != end` → `it->second`（P220/P47）。旧「const 迭代器 + ==end」形
    // 多生成转换构造与 operator== 外链（44 vs 34 insn）。
    std::map<int, int>::iterator it = m_useEmblemGradeRateMap.find(grade);
    if (it != m_useEmblemGradeRateMap.end())
        return it->second;
    return 0;
}

// ORIG 0x08190dea（静态）：Acquire 流 → make_internal_stream_jpn(0x31, -1) → put_binary 8 字节
// → DB_Q。不触碰 this，与 ORIG 一致保持静态。
#include "AvatarRechargeServer/SaveAvatarEnduranceUnit.inc"
{
    // [b28d-solo-AvatarRechargeServer] AE：ORIG（idx66 报告）Acquire 结果直传
    // CStreamGuard ctor 出参槽（P0 无具名局部）；make_internal_stream_jpn 后经
    // operator->() 再 put_binary。旧 DIFF 根因 = 实参冗余 `(char*)` cast 令 4.4
    // 提前把 &save 升格 %ebx 跨 call 保活；删 cast 写 `put_binary(&save, 8)` 后
    // 复现 ORIG `mov 0x8(%ebp),%edx; mov %edx,0x4(%esp)`（52=52，compare_tu AE）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/Arad_EmblemReformServer.cpp", 0x10f),
        true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x31, 0xffffffff);
    guard->put_binary(&save, 8);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08190eae（静态）：SigAradAvatarEnduranceSave ctor（清 +0/+4）后
// 写 uiId/endurance 两字段，走引用重载。
#include "AvatarRechargeServer/SaveAvatarEnduranceUnit_1.inc"
{
    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)key;
    save.endurance = (unsigned int)value;
    SaveAvatarEnduranceUnit(save);
}

// ============================================================================
// ARAD::SCRIPT::AvatarRecharge —— ORIG C1 0x088b0dd0 / D1 0x088b0e46 /
// loadScript 0x088b0eba / clearScript 0x088b12d8 / 访问器 0x88b1314..0x88b13a6
// ============================================================================

// ORIG C1：三个 map 默认构造 + 清 +0x4c 已装载标志（+0x00 上限不初始化）。
#include "AvatarRechargeServer/AvatarRecharge.inc"
    : m_loaded(false)
{
}

#include "AvatarRechargeServer/dtor_AvatarRecharge.inc"
{
}

// ORIG 0x088b12d8：上限清零 + 三 map clear。
#include "AvatarRechargeServer/clearScript.inc"
{
    m_rechargeLimit = 0;
    m_emblemRechargeStoneMap.clear();
    m_emblemRechargeMap.clear();
    m_useEmblemGradeRateMap.clear();
}

#include "AvatarRechargeServer/getAvatarRecharageLimit.inc"
{
    return m_rechargeLimit;   // ORIG return *(int*)this
}

#include "AvatarRechargeServer/IsEmblemRechargeStone.inc"
{
    // ORIG 0x088b13a6：+0x04 map find 命中返回 true。
    // P56：ORIG 尾部 choose 岛（test;je;mov$1;jmp;mov$0）⇒ if-return 形。
    if (m_emblemRechargeStoneMap.find(itemIndex) != m_emblemRechargeStoneMap.end())
        return true;
    return false;
}

#include "AvatarRechargeServer/getEmblemRechargeStoneInfo.inc"
{
    return &m_emblemRechargeStoneMap;   // ORIG return this+4
}

#include "AvatarRechargeServer/getEmblemRecharge.inc"
{
    // ORIG 0x088b132a：+0x1c map 值或 0。
    // b28d-lane25：P167 差集实证 ORIG 用非 const iterator + 先声明后赋值（C1Ev）
    // + `!=` 正极性（P50 反例复活 / P96）。
    std::map<int, int>::iterator it;
    it = m_emblemRechargeMap.find(grade);
    if (it != m_emblemRechargeMap.end())
        return it->second;
    return 0;
}

// ORIG 0x088b0eba loadScript。忠实转写：
//   已装载→0；打开失败→-1；重复键 printf（ORIG 无换行符）后分别 -2/-3/-4 且不卸载脚本；
//   主循环 ScanType 失败即 unloadRDARScriptFile + 置已装载 + 返回 0。
// [recharge use emblem info] 段 grade>=8 的行不消费数值直接退出内层（ORIG 同）。
#include "AvatarRechargeServer/loadScript.inc"
{
    if (m_loaded)
        return 0;

    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_recharge_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[avatar recharge limit]")
        {
            m_rechargeLimit = ScanInt((bool*)NULL);
        }
        else if (token == "[emblem recharge stone info]")
        {
            for (;;)
            {
                int idx = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                // b28d-lane25：P167 差集实证 ORIG 三段均为
                // `if (find==end) insert(value_type(k,v)); else {printf;return}`（eq 形）。
                if (m_emblemRechargeStoneMap.find(idx) == m_emblemRechargeStoneMap.end())
                {
                    m_emblemRechargeStoneMap.insert(
                        std::map<int, int>::value_type(idx, value));
                }
                else
                {
                    printf("[emblem recharge stone info] duplicate item_index:%d", idx);
                    return -2;
                }
            }
        }
        else if (token == "[emblem recharge]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_emblemRechargeMap.find(grade) == m_emblemRechargeMap.end())
                {
                    m_emblemRechargeMap.insert(
                        std::map<int, int>::value_type(grade, value));
                }
                else
                {
                    printf("[emblem recharge] duplicate grade:%d", grade);
                    return -3;
                }
            }
        }
        else if (token == "[recharge use emblem info]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                if (grade >= 8)
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_useEmblemGradeRateMap.find(grade) == m_useEmblemGradeRateMap.end())
                {
                    m_useEmblemGradeRateMap.insert(
                        std::map<int, int>::value_type(grade, value));
                }
                else
                {
                    printf("[recharge use emblem info] duplicate grade:%d", grade);
                    return -4;
                }
            }
        }
    }

    unloadRDARScriptFile();
    m_loaded = true;
    return 0;
}
