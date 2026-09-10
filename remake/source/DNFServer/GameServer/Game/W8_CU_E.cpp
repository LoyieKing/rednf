// ============================================================================
// W8_CU_E.cpp —— Wave8-CUser 残余 E 批：技能命令族 11 符号
// worklist /tmp/w8_batches/E_skill.txt（ORIG T/W 符号均由 nm 核对）
//
// 逐符号证据来源：
//   1. findSkillCommandData(int,ST&)      docs/class_func_reports/CUser/findSkillCommandData.md        ORIG T 0x08654b94
//   2. findSkillCommandData(int,ST&,bool) docs/class_func_reports/CUser/findSkillCommandData_08654e6a.md ORIG T 0x08654e6a
//   3. customizingSkillCommand(ST&)       docs/class_func_reports/CUser/customizingSkillCommand.md     ORIG T 0x08654f74
//   4. defaultSkillCommand()              docs/class_func_reports/CUser/defaultSkillCommand.md         ORIG T 0x086550ee
//   5. initTPSkill()                      docs/class_func_reports/CUser/initTPSkill.md                 ORIG T 0x0868fdbc
//   6. initAllSkill()                     docs/class_func_reports/CUser/initAllSkill.md                ORIG T 0x0868fc46
//   7. updateComboSkill()                 docs/class_func_reports/CUser/updateComboSkill.md            ORIG T 0x08692396
//   8. get_skill_count()                  docs/class_func_reports/CUser/get_skill_count.md             ORIG T 0x0866af86
//   9. set_skill_slot(char*,char*,int)    docs/class_func_reports/CUser/set_skill_slot.md              ORIG T 0x08662aba
//  10. st_monstorkill::Check(...)         报告缺失，objdump 直转（0x0823049c..0x08230668，W）
//  11. SIG_COMBO_SKILL::reset()           本批依赖（updateComboSkill 调用），objdump 直转
//                                         （0x08694d04..0x08694d69，W）；声明落 SigTypes.h。
// 头变更：CUser.h（st_monstorkill 注释→真实嵌套类 + initTPSkill/initAllSkill 声明）、
// SigTypes.h（SIG_COMBO_SKILL::reset 声明）；已 hub all 广播。
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
bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
{
    // b28d-35: DIFF→N82（帧/槽）。ORIG @0x08654b94 size 0x2d6。
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
        for (int i = 0; i < (signed char)hit->second.m_count; ++i)
        {
            // [L23-r2 P291 实测·本函数回退] 该函数的 `m_keyBytes[i]` 形反而
            //   209→212 条（本函数内 4 处访问点均被 4.4 拆成
            //   `mov 0xc(%ebp),%eax; lea 0x3(%eax),%edx; lea (%edx,%eax,1),%eax;
            //   movzbl (%eax)`），而 `(&x.m_key0)[i]` 形 209 条更接近 ORIG 199；
            //   同 TU 另两函数（overload-3 / customizingSkillCommand）改用视图后
            //   84→78、120→114（=ORIG）为净收益 ⇒ 仅本函数回退（实测为准）。
            unsigned char key = (&hit->second.m_key0)[i];
            (&out.m_key0)[i]  = key;
            if (key == 7)
                (&out.m_key0)[i] = 8;
        }
        return true;
    }

    // 未定制：取当前职业技能列表（ORIG G_CDataManager()+0x10 → CSkillList 实例指针）
    int job = get_charac_job();
    __gnu_cxx::hash_map<int, CSkill*>* list =
        G_CDataManager()->m_skillList->getSkillList(job);
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

    CSkill* skill = sit->second;          // node.first=键（与 skillId 相同）、second=CSkill*
    size_t n = skill->m_keyCommandList.size();
    if (!(n < 6) || n == 0)
    {
        if (n != 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x2102, 5);
            trace("SKILL_COMMAND_LOG : ABNOMAL COMMAND CNT!! (%d) (%d)",
                  (int)sit->first, (int)n);
        }
        return false;
    }

    out.m_command = (unsigned short)sit->first;
    out.m_count   = (unsigned char)n;
    for (size_t i = 0; i < n; ++i)
    {
        // ORIG 读第 i 个 STKeyCommandUnit.m_ids[0] 低字节作命令键位
        int v = skill->m_keyCommandList[i].m_ids[0];
        (&out.m_key0)[i] = (unsigned char)v;
        if ((unsigned char)v == 7)
            (&out.m_key0)[i] = 8;
    }
    return true;
}

// ---------------------------------------------------------------------------
// 2. bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out, bool flag)
//    @0x08654e6a（仅查玩家命令表重载；flag 参与 Ghidra 尾参杂散，逻辑不依赖）
//    [b28d] 返回类型 int→bool（ORIG 调用点 0x081bf7d1 test %al 直判，P292；mangled 不变）。
bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out, bool flag)
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
int CUser::customizingSkillCommand(STSkillCommandData& data)
{
    // b28d-35: DIFF→N82/N7（帧 0x54 vs 0x58；寄存器分配差）。ORIG @0x08654f74。
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
bool CUser::defaultSkillCommand()
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
void CUser::initTPSkill()
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
void CUser::initAllSkill()
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
void CUser::updateComboSkill()
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
int CUser::get_skill_count()
{
    // ORIG 0x0866af86：getCurCharacSkillR 结果直接内联为链式 receiver（P0/P24），
    // 具名局部 slot 触发 -0xc 槽往返 + 帧 0x28 扩张。
    return ((SkillSlot*)getCurCharacSkillR())->get_skill_count();
}

// ---------------------------------------------------------------------------
// 9. void CUser::set_skill_slot(char* data1, char* data2, int size) @0x08662aba
void CUser::set_skill_slot(char* data1, char* data2, int size)
{
    // P0：ORIG 无 slot 槽，getCurCharacSkillW 结果直传（%eax→(%esp)）。
    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot(data1, data2, size);
}

// ---------------------------------------------------------------------------
// 10. bool CUser::st_monstorkill::Check(unsigned int nowTick, unsigned short nx,
//         unsigned short ny, unsigned short xLim, unsigned short yLim,
//         unsigned int windowLimit)
//     @0x0823049c size 0x1cd（objdump 直转）
bool CUser::st_monstorkill::Check(unsigned int nowTick, unsigned short nx,
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

    if ((int)(short)m_deltaX >= (int)xLim)                // 82305e6..f3: jge → true 块
        return true;
    if ((int)(short)m_deltaY < (int)yLim)                 // 82305ff..05: jl → false 块
        return false;
    return true;
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
    return false;                                         // 823065e: mov $0x0,%eax
}

// ---------------------------------------------------------------------------
// 11. void SIG_COMBO_SKILL::reset() @0x08694d04（objdump 直转；
//     ctor _ZN15SIG_COMBO_SKILLC1Ev @0x0848fbbe 写序一致）
void SIG_COMBO_SKILL::reset()
{
    m_characNo = 0;
    m_tree1Count = 0;
    m_tree2Count = 0;
    m_job = 0xb;
    memset(m_tree1Data, 0, 0x54);
    memset(m_tree2Data, 0, 0x54);
}
