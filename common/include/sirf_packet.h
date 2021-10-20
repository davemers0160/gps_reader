#ifndef _SIRF_PACKET_H_
#define _SIRF_PACKET_H_

#include <cstdint>

//-----------------------------------------------------------------------------

class sirf_packet
{
public:
    
    enum protocol_mode {

        BINARY = 1,         /**< switch to sirf binary mode */
        NEMA = 2,           /**< switch to NMEA mode */
        ASCII = 3,          /**< switch to ASCII mode */
        RTCM = 4,           /**< switch to RTCM mode */
        USER1 = 5,          /**< switch to User1 mode*/
        SIRFLOC = 6,        /**< switch to SiRFLoc mode */
        STATISTIC = 7       /**< switch to statistic mode */

    };
    
    std::vector<uint8_t> payload;
    
//-----------------------------------------------------------------------------
    sirf_packet()
    {
        length = 0;
        payload.clear();
        checksum = 0;
    }
    
    sirf_packet(std::vector<uint8_t> p_)
    {
        payload = p_;
        length = p_.size();
       
        checksum = calc_checksum();
    }
    
    
//-----------------------------------------------------------------------------
    uint16_t get_checksum(void) { return checksum; }
    
    
//-----------------------------------------------------------------------------
    void parse_sirf_binary(void)
    {
        
        
        
        
        
        
    }
    
    
//-----------------------------------------------------------------------------
    std::vector<uint8_t> to_vector(void)
    {
        std::vector<uint8_t> d;

        // copy the start sequence
        std::copy(start_sequence.begin(), start_sequence.end(), std::back_inserter(d));
        
        // push the length 
        d.push_back((length >> 8) & 0x00FF);
        d.push_back(length & 0x00FF);
        
        // insert the payload
        d.insert(d.end(), payload.begin(), payload.end());

        // insert the checksum
        d.push_back((checksum >> 8) & 0x00FF);
        d.push_back(checksum & 0x00FF);
        
        // insert the end sequence
        d.insert(d.end(), end_sequence.begin(), end_sequence.end());

        return d;
    }
    
    
    
//-----------------------------------------------------------------------------    
private:
    std::vector<uint8_t> start_sequence = {0xA0, 0xA2};
    std::vector<uint8_t> end_sequence  = {0xB0, 0xB3};
    
    uint16_t checksum;
    uint16_t length;
    
    
    //-----------------------------------------------------------------------------
    uint16_t calc_checksum(void)
    {
        uint16_t checksum = 0;

        for (uint16_t idx = 0; idx < length; ++idx)
        {
            checksum = (uint16_t)((checksum + (uint16_t)payload[idx]) & 0x007FFF);
        }

        return checksum;

    }   // end genChecksum
};


#endif  // _SIRF_PACKET_H_
