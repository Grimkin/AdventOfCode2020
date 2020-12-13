#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>

struct InputData
{
    int startTime = 0;
    std::vector<int> busIds;
};

InputData getData( const std::string& path )
{
    InputData inputData;
    std::ifstream file( path );
    file >> inputData.startTime;
    for( std::string line; std::getline( file, line, ',' ); ) {
        if( line[0] == 'x' )
            continue;
        inputData.busIds.push_back( std::stoi( line ) );
    }

    return inputData;
}

int getTimeDistance( int refTime, int busId )
{
    return busId - ( refTime % busId );
}

int getIdWaitTime( const InputData& inputData )
{
    std::map<int, int> waitMap;

    std::transform( begin( inputData.busIds ), end( inputData.busIds ), std::inserter( waitMap, waitMap.end() ),
        [&inputData] ( int busId ) -> std::pair<int, int> {
            return { getTimeDistance( inputData.startTime, busId ),busId };
        } );

    return waitMap.begin()->first * waitMap.begin()->second;
}

struct Value
{
    int64_t offset;
    int64_t add;
};

Value getValue( const Value& a, const Value& b, int64_t dif )
{
    int64_t resA = a.offset;
    int64_t resB = b.offset;

    int64_t results[2];
    for( size_t i = 0; i < 2; i++ ) {
        while( resA + dif != resB ) {
            if( resA + dif < resB )
                resA += std::max( ( resB - resA ) / a.add * a.add, a.add );
            else
                resB += std::max( ( resA - resB ) / b.add * b.add, b.add );
        }
        results[i] = resA;
        resB += b.add;
        resA += a.add;
    }

    return { results[0], results[1] - results[0] };
}

struct Bus
{
    int64_t busId;
    int64_t offset;
};

std::vector<Bus> getBuses( const std::string& path )
{
    std::vector<Bus> buses;
    std::ifstream file( path );
    std::string line;
    std::getline( file, line );
    int64_t offset = 0;
    for( ; std::getline( file, line, ',' ); offset++ ) {
        if( line[0] == 'x' )
            continue;
        buses.push_back( { std::stoi( line ), offset } );
    }

    return buses;
}

int64_t getAscendingTime( const std::vector<Bus>& buses )
{
    Value result = { buses.front().busId, buses.front().busId };
    for( size_t i = 1; i < buses.size(); i++ ) {
        result = getValue( result, { buses[i].busId, buses[i].busId }, buses[i].offset );
    }

    return result.offset;
}

void Day13()
{
    std::cout << "Day 12:\n";
    std::cout << "Part1: Bus Id x lowest wait time: " << getIdWaitTime( getData( "Day13Input.txt" ) ) << "\n";
    std::cout << "Part1: First ascending order time: " << getAscendingTime( getBuses( "Day13Input.txt" ) ) << "\n";
}

