#pragma once

#include "EventAction.hpp"
#include "EventProtocol.hpp"
#include <Geode.hpp>
#include <functional>

namespace geode {
    class EventCenter {
     protected:
        std::vector<EventAction> m_actions;

     public:
        static EventCenter* get();

        void broadcast(EventProtocol* evt);
        EventAction& registerAction(EventProtocol* evt, std::function<void(EventProtocol*)> f);
        void addAction(EventAction ea);
        void unregisterAction(EventAction ea);
    };
} // namespace geode