#include "Controller.h"

#include <QDir>
#include <QDebug>
#include <QQmlContext>

#include <algorithm>

#include <math.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

Controller * Controller::_instance = NULL;

const QString Controller::ControllerPropertyName = "controller";
const QString Controller::USBDevicesPropertyName = "usbDevices";

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
        qDebug() << "Disconnecting from LiDAR";
        _lidarDriver->stop();
        _lidarDriver->stopMotor();

        RPlidarDriver::DisposeDriver(_lidarDriver);
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

    _lidarDriver = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    _Connect(_usbDevices[index], 115200);
}

void
Controller::_PopulateUSBDeviceList()
{
    QDir devDir("/dev");
    QStringList filters;
    filters << "*USB*";
    //filters << "std*";

    QFileInfoList infos = devDir.entryInfoList(filters, QDir::System);
   _usbDevices.clear();
    std::transform(infos.cbegin(),infos.cend(), std::back_inserter(_usbDevices),
                   [] (const QFileInfo& info) {
                       return info.absoluteFilePath();
                   });
    _context->setContextProperty(USBDevicesPropertyName, QVariant(_usbDevices));
}

void
Controller::_Connect(const QString& usbPath, int baudrate)
{
    //rplidar_response_device_health_t healthinfo;
    rplidar_response_device_info_t devinfo;

    // try to connect
    u_result op_result;
    op_result = _lidarDriver->connect(usbPath.toUtf8().constData(), baudrate);
    if (op_result & RESULT_FAIL_BIT) {
        qDebug() << "Failed toonnect to " << usbPath << ": " << op_result;
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
        serialNum.append(devinfo.serialnum[i]);
    }
    qDebug() << "Connected to " << serialNum;

    _lidarDriver->startMotor();
    op_result = _lidarDriver->startScan( 0,1 );
    if (op_result & RESULT_FAIL_BIT) {
        qDebug() << "Unable to start scan";
    }

    _ReadDevice();

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

    // fetech extactly one 0-360 degrees' scan
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
