#include <fstream>
#include <iostream>
#include <string>

const char* read_input(std::string path) {
    // File path
    std::string filename = path;

    // Create an input file stream
    std::ifstream in(filename);

    // Check if the file was opened successfully
    if (in.is_open()) {
        // File opened successfully
        std::cout << "File opened successfully." << std::endl;

        // Create a char array for the code with a maximum length of 1024 characters
        static char code[1024]; // Declare as static to ensure it's not destroyed when the function exits

        // Read the file into the code array
        in.read(code, 1024);

        // Close the file
        in.close();

        return code; // Return a pointer to the code array
    }
    else {
        // Failed to open the file
        std::cerr << "Failed to open the file: " << filename << std::endl;
    }

    return nullptr; // Return nullptr in case of failure
}

