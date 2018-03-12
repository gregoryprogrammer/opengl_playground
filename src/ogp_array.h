#ifndef OGP_ARRAY_H
#define OGP_ARRAY_H

#include "ogp_defines.h"
#include "ogp_utils.h"

#include <vector>
#include <typeinfo>

namespace ogp
{

struct slot_t
{
    index_value_t data_index {-1};
    i64 version {-1};
};

template <typename T, i32 N>
class array_t
{
    i32 m_capacity {N};
    index_value_t m_top_data {0};
    index_value_t m_top_slot {0};
    i64 m_version_counter {0};

    std::vector<T> data {N};
    std::vector<slot_t> slots {N};
    std::vector<index_value_t> m_free_slots_indices {};

public:

    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    array_t()
    {
        static_assert(N > 0, "Array size should be greater than zero");
        ogp_log_debug("array_t<%s, %d>, initial capacity = %d", typeid(T).name(), N, m_capacity);
        data.reserve(N);
        slots.reserve(N);
        m_free_slots_indices.reserve(N);
        m_free_slots_indices.clear();
    }

    array_t(array_t const &) = delete;
    array_t(array_t &&) = delete;

    index_t add(T const &elem)
    {
        if (m_top_data >= m_capacity * 0.9f) {
            ogp_log_warning("More than 90%% elements in the array! %s %d / %d", typeid(T).name(), m_top_data, m_capacity);
            i32 num = static_cast<f32>(m_capacity) * 0.1f;
            enlarge(num);
        }

        if (m_top_data >= m_capacity) {
            // TODO Add a runtime flag to enable error and terminate() here instead of warning
            // TODO check if this is possible
            ogp_log_warning("Max elements exceed in the array! %s %d / %d", typeid(T).name(), m_top_data, m_capacity);
            return index_t::invalid();
        }

        // Place new element on top, always on top
        data[m_top_data] = elem;

        // Create a slot info and increase the data counter
        slot_t slot {};
        slot.data_index = m_top_data;
        slot.version = m_version_counter;
        m_top_data++;

        index_t user_index {};
        user_index.version = m_version_counter;

        // Slot info will is placed at top position
        if (m_free_slots_indices.size() > 0) {
            index_value_t available = m_free_slots_indices.back();
            m_free_slots_indices.pop_back();
            slots[available] = slot;
            user_index.value = available;
        }
        else {
            user_index.value = m_top_slot;
            slots[m_top_slot] = slot;
            m_top_slot++;
        }

        if (m_top_slot > m_capacity) {
            ogp_log_warning("top_slot >= capacity .......  %d >= %d", m_top_slot, m_capacity);
            terminate("what? possible?");
        }

        m_version_counter++;

        return user_index;
    }

    // For some container testing
    T data_at(i32 index) const
    {
        return data[index];
    }

    i32 capacity() const
    {
        return m_capacity;
    }

    void enlarge(i32 num)
    {
        m_capacity = m_capacity + num;
        ogp_log_info("Enlarging %s by %d, new capacity = %d", typeid(T).name(), num, m_capacity);
        data.resize(m_capacity);
        slots.resize(m_capacity);
    }

    T *get(index_t index)
    {
        return const_cast<T *>(static_cast<array_t<T, N> const *>(this)->get(index));
    }

    /** Get element by its index.
     * @param index Indicate slot info.
     */
    // TODO use 'exists' method
    T const *get(index_t index) const
    {
        if (index.value >= m_top_slot || index.value < 0) {
            ogp_log_warning("Index out of range (index.value = %d) >= (top_slot = %d), T = %s", index.value, m_top_slot, typeid(T).name());
            return nullptr;
        }

        slot_t slot = slots[index.value];

        if (slot.version == index.version) {
            return &data[slot.data_index];
        }

        return nullptr;
    }

    bool exists(index_t index) const
    {
        if (index.value >= m_top_slot || index.value < 0) {
            return false;
        }

        slot_t slot = slots[index.value];

        if (slot.version == index.version) {
            return true;
        }

        return false;
    }

    array_t::iterator begin()
    {
        return std::begin(data);
    }

    array_t::iterator end()
    {
        return std::begin(data) + m_top_data;
    }

    array_t::const_iterator begin() const
    {
        return std::begin(data);
    }

    array_t::const_iterator end() const
    {
        return std::begin(data) + m_top_data;
    }

    /** Remove element by its index.
     *  It replace pointed element with last element in data array.
     *  Slot info in slots array is fixed then.
     *  It works for one element array becase that element is switched with himself.
     *  \param index Indicate slot info.
     *
     *  \todo After some removals (about 50%), sort data within slots
     */
    void remove_index(index_t index)
    {
        if (index.value >= m_top_slot || index.value < 0) {
            ogp_log_warning("Index out of range (index.value = %d) >= (top_slot = %d)", index.value, m_top_slot);
            return;
        }

        slot_t slot = slots[index.value];

        if (slot.version != index.version) {
            ogp_log_warning("Cannot destroy, element does not exist (double destroy?)");
            return;
        }

        // Invalidate slot
        slot.version = -1;

        index_value_t data_index = slot.data_index;

        // Write back slot info
        slots[index.value] = slot;

        m_free_slots_indices.push_back(index.value);

        // Swap last element with removed one
        std::swap(this->data[data_index], this->data[m_top_data - 1]);

        // Find who is pointing with 'data_index' 'to 'top_data - 1' (last)
        // and change its pointer/index to 'removed slot.data_index'

        for (i32 i = 0; i < m_top_slot; ++i) {
            // TODO Checking slots from 0 to top_slot but it will be faster
            // to check in reverse order because last element was moved

            if (slots[i].version != -1 && slots[i].data_index == m_top_data - 1) {
                slots[i].data_index = data_index;
                break;
            }
        }

        m_top_data--;
    }

    // Rremove element by value (all occurences).
    i32 remove_element(T const &elem)
    {
        i32 count = 0;
        for (i32 i = 0; i < m_top_slot;) {
            slot_t slot = slots[i];

            if (slot.version == -1) {
                i = i + 1;
                continue;
            }

            if (this->data[slot.data_index] == elem) {
                index_t elem_index {};
                elem_index.value = i;
                elem_index.version = slot.version;
                remove_index(elem_index);
                count++;
                continue;
            }

            i = i + 1;
        }

        return count;
    }

    i32 size() const
    {
        return m_top_data;
    }

    /// Reset array
    void reset()
    {
        this->m_top_data = 0;
        this->m_top_slot = 0;
    }
};

}  // namespace ogp

#endif  // OGP_ARRAY_H
