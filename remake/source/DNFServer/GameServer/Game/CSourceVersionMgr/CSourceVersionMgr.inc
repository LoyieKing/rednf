CSourceVersionMgr::CSourceVersionMgr()
    : source_version_list_()
{
    InsertSourceVersion(".", 0x16bdc);
    InsertSourceVersion("../ServerCommon", 0x16bdc);
    InsertSourceVersion("../../DNFShared", 0x16bdc);
