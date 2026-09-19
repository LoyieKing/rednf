int BlueMarble::finishBlueMarble(int seat)
{
    setUserGrade(seat);
    changeState((BlueMarbleState::T)3);
    sendFinishBlueMarble();
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}