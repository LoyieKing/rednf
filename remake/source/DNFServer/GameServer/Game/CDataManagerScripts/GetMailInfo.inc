// ORIG 0x8ac2180：返回脚本 +0x00/+0x04 两字符串（发件人名/正文模板）。
void DeleteInvalidItemScript::GetMailInfo(std::string& title,
                                          std::string& body) const
{
    title = m_field0;
    body = m_field4;
}