#ifndef LINKED_LIST_H
#define LINKED_LIST_H

template <typename T>
class LinkedList {
public:
    virtual ~LinkedList() = default;

    virtual void destroy() = 0;

    virtual int  get_size() = 0;
    virtual bool empty()    = 0;

    virtual T&   get_item(int pos) = 0;
    virtual T    get_first()       = 0;
    virtual T    get_last()        = 0;
    virtual T    get_middle()      = 0;
    virtual T    get_max()         = 0;
    virtual T    get_min()         = 0;
    virtual bool search(T value)   = 0;
    virtual void update_min_max()  = 0;

    virtual void insert_at_first    (T value, bool show = true)          = 0;
    virtual void insert_at_end      (T value, bool show = true)          = 0;
    virtual void insert_at_position (int pos, T value, bool show = true) = 0;

    virtual void delete_at_first    (bool show = true)                   = 0;
    virtual void delete_at_end      (bool show = true)                   = 0;
    virtual void delete_at_position (int pos, bool show = true)          = 0;

    virtual void print() = 0;
    virtual void reset() = 0;
};

#endif