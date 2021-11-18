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
            void Incr()
            {
                count++;
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
            RefCounter *reference;

        public:
            shared_ptr() : pData(0), reference(0)
            {
                reference = new RefCounter();
                reference->Incr();
            }

            shared_ptr(T *pValue) : pData(pValue), reference(0)
            {
                reference = new RefCounter();
                reference->Incr();
            }

            shared_ptr(const shared_ptr<T> &sp) : pData(sp.pData), reference(sp.reference)
            {
                reference->Incr();
            }

            ~shared_ptr()
            {
                if (reference->Decr() == 0)
                {
                    delete pData;
                    delete reference;
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
                    if (reference->Decr() == 0)
                    {
                        delete pData;
                        delete reference;
                    }

                    pData = sp.pData;
                    reference = sp.reference;
                    reference->Incr();
                }
                return *this;
            }
        };
    }
}

#endif