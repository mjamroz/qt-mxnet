#ifndef MXPREDICT_H
#define MXPREDICT_H

#include <QObject>
#include <mxnet/c_predict_api.h>
#include "bufferfile.h"
class MXPredict : public QObject
{
    Q_OBJECT
public:
    explicit MXPredict(QObject *parent = nullptr);
    Q_INVOKABLE int predict(QString path);
private:
    std::vector<std::string> LoadSynset(std::string synset_file);
    void GetImageFile(QString image_file, mx_float* image_data, const int channels, const int width);
    void PrintOutputResult(const std::vector<float>& data, const std::vector<std::string>& synset);
    static BufferFile json_data;
    static BufferFile param_data;
signals:

public slots:
};

#endif // MXPREDICT_H
