// ORIG 0x08150880 _ZNK16CAICharacterList10getAPCountEv
// ORIG 调用方（CBossStage/CDeathTower consistMap）把返回值作为
// map<int,int>::operator=(const map&) 的实参 —— 即返回 m_apStatMap 引用。
std::map<int, int> const& CAICharacterList::getAPCount() const
{
    return m_apStatMap;
}