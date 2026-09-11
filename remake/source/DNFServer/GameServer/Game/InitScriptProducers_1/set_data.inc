// ORIG W 0x0891bd0e：find 命中则向既有 vector push_back，否则新建单元素 vector 插入。
// b28-34：比较写 `m_map0.end() == it`（临时为 this、it 为形参）——ORIG 先装载
// 0x4(%esp)=&it 后装载 (%esp)=&end 临时；写 `it == m_map0.end()` 装载序反成 DIFF。
// b28d-lane25：ORIG 0x0891bd0e。P167 差集实证 ORIG 调 make_pair<IRKi,RSt6vector…>
// （键为 const int& 绑定形参本身）⇒ 形参顶层 const（不入 mangled），定义处还原。
void stStepRewardStackableItemScript::set_data(const int step, StackableItemData& data)
{
    std::map<int, std::vector<StackableItemData> >::iterator it = m_map0.find(step);
    if (m_map0.end() == it)
    {
        std::vector<StackableItemData> items;
        items.push_back(data);
        m_map0.insert(std::make_pair(step, items));
    }
    else
    {
        it->second.push_back(data);
    }
}
