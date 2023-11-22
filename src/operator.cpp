#include "operator.h"

Operator::Operator(CallCenter* parentCC) 
    :_id{Operator::_id_counter++}, _parentCC{parentCC}
{}

int Operator::getID() const {
    return _id;
}

std::string Operator::getIDStr() const {
    return std::to_string(_id);
}

void Operator::RunCall(int call_duration) {
    
    std::this_thread::sleep_for(std::chrono::milliseconds(call_duration));
}