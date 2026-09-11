int GetIdentityFromDB(MySQL* mysql)
{
    return (int)WongWork::DBCommon::GetIdentity(mysql);
}