// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BossTowerDepsC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"            // DataManagerInitDepends.h 前置要求
#include "DataManagerInitDepends.h"  // TowerOfDespairMgr/TOD_Layer/TOD_UserState 权威布局

#include "CAssaultMgr.h"             // pvp_assault::CAssaultMgr 声明
#include "CGameManager.h"            // pvp_assault::CAssaultPlace 完整类型
#include "CUser.h"                   // CUser 方法集（含 CUserCharacInfo 继承面）
#include "CDungeon.h"                // CDungeon::m_entryRequiredItem（RequiredItem）
#include "CInventory.h"              // check_item_exist/GetInvenSlot/Inven_Item
#include "CParty.h"                  // CParty::get_member_count
#include "CPowerManager.h"           // CPowerManager::IsPowerWarEventOn
#include "GlobalData.h"              // s_systemTime_/s_power_manager
#include "CEnvironment.h"            // G_CEnvironment()->m_serverEnvir.m_fatigueTime
#include "GameWorld.h"               // move_area/send_to_area/G_GameWorld
#include "PacketGuard.h"

// ORIG _Z22CheckDailyScheduleTimeill T 0x0846c0a8；定义 GameUtils.cpp:88。
// 与 CBoosterGage.cpp:122 / CGameManager.cpp:114 同款局部声明口径（无权威头）。
#include "BossTowerDepsC2/CheckDailyScheduleTime.inc"

