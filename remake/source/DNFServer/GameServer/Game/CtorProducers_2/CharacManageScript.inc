CharacManageScript::CharacManageScript()
{
    // ORIG 0x088e06cc：vector<ManagePointBonus>@+0、vector<int>@+0xc、
    // map<int,int>@+0x18/+0x30、map<int,string>@+0x4c、map<int,int>@+0x64、
    // vector<ContentsOptionRevision>@+0x7c 默认构造（成员自动）后调 clear()。
    clear();
