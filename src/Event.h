#pragma once
#include <functional>
#include <vector>
#include <iostream>

template<typename... Args>
class Event
{
private:
    std::vector<void(*)(Args...)> calls;
    
public:
    inline void Add(void(*f)(Args...))
    {
        calls.push_back(f);
    }
    
    inline void Remove(void(*f)(Args...))
    {
        calls.erase(std::remove(calls.begin(), calls.end(), f),calls.end());
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