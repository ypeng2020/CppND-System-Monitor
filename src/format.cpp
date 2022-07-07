#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int H, M, S;
    H = seconds / 3600;
    M = (seconds % 3600) / 60;
    S = (seconds % 3600) % 60;
    // convert int to string with HH:MM:SS format
    string HH = H < 10 ? '0' + std::to_string(H) : std::to_string(H);
    string MM = M < 10 ? '0' + std::to_string(M) : std::to_string(M);
    string SS = S < 10 ? '0' + std::to_string(S) : std::to_string(S);
    return HH + ':' + MM + ':' + SS; 
}