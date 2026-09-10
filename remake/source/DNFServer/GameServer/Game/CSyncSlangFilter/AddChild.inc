SlangNode* SlangNode::AddChild(unsigned int ch)
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it = children_.find(ch);
    if (it == children_.end())
    {
        it = children_.insert(std::pair<const unsigned int, SlangNode*>(ch, new SlangNode())).first;
    }
    return it->second;
