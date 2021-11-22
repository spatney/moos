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

int32_t StringUtil::strcmp(const int8_t *X, const int8_t *Y)
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

    return *(const uint8_t *)X - *(const uint8_t *)Y;
}

int32_t StringUtil::atoi(const int8_t* S)
{
    int32_t num = 0;
 
    int32_t i = 0;

    while (S[i] && (S[i] >= '0' && S[i] <= '9'))
    {
        num = num * 10 + (S[i] - '0');
        i++;
    }
 
    return num;
}