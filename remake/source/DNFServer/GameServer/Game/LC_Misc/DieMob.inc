// ---------------------------------------------------------------------------
// ORIG 0x081468aa（报告 Global__CBossTowerFunc/DieMob.md）：
// b28c-L8 判词 [N(N7)]：fresh 复验 35=35、p167 调用点差集 0/0、sub $0x44 +
// push/pop %ebx 全等。唯一残差 = ORIG 0x081468db `mov (%eax),%ebx`（虚槽 +0x24
// handleKillMonster 目标留 callee-saved %ebx）、实参装载走 %edx/%eax/%ecx；4.4 对间接
// call 目标恒留 %eax 且装载序整体前移一格（%ecx/%edx/%ebx）。P226 语句表达式升格只对
// 「call 结果值」有效、对 call 目标寄存器无效；-march=i486 对 N7 亦无效（Main 口径）
// ⇒ N7 封顶（与 lane12 Secu_MonsterCheck 族同判）。
// getBossTower() 非空 → 虚槽 +0x24（handleKillMonster）(user,mobSeq,param,msg,0)；
// 恒返回 1。
int Global::CBossTowerFunc::DieMob(CUser* user, unsigned short mobSeq,
                                   unsigned short param,
                                   const MSG_MONSTER_DIE& msg, int* out)
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
        tower->handleKillMonster(user, mobSeq, param, msg, 0);  // ORIG 第5实参=$0
    return 1;
}