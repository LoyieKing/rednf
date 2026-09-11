// [b28d-solo-CSyncSlangFilter] AE：键类型改 pair<const unsigned int, SlangNode*>（消 pair<uint,..> 转换构造）+ new 内联进 insert 实参（消具名 node 槽，P226/P278 匿名临时，槽集 it/end/insert/pair/new 与 ORIG 逐槽全等）；ORIG 0x0862ea5a。
SlangNode* SlangNode::AddChild(unsigned int ch)
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it = children_.find(ch);
    if (it == children_.end())
    {
        it = children_.insert(std::pair<const unsigned int, SlangNode*>(ch, new SlangNode())).first;
    }
    return it->second;
}