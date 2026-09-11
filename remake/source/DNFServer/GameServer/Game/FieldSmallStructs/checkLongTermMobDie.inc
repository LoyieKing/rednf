// ORIG W _ZN25stMapMonsterKillChecker_t19checkLongTermMobDieEjj @ 0x830e7a4：
//   mov 0x10(%eax),%eax; test; je ret         → last(+0x10)==0 直接返回 m_0c
//   tick - last ≥ limit（无符号 jb 判 <）时 ++m_0c
//   返回 *(int*)(this+0xc)（本头 m_dieCnt 槽；ORIG 该槽存"非连续计数"）
int stMapMonsterKillChecker_t::checkLongTermMobDie(unsigned int tick,
