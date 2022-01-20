#include "logger.h"
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string


void Logger::flush()
{
    save();
}


void Logger::save()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);      
    std::ostringstream out2;
    out2 << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ") ;
    out2 << out.str() ;
    auto handle = std::ofstream(fileName, std::ios_base::app);
    handle << out2.str() << "\n";
    out.clear();
}




// class Logger {
//     private:
//         ostringstream oss;
//     public:
//         template <typename T>
//         Logger& operator<<(T a);

//     Logger& operator<<( std::ostream&(*f)(std::ostream&) )
//     {
//         if( f == std::endl )
//         {
//             std::cout << "12-09-2009 11:22:33" << oss.str() << std::endl;   
//             oss.str("");
//         }
//         return *this;
//     }
// };

// template <typename T>
// Logger& Logger::operator<<(T a) {
//     oss << a;
//     return *this;
// }

// void functionTest(void) {
//     Logger log;
//     log << "Error: " << 5 << " seen" << std::endl;
// }

// int main()
// {
//     functionTest();
// }


