#pragma once

#include "../APIMacros.hpp"

namespace geode {
    template<class T>
    class Ref final {
        static_assert(
            std::is_base_of_v<cocos2d::CCObject, T>,
            "Ref can only be used with a CCObject-inheriting class!"
        );

        T* m_obj = nullptr;

    public:
        Ref(T* obj) : m_obj(obj) {
            CC_SAFE_RETAIN(obj);
        }
        Ref(Ref<T> const& other) : Ref(other.data()) {}
        Ref(Ref<T>&& other) : m_obj(other.m_obj) {
            other.m_obj = nullptr;
        }
        Ref() = default;
        ~Ref() {
            CC_SAFE_RELEASE(m_obj);
        }

        void swap(T* other) {
            CC_SAFE_RELEASE(m_obj);
            m_obj = other;
            CC_SAFE_RETAIN(other);
        }
        T* data() const {
            return m_obj;
        }

        operator T*() const {
            return m_obj;
        }
        T* operator*() const {
            return m_obj;
        }
        T* operator->() const {
            return m_obj;
        }
        T* operator=(T* obj) {
            this->swap(obj);
            return obj;
        }
        Ref<T>& operator=(Ref<T> const& other) {
            this->swap(other.data());
            return *this;
        }
        Ref<T>& operator=(Ref<T>&& other) {
            this->swap(other.data());
            return *this;
        }
        bool operator==(T* other) const {
            return m_obj == other;
        }
        bool operator==(Ref<T> const& other) const {
            return m_obj == other.m_obj;
        }
    };
}
