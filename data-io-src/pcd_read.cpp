
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

int
main (int argc, char** argv)
{
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
  if (pcl::io::loadPCDFile<pcl::PointXYZRGB> ("test_pcd.pcd", *cloud) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file test_pcd.pcd \n");
    return (-1);
  }
  double cloudSize   = cloud->points.size();
  // std::cout << cloudSize << std::endl;

  std::cout << "Loaded "
            << cloud->width * cloud->height
            << " data points from test_pcd.pcd with the following fields: "
            << std::endl;
  
  DIR *dir;
  struct dirent *ent;
  // Data Directory Name
  std::string XMLDirName   = "./trial-dir";
  std::string DataDirName   = "./pspi-data";
  // Open That Directory
  if ((dir = opendir (XMLDirName.c_str())) != NULL) 
  {
    /* All the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) 
    {
      // printf ("%s\n", ent->d_name);

      // Get The Filename
      std::string FileName = ent->d_name;
      // Checking for . and .. names
      if ((FileName.compare(".") != 0) && (FileName.compare("..") != 0))
      {
        // Make File Name For Writing Out
        int dotPos            = FileName.find_last_of(".");
        std::string RawName   = FileName.substr(0, dotPos);
        // std::cout << RawName << std::endl;

        // Open FileName.xml And Read In Keyboard Idices
        // Create Read-in Filename and Write-Out Filename
        std::string ReadFileName    = XMLDirName + "/" + FileName;
        std::string WriteFileName   = DataDirName + "/" + RawName + "_Keyboard.txt";
        // Open FileName
        std::ifstream myReadFile;
        myReadFile.open(ReadFileName.c_str());
        // std::cerr << "Error: " << strerror(errno);
        std::cout << FileName.c_str() << "..." << std::endl;
        // Output String
        std::string LineReadIn;
        // --------------------------------------------------
        // Open File and Start Processing For Keyboard Points
        // --------------------------------------------------
        if (myReadFile.is_open())
        {
          // Initialise a Vector To Hold Indices
          std::vector<bool> IsIndxKeyboard(cloudSize, 0);
          int NumOfKeyboardPts   = 0;
          int NumOfKeyboards     = 0;
          // Set Some Flags
          bool KeyboardFound         = false;
          bool NextLineIndices       = false;
          bool KeyboardFoundInFile   = false;
          while (!myReadFile.eof()) 
          {
            // Get a New Line
            getline(myReadFile, LineReadIn);
            
            // std::cout << KeyboardFound << "   "<< NextLineIndices << std::endl;
            // std::cout << LineReadIn.substr(0, 200) << std::endl;
            
            if (KeyboardFound && NextLineIndices) // This Line Has Keyboard Points
            {
              std::cout << "I have found keyboard points. Processing now...\n";
              ++NumOfKeyboards;

              KeyboardFound     = false;
              NextLineIndices   = false;

              // Create a Vector To Hold Indices
              std::vector<int> KeyboardIndxs;
              // Remove the <indices> and </indices>
              int startAt                  = LineReadIn.find(">");
              int endAt                    = LineReadIn.find("</indices>");
              std::string NewLineReadIn    = LineReadIn.substr(startAt+1, endAt-startAt-1);
              
              // std::cout << NewLineReadIn << std::endl;
              
              // Start Reading in Integers Piece By Piece To Get All Keyboard Indices
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
                KeyboardIndxs.push_back(n);
                ++NumOfKeyboardPts;
                // For Next Integer
                NewLineReadIn.erase(0, SpacePos+1);
              }
              // Set All These Indices to char=1
              for(int i = 0; i < KeyboardIndxs.size(); ++i)
              {
                IsIndxKeyboard.at(KeyboardIndxs.at(i))   = 1;
              }
              KeyboardFoundInFile   = true;
            }

            if (LineReadIn.find("Keyboard") != std::string::npos)
            {
              KeyboardFound   = true;
              // std::cout << LineReadIn << std::endl;
            }

            if (KeyboardFound && (LineReadIn.find("/dimensions") != std::string::npos))
            {
              NextLineIndices   = true;
            }            

          } // Go To Next Line In File
          
          if (KeyboardFoundInFile) 
          {
            std::cout << "... completed processing. Found " << NumOfKeyboardPts 
                      << " keyboard points from " << NumOfKeyboards << " number of keyboard(s)." 
                      << std::endl;
            // --------------------------------
            // Write Out Results For This File
            // --------------------------------
            std::ofstream outFilePtr;
            outFilePtr.open(WriteFileName.c_str());
            for(int it = 0; it < IsIndxKeyboard.size(); ++it)
            {
              outFilePtr << IsIndxKeyboard.at(it);
            }
            outFilePtr.close();
          }
        } // If File Has Openned ...
        myReadFile.close();
      } // Valid Filenames Only

    } // Loop Over All Files In Directory
    closedir (dir);
  }

  else 
  {
    /* could not open directory */
    perror ("");
    return EXIT_FAILURE;
  }

  return (0);
}