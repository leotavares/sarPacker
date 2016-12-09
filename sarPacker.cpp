/*
Autores:
    Leonardo Tavares Oliveira; RA: 628174
    Thiago Morano da Silva;    RA: 496227
    Tulio Reis Carvalho;       RA: 628050
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>
#include <queue>
#include <math.h>
#include <dirent.h>
#include <fstream>

using namespace std;

const int blockSize = 4096;
///fun��o recursiva para listar todos os arquivos e pastas
///recebe como par�metro o diret�rio para listar
///retorna uma fila com todos as pastas e arquivos
queue<string> dirList(const char * parent){

    DIR *directoryToWork;
    struct dirent *fileOrFolder;
    queue<string> directories,resulDir; ///directories recebe todos os arquivos e pastas do diret�rio atual, resul recebe recursivamente
    string parentStr = parent;

    directoryToWork = opendir (parent); ///tenta abrir o "diret�rio" atual
    if (directoryToWork != NULL)
    {
        ///enquanto houver diret�rios, coloca-os na fila
        while (fileOrFolder = readdir (directoryToWork)){
            directories.push(fileOrFolder->d_name);
        }
        (void) closedir (directoryToWork);  ///fecha o diret�rio

        directories.pop();  ///a opera��o superior sempre retorna . e .., os pops abaixo s�o para retira-los
        directories.pop();
    }
    ///coloca o diret�rio na fila de diret�rios
    resulDir.push(parentStr);
    ///caso o diret�rio atual seja um arquivo, n�o passa por esse while e retorna diretamente pro diret�rio superior
    while(!directories.empty()){
        string currentDirectory = directories.front();  ///pega o primeiro diret�rio da fila
        string nextDirectory = parent;  ///salva o diret�rio pai em outra string para ser editado
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

int createFile(const char * dir){
    if(opendir(dir)!=NULL){
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
        return 0;
    }else
        return 1;
}

//Fun��o realiza a leitura e impress�o dos arquivos salvos no .sar
//Recebe o diret�rio em que o .sar se encontra
//Retorna 0, caso tenha realizado a leitura com sucesso, ou 2 caso o diret�rio passado n�o seja .sar
int printFile(const char * dir){
    std::fstream fileSar;
    char buffer;
    string numBytes = "";
    int flag = 0;
        /* flag status:
                0 = ler o diret�rio
                1 = pular os pr�ximos numeros de bytes do arquivo se necess�rio
                2 = ler o tamanho do diret�rio
        */
    fileSar.open (dir, std::fstream::in | std::fstream::binary);
    if ((string(dir).compare(string(dir).size()-4, 4, ".sar") == 0) && fileSar.is_open()) {
        fileSar.seekg (2, fileSar.beg);
        while (!fileSar.eof())
        {
            fileSar >> std::noskipws >> buffer;
            switch (flag) {
                case 0:
                    if(buffer=='|'){
                        flag = 1;
                        cout<<endl;
                    }else
                        cout << buffer;
                    break;
                case 1:
                    if(buffer=='|'){
                        long int nBytes = strtol(numBytes.c_str(), NULL, 10);
                        if(nBytes!=-1)
                            fileSar.seekg (nBytes+1, fileSar.cur);
                        else
                            fileSar.seekg (-1, fileSar.cur);
                        flag = 2;
                        numBytes.clear();
                    }
                    else {
                        numBytes+=buffer;
                    }
                    break;

                case 2:
                    fileSar.seekg (2, fileSar.cur);
                    flag = 0;
                    break;
            }
        }
    }
    else {
        return 2;
    }
}

//Fun��o realiza a extra��o dos arquivos salvos no .sar
//Recebe o diret�rio em que o .sar se encontra
//Retorna 0, caso tenha extra�do com sucesso, ou 2 caso o diret�rio passado n�o seja .sar
int extractFile (const char * dir) {
    fstream fileSar;
    fstream fileBeingWritten;
    char buffer;
    string numBytes = "";
    string fileName = "";
    int flag = 0;
    long int bytesWritten = 0;
    long int nBytes = 0;
    bool write;
        /* flag status:
                0 = ler o diret�rio
                1 = pular os pr�ximos numeros de bytes do arquivo, se necess�rio; ou preparar para escrever no arquivo
                2 = escrever no arquivo
        */
    fileSar.open (dir, fstream::in | fstream::binary);
    if ((string(dir).compare(string(dir).size()-4, 4, ".sar") == 0) && fileSar.is_open()) {
        fileSar.seekg (2, fileSar.beg);
        while (!fileSar.eof())
        {
            fileSar >> std::noskipws >> buffer;
            switch (flag) {
                case 0:
                    if(buffer=='|'){
                        flag = 1;
                    }else
                        fileName += buffer;
                    break;
                case 1:
                    if(buffer=='|'){
                        nBytes = strtol(numBytes.c_str(), NULL, 10);
                        if(nBytes == -1) {
                            fileSar.seekg (-1, fileSar.cur);
                            system(("mkdir .\\"+fileName).c_str());
                            write = false;
                        }
                        else {
                            fileBeingWritten.open((".\\"+fileName).c_str(), fstream::out | fstream::binary);
                            write = true;
                        }
                        flag = 2;
                        fileName.clear();
                        numBytes.clear();
                    }
                    else {
                        numBytes+=buffer;
                    }
                    break;
                case 2:
                    if(write) {
                        if(bytesWritten == (nBytes+1)) {
                            fileSar.seekg (2, fileSar.cur);
                            flag = 0;
                            fileBeingWritten.close();
                            bytesWritten = 0;
                            nBytes = 0;
                        } else {
                            fileBeingWritten << buffer;
                            bytesWritten++;
                        }
                    }
                    else {
                        fileSar.seekg (2, fileSar.cur);
                        flag = 0;
                    }
                    break;
            }
        }
        return 0;
    }
    else {
        return 2;
    }
}

int main (int argc, char *argv[])
{
    if(argc>=3){
        if(string(argv[1]).compare("-e")==0){
            int retorno = 0;
            retorno = extractFile(argv[2]);
            return retorno;
        }
        if(string(argv[1]).compare("-c")==0){
            int retorno = 0;
            retorno = createFile(argv[2]);
            return retorno;
        }if(string(argv[1]).compare("-l")==0){
            int retorno = 0;
            retorno = printFile(argv[2]);
            return retorno;
        }
    }else{
        cout<<"Argumentos errados";
    }
}
