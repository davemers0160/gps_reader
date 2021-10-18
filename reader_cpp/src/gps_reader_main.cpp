#define _CRT_SECURE_NO_WARNINGS

// FTDI Driver Includes
#include "ftd2xx_functions.h"

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
//#include <windows.h> 
#include <conio.h>
#elif defined(__linux__)
#include <termios.h>           // Contains POSIX terminal control definitions
#endif

// C++ Includes
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

// Custom Includes
#include "num2string.h"
#include "get_current_time.h"
#include "file_ops.h"
#include "file_parser.h"
#include "sleep_ms.h"

// Project Includes
#include "nmea_msg_parser.h"


//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx;
	int8_t stop = 0;
    bool status;
    std::string console_input;
	std::string value_str;
	int32_t value = 0;
    bool direct = false;


    // Controller Variables
    uint32_t ftdi_device_count = 0;
    ftdiDeviceDetails driver_details;
    FT_HANDLE gps_handle = NULL;
    uint32_t driver_device_num = 0;
    uint32_t connect_count = 0;
    uint32_t read_timeout = 30000;
    uint32_t write_timeout = 1000;
    std::vector<ftdiDeviceDetails> ftdi_devices;

    // gps varaibles
    std::vector<int32_t> baud_rates = {9600, 19200, 38400, 57600, 115200};
    

#if defined(__linux__)
    struct termios old_term, new_term;
#endif

    try
    {
/*
        ftdi_device_count = get_device_list(ftdi_devices);
        if (ftdi_device_count == 0)
        {
            std::cout << "No ftdi devices found... Exiting!" << std::endl;
            std::cin.ignore();
            return -1;
        }

        for (idx = 0; idx < ftdi_devices.size(); ++idx)
        {
            std::cout << ftdi_devices[idx];
        }

        std::cout << "Select GPS Unit: ";
        std::getline(std::cin, console_input);
        driver_device_num = stoi(console_input);

        std::cout << std::endl << "Connecting to GPS Unit..." << std::endl;
        ftdi_devices[driver_device_num].baud_rate = 9600;
        while ((gps_handle == NULL) && (connect_count < 10))
        {
            gps_handle = open_com_port(ftdi_devices[driver_device_num], read_timeout, write_timeout);
            ++connect_count;
        }

        if (gps_handle == NULL)
        {
            std::cout << "No GPS Unit found... Exiting!" << std::endl;
            std::cin.ignore();
            return -1;
        }
*/
        
        //-----------------------------------------------------------------------------
        // a bunch of nmea message parsing testing
        std::string gga_msg = "$GPGGA,053740.000,,,,,0,00,,,M,0.0,M,,0000*48";
        gga_msg = "$GPGGA,053740.000,2503.6319,N,12136.0099,E,1,08,1.1,63.8,M,15.2,M,,0000*64";

        std::string gsv_msg = "$GPGSV,3,1,12,01,00,000,,02,00,000,,03,00,000,,04,00,000,*7C";
        gsv_msg = "$GPGSV,3,3,12,08,22,214,38,27,08,190,16,19,05,092,33,23,04,127,*7B";

        //auto t1 = stod(""); doesn't work
        //auto t2 = stod(" "); doesn't work


        gga_struct gga = parse_gga_msg(gga_msg);
        
        
        gsv_struct gsv = parse_gsv_msg(gsv_msg);
        
        // flush_port(gps_handle);
        // ctrl.tx = data_packet(DRIVER_CONNECT);

        // //send connection request packet and get response back
        // ctrl.send_packet(gps_handle, ctrl.tx);
        // status = ctrl.receive_packet(ctrl_handle, 6, ctrl.rx);

        // if (status == false)
        // {
            // std::cout << "Error communicating with GPS Unit... Exiting!" << std::endl;
            // std::cin.ignore();
            // return -1;
        // }


    }
    catch (std::exception e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

	close_com_port(gps_handle);

    std::cout << "Program Compete!" << std::endl;
    return 0;

}   // end of main
