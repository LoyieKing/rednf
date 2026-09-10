// ============================================================================
// df_game_r 还原 —— CCharacter（依据 docs/class_func_reports/CCharacter/ 报告）。
// ctor ORIG _ZN10CCharacterC1Ev T 0x0834828c / dtor D1 T 0x0834842a。
//
// batch-15 近战池（ledger class=A 共 2 行）亲验归因：
//   1) _ZN9__gnu_cxx13new_allocatorISt4pairIiiEE9constructIIS2_EEEvPS2_DpOT_
//      (ORIG 0x080dda40 22 / 我 38)——libstdc++ 头生成体，差 16 = 4.4 placement
//      new 的 EH cleanup（_ZdlPvS_+_Unwind_Resume 10 条，N 类『new-expression
//      landing-pad 抑制』）+ N83 外联 op new + 寄存器物化；源码侧无合法解，豁免。
//   2) set_data：见其函数头 §6.2 标注；逐块亲验——调用多重集全等、目标常量写址
//      多重集仅 ORIG 以 +0x7d5 为基 + (i-1) 缩放 vs 我方 +0x7d4 基 + i（同一地址
//      集，常量基表达差 1）与 1 处 +0x10 标量写形态差 ⇒ 语义完成维持。
// ============================================================================
#include "CCharacter.h"

// ORIG STSecondGrowTypeInfo::STSecondGrowTypeInfo W 0x8373934：
// 2 个 _Additioal_info 构造 + 2 个 vector 默认构造 + clear()。
STSecondGrowTypeInfo::STSecondGrowTypeInfo()
    // ORIG W 0x08373934：两组数组循环构造后直接 call clear()；
    // m_fieldbc 不在 C1 写（仅 clear 的 memset 2B 覆盖）。
    : m_addInfo(), m_skills()
{
    clear();   // ORIG 0x8373a1e：成员构造后调 clear()（m_fieldbc 低 2 字节置 0）
}

// ORIG ~STSecondGrowTypeInfo W 0x8376324：逆序析构 +0xa4/+0xb0 两个 vector。
STSecondGrowTypeInfo::~STSecondGrowTypeInfo()
{
}

// ORIG CCharacter::CCharacter @ 0x834828c：
//   _Additioal_info[7]（this+0 起，步长 0x52）
//   vector[7]（+0x240，步长 0xc）
//   STSecondGrowTypeInfo[7]（+0x294，步长 0xc0）→ clear()。
CCharacter::CCharacter()
    : m_baseInfo0(), m_baseInfoGrow(), m_giveSkillList(), m_secondGrowTypes()
{
    // ORIG 0x834828c：各成员构造完成后调 clear()。
    clear();
}

// ORIG ~CCharacter @ 0x834842a：destroy → 逆序析构 STSecondGrowTypeInfo[7]、
// vector[7]（成员析构由编译器按声明逆序生成，语义一致）。
CCharacter::~CCharacter()
{
    // ORIG ~CCharacter @ 0x834842a：先调 destroy()（→clear()）再析构成员。
    destroy();
}

// ORIG get_give_skill_firstGrowtype @ 0x83485d8。
// ORIG 地址 = this + 0x240 + 12*(grow+1)（set_data 填满 [0..6]，本查询跳过 0 号基础槽）。
char CCharacter::get_give_skill_firstGrowtype(int grow,
                                              std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    std::vector<std::pair<int, int> >::const_iterator it = m_giveSkillList[grow + 1].begin();
    std::vector<std::pair<int, int> >::const_iterator e = m_giveSkillList[grow + 1].end();
    for (; it != e; ++it)
    {
        const std::pair<int, int>& p = *it;
        out.push_back(p);
    }
    return 1;
}

// ORIG get_give_skill_secondGrowtype @ 0x8348694。
// 注意：secondGrow 越界（<1 或 >2）时返回 1 且不拷贝（ORIG uVar2=1 分支）。
// ORIG 地址 = this + 0x330 + 0xc0*(grow+1) + 12*(secondGrow-1) + 8
//          = &m_secondGrowTypes[grow+1].m_skills[secondGrow-1]（同样跳过 0 号基础槽）。
char CCharacter::get_give_skill_secondGrowtype(int grow, int secondGrow,
                                               std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    if (secondGrow < 1 || secondGrow > 2)
        return 1;
    std::vector<std::pair<int, int> >::const_iterator it =
        m_secondGrowTypes[grow + 1].m_skills[secondGrow - 1].begin();
    std::vector<std::pair<int, int> >::const_iterator e =
        m_secondGrowTypes[grow + 1].m_skills[secondGrow - 1].end();
    for (; it != e; ++it)
    {
        const std::pair<int, int>& p = *it;
        out.push_back(p);
    }
    return 1;
}

