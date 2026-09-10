void CLog::destroyInstance()
{
    if (this_ != 0)
	    {
		        delete this_;
	    }
	this_ = 0;
