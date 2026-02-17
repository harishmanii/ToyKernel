int str_to_int(char *str)
{
    int result = 0;

    while (*str)
    {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}

// uint32_t atoi(const uint8_t *string)
// {
//     uint32_t result = 0;

//     // TODO: Handle hex number string e.g. 1st 2 characters will be "0x" or "0X"
    
//     for (uint32_t i = 0; string[i] != '\0'; i++)
//         result = result * 10 + string[i] - '0';

//     return result;
// }