void tokenizeStr(const std::string& s, std::vector<std::string>& out,
                 const std::string& delims)
{
    std::size_t start = s.find_first_not_of(delims, 0);
    std::size_t end = s.find_first_of(delims, start);
    out.clear();
    while (end != std::string::npos || start != std::string::npos)
    {
        out.push_back(s.substr(start, end - start));
        start = s.find_first_not_of(delims, end);
        end = s.find_first_of(delims, start);
    }
