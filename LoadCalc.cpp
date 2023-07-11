#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#ifdef _WIN32     
    #include <filesystem>  
    namespace fs = std::filesystem;   
#else
    #include <experimental/filesystem> //this for debian //add -lstdc++fs for libs
    namespace fs = std::experimental::filesystem;
#endif

/*Compile instructions
 * windows: g++ -Wall -o calc_loads.exe -std=c++17 LoadCalc.cpp
 * linux  : g++ -Wall -o calc_loads -std=c++17 LoadCalc.cpp -lstdc++fs  
 */



using namespace std;

float Import_Data_Line(string &_line);
void Help_Function();


int main(int argc, char** argv) {    
    
//variables section
    float               pi = 3.1415926535897932384626433;
    bool                radial = 1;         //if radial total force is per radian, linear is per meter
    map<string,string>  main_config_map;    //Initial basic map used to import the argv into
    size_t              location;           //for parsing strings
    string              directory;          // = main_config_map["Dir"]
    string              output_file_name;   // = main_config_map["File"]
    vector<string>      dat_names_vector;   
    ifstream            dat_file;           //file pointer to the data file that is opened
    string              dat_file_line;      //line read from data file as iterates through

    struct data_point{
        //values read in from data file
        float posx,posy,posz,nx,ny,nz;
        float area,cellwidth,outsign,pressure,rho,T;
        float velx,vely,velz,mu,a,reynolds,yplus;
        float taux,tauy,tauz,qtotal,qcond,qdif;
        //values calculated        
        float force_total,force_x,force_y,force_z;

    };
    vector<data_point> data_point_vector;   //vector is filled with ALL data points from all dat files in directory
    float force_body_x  = 0;                 //sum of all forces on body
    float force_body_y  = 0;
    float force_body_z  = 0;
    float q_total_body  = 0;
    float free_stream_rho           = 0;
    float free_stream_speed         = 0;
    bool calc_Cd        = 0;
    float average_p     = 0;

//Input args and error check
    for (int i=0;i<argc;++i){
       location = string(argv[i]).find(':');       
       main_config_map[string(argv[i]).substr(0,location)]=string(argv[i]).substr(location+1);
    }  

    if(main_config_map.count("help")){
        Help_Function();
        return 0;
    }

    if(main_config_map.count("dir")){
        directory = main_config_map["dir"];
    }else{
        cout<<"Error - no Dir.  add Dir:<full path to directory>\n"<<" add help to command for examples\n";
        return 0;
    }

    if(main_config_map.count("file")){
        output_file_name = main_config_map["file"];
    }else{
        cout<<"Warning - No output file nominated - default output to calculated_loads.txt\nSet file:<filename> if require different output file.\n\n";
        output_file_name = "calculated_loads.txt";        
    }

    if(main_config_map.count("type")){                
        if(main_config_map["type"]=="linear"){
            cout<<"Linear problem, y load calculated\ncalculated forces are per m\n";
            radial = 0;
        };
    }else{
        cout<<"Axisymmetric problem, setting y and z loads to 0.\n";               
    }

//parameters for Cd
    if(!main_config_map.count("rho") || !main_config_map.count("speed")){                
        cout<<"\nWarning - rho: and speed: not set - Cd not calculated - set rho:XXXe-x speed:XXX for Cd calc.\n"; 
    }else{
        free_stream_rho   = stof(main_config_map["rho"]);
        free_stream_speed = stof(main_config_map["speed"]);
        calc_Cd = 1;
        cout<<"\nCd calculated with rho:"<<free_stream_rho<<"  speed:"<<free_stream_speed<<"\n";;              
    }


//READ Dir into dat_names_vector if they are .dat
    for (const auto &entry : fs::directory_iterator(directory)){
        //std::cout<<entry.path().string()<<std::endl;
        if(entry.path().extension().string()==".dat"){
            //dat_names_vector.push_back(entry.path().stem().string()  );
            dat_names_vector.push_back(entry.path().filename().string()  );
        }
    }

//Main for loop - Iterate through the dat files - sum loads - creates data_point_vector with all points from directory   

    for(int i=0;i<dat_names_vector.size();++i){
        cout<<"Importing file:"<<dat_names_vector.at(i)<<endl;     
        
        dat_file.open (directory+dat_names_vector.at(i));  
        if( !dat_file ){ cout<<"Data file not open - do you have the directory correct?\n"; }
    
    //read each line from individual dat file into vector
        while(getline(dat_file, dat_file_line)){   
            if(dat_file_line.find("#")==std::string::npos && dat_file_line.size()>0){   //check for comment item and don't add zero length lines to vector
                          
                data_point _data_point;                                                 //temporary data_point to push_back into vector - destroyed after each iteration

           /* Import_Data_Line destroys the strng by erasing the number at the front
            * This is messy but it is verbose and debuggable
            * Order of reading has to be in the same order in data file - can't skip points
            */
                _data_point.posx        = Import_Data_Line(dat_file_line);   
                _data_point.posy        = Import_Data_Line(dat_file_line);  
                _data_point.posz        = Import_Data_Line(dat_file_line); 
                _data_point.nx          = Import_Data_Line(dat_file_line); 
                _data_point.ny          = Import_Data_Line(dat_file_line); 
                _data_point.nz          = Import_Data_Line(dat_file_line); 
                _data_point.area        = Import_Data_Line(dat_file_line); 
                _data_point.cellwidth   = Import_Data_Line(dat_file_line); 
                _data_point.outsign     = Import_Data_Line(dat_file_line); 
                _data_point.pressure    = Import_Data_Line(dat_file_line); 
                _data_point.rho         = Import_Data_Line(dat_file_line);  
                _data_point.T           = Import_Data_Line(dat_file_line);  
                _data_point.velx        = Import_Data_Line(dat_file_line);
                _data_point.vely        = Import_Data_Line(dat_file_line); 
                _data_point.velz        = Import_Data_Line(dat_file_line);  
                _data_point.mu          = Import_Data_Line(dat_file_line);  
                _data_point.a           = Import_Data_Line(dat_file_line); 
                _data_point.reynolds    = Import_Data_Line(dat_file_line);  
                _data_point.yplus       = Import_Data_Line(dat_file_line);  
                _data_point.taux        = Import_Data_Line(dat_file_line);  
                _data_point.tauy        = Import_Data_Line(dat_file_line);  
                _data_point.tauz        = Import_Data_Line(dat_file_line); 
                _data_point.qtotal      = Import_Data_Line(dat_file_line);
                _data_point.qcond       = Import_Data_Line(dat_file_line);  
                _data_point.qdif        = Import_Data_Line(dat_file_line); 

                data_point_vector.push_back(_data_point);          
            }
        }   
        dat_file.close();

    }//end of individual file loop   

//Calculate results - kept separate from main for loop if we want to multithread for some reason.
    
    for (int i=0;i<data_point_vector.size(); ++i){            
        //cout<<"posx "<<data_point_vector.at(i).posx<<"posy "<<data_point_vector.at(i).posy<<endl;   
        //cout<<"qtotal "<<data_point_vector.at(i).qtotal<<"pressure "<<data_point_vector.at(i).pressure<<endl; 
        data_point_vector.at(i).force_total = data_point_vector.at(i).area*data_point_vector.at(i).pressure;
        data_point_vector.at(i).force_x     = data_point_vector.at(i).force_total*data_point_vector.at(i).nx*data_point_vector.at(i).outsign;
        data_point_vector.at(i).force_y     = data_point_vector.at(i).force_total*data_point_vector.at(i).ny*data_point_vector.at(i).outsign;
        data_point_vector.at(i).force_z     = data_point_vector.at(i).force_total*data_point_vector.at(i).nz*data_point_vector.at(i).outsign;
        force_body_x += data_point_vector.at(i).force_x;
        force_body_y += data_point_vector.at(i).force_y;
        force_body_z += data_point_vector.at(i).force_z;
        q_total_body += data_point_vector.at(i).qtotal*data_point_vector.at(i).area;
       
    }

    if(radial){
        force_body_x *= 2*pi;
        force_body_y = 0;       //axisymmetric obviously has no y or z net loads.
        force_body_z = 0;
        q_total_body *= -2*pi;  //q_total is calculated on gas - gas losing heat = body gaining heat
    }  

    cout<<"Force x (N):"<<force_body_x<<
        "\nForce y (N):"<<force_body_y<<
        "\nForce z (N):"<<force_body_z<<endl; 
//Output loads to file
    ofstream output_file(directory+output_file_name);
    output_file<<"Directory:\n    "<<directory<<endl<<endl;

    output_file<<"Body forces summation from files:\n";
    for(int i=0;i<dat_names_vector.size();++i){
        output_file<<"File "<<i+1<<" : "<<dat_names_vector.at(i)<<endl;
    }
    output_file<<"\n\nCalculated body loads:\n";
    output_file<<"     X (N) :"<<force_body_x<<
               "\n     Y (N) :"<<force_body_y<<
               "\n     Z (N) :"<<force_body_z<<
               "\n\nCalculated body heat (w) :"<<q_total_body;


//calc drag coeff for radial - linear not sorted
    if(calc_Cd){
        float frontal_area = 0;
        float coeff_drag = 0;
        int num_calc = 0;
        //free_stream_rho = 3.709e-05;    //TODO - add this to the args
        //free_stream_speed = 9000;                //TODO - add this to the args

        float pressure_averaging_counter = 0;

        for (int i=0;i<data_point_vector.size(); ++i){            
            if(data_point_vector.at(i).outsign>0){
                frontal_area+=data_point_vector.at(i).area*data_point_vector.at(i).nx; 
            }
            if(data_point_vector.at(i).outsign*data_point_vector.at(i).nx>0.001){
                average_p += data_point_vector.at(i).pressure;
                pressure_averaging_counter++;
            }
        }
        average_p/=pressure_averaging_counter;
        
        frontal_area *= 2*pi;
        coeff_drag = 2*force_body_x/(free_stream_rho*frontal_area*pow(free_stream_speed,2));
        cout<<"Drag Coefficient:\n"<<  
              "   body frontal area    m2:"<<frontal_area<<"\n"<<
              "   free stream rho   kg/m3:"<<free_stream_rho<<"\n"<<
              "   free stream speed   m/s:"<<free_stream_speed<<"\n"<<
              "   calculated Cd          :"<<coeff_drag<<"\n"<<
              "   average pressure     Pa:"<<average_p<<"\n";
        
    }


}




