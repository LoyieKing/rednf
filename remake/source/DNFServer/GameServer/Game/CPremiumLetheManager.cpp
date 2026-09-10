// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPremiumLetheManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPremiumLetheManager（G2-4 批次，忘川水/付费技能重置管理）
// 参照 docs/class_func_reports/CPremiumLetheManager.md 与 ORIG 反汇编逐函数实现。
// 原源文件 PremiumLetheManager.cpp；数据全局态经 DB 队列（_update）下账。
#include <string.h>

#include <vector>

#include "CDataManager.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "SkillSlot.h"
#include "StreamPool.h"
#include "GameWorld.h"
#include "PacketGuard.h"

// ---- 全局单例（CUserCharacInfo.h 已声明 namespace GlobalData；不引入
//      GlobalData.h 以避免 class/namespace 形态冲突） ----
namespace GlobalData
{
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
}

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；构造/析构由
// GameStubs 提供桩符号，本 TU 只声明不定义）
// ============================================================================


// ---- 忘川水状态（ORIG 数据段 .data 0x93abc60 / .bss 0x9472dbc）----
class CPremiumLetheManager
{
public:
    CPremiumLetheManager();
    ~CPremiumLetheManager();

    bool ResetSkillReq(CUser* user);
    bool ConfirmSkillReq(CUser* user);
    bool BackToPre(CUser* user, _Mastered_skill* skills,
                   ENUM_SKILL_TREE_KIND kind);
    bool UpdateBackupSkillFlag(CUser* user, ENUM_SKILL_TREE_KIND kind);
    bool InitLetheSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _resetSkill(CUser* user);
    void InitSkill(CUser* user, bool flag, ENUM_SKILL_TREE_KIND kind);
    bool BackupSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _update(CUser* user, unsigned char state, ENUM_SKILL_TREE_KIND kind);
    void _removeInitSkill(CUser* user, _Mastered_skill* skills);

    static unsigned char USE_LETHE_STATE;       // +0x093abc60 = 1
    static unsigned char SKILL_CONFIRM_STATE;   // +0x093abc61 = 2
    static unsigned char BACK_TO_PRE_STATE;     // +0x093abc62 = 3
    static unsigned char INIT_LETHE_SKILL;      // +0x093abc63 = 4
    static unsigned char NOT_USE_LETHE_STATE;   // +0x09472dbc = 0
};

#include "CPremiumLetheManager/CPremiumLetheManager.inc"
{
}

unsigned char CPremiumLetheManager::USE_LETHE_STATE = 1;
unsigned char CPremiumLetheManager::SKILL_CONFIRM_STATE = 2;
unsigned char CPremiumLetheManager::BACK_TO_PRE_STATE = 3;
unsigned char CPremiumLetheManager::INIT_LETHE_SKILL = 4;
unsigned char CPremiumLetheManager::NOT_USE_LETHE_STATE = 0;

// SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD 权威定义已迁移至 SigTypes.h
// （FixMissingSigTypes；DB 队列负载，pack(1)，sizeof=0x1a7）。

// ---- CUser 方法声明见 CUser.h（本 TU 直接真实调用） ----

#include "CPremiumLetheManager/dtor_CPremiumLetheManager.inc"
{
}

#include "CPremiumLetheManager/ResetSkillReq.inc"
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    _resetSkill(user);
    return true;
}

#include "CPremiumLetheManager/ConfirmSkillReq.inc"
{
    if (user == 0)
    {
        return false;
    }
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x35, 0)(
        "ONE_DAY_LETHE : CONFIRM_SKILL, char(%s)", user->getCurCharacName());
    _update(user, SKILL_CONFIRM_STATE,
            (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
    return true;
}

#include "CPremiumLetheManager/BackToPre.inc"
                                     ENUM_SKILL_TREE_KIND kind)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    InitSkill(user, false, kind);
    if (!user->master_new_skills(skills, kind))
    {
        return false;
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        user->SetOneDayLetheFlag(NOT_USE_LETHE_STATE);
    }
    else
    {
        user->SetOneDayLetheFlag2ND(NOT_USE_LETHE_STATE);
    }
    cMyTrace(__PRETTY_FUNCTION__,
             0x4f, 0)("ONE_DAY_LETHE : SUCCESS_BACK_TO_PRE, char(%s)",
                      user->getCurCharacName());
    return true;
}

