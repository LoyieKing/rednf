// ORIG isUnCommonItem 0x8918d9c：m_mapC.find(itemIdx) != end()
bool stBroadCastItemScript::isUnCommonItem(int itemIdx) const
{
    // ORIG 0x8918da8：find sret→具名 it(-0x10，声明即构造)；end 临时 -0xc；
    // 0x8918de6 ne(it,end) 后 je→mov$0/落空→mov$1 ⇒ 显式 if-return 形。
    std::map<int, std::vector<int> >::const_iterator it = m_mapC.find(itemIdx);
    if (it != m_mapC.end())
        return true;
    return false;
}