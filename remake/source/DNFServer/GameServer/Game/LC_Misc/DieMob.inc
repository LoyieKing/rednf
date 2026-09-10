int Global::CBossTowerFunc::DieMob(CUser* user, unsigned short mobSeq,
                                   unsigned short param,
                                   const MSG_MONSTER_DIE& msg, int* out)
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
        tower->handleKillMonster(user, mobSeq, param, msg, 0);  // ORIG 第5实参=$0
    return 1;
