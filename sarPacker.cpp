#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>
#include <queue>

#include <dirent.h>
#include <fstream>

using namespace std;

const int blockSize = 4096;
///função recursiva para listar todos os arquivos e pastas
///recebe como parâmetro o diretório para listar
///retorna uma fila com todos as pastas e arquivos
queue<string> dirList(const char * parent){

    DIR *directoryToWork;
    struct dirent *fileOrFolder;
    queue<string> directories,resulDir; ///directories recebe todos os arquivos e pastas do diretório atual, resul recebe recursivamente
    string parentStr = parent;

    directoryToWork = opendir (parent); ///tenta abrir o "diretório" atual
    if (directoryToWork != NULL)
    {
        ///enquanto haver diretórios, coloca-os na fila
        while (fileOrFolder = readdir (directoryToWork)){
            directories.push(fileOrFolder->d_name);
        }
        (void) closedir (directoryToWork);  ///fecha o diretório

        directories.pop();  ///a operação superior sempre retorna . e .., os pops abaixo são para retira-los
        directories.pop();
    }
    ///coloca o diretório na fila de diretórios
    resulDir.push(parentStr);
    ///caso o diretório atual seja um arquivo, não passa por esse while e retorna diretamente pro diretório superior
    while(!directories.empty()){
        string currentDirectory = directories.front();  ///pega o primeiro diretório da fila
        string nextDirectory = parent;  ///salva o diretório pai em outra string para ser editado
        if(nextDirectory[nextDirectory.size()-1]!='\\') ///caso
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

///função ainda pra ser implementada, não faz sentido
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
    createFile("C:\\Users\\leona\\Documents\\My Games\\Terraria\\Captures");
    //readFile("C:\\Users\\leona\\Documents\\GitHub\\sarPacker\\nome.sar");
}
