// p3dtool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>
#include <string>
#include <vector>
#include "pure3d.h"

int main(int argc, char* argv[])
{

    if (argc == 1)
    {
        std::cout << "Usage: p3dtool <file/folder>" << std::endl;
        return 1;
    }
    std::string cmd = argv[1];

    std::string input = cmd.substr(0, cmd.find_first_of(".", 0));


    if (std::filesystem::exists(cmd))
    {
        if (std::filesystem::is_directory(cmd))
        {
            std::string output = cmd;
            output += ".p3d";
            std::ofstream oFile(output , std::ofstream::binary);

           // std::filesystem::current_path(cmd);

            std::vector<std::string> filePaths;
            std::vector<int> fileSizes;

            for (const auto& file : std::filesystem::recursive_directory_iterator(cmd))
            {
                if (file.path().has_extension())
                {
                    if (file.path().extension().string() == ".p3d_chunk")
                    {
                        fileSizes.push_back(std::filesystem::file_size(file.path()));
                        filePaths.push_back(file.path().string());
                    }
                }
            }

            p3d_header header;
            header.header = 0xFF443350;
            header.version = 12;

            int totalSize = 0;
            for (unsigned int i = 0; i < fileSizes.size(); i++)
                totalSize += fileSizes[i];
            totalSize += sizeof(p3d_header);
            header.fileSize = totalSize;

            oFile.write((char*)&header, sizeof(p3d_header));

            for (unsigned int i = 0; i < filePaths.size(); i++)
            {
                std::ifstream pFile(filePaths[i], std::ifstream::binary);
                std::cout << "Processing: " << filePaths[i] << std::endl;

                if (!pFile)
                {
                    std::cout << "ERROR: Could not open " << filePaths[i] << "!" << std::endl;
                    return 1;
                }

                int chunkSize = fileSizes[i];
                std::unique_ptr<char[]> chunkData = std::make_unique<char[]>(chunkSize);
                pFile.read(chunkData.get(), chunkSize);

                oFile.write(chunkData.get(), chunkSize);

            }
            std::cout << "Built '" << output << "'!" << std::endl;
            std::cout << "Finished." << std::endl;
        }
        else
        {
            std::ifstream pFile(cmd, std::ifstream::binary);
            
            if (!pFile)
            {
                std::cout << "ERROR: Could not open " << cmd << "!" << std::endl;
                return 1;
            }

            if (!std::filesystem::exists(input))
              std::filesystem::create_directory(input);
            std::filesystem::current_path(input);

            p3d_header header;
            pFile.read((char*)&header, sizeof(p3d_header));
            int chunkId = 0;

            if (header.header != 0xFF443350)
            {
                std::cout << "ERROR: Input file is not a valid Pure3D file!" << std::endl;
                return 1;
            }

            // get chunks
            while ((int)pFile.tellg() != header.fileSize)
            {
                p3d_chunk chunk;
                pFile.read((char*)&chunk, sizeof(p3d_chunk));

                int chunkSize = chunk.dataSize - sizeof(p3d_chunk);

                std::unique_ptr<char[]> chunkData = std::make_unique<char[]>(chunkSize);

                pFile.read(chunkData.get(), chunkSize);

                int readData = chunk.dataSize;
                int childCount = 0;

                std::string output = std::to_string(chunkId);
                output += "_chunk_";

                // could probably use stringstream but eh
                char hexBuff[64] = {};
                sprintf_s(hexBuff, "%X", chunk.id);

                output += hexBuff;
                output += ".p3d_chunk";

                std::ofstream oFile(output, std::ofstream::binary);
                oFile.write((char*)&chunk, sizeof(p3d_chunk));
                oFile.write(chunkData.get(), chunkSize);

                while (!(readData == chunk.size))
                {

                    p3d_chunk child;
                    pFile.read((char*)&child, sizeof(p3d_chunk));

                    int childSize = child.dataSize - sizeof(p3d_chunk);

                    std::unique_ptr<char[]> childData = std::make_unique<char[]>(childSize);
                    pFile.read(childData.get(), childSize);
                    oFile.write((char*)&child, sizeof(p3d_chunk));
                    oFile.write(childData.get(), childSize);
                    childCount++;
                    readData += child.dataSize;

                }
                chunkId++;
                std::cout << "Processing 0x" << std::hex << chunk.id << std::dec << " Children: " << childCount << std::endl;
            }
            std::cout << "Extracted " << chunkId << " chunks to '" << input << "'!" << std::endl;
            std::cout << "Finished." << std::endl;
                
        }
    }
    else
    {
        std::cout << "ERROR: Input file or folder " << cmd << " does not exist!" << std::endl;
        return 1;
    }

    return 0;
}
