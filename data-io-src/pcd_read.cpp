
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

// #include "global.h"

// Function Declarations
int flagIndxsOfObject(std::vector<bool> &, std::string, std::string);
int writeOutIndxFlags(std::vector<bool> &, std::string);
int findCombinedIndxFlags(std::vector<bool> &, std::vector<bool> &, std::vector<bool> &);

// Data Directory Name
// std::string XMLDirName    = "./xml-dir";
// std::string PCDDirName    = "./pcd-dir";
// std::string KeyboardsDataDirName   = "../pspi-data";

std::string XMLDirName                    = "../xml-data";
std::string PCDDirName                    = "/media/18F69AB9F69A969A/final-data/pcd-annotated";
std::string KeyboardsDataDirName          = "../keyboards-groundtruth";
std::string BooksDataDirName              = "../books-groundtruth";
std::string LaptopsDataDirName            = "../laptops-groundtruth";
std::string KeyboardsBooksDataDirName     = "../keyboards-books-groundtruth";

// Main Function
int
main (int argc, char** argv)
{ 
  DIR *dir;
  struct dirent *ent;

  // Open That Directory
  if ((dir = opendir (XMLDirName.c_str())) != NULL) 
  {
    /* All the files and directories within directory */
    int NumOfFiles   = 0;
    while ((ent = readdir (dir)) != NULL) 
    {
      // printf ("%s\n", ent->d_name);

      // Get The Filename
      std::string FileName = ent->d_name;
      // Checking for . and .. names
      if ((FileName.compare(".") != 0) && (FileName.compare("..") != 0))
      {
        ++NumOfFiles;
// >>>>>>>>>>

// <<<<<<<<<<
        int resKeyboard, resBook, resLaptop = 99;
        // Extract Raw File Name - No Extensions
        int dotPos            = FileName.find_last_of(".");
        std::string RawName   = FileName.substr(0, dotPos);
        // std::cout << RawName << std::endl;
        //----------------------------------------------------
        // KEYBOARD
        //----------------------------------------------------
        // Declare Object & Initialize Flag Container
        std::string Object1("Keyboard");
        std::vector<bool> IsIndxKeyboard;
        // Find Out Which Points Belong To Object
        resKeyboard   = flagIndxsOfObject(IsIndxKeyboard, Object1, RawName);
        // Write Out Results For This File
        std::string WriteFileName1      = KeyboardsDataDirName + "/" + RawName + "_" + Object1 +"_GroundTruth.txt";-
        writeOutIndxFlags(IsIndxKeyboard, WriteFileName1);


        //----------------------------------------------------
        // BOOK
        //----------------------------------------------------
        // Declare Object & Initialize Flag Container
        std::string Object2("Book");
        std::vector<bool> IsIndxBook;
        // Find Out Which Points Belong To Object
        resBook   = flagIndxsOfObject(IsIndxBook, Object2, RawName);
        // Write Out Results For This File
        std::string WriteFileName2      = BooksDataDirName + "/" + RawName + "_" + Object2 +"_GroundTruth.txt";-
        writeOutIndxFlags(IsIndxBook, WriteFileName2);


        //----------------------------------------------------
        // LAPTOP
        //----------------------------------------------------
        // Declare Object & Initialize Flag Container
        // std::string Object3("Laptop");
        // std::vector<bool> IsIndxLaptop;
        // // Find Out Which Points Belong To Object
        // resLaptop   = flagIndxsOfObject(IsIndxLaptop, Object3, RawName);
        // // Write Out Results For This File
        // std::string WriteFileName3      = LaptopsDataDirName + "/" + RawName + "_" + Object3 +"_GroundTruth.txt";-
        // writeOutIndxFlags(IsIndxLaptop, WriteFileName3);


        //----------------------------------------------------
        // KEYBOARDS BOOKS LAPTOPS COMBINED
        //----------------------------------------------------
        std::vector<bool> IsIndxObjects(IsIndxKeyboard.size(),0);
        findCombinedIndxFlags(IsIndxObjects, IsIndxKeyboard, IsIndxBook);
        // Write Out Results For This File
        std::string WriteFileName4      = KeyboardsBooksDataDirName + "/" + RawName + "_" + Object1+"_"+Object2 +"_GroundTruth.txt";
        writeOutIndxFlags(IsIndxObjects, WriteFileName4);

        // Find Combined Flags For Indices
        if ((resKeyboard<0) ||(resLaptop<0) ||(resBook<0))
        {
          std::cout << FileName << "Was not processed! Moving on..."<< std:: endl;
          continue;
        }
        // DEBUG
        // if (NumOfFiles>1)
        //   break;
      } // Valid Filenames Only

    } // Loop Over All Files In Directory
    closedir (dir);
    std::cout << std::endl
              << std::endl
              << std::endl
              << std::endl
              << std::endl
              << --NumOfFiles
              << std::endl;
  }

  else 
  {
    /* could not open directory */
    perror ("");
    return EXIT_FAILURE;
  }

  return (0);
}



