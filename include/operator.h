#ifndef CCEMU_OPERATOR_H
#define CCEMU_OPERATOR_H

#include "cdrentry.h"

#include <chrono>
#include <thread>

class CallCenter;

class Operator {
public:
    Operator(CallCenter* parentCC);

    int getID() const;
    std::string getIDStr() const;

    void setCDR(CDREntry* cdr);

    void RunCall(int call_duration);

private:
    CallCenter* _parentCC {nullptr};
    CDREntry* _cdrPtr {nullptr};

    inline static int _id_counter {0};
    const int _id;
};



#endif