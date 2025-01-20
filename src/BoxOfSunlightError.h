#pragma once

#include <iostream>

class BoxOfSunlightError : public std::runtime_error
{
public:
    BoxOfSunlightError(const std::string& message)
        : runtime_error{message} {};
};