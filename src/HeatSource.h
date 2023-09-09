#ifndef HEATSOURCE_H
#define HEATSOURCE_H

#include <QObject>

#include <optional>

class QSerialPort;

class HeatSource : public QObject
{
    Q_OBJECT;

    typedef HeatSource Self;
    typedef QObject Parent;
    
public:
    HeatSource(QSerialPort *port, QObject *parent = NULL);

Q_SIGNALS:
    void HeatMapRead(const QVector<float>& map);

protected:
    void _ReadDevice();
    std::optional<float> _Add(const char& data);
    
private:
    
    QSerialPort *_serialPort;
    QVector<float> _temps;
};

#endif
