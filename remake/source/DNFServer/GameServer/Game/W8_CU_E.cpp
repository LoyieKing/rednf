// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_CU_E/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <string.h>
#include <stdlib.h>

#include "CUser.h"
#include "SigTypes.h"
#include "CDataManager.h"
#include "CSkill.h"
#include "SkillSlot.h"
#include "GameTypes.h"          // cMyTrace
#include "CGameManager.h"       // G_CGameManager()
#include "CPremiumLetheManager.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GlobalData.h"

static_assert(sizeof(CUser::st_monstorkill) == 0x14, "st_monstorkill ABI drift");

// ---------------------------------------------------------------------------
// 1. bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
//    @0x08654b94（findSkillCommandData.md ##汇编 + ##反编译 C 逐行转写）
#include "W8_CU_E/findSkillCommandData.inc"
{
    // b28d-35: DIFF→N82（帧/槽）。ORIG @0x08654b94 size 0x2d6。
    // [M3S1 2026-09-11 本波总读数] 209 → **199 = ORIG 199（gap 0，ORIG-only/OURS-only 皆空）**。
    //   四旋钮：①两处循环上界 `hit->second.m_count`→**out**.m_count（P0，ORIG 8654c72 实证）；
    //   ②`m_key0` 裸下标 → union `m_keyBytes[i]` 数组视图 + 重读 out（P291/P0，消 4 步 lea）；
    //   ③未定制分支 `size_t n` → **int n** 且条件 `n > 5 || n <= 0`（P196/P290，ORIG jg/setl 有符号）；
    //   ④`int job` 具名局部删 → `getSkillList(get_charac_job())` 右值直传（P0，ORIG %ebx 保活无槽）。
    //   残余 ndiff=102 全为**槽位映射**（我方 cmds=-0x28/迭代器=-0x38/-0x34 vs ORIG -0x24/-0x54/-0x50）：
    //   迭代器/类对象槽受 P331 约束不可按 P339 提头；**帧同为 `sub $0x74`**（逐值相同）
    //   ⇒ 判 N82/N93 族（方向已无 gap，槽位映射不可控）。
    // [旧判词作废] axisB_points.tsv 的 0x8654d47/0x8654d4d 真分叉（ORIG `jg` 有符号 vs
    //   我方 `ja` 无符号）**本波已由 `int n` + `n > 5 || n <= 0` 修掉**（旧记录「改符号性
    //   209→211 劣化」是在 `unsigned char key` 具名局部仍在时测得；删 key 局部后该冲突消失）。
    // 命令表 getter（CUserCharacInfo +0x1254 区容器）：std::map<int,STSkillCommandData>*
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    std::map<int, STSkillCommandData>::iterator hit = cmds->find(skillId);
    if (hit != cmds->end())
    {
        out.m_command = hit->second.m_command;
        out.m_count   = hit->second.m_count;
        // [M3S1 本波实测·P0] ORIG 8654c72 上界为 `mov 0x10(%ebp),%eax;
        //   movzbl 0x2(%eax),%eax; movsbl %al,%eax; cmp -0x20(%ebp),%eax; setg`
        //   ⇒ 读 **out**.m_count（第二形参），不是 hit->second.m_count
        //   （后者要多一次 ptEv 调用 + lea，我方原多 1 条）。
        for (int i = 0; i < (signed char)out.m_count; ++i)
        {
            // [旧判词作废] 曾记「m_keyBytes 视图 209→212 / 删 key 局部 209→214」而保留
            //   裸下标；本波复核：当时劣化来自**同处仍读 hit->second.m_count**（多一次
            //   ptEv+lea）与 `size_t` 无符号序的叠加，非视图本身。
            // [M3S1 本波实测·P291/P0] ORIG 8654c37..8654c69 逐条为
            //   `movzbl 0x7(%eax,%edx,1)`（单条 disp+scaled 读 hit->second.m_keyBytes[i]）
            //   → `mov %dl,0x3(%eax,%ebx,1)`（单条写 out.m_keyBytes[i]）
            //   → 重读 `movzbl 0x3(%edx,%eax,1)` 与 7 比较，全程无 key 槽
            // ⇒ 用 union 数组视图 + 重读目标表达式；裸下标 `(&x.m_key0)[i]` 4.4 会发 lea 链。
            out.m_keyBytes[i] = hit->second.m_keyBytes[i];
            if (out.m_keyBytes[i] == 7)
                out.m_keyBytes[i] = 8;
        }
        return true;
    }

    // 未定制：取当前职业技能列表（ORIG G_CDataManager()+0x10 → CSkillList 实例指针）
    // [M3S1 本波实测·P0] ORIG 8654c96 `call get_charac_job; mov %eax,%ebx; call
    //   G_CDataManager; … mov %ebx,0x4(%esp)` —— job 值无槽往返（%ebx 保活）⇒ 原码
    //   是**表达式右值直传**（4.4 对 `receiver->f(arg)` 先求 arg 再求 receiver，与
    //   ORIG 同序）；具名 `int job` 局部多出 `mov %eax,-0xN` + `mov -0xN,%edx`（+1 条）。
    __gnu_cxx::hash_map<int, CSkill*>* list =
        G_CDataManager()->m_skillList->getSkillList(get_charac_job());
    if (list == 0)
        return false;

    __gnu_cxx::hash_map<int, CSkill*>::iterator sit = list->find(skillId);
    if (sit == list->end())
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x2113, 5);
        trace("SKILL_COMMAND_LOG : KEY INDEX DUPLICATE!!");
        return false;
    }

    // [M3S1 本波实测·P0] ORIG 8654d1a/8654d26 为**两次** ptEv：`mov 0x4(%eax),%eax;
    //   mov %eax,-0x18` （skill）与 `mov (%eax),%eax; mov %eax,-0x14`（key）⇒ 原码有
    //   具名 key（int）；我方原用 `sit->first` 表达式在三处使用 ⇒ 多两次 ptEv（-3 条）。
    CSkill* skill = sit->second;          // node.first=键（与 skillId 相同）、second=CSkill*
    int key = sit->first;
    // [M3S1 本波实测·P196/P290] ORIG 8654d43..8654d4d 为 `cmpl $0x5,-0x10(%ebp);
    //   jg 8654d4f` + `cmpl $0x0,-0x10(%ebp); jg 8654da3`（**有符号**），循环底
    //   8654e19 `cmp -0x10(%ebp),%eax; setl`（有符号）⇒ n 是 int，条件写
    //   `n > 5 || n <= 0`（`!(n<6)` 在 size_t 下产 `ja`，且 `n >= 6` 产 jge 而非 jg）。
    int n = (int)skill->m_keyCommandList.size();
    if (n > 5 || n <= 0)
    {
        if (n != 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x2102, 5);
            trace("SKILL_COMMAND_LOG : ABNOMAL COMMAND CNT!! (%d) (%d)",
                  key, (int)n);
        }
        return false;
    }

    out.m_command = (unsigned short)key;      // ORIG 8654da3 从 -0x14 槽读
    out.m_count   = (unsigned char)n;
    // [轴 B 实测·本函数不适用] ORIG 0x08654e19 虽为 `cmp -0x10(%ebp),%eax; setl`
    // 有符号序（P196 前置通过），但本函数改 `int i; i < (int)n` 实测
    // **209→211 劣化**（`unsigned char key` 具名局部的 `movzbl` 链与 int 循环
    // 的寄存器分配冲突）⇒ 回退 `size_t`；残差归 N82/N93（槽向 + getSkillCommand
    // 具名缓存不可去），已按 P196/S1「一次旋钮、无改善即回滚」留痕。
    for (int i = 0; i < n; ++i)
    {
        // [M3S1 本波实测·P291/P0] ORIG 8654dc2..8654e21 同族：单条 scaled 读
        //   `mov (%eax),%eax`（vector<int>::operator[] 结果直传，无 v 槽）→ 单条写
        //   out.m_keyBytes[i] → 重读 out.m_keyBytes[i] 与 7 比较 ⇒ 删 `int v` 具名局部，
        //   表达式右值直传 + 数组视图。
        out.m_keyBytes[i] = (unsigned char)skill->m_keyCommandList[i].m_ids[0];
        if (out.m_keyBytes[i] == 7)
            out.m_keyBytes[i] = 8;
    }
    return true;
}

