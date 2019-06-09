#ifndef POINTSITEM_H
#define POINTSITEM_H

#include <QtQuick/QQuickPaintedItem>

class PointsItem : public QQuickPaintedItem
{
    Q_OBJECT

    typedef PointsItem Self;
    typedef QQuickPaintedItem Parent;
public:
    PointsItem(QQuickItem *parent = 0);
    void paint(QPainter *painter) override;

public slots:
    void _OnPointsUpdated();

private:
    QVector<QPoint> _points;
};

#endif
