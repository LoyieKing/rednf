CCreatureScriptMgr::~CCreatureScriptMgr()
{
    // map<CCreatureScript*> 默认析构仅释放节点，不释放指针指向对象
    // （ORIG 显式释放见 Destroy()）。
}