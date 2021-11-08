#ifndef __GDT_H
#define __GDT_H

#include <common/types.h>

namespace moos
{
  class GlobalDescriptorTable
  {
  public:
    class SegmentDescriptor
    {
    private:
      moos::common::uint16_t limit_lo;
      moos::common::uint16_t base_lo;
      moos::common::uint8_t base_hi;
      moos::common::uint8_t type;
      moos::common::uint8_t limit_hi;
      moos::common::uint8_t base_vhi;

    public:
      SegmentDescriptor(moos::common::uint32_t base, moos::common::uint32_t limit, moos::common::uint8_t type);
      moos::common::uint32_t Base();
      moos::common::uint32_t Limit();
    } __attribute__((packed));

  private:
    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;

  public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    moos::common::uint16_t CodeSegmentSelector();
    moos::common::uint16_t DataSegmentSelector();
  };
}

#endif