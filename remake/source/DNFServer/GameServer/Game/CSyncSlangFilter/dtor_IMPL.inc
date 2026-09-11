CSyncSlangFilter::IMPL::~IMPL()
{
    if (root)
    {
        delete root;
        root = 0;
    }
}