#ifndef CONTROLLER_H
#define CONTROLLER_H

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

    Q_INVOKABLE void SetUsbDevice(int index, bool isToggled);

    QVector<QPointF> GetPoints() const;

Q_SIGNALS:
    void PointsUpdated();

    protected:
    void _PopulateUSBDeviceList();

    void _Connect(const QString& usbPath, int baudrate);
    void _Disconnect();
    void _ReadDevice();
    QPointF _MakePoint(float angle, float dist);
    static const QString ControllerPropertyName;
    static const QString USBDevicesPropertyName;

private:
    static Controller *_instance;

private:
    QStringList _usbDevices;
    int _selectedDevice;

    QVector<QPointF> _points;
    QQmlContext *_context;
    QTimer *_scanTimer;
    
    rp::standalone::rplidar::RPlidarDriver *_lidarDriver;
};

#endif // CONTROLLER_H
