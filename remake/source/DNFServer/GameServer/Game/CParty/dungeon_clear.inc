void CParty::dungeon_clear()
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85ab736
    // [G7-sub] 本 .inc 只承载函数头 + SaveCoin 一句（主体在聚合器 cpp，非本泳道写域）。
    // 复核 ORIG 085ab73e/085ab74c：`movl $0x1,0x4(%esp); mov 0x8(%ebp),%eax;
    // mov %eax,(%esp); call SaveCoin` 与我方逐条一致（diff 无该段差异）⇒ 域内无可改点；
    // 余差（我方 395 vs 390 = +5，`movl $0x0,-0x24` 循环初值/声明序）位于 cpp 侧。
    SaveCoin((eCoinSubReason)1);
    // P183：ORIG 对 bd 每个使用点重取址（无引用缓存槽）⇒ 宏直用。
