SlangNode::~SlangNode()
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it;
    it = children_.begin();
    for (; it != children_.end(); ++it)
    {
        delete it->second;
    }
    children_.clear();
