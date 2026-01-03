#include <bitset>
#include <iostream>
#include <string>

#include "Random.h"

class Test
{    
public:
    inline static void RunAllTests()
    {
        std::cout << "Run All Tests ------\n";
        GetMaskTest();
        WriteSingleBitsTest();
        ReadSingleBitsTest();
        ReadWriteSingleBitsTest();
        ReadWriteBytesTest();
        std::cout << "Finished Tests ------\n";
    }
    
    inline static void GetMaskTest()
    {
        std::vector<int> starts = {0,0,0,2,4};
        std::vector<int> ends = {7,1,3,6,7};
        std::vector<uint8_t> masks = {0b11111111,0b00000011,0b00001111,0b01111100,0b11110000};

        int failed = 0;
        int size = starts.size();
        for (int i=0; i<size; i++)
        {
            uint8_t mask = BitStream::GetMask(starts[i], ends[i]);
            if (mask != masks[i])
            {
                failed++;
                std::cout << std::format("FAILED! start:{} end:{} mask:{} got:{}\n", starts[i], ends[i], masks[i], mask);
            }
        }

        std::cout << std::format("GetMaskTest. Runs:{} Failed:{}\n", size, failed);
    }

    inline static void WriteSingleBitsTest()
    {
        //create stream
        int amount = 100;
        std::vector<uint8_t> data(amount);
        BitStream bs(data);
    
        //crete datas
        std::vector<int> starts(amount);
        std::vector<int> ends(amount);
        std::vector<uint8_t> values(amount);
        for (int i=0; i<amount; i++)
        {
            starts[i] = Random::GetUint(7);
            ends[i] = Random::GetUint(starts[i]+1,8);
            values[i] = BitStream::GetMask(starts[i], ends[i]);
        }
    
        //the test
        for (int i=0; i<amount; i++)
        {
            bs.WriteBits(i, starts[i], ends[i], Random::GetByte());
            bs.WriteBits(i, starts[i], ends[i], values[i]);
        }
    
        //read results
        int failed = 0;
        for (int i=0; i<amount; i++)
        {
            if (bs.data[i] != values[i])
            {
                failed++;
                std::cout << std::format("{}:Failed! Got:{} Expected:{} \n",i, bs.data[i], values[i]);
            }
        }
    
        std::cout << std::format("WriteSingleBit Test ended of {} with failed:{}\n",amount, failed);
    }

    inline static void ReadSingleBitsTest()
    {
        int amount = 100;
        std::vector<int> starts(amount);
        std::vector<int> ends(amount);
        std::vector<uint8_t> values(amount);
        for (int i=0; i<amount; i++)
        {
            starts[i] = Random::GetUint(7);
            ends[i] = Random::GetUint(starts[i]+1,8);
            values[i] = Random::GetByte() & BitStream::GetMask(starts[i], ends[i]);
        }

        //Create random data
        std::vector<uint8_t> data(amount);
        for (int i=0; i<amount; i++)
        {
            data[i] = Random::GetByte();
            data[i] &= ~BitStream::GetMask(starts[i], ends[i]);
            data[i] |= values[i];
        }
        BitStream bs(values);

        //the test
        int failed = 0;
        for (int i=0; i<amount; i++)
        {
            uint8_t read = bs.ReadBits(i, starts[i], ends[i]);
            if (read != values[i])
            {
                failed++;
                std::cout << std::format("{} - Read failed! got:{} expected:{}", i, read, values[i]);
            }
        }

        std::cout << std::format("ReadSingleBitTest ended of {} with failed:{}\n",amount, failed);
    }

    inline static void ReadWriteSingleBitsTest()
    {
        int amount = 100;
        
        //create random data
        std::vector<uint8_t> data(amount);
        for (int d=0; d<amount; d++)
        {
            data[d] = Random::GetByte();
        }
        BitStream bs(data);
    
        //create values
        std::vector<int> starts(amount);
        std::vector<int> ends(amount);
        std::vector<uint8_t> values(amount);
        for (int d=0; d<amount; d++)
        {
            starts[d] = Random::GetUint(7);
            ends[d] = Random::GetUint(starts[d]+1,8);
    
            uint8_t value = Random::GetByte();
            uint8_t mask = BitStream::GetMask(starts[d], ends[d]);
            value &= mask;
            values[d] = value;
        }
    
        //write datas
        for (int d=0; d<amount; d++)
        {
            bs.WriteBits(d, starts[d], ends[d], values[d]);
        }
    
        //read datas
        int failed = 0;
        for (int d=0; d<amount; d++)
        {
            uint8_t readed = bs.ReadBits(d, starts[d], ends[d]);
            if (readed != values[d])
            {
                failed++;
                auto val = std::bitset<8>(values[d]);
                auto read = std::bitset<8>(readed);
                std::cout << d <<" : Read failed! Expected:"<< val <<" Got:"<< read
                <<" Mask:"<< std::bitset<8>(BitStream::GetMask(starts[d], ends[d]))<< "\n";
            }
        }
    
        std::cout << std::format("ReadWriteSingleBit test ended. Runs:{} Failed:{}\n", amount, failed);
    }

    inline static void ReadWriteBytesTest()
    {
        int amount = 100;
        int failed = 0;
        int dataLength = 50;
        int bitDepth = 4;
        int targetLength = (8/bitDepth) * dataLength + 1;
        for (int a=0; a<amount; a++)
        {
            //create source data
            std::vector<uint8_t> data = Random::GetBytes(dataLength);
            
            //create target data
            std::vector<uint8_t> target = Random::GetBytes(targetLength);
            BitStream bs(target);
            
            //write data
            bs.WriteBytes(0, 0, data, bitDepth);
            
            //read data
            int bitCount = dataLength * 8;
            auto readed = bs.ReadBytes(0,0, bitCount, bitDepth);
            
            //match data
            if (readed.size() != data.size())
            {
                std::cout << "ERROR! readed size not match data size!\n";
                failed++;
                continue;
            }

            for (int d=0; d<dataLength; d++)
            {
                if (data[d] != readed[d])
                {
                    std::cout << "ERROR! Read byte not match data byte!\n";
                    failed++;
                    break;
                }
            }
        }

        std::cout << "Read-Write Bytes Test Ended! Runs:"<<amount<<" Failed:"<<failed<<"\n"; 
    }
};



