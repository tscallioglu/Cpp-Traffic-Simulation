#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // wait for and receive new messages and pull them from the queue using move semantics. 
    std::unique_lock<std::mutex> _ulock(_mutex);
    _condition.wait(_ulock, [this] { return !_queue.empty(); });
    T v = std::move(_queue.back());

    _queue.clear();

    return v;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> _ulock(_mutex);

    // Add vector to queue
    _queue.push_back(std::move(msg));

    // Notify client when adding a new vehicle into vector
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        _currentPhase = _messageQueue.receive();
        if (_currentPhase == TrafficLightPhase::green)
        {
            break;
        }
    }
    return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // Start a thread when the public method „simulate“ is called. Use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // Measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    std::random_device rndm;
    std::mt19937 mt1(rndm());

    // The cycle duration is a random value between 4 and 6 seconds. 
    std::uniform_int_distribution<> distbt(4000, 6000);
    int cycleDuration = distbt(mt1); // Random duration of a cycle

    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::milliseconds passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    while(true){
        // Waits 1ms between two cycles. 
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (passedTime.count() > cycleDuration)
        {
            if(_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
                startTime = std::chrono::system_clock::now(); 
            }
            else
            {
                _currentPhase = TrafficLightPhase::red;
                startTime = std::chrono::system_clock::now();                         
            }
        }

        cycleDuration = distbt(mt1);   // Random duration for the next cycle

        endTime = std::chrono::system_clock::now();
        passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        _messageQueue.send(std::move(_currentPhase));
    }
}

