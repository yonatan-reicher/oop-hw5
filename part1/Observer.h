#pragma once

template <typename T>
struct Observer {
    virtual void handleEvent(const T&) = 0;
};
