#ifndef __MOOS__COMMON__LINKED_LIST_H
#define __MOOS__COMMON__LINKED_LIST_H

#include <common/types.h>

namespace moos
{
    namespace common
    {
        class LinkedListNode
        {
        private:
            LinkedListNode *next;
            LinkedListNode *prev;

            LinkedListNode(void *data);
            ~LinkedListNode();

        public:
            friend class LinkedList;
            void *data;
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
        };
    }
}

#endif