#include "PointsItem.h"

#include "Controller.h"

#include <QBrush>
#include <QPainter>
#include <QPen>

#include <QDebug>



PointsItem::PointsItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    qDebug() << "PointsItem created";

    Controller *ctlr = Controller::GetInstance();
    connect(ctlr, &Controller::PointsUpdated,
            this, &Self::_OnPointsUpdated);
}

void PointsItem::paint(QPainter *painter)
{
    QBrush brush(QColor("#007430"));

    QPen pen;
    pen.setBrush(brush);
    pen.setWidth(3);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);
    
    std::for_each(_points.cbegin(), _points.cend(),
                  [painter] (const QPoint& p) {
                      painter->drawPoint(p);
                  });
}

void
PointsItem::_OnPointsUpdated()
{
    _points.clear();
    
    QVector<QPointF> normalizedPoints = Controller::GetInstance()->GetPoints();

    std::transform(normalizedPoints.cbegin(), normalizedPoints.cend(),
                   std::back_inserter(_points),
                   [this](const QPointF& normalized) -> QPoint {
                       float halfWidth = width() / 2.0;
                       float halfHeight = height() / 2.0;
                       return QPoint((normalized.x() + 1.0) * halfWidth,
                                     (normalized.y() + 1.0) * halfHeight);
                   });
    update();
}
