#ifndef CCEMU_OPERATOR_H
#define CCEMU_OPERATOR_H

#include "cdrentry.h"

#include <chrono>
#include <thread>

class CallCenter;
class Call;

class Operator {
public:
    Operator(CallCenter* parentCC);

    int getID() const;

    void setCurrentCall(Call* call);
    Call* getCurrentCall();
    const Call* getCurrentCall() const;

    void RunCall(int call_duration);

private:
    CallCenter* _parentCC {nullptr};
    Call* _currentCall {nullptr};

    inline static int _id_counter {0};
    const int _id;
};


#endif