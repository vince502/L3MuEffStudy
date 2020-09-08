#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
void compareEN(){
  string freco = "L3C.txt";
  string fraw  = "log.txt";
  std::ifstream inreco(freco.c_str());
  std::ifstream inraw(fraw.c_str());
  std::ofstream out1;

  out1.open("Matching_ENum.txt");
  std::string en_re, en_ra;
  int nre, nra;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;
  while(std::getline(inreco, en_re)){
    istringstream tre(en_re);
    tre >> nre;
    while(std::getline(inraw, en_ra)){
      count3++;
      istringstream tra(en_ra);
      tra >> nra;
      if (nre == nra){out1 << nra << std::endl; count1++;}
    }
    count2++;
    inraw.clear();
    inraw.seekg(0);
  }
  out1 << count1 << endl;
  out1.close();
}
