// ---- 击杀节奏反外挂链（kill_monster +0x1585..+0x18a8 消费）----
// ORIG W _ZN25stMapMonsterKillChecker_t15setLastMobDieTimeEj @ 0x830e796：
//   mov 0xc(%ebp),%edx; mov %edx,0x10(%eax)   → 写 +0x10（本头 m_field10 槽；
//   ORIG 中该槽即"上次击杀时刻"，与 dieMonster 用的 +0x04 是两个独立槽位）。
void stMapMonsterKillChecker_t::setLastMobDieTime(unsigned int tick)
{
    m_field10 = tick;
}