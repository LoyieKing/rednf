void AvatarRoulette::getAvatarList(
    bool& isOk, ENUM_CHARACTERJOB job,
    std::vector<std::vector<std::pair<unsigned long, int> > >& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_avatars[job];
        isOk = true;
    }
    return;
