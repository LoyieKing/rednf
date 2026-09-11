void GlobalScriptTutorial::LoadDungeonIndex(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    {
    int index;
        for (;;)
{
            if (!ScanInt(&index))
                goto done;
            m_data[static_cast<int>(job)].setDungeonIndex(
                static_cast<unsigned int>(index));
    }
    }
done:
    ;
}