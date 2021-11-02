#include <fstream>
#include "ContextManager.h"
#include "easylogging++.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Please specify a path to a PNG file\n");
        return EXIT_FAILURE;
    }

    //读取配置文件
    if (!gContextInstance->ParserCmdLine(argc, argv)) {
        LOG(ERROR) << "parse command line error argc:" << argc;
        return -1;
    }

    //读取配置文件
    if (!gContextInstance->ParserConfig()) {
        LOG(WARNING) << "parse config file fail,use default" ;
    }

    //遍历所有文件
    gContextInstance->TraversePath();

    //创建文件
    gContextInstance->MakePathDir();

    //obj格式压缩
    if (!gContextInstance->EncodeFiles()) {
        LOG(ERROR) << "Encode file fail";
        return -1;
    }

    return 0;
}