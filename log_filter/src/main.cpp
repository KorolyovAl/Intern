#include <fstream>

#include "log_filter.h"

int main() {

    std::ifstream input("../source/input.txt");
    if (!input.is_open()) {
        std::cout << "Input file is not open" << std::endl;
        return 1;
    }

    std::ofstream output("../source/output.txt"); 
    if (!output.is_open()) {
        std::cout << "Output file is not open" << std::endl;
        return 1;
    }

    auto filter = create_filter({
        {"type", "subnet", "value", "10.0.0.0/24"},
        {"type", "range", "value", "10.0.0.1-10.0.0.100"}
    });

    process_stream(input, output, filter);

    return 0;
}