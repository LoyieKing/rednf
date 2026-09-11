// ORIG W 0x08110edc：返回绑定脚本 +0x0c 的 [user type] 字节。
char CFatigueQuantity::get_user_type() const
{
    return static_cast<char>(
        static_cast<FatigueQuantityScript*>(m_script1c)->m_userType);
}