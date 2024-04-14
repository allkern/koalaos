OUTPUT_FORMAT(binary)

MEMORY
{
    ROM (rx) : org = 0xbfc00000, len = 256k
    RAM (wx) : org = 0x80000000, len = 16M
}