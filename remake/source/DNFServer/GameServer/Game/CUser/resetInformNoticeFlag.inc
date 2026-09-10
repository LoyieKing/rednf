void CUser::resetInformNoticeFlag()
{
    // ORIG memset(this+0x8ebb1, 0, 1)：只清当前通知 flag 首字节（m_informNoticeFlags[0]），
    // 不动其后的 15B（已保存的通知 flag 板）。
    memset(m_informNoticeFlags, 0, 1);
