#ifndef _NMEA_MSG_PARSER_H_
#define _NMEA_MSG_PARSER_H_

#include <cstdint>
#include <string>
#include <sstream>

#include "file_parser.h"

//-----------------------------------------------------------------------------
typedef struct gga_struct
{
    double utc_time;
    double latitude;
    double longitude;
    uint8_t fix;
    uint8_t satellites;
    double hdop;
    double altitude;
    double geoid_sep;
    uint32_t diff_age;
    uint16_t diff_station;
    
    bool valid_checksum;

} gga_struct;

//-----------------------------------------------------------------------------
typedef struct gsv_struct
{
    uint8_t total_msgs;
    uint8_t msg_num;
    uint8_t satellites;
    
    std::vector<uint8_t> id;
    std::vector<uint8_t> el;
    std::vector<uint16_t> az;
    std::vector<uint8_t> snr;
    
    bool valid_checksum;

} gsv_struct;



//-----------------------------------------------------------------------------
uint8_t calc_nmea_checksum(std::string data)
{
    uint16_t idx;
    uint8_t checksum = 0;
    std::string msg_data;
    
    // make sure that '$' is in the first spot
    if(data.length() <=0 || data[0] != '$')
        return checksum;
    
    // find the location of the '*' and chop up the string to only include the data
    auto checksum_location = data.find('*');
    if (checksum_location == std::string::npos)
        return checksum;

    msg_data = data.substr(1, checksum_location-1);
       
    for(idx=0; idx<msg_data.length(); ++idx)
    {
        checksum ^= (uint8_t)(msg_data[idx]);
    }
      
    return checksum;
}

//-----------------------------------------------------------------------------
uint8_t get_nmea_checksum(std::string data)
{
    uint8_t checksum = 0;
    
    // find the start of the checksum
    auto checksum_location = data.find('*');
    if (checksum_location == std::string::npos)
        return checksum;
    
    std::string msg_data = data.substr(checksum_location, data.length()-1);
    
    // convert hex string checksum into a number
    checksum = (uint8_t)std::stoi(msg_data, 0 , 16);
    
    return checksum;
}


std::string parse_nmea_param(std::string &data)
{
    std::string data_out;

    // find the first comma
    auto comma_pos = data.find(',');
    //auto comma2 = data.find(',', comma1 + 1);

    // check the difference between the two
    if (comma_pos < 1)
    {
        // this means that the string is empty :-(
        data_out = "";
        data = data.substr(1, data.length());
    }
    else if (comma_pos == std::string::npos)
    {
        data_out = (data[0] == ' ') ? "" : data.substr(0, data.length());
        data = "";
    }
    else if (comma_pos == data.length()-1)
    {
        data_out = data.substr(0, comma_pos);
        data = " ";
    }
    else
    {
        // parse the string
        data_out = data.substr(0, comma_pos);
        data = data.substr(comma_pos+1, data.length());
    }

    return data_out;
}