float Import_Data_Line(string &_line){
    float _output = 0;
    string _delimiter = " ";
    size_t _location;  
    _location =_line.find(_delimiter); 
    _output = stof(_line.substr(0, _location));
    _line.erase(0, _location + _delimiter.length());
    return _output;

}



void Help_Function(){
    cout<<  "Instructions\n\n"<<
            "Required args:\n"<<
            "     dir:<full path to directory>     All .dat files in this directory will be summed\n"<<
            "Optional args:\n"<<
            "     file:<output file name>          This sets a different output file name to the default\n"<<
            "     type:linear                      Setting this to linear calculates force on linear body per m.  Default is radial\n"<<
            "     rho:<XXXe-XX>                    units kg/m3 - Setting this and speed initiates Cd calc.  If either is not set Cd no calc'd\n"<<
            "     speed:<XXXX>                     units m/s\n\n"
            "Examples- note directory structure:\n"<<
            "     windows: calc_loads dir:H:\\CFD\\sandpit2\\loads\\t0170\\ \n"<<
            "              calc_loads dir:H:\\CFD\\sandpit2\\loads\\t0170\\ file:newfilename.txt \n"<<
            "              calc_loads dir:H:\\CFD\\sandpit2\\loads\\t0170\\ file:somefilename.txt type:linear\n"<<
            "              calc_loads dir:H:\\CFD\\sandpit2\\loads\\t0170\\ rho:3.7e-5 speed:9000\n\n"<<
            "     linux  : ./calc_loads dir:/home/aaron/CFD/sandpit2/loads/t0170/ \n";
}