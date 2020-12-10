#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>
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

int getChargerDif( std::vector<size_t> data )
{
    std::sort( data.begin(), data.end() );
    std::vector<size_t> diffs( 4 );
    size_t oldInput = 0;
    for( auto& charger : data ) {
        diffs[charger - oldInput]++;
        oldInput = charger;
    }
    diffs[3]++;
    return diffs[1] * diffs[3];
}

size_t getNumPossibilitesRec( std::span<size_t> data, size_t idx, std::map<size_t, size_t>& checked )
{
    if( data.size() <= 2 )
        return 1;

    if( auto it = checked.find( idx ); it != checked.end() )
        return it->second;

    size_t possibilities = getNumPossibilitesRec( data.subspan( 1 ), idx + 1, checked );
    if( data[2] - data[0] <= 3 )
        possibilities += getNumPossibilitesRec( data.subspan( 2 ), idx + 2, checked );
    if( data.size() > 3 && data[3] - data[0] <= 3 )
        possibilities += getNumPossibilitesRec( data.subspan( 3 ), idx + 3, checked );

    checked[idx] = possibilities;

    return possibilities;
}

size_t getNumPossibilites( std::vector<size_t> data )
{
    data.push_back( 0 );
    std::sort( data.begin(), data.end() );
    data.push_back( data.back() + 3 );

    std::map<size_t, size_t> checked;

    return getNumPossibilitesRec( data, 0, checked );
}


void Day10()
{
    auto data = getData( "Day10Input.txt" );
    

    std::cout << "Day10:\n";
    std::cout << "Part1: Jolt dif sum: " << getChargerDif( data ) << "\n";
    std::cout << "Part2: Adapter Combinations: " << getNumPossibilites( data ) << "\n";
}
