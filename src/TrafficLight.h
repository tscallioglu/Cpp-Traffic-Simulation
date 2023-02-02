#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"
#include <chrono>

// forward declarations to avoid include cycle
class Vehicle;

enum TrafficLightPhase {red=0, green};

// Defines an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
template <class T>
class MessageQueue
{
public:
    void send(T &&msd);
    T receive();
    std::deque<TrafficLightPhase> _queue; 

private:
    std::condition_variable _condition;
    std::mutex _mutex;
};

// TrafficLightPhase is an enum that can be either „red“ or „green“.
// _currentPhase can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate();

    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;

    MessageQueue<TrafficLightPhase> _messageQueue;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif