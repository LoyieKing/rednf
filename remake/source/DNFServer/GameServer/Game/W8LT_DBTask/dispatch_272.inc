// ============================================================================
// DB_NoticeIngameAdvertisement::dispatch —— ORIG 0x0844710e
// InGameADManagerInstance()->ReloadADExposeInfo()；失败 logFormat 0xb17f
// 返 0；成功返 1。
// ============================================================================
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
}