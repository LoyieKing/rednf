// ----------------------------------------------------------------------------
// arad_parsing —— ORIG T 0x08483e98 (size 0x115)
// b28d-60 跨 TU 迁移（P210②，Main 批准）：ORIG TU = DungeonClear.cpp
//（orig_tu_names.tsv:429 GI 键名 `_GLOBAL__I__ZN8WongWork12arad_parsing...`；
//  且 ORIG 中本函数 0x08483e98 结束 0x08483fad 紧接
//  CDungeonClear::AradfromString T 0x08483fae）。原定义在 WongWorkEvents_A.cpp。
// start=find_first_not_of(delims,pos)/end=find_first_of(...)，非全 npos 则循环
// substr 入 vector（move push_back）；尾段 end==npos 但 start!=npos 也输出。
// ----------------------------------------------------------------------------
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
}