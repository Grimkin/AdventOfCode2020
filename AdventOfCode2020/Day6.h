#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <set>
#include <map>

std::vector<std::vector<std::string>> getData( std::string_view path )
{
    std::vector<std::vector<std::string>> data;
    std::ifstream file( path.data() );
    std::string line;
    data.push_back( {} );
    for( std::string line; std::getline( file, line ); ) {
        if( line.empty() )
            data.push_back( {} );
        else
            data.back().push_back( line );
    }
    return data;
}

int getNumGroupAnswer( const std::vector<std::string>& groupData )
{
    std::set<char> answers;
    for( auto& member : groupData ) {
        for( auto& answer : member ) {
            answers.insert( answer );
        }
    }
    return answers.size();
}

int getSumAnswers( const std::vector<std::vector<std::string>>& data )
{
    return std::accumulate( begin( data ), end( data ), 0,
        [] ( int sum, const auto& groupData ) {
            return sum + getNumGroupAnswer( groupData );
        } );
}
int getNumGroupAnswer2( const std::vector<std::string>& groupData )
{
    std::map<char,int> answers;
    for( auto& member : groupData ) {
        for( auto& answer : member ) {
            answers[ answer ]++;
        }
    }
    int numGroupMembers = groupData.size();
    return std::accumulate( begin( answers ), end( answers ), 0,
        [numGroupMembers] ( int sum, const auto& answerPair ) {
            return sum + ( answerPair.second == numGroupMembers );
        } );
}

int getSumAnswers2( const std::vector<std::vector<std::string>>& data )
{
    return std::accumulate( begin( data ), end( data ), 0,
        [] ( int sum, const auto& groupData ) {
            return sum + getNumGroupAnswer2( groupData );
        } );
}

void Day6()
{
    auto data = getData( "Day6Input.txt" );

    std::cout << "Day6:\n";
    std::cout << "Part1: Sum of answers: " << getSumAnswers( data ) << "\n";
    std::cout << "Part2: Sum of answers: " << getSumAnswers2( data ) << "\n";
}
