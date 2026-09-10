STStackableScript::STStackableScript(STStackableScript& other)
{
    operator=(other);
    if (!other.m_module4bc.empty())
    {
        // ORIG 0x0898fbf7..0x0898fd68：4.1 for 布局（begin 后 jmp 尾条件、每次
        // 迭代重调 other.end()）；循环体内 dst_end=end() → it->first(ptEv) →
        // find → ne(found,dst_end)；body 为 src_pair 逐成员赋值
        // （src_pair.first=it->first；src_pair.second=new ...；*second=*it->second）。
        // [b28d-solo-STStackableScript] N93/P212 封顶：ORIG 槽序（4.1 块内先声明
        // 深）it(-0x38) > dst_end(-0x30) > found(-0x2c)；4.4 块内先声明浅
        // （dst_end -0x34 / found -0x38，方向相反）。三种改写实测：
        // ①三迭代器提函数头：+3 C1Ev call、帧 +0x10（P50 反例）；
        // ②found/dst_end 提 for 前：+2 C1Ev call、帧 +0x10（ndiff 24 但引入
        //   EXTRA 调用）；③循环体内声明（本形）：orig=643 our=643、无额外 call、
        //   残差仅槽号，ndiff 44 NEAR ⇒ 取 ③，p167 无缺口。
        for (std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it =
                 other.m_module4bc.begin();
             it != other.m_module4bc.end(); ++it)
        {
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator dst_end = m_module4bc.end();
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator found = m_module4bc.find(it->first);
            if (found != dst_end)
            {
                std::pair<ENUM_MODULE_TYPE, EquipmentParameterInfo*> src_pair;
                src_pair.first = it->first;
                src_pair.second = new EquipmentParameterInfo();
                *src_pair.second = *it->second;
                m_module4bc.insert(src_pair);
            }
        }
    }
