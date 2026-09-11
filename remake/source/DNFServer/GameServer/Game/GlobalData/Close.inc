void CDBConnector::Close()
{
    // ORIG 0x080ec668：转发 CloseMaster()。
    CloseMaster();
}