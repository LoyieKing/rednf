void ConInterface::setInfo(int con_id, const char* remote_ip, int remote_port,
                           TCPUser::ENUM_DATA_TYPE send_data_type,
                           TCPUser::ENUM_DATA_TYPE recv_data_type, bool need_to_recon)
{
    this->con_id = con_id;
    this->remote_port = remote_port;
    this->send_data_type = send_data_type;
    this->recv_data_type = recv_data_type;
    this->need_to_recon = need_to_recon;
    strcpy(this->remote_ip, remote_ip);
    this->mConnected = false;
    this->mIsRejected = false;