// ---------------------------------------------------------------------------
// 2. bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out, bool flag)
//    @0x08654e6a（仅查玩家命令表重载；flag 参与 Ghidra 尾参杂散，逻辑不依赖）
//    [b28d] 返回类型 int→bool（ORIG 调用点 0x081bf7d1 test %al 直判，P292；mangled 不变）。
#include "W8_CU_E/findSkillCommandData_1.inc"
{
    // b28d-35: DIFF→N82（帧 0x34 vs 0x48、槽平移；bool 形参截断槽）。ORIG @0x08654e6a。
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    std::map<int, STSkillCommandData>::iterator it = cmds->find(skillId);
    // P222/P59：ORIG 0x08654ed3 `test %al,%al; je 8654f69`（**尾部** `mov $0x0,%eax`
    // 岛）+ 主体 fall-through 到 `mov $0x1,%eax; jmp epilogue` ⇒ 源码是**正向包裹**
    // `if (it != end) { 主体; return true; } return false;`；原「早返 return false +
    // 尾部 return true」形 4.4 恒产反向岛。
    if (it != cmds->end())
    {
        out.m_command = it->second.m_command;
        out.m_count   = it->second.m_count;
        // P0：ORIG 0x08654f4e 循环上界读 **out**+0x2（`mov 0x10(%ebp),%eax;
        // movzbl 0x2(%eax),%eax; movsbl %al,%eax; cmp -0xc(%ebp),%eax`）而非
        // it->second ⇒ 上界表达式须写 out.m_count。
        for (int i = 0; i < (signed char)out.m_count; ++i)
        {
            // P0/P291：ORIG 0x08654f13..0x08654f3d 为「读 it->second.m_keyBytes[i] →
            // 写 out.m_keyBytes[i] → **重读 out.m_keyBytes[i]** 与 7 比较」，全程无中间
            // 字节槽（我方原 `key` 局部多出 -0x9 槽 + `mov %al,-0x9`/`cmpb $7,-0x9`
            // 三处偏移）⇒ 删具名局部、条件处重读目标表达式。
            out.m_keyBytes[i] = it->second.m_keyBytes[i];
            if (out.m_keyBytes[i] == 7)
                out.m_keyBytes[i] = 8;
        }
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// 3. int CUser::customizingSkillCommand(STSkillCommandData& data)
//    @0x08654f74
#include "W8_CU_E/customizingSkillCommand.inc"
{
    // b28d-35: DIFF→N82/N7（帧 0x54 vs 0x58；寄存器分配差）。ORIG @0x08654f74。
    // [M3S1 2026-09-11 复核·登记勿动] show_func_diff：orig=114 our=114 **gap=0 ndiff=0**
    //   （逐指令全等；含 ORIG 0x0865502b/0x08655040 的 operator-> 双取、0x086550a3
    //   `make_pair<short&, T&>` 左值 data.m_command 实参形、0x086550b8 pair 转换 ctor）
    //   ⇒ 本件已是 ORIG 形，P212/P309/P339 均无可转之旋钮。
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    // 先还原 ORIG 的占位键 0x8 → 0x7，再以上下文键寻位
    for (int i = 0; i < (signed char)data.m_count; ++i)
    {
        // [L23-r2 P291] 同族：具名数组视角 `m_keyBytes[5]`（ORIG 单条 disp+scaled）。
        if (data.m_keyBytes[i] == 8)
            data.m_keyBytes[i] = 7;
    }

    int key = (int)(short)data.m_command;    // ORIG cwtl 有符号扩展
    std::map<int, STSkillCommandData>::iterator it = cmds->find(key);
    if (it != cmds->end())
    {
        it->second.m_command = data.m_command;
        it->second.m_count   = data.m_count;
        for (int i = 0; i < (signed char)data.m_count; ++i)
            it->second.m_keyBytes[i] = data.m_keyBytes[i];
    }
    else
    {
        // ORIG 0x086550?? make_pair 实参为 **data.m_command 左值**（mangled
        // `_ZSt9make_pairIRsR18STSkillCommandDataE` = make_pair<short&, T&>；
        // arg1 直接传 data 指针 = &data.m_command，因 m_command 偏移 0）
        // ⇒ 须写成员表达式而非具名 int key 局部（后者产 pair<int&,T&> 另一实例）。
        cmds->insert(std::make_pair(data.m_command, data));
    }
    enableSaveSkill();
    return true;
}

// ---------------------------------------------------------------------------
// 4. bool CUser::defaultSkillCommand() @0x086550ee
#include "W8_CU_E/defaultSkillCommand.inc"
{
    // ORIG 0x086550ee：R=(cmds==0||empty) 常量岛 mov$1/mov$0 落 eax 直测
    // （test %al; je body），无具名槽；空/无 → return false，非空 →
    // clear+enableSaveSkill+return true（P56 岛形直测）。
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    // N96 清绑：原 `register bool emptyOrNone asm("eax")` 已删。ORIG 该 bool 无槽
    // （cmpl $0,cmds;je L1;call empty;test %al;je L2;L1:mov$1;L2:mov$0;test %al;je）
    // ⇒ 按 P226扩展/P316① 用语句表达式包住短路条件。清绑前后 30=30、与 ORIG 逐条
    // 全等（仅 .o 重定位地址差）⇒ 残差 0；具名 bool 局部形为 31（+1 槽物化）。
    if (({ (cmds == 0) || cmds->empty(); }))
        return false;
    cmds->clear();
    enableSaveSkill();
    return true;
}

// ---------------------------------------------------------------------------
// 5. void CUser::initTPSkill() @0x0868fdbc
#include "W8_CU_E/initTPSkill.inc"
{
    // ORIG 0x0868fdbc：直接读 getCurCharacR()->m_field7b(+0x7b) 与
    // 0x500723d0 比较（setl），不经 getCurCharacLastPlayTick（NEW 曾用
    // <= 0x500723cf，等价但字节不同）。
    // P0：ORIG 有 threshold 槽（movl $0x500723d0,-0xc 预置）；SkillSlot 右值直传。
    int tpSkillLimit = (int)0x500723d0;
    if (getCurCharacR()->m_field7b < tpSkillLimit)
    {
        ((SkillSlot*)getCurCharacSkillW())->initialize_sfp_skill();
        send_skill_info();
    }
}

// ---------------------------------------------------------------------------
// 6. void CUser::initAllSkill() @0x0868fc46
#include "W8_CU_E/initAllSkill.inc"
{
    // ORIG 0x0868fc46：直接读 getCurCharacR()->m_field7b(+0x7b) 与
    // 0x50c79ed0 比较（setl），不经 getCurCharacLastPlayTick 转发。
    // P0：ORIG threshold 槽双写（0x0868fc5b: movl $0x50076318 后立即
    // movl $0x50c79ed0 覆写同槽）⇒ 源码「声明=旧值 + 覆写」两句式（P197③ 族）。
    int allSkillLimit = (int)0x50076318;
    allSkillLimit = (int)0x50c79ed0;
    if (getCurCharacR()->m_field7b < allSkillLimit)
    {
        ((SkillSlot*)getCurCharacSkillW())->clear_all_skills((ENUM_SKILL_TREE_KIND)0);
        ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
        ((SkillSlot*)getCurCharacSkillW())->InitSkill(false, (ENUM_SKILL_TREE_KIND)0);

        ((SkillSlot*)getCurCharacSkillW())->clear_all_skills((ENUM_SKILL_TREE_KIND)1);
        ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
        ((SkillSlot*)getCurCharacSkillW())->InitSkill(true, (ENUM_SKILL_TREE_KIND)1);

        if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
        {
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (lethe != 0)
            {
                SetOneDayLetheFlag(CPremiumLetheManager::NOT_USE_LETHE_STATE);
                lethe->BackupSkill(this, (ENUM_SKILL_TREE_KIND)0);
                SetOneDayLetheFlag2ND(CPremiumLetheManager::NOT_USE_LETHE_STATE);
                lethe->BackupSkill(this, (ENUM_SKILL_TREE_KIND)1);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// 7. void CUser::updateComboSkill() @0x08692396
#include "W8_CU_E/updateComboSkill.inc"
{
    if (get_charac_job() != 9)
        return;
    // ORIG 0x08692396：Acquire 结果 %eax 直传 ctor（P0 无 stream 槽）；
    // 每 op<< 前 operator* 重取（P48），put 前 operator-> 重取。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xb697),
                       true);
    guard.operator*()->operator<<((int)0x31d);
    guard.operator*()->operator<<((int)-1);
    // ORIG：operator-> 重取 → GetInBuffer call → sig 落槽 -0xc → reset() →
    // getCurCharacSkillR → getComboSkillSaveData(sig)。
    SIG_COMBO_SKILL* sig = guard.operator->()->GetInBuffer<SIG_COMBO_SKILL>();
    sig->reset();
    ((SkillSlot*)getCurCharacSkillR())->getComboSkillSaveData(sig);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// 8. int CUser::get_skill_count() @0x0866af86
#include "W8_CU_E/get_skill_count.inc"
{
    // ORIG 0x0866af86：getCurCharacSkillR 结果直接内联为链式 receiver（P0/P24），
    // 具名局部 slot 触发 -0xc 槽往返 + 帧 0x28 扩张。
    return ((SkillSlot*)getCurCharacSkillR())->get_skill_count();
}

// ---------------------------------------------------------------------------
// 9. void CUser::set_skill_slot(char* data1, char* data2, int size) @0x08662aba
#include "W8_CU_E/set_skill_slot.inc"
{
    // P0：ORIG 无 slot 槽，getCurCharacSkillW 结果直传（%eax→(%esp)）。
    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot(data1, data2, size);
}

// ---------------------------------------------------------------------------
// 10. bool CUser::st_monstorkill::Check(unsigned int nowTick, unsigned short nx,
//         unsigned short ny, unsigned short xLim, unsigned short yLim,
//         unsigned int windowLimit)
//     @0x0823049c size 0x1cd（objdump 直转）
#include "W8_CU_E/Check.inc"
                                  unsigned short ny, unsigned short xLim,
                                  unsigned short yLim, unsigned int windowLimit)
{
    // ORIG 块布局（82304cc `ja 823060e` / 82305f3 `jge 8230607` / 8230605 `jl 823065e`）：
    // 超窗 reset 块落在**函数尾部**（823060e..823065a，紧接尾 `mov $0x0,%eax`），
    // 且 `return true` 块（8230607 `mov $0x1,%eax; jmp epilogue`）夹在主体与 reset
    // 块之间 ⇒ 原码用 **goto 尾标签**（P283/P223 形），非「早返块内联 + 尾部 return」。
    if ((unsigned int)(nowTick - m_lastTick) > windowLimit)   // ORIG 82304cc: cmp/ja 无符号超窗
        goto L_reset;

    ++m_count;                                            // ORIG 82304dd: (+0xc)+1

    if ((int)(short)m_minX - (int)(short)nx > 0)          // 82304e9: movswl/cwtl 32位有符号差
        m_minX = nx;
    if ((int)(short)m_minY - (int)(short)ny > 0)          // 8230521
        m_minY = ny;
    if ((int)(short)m_maxX - (int)(short)nx < 0)          // 823053d: 旧值<新值才更新（保 max）
        m_maxX = nx;
    if ((int)(short)m_maxY - (int)(short)ny < 0)          // 823055f
        m_maxY = ny;

    m_deltaX = (short)((unsigned short)m_maxX - (unsigned short)m_minX);  // 8230577: 16位差直存 +0x10
    m_deltaY = (short)((unsigned short)m_maxY - (unsigned short)m_minY);  // 8230594 16位差直存 +0x12

    m_deltaX = (short)std::abs((int)(short)m_deltaX);     // 82305b9: cwtl/sar31/xor/sub 绝对化回写
    m_deltaY = (short)std::abs((int)(short)m_deltaY);     // 82305c9

    // ORIG 82305f3 `jge 8230607`（true 出口被 A 真与 B 假双到达）+ 8230605 `jl 823065e`
    // （false 出口在 reset 块**之后**，即 `jl` 直跳末尾 mov $0）⇒ 源形是
    // `if (A || B) return true; else goto L_false;`（短路 `||`；B 为 yLim 侧的 `>=`）。
    // [M3S1 实测] 拆成两个 `if (x) goto` 派 4.4.7 在标签处插 nop+jmp（162）；合并 `||` 后
    //   `jge L_true`/`jl L_false` 与 ORIG 逐条同构（f14 探针 /tmp/m3s1/）。
    if (((int)(short)m_deltaX >= (int)xLim)               // 82305e6..f3: jge → true 块
        || ((int)(short)m_deltaY >= (int)yLim))           // 82305ff..05: jl → false 块
    {
        return true;
    }
    else
    {
        goto L_false;
    }
L_reset:
    // 超窗重置检测框（823060e..823065a，fall-through 到尾 mov $0x0）
    m_deltaX = 0;
    m_deltaY = 0;
    m_count = 0;
    m_lastTick = nowTick;
    m_minX = nx;
    m_minY = ny;
    m_maxX = nx;
    m_maxY = ny;
L_false:
    return false;                                         // 823065e: mov $0x0,%eax
}

// ---------------------------------------------------------------------------
// 11. void SIG_COMBO_SKILL::reset() @0x08694d04（objdump 直转；
//     ctor _ZN15SIG_COMBO_SKILLC1Ev @0x0848fbbe 写序一致）
#include "W8_CU_E/reset.inc"
{
    m_characNo = 0;
    m_tree1Count = 0;
    m_tree2Count = 0;
    m_job = 0xb;
    memset(m_tree1Data, 0, 0x54);
    memset(m_tree2Data, 0, 0x54);
}
