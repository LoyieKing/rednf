SlangNode* SlangNode::FindChild(unsigned int ch) const
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::const_iterator it = children_.find(ch);
    if (it != children_.end())
        return it->second;
    return 0;
}