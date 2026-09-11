// [b28d-solo-CListTables N82/N93 NEAR] ORIG 0x083519a6：125=125 + p167 CLEAR(14/14)。
// 真修复：分支体反写——ORIG `call eq; je trace 尾块`（it==end 走 insert fall-through）
// ⇒ 源码须 `if (it == end) { insert; return; } trace;`（旧 `if (it != end) { trace;
// return; }` 令 4.4 选 ne 实例并反转块序）。残差为 idx/it/end/trace1/trace2 槽分配
// 差（ORIG trace1 -0x54/trace2 -0x18 分散，4.4 集中 -0x60/-0x50）⇒ 封顶。
// [round1-L29 2026-09-10 复核·判据⑪ 槽直方图] 我方槽集合 = ORIG 槽集合 + 0x10 常数平移
//   （ours -0x50/-0x48/-0x44… vs ORIG -0x60/-0x58/-0x54…，逐值间距全同）⇒ **N126 版本首槽基址
//   常数**，声明重排无效（按判据⑪ 未耗编译轮次）。trace1 已改全表达式临时链（P227）
//   使平移由段内错位归一为单一常数。剩 N126 封顶（ORIG 0x083519a6）。
void CSkillList::insert_skill(int jobType, CSkill* skill)
{
    // [b28d-solo-CListTables] it 先 C1Ev 后 find 赋值（P50）已对齐；残差见函数头判词。
    if (skill == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xf91, 5)(
            "[%s][%d]_(0 == pSkill)",
              __PRETTY_FUNCTION__, 0xf91);
        return;
    }
    int idx = skill->get_index();
    // [b28d-solo-CListTables] ORIG 0x083519a6：it 先 C1Ev 默认构造（-0x60）再
    // `it = find()`（sret -0x70 拷贝，P50 复活）；end 初始化式（-0x44）。
    __gnu_cxx::hash_map<int, CSkill*>::iterator it;
    it = m_skillList[jobType].find(idx);
    // [b28d-solo-CListTables] ORIG 0x08351a82 调 operator==（eq）后 `je trace`：
    // 相等走 insert fall-through、不等落 trace 尾块 ⇒ 源码必须是正向
    // `if (it == end) { insert; return; } trace;`（写 `if (it != end) { trace;
    // return; }` 会让 4.4 选 ne 实例并反转块序）。
    // ORIG 0x08351aa3/0x08351af5：两处插入各只有两次 call —— 就地构造 value_type
    //   +0x0fd _ZNSt4pairIKiP6CSkillEC1IRiRS2_EEOT_OT0_（idx 左值）→ hash_map::insert
    //   +0x14f _ZNSt4pairIKiP6CSkillEC1IiRS2_EEOT_OT0_ （get_group() 右值，无中间变量）
    //          → multimap::insert；make_pair 会多出转换临时（第三次 call）。
    if (it == m_skillList[jobType].end())
    {
        m_skillList[jobType].insert(
            __gnu_cxx::hash_map<int, CSkill*>::value_type(idx, skill));
        m_groupSkillMap[jobType].insert(
            std::multimap<int, CSkill*>::value_type(skill->get_group(), skill));
        return;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0xfa4, 5);
    trace("[%s][%d]_( itr != m_SkillList[job_type].end() )",
          __PRETTY_FUNCTION__, 0xfa4);
}