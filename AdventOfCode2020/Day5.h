#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>

std::vector<std::string> getData( std::string_view path )
{
    std::vector<std::string> data;
    std::ifstream file( path.data() );
    std::string line;
    for( std::string line; std::getline( file, line ); ) {
        data.push_back( line );
    }
    return data;
}

int getSeatRow( const std::string& seatString )
{
    int acc = 64;
    int row = 0;
    for( size_t i = 0; i < 7; i++ ) {
        if( seatString[i] == 'B' )
            row += acc;
        acc /= 2;
    }
    return row;
}

int getSeatColumn( const std::string& seatString )
{
    int acc = 4;
    int column = 0;
    for( size_t i = 7; i < 10; i++ ) {
        if( seatString[i] == 'R' )
            column += acc;
        acc /= 2;
    }
    return column;
}

int getSeatId( const std::string& seatString )
{
    return getSeatRow( seatString ) * 8 + getSeatColumn( seatString );
}

std::vector<int> getSeatIds( const std::vector<std::string>& data )
{
    std::vector<int> ids;
    ids.reserve( data.size() );
    std::transform( begin( data ), end( data ), std::back_inserter( ids ),
        [] ( const std::string& seatString ) {return getSeatId( seatString ); }
    );
    return ids;
}

int getHighestSeatId( const std::vector<int>& ids )
{
    return ids.back();
}

int getMissingId( const std::vector<int>& ids )
{
    int minId = ids.front();
    for( size_t i = 0; i < ids.size(); i++ ) {
        if( ids[i] != i + minId )
            return i + minId;
    }
    return -1;
}

int getRow( int id )
{
    return id / 8;
}

int getYourSeatId( const std::vector<int>& ids )
{
    int minId = ids.front();
    int maxId = ids.back();

    if( ids.size() != 1ull + maxId - minId )
        return getMissingId( ids );

    int minRow = getRow( minId );
    int maxRow = getRow( maxId );

    if( minId != minRow * 8 )
        return minRow * 8;
    return maxRow * 8 + 7;
}

void Day5()
{
    auto data = getData( "Day5Input.txt" );
    auto seatIds = getSeatIds( data );
    std::sort( begin( seatIds ), end( seatIds ) );

    std::cout << "Day5:\n";
    std::cout << "Part1: Highest seat id: " << getHighestSeatId( seatIds ) << "\n";
    std::cout << "Part2: Your seat id: " << getYourSeatId( seatIds ) << "\n";
}

