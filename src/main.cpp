#include "main.h"

const int vID = 0x00; //Your VID
const int pID = 0x00; //Your PID

Keyboard::Keyboard() {
    int r = libusb_init(&ctx);
    if (r < 0) {
        std::cerr << "Failed to initialize libusb: " << libusb_strerror(static_cast<libusb_error>(r)) << std::endl;
        ctx = nullptr;
    } 
    
    // else {
    //     libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);
    // }

    lsize = libusb_get_device_list(ctx, &list);
    if (lsize < 0) {
        std::cerr << "Failed to get device list: " << libusb_strerror(static_cast<libusb_error>(lsize)) << std::endl;
    }
    handle = nullptr;

}

Keyboard::~Keyboard() {
    if(handle) {
        libusb_attach_kernel_driver(handle, interface);
        libusb_release_interface(handle, interface);
        libusb_close(handle);
    }
    
    if (ctx) {
        libusb_exit(ctx);
    }
}

int Keyboard::printList() {
    for (ssize_t i = 0; i < lsize; ++i) {
        int rv = libusb_get_device_descriptor(list[i], &desc);
        if (rv < 0) {
            std::cerr << "Failed to get device descriptor: " << libusb_strerror(static_cast<libusb_error>(rv)) << std::endl;
            libusb_free_device_list(list, 1);
            return -1;
 
        }

        std::cout << "Vendor ID: " << std::hex << desc.idVendor << " | "
                << "Product ID: " << std::hex << desc.idProduct << std::endl;
    }
    libusb_free_device_list(list, 1);
    return 0;
}



int Keyboard::verifyKeyboard() {
    int result = 0;
    for (ssize_t i = 0; i < lsize; ++i) {
        int rv = libusb_get_device_descriptor(list[i], &desc);
        if (rv < 0) {
            std::cerr << "Failed to get device descriptor: " << libusb_strerror(static_cast<libusb_error>(rv)) << std::endl;
            libusb_free_device_list(list, 1);
            return -1;
        }
        if(desc.idVendor == vID && desc.idProduct == pID) {
            int rx = libusb_open(list[i], &handle);
            if(rx != 0) {
                std::cout << "Could Not Open Keyboard" << std::endl;
                return -1;
            }
            int ry = libusb_set_auto_detach_kernel_driver(handle, 1);
            if(ry != 0) {
                std::cout << "Failed to enable auto detach" << std::endl;
                return -1;
            }
            int rz = libusb_claim_interface(handle, interface);
            if(rz != 0) {
                std::cout << "Failed to claim interface: " << libusb_error_name(rz) << std::endl;
                return -1;
            }
            result = 1;
        }
    }
    if(result == 1) {
        std::cout << "Keyboard Connected: Vendor ID: 258a | Product ID: e7" << std::endl;
        libusb_free_device_list(list, 1);
        return 0;
    } else {
        std::cout << "Could Not Verify Keyboard" << std::endl;
        libusb_free_device_list(list, 1);
        return -1;
    }
}


int Keyboard::brightness(int level) {
    if (level < 0 || level > 5) {
        std::cerr << "Invalid brightness level. It should be between 0 and 5." << std::endl;
        return -1;
    }

    unsigned char result = static_cast<unsigned char>(level);
    int buff = bufferManagment(n_bright, result);
    if(buff < 0) {
        std::cout << "Error Changing Brightness" << std::endl;
        return -1;
    }
    int r = libusb_control_transfer(handle, request_type, request, wval, windex, buffer, sizeof(buffer), timeout);
    if (r < 0) {
        std::string error = libusb_error_name(r);
        std::cerr <<  error <<  "  Control transfer failed: " << libusb_strerror(static_cast<libusb_error>(r)) << std::endl;
        return -1;
    }

    std::cout << "Brightness set to level " << level << std::endl;
    return 0;
}

int Keyboard::mode(int number) {
    if (number < 1 || number > 21) {
        std::cerr << "Invalid mode index. It should be between 1 and 21." << std::endl;
        return -1;
    }

    unsigned char result = static_cast<unsigned char>(number);
    int buff = bufferManagment(n_mode, result);
    if(buff < 0) {
        std::cout << "Error Changing Mode" << std::endl;
        return -1;
    }
    int rc = libusb_control_transfer(handle, request_type, request, wval, windex, buffer, sizeof(buffer), timeout);
    if (rc < 0) {
        std::string error = libusb_error_name(rc);
        std::cerr <<  error <<  "  Control transfer failed: " << libusb_strerror(static_cast<libusb_error>(rc)) << std::endl;
        return -1;
    }

    std::cout << "Mode Number: " << number << std::endl;
    return 0;
}

int Keyboard::colour() {
    int16_t r, g, b;
    std::cout << "Red: ";
    std::cin >> r;
    std::cout << "Green: ";
    std::cin >> g;
    std::cout << "Blue: ";
    std::cin >> b;

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        std::cout << "Invalid Range for colours {0 - 255}" << std::endl;
        return -1;
    }

    unsigned char r1 = static_cast<unsigned char>(r);
    unsigned char g1 = static_cast<unsigned char>(g);
    unsigned char b1 = static_cast<unsigned char>(b);

    FILE* rFile = fopen(filename, "rb");
    if (!rFile) {
        std::cout << "Error Opening File" << std::endl;
        return -1;
    }

    int read = fread(buffer, 1, 65, rFile);
    fclose(rFile);
    if (read != 65) {
        std::cout << "Error Reading File" << std::endl;
        return -1;
    }

    buffer[r_colour] = r1;
    buffer[g_colour] = g1;
    buffer[b_colour] = b1;

    FILE* wFile = fopen(filename, "wb");
    if (!wFile) {
        std::cout << "Error Opening File" << std::endl;
        return -1;
    }

    int write = fwrite(buffer, 1, 65, wFile);
    fclose(wFile);
    if (write != 65) {
        std::cout << "Error Writing to File" << std::endl;
        return -1;
    }

    std::cout << "Settings saved to " << filename << std::endl;

    int rc = libusb_control_transfer(handle, request_type, request, wval, windex, buffer, 65, timeout);
    if (rc < 0) {
        std::cerr << "Control transfer failed: " << libusb_strerror(static_cast<libusb_error>(rc)) << std::endl;
        return -1;
    }

    std::cout << "Changed Colour" << std::endl;
    return 0;
}

int Keyboard::speed(int number) {
    if (number < 1 || number > 5) {
        std::cerr << "Invalid animation speed. It should be between 1 and 5." << std::endl;
        return -1;
    }

    unsigned char result = static_cast<unsigned char>(number);
    int buff = bufferManagment(n_speed, result);
    if(buff < 0) {
        std::cout << "Error Changing Mode" << std::endl;
        return -1;
    }
    int rc = libusb_control_transfer(handle, request_type, request, wval, windex, buffer, sizeof(buffer), timeout);
    if (rc < 0) {
        std::string error = libusb_error_name(rc);
        std::cerr <<  error <<  "  Control transfer failed: " << libusb_strerror(static_cast<libusb_error>(rc)) << std::endl;
        return -1;
    }

    std::cout << "Animation Speed: " << number << std::endl;
    return 0;
}


int Keyboard::bufferManagment(int index, unsigned char value) {
    FILE* rFile = fopen(filename, "rb");
    if(!rFile) {
        std::cout << "Error Openeing File" << std::endl;
        return -1;
    }
    int read = fread(buffer, 1, 65, rFile);
    if(read < 0) {
        std::cout << "Error Reading From File" << std::endl;
        return -1;
    }
    fclose(rFile);
    std::cout << "Settings loaded from " << filename << std::endl;
    buffer[index] = value;

    FILE* wFile = fopen(filename, "wb");
    if(!wFile) {
        std::cout << "Error Openeing File" << std::endl;
        return -1;
    }
    int write = fwrite(buffer, 1, 65, wFile);
    if (write < 0) {
        std::cout << "Error Writing to File" << std::endl;
        return -1;
    }
    std::cout << "Setting saved to " << filename << std::endl;
    fclose(wFile);
    return 0;
}


void Keyboard::help() {
    std::cout << "Invalid Request" << std::endl;
    std::cout << "Flags: " << std::endl  << "\t-l\tList Devices\n\t-b\tChange Brightness(0 - 5)\n\t-m\tChange Animation Mode (0 - 21)\n\t-s\tChange Animation Speed(0 - 5)\n\t-c\tChange Colour (0 - 255)" << std::endl;
    exit(-1);
}


int checkAndCreateFile(const char* filename) {
    if (!std::filesystem::exists(filename)) {
        unsigned char buffer[65] = {
            0x0a, 0x01, 0x01, 0x02, 0x29, 0x14, 0x01, 0x01, 0x05, 0x00, 
            0xff, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00
        };

        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Cannot open file for writing: " << filename << std::endl;
            return 1; 
        }

       
        outFile.write(reinterpret_cast<const char*>(buffer), sizeof(buffer));
        outFile.close();

        std::cout << "File created with specified values: " << filename << std::endl;
        return 0; 
    } else {
        std::cout << "File already exists: " << filename << std::endl;
        return 2; 
    }
}


void Keyboard::getConfigFilePath() {
    const char* home = getenv("HOME");
    
    if (home != nullptr) {
        const char* configFileName = "/.key-config.bin";
        
        int fullPathLength = strlen(home) + strlen(configFileName) + 1;
        
        char* fullPath = new char[fullPathLength];
        
        strcpy(fullPath, home);
        strcat(fullPath, configFileName);

        filename = fullPath;
    } else {
        std::cout << "Error";
        exit(-1);
    }
}


int main(int argc, char* argv[]) {
    Keyboard k;
    k.getConfigFilePath();
    checkAndCreateFile(k.filename);

    if(argc > 3 | argc < 2) {
        k.help();
    }



    if(argc == 2) {
        std::string arg1 = argv[1];
        if(arg1 == "-l") {
            k.printList();
        } else if(arg1 == "-c") {
            if(k.verifyKeyboard() != 0) {
                return -1;
            }
            k.colour();
        } else {
            k.help();
        }
    }

    if(argc == 3) {
        std::string arg1 = argv[1];
        std::string arg2 = argv[2];

        if(k.verifyKeyboard() != 0) {
            return -1;
        }
        const std::set<std::string> valid_brightness = {"0", "1", "2", "3", "4", "5"};
        const std::set<std::string> valid_speed = {"1", "2", "3", "4", "5"};
        const std::set<std::string> valid_modes = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10","11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21"};
        if(arg1 == "-b") {
            if(valid_brightness.find(arg2) == valid_brightness.end()) {
                std::cout << "Invalid: Brightness Should have value from 0 to 5" << std::endl;
                return -1;
            }
            int level = stoi(arg2);
            k.brightness(level);
        } else if(arg1 == "-m") {
            if(valid_modes.find(arg2) == valid_modes.end()) {
                std::cout << "Invalid: Mode Should have value from 1 to 21" << std::endl;
                return -1;
            }
            int level = stoi(arg2);
            k.mode(level);
        } else if(arg1 == "-s") {
            if(valid_speed.find(arg2) == valid_speed.end()) {
                std::cout << "Invalid: Animation Speed Should have value from 1 to 5" << std::endl;
                return -1;
            }
            int level = stoi(arg2);
            k.speed(level);
        } else {
            k.help();
        }
    }
    return 0;
}
