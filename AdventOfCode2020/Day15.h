#pragma once
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>

std::vector<int64_t> getStartNumbers( const std::string& path )
{
    std::vector<int64_t> numbers;
    std::ifstream file( path );
    int64_t idx = 0;
    for( std::string line; std::getline( file, line, ',' ); ++idx ) {
        numbers.push_back( std::stoll( line ) );
    }
    return numbers;
}

int64_t getNumberSpoken( const std::vector<int64_t>& startNumbers, int64_t numToSpeak )
{
    std::map<int64_t, int64_t> numbers;
    for( size_t i = 0; i < startNumbers.size(); i++ ) {
        numbers[startNumbers[i]] = i;
    }

    int64_t newNum = 0;
    for( int64_t idx = startNumbers.size(); idx < numToSpeak - 1; ++idx ) {
        auto it = numbers.find( newNum );
        if( it != numbers.end() ) {
            newNum = idx - it->second;
            it->second = idx;
        }
        else {
            numbers.insert( { newNum,idx } );
            newNum = 0;
        }
    }
    return newNum;
}

void Day15()
{
    auto numbers = getStartNumbers( "Day15Input.txt" );

    std::cout << "Day15:\n";
    std::cout << "Part1: 2020th number spoken: " << getNumberSpoken( numbers, 2020 ) << "\n";
    std::cout << "Part1: 30'000'000th number spoken: " << getNumberSpoken( numbers, 30000000 ) << "\n";
}
