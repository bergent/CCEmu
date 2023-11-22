#ifndef CCEMU_RANDOMIZER_H
#define CCEMU_RANDOMIZER_H

#include <random>

class Randomizer {
public:
    explicit Randomizer(int min_queue, int max_queue,
               int min_call, int max_call)
    : _min_queue{min_queue}, _max_queue{max_queue},
      _min_call{min_call}, _max_call{max_call}
    {
        _queue_distrib = new std::uniform_int_distribution<int>(_min_queue, _max_queue);
        _call_distrib = new std::uniform_int_distribution<int>(_min_call, _max_call);
    }

    ~Randomizer() {
        delete _queue_distrib;
        delete _call_distrib;
    }

    int getQueueTime() {
        int time { (*(_queue_distrib))(_mt) };
        return time;
    }

    int getCallTime() {
        int time { (*(_call_distrib))(_mt) };
        return time;
    }

private:
    const int _min_queue;
    const int _max_queue;
    const int _min_call;
    const int _max_call;

    std::random_device _rd;
    std::mt19937_64 _mt {_rd()};
    
    std::uniform_int_distribution<int>* _queue_distrib {nullptr};
    std::uniform_int_distribution<int>* _call_distrib {nullptr};
};

#endif