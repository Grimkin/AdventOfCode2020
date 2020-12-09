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

struct Bag
{
    std::vector<std::pair<int, std::string>> contains;
};

std::string getParentColor( const std::string& string )
{
    static std::regex regex( R"((\w* \w*).*)" );
    std::smatch match;
    if( !std::regex_match( string, match, regex ) )
        throw std::runtime_error( "something is wrong" );

    return match[1];
}

std::vector<std::pair<int, std::string>> getContain( const std::string& string )
{
    static std::regex regex( R"((\d) (\w* \w*) bags?)" );

    std::vector<std::pair<int, std::string>> contains;

    std::smatch match;
    std::sregex_iterator bagsBegin( begin( string ), end( string ), regex );
    std::sregex_iterator bagsEnd;

    for( auto it = bagsBegin; it != bagsEnd; ++it ) {
        auto& match = *it;
        contains.push_back( { std::stoi( match[1] ), match[2] } );
    }
    return contains;
}

std::pair<std::string,Bag> getBag( const std::string& string )
{
    return { getParentColor( string ),{ getContain( string ) } };
}

std::map<std::string, Bag> getBags( std::string_view path )
{
    std::map<std::string,Bag> bags;
    std::ifstream file( path.data() );
    std::string line;
    for( std::string line; std::getline( file, line ); ) {
        bags.insert( getBag( line ) );
    }
    return bags;
}

bool canContain( const std::string& searchColor, const Bag& contains, const std::map<std::string, Bag>& bags )
{
    for( auto& [num,color] : contains.contains ) {
        if( searchColor == color )
            return true;
        if( canContain( searchColor, bags.at( color ), bags ) )
            return true;
    }
    return false;
}

int numCanContains( const std::map<std::string, Bag>& bags, const std::string& color )
{
    return std::accumulate( begin( bags ), end( bags ), 0, [&color,&bags] ( int sum, const auto& bag ) {
        return sum + canContain( color, bag.second, bags );
        } );
}

int getNumBags( const Bag& contains, const std::map<std::string, Bag>& bags )
{
    return std::accumulate( begin( contains.contains ), end( contains.contains ), 1,
        [bags] ( int sum, const auto& bag ) {
            return sum + bag.first * getNumBags( bags.at( bag.second ), bags );
        } );
}

void Day7()
{
    auto bags = getBags( "Day7Input.txt" );

    std::cout << "Day7:\n";
    std::cout << "Part1: Bags that can contain shiny gold: " << numCanContains( bags, "shiny gold" ) << "\n";
    std::cout << "Part2: Bags inside shiny gold: " << getNumBags( bags.at( "shiny gold" ), bags ) - 1 << "\n";
}
