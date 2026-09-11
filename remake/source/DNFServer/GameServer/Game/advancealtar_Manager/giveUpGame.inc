// ----------------------------------------------------------------------------
// ORIG T 0x08130fa8 _ZN12advancealtar7Manager10giveUpGameEP5CUser
// user/getAdvanceAltar() 判空直接返回；setState(6) +
// RegistTimerEPLP(getIndex(), END_N2) + dungeonClearInfo(user, control, false)。
// ----------------------------------------------------------------------------
void Manager::giveUpGame(CUser* user)
{
    if (user == 0)
        goto L_out;
    StageControl* control;
    control = user->getAdvanceAltar();
    if (control != 0)
    {
        control->getProcStage()->setState(ProcStage::State::S_N6);
        Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                            StageEndType::END_N2);
        HistoryLog::dungeonClearInfo(*user, *control, false);
    }
L_out:;
}