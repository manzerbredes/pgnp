#include <unordered_map>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <exception>

namespace pgnp {


    typedef struct HalfMove {

    } HalfMove;


    class PGN {
        private:
        std::unordered_map<std::string,std::string> tags;
        HalfMove moves;
        std::string pgn_content;

        public:
        void FromFile(std::string);
        void FromString(std::string);


        private:

        /// @brief Populate @a tags with by parsing the one starting at location in argument
        int ParseNextTag(int);
        
        /// @brief Get the next non-blank char location starting from location in argument
        int NextNonBlank(int);
    };



    struct UnexpectedEOF : public std::exception
    {
        const char * what () const throw ()
        {
            return "Unexpected end of pgn file";
        }
    };

}
