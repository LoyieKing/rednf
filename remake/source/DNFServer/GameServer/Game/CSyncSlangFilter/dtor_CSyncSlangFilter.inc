CSyncSlangFilter::~CSyncSlangFilter()
{
    if (impl_)
    {
        delete impl_;
        impl_ = 0;
    }
    if (implName_)
    {
        delete implName_;
        implName_ = 0;
    }
