#include <iostream>

using namespace std;

template<class T>
class Queue{
    public:
    Queue(int queueCapacity = 10);
    bool isEmpity() const;
    T& Front() const;
    T& Rear() const;
    void Push(const T& x);
    void Pop();
    int Size();
    int Capacity();

    private:
    T *queue;
    int front, rear, capacity;
};

template<class T>
Queue<T>::Queue(int queueCapacity)
:capacity{1}, front{0}, rear{0}{
    queue = new T [capacity];
}

template<class T>
inline bool Queue<T>::isEmpity() const{
    return (front == rear);
}

template<class T>
inline T& Queue<T>::Front() const{
    if (isEmpity()) throw "Queue is empity.";
    return queue[(front + 1) % capacity];
}

template<class T>
inline T& Queue<T>::Rear() const{
    if (isEmpity()) throw "Queue is empity.";
    return queue[rear];
}

template<class T>
void Queue<T>::Push(const T& x){
    if ((rear + 1) % capacity == front){
        T *temp = new T [2 * capacity];
        int start = (front + 1) % capacity;
        if (start < 2){
            copy(queue + start, queue + rear + 1, temp);
        }else {
            copy(queue + start, queue + capacity, temp);
            copy(queue, queue + rear + 1, temp + capacity - start);
        }
        front = 2 * capacity - 1;
        rear = capacity - 2;
        capacity *= 2;
        delete [] queue;
        queue = temp;
    }
    rear = (rear + 1) % capacity;
    queue[rear] = x;
}

template<class T>
void Queue<T>::Pop(){
    if (isEmpity()) throw "Queue is empity.";
    front = (front + 1) % capacity;
    queue[front].~T();
}

template<class T>
int Queue<T>::Size(){
    if (isEmpity()) return 0;
    if (front < rear){
        return rear - front;
    } else if (front > rear){
        return capacity - (front - rear);
    }
}

template<class T>
int Queue<T>::Capacity(){
    return capacity;
}