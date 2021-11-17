#include <common/strings.h>

using namespace moos::common;

int32_t StringUtil::strlen(const int8_t *s)
{
    int32_t count = 0;
    while (*s != '\0')
    {
        count++;
        s++;
    }
    return count;
}

int32_t StringUtil::strcmp(const char *X, const char *Y)
{
    while (*X)
    {
        if (*X != *Y)
        {
            break;
        }

        X++;
        Y++;
    }

    return *(const unsigned char *)X - *(const unsigned char *)Y;
}