#include "Controller.h"

#include <QDir>
#include <QDebug>
#include <QQmlContext>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QTimer>

#include <algorithm>

#include <math.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

//#include "HeatSource.h"

Controller * Controller::_instance = NULL;

const QString Controller::ControllerPropertyName = "controller";
const QString Controller::USBDevicesPropertyName = "usbDevices";

const quint16 Controller::CP2104VendorID = 4292;
const QString Controller::AMG8833Serial = "01BE21A0";
const QString Controller::RPLidarSerial = "0001";

using namespace rp::standalone::rplidar;

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

Controller*
Controller::GetInstance()
{
    if (_instance == NULL) {
        qDebug() << "Instantiated";
        _instance = new Controller();
    }

    return _instance;
}

Controller::Controller(QObject *parent) :
    Parent(parent),
    _selectedDevice(-1),
    _lidarDriver(NULL)
{
}

Controller::~Controller()
{
    if (_lidarDriver != NULL) {
        _Disconnect();
        delete _scanTimer;
    }
}

void
Controller::SetContext(QQmlContext *context)
{
    _context = context;
    _context->setContextProperty(ControllerPropertyName, this);

    _PopulateUSBDeviceList();
}

void
Controller::SetUsbDevice(int index, bool isToggled)
{
    qDebug() << index << " was set to " << isToggled;
    _selectedDevice = index;

    // Get the serial device
    const QSerialPortInfo p = _serialPorts[index];
    if (p.serialNumber() == Controller::RPLidarSerial) {
        _lidarDriver = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
        _ConnectToLidar(p.systemLocation());
    } else if (p.serialNumber() == Controller::AMG8833Serial) {
        QSerialPort *serialPort = new QSerialPort(p, this);
        serialPort->setBaudRate(115200);
        serialPort->open(QIODevice::ReadOnly);
        //_heatSource = new HeatSource(serialPort, this);
    } else {
        qDebug() << "unapplicable serial port";
    }

    // set timer to scan
    _scanTimer = new QTimer(this);
    connect(_scanTimer, &QTimer::timeout, this, &Self::_ReadDevice);
    _scanTimer->start(1000);
}

void
Controller::_PopulateUSBDeviceList()
{
    QList<QSerialPortInfo> availablePorts  = QSerialPortInfo::availablePorts();
    // This is the same type and we're doing a copy. This saves me from
    // changing variables names in the iterator if I change my mind with the
    // member variable
    _serialPorts = availablePorts;

    qDebug() << "Found " << availablePorts.length() << ".";
    _usbDevices.clear();
    int index = 0;
    std::for_each(availablePorts.cbegin(), availablePorts.cend(),
                  [this](const QSerialPortInfo &p) {
                      qDebug() << "port serial number: " << p.serialNumber() ;
                      _usbDevices[p.serialNumber()] = p.portName() + ":" + p.systemLocation();
                  });
    _context->setContextProperty(USBDevicesPropertyName, QVariant(_usbDevices));
}

void
Controller::_ConnectToLidar(const QString& usbPath)
{
    //rplidar_response_device_health_t healthinfo;
    rplidar_response_device_info_t devinfo;

    // try to connect
    u_result op_result;
    op_result = _lidarDriver->connect(usbPath.toUtf8().constData(), 115200);
    if (op_result & RESULT_FAIL_BIT) {
        qDebug() << "Failed to connect to " << usbPath << ": " << op_result;
        return;
    }
    // retrieving the device info
    ////////////////////////////////////////
    op_result = _lidarDriver->getDeviceInfo(devinfo);
    if (op_result & RESULT_FAIL_BIT) {
        qDebug() << "devInfo " << usbPath << ": " << op_result;
        return;
    }
    QString serialNum;
    for (int i = 0 ; i < 16 ; ++i) {
        serialNum.append(QChar(devinfo.serialnum[i]));
    }
    qDebug() << "Connected to " << serialNum;

    _lidarDriver->startMotor();
    op_result = _lidarDriver->startScan( 0,1 );
    if (op_result & RESULT_FAIL_BIT) {
        qDebug() << "Unable to start scan";
    }
}

void
Controller::_Disconnect()
{
    qDebug() << "disconnecting";
    _lidarDriver->stop();
    _lidarDriver->stopMotor();

    RPlidarDriver::DisposeDriver(_lidarDriver);
}

QVector<QPointF>
Controller::GetPoints() const
{
    return _points;
}

const int BARCOUNT =  75;
const float ANGLESCALE = 360.0f/BARCOUNT;
const int DISTSCALE = 10000;

#define TO_RADIANS(x) x/360.0*2*M_PI
QPointF
Controller::_MakePoint(float angle, float dist)
{
    float relativeDist = dist/DISTSCALE;
    float x = sin(TO_RADIANS(angle)) * relativeDist;
    float y = cos(TO_RADIANS(angle)) * relativeDist;

    return QPointF(x,y);
}

void
Controller::_ReadDevice()
{
    rplidar_response_measurement_node_hq_t nodes[8192];
    size_t count = _countof(nodes);

    qDebug() << "Count: " << count;

    qDebug() << "waiting for data...";

    if (!_lidarDriver) {
        return;
    }        
    // fetch exactly one 0-360 degrees' scan
    u_result res = _lidarDriver->grabScanDataHq(nodes, count);
    if (res & RESULT_FAIL_BIT) {
        qDebug() << "Error in scanning";
    }
    res = _lidarDriver->ascendScanData(nodes, count);
    if (res & RESULT_FAIL_BIT) {
        qDebug() << "Error in sorting";
    }
    int max = 0;
    qint16 total = 0;
    _points.clear();
    for (int pos =0 ; pos < (int)count; ++pos) {
        float deg = nodes[pos].angle_z_q14 * 90.f / 16384.f;
        qDebug() << "angle: " << deg << ": " << nodes[pos].dist_mm_q2;
        // if (nodes[pos].dist_mm_q2 > max) {
        //     max = nodes[pos].dist_mm_q2;
        // }
        _points.push_back(_MakePoint(deg, nodes[pos].dist_mm_q2));
        total += nodes[pos].dist_mm_q2;
    }
    float avg = total/count;
    qDebug() << "Processed scan data: max dist: " << max
             << "avg: " << avg;
    emit PointsUpdated();
}
