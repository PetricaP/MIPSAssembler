#include "assembler.h"

namespace mips {

Assembler::Assembler(std::string const &file_path)
        : file_path_(file_path) {
    std::ifstream file(file_path_);
    if (file.is_open()) {
        parser_ = std::make_unique<Parser>(file);
        auto const &data = parser_->instructions();
        for (auto const &instructionData : data) {
            instructions_.push_back(InstructionFactory::CreateInstruction(instructionData));
        }
        file.close();
    } else {
        throw FileNotFoundException(file_path);
    }
}

void Assembler::WriteToFile(std::string const &file_path) {
    std::ofstream file(file_path, std::ios::out);
    if(file.is_open()) {
        for(auto const &instruction : instructions_) {
            std::cout.fill('0');
            std::cout.width(8);
            std::cout << std::hex << instruction->GetInstruction();
            std::cout << std::endl;
            std::cout << std::bitset<32>(instruction->GetInstruction());
            std::cout << std::endl;
            file.width(8);
            file.fill('0');
            file << std::right << std::hex << instruction->GetInstruction() << std::endl;
        }
        file.close();
    }
}


}
