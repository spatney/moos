#ifndef __CORE__SHARED_PTR_H
#define __CORE__SHARED_PTR_H

#include <common/types.h>

namespace moos
{
    namespace core
    {

        class RefCounter
        {
        private:
            common::size_t count;

        public:
            common::size_t Incr()
            {
                return ++count;
            }

            common::size_t Decr()
            {
                return --count;
            }
        };

        template <typename T>
        class shared_ptr
        {
        private:
            T *pData;
            RefCounter *refCounter;

        public:
            shared_ptr() : pData(0), refCounter(0)
            {
                refCounter = new RefCounter();
                refCounter->Incr();
            }

            shared_ptr(T *pValue) : pData(pValue), refCounter(0)
            {
                refCounter = new RefCounter();
                refCounter->Incr();
            }

            shared_ptr(const shared_ptr<T> &sp) : pData(sp.pData), refCounter(sp.refCounter)
            {
                refCounter->Incr();
            }

            ~shared_ptr()
            {
                if (refCounter->Decr() == 0)
                {
                    delete pData;
                    delete refCounter;
                }
            }

            T &operator*()
            {
                return *pData;
            }

            T *operator->()
            {
                return pData;
            }

            shared_ptr<T> &operator=(const shared_ptr<T> &sp)
            {
                if (this != &sp)
                {
                    if (refCounter->Decr() == 0)
                    {
                        delete pData;
                        delete refCounter;
                    }

                    pData = sp.pData;
                    sp.refCounter->Incr();
                }
                return *this;
            }
        };
    }
}

#endif