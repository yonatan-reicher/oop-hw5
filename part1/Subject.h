#pragma once

#include <vector>
#include "Observer.h"
#include "OOP5EventException.h"

template <typename T>
class Subject {
    std::vector<Observer<T>*> m_observers;

    int tryGetIndex(Observer<T>& o) {
        for (int i = 0; i < m_observers.size(); ++i) {
            if (m_observers[i] == &o) {
                return i;
            }
        }
        return -1;
    }
public:

    Subject() : m_observers() {}    
    
    void notify(const T& x) {
        for (Observer<T>* ptr : m_observers) {
            ptr->handleEvent(x);
        }
    }

    void addObserver(Observer<T>& o) {
        int i = tryGetIndex(o);

        if (i != -1) {
            throw ObserverAlreadyKnownToSubject();
        };

        this->m_observers.push_back(&o);
    }

    void removeObserver(Observer<T>& o) {
        int i = tryGetIndex(o);

        if (i == -1) {
            throw ObserverUnknownToSubject();
        }

        m_observers.erase(m_observers.begin() + i);
    }

    Subject<T>& operator += (Observer<T>& o) {
        addObserver(o);
        return *this;
    }

    Subject<T>& operator -= (Observer<T>& o) {
        removeObserver(o);
        return *this;
    }

    Subject<T>& operator () (const T& x) {
        notify(x);
        return *this;
    }
};