#include "CPremiumLetheManager/UpdateBackupSkillFlag.inc"
                                                 ENUM_SKILL_TREE_KIND kind)
{
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (user->GetOneDayLetheFlag() != NOT_USE_LETHE_STATE)
        {
            return false;
        }
    }
    else
    {
        if (user->GetOneDayLetheFlag2ND() != NOT_USE_LETHE_STATE)
        {
            return false;
        }
    }
    _update(user, NOT_USE_LETHE_STATE, kind);
    cMyTrace(__PRETTY_FUNCTION__,
             0x69, 0)("ONE_DAY_LETHE : SUCCESS_INIT_FOR_LEGACY_LETHE, char(%s)",
                      user->getCurCharacName());
    return true;
}

#include "CPremiumLetheManager/InitLetheSkill.inc"
                                          ENUM_SKILL_TREE_KIND kind)
{
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        return false;
    }
    _update(user, INIT_LETHE_SKILL, kind);
    cMyTrace(__PRETTY_FUNCTION__,
             0x75, 0)("ONE_DAY_LETHE : SUCCESS_INIT_FOR_1ST_GROW_CHAR, char(%s)",
                      user->getCurCharacName());
    return true;
}

// [b28d-solo 2026-09-11] ORIG 0x085c40aa | 101 / our 102（gap −1）。全条比对仅 1 条差：
// ORIG 0x085c41d1..0x085c41dc `call PacketGuard::~PacketGuard; jmp <epilogue>` 之后无填充，
// 我方同点多 1 条 jmp 后 `nop`（对齐填充；属 P6.1 nop 豁免族，但工具只剥 ret/call 之后者）。
// 条件段 `call isAffectedPremium; xor $1; test %al,%al; jne`（bool 消费侧）两侧全等；
// 无声明序/槽映射残差（P339/P331 不适用）⇒ N 封顶，源码零改动。
#include "CPremiumLetheManager/_resetSkill.inc"
{
    unsigned char flag;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        flag = user->GetOneDayLetheFlag();
    }
    else
    {
        flag = user->GetOneDayLetheFlag2ND();
    }
    if (flag != USE_LETHE_STATE)
        goto FAIL_TRACE;    // ORIG jne 尾置 else 块（4.4 threading 会前移，goto 阻断）
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
    }
    else
    {
        InitSkill(user, true,
                  (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
        PacketGuard guard;
        guard.put_header(1, 0xd5);
        guard.put_byte(1);
        guard.finalize(true);
        user->Send(guard);
    }
    return;
FAIL_TRACE:
    {
        // [b28c-L29] P221：具名 trace 对象使 PacketGuard(-0x28)/trace(-0x1c)
        // 的 sibling 块槽方向与 ORIG 相反；全表达式临时链复现 ORIG 槽深。
        cMyTrace(__PRETTY_FUNCTION__, 0x93, 0)("ONE_DAY_LETHE : CAN'T USE INIT STATE");
    }
}

// [b28d-solo 2026-09-11] ORIG 0x085c421e | 328 / our 327（gap −1；本轮 340→327，−13 条）。
// ① 语义轴真修 2 处：kind2/kind3 映射 —— ORIG 0x085c427c / 0x085c45e4 直证
//    `movl $0x0; cmp $0xffffffff,%eax; je →1; test %eax,%eax; jne →2`（Ghidra 同判
//    `(kind == -1) || (kind == 0) ? 1 : 2`），原码误作 `-1 → 0` ⇒ givePvPSkillTree /
//    SkillInitialize 的第三实参在 kind==SKILL_TREE_NONE 时错值（运行行为可见）。
// ② 消指令（P0/P329/P183）：PvP 分支 5 个实参局部全内联（ORIG `mov 0x14(%eax),%edi`
//    + grade→%esi / second→-0x110 / first→%edi / job→%ebx / script 直传）、B 分支
//    ch/second/first/expertJob/level/skillIdx/vector 全内联、`+0x14` 具名化为
//    m_characList（disp 折叠）、主循环补回 ORIG 两级具名拷贝（a/b，+4 条）、
//    got 消费侧配对 bool（P248/A06）。
// 残余 1 条 = ORIG `movzbl; xor $1; test; jne`（4 条）vs our `cmpb $1; jne`（2 条）的
// bool 条件发射差。已试并回滚：`if (!got) goto SkipGiveSkill;` 负条件形 ⇒ our 331、
// 帧 0x12c→0x13c（反劣）；`if (got == 1)` 为 4.4 直产形 ⇒ 封顶。
#include "CPremiumLetheManager/InitSkill.inc"
                                     ENUM_SKILL_TREE_KIND kind)
{
    // [L16 round3] P0/P183：ORIG 段为 `call getCurCharacSkillW; mov 0x14(%ebp),%edx;
    // mov %edx,0x4(%esp); mov %eax,(%esp); call clear_all_skills`（返回值 %eax 直传、
    // 无具名 slot 槽往返）⇒ 删具名缓存、逐点链式重取。
    ((SkillSlot*)user->getCurCharacSkillW())->clear_all_skills(kind);
    ((SkillSlot*)user->getCurCharacSkillW())->set_parent(user);
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG 0x085c427c：`movl $0x0,-0x28(%ebp); cmp $0xffffffff,%eax; je →1`;
        // `test %eax,%eax; jne →2` ⇒ `kind==NONE(-1) || kind==1(0)` → 1，其余 → 2
        // （Ghidra 同判：`(param_4 == -1) || (param_4 == 0) ? 1 : 2`）。
        int kind2 = 0;
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
            kind2 = 1;
        else
            kind2 = 2;
        user->givePvPSkillTree(0, true, kind2);
        // [b28d-solo] P0/P329：ORIG 0x085c42cf..0x085c432b 五处实参表达式全内联、
        // 右→左求值并各自保活（grade→%esi / second→-0x110 槽 / first→%edi /
        // job→%ebx / script→`G_CDataManager()+0xa800` 直传），无任何具名局部。
        int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
            user->get_charac_job(),
            (signed char)user->getCurCharFirstGrowType(),
            (signed char)user->getCurCharSecondGrowType(),
            user->get_pvp_grade(), false);
        ((SkillSlot*)user->getCurCharacSkillW())->set_remain_sp_at_index(point, kind);
        if (flag)
        {
            user->send_skill_info();
            const char* name = user->getCurCharacName();
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0xb1, 0);
            trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                  name);
        }
    }
    else
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x46),
                user->get_charac_job());
        }
        else
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x1de),
                user->get_charac_job());
        }
        std::vector<std::pair<int, int> > vec;
        std::pair<int, int> p;
        // [b28d-solo] P0/P329：ORIG 0x085c442f..0x085c4481 三个实参全内联——
        // second→%esi、first→%ebx、receiver base 由 `G_CDataManager()+0x14`
        // 载入 %edi 后 `imul $0x7dc` + `lea` 直传（无 ch/second/first 具名槽）。
        // [b28d-solo] ① 裸偏移 `+0x14` 具名化为 `m_characList`（ORIG 0x085c4459
        // `mov 0x14(%eax),%edi` 单条 disp 折叠；裸偏移写法 4.4 多一条 `add $0x14`）；
        // ② 消费侧与 D06 已 bool 化的 `CCharacter::get_give_skill` 配对（P248/A06
        // 净账必配对律）：`bool got` 直存 1 字节槽（ORIG `mov %al,-0x31(%ebp)`）。
        // 注：CCharacList 在 CDataManager.h 仅前向声明（不完全类型不可取下标），
        // 故按 `job*0x7dc` 步长做指针算术（等价 `&m_characList->m_characs[job]`）。
        bool got = ((CCharacter*)((char*)G_CDataManager()->m_characList +
                                  user->get_charac_job() * 0x7dc))
                       ->get_give_skill(
                           (signed char)user->getCurCharFirstGrowType(),
                           (signed char)user->getCurCharSecondGrowType(), vec, 0);
        // ORIG 0x085c4486..0x085c4492：`movzbl -0x31(%ebp),%eax; xor $0x1,%eax;
        // test %al,%al; jne <vec dtor>` = bool 消费侧（P292/P174）的负极性条件。
        // [已试·回滚] `if (!got) goto SkipGiveSkill;` 负条件直跳：our 327→331
        // （帧 0x12c→0x13c，+6 EH/布局条），劣化 ⇒ 保留 4.4 直产 `cmpb $1; jne`。
        if (got == 1)
        {
            int i;
            for (i = 0; i < (int)vec.size(); ++i)
            {
                p = vec[i];
                // ORIG 0x085c44b8..0x085c44c7：`p`(-0x70/-0x6c) 之后另有两次
                // 具名 int 落槽（-0x3c/-0x38）再从槽装载实参 ⇒ 源码为两个具名
                // int 局部（4.4 若直传 p.first 会省掉这级拷贝，少 4 条）。
                int a = p.first;
                int b = p.second;
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), a, b, kind);
            }
            // [b28d-solo] P0/P329：ORIG 0x085c451c..0x085c45d0 中 expertJob/level/
            // skillIdx/vector 指针全为内联右值（expertJob→%ebx、level→%esi、
            // skillIdx→%edi、vector 每处重发 `script+0xc`，无具名槽）；
            // 仅 `script` 具名（ORIG -0x30）。实参右→左求值序亦逐条对应。
            void* script = G_CDataManager()->GetExpertJobScript(
                user->GetCurCharacExpertJobType());
            if (script != 0)
            {
                unsigned int i2;
                for (i2 = 0;
                     i2 < ((std::vector<std::pair<int, int> >*)((char*)script + 0xc))->size();
                     ++i2)
                {
                    ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                        user->get_charac_job(),
                        (*(std::vector<std::pair<int, int> >*)((char*)script + 0xc))[i2].first,
                        user->GetCurExpertJobLevel(
                            user->GetCurCharacExpertJobExp()),
                        kind);
                }
            }
            WongWork::CSkillChanger changer;
            // ORIG 0x085c45e4：与 kind2 同形（`-0x2c` 槽）⇒ NONE(-1)/1(0) → 1，其余 → 2。
            int kind3 = 0;
            if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
                kind3 = 1;
            else
                kind3 = 2;
            changer.SkillInitialize(user, kind3, false);
            if (flag)
            {
                user->send_skill_info();
                const char* name = user->getCurCharacName();
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0xf7, 0);
                trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                      name);
            }
        }
    }
}

