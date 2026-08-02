#pragma once

#include <cstdint>
#include <atomic>
#include <ostream>
#include <iostream>
namespace courier
{

    struct ObjectId
    {
        private:
        
        inline static std::atomic<uint32_t> g_id = 0;
        uint64_t m_id : 32;
        [[maybe_unused]] uint64_t m_other : 15;
        uint64_t m_parent : 1;
        uint64_t m_subId : 16;
        
        public:

        explicit ObjectId() : m_id(++g_id), m_parent(1), m_subId(0)
        {

        }

        ObjectId(const ObjectId& copy) : m_id(copy.m_id), m_parent(copy.m_parent), m_subId(copy.m_subId)
        {

        }

        ObjectId& operator=(ObjectId& other)
        {
            m_parent = other.m_parent;
            m_id = other.m_id;
            m_subId = other.m_subId;
            return *this;
        }

        ObjectId& operator=(ObjectId&& other)
        {
            m_parent = other.m_parent;
            m_id = other.m_id;
            m_subId = other.m_subId;
            return *this;
        }

        inline ObjectId newSubId()
        {
            return ObjectId(0, m_id, ++m_subId);
        }

        inline uint64_t id() const
        {
            uint64_t val = m_id;
            if(m_parent)
                return (val << 16);
            return (val << 16) | m_subId;
        };

        inline uint64_t rawId() const
        {
            uint64_t val = m_id;
            val = val << 32;
            val |= m_parent << 17;
            val |= m_subId;
            return val;
        }

        inline uint64_t parentId() const
        {
            uint64_t val = m_id;
            return val << 16;
        };

        inline bool operator!=(const ObjectId& other) const
        {
            return id() != other.id();
        }
        
        inline bool operator==(const ObjectId& other) const
        {
            return id() == other.id();
        }

        inline bool operator>(const ObjectId&other) const
        {
            return id() > other.id();
        }
        inline bool operator<(const ObjectId&other) const
        {
            return id() < other.id();
        }
        static inline ObjectId from(uint64_t rawId)
        {
            return ObjectId(rawId & (1<< 17), rawId >> 32, rawId & 0xffffu);
        }

        private:
        
        ObjectId(bool parent, uint32_t id, uint16_t subId) : m_id(id), m_parent(parent), m_subId(subId)
        {
        }

        inline friend std::ostream& operator<<(std::ostream& os, const courier::ObjectId& id)
        {
            os << "{id=" << (id.m_id << 16) << ", subId=" << id.m_subId << ", parent=" << id.m_parent << "}";
            return os;
        }
    };

}