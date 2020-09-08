#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

void ExtractPF(){

string fname = "Exported_L3MuEffTest_v5.py";
std::ifstream infile(fname.c_str());
std::ofstream out1;
std::ofstream out2;

out1.open("List_EDFilter.txt");
out2.open("List_EDProducer.txt");

std::string line; 
out1 << "Module name, EDFilter name" << endl;
out2 << "Module name, EDProducer name" << endl;

int count =0;
int count2 =0;
while(std::getline(infile, line)){ 
  if(line.size()>15 && line.find("cms.EDFilter") != std::string::npos){
    std::string tline(line);
    std::size_t pos = tline.find("( \"");
    std::size_t bpos = tline.find("\",");
    std::string pname = tline.substr(pos+3,pos+3-bpos);
    pname.pop_back();
    pname.pop_back();

    std::size_t pos2 = tline.find("process.");
    std::size_t bpos2 = tline.find(" = cms.ED");
    std::string mname = tline.substr(pos2+8,bpos2-pos2-8);
    out1 << mname <<", "<<pname <<endl;
    count ++;
  }
  
  else if(line.size()>20 && line.find("cms.EDProducer") != std::string::npos){
    std::string tline(line);
    std::size_t pos = tline.find("( \"");
    std::size_t bpos = tline.find("\",");
    std::string pname = tline.substr(pos+3,pos+3-bpos);
    pname.pop_back();
    pname.pop_back();

    std::size_t pos2 = tline.find("process.");
    std::size_t bpos2 = tline.find("cms.ED");
    std::string mname = tline.substr(pos2+8,bpos2-pos2-11);
    out2 << mname <<", "<<pname <<endl;
    count2 ++;
  }
}

out1.close();
out2.close();
std::cout <<count<<", "<< count2  <<" DONE! " << std::endl;
}


