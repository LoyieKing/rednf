// ORIG W _ZN25stMapMonsterKillChecker_t19setUseSkillMaterialEj @ 0x830e72c。
// 无独立函数报告；本实现经 ORIG objdump（0x830e72c..0x830e748）逐指令核实，
// 调用方报告 CBattle_Field/onUseItem.md 已完整核对：
//   mov 0xc(%ebp),%edx; mov %edx,(%eax)        → m_useSkillMaterialTime = tick
//   mov 0x14(%eax),%eax; lea 0x1(%eax),%edx;
//   mov %edx,0x14(%eax)                        → m_field14 += 1
void stMapMonsterKillChecker_t::setUseSkillMaterial(unsigned int tick)
{
    m_useSkillMaterialTime = tick;
    ++m_field14;
}