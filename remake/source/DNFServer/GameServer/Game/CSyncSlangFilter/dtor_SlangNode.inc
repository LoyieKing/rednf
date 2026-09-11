// [b28d-solo-CSyncSlangFilter] AE：P50 先声明后赋值（ORIG 有独立 iterator::C1Ev，初始化式会被 4.4 折叠）+ P141 无条件 delete 复现 ORIG 空指针守卫；ORIG 0x0862e92a。
SlangNode::~SlangNode()
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it;
    it = children_.begin();
    for (; it != children_.end(); ++it)
    {
        delete it->second;
    }
    children_.clear();
}