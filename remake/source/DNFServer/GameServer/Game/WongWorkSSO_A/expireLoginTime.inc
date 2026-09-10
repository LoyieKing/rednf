void WongWork::CSimpleSSO::expireLoginTime(unsigned int mId)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG+0x006..+0x01c call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG+0x021..+0x037 call memcpy
    if (mId != 0)
    {
        register char* idStr = NumberToString(mId, 0);
        m_mysql->set_query("upDate %s set expire_time=%d where m_id=%s",
                           tableName, GlobalData::s_systemTime_.getCurSec(),
                           idStr);
        m_mysql->exec(true);
    }
