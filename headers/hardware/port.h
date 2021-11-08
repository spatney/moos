#ifndef __MOOS__HARDWARE__PORT_H
#define __MOOS__HARDWARE__PORT_H

#include <common/types.h>

namespace moos
{
  namespace hardware
  {
    class Port
    {
    protected:
      Port(moos::common::uint16_t portnumber);
      ~Port();
      moos::common::uint16_t portnumber;
    };

    class Port8Bit : public Port
    {
    public:
      Port8Bit(moos::common::uint16_t portnumber);
      ~Port8Bit();

      virtual moos::common::uint8_t Read();
      virtual void Write(moos::common::uint8_t data);

    protected:
      static inline moos::common::uint8_t Read8(moos::common::uint16_t _port)
      {
        moos::common::uint8_t result;
        __asm__ volatile("inb %1, %0"
                         : "=a"(result)
                         : "Nd"(_port));
        return result;
      }

      static inline void Write8(moos::common::uint16_t _port, moos::common::uint8_t _data)
      {
        __asm__ volatile("outb %0, %1"
                         :
                         : "a"(_data), "Nd"(_port));
      }
    };

    class Port8BitSlow : public Port8Bit
    {
    public:
      Port8BitSlow(moos::common::uint16_t portnumber);
      ~Port8BitSlow();

      virtual void Write(moos::common::uint8_t data);

    protected:
      static inline void Write8Slow(moos::common::uint16_t _port, moos::common::uint8_t _data)
      {
        __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                         :
                         : "a"(_data), "Nd"(_port));
      }
    };

    class Port16Bit : public Port
    {
    public:
      Port16Bit(moos::common::uint16_t portnumber);
      ~Port16Bit();

      virtual moos::common::uint16_t Read();
      virtual void Write(moos::common::uint16_t data);

    protected:
      static inline moos::common::uint16_t Read16(moos::common::uint16_t _port)
      {
        moos::common::uint16_t result;
        __asm__ volatile("inw %1, %0"
                         : "=a"(result)
                         : "Nd"(_port));
        return result;
      }

      static inline void Write16(moos::common::uint16_t _port, moos::common::uint16_t _data)
      {
        __asm__ volatile("outw %0, %1"
                         :
                         : "a"(_data), "Nd"(_port));
      }
    };

    class Port32Bit : public Port
    {
    public:
      Port32Bit(moos::common::uint16_t portnumber);
      ~Port32Bit();

      virtual moos::common::uint32_t Read();
      virtual void Write(moos::common::uint32_t data);

    protected:
      static inline moos::common::uint32_t Read32(moos::common::uint16_t _port)
      {
        moos::common::uint32_t result;
        __asm__ volatile("inl %1, %0"
                         : "=a"(result)
                         : "Nd"(_port));
        return result;
      }

      static inline void Write32(moos::common::uint16_t _port, moos::common::uint32_t _data)
      {
        __asm__ volatile("outl %0, %1"
                         :
                         : "a"(_data), "Nd"(_port));
      }
    };
  }
}
#endif