bool GiveGrowCreatureEvent::isGrowCreatureMaxLevel(CUser& user)   // 0x081b7156
{
    // ORIG: 三连 call 结果直传 (%esp)，无具名局部（P0：无栈槽→无具名局部）。
    return user.getCurCharacInvenR()->GetCreatureMgrR()->isGrowCreatureMaxLevel();
}