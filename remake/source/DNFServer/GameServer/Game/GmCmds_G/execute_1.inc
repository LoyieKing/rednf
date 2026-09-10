void CTeleportCmd::execute()
{
    expert_job::CAlchemist::OnTeleportCharacter(
        GetUser(),
        (int)(signed char)GetUser()->getCurCharacVill(),
        0, 0, 0, true);
