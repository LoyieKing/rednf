char CFatigueQuantity::get_user_type() const
{
    return static_cast<char>(
        static_cast<FatigueQuantityScript*>(m_script1c)->m_userType);
