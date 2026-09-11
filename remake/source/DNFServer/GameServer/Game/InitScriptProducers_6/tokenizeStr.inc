// ----------------------------------------------------------------------------
// tokenizeStr —— ORIG T 0x088945e2。先求首/尾锚点并 clear 输出容器，随后
// while(end!=npos || start!=npos) 循环 push_back(substr(start,end-start)) 并
// 推进双锚点。循环条件为「或」（88946cd 处 jne/je 组合实证），与常规实现不同。
// ----------------------------------------------------------------------------
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
}