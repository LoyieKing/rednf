bool CMission_combo_clear::_isAcceptCondition(const CUser& user) const
{
    if (!_isAcceptCondition_Series(user)) return 0;

    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();
    // b28d-50：①ORIG 0x085e438c 尾为 test %al,%al; je <mov $0>; mov $1; jmp;
    // mov $0（P56 分支形态）；②ORIG 求值序为 **find 先、end 后**（find sret -0x14、
    // end sret -0xc），单表达式 `find()!=end()` 4.4 反产 end 先（P293 具名 it 形）。
    // 具名 const_iterator + 分支 return + 反序实参（end() 在左）即三处同复现
    // （ORIG 0x085e4387 实参序 (%esp)=end 槽、0x4(%esp)=it 槽，P74补②/P220）。
    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    if (m_comboMap.end() != it)
        return true;
    return false;
}