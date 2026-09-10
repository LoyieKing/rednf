void turnOffSecurityFlag(MySQL* db, unsigned int m_id);         // ORIG T 0x0842af12（DBSecuLink.cpp）
// ORIG 0x842b6e6：GetDBHandle(DB_HANDLE_6,0) →
// GetOutBuffer<SIG_SECURITY_CARD_DISUSE> → turnOffSecurityFlag(db, *(uint*)sig)
// → set_query("upDate member_security_card set apply_flag=0 where m_id=%s",
// NumberToString(m_id,0)) → exec(true)（结果丢弃）→
// saveSecurityCardHistory(db, m_id, ENUM_SECURITYCARD_MOD_TYPE_DISUSE=2)
// → 返 1。
// ============================================================================
