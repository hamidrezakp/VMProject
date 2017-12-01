#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <cxxopts.hpp>

//Database file name
#define dbfile "database"
using namespace std;
class vm;
vector<vm> vms;

//class vm for holding vm's data
class vm{
    string name, fileName, OSType, stat;
    int diskSize, memorySize;

 public:
    vm(){}
    vm(string, string, string, string, int, int);
    bool powerOn();
    bool powerOFF();
    bool suspend();

    void setN(string n){name = n;}
    void setFN(string fn){fileName = fn;}
    void setOST(string ost){OSType = ost;}
    void setSTT(string stt){stat = stt;}
    void setDS(int ds){diskSize = ds;}
    void setMS(int ms){memorySize = ms;}

    string getN(){return name;}
    string getFN(){return fileName;}
    string getOST(){return OSType;}
    string getSTT(){return stat;}
    int getDS(){return diskSize;}
    int getMS(){return memorySize;}

};

//class vm construction
vm::vm(string name, string fileName="", string OSType="", string stat="",int diskSize=0, int memorySize=0){
    this->name = name;
    this->fileName = fileName == ""? this->name + ".vm" : fileName;
    this->OSType = OSType == ""? "Ubuntu" : OSType;
    this->stat = stat == ""? "OFF" : stat;
    this->diskSize = diskSize == 0? 200000 : diskSize;
    this->memorySize = memorySize == 0? 90000 : memorySize;
}

//vm powerOn method to powering on vm
bool vm::powerOn(){
    stat = "Running";
    return true;
}

//vm powerOFF method to powering off vm
bool vm::powerOFF(){
    stat = "OFF";
    return true;
}

//vm suspend method to suspend vm
bool vm::suspend(){
    stat = "Suspended";
    return true;
}

//loadsVMs function to read data from database and store it to a vm class array
vector<vm> loadVMs(){
    vector<vm> vms;
    string line;
    ifstream db;
    db.open(dbfile);

    if(db.is_open()){
        while(getline(db,line)){
            string name, fileName, OSType, stat, diskSize, memorySize;
            name = line.substr(0, line.find('|')); line = line.substr(line.find('|')+1);
            fileName = line.substr(0, line.find('|')); line = line.substr(line.find('|')+1);
            OSType = line.substr(0, line.find('|')); line = line.substr(line.find('|')+1);
            stat = line.substr(0, line.find('|')); line = line.substr(line.find('|')+1);
            diskSize = line.substr(0, line.find('|')); line = line.substr(line.find('|')+1);
            memorySize = line.substr(0, line.find('|')); line = line.substr(line.find('|')+1);

            vms.push_back(vm(name, fileName, OSType, stat, atoi(diskSize.c_str()), atoi(memorySize.c_str())));
        }
        db.close();
    }
    return vms;
}

//saveVMs function to save data from a vm class array tp database
bool saveVMs(vector<vm> vms){
    ofstream db;
    db.open(dbfile);

    if(db.is_open()){
        for(auto &vm : vms){
            db << vm.getN() << '|' << vm.getFN() << '|' << vm.getOST() << '|' << vm.getSTT() << '|' << vm.getDS() << '|' << vm.getMS() << endl;
        }
        db.close();
        return true;
    }
    return false;
}

//printVMs function to print vm's list from a vm class array
void printVms(vector<vm> vms){
    cout << "\033[1;32m" << setw(20) << left << "#" << setw(20) << left << "Name" << setw(20) << left << "OS Type" << setw(20) << left << "Disk Size" << setw(20) << left << "Memory Size" << setw(20) << left << "Stat" << "\033[0m\n";
    int i=0;
    for(auto &v : vms){
        cout << setw(20) << left << i << setw(20) << left << v.getN() << setw(20) << left << v.getOST() << setw(20) << left << v.getDS() << setw(20) << left << v.getMS() << setw(20) << left << v.getSTT() << endl;
        i++;
    }
}

//parseArgs function to parsing arguments that given to program
void parseArgs(int argc, char** argv){
        try{
        cxxopts::Options options(argv[0], "A Simple Virtual Machine manager.");
        options.add_options()
        ("l,list", "prints VMs list")
        ("c,create-vm", "creates a VM")
        ("e,edit-vm", "edits a VM with # ID", cxxopts::value<int>(), "#")
        ("r,remove-vm", "removes a VM with # ID", cxxopts::value<int>(), "#")
        ("h,help", "Print this help")

        ("n,name", "name of VM ", cxxopts::value<string>(), "NAME")
        ("f,filename", "VM file address", cxxopts::value<string>(), "FILENAME")
        ("o,os-type", "VM OS type", cxxopts::value<string>(), "TYPE")
        ("s,disk-size", "Size of VM disk", cxxopts::value<int>(), "SIZE")
        ("m,mem-size", "VM RAM memory size", cxxopts::value<int>(), "SIZE");

        options.add_options("Power")
        ("power-on", "Power on VM with # ID", cxxopts::value<int>(), "#")
        ("power-off", "Power off VM with # ID", cxxopts::value<int>(), "#")
        ("suspend", "Suspend VM with # ID", cxxopts::value<int>(), "#");


        auto result = options.parse(argc, argv);

        if(result.count("help")){
            cout << options.help({"", "Power"}) << endl;
            return;
        }

        if(result.count("remove-vm")){
            int id = result["remove-vm"].as<int>();
            vms.erase(vms.begin() + id);
        }

        if(result.count("create-vm")){
            if(!result.count("name")){
                cout << "Error : Option \"-n\" for creating VM is require!\n";
                return;
            }

            string name, fileName, OSType;
            int diskSize, memorySize;
    //
            name = result["name"].as<string>();
            fileName = result.count("filename")? result["filename"].as<string>() : "";
            OSType = result.count("os-type")? result["os-type"].as<string>() : "";
            diskSize = result.count("disk-size")? result["disk-size"].as<int>() : 0;
            memorySize = result.count("mem-size")? result["mem-size"].as<int>() : 0;

            vms.push_back(vm(name, fileName, OSType, "", diskSize, memorySize));
        }

        if(result.count("edit-vm")){
            int id = result["edit-vm"].as<int>();

            if(vms[id].getSTT() != "OFF"){
                cout << "For editing, VM must be OFF!\n"
                        "First turn off with \'VM --power-off " << id << "\' and then try again.\n";
                        return;
            }

            if(result.count("name")) vms[id].setN(result["name"].as<string>());
            if(result.count("filename")) vms[id].setFN(result["filename"].as<string>());
            if(result.count("os-type")) vms[id].setOST(result["os-type"].as<string>());
            if(result.count("disk-size")) vms[id].setDS(result["disk-size"].as<int>());
            if(result.count("mem-size")) vms[id].setMS(result["mem-size"].as<int>());
        }

        if(result.count("power-on")){
            int id = result["power-on"].as<int>();
            vms[id].setSTT("Running");
        }

        if(result.count("power-off")){
            int id = result["power-off"].as<int>();
            vms[id].setSTT("OFF");
        }

        if(result.count("suspend")){
            int id = result["suspend"].as<int>();
            vms[id].setSTT("Suspended");
        }

        if(result.count("list")){
                printVms(vms);
        }
    }
    catch (const cxxopts::OptionException& e){
        cout << "Error parsing options : " << e.what() << endl;
        exit(1);
    }
}

//program main function
int main(int argc, char** argv)
{
    vms = loadVMs();
    parseArgs(argc, argv);
    saveVMs(vms);
    return 0;
}
