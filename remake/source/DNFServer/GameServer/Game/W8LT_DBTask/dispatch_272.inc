bool DB_NoticeIngameAdvertisement::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    if (!InGameADManagerInstance()->ReloadADExposeInfo())
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb17f, "Fail Load In Game Advertisement Expose Info");
        return 0;
    }
    return 1;
