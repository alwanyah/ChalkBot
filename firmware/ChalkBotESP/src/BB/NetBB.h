#ifndef NET_BB_H
#define NET_BB_H

#include <Arduino.h>

class NetBB {
    friend class Net;
    
    IPAddress localAddress;

public:
    const IPAddress &getLocalAddress() const {
        return localAddress;
    }
};


#endif
