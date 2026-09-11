// ===================== RandomItemTable::Set（G6 物品表；ORIG T 0x89bba40）=====================
// [b28d-solo-CEquipItem] ORIG 0x089bba40 逐条还原：
//   1. m_vec4.clear(); m_field0 = 0;
//   2. 遍历 source：pair 默认构造；pr.first = *it；it++；
//      若 it == end 则 break；否则 m_field0 += *it；
//      **pr.second = m_field0**（累计权重上界，GetRandomItemIndex 依此判定区间；
//      旧重建漏写该赋值 = 真语义 bug）；m_vec4.push_back(pr)；it++。
// 增量用后置 it++（ORIG 增量段为 operator++(int)/ppEi，P28 增量侧推论）。
void RandomItemTable::Set(std::vector<int>* source)
{
    m_vec4.clear();
    m_field0 = 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        std::pair<int, int> pr;
        pr.first = *it;
        it++;
        if (it == source->end())
            break;
        pr.second = *it;
        m_field0 += pr.second;
        pr.second = m_field0;
        m_vec4.push_back(pr);
        it++;
    }
}