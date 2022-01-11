#ifndef NET_BB_H
#define NET_BB_H

#include <IPAddress.h>

class NetBB {
    friend class NetClass;
    
    IPAddress localAddress;

public:
    const IPAddress &getLocalAddress() const {
        return localAddress;
    }
};


#endif
