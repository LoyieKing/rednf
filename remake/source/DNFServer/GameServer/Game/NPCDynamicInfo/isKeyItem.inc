// ORIG 0x08580584 T：转发 CNPCScript::isKeyItem，bool 直传 eax（无规范化）。
bool CNPCDynamicInfo::isKeyItem(unsigned long key, unsigned int id) const
{
    return m_script->isKeyItem(key, id);
}