// ORIG get_growtype_stat @ 0x834883e：按 (firstGrow, secondGrow) 返回成长属性块
// 指针；越界（firstGrow>5 或 secondGrow>2）返回 NULL。secondGrow==0 时取
// firstGrow*0x52+0x52（= m_baseInfo[firstGrow+1]，块宽 sizeof(_Additioal_info)=0x52），
// 否则取 (secondGrow-1)*0x52+(firstGrow+1)*0xc0+0x294
// （= &m_secondGrowTypes[firstGrow+1].m_addInfo[secondGrow-1]，
//   m_secondGrowTypes @+0x294、步长 sizeof(STSecondGrowTypeInfo)=0xc0、
//   m_addInfo 元素步长 0x52；布局断言见 CCharacter.h）。
const _Additioal_info* CCharacter::get_growtype_stat(int firstGrow,
                                                     int secondGrow) const
{
    if (firstGrow > 5 || secondGrow > 2)
        return 0;
    if (secondGrow == 0)
        return &m_baseInfoGrow[firstGrow];
    return &m_secondGrowTypes[firstGrow + 1].m_addInfo[secondGrow - 1];
}

// ORIG get_give_skill @ 0x8348798。
char CCharacter::get_give_skill(int firstGrow, int secondGrow,
                                std::vector<std::pair<int, int> >& out,
                                unsigned int flag) const
{
    // ORIG 0x08348798：所有分支经 char r 槽（-0x9）物化后统一 movzbl 尾返；
    // flag==0 支双赋值（r=f1(); r=f2(); 死写照抄 P171）。
    char r;
    if (flag == 0)
    {
        r = get_give_skill_firstGrowtype(firstGrow, out);
        r = (char)get_give_skill_secondGrowtype(firstGrow, secondGrow, out);
    }
    else if (flag == 1)
        r = get_give_skill_firstGrowtype(firstGrow, out);
    else if (flag == 2)
        r = (char)get_give_skill_secondGrowtype(firstGrow, secondGrow, out);
    else
        r = 0;
    return r;
}

// ============================================================================
// 以下为 CharacterAIA2 批次追加（报告 docs/class_func_reports/CCharacter/
// {CCharacter.md,~CCharacter.md} 完整转写；destroy/set_data/clear 报告缺失
// → objdump 转写，ORIG nm：destroy T 0x083485c4 / clear T 0x0834852e /
//   set_data T 0x08348894）。
// ============================================================================
#include "STCharacterScript.h"

// ORIG STSecondGrowTypeInfo::clear W 0x08373a1e（objdump 转写）：
// i=0..1 { m_addInfo[i].clear(); m_skills[i].clear(); } 后 memset(&m_fieldbc,0,2)
// —— 仅清 m_fieldbc 低 2 字节，高 2 字节保留（ORIG 固有）。
void STSecondGrowTypeInfo::clear()
{
    for (int i = 0; i <= 1; ++i)
    {
        m_addInfo[i].clear();
        m_skills[i].clear();
    }
    memset(&m_fieldbc, 0, 2);
}

// ORIG CCharacter::destroy @ 0x083485c4：sub $0x18 帧 + call clear()。
void CCharacter::destroy()
{
    clear();
}

void CCharacter::clear()
{
    // ORIG 0x0834852e..0x0834856c：base case 单条 + 循环 i≤5 写 m_baseInfoGrow[i]
    // （imul $0x52 +0x50/+0x2）——与 C1 混合构造形一致。
    m_baseInfo0.clear();
    for (int i = 0; i <= 5; ++i)
        m_baseInfoGrow[i].clear();
    for (int i = 0; i <= 6; ++i)
    {
        m_giveSkillList[i].clear();
        m_secondGrowTypes[i].clear();
    }
}

