#include <common/linkedlist.h>
#include <common/console.h>

using namespace moos::common;

LinkedListNode::LinkedListNode(void *data)
{
    next = 0;
    prev = 0;
    this->data = data;
}

LinkedListNode::~LinkedListNode()
{
    next = 0;
    prev = 0;
}

LinkedList::LinkedList()
{
    head = 0;
    tail = 0;
}

LinkedList::~LinkedList()
{
    head = 0;
    tail = 0;
}

void LinkedList::AddFirst(void *data)
{
    if (head == 0)
    {
        head = new LinkedListNode(data);
        tail = head;
    }
    else
    {
        LinkedListNode *temp = head;
        head = new LinkedListNode(data);
        head->next = temp;
        temp->prev = head;
    }
}

void LinkedList::AddLast(void *data)
{
    if (tail == 0)
    {
        tail = new LinkedListNode(data);
        head = tail;
    }
    else
    {
        auto *temp = tail;
        tail = new LinkedListNode(data);
        tail->prev = temp;
        temp->next = tail;
    }
}

void *LinkedList::RemoveFirst()
{
    if (head == 0)
    {
        return 0;
    }

    auto *result = head->data;
    auto temp = head;
    head = head->next;
    delete temp;

    if (head != 0)
    {
        head->prev = 0;
    }

    return result;
}

void *LinkedList::RemoveLast()
{
    if (tail == 0)
    {
        return 0;
    }

    auto *result = tail->data;
    auto temp = tail;
    tail = tail->prev;
    delete temp;

    if (tail != 0)
    {
        tail->next = 0;
    }

    return result;
}

void *LinkedList::PeekFirst()
{
    if (head == 0)
    {
        return 0;
    }

    return head->data;
}

void *LinkedList::PeekLast()
{
    if (tail == 0)
    {
        return 0;
    }

    return tail->data;
}

void LinkedList::FreeList()
{
    auto curr = head;
    while (curr != 0)
    {
        auto temp = curr->next;
        delete curr;
        curr = temp;
    }
    delete this;
}