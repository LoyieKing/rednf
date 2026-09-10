bool WongWork::CSimpleSSO::updateSSOExpireTime(unsigned int mId, unsigned int loginTime)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    // [b28d-D13] ORIG 0x08602a70：idStr→%esi、lastPlayBase→%ebx、expireBase→%eax
    // （lea 0xb4(%eax),%edx）——三者均无栈槽 ⇒ 内联实参（P24/P0），右起求值序
    // NumberToString→getCurSec→getCurSec 与 ORIG 一致。ORIG 0x08602ae8 尾部
    // xor$1/test/je+双 mov ⇒ 源码 if(!exec())return false; return true;（P101）。
    m_mysql->set_query(
        "upDate %s set login_time=%d, expire_time=%d, last_play_time=%d where m_id=%s",
        tableName, loginTime,
        GlobalData::s_systemTime_.getCurSec() + 0xb4,
        GlobalData::s_systemTime_.getCurSec(),
        NumberToString(mId, 0));
    if (!m_mysql->exec(true))
        return false;
    return true;
