// ---------------------------------------------------------------------------
// 1) CUser::_CalcItemVectorMultiply(vector<pair<int,int>>&, int)
//    报告：docs/class_func_reports/CUser/_CalcItemVectorMultiply.md
//    ORIG 0x08672042：循环 it->second *= mult。
// ---------------------------------------------------------------------------
void CUser::_CalcItemVectorMultiply(std::vector<std::pair<int, int> >& items, int mult)
{
    // ORIG 0x08672049-0x08672080：三前置调用 = begin()→-0x10、end() 临时→-0xc、
    // __normal_iterator 转换 ctor→cend(-0x14)（P13 缓存 end + const_iterator 转换，
    // 技巧28 槽序 it 先声明）；循环条件 `it != cend` 走 mixed operator!=@0x0869b2d3。
    std::vector<std::pair<int, int> >::iterator it = items.begin();
    const std::vector<std::pair<int, int> >::const_iterator cend = items.end();
    for (; it != cend; ++it)
    {
        it->second = it->second * mult;
    }
}