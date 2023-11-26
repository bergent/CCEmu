#ifndef CCEMU_OPERATOR_H
#define CCEMU_OPERATOR_H

#include "callcenter.h"

#include <chrono>
#include <mutex>
#include <thread>

class CallCenter;

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

    void StartConversation(int call_duration);

    inline static int _id_counter {1};
    const int _id;
};


#endif