#include <QCoreApplication>
#include "mxpredict.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MXPredict predict;
    predict.predict("cnn/image.jpg");
    return a.exec();
}
