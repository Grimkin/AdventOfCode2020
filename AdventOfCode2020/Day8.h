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

enum class CommandType
{
    nop,
    acc,
    jmp
};

struct Command
{
    CommandType type;
    int value;
};

struct Program
{
    std::vector<Command> commands;
};

CommandType getCommandType( const std::string& command )
{
    if( command == "nop" )
        return CommandType::nop;
    if( command == "acc" )
        return CommandType::acc;
    if( command == "jmp" )
        return CommandType::jmp;
    throw std::runtime_error( "invalid command type" );
}

Program parseProgram( const std::string& path )
{
    Program program;
    std::ifstream file( path );

    std::regex regex( R"((\w*) ([+-]?\d+))" );
    for( std::string line; std::getline( file, line ); ) {
        std::smatch match;
        std::regex_match( line, match, regex );
        program.commands.push_back( { getCommandType( match[1] ), std::stoi( match[2] ) } );
    }

    return program;
}

int executeCommand( const Command& command, int& acc )
{
    switch( command.type ) {
    case CommandType::nop:
        return 1;
    case CommandType::acc:
        acc += command.value;
        return 1;
    case CommandType::jmp:
        return command.value;
    }
    throw std::runtime_error( "invalid command" );
}

int getMaxAcc( const Program& program )
{
    int acc = 0;
    size_t idx = 0;
    std::set<size_t> executedCommands;
    while( true ) {
        if( executedCommands.count( idx ) != 0 )
            return acc;

        executedCommands.insert( idx );
        idx += executeCommand( program.commands[idx], acc );
    }
}

std::optional<int> getFinalResult( const Program& program )
{
    int acc = 0;
    size_t idx = 0;
    std::set<size_t> executedCommands;
    while( idx < program.commands.size() ) {
        if( executedCommands.count( idx ) != 0 )
            return {};

        executedCommands.insert( idx );
        idx += executeCommand( program.commands[idx], acc );
    }
    return acc;
}

Program getNextProgramIt( const Program& program, size_t& idx )
{
    while( program.commands[idx].type != CommandType::jmp && program.commands[idx].type != CommandType::nop ) {
        ++idx;
    }
    Program newProgram = program;
    auto& commandType = newProgram.commands[idx].type;
    if( commandType == CommandType::nop )
        commandType = CommandType::jmp;
    if( commandType == CommandType::jmp )
        commandType = CommandType::nop;
    return newProgram;
}

int getCorrectedResult( const Program& program )
{
    size_t idx = 0;
    while( idx < program.commands.size() ) {
        auto programIt = getNextProgramIt( program, idx );
        auto result = getFinalResult( programIt );
        if( result )
            return *result;
        ++idx;
    }
    return -std::numeric_limits<int>::infinity();
}

void Day8()
{
    auto program = parseProgram( "Day8Input.txt" );

    std::cout << "Day8:\n";
    std::cout << "Part 1: Acc after loop:" << getMaxAcc( program ) << "\n";
    std::cout << "Part 2: Final Acc result:" << getCorrectedResult( program ) << "\n";
}
