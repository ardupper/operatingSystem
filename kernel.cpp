
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

uint8_t column = 0;
uint8_t isRunCommand = 1;
uint8_t isCalculatorRunCommand = 1;
uint8_t isCalculatorRuntime = 0;
char digits[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

void printf(char* str);


void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;
    
    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            
            case '~':
                x = x - 1;
                printf(" ");
                x = x - 1;
                break;
            
                
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }
        
        if(x >= 80)
        {
            x = 0;
            y++;
        }
        
        if(x < 0) {
            x = 79;
            y--;
        }
        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello World!");
    
    uint8_t win[5] = {0, 10, 20, 30, 40};

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);
    KeyboardDriver keyboard(&interrupts);
    interrupts.Activate();

    while(1);
}