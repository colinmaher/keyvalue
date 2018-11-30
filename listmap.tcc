// $Id: listmap.tcc,v 1.11 2018-01-25 14:19:14-08 - - $

#include "debug.h"
#include "listmap.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key, Value, Less>::node::node(node* next, node* prev,
    const value_type& value)
    : link(next, prev)
    , value(value)
{
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key, Value, Less>::~listmap()
{
    DEBUGF('l', reinterpret_cast<const void*>(this));
    node* current = begin().where;
    while (current != end().where) {
        node* next = current->next;
        delete current;
        current = next;
    }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator
listmap<Key, Value, Less>::insert(const value_type& pair)
{
    DEBUGF('l', &pair << "->" << pair);
    node* new_node;
    iterator itor;
    if (this->empty()) {
        new_node = new node(anchor(), anchor(), pair);
        anchor_.next = new_node;
        anchor_.prev = new_node;
        itor = begin();
    } else {
        itor = find(pair.first);
        // if found replace value
        if (itor != end()) {
            itor->second = pair.second;
        }

        // otherwise iterate over list until either the end is
        // reached or the first position where the key is
        // lexicographically less than the previous key.
        else {
            itor = begin();
            while (itor != end() && less(itor->first, pair.first)) {
                ++itor;
            }
            new_node = new node(itor.where, itor.where->prev, pair);
            itor.where->prev->next = new_node;
            itor.where->prev = new_node;
            itor = new_node;
        }
    }
    return itor;
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator
listmap<Key, Value, Less>::find(const key_type& that)
{
    DEBUGF('l', that);
    iterator itor = begin();
    while (itor != end() && itor->first != that) {
        ++itor;
    }
    return itor;
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator
listmap<Key, Value, Less>::erase(iterator position)
{
    DEBUGF('l', &*position);
    iterator itor = position.where->next;
    position.where->prev->next = position.where->next;
    position.where->next->prev = position.where->prev;
    delete position.where;
    return itor;
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::value_type&
    listmap<Key, Value, Less>::iterator::operator*()
{
    DEBUGF('l', where);
    return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::value_type*
    listmap<Key, Value, Less>::iterator::operator->()
{
    DEBUGF('l', where);
    return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator&
listmap<Key, Value, Less>::iterator::operator++()
{
    DEBUGF('l', where);
    where = where->next;
    return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator&
listmap<Key, Value, Less>::iterator::operator--()
{
    DEBUGF('l', where);
    where = where->prev;
    return *this;
}

//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key, Value, Less>::iterator::operator==(const iterator& that) const
{
    return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key, Value, Less>::iterator::operator!=(const iterator& that) const
{
    return this->where != that.where;
}
