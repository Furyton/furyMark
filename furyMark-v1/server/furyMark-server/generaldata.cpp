#include "generaldata.h"


QDataStream &operator <<(QDataStream & output,generalData  const &obj)
{
    output << obj.type;
//    output << obj.timeStamp;


    if(obj.type == 0) {
        output << obj.timeStamp;
        output << obj.fileName;
        output << obj.content;
    } else if(obj.type == 2) {
        output << obj.timeStamp;
    } else if(obj.type == 3) {
        output << obj.timeStamp;
    } else if(obj.type == 4) {
        output << obj.timeStamp;
    }

    else if(obj.type == 5) {
        output << obj.listLength;
        queue<int> stampList = obj.fileStampList;
        for(int i = 0; i < obj.listLength; i++) {
            output << stampList.front();
            stampList.pop();
        }
        queue<QString> nameList = obj.fileNameList;
        for(int i = 0; i < obj.listLength; i++) {
            output << nameList.front();
            nameList.pop();
        }
    } else if(obj.type == 6) {
        output << obj.timeStamp;
        output << obj.fileName;
        output << obj.content;
    } else if(obj.type == 7) {
        output << obj.timeStamp;
        output << obj.fileName;
        output << obj.attachPDF;
    } else if(obj.type == 8) {
        output << obj.timeStamp;
    }

    return output;
}
QDataStream &operator >>(QDataStream & input, generalData &obj)
{
    input >> obj.type;
//    input >> obj.timeStamp;

    if(obj.type == 0) {
        input >> obj.timeStamp;
        input >> obj.fileName;
        input >> obj.content;
    } else if(obj.type == 2) {
        input >> obj.timeStamp;
    } else if(obj.type == 3) {
        input >> obj.timeStamp;
    } else if(obj.type == 4) {
        input >> obj.timeStamp;
    }

    else if(obj.type == 5) {
        input >> obj.listLength;
        int stamp = -1;
        for(int i = 0; i < obj.listLength; i++) {
            input >> stamp;
            obj.fileStampList.push(stamp);
        }
        QString name;
        for(int i = 0; i < obj.listLength; i++) {
            input >> name;
            obj.fileNameList.push(name);
        }
    } else if(obj.type == 6) {
        input >> obj.timeStamp;
        input >> obj.fileName;
        input >> obj.content;
    } else if(obj.type == 7) {
        input >> obj.timeStamp;
        input >> obj.fileName;
        input >> obj.attachPDF;
    } else if(obj.type == 8) {
        input >> obj.timeStamp;
    }

    return input;
}
