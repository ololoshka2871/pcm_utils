#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cassert>

class semaphore
{
public:
    semaphore(unsigned int count) : m_count(count) {}
    semaphore(const semaphore&&) = delete;
    semaphore(semaphore&&) = delete;
    semaphore& operator = (const semaphore&) = delete;
    semaphore& operator = (semaphore&&) = delete;
    ~semaphore() = default;

    void post()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_count;
        m_cv.notify_one();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&] { return m_count > 0; });
        --m_count;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    unsigned int m_count;
};

template<typename T>
class LockingQueue
{
public:
    LockingQueue(unsigned int size)
        : m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
        m_data((T*)operator new(size * sizeof(T))),
        m_openSlots(size), m_fullSlots(0) {}

    LockingQueue(const LockingQueue&) = delete;
    LockingQueue(LockingQueue&&) = delete;
    LockingQueue& operator = (const LockingQueue&) = delete;
    LockingQueue& operator = (LockingQueue&&) = delete;

    ~LockingQueue()
    {
        while (m_count--)
        {
            m_data[m_popIndex].~T();
            m_popIndex = ++m_popIndex % m_size;
        }
        operator delete(m_data);
    }

    void push(const T& item)
    {
        m_openSlots.wait();
        {
            std::lock_guard<std::mutex> lock(m_cs);
            new (m_data + m_pushIndex) T(item);
            m_pushIndex = ++m_pushIndex % m_size;
            ++m_count;
        }
        m_fullSlots.post();
    }

    void push(T&& item)
    {
        m_openSlots.wait();
        {
            std::lock_guard<std::mutex> lock(m_cs);
            new (m_data + m_pushIndex) T(std::move(item));
            m_pushIndex = ++m_pushIndex % m_size;
            ++m_count;
        }
        m_fullSlots.post();
    }

    void pop(T& item)
    {
        m_fullSlots.wait();
        {
            std::lock_guard<std::mutex> lock(m_cs);
            item = m_data[m_popIndex];
            m_data[m_popIndex].~T();
            m_popIndex = ++m_popIndex % m_size;
            --m_count;
        }
        m_openSlots.post();
    }

    void pop(T&& item)
    {
        m_fullSlots.wait();
        {
            std::lock_guard<std::mutex> lock(m_cs);
            std::swap(item,  m_data[m_popIndex]);
            m_data[m_popIndex].~T();
            m_popIndex = ++m_popIndex % m_size;
            --m_count;
        }
        m_openSlots.post();
    }


    bool empty()
    {
        std::lock_guard<std::mutex> lock(m_cs);
        return m_count == 0;
    }

private:
    unsigned int m_size;
    unsigned int m_pushIndex;
    unsigned int m_popIndex;
    unsigned int m_count;
    T* m_data;

    semaphore m_openSlots;
    semaphore m_fullSlots;
    std::mutex m_cs;
};