// Function Definitions
int flagIndxsOfObject(std::vector<bool> &IsIndxObject, std::string Object, std::string RawName)
{

  // Open FileName.xml And Read In Object Idices
  // Create Read-in Filename and Write-Out Filename
  std::string ReadFileNameXml    = XMLDirName + "/" + RawName + ".xml";
  std::string ReadFileNamePcd    = PCDDirName + "/" + RawName + ".pcd";
  
  // Open FileName
  std::ifstream myReadFile;
  myReadFile.open(ReadFileNameXml.c_str());
  // std::cerr << "Error: " << strerror(errno);
  std::cout << RawName.c_str() << "..." << std::endl;

  // --------------------------------------------------
  // Open PCD File & Load Point Cloud
  // --------------------------------------------------
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
  if (pcl::io::loadPCDFile<pcl::PointXYZRGB> (ReadFileNamePcd.c_str(), *cloud) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file %s \n", ReadFileNamePcd.c_str());
    return(-1);
  }
  double cloudSize   = cloud->points.size();
  // std::cout << cloudSize << std::endl;

  std::cout << "Loaded "
            << cloud->width * cloud->height
            << " data points from " << ReadFileNamePcd
            << std::endl;

  // --------------------------------------------------
  // Open File and Start Processing For Object Points
  // --------------------------------------------------
  if (myReadFile.is_open())
  {
    // Output String
    std::string LineReadIn;
    // Initialise a Vector To Hold Indices

    IsIndxObject.assign(cloudSize, 0);
    
    int NumOfObjectPts   = 0;
    int NumOfObjects     = 0;
    // Set Some Flags
    bool ObjectFound         = false;
    bool NextLineIndices       = false;
    bool ObjectFoundInFile   = false;
    while (!myReadFile.eof()) 
    {
      // Get a New Line
      getline(myReadFile, LineReadIn);
      
      // std::cout << ObjectFound << "   "<< NextLineIndices << std::endl;
      // std::cout << LineReadIn.substr(0, 200) << std::endl;
      
      if (ObjectFound && NextLineIndices) // This Line Has Object Points
      {
        std::cout << "I have found "<< Object <<" points. Processing now...\n";
        ++NumOfObjects;

        ObjectFound     = false;
        NextLineIndices   = false;

        // Create a Vector To Hold Indices
        std::vector<int> ObjectIndxs;
        // Remove the <indices> and </indices>
        int startAt                  = LineReadIn.find(">");
        int endAt                    = LineReadIn.find("</indices>");
        std::string NewLineReadIn    = LineReadIn.substr(startAt+1, endAt-startAt-1);
        
        // std::cout << NewLineReadIn << std::endl;
        
        // Start Reading in Integers Piece By Piece To Get All Object Indices
        while(1)
        {
          int n;
          int SpacePos       = NewLineReadIn.find(" ");
          if (SpacePos < 0)
          {
            break;
          }
          std::string iInt   = NewLineReadIn.substr(0, SpacePos);
          n   = atoi(iInt.c_str());

          // std::cout << "Found integer: " << iInt << "\n";
          
          // Push Integer Into Vector
          ObjectIndxs.push_back(n);
          ++NumOfObjectPts;
          // For Next Integer
          NewLineReadIn.erase(0, SpacePos+1);
        }
        // Set All These Indices to char=1
        for(int i = 0; i < ObjectIndxs.size(); ++i)
        {
          IsIndxObject.at(ObjectIndxs.at(i))   = 1;
        }
        ObjectFoundInFile   = true;
      }

      if (LineReadIn.find(Object) != std::string::npos)
      {
        ObjectFound   = true;
        // std::cout << LineReadIn << std::endl;
      }

      if (ObjectFound && (LineReadIn.find("/dimensions") != std::string::npos))
      {
        NextLineIndices   = true;
      }            

    } // Go To Next Line In File
    
    if (ObjectFoundInFile) 
    {
      std::cout << "... completed processing. Found " << NumOfObjectPts 
                << " "<< Object <<" points from " << NumOfObjects << " number of "
                << Object <<"(s)." 
                << std::endl;
    }
    myReadFile.close();
    return(0);
  } // If File Has Openned ...
  else
  {
    PCL_ERROR ("Couldn't read file %s \n", ReadFileNameXml.c_str());
    return(-1);
  }
}


int writeOutIndxFlags(std::vector<bool> &IsIndxObject, std::string WriteFileName)
{
  // Openning Write Out File
  std::ofstream outFilePtr;
  outFilePtr.open(WriteFileName.c_str());
  for(int it = 0; it < IsIndxObject.size(); ++it)
  {
    outFilePtr << IsIndxObject.at(it);
  } 
    outFilePtr.close();

}

int findCombinedIndxFlags(std::vector<bool> &AB, std::vector<bool> &A, std::vector<bool> &B)
{
  if (A.size() != B.size())
        throw std::invalid_argument("differently sized bitwise operands");

  std::vector<bool>::const_iterator itA = A.begin();
  std::vector<bool>::const_iterator itB = B.begin();
  std::vector<bool>::iterator itAB = AB.begin();
  while (itAB < AB.end())
      *(itAB._M_p ++) = *(itA._M_p ++) | *(itB._M_p ++); // word-at-a-time bitwise operation

  return 0;

}



                            // std::vector<bool> A({0,1,1,0,0});
                            // std::vector<bool> B({0,1,1,0,1});

                            // for(std::vector<bool>::iterator iter = A.begin(); iter != A.end(); ++iter)
                            // {
                            //     std::cout << *iter << " ";
                            // }
                            // std::cout<< std::endl;


                            // for(std::vector<bool>::iterator iter = B.begin(); iter != B.end(); ++iter)
                            // {
                            //     std::cout << *iter << " ";
                            // }
                            // std::cout<< std::endl;

                            // std::vector<bool> AB(5,0);

                            // findCombinedIndxFlags(AB, A, B);

                            // for(std::vector<bool>::iterator iter = AB.begin(); iter != AB.end(); ++iter)
                            // {
                            //     std::cout << *iter << " ";
                            // }
                            // std::cout<< std::endl;