int BlueMarble::finishBlueMarble(int seat)
{
    setUserGrade(seat);
    changeState((BlueMarbleState::T)3);
    sendFinishBlueMarble();
}