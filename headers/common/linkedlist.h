#ifndef __MOOS__COMMON__LINKED_LIST_H
#define __MOOS__COMMON__LINKED_LIST_H

#include <common/types.h>

namespace moos
{
    namespace common
    {
        class LinkedListNode
        {
            friend class LinkedListNodeIterator;
            friend class LinkedList;

            LinkedListNode *next;
            LinkedListNode *prev;
            void *data;

            LinkedListNode(void *data);
            ~LinkedListNode();
        };

        class LinkedListNodeIterator
        {
        private:
            LinkedListNode *node;

        public:
            LinkedListNodeIterator(LinkedListNode *node) { this->node = node; }

            LinkedListNodeIterator const &operator++()
            {
                if (node != 0)
                {
                    node = node->next;
                }
                return *this;
            }

            friend bool operator==(const LinkedListNodeIterator &a, const LinkedListNodeIterator &b) { return a.node == b.node; };
            friend bool operator!=(const LinkedListNodeIterator &a, const LinkedListNodeIterator &b) { return a.node != b.node; };
        };

        class LinkedList
        {
        private:
            LinkedListNode *head;
            LinkedListNode *tail;

        public:
            LinkedList();
            ~LinkedList();
            void AddFirst(void *data);
            void AddLast(void *data);

            void *RemoveFirst();
            void *RemoveLast();

            void *PeekFirst();
            void *PeekLast();

            void *PeekAt(int32_t index);

            LinkedListNodeIterator begin() { return LinkedListNodeIterator(head); }
            LinkedListNodeIterator end() { return LinkedListNodeIterator(0); }
        };
    }
}

#endif