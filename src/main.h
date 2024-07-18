#ifndef _USB_KEYBOARD
#define _USB_KEYBOARD

#include <stdint.h>
#include <iostream>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <set>
#include <fstream>
#include <filesystem>


class Keyboard {
public:
    const char* filename;
    Keyboard();
    ~Keyboard(); 
    int printList();
    int verifyKeyboard();
    void help();

    int brightness(int level);
    int mode(int number);
    int colour();
    int speed(int number);

    void getConfigFilePath();

private:
    libusb_context *ctx;
    libusb_device **list;
    libusb_device_descriptor desc;
    libusb_device_handle *handle;
    ssize_t lsize;

    int interface = 1;
    uint8_t request_type = 0x21;
    uint8_t request = 9;
    uint16_t wval = 0x030a;
    uint16_t windex = 0x0001;
    uint16_t wlen = 65;
    uint timeout = 1000;

    unsigned char buffer[65] = {0};

    int n_bright = 8;
    int n_mode = 5;
    int n_speed = 7;

    int b_colour = 11;
    int g_colour = 10;
    int r_colour = 9;

    int bufferManagment(int index, unsigned char value); 
};

#endif