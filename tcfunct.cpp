#include <iostream>
#include <memory>
#include <deque>
#include <algorithm>
#include <chrono>
 
template<typename T>
class Queue {
    struct Node {
        std::deque<T> front, back;
 
        Node(std::deque<T> f, std::deque<T> b) : front(f), back(b) {}
    };
    std::shared_ptr<Node> state;
 
public:
    Queue() : state(std::make_shared<Node>(std::deque<T>(), std::deque<T>())) {}
 
    Queue enqueue(T value) const {
        auto newBack = state->back;
        newBack.push_back(value);
        return Queue(std::make_shared<Node>(state->front, newBack));
    }
 
    std::pair<bool, T> dequeue() const {
        if (state->front.empty() && state->back.empty()) return {false, T()};
        auto newFront = state->front;
        if (newFront.empty()) {
            newFront = state->back;
            std::reverse(newFront.begin(), newFront.end());
        }
        T value = newFront.front();
        newFront.pop_front();
        return {true, value};
    }
 
    size_t getMemorySize() const {
        size_t size = sizeof(*state);
        size += sizeof(std::deque<T>) * 2;
        size += sizeof(T) * (state->front.size() + state->back.size());
        return size;
    }
 
private:
    Queue(std::shared_ptr<Node> node) : state(node) {}
};
 
template <typename Func>
void measureTime(const std::string& operation, Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << operation << " took " << duration.count() << " ms" << std::endl;
}
 
void testQueueOperations(size_t numElements) {
    Queue<int> queue;
 
    std::cout << "\nTesting " << numElements << " elements:\n";

    measureTime("Enqueue operation", [&]() {
        for (size_t i = 0; i < numElements; ++i) {
            queue = queue.enqueue(i);
        }
    });
    std::cout << "Memory usage after enqueue: " << queue.getMemorySize() << " bytes" << std::endl;

    measureTime("Dequeue operation", [&]() {
        for (size_t i = 0; i < numElements; ++i) {
            auto result = queue.dequeue();
        }
    });
}
 
int main() {
    testQueueOperations(10000);
    testQueueOperations(20000);
    testQueueOperations(30000);

    return 0;
}