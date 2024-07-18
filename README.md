
# Royal Kludge G68 Linux Driver

## Overview

This project aims to create a Linux driver for the Royal Kludge G68 mechanical keyboard, specifically designed for users like myself who daily drive Linux (ArchLinux btw). The keyboard lacks official Linux support, prompting the need for a custom driver.

## Features

- **Libusb-based Driver:** Utilizes libusb to communicate with the keyboard in user space.
- **Command-line Interface:** Provides a CLI interface for interacting with the keyboard settings.
- **Supported Operations:**
  - List devices (`-l`)
  - Change animation mode (`-m`)
  - Adjust animation speed (`-s`)
  - Control brightness (`-b`)
  - Modify color settings (`-c`)

## Development Process

1. **Research and Analysis:**
   - Studied USB protocol specifications to understand communication patterns.
   - Setup a Windows VM with USB passthrough to capture data using Wireshark.
   - Reverse-engineered binary data packets sent to the keyboard.

2. **Implementation:**
   - Developed a C++ driver using libusb to send commands to the keyboard.
   - Implemented CLI options to mimic the functionality observed from the captured data.

3. **Learning Experience:**
   - Enhanced knowledge of USB protocol intricacies.
   - Explored libusb for user space USB device interaction.
   - Gained insights into Linux system-level programming and secure C++ development.