// ORIG CCharacter::set_data @ 0x08348894（objdump 全量转写，2901B / 932 条）。
// §6.2 超大函数（932 insns）验收：调用点差集 ZERO；gap 43→16（1.7%），残余为
// 4.1 常量拆分二段 `add $0x50/add $0x2` 寻址 vs 4.4 `lea disp(,%e)` 折叠
// （ORIG-only add 58 / OURS-only lea 46 净额，N82/P183 同族机器码折叠差）
// ⇒ 判语义完成、byte 层 N 封顶。
// ORIG 结构为 if(i==0)/else **双展开体**，两支各写一整套 24 条赋值，不共享缓存：
//   读侧两支统一为 script->m_status[i]（ORIG 每次读都重算 script+i*0x57+0x24；
//   i=0 落 m_initialStatus，i=1..6 落 m_growStatus[i-1]，布局见 STCharacterScript.h）；
//   写侧 i==0 支为常量偏移 this+0x00 一整套（m_baseInfo[0] / m_growTypeFlag[0]），
//   else 支为 m_baseInfo[i] / m_growTypeFlag[i]（ORIG 写侧算式为
//   (i-1)*0x52+0x52 与 (i-1)+0x7d5，地址等价；本工程 g++4.4.7 会把
//   (i-1)*0x52+0x52 常量折叠成 i*0x52，两种写法生成码逐字节相同，
//   故取无 UB 的下标形式）。
//   两支之后共用 m_giveSkillList[i] = script->m_skillList[i]（vector::operator=）。
// 第二成长域（a=0..6 × b=0..1）同样逐字段重算地址、不缓存引用：
//   dst = m_secondGrowTypes[a].m_addInfo[b]（this + 0x294 + a*0xc0 + b*0x52），
//   src = script->m_secondGrow[a].m_statusA[b]（script + 0x3d0 + a*0x100 + b*0x57），
//   resist 两段循环按具名字段寻址即与 ORIG 逐字节一致（+0x2a4 / +0x2ac），
//   尾字节写 ((char*)&m_secondGrowTypes[a].m_fieldbc)[b]（ORIG +0x350+a*0xc0+b），
//   末尾 m_secondGrowTypes[a].m_skills[b] = script->m_secondGrow[a].m_skills[b]。
// [b28d-7 N82] ORIG 0x08348894：帧 $0x5c + `imul $0x57` 后 add 两步寻址（-0x3c 先载），4.4 帧 $0x4c + lea 折叠；P167 CLEAR ⇒ 封顶。
void CCharacter::set_data(STCharacterScript* script)
{
    for (int i = 0; i <= 6; ++i)
    {
        if (i == 0)
        {
            m_baseInfo0.m_field00 = script->m_status[i].m_hpMax;
            m_baseInfo0.m_field04 = script->m_status[i].m_mpMax;
            m_baseInfo0.m_field08 = script->m_status[i].m_physAttack;
            m_baseInfo0.m_field0a = script->m_status[i].m_physDefense;
            m_baseInfo0.m_field0c = script->m_status[i].m_magAttack;
            m_baseInfo0.m_field0e = script->m_status[i].m_magDefense;
            for (int j = 0; j <= 3; ++j)
                m_baseInfo0.m_field10[j] = script->m_status[i].m_elemResist[j];
            for (int k = 0; k <= 16; ++k)
                m_baseInfo0.m_field18[k] = script->m_status[i].m_activeResist[k];
            m_baseInfo0.m_field3a = script->m_status[i].m_inventoryLimit;
            m_baseInfo0.m_field3e = script->m_status[i].m_hpRegen;
            m_baseInfo0.m_field40 = script->m_status[i].m_mpRegen;
            m_baseInfo0.m_field42 = script->m_status[i].m_moveSpeed;
            m_baseInfo0.m_field46 = script->m_status[i].m_attackSpeed;
            m_baseInfo0.m_field48 = script->m_status[i].m_castSpeed;
            m_baseInfo0.m_field4a = script->m_status[i].m_hitRecovery;
            m_baseInfo0.m_field4c = script->m_status[i].m_jumpPower;
            m_baseInfo0.m_field4e = script->m_status[i].m_weight;
            m_growTypeFlag[0] = (char)script->m_status[i].m_field52;
        }
        else
        {
            m_baseInfoGrow[i - 1].m_field00 = script->m_status[i].m_hpMax;
            m_baseInfoGrow[i - 1].m_field04 = script->m_status[i].m_mpMax;
            m_baseInfoGrow[i - 1].m_field08 = script->m_status[i].m_physAttack;
            m_baseInfoGrow[i - 1].m_field0a = script->m_status[i].m_physDefense;
            m_baseInfoGrow[i - 1].m_field0c = script->m_status[i].m_magAttack;
            m_baseInfoGrow[i - 1].m_field0e = script->m_status[i].m_magDefense;
            for (int j = 0; j <= 3; ++j)
                m_baseInfoGrow[i - 1].m_field10[j] = script->m_status[i].m_elemResist[j];
            for (int k = 0; k <= 16; ++k)
                m_baseInfoGrow[i - 1].m_field18[k] = script->m_status[i].m_activeResist[k];
            m_baseInfoGrow[i - 1].m_field3a = script->m_status[i].m_inventoryLimit;
            m_baseInfoGrow[i - 1].m_field3e = script->m_status[i].m_hpRegen;
            m_baseInfoGrow[i - 1].m_field40 = script->m_status[i].m_mpRegen;
            m_baseInfoGrow[i - 1].m_field42 = script->m_status[i].m_moveSpeed;
            m_baseInfoGrow[i - 1].m_field46 = script->m_status[i].m_attackSpeed;
            m_baseInfoGrow[i - 1].m_field48 = script->m_status[i].m_castSpeed;
            m_baseInfoGrow[i - 1].m_field4a = script->m_status[i].m_hitRecovery;
            m_baseInfoGrow[i - 1].m_field4c = script->m_status[i].m_jumpPower;
            m_baseInfoGrow[i - 1].m_field4e = script->m_status[i].m_weight;
            m_growTypeFlag[i] = (char)script->m_status[i].m_field52;
        }
        m_giveSkillList[i] = script->m_skillList[i];   // vector::operator=
    }

    for (int a = 0; a <= 6; ++a)
    {
        for (int b = 0; b <= 1; ++b)
        {
            m_secondGrowTypes[a].m_addInfo[b].m_field00 = script->m_secondGrow[a].m_statusA[b].m_hpMax;
            m_secondGrowTypes[a].m_addInfo[b].m_field04 = script->m_secondGrow[a].m_statusA[b].m_mpMax;
            m_secondGrowTypes[a].m_addInfo[b].m_field08 = script->m_secondGrow[a].m_statusA[b].m_physAttack;
            m_secondGrowTypes[a].m_addInfo[b].m_field0a = script->m_secondGrow[a].m_statusA[b].m_physDefense;
            m_secondGrowTypes[a].m_addInfo[b].m_field0c = script->m_secondGrow[a].m_statusA[b].m_magAttack;
            m_secondGrowTypes[a].m_addInfo[b].m_field0e = script->m_secondGrow[a].m_statusA[b].m_magDefense;
            for (int t = 0; t <= 3; ++t)
                m_secondGrowTypes[a].m_addInfo[b].m_field10[t] = script->m_secondGrow[a].m_statusA[b].m_elemResist[t];
            for (int k = 0; k <= 16; ++k)
                m_secondGrowTypes[a].m_addInfo[b].m_field18[k] = script->m_secondGrow[a].m_statusA[b].m_activeResist[k];
            m_secondGrowTypes[a].m_addInfo[b].m_field3a = script->m_secondGrow[a].m_statusA[b].m_inventoryLimit;
            m_secondGrowTypes[a].m_addInfo[b].m_field3e = script->m_secondGrow[a].m_statusA[b].m_hpRegen;
            m_secondGrowTypes[a].m_addInfo[b].m_field40 = script->m_secondGrow[a].m_statusA[b].m_mpRegen;
            m_secondGrowTypes[a].m_addInfo[b].m_field42 = script->m_secondGrow[a].m_statusA[b].m_moveSpeed;
            m_secondGrowTypes[a].m_addInfo[b].m_field46 = script->m_secondGrow[a].m_statusA[b].m_attackSpeed;
            m_secondGrowTypes[a].m_addInfo[b].m_field48 = script->m_secondGrow[a].m_statusA[b].m_castSpeed;
            m_secondGrowTypes[a].m_addInfo[b].m_field4a = script->m_secondGrow[a].m_statusA[b].m_hitRecovery;
            m_secondGrowTypes[a].m_addInfo[b].m_field4c = script->m_secondGrow[a].m_statusA[b].m_jumpPower;
            m_secondGrowTypes[a].m_addInfo[b].m_field4e = script->m_secondGrow[a].m_statusA[b].m_weight;
            // ORIG this + a*0xc0 + b + 0x350：m_fieldbc 的第 b 个字节（源 +0x52）
            reinterpret_cast<char*>(&m_secondGrowTypes[a].m_fieldbc)[b] =
                (char)script->m_secondGrow[a].m_statusA[b].m_field52;
            m_secondGrowTypes[a].m_skills[b] = script->m_secondGrow[a].m_skills[b];
        }
    }
}
