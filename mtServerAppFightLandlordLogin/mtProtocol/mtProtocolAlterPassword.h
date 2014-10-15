#pragma once
#include "mtProtocol.h"

class    mtProtocolAlterPassword : public mtProtocol {
  public:
    struct  DataSend {
        mtProtocol::DataSend    kDataHead;
    };
    
    struct  DataRecv {
        mtProtocol::DataRecv    kDataHead;
        long                    lAccountType;
        char                    pcAccount[MT_BYTES_OF_ACCOUNT];
        char                    pcPwd[MT_BYTES_OF_PWD];
        char                    pcNewPwd[MT_BYTES_OF_PWD];
    };
    
  public:
    mtProtocolAlterPassword();
    virtual ~mtProtocolAlterPassword();
    
    int     init( DataInit* pkDataInit );
    virtual int exit();
    
    virtual int run( mtThreadWork::DataUser* pkDataUser );
};