void CCommandMap::add(std::string name, CCommand* cmd)  // 0x082a7cdc
{
    // ORIG pair ctor 模板 C1IRSsRS3_EEOT_OT0_（引用绑定）直接构造 value_type 临时，
    // 由 insert(RKS5_) 按引用消费 ⇒ 禁 make_pair（值 pair 额外一次拷贝构造+析构）。
    m_byName.insert(std::pair<const std::string, CCommand*>(name, cmd));
}