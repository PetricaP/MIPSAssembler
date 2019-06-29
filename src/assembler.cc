#include "algorithms.h"
#include "assembler.h"

namespace mips {

Assembler::Assembler(std::string const &file_path)
        : file_path_(file_path) {
    std::ifstream file(file_path_);
    if (file.is_open()) {
        parser_ = std::make_unique<Parser>(file);
        auto const &data = parser_->instructions();
        std::transform(std::begin(data), std::end(data), std::back_inserter(instructions_),
                       [](auto &&instructionData) {
            return InstructionFactory::CreateInstruction(instructionData);
        });
        file.close();
    } else {
        throw FileNotFoundException(file_path);
    }
}

void Assembler::WriteToFile(std::string const &file_path) {
    std::ofstream file(file_path, std::ios::out);
    if (file.is_open()) {
        for (auto const &instruction : instructions_) {
            file.width(8);
            file.fill('0');
            file << std::right << std::hex << instruction->GetRepresentation() << std::endl;
        }
        file.close();
    }
}

FileNotFoundException::FileNotFoundException(const std::string &file_path) {
    message_ = "File " + file_path + " was not found.";
}

}
