void CSyncSlangFilter::AddSlang(const std::string& str)
{
    char buf[0x101] = {0};
    OS_API::snprintf(buf, 0x100, "%s", str.c_str());
    buf[0x100] = 0;
    SlangNode* node = impl_->root;
    if ((unsigned char)buf[0] == 0xbb &&
        (unsigned char)buf[1] == 0xd8 &&
        (unsigned char)buf[2] == 0xc3)
    {
        unsigned skip0 = 0;
        unsigned skip1 = 0;
        (void)skip0;
        (void)skip1;
    }
    unsigned i = 0;
    for (i = 0; i < str.size(); ++i)
    {
        writeGuard<rwLock> guard(&lock_);
        node = node->AddChild((unsigned char)buf[i]);
    }
    node->SetEndNode(true);