namespace pvp_assault
{

// ============================================================================
// pvp_assault::CForcedMoveCharacter —— 强制移动请求值对象（仅本 TU 使用，
// 类定义置于 .cpp 内避免跨 TU ODR 风险；后续 CExtraMatrix/CDynamicDisposition/
// CStaticDisposition::MoveUser(CForcedMoveCharacter*) 批次可迁至权威头）。
//
// 布局依据 ForcedMove.md 汇编读取偏移（this+0x08..+0x1c）；ORIG 唯一构造点
// （CAssaultPlace::ForcedMoveAtPowerWarPlace 栈上组装）不调用任何构造函数、
// 无 vtable 写入证据，故 +0x00..+0x07 未建模。
// ============================================================================
// pvp_assault::CForcedMoveCharacter 权威完整定义见 CAssaultMgr.h（本 TU 本地镜像已删，
// 避免与全域权威头 ODR 冲突）。以下 ForcedMove ORIG T 0x082e567c 报告全文转写。
// [b28d-L8] N82 | 块序 + 槽平移（ORIG 0x08xxxxxx）
#include "BossTowerDepsC2/ForcedMove.inc"
{
    // [M3S5 2026-09-11 · P222 落地] ORIG 0x082e567c 头部 `je +0x210` 直跳
    //   epilogue；早退形（`if (m_user == 0) return;`）我方多 2 条（`212: nop` +
    //   send_to_area 后的 `jmp 205`，our 160 > orig 158）；改「if (m_user != 0)
    //   正向包裹」后 our **158 = orig 158（gap 0）**、TU 四数零劣化 ⇒ 按 §6.4 保留。
    if (m_user != 0)
    {
        // ORIG：%al 标志直传（无内存 bool 中转）。先比村庄（movsbl 后有符号
        // 比较），不等即走 move_area；相等再比 get_area(false)，均等才原地
        // set_position。短路结果无槽物化 ⇒ P226 语句表达式（旧具名 int
        // needMove 落槽致帧/指令膨胀）。ORIG 0x082e567c。
        if (({ static_cast<int>(static_cast<signed char>(m_user->getCurCharacVill())) !=
                m_destVill ||
              m_user->get_area(false) != m_destArea; }))
        {
            G_GameWorld()->move_area(m_user, m_destVill, m_destArea, m_x, m_y,
                                     m_dir, false, 0, 0, 0);
        }
        else
        {
            m_user->set_position(static_cast<unsigned short>(m_x),
                                 static_cast<unsigned short>(m_y),
                                 static_cast<char>(m_dir));
        }

        PacketGuard packet;
        packet.put_header(0, 0x8a);
        packet.put_short(
            static_cast<int>(static_cast<unsigned short>(m_user->get_unique_id())));
        packet.put_short(m_x);
        packet.put_short(m_y);
        packet.put_byte(m_dir);
        packet.finalize(true);

        // [VerifyN2 2026-09-11 · 三轴免疫件实测翻正] 旧判 N82「块序封顶」作废：
        //   写成「否定前置空 then + else」即复现 ORIG 0x082e567c 的块序
        //   （send_to_area → ~PacketGuard → jmp epilogue；正向 if 形 4.4 把
        //   ~PacketGuard 放到 jmp 之后）⇒ 件级 DIFF→AE（158=158，ndiff 6→0）。
        if (m_destVill == 0 && m_destArea == 0)
        {
        }
        else
        {
            G_GameWorld()->send_to_area(m_destVill, m_destArea, packet);
        }
    }
}

// ============================================================================
// CAssaultPlace::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed6a6（静态成员，
// asm 无 this 槽）。报告全文转写：IsPowerWarEventOn() 且村庄==7 才处理；
// getPowerSide 两次独立调用（ORIG 指令流如此）；非 1/2 时其余字段不写
// （保持未初始化栈值，与 ORIG 一致）。
// ============================================================================
// [b28d-L8][W6] P226：ORIG 短路 bool 无槽物化（%eax 直传 + test %al,%al; je 尾）
// ⇒ 用语句表达式包住整个 && 条件（内不含具名变量），消掉 int enter 落槽
// （旧形帧 0x48 vs ORIG 0x38）。ORIG 0x082ed6a6。
#include "BossTowerDepsC2/ForcedMoveAtPowerWarPlace.inc"
{
    if (({ GlobalData::s_power_manager->IsPowerWarEventOn() &&
           user->getCurCharacVill() == 7; }))
    {
        CForcedMoveCharacter request;   // ORIG 栈对象，无构造调用
        request.m_user = user;
        if (user->getPowerSide() == 1)
        {
            request.m_destVill = 7;
            request.m_destArea = 0;
            request.m_x = 0x255;
            request.m_y = 300;
        }
        else if (user->getPowerSide() == 2)
        {
            request.m_destVill = 7;
            request.m_destArea = 4;
            request.m_x = 0x139;
            request.m_y = 0x13a;
        }
        request.ForcedMove();
    }
}

// ============================================================================
// CAssaultMgr::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed672（报告全文转写）：
//   try { CAassaultPlace::ForcedMoveAtPowerWarPlace(user); } catch(...) {}——
//   landing pad 取异常指针入局部后 __cxa_end_catch 吞掉。
// [b24F] DONE：catch 形参 void*→int（ORIG 解引用 (%eax),%eax = 值类型捕获）。
// ============================================================================
#include "BossTowerDepsC2/ForcedMoveAtPowerWarPlace_1.inc"
{
    try
    {
        CAssaultPlace::ForcedMoveAtPowerWarPlace(user);
    }
    catch (int caughtException)
    {
        // ORIG landing pad：__cxa_begin_catch 取异常对象内容（指针值）入栈
        // 局部后 __cxa_end_catch 吞掉——按 void* 值捕获同型。
        (void)caughtException;
    }
}

}  // namespace pvp_assault

// ============================================================================
// TOD_UserState::{getEnterCount,getLastClearTime} —— ORIG T 0x08643872/0x0864387e
// （两报告均为 2 条指令字段读；声明/字段见 DataManagerInitDepends.h）。
// ============================================================================
#include "BossTowerDepsC2/getEnterCount.inc"
{
    return m_data.m_enterCount;      // ORIG：*(int*)(this+0x10)
}

#include "BossTowerDepsC2/getLastClearTime.inc"
{
    return m_data.m_lastClearTime;   // ORIG：*(int*)(this+0x1c)
}

