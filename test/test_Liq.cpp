#include <QtTest>

#include "Controller.h"
#include <QDebug>
#include <QString>

class Test_Liq : public QObject
{
    Q_OBJECT

public:
    Test_Liq();

protected:
    void _ExitEventLoop();

public slots:
                         
private Q_SLOTS:
    void testController();

private:
    QEventLoop _eventLoop;
    bool _expect;
};

Test_Liq::Test_Liq()
{
}

void
Test_Liq::_ExitEventLoop()
{
    qDebug() << "Request completed. Exiting";
}

void
Test_Liq::testController()
{
    Controller controller(NULL);
}    


QTEST_MAIN(Test_Liq)

#include "test_Liq.moc"
