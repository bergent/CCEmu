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

    void setCurrentCall(std::shared_ptr<Call> call);
    std::shared_ptr<Call> getCurrentCall();
    const std::shared_ptr<Call> getCurrentCall() const;

    void RunCall(int call_duration);

private:
    CallCenter* _parentCC {nullptr};
    std::shared_ptr<Call> _currentCall {nullptr};

    void StartConversation(int call_duration);

    inline static int _id_counter {1};
    const int _id;
};


#endif