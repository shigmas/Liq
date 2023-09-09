#include "HeatSource.h"


HeatSource::HeatSource(QSerialPort *port, QObject *parent) : Parent(parent)

{

std::optional<float>
HeatSource::_Add(const char& data) {

    if (QChar::isDigit(data)) {
        if (!_inNum) {
            // starting newNum
            _inNum = true;
            if (_curIndex > 63) {
                qDebug() << "Too much data";
            }
            _heatMap[_curIndex] = _curTemp.toFloat();
            _curTemp = QString();
            _curIndex++;
            return _heatMap[_curIndex];
        }
        _curTemp += data;
    } else if (data == '.') {
        _curTemp += data;
    } else {
        _inNum = false;
    }

    return std::nullopt;
}

void
HeatSource::_ReadDevice()
{
    char data;
    qint64 amtRead;
    
    if (!_dataOpened) {
        amtRead = _serialPort->read(&data, 1);
        if (data == '[') {
            qDebug() << "Opened data";
            _dataOpened = true;
            _temps.clear()
        } else {
            qDebug() << "waiting for data";
        }
    }
    
    while (_dataOpened) {
        amtRead = _serialPort->read(&data, 1);
        if (amtRead != 1) {
            break;
        }
        if (data == ']') {
            _dataOpened = false;
            _inNum = false;
            _curIndex = 0;
            qDebug() << "Closed data: " << _heatMap;
            emit HeatMapRead(_temps);
        } else {
            std::optional<float> res = _Add(data);
            if (res) {
                _temps.push_back(res.value());
                qDebug() << "Parsed: " << res.value();
            }
        }
            
    }
}
