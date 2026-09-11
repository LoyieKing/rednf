bool SDCActiveStatusSyncServer::loadActiveStatusScript()
{
    // ORIG T 0x0808e13c：尾透传 return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc")。
    return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc");
}