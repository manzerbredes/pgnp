
#include "pgnp.hpp"
#include <iostream>

#define IS_BLANK(c) (c==' ' || c=='\n' || c=='\t')
#define IS_EOF(loc) (loc>=pgn_content.size())
#define EOF_CHECK(loc) {if(IS_EOF(loc)) throw UnexpectedEOF();}

namespace pgnp {

  void PGN::FromFile(std::string filepath){
    std::ifstream file(filepath);
    
    std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    FromString(content);
  }


  void PGN::FromString(std::string pgn_content){
    this->pgn_content=pgn_content;
    int loc=0;
    while(!IS_EOF(loc)) {
      char c=pgn_content[loc];
      if(!IS_BLANK(c)){
        switch (c) {
          case '[':
            loc=ParseNextTag(loc);
            break;
        }
      }
      loc++;
    }

    /*for (auto const& [key, val] : tags){
      std::cout << key <<"="<<val<<std::endl;
    }*/
  }

  int PGN::ParseNextTag(int start_loc){
    // Parse key
    std::string key;
    int keyloc=start_loc+1;
    EOF_CHECK(keyloc);
    char c=pgn_content[keyloc];
    while(!IS_BLANK(c)){
      key+=c;
      keyloc++;
      EOF_CHECK(keyloc);
      c=pgn_content[keyloc];
    }

    // Parse value
    std::string value;
    int valueloc=NextNonBlank(keyloc)+1;
    EOF_CHECK(keyloc);
    c=pgn_content[valueloc];
    while(c!='"' or IS_EOF(valueloc)){
      value+=c;
      valueloc++;
      EOF_CHECK(keyloc);
      c=pgn_content[valueloc];
    }

    // Add tag
    tags[key]=value;

 
    return(valueloc+1); // +1 For the last char of the tag which is ']'
  }

  int PGN::NextNonBlank(int loc){
    char c=pgn_content[loc];
    while(IS_BLANK(c)){
      loc++;
      c=pgn_content[loc];
    }
    return(loc);
  }

}