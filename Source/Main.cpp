#include <fstream>
#include "ContextManager.h"
#include "easylogging++.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Please specify a path to a PNG file\n");
        return EXIT_FAILURE;
    }

    //��ȡ�����ļ�
    if (!gContextInstance->ParserCmdLine(argc, argv)) {
        LOG(ERROR) << "parse command line error argc:" << argc;
        return -1;
    }

    //��ȡ�����ļ�
    if (!gContextInstance->ParserConfig()) {
        LOG(WARNING) << "parse config file fail,use default" ;
    }

    //���������ļ�
    gContextInstance->TraversePath();

    //�����ļ�
    gContextInstance->MakePathDir();

    //obj��ʽѹ��
    if (!gContextInstance->EncodeFiles()) {
        LOG(ERROR) << "Encode file fail";
        return -1;
    }

    return 0;
}