//-----------------------------------------------------------------------------
gga_struct parse_gga_msg(std::string data)
{
    gga_struct gga;
    std::string msg_data, checksum_str, tmp_str;
    std::vector<std::string> params;
    
    // make sure that the message is the correct one
    auto msg_id = data.find("$GPGGA,");
    if (msg_id == std::string::npos)
        return gga;
    
    // find the start of the checksum
    auto checksum_location = data.find('*');
    if (checksum_location == std::string::npos)
        return gga;
    
    // cut the message between the message name and the checksum
    msg_data = data.substr(7, checksum_location-7);
    checksum_str = data.substr(checksum_location+1, data.length());
    
    // pull out all of the data fields between the msg name and the checksum
    while (msg_data.length() > 0)
    {
        tmp_str = parse_nmea_param(msg_data);
        params.push_back(tmp_str);
    }

    try
    {
        // parse UTC time
        gga.utc_time = params[0].empty() ? 0.0 : (stod(params[0].substr(0, 2)) * 3600 + stod(params[0].substr(2, 2)) * 60 + stod(params[0].substr(4, 6)));

        // parse the latitude
        if (params[1].empty() || params[2].empty())
        {
            gga.latitude = 0;
        }
        else
        {
            //"2503.6319,N"
            gga.latitude = stod(params[1].substr(0, 2)) + stod(params[1].substr(2, params[1].length())) / 60.0;
            if (params[2] == "S")
                gga.latitude *= -1;
        }

        // parse the longitude
        if (params[3].empty() || params[4].empty())
        {
            gga.longitude = 0;
        }
        else
        {
            gga.longitude = stod(params[3].substr(0, 3)) + stod(params[3].substr(3, params[3].length())) / 60.0;
            if (params[4] == "E")
                gga.longitude *= -1;
        }

        // parse the fix
        gga.fix = params[5].empty() ? 0.0 : (uint8_t)stoi(params[5]);

        // parse the satellites
        gga.satellites = params[6].empty() ? 0.0 : (uint8_t)stoi(params[6]);

        // parse the hdop
        gga.hdop = params[7].empty() ? 0.0 : stod(params[7]);

        // parse the altitude
        gga.altitude = params[8].empty() ? 0.0 : stod(params[8]);

        // parse the geoid_sep
        gga.geoid_sep = params[10].empty() ? 0.0 : stod(params[10]);

        // parse the diff_age
        gga.diff_age = params[12].empty() ? 0.0 : (uint8_t)stoi(params[12]);

        // parse the diff_station
        gga.diff_station = params[13].empty() ? 0.0 : (uint16_t)stoi(params[13]);
    }
    catch (std::exception e)
    {
        std::cout << "Error processing $GPGGA message: " << e.what() << std::endl;
    }

    // calculate the checksum and validate
    gga.valid_checksum = (calc_nmea_checksum(data) == (uint8_t)std::stoi(checksum_str, 0 , 16));
    
    return gga;
}   // end of parse_gga_msg

//-----------------------------------------------------------------------------
gsv_struct parse_gsv_msg(std::string data)
{
    uint8_t idx = 0;
    gsv_struct gsv = { 0 };
    std::string msg_data, checksum_str, tmp_str;
    std::vector<std::string> params;

    // make sure that the message is the correct one
    auto msg_id = data.find("$GPGSV,");
    if (msg_id == std::string::npos)
        return gsv;

    // find the start of the checksum
    auto checksum_location = data.find('*');
    if (checksum_location == std::string::npos)
        return gsv;

    // cut the message between the message name and the checksum
    msg_data = data.substr(7, checksum_location - 7);
    checksum_str = data.substr(checksum_location + 1, data.length());

    // pull out all of the data fields between the msg name and the checksum
    while (msg_data.length() > 0)
    {
        tmp_str = parse_nmea_param(msg_data);
        params.push_back(tmp_str);
        idx = params.size();
    }

    try
    {
        // parse the total_msgs
        gsv.total_msgs = params[0].empty() ? 0.0 : (uint8_t)stoi(params[0]);

        // parse the msg_num
        gsv.msg_num = params[1].empty() ? 0.0 : (uint8_t)stoi(params[1]);

        // parse the satellites
        gsv.satellites = params[2].empty() ? 0.0 : (uint8_t)stoi(params[2]);

        for (idx = 3; idx < params.size(); idx+=4)
        {
            gsv.id.push_back(params[idx].empty() ? 0.0 : (uint8_t)stoi(params[idx]));
            gsv.el.push_back(params[idx+1].empty() ? 0.0 : (uint8_t)stoi(params[idx+1]));
            gsv.az.push_back(params[idx+2].empty() ? 0.0 : (uint16_t)stoi(params[idx+2]));
            gsv.snr.push_back(params[idx+3].empty() ? 0.0 : (uint8_t)stoi(params[idx+3]));
        }

    }
    catch (std::exception e)
    {
        std::cout << "Error processing $GPGSV message: " << e.what() << std::endl;
    }

    // calculate the checksum and validate
    gsv.valid_checksum = (calc_nmea_checksum(data) == (uint8_t)std::stoi(checksum_str, 0, 16));

    return gsv;
}   // end of parse_gsv_msg


#endif  // _NMEA_MSG_PARSER_H_
