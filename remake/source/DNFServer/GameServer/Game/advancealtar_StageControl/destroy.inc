void StageControl::destroy()
{
    reset();
    G_CGameManager()->returnAdvanceAltar(this);
