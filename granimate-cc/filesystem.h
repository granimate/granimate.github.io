#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <cstdio>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

std::string get_pwd()
{
	char * pwd_char = getcwd(NULL, 0);
	std::string pwd = pwd_char;
	free(pwd_char);
	return pwd;
}

bool path_exists(const std::string& path)
{
	struct stat info;

	if(stat( path.c_str(), &info ) != 0) //no file
		return 0;
	else
		return 1;
}

bool dir_exists(const std::string& dir_path)
{
	struct stat info;

	if(stat( dir_path.c_str(), &info ) != 0) //no file
		return 0;
	else if(info.st_mode & S_IFDIR) //dir
		return 1;
	else //file
		return 0;
}

bool file_exists(const std::string& file_path)
{
	struct stat info;

	if(stat( file_path.c_str(), &info ) != 0) //no file
		return 0;
	else if(info.st_mode & S_IFDIR) //dir
		return 0;
	else //file
		return 1;
}

bool can_write(const std::string& path)
{
	if(!path_exists(path))
		return 1;

	struct stat  st;

	if (stat(path.c_str(), &st) < 0)
		return 0;
	if ((st.st_mode & S_IWUSR) != 0)
		return 1;
	return 0;
}

bool remove_file(const std::string& path)
{
	if(path != "" && path_exists(path))
		if(std::remove(path.c_str()))
			return 1;

	return 0;
}

std::string ifs_to_string(std::ifstream& ifs)
{
	std::string s;
	getline(ifs, s, (char) ifs.eof());
	return s;
}

std::string string_from_file(const std::string& path)
{
	std::string s;
	std::ifstream ifs(path);
	getline(ifs, s, (char) ifs.eof());
	ifs.close();
	return s;	
}

int cp_file(const std::string& source, 
           const std::string& target)
{
	std::ifstream ifs(source);
	std::ofstream ofs(target);

	if(!ifs.is_open())
	{
		std::cout << "err: cp(" << source << ", " << target << "): failed to open " << source << " for reading" << std::endl;
		return 1;
	}
	else if(!ofs.is_open())
	{
		std::cout << "err: cp(" << source << ", " << target << "): failed to open " << target << " for writing" << std::endl;
		return 1;
	}

	ofs << ifs.rdbuf();

	//ofs << string_from_file(source) << std::endl;

	ifs.close();
	ofs.close();

	return 0;
}

#endif //FILESYSTEM_H_