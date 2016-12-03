#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <stdlib.h>

using namespace std;

const int blockSize = 4096;

queue<string> dirList(const char * parent){

    DIR *dp;
    struct dirent *ep;
    queue<string> directories,resulDir;
    string parentStr = parent;

    dp = opendir (parent);
    if (dp != NULL)
    {
        while (ep = readdir (dp)){
            directories.push(ep->d_name);
        }
        (void) closedir (dp);

        directories.pop();
        directories.pop();
    }

    resulDir.push(parentStr);

    while(!directories.empty()){
        string currentDirectory = directories.front();
        string nextDirectory = parent;
        if(nextDirectory[nextDirectory.size()-1]!='\\')
            nextDirectory.append("\\");
        nextDirectory.append(currentDirectory);
        directories.pop();

        queue<string> auxDir = dirList(nextDirectory.c_str());

        while(!auxDir.empty()){
            resulDir.push(auxDir.front());
            auxDir.pop();
        }
    }
    return resulDir;
}

void createFile(const char * dir){
    cout<< "Armazenando " << dir << " em nome.sar" << endl;
    string dirStr = dir;
    queue<string> normalizedDirectories;
    queue<string> directories = dirList(dir);

    directories.pop();
    queue<string> auxDir = directories;

    std::fstream fs;
    fs.open ("nome.sar", std::fstream::out | std::fstream::binary);

    while(!auxDir.empty()){
        normalizedDirectories.push(auxDir.front().erase(0,dirStr.size()+1));
        auxDir.pop();
    }
    std::fstream fileOpened;
    while(!normalizedDirectories.empty()){
        fs<<"#|"<<normalizedDirectories.front()<<"|";
        fileOpened.open (directories.front().c_str(), std::fstream::in| std::fstream::binary |std::fstream::ate);
        if(fileOpened.is_open()){
            fs<<fileOpened.tellg()<<"|";
            fileOpened.seekg (0, fileOpened.beg);

            char buffer;
            while (!fileOpened.eof())
            {
                    fileOpened >> std::noskipws>> buffer;
                    fs << buffer;
            }
            fs <<"|";
        }else
            fs<<"-1|";

        cout<<normalizedDirectories.front()<<endl;

        normalizedDirectories.pop();
        if(!directories.empty())
            directories.pop();
        fileOpened.close();
    }
    fs.close();
}

int readFile(const char * dir){
    std::fstream fs, bf;
    fs.open (dir, std::fstream::in | std::fstream::binary);
    bf.open ("teste.txt", std::fstream::out | std::fstream::binary);
    queue<string> paths;

    char buffer;

    if (fs.is_open())
    {
        while (!fs.eof())
        {
            fs >> std::noskipws>> buffer;
            bf << buffer;
        }
    }
    /*char * buffer = new char [blockSize];
    do{
        fs.get (buffer,blockSize);
        cout<<buffer<<endl;
    }while(!fs.eof());

    return 0;*/

}

int main (void)
{
    //createFile("C:\\Users\\leona\\Documents\\GitHub\\sarPacker");
    createFile("C:\\Users\\leona\\Documents\\My Games\\Terraria\\Players");
    //readFile("C:\\Users\\leona\\Documents\\GitHub\\sarPacker\\nome.sar");
}
