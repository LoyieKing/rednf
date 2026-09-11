// ORIG 0x08ac2066 T：清 +0x00/+0x04 两 string 与 +0x8 map（loadScript 前置）。
void DeleteInvalidItemScript::clearScript()
{
    m_field0.clear();
    m_field4.clear();
    m_deleteMap.clear();
}