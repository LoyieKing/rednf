void HeroMissionValue::setStringToken(std::string token)
{
    clear();

    typedef boost::token_iterator<
        boost::char_separator<char>,
        __gnu_cxx::__normal_iterator<const char*, std::string>,
        std::string> TokenIterator;

    boost::char_separator<char> separator(" ", 0,
                                          boost::drop_empty_tokens);
    boost::tokenizer<boost::char_separator<char>,
                     __gnu_cxx::__normal_iterator<const char*, std::string>,
                     std::string>
        tokens(token, separator);

    TokenIterator tit = tokens.begin();

    // P23：ORIG 每步以 ++ 返回值直传下一调用（`mov %eax,(%esp); call ptEv`），
    // 拆语句 `++tit; tit->...` 会重复 lea 迭代器地址（每处 +2 条）。
    m_mission = atoi(tit->c_str());                     // +0x00
    m_field4 = (unsigned short)atoi((++tit)->c_str());  // +0x04
    m_field6 = (unsigned short)atoi((++tit)->c_str());  // +0x06
    m_fieldA = (unsigned short)atoi((++tit)->c_str());  // +0x0a
    m_field8 = (unsigned short)atoi((++tit)->c_str());  // +0x08
