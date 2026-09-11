// ORIG _ZN4CLog15destroyInstanceEv（0x82a3cfd）：非空检查后 delete（delete
// 内部自带二次空检查）；全局指针无条件置 0。
void CLog::destroyInstance()
{
    if (this_ != 0)
	    {
		        delete this_;
	    }
	this_ = 0;
}