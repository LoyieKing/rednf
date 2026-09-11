// ORIG 0x0899013c：体部先清理 m_module4bc 的指针值（空检查 + 遍历 delete +
// clear），随后编译器按声明序析构全部成员。
STStackableScript::~STStackableScript()
{
    if (!m_module4bc.empty())
    {
        for (std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it = m_module4bc.begin();
             it != m_module4bc.end(); ++it)
        {
            if (it->second != 0)
                delete it->second;
            it->second = 0;   // ORIG +0x79：赋 0 在 if 之外（两次 je 同落 +0x79）
        }
        m_module4bc.clear();
    }
}