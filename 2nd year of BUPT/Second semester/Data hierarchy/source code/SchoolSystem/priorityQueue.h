
#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
#include <vector>

template<typename T>
class PriorityQueue {
private:
    std::vector<T> heap;

    // 调整堆使其满足堆的性质
    void heapifyUp(int index) {
        if (index == 0) {
            return;
        }

        int parentIndex = (index - 1) / 2;
        if (heap[parentIndex] < heap[index]) {
            std::swap(heap[parentIndex], heap[index]);
            heapifyUp(parentIndex);
        }
    }

    // 调整堆使其满足堆的性质
    void heapifyDown(int index) {
        int leftChildIndex = 2 * index + 1;
        int rightChildIndex = 2 * index + 2;
        int largest = index;

        if (leftChildIndex < heap.size() && heap[leftChildIndex] > heap[largest]) {
            largest = leftChildIndex;
        }

        if (rightChildIndex < heap.size() && heap[rightChildIndex] > heap[largest]) {
            largest = rightChildIndex;
        }

        if (largest != index) {
            std::swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    // 将元素插入队列
    void push(const T& value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    // 弹出队列中的最大（或最小）元素
    void pop() {
        if (heap.empty()) {
            return; // 队列为空
        }

        std::swap(heap[0], heap[heap.size() - 1]);
        heap.pop_back();
        heapifyDown(0);
    }

    // 返回队列中的最大（或最小）元素
    const T& top() const {
        return heap[0];
    }

    // 检查队列是否为空
    bool empty() const {
        return heap.empty();
    }

};
#endif // PRIORITYQUEUE_H