// ============================================================================
// TowerOfDespairMgr::CheckEnterable —— ORIG T 0x086440ee（报告全文转写）：
// [b23S3C] N：帧重排+跨 call 寄存器缓存（N82/N18a 族），语义等价已核（详见 b22_s3_verdicts.tsv）
//   user 空 → 0x15；level < m_minLevel(+0x34e) → 0xe；
//   TOD_UserState(getCharacExpandData 0xd).getEnterCount()>0 时：
//     非 GM 且 !CheckDailyScheduleTime(env.m_fatigueTime(+0x37c),
//     lastClearTime, curSec) → 0xf5；
//     check_item_exist(requiredItem1.m_0)==-1 → 0xf5；
//     GetInvenSlot(1,slot).get_add_info()(+0x07) < requiredItem1.m_4 → 0x11；
//   party 空且未走上方分支 → 0x15；member_count>1 → 0xab；否则 0。
//   （env+0x37c = Server_Envir::m_fatigueTime，CEnvironment.h 权威布局。）
// ============================================================================
// [b28d-L8] N82 | 帧差 + 参数/字段装载序（ORIG 0x08xxxxxx）
#include "BossTowerDepsC2/CheckEnterable.inc"
{
    if (user == 0)
    {
        return 0x15;
    }

    if (user->get_charac_level() < static_cast<int>(m_minLevel))
    {
        return 0xe;
    }

    TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    if (state->getEnterCount() > 0)
    {
        if (!user->isGMUser())
        {
            // ORIG：两值跨后续调用保活于 callee-saved（%esi/%ebx，无栈槽）⇒
            // 按实参右→左求值内联书写（P0/P52），勿落具名局部（旧形两槽
            // ⇒ 帧 0x60→0x78 且缺 push %esi/%ebx）。
            if (!CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                        state->getLastClearTime(),
                                        GlobalData::s_systemTime_.getCurSec()))
            {
                return 0xf5;
            }
        }

        // ORIG 无 inven 具名槽（两次独立 `getCurCharacInvenR()` 内联调用，
        // P0/P48）⇒ 内联成员链。ORIG 也无 Inven_Item 具名对象（直接对
        // GetInvenSlot 的匿名 sret 结果读 +0x7 = m_addInfo2）。
        int slot = user->getCurCharacInvenR()->check_item_exist(
            dungeon->m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
        {
            return 0xf5;
        }

        // [M3S5 2026-09-11 · P0 具名对象推翻旧判词] ORIG 0x08644214 直接
        //   `mov -0x4a(%ebp),%edx`（sret 目标 -0x51 的对象字段 +0x7）后
        //   `cmp %eax,%edx; jge +0x13d` —— **直跳**（无 setcc 物化）。
        //   4.4 对「调用返回的**临时对象**成员」参与比较恒物化 `setl;test;je`
        //   （/tmp/p_m3s5.cc 探针 f2：setl;test;je），而对**具名对象**字段比较
        //   直出 `jge`（探针 f8：call;mov -0x14(%ebp),%edx;cmp;jge，无额外槽）。
        //   ⇒ 原码是具名 Inven_Item 接收 GetInvenSlot 结果（sret 直写该对象，
        //   故 ORIG 无拷贝构造调用）。
        Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        if (slotItem.m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
        {
            return 0x11;
        }
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    // [M3S5 2026-09-11 · P222/P59 块序·落地] ORIG 0x0864423d `je 0864425b`（party==0
    //   → `mov $0x15` 块）、0x08644262 `mov $0x0` 是**最后一块且 fall-through 到出口**；
    //   早退形（`if (party == 0) return 0x15;`）生成相反块序 ⇒ 改「if (party != 0)
    //   {…} else { return 0x15; } return 0;」后 CheckEnterable **DIFF → AE**
    //   （112 = 112；TU 四数 ae 4→5、diff 3→2、near 1→0）。
    if (party != 0)
    {
        if (party->get_member_count() > 1)
        {
            return 0xab;
        }
    }
    else
    {
        return 0x15;
    }
    return 0;
}
