unsigned int DBCommon::GetIdentity(MySQL* db)
{
    unsigned int identity;  // ORIG 无预清 store（仅 get_uint 出参写回）
    db->set_query("seLect @@identity");
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_uint(0, identity))
        return 0;
    return identity;