#include "CPremiumLetheManager/BackupSkill.inc"
                                       ENUM_SKILL_TREE_KIND kind)
{
    if (user->getCurCharacR() == 0)
    {
        return false;
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (user->GetOneDayLetheFlag() != NOT_USE_LETHE_STATE &&
            user->GetOneDayLetheFlag() != SKILL_CONFIRM_STATE)
        {
            return false;
        }
    }
    else
    {
        if (user->GetOneDayLetheFlag2ND() != NOT_USE_LETHE_STATE &&
            user->GetOneDayLetheFlag2ND() != SKILL_CONFIRM_STATE)
        {
            return false;
        }
    }
    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        user->SetOneDayLetheFlag(USE_LETHE_STATE);
        _update(user, USE_LETHE_STATE, SKILL_TREE_1);
    }
    else
    {
        user->SetOneDayLetheFlag2ND(USE_LETHE_STATE);
        _update(user, USE_LETHE_STATE, SKILL_TREE_2);
    }
    return true;
}

#include "CPremiumLetheManager/_update.inc"
                                   ENUM_SKILL_TREE_KIND kind)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PremiumLetheManager.cpp",
                                                          0x125),
                       true);
    **guard << 0x15c;
    // (int)：CUser::GetUID() 返回 unsigned int（CUser.h:2910），不加转换会选中
    // operator<<(unsigned int)；ORIG 0x085c48b2 调用点为 operator<<(int)
    // （P167 调用点差集实证 ORIG lsEi×2 / 修复前 lsEi+lsEj）。
    **guard << (int)user->GetUID();
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* result =
        guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
    result->m_accID = user->get_acc_id();
    // [b28d-solo 负结论] ORIG 0x085c48f4 `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)`
    // （2 条形）⇒ ORIG 的 m_characNo 与 `getCurCharacNo()`（unsigned int）同型；
    // 头内声明为 int（SigTypes_E.h:959）时 4.4 恒插 `mov %eax,%edx` 值搬运
    // （our 128，gap −1，残余仅此 1 条）。已试并回滚：本 TU 内
    // `*reinterpret_cast<unsigned int*>(&result->m_characNo) = …` ⇒ our 129
    // （更差）；源侧 static_cast<int> 同族无效（P211，Arad_AvatarRoulette.cpp:88-91
    // 实证字段改 unsigned int 后逐条全等）⇒ 需改头，已上报父泳道。
    result->m_characNo = user->getCurCharacNo();
    if (state == USE_LETHE_STATE || state == SKILL_CONFIRM_STATE ||
        INIT_LETHE_SKILL != 0)
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            memcpy(result->m_skills, user->getCurCharacSkillR() + 0x46, 0x198);
        }
        else
        {
            memcpy(result->m_skills, user->getCurCharacSkillR() + 0x1de, 0x198);
        }
    }
    else
    {
        memset(result->m_skills, 0, 0x198);
    }
    result->m_state = state;
    result->m_state2 = state;
    result->m_treeKind = (unsigned char)kind;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "CPremiumLetheManager/_removeInitSkill.inc"
                                            _Mastered_skill* skills)
{
    _Mastered_skill* pSkill;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        pSkill = (_Mastered_skill*)((char*)user->getCurCharacSkillR() + 0x46);
    }
    else
    {
        pSkill = (_Mastered_skill*)((char*)user->getCurCharacSkillR() + 0x1de);
    }
    for (int i = 0; i <= 0xcb; ++i)
    {
        for (int j = 0; j <= 0xcb; ++j)
        {
            if (pSkill[i].m_index != 0 &&
                pSkill[i].m_index == skills[j].m_index)
            {
                skills[j].m_level =
                    (char)(skills[j].m_level - pSkill[i].m_level);
                if (skills[j].m_level < 1)
                {
                    skills[j].m_index = 0;
                    skills[j].m_level = 0;
                }
            }
        }
    }
}
