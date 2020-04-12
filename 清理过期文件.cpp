#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <time.h>
using namespace std;

const long day = 86400;

bool IsFormat(string path)
{
    int len = path.length();
    string format = path.substr(len - 4, 4);//不同文件类型需要获取的后缀长度不同。
    if(format == ".exe"){//删除后缀为".exe"文件
        return true;
    }
    else{
        return false;
    }
    
}

//获取文件的给更新时间
long get_file_modify_time(string filepath)
{
    struct stat filehand;
    FILE *fp;
    fp = fopen(filepath.c_str(), "r");
    int fileid = fileno(fp);
    fstat(fileid, &filehand);
    fclose(fp);
    return filehand.st_mtime;
}
//获取文件夹中的所有文件
void get_files(const string dirname, vector<string> &filelist)
{
    if(dirname.empty())
        return;
    struct stat s;
    stat(dirname.c_str(), &s);
    if(!S_ISDIR(s.st_mode))
        return;
    DIR *dirhand = opendir(dirname.c_str());
    if(NULL == dirhand){
        exit(EXIT_FAILURE);
    }
    dirent *fp = nullptr;
    while((fp = readdir(dirhand)) != nullptr){
        if(fp->d_name[0] != '.'){//十分重要的一行（？）
            string filename = dirname + "/" + string(fp->d_name);
            struct stat filemod;
            stat(filename.c_str(), &filemod);
            if(S_ISDIR(filemod.st_mode)){
                get_files(filename, filelist);
            }
            else if(S_ISREG(filemod.st_mode)){
                filelist.push_back(filename);
            }
        }
    }
    closedir(dirhand);
    return;
}

bool delete_file(string filepath)
{
    return remove(filepath.c_str());
}

bool date_from_now(long now, long modify, int howlong = 14)//默认清除14天前的文件
{
    int dis = int((1.0 * (now - modify) / day + 0.5));
    return dis >= howlong;//删除最近更新时间距今超过'howlong'天的文件
}

void delete_by_fileformat(string address)
{
    if(IsFormat(address)){
        if(!delete_file(address)){
            cout << "The file named : " << address << " has been deleted." << endl;
        }
        else{
            cout << "Delete Failed!" << endl;
        }
    }
    return;
}

void delete_by_time(string address)
{
    time_t now;
    time(&now);//获取当前系统时间
    if(date_from_now(now, get_file_modify_time(address))){
        if(!delete_file(address)){
            cout << "The file named : " << address << " has been deleted." << endl;
        }
        else{
            cout << "Delete Failed!" << endl;
        }
    }
    return;
}

int main()
{
    string dir = "/media/leaflove/DATA/ACM";//需要处理的文件夹
    vector<string> filelist;
    get_files(dir, filelist);//获取文件夹中的所有文件
    for(auto i : filelist){
        //delete_by_fileformat(i);
        delete_by_time(i);
    }
    return 0;
}
