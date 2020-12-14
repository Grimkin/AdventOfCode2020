#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>
#include <bitset>
#include <variant>
#include <algorithm>

using IntType = std::bitset<36>;

struct Mask
{
    IntType enable;
    IntType value;
};

struct Memory
{
    IntType value;
    size_t position;
};

using Operation = std::variant<Mask, Memory>;

Mask getMask( const std::string& maskString )
{
    Mask mask;
    size_t idx = 0;
    for( auto it = maskString.rbegin(); it < maskString.rend(); ++it, ++idx ) {
        if( *it != 'X' ) {
            mask.enable[idx] = true;
            if( *it == '1' )
                mask.value[idx] = true;
        }
    }
    return mask;
}

Memory getMemory( const std::string& posString, const std::string& valueString )
{
    return { std::stoull( valueString ) ,std::stoull( posString ) };
}

Operation getOperation( const std::string& string )
{
    static std::regex maskRegex( R"(mask = ([X|0|1]{36}))" );
    static std::regex memRegex( R"(mem\[(\d+)\] = (\d+))" );

    std::smatch match;
    if( std::regex_match( string, match, maskRegex ) )
        return getMask( match[1] );
    std::regex_match( string, match, memRegex );
    return getMemory( match[1], match[2] );
}

std::vector<Operation> getOperations( const std::string& path )
{
    std::vector<Operation> operations;
    std::ifstream file( path );
    for( std::string line; std::getline( file, line );) {
        operations.push_back( getOperation( line ) );
    }

    return operations;
}

std::bitset<36> applyMask( const IntType& value, const Mask& mask )
{
    return value & ~mask.enable | mask.value;
}

size_t getSumOfResults( const std::vector<Operation>& operations )
{
    std::map<size_t, IntType> memory;
    Mask mask;
    for( auto& operation : operations ) {
        if( auto newMaskValue = std::get_if<Mask>( &operation ) )
            mask = *newMaskValue;
        else {
            auto memOp = std::get<Memory>( operation );
            memory[memOp.position] = applyMask( memOp.value, mask );
        }
    }

    return std::accumulate( begin( memory ), end( memory ), 0ull,
        [] ( size_t sum, const auto& memPair ) {
            return sum + memPair.second.to_ullong();
        } );
}

std::vector<size_t> getSetPositions( const IntType& value )
{
    std::vector<size_t> setPositions;

    for( size_t i = 0; i < value.size(); i++ ) {
        if( !value[i] )
            setPositions.push_back( i );
    }

    return setPositions;
}

size_t getAdress( const IntType& initialAdress, const std::vector<size_t>& positions, const IntType& setMask )
{
    IntType adress = initialAdress;
    for( size_t i = 0; i < positions.size(); i++ ) {
        adress[positions[i]] = setMask[i];
    }
    return adress.to_ullong();
}

std::vector<size_t> getAdresses( const IntType& pos, const Mask& mask )
{
    std::vector<size_t> adresses;
    auto tempAdress = pos | mask.value;
    if( !( ~mask.enable ).any() )
        return { tempAdress.to_ullong() };

    auto setPositions = getSetPositions( mask.enable );
    size_t maxVal = 1ull << setPositions.size();
    for( size_t i = 0; i < maxVal; i++ ) {
        adresses.push_back( getAdress( tempAdress, setPositions, i ) );
    }

    return adresses;
}

size_t getSumOfResults2( const std::vector<Operation>& operations )
{
    std::map<size_t, IntType> memory;
    Mask mask;
    for( auto& operation : operations ) {
        if( auto newMaskValue = std::get_if<Mask>( &operation ) )
            mask = *newMaskValue;
        else {
            auto memOp = std::get<Memory>( operation );
            auto adresses = getAdresses( memOp.position, mask );
            for( auto& adress : adresses ) {
                memory[adress] = memOp.value;
            }
        }
    }

    return std::accumulate( begin( memory ), end( memory ), 0ull,
        [] ( size_t sum, const auto& memPair ) {
            return sum + memPair.second.to_ullong();
        } );
}

void Day14()
{
    auto operations = getOperations( "Day14Input.txt" );

    std::cout << "Day14:\n";
    std::cout << "Part1: Sum of results: " << getSumOfResults( operations ) << "\n";
    std::cout << "Part2: Sum of results: " << getSumOfResults2( operations ) << "\n";
}
