int get_rand_int(int range);

// ============================================================================
// DisJoint::GetAddtionalSkill @ 0x08473184
//   遍历 m_skills（[skill info] 行集，this+0xc0）找
//   m_field00==job && m_field04==level；命中 → outSkill=m_field08、
//   outLevel=m_field0c；未命中 → 两个 out 置 0。
//   ORIG 形态：局部 DisjointSkillScript entry 先走 ctor（{0xb,0,0,4}），
//   循环内 vector::operator[] 一次取行后拷贝到 entry 再双条件比较
//   （P54 逐字段赋值，勿用聚合拷贝）；循环界为无符号 size 比较（seta）。
//   [b28d-58] AE：outLevel=m_field0c 的 store ORIG 0x084731e5 为两段中转
//   （`mov -0x10(%ebp),%eax; mov %eax,%edx`）⇒ 该字段须为 unsigned int
//   （DisJoint.h 已改；探针 J1 int 单段 / J2 unsigned 两段）——改后逐条全等。
// ============================================================================
