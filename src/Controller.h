#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringListModel>

#include <QPointF>

class QQmlContext;
class QTimer;
// Let's put this in another class
// And, uh... overdoing it on the namespaces a bit?
namespace rp {
    namespace standalone {
        namespace rplidar {
            class RPlidarDriver;
        }
    }
}

class HeatSource;

class Controller : public QObject
{
    Q_OBJECT

    typedef Controller Self;
    typedef QObject Parent;

public:
    static Controller * GetInstance();

public:
    Controller(QObject *parent=nullptr);
    virtual ~Controller();

    void SetContext(QQmlContext *context);

    //Q_INVOKABLE void SetUsbDevice(int index, bool isToggled);

    QVector<QPointF> GetPoints() const;

Q_SIGNALS:
    void PointsUpdated();

protected:
    void SetUsbDevice(int index, bool isToggled);
    void _PopulateUSBDeviceList();

    void _ConnectToLidar(const QString& usbPath);
    void _Disconnect();
    void _ReadDevice();
    QPointF _MakePoint(float angle, float dist);
    static const QString ControllerPropertyName;
    static const QString USBDevicesPropertyName;

    static const quint16 CP2104VendorID;
    static const QString AMG8833Serial;
    static const QString RPLidarSerial;

private:
    static Controller *_instance;

private:
    // Holds the QSerialPort
    QVariantMap _usbDevices;
    int _selectedDevice;

    QVector<QPointF> _points;
    QQmlContext *_context;
    QTimer *_scanTimer;
    
    rp::standalone::rplidar::RPlidarDriver *_lidarDriver;
    HeatSource *_heatSource;
};

#endif // CONTROLLER_H
