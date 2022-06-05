#pragma once

#include "EventProtocol.hpp"
#include <Geode.hpp>

namespace geode {
    class EventCenter;

    class EventAction {
        dispatch_handle m_callback;
        EventProtocol* m_filter;
        void intercept(EventProtocol* evt);
        EventAction(dispatch_handle cb, EventProtocol* ft) : m_callback(cb), m_filter(ft) {}

        friend class EventCenter;

     public:
        void unregister();
        bool operator==(EventAction const&) const;
    };
} // namespace geode