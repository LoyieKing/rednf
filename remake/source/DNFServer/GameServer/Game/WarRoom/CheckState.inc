void WarRoom::CheckState()
{
    if (GetWaiterCount() == 0)
    {
        ChangeState(WARROOM_STATE_0);
    }
    else if ((GetWaiterCount() < 3) && (GetState() == 1))
    {
        ChangeState(WARROOM_STATE_0);
    }
}