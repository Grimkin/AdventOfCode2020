#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <list>

struct FieldData
{
    std::string name;
    int min1 = 0;
    int max1 = 0;
    int min2 = 0;
    int max2 = 0;
};

bool isInRange( int value, const FieldData& fieldData )
{
    return value >= fieldData.min1 && value <= fieldData.max1 || value >= fieldData.min2 && value <= fieldData.max2;
}

struct TicketData
{
    std::vector<int> values;
};

struct InputData
{
    std::vector<FieldData> fields;
    TicketData myTicket;
    std::vector<TicketData> nearbyTickets;
};

InputData getInputData( const std::string& path )
{
    InputData inputData;
    std::ifstream file( path );
    std::regex fieldRegex( R"((.*): (\d+)-(\d+) or (\d+)-(\d+))" );
    std::string line;
    for( ; std::getline( file, line ); ) {
        std::smatch match;
        if( !std::regex_match( line, match, fieldRegex ) )
            break;
        inputData.fields.push_back( { match[1],std::stoi( match[2] ),std::stoi( match[3] ) ,std::stoi( match[4] ) ,std::stoi( match[5] ) } );
    }
    std::getline( file, line );
    std::getline( file, line );
    std::stringstream myTicketLine( line );

    for( std::string value; std::getline( myTicketLine, value, ',' ); ) {
        inputData.myTicket.values.push_back( std::stoi( value ) );
    }
    std::getline( file, line );
    std::getline( file, line );

    for( std::string value; std::getline( file, line ); ) {
        std::stringstream ticketLine( line );
        inputData.nearbyTickets.push_back( {} );
        for( std::string value; std::getline( ticketLine, value, ',' ); ) {
            inputData.nearbyTickets.back().values.push_back( std::stoi( value ) );
        }
    }

    return inputData;
}

bool isValueValid( int value, const std::vector<FieldData>& fields )
{
    for( auto& field : fields ) {
        if( isInRange( value, field ) )
            return true;
    }
    return false;
}

int getSumInvalidData( const InputData& inputData )
{
    return std::accumulate( begin( inputData.nearbyTickets ), end( inputData.nearbyTickets ), 0ll,
        [&inputData] ( int64_t sum, const TicketData& ticket ) {
            return sum + std::accumulate( begin( ticket.values ), end( ticket.values ), 0ll,
                [&inputData] ( int64_t sum, int value ) {
                    return sum + ( isValueValid( value, inputData.fields ) ? 0 : value );
                } );
        } );
}

bool isTicketValid( const TicketData& ticket, const std::vector<FieldData>& fields )
{
    for( auto& value : ticket.values ) {
        if( !isValueValid( value, fields ) )
            return false;
    }
    return true;
}

void addTicketToValues( const TicketData& ticket, std::map<int, std::set<int>>& values )
{
    for( size_t i = 0; i < ticket.values.size(); i++ )
        values[i].insert( ticket.values[i] );
}

std::map<int, std::set<int>> getSetValues( const InputData& inputData )
{
    std::map<int, std::set<int>> values;

    for( auto& ticket : inputData.nearbyTickets ) {
        if( !isTicketValid( ticket, inputData.fields ) )
            continue;

        addTicketToValues( ticket, values );
    }
    return values;
}

bool valuesValidForField( const std::set<int>& values, const FieldData& field )
{
    for( auto& value : values ) {
        if( !isInRange( value, field ) )
            return false;
    }
    return true;
}

std::vector<std::string> getValidFieldName( const std::set<int>& values, const std::map<std::string, FieldData>& fields )
{
    std::vector<std::string> validFieldNames;
    for( auto& [name, field] : fields ) {
        if( valuesValidForField( values, field ) )
            validFieldNames.push_back( name );
    }
    return validFieldNames;
}

std::vector<std::string> getFieldOrder( const InputData& inputData )
{
    auto values = getSetValues( inputData );
    std::map<std::string, FieldData> availableFields;
    std::transform( begin( inputData.fields ), end( inputData.fields ), std::inserter( availableFields, availableFields.end() ),
        [] ( const auto& field ) -> std::pair<std::string, FieldData> {
            return { field.name,field };
        } );

    std::vector<std::string> fieldNames( values.size() );
    do {
        for( auto& [idx, ticketValues] : values ) {
            auto validFieldNames = getValidFieldName( ticketValues, availableFields );
            if( validFieldNames.size() == 1 ) {
                fieldNames[idx] = validFieldNames.front();
                availableFields.erase( validFieldNames.front() );
            }
        }
    } while( !availableFields.empty() );

    return fieldNames;
}

int64_t getDepartureProduct( const InputData& inputData )
{
    auto fieldOrder = getFieldOrder( inputData );

    int64_t sum = 1;
    for( size_t i = 0; i < fieldOrder.size(); i++ ) 		{
        if( fieldOrder[i].starts_with( "departure" ) )
            sum *= inputData.myTicket.values[i];
    }
    return sum;
}

void Day16()
{
    auto inputData = getInputData( "Day16Input.txt" );

    std::cout << "Day16:\n";
    std::cout << "Part1: Get sum invalid data: " << getSumInvalidData( inputData ) << "\n";
    std::cout << "Part2: Get sum with departure: " << getDepartureProduct( inputData ) << "\n";
}

