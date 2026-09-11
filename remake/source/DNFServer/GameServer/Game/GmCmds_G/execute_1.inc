// 调炼金术师 OnTeleportCharacter(本人, 当前村庄, 0,0,0, true) 回城传送。
void CTeleportCmd::execute()
{
    expert_job::CAlchemist::OnTeleportCharacter(
        GetUser(),
        (int)(signed char)GetUser()->getCurCharacVill(),
        0, 0, 0, true);
}