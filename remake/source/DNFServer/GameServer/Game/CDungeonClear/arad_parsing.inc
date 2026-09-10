void arad_parsing(const std::string& src,
                  std::vector<std::string>& out,
                  const std::string& delims)
{
    std::string::size_type start = src.find_first_not_of(delims, 0);
    std::string::size_type end = src.find_first_of(delims, start);
    out.clear();

    while (end != std::string::npos || start != std::string::npos)
    {
        out.push_back(src.substr(start, end - start));
        start = src.find_first_not_of(delims, end);
        end = src.find_first_of(delims, start);
    }
