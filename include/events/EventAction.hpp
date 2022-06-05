#pragma once

#include "EventProtocol.hpp"
#include <Geode.hpp>

namespace geode {
    class EventAction {
     private:
        dispatch_handle m_callback;
        EventProtocol* m_filter;
        void intercept(EventProtocol* evt);
        EventAction(dispatch_handle cb, EventProtocol* ft) : m_callback(cb), m_filter(ft) {}

     public:
        void unregister();
    };
} // namespace geode