#include "operator.h"

Operator::Operator(CallCenter* parentCC) 
    :_id{_id_counter++}, _parentCC{parentCC}
{}

int Operator::getID() const {
    return _id;
}

void Operator::setCurrentCall(Call* call) {
    _currentCall = call;
}

Call* Operator::getCurrentCall() {
    return _currentCall;
}

const Call* Operator::getCurrentCall() const {
    return _currentCall;
}

void Operator::RunCall(int call_duration) {
    
    std::this_thread::sleep_for(std::chrono::milliseconds(call_duration));
}

