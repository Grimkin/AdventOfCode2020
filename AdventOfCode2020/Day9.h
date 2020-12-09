#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <set>
#include <map>
#include <deque>
#include <span>

std::vector<size_t> getData( const std::string& path )
{
    std::vector<size_t> data;
    std::ifstream file( path );
    while( !file.eof() ) {
        data.push_back( 0 );
        file >> data.back();
    }
    return data;
}

bool isSumOfPrevious( size_t targetSum, const std::deque<std::pair<size_t, std::set<size_t>>>& previousSums )
{
    for( auto& [value, sums] : previousSums ) {
        if( sums.count( targetSum ) != 0 )
            return true;
    }
    return false;
}

void updatePreviousSums( size_t newValue, std::deque<std::pair<size_t, std::set<size_t>>>& previousSums, size_t maxNumPrevious )
{
    while( previousSums.size() >= maxNumPrevious )
        previousSums.pop_front();

    for( auto& [value, sums] : previousSums ) {
        sums.insert( value + newValue );
    }

    previousSums.push_back( { newValue, {} } );
}

int firstNotSumOfPrevious( const std::vector<size_t>& data, size_t numPrevious )
{
    std::deque<std::pair<size_t, std::set<size_t>>> previousSums;
    for( size_t i = 0; i < numPrevious; i++ ) {
        updatePreviousSums( data[i], previousSums, numPrevious );
    }

    for( size_t i = numPrevious; i < data.size(); i++ ) {
        if( !isSumOfPrevious( data[i], previousSums ) )
            return data[i];
        updatePreviousSums( data[i], previousSums, numPrevious );
    }
    return std::numeric_limits<int>::min();
}

std::optional<size_t> sumOfContigiousLimit( std::span<size_t> data, size_t value )
{
    if( data.empty() )
        return {};
    size_t sum = 0;
    size_t idx = 0;
    while( sum < value ) {
        sum += data[idx++];
        if( sum == value )
            return data[0] + data[idx - 1];
    }
    return sumOfContigiousLimit( data.subspan( 1 ), value );
}

void Day9()
{
    auto data = getData( "Day9Input.txt" );

    auto keyValue = firstNotSumOfPrevious( data, 25 );

    std::cout << "Day9:\n";
    std::cout << "Part 1: Get First not Sum of Previous: " << keyValue << "\n";
    std::cout << "Part 2: Get Weakness Sum: " << sumOfContigiousLimit( data, keyValue ).value_or( 0 ) << "\n";
}
