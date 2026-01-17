#pragma once
#include <functional>
#include <vector>
#include <iostream>

template<typename... Args>
class Event
{
private:
    std::vector<std::function<void(Args...)>> calls;
    
public:
    inline void Add(std::function<void(Args...)> f)
    {
        calls.push_back(f);
    }
    
    inline void Invoke(Args... args)
    {
        for (int i = 0; i < calls.size(); ++i)
        {
            try
            {
                calls[i](args...);
            }catch (const std::exception& e)
            {
                std::cout << e.what() << "\n";
            }catch (...)
            {
                std::cout << "Unhandled exception\n";
            }
        }
    }
};