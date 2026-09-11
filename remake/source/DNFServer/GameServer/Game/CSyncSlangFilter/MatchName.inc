int CSyncSlangFilter::MatchName(const std::string& name) const
{
    if (name.empty())
        return 0;
    SlangNode* node = implName_->root;
    unsigned i = 0;
    for (i = 0; i < name.size(); ++i)
    {
        unsigned ch = (unsigned char)name[i];
        node = node->FindChild(ch);
        if (node == 0)
            return 0;
        if (node->IsEndNode())
            return (int)(i + 1);
    }
    return 0;
}