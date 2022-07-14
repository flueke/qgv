/***************************************************************
QGVCore
Copyright (c) 2014, Bergont Nicolas, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
***************************************************************/
#include <QGVNode.h>
#include <QGVCore.h>
#include <QGVScene.h>
#include <QGVGraphPrivate.h>
#include <QGVNodePrivate.h>
#include <QTextDocument>
#include <QDebug>
#include <QPainter>
#include <QStaticText>
#include <QFontMetricsF>
#include <QPicture>

QGVNode::QGVNode(QGVNodePrivate *node, QGVScene *scene): _scene(scene), _node(node)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QGVNode::~QGVNode()
{
    _scene->removeItem(this);
    delete _node;
}

QString QGVNode::label() const
{
    return getAttribute("label");
}

void QGVNode::setLabel(const QString &label)
{
    setAttribute("label", label);
}

QRectF QGVNode::boundingRect() const
{
    return _path.boundingRect();
}

void QGVNode::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();

    painter->setPen(_pen);

    if(isSelected())
    {
        QBrush tbrush(_brush);
        tbrush.setColor(tbrush.color().darker(120));
        painter->setBrush(tbrush);
    }
    else
        painter->setBrush(_brush);

    painter->drawPath(_path);

    painter->setPen(QGVCore::toColor(getAttribute("labelfontcolor")));

    const QRectF rect = boundingRect().adjusted(2,2,-2,-2); //Margin

    if(_icon.isNull())
    {
        auto text = label();

        #if 0
            // original version (does not support any html formatting)
            painter->drawText(rect, Qt::AlignCenter , QGVNode::label());
        #elif 0
            // using QTextDocument to render
            // text looks ok but the position is too far to the top-left
            QTextDocument doc;
            doc.setHtml(text);
            doc.drawContents(painter);
            auto oldPen = painter->pen();
            painter->setPen(QPen(Qt::red));
            painter->drawRect(boundingRect());
            painter->setPen(oldPen);
        #elif 0
            // using QStaticText
            // similar to the QTextDocument version: looks ok but text is too far
            // to the top-left
            QStaticText staticText(text);
            //staticText.prepare(painter->

            auto oldPen = painter->pen();
            painter->setPen(QPen(Qt::red));
            painter->drawRect(boundingRect());
            painter->setPen(oldPen);
            painter->drawStaticText(rect.topLeft(), QStaticText(text));
        #elif 0
            // using QFontMetrics to determine the bounding box of the rendered
            // text, then position the painter so that the text bounding rect is
            // centered inside this nodes bounding rect.
            QFontMetricsF fm(painter->font());
            auto textRect = fm.boundingRect(text); // note: does not take html linebreaks into account
            auto textRectMapped = painter->transform().mapRect(textRect);
            qDebug() << "nodeRect:" << rect << ", textRect:" << textRect << ", textRectMapped:" << textRectMapped;
            painter->setPen(QPen(Qt::green));
            painter->drawRect(boundingRect());

            painter->setPen(QPen(Qt::blue));
            painter->drawRect(textRect);
            painter->drawStaticText(textRect.topLeft(), QStaticText(text));

            painter->setPen(QPen(Qt::red));
            painter->drawRect(textRectMapped);
        #elif 1
            // use a qpainter to paint into a qpicture.
            // then use the original painter to draw the picture
            QPicture pic;
            QPainter picPainter(&pic);
            //picPainter.setTransform(painter->transform());
            picPainter.drawStaticText(rect.topLeft(), QStaticText(text));
            picPainter.end();
            qDebug() << "pic boundingRect" << pic.boundingRect();
            painter->drawPicture(0, 0, pic);
        #endif
    }
    else
    {
        painter->drawText(rect.adjusted(0,0,0, -rect.height()*2/3), Qt::AlignCenter , QGVNode::label());

        const QRectF img_rect = rect.adjusted(0, rect.height()/3,0, 0);
        QImage img = _icon.scaled(img_rect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawImage(img_rect.topLeft() + QPointF((img_rect.width() - img.rect().width())/2, 0), img);
    }
    painter->restore();
}

void QGVNode::setAttribute(const QString &name, const QString &value)
{
    char empty[] = "";
    agsafeset(_node->node(), name.toLocal8Bit().data(), value.toLocal8Bit().data(), empty);
}

QString QGVNode::getAttribute(const QString &name) const
{
    char* value = agget(_node->node(), name.toLocal8Bit().data());
    if(value)
        return value;
    return QString();
}

void QGVNode::setIcon(const QImage &icon)
{
    _icon = icon;
}

void QGVNode::updateLayout()
{
    prepareGeometryChange();
    qreal width = ND_width(_node->node())*DotDefaultDPI;
    qreal height = ND_height(_node->node())*DotDefaultDPI;

    //Node Position (center)
    qreal gheight = QGVCore::graphHeight(_scene->_graph->graph());
    setPos(QGVCore::centerToOrigin(QGVCore::toPoint(ND_coord(_node->node()), gheight), width, height));

    //Node on top
    setZValue(1);

    //Node path
    _path = QGVCore::toPath(ND_shape(_node->node())->name, (polygon_t*)ND_shape_info(_node->node()), width, height);
    _pen.setWidth(1);

    _brush.setStyle(QGVCore::toBrushStyle(getAttribute("style")));
    _brush.setColor(QGVCore::toColor(getAttribute("fillcolor")));
    _pen.setColor(QGVCore::toColor(getAttribute("color")));

    setToolTip(getAttribute("tooltip"));
}
