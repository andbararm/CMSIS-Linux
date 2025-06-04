# CMSIS-Linux
This repository contains an example for heterogeneous multi-core debugging.

# Dual Debug Demo (Linux project and M4 Core project) for MCIMX7SABRE board in MDK6

The projects used here are based on the examples found in the Keil iMX7D_DFP 1.7.8.

Steps to run the projects are the following:

# Board Setup

- Download SD-Card image from: https://www.nxp.com/design/design-center/software/embedded-software/i-mx-software/embedded-linux-for-i-mx-applications-processors:IMXLINUX (choose the i.MX 6UltraLite EVK, i.MX 6ULL EVK, i.MX 6ULZ EVK, i.MX 7Dual SABRE-SD version)
- Write the 'imx-image-full-imx6ul7d.wic' Image to SD-Card (connected to the PC), with Rufus (https://rufus.ie/downloads/ (rufus-4.7p)) or Win32 Disk Imager (https://win32diskimager.org/ ) or Linux dd command
- Build the RPMSG_TTY_CMSIS-RTOS2_M4 project which also produces a bin file and copy this RPMSG_TTY_CMSIS-RTOS2_M4.bin as 'm4.bin' to the fat partition of the SD-Card
- Get the Universal Update Utility (uuu) tool (https://github.com/nxpfrankli/mfgtoolwiki/blob/master/How-to-use-UUU-on-Windows.md) and copy it to the wic image folder
- Set all 10 boot switches to 0 (SW2,SW3)
- Connect the board with 2 USB cables to the PC
- Remove the SD-card from the board's card slot, if it was plugged in
- Power on the board and it will go into the SDP (Serial Download Programming) mode
- Insert the SD-Card again
- Execute the command "uuu -b sd u-boot-imx7dsabresd_sd.imx" to update the SD-Card with the correct u-boot version
- Start VSC and install the Serial Monitor extension by Microsoft, "New Terminal" will trigger a new SERIAL MONITOR tab, configure monitors for both Ports
- The USB interface consists of two virtual COM ports. The first one is for the Cortex-A7, the second one for Cortex-M4
- Power cycle the board and the u-boot will start now and interrupt the boot process with any key. Now it should allow to configure the u-boot environment
- To use the Cortex-M4 and Cortex-A7 at the same time, the default Device Tree Blob (sdb.dtb) must be exchanged with imx7d-sdb-m4.dtb, which is located with other *.dtb files in the boot partition of the SD card image created by the step 2. Execute the u-boot command "setenv fdt_file imx7d-sdb-m4.dtb"
- Change the m4image to use yours: "setenv m4image m4.bin"
- Note down the exact file size of the m4.bin file and execute u-boot command "setenv filesize _your_file_size_"   (in byte)
- Change the loadm4image script to : "setenv loadm4image 'fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${m4image}; cp.b ${loadaddr} 0x7F8000 ${filesize}'"
- Execute "printenv bootcmd" and copy the content/output to the clipboard
- Enable automatic start of M4 core by u-boot by changing the bootcmd script: "setenv bootcmd 'run m4boot; _your_clipboard_content_'"
- save the environment with the u-boot command "saveenv"
- Power cycle the board and now u-boot should start the M4 automatically and followed by the Linux boot
- Connect board to network
- Use in Linux Console the command "IP address" to find out the IP addresses assigned to the board
- In Linux console use "modprobe imx_rpmsg_tty" to load required kernel module (Note: calling "echo imx_rpmsg_tty > /etc/modules-load.d/rpmsg.conf" allows a Linux boot by loading this kernel module automatically)

# M4 Project Setup

- As the project was built already, the debug session can be started

# Linux Project Setup

- Compiler:  select and download the "AArch32 GNU/Linux target with hard float (arm-none-linux-gnueabihf" variant on https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
- In the makefile update the "CC" path to your location of the GNU toolchain
- In the .vscode/tasks.json update the IP address of the board in the 3 locations marked with related comments
- In the .vscode/launch.json update the IP address of the board in 1 location marked with related comments and adjust the "miDebuggerPath" to the GNU toolchain path
- Make sure Microsoft C/C++ Extension is installed in VSC for "cppdbg" debug configuration type
- The project can be built with "Run Task -> Linux App Build"
- Transfer the Linux executable by "Run Task -> Linux prepare executable"
- Start debug session via debug config "(gdb) Launch AB (Linux_Application_TTY)"

# Testing the Projects

- Run the M4 project, this is an endless loop and will not exit
- Run the Linux application, this is one-shot and exits after one run
- Notice the output of the applications in the respective serial monitor windows
- Note, sometimes to communication between the cores does not seem to work. It can help, to run the Linux application several times directly in the Linux console until the M4 and Linux app are "in sync" and correct output is seen